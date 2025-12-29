#include "DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
DatabaseManager::DatabaseManager(string filename) : categoryTree(3) { 
    dbFile = filename;
    allBooks.reserve(15000);  
    loadFromDisk(); 
}

void DatabaseManager::addBook(string isbn, string title, string author, string cat, double price, int stock) {
    std::lock_guard<std::mutex> lock(dbMutex);
    Book* existing = isbnIndex.search(isbn);
    if (existing != nullptr) {
        cout << "Book with ISBN " << isbn << " already exists!" << endl;
        return;
    }
    Book newBook(isbn, title, author, cat, price, stock, 0);
    allBooks.push_back(newBook);
    Book* bookPtr = &allBooks.back(); 
    isbnIndex.insert(newBook);          
    categoryTree.insert(cat, newBook);   
    priceIndex.insert(bookPtr);
    lowStockHeap.insert(isbn, stock);
    bestSellersHeap.insert(isbn, 0);
    saveToDisk();
}

bool DatabaseManager::deleteBook(string isbn) {
    std::lock_guard<std::mutex> lock(dbMutex);
    auto it = allBooks.begin();
    for (; it != allBooks.end(); ++it) {
        if (it->isbn == isbn) {
            break;
        }
    }
    if (it == allBooks.end()) {
        cout << "Book with ISBN " << isbn << " not found!" << endl;
        return false;
    }
    double price = it->price;
    isbnIndex.remove(isbn);
    priceIndex.remove(isbn, price);
    lowStockHeap.remove(isbn);
    bestSellersHeap.remove(isbn);
    allBooks.erase(it);
    categoryTree = BTree(3);
    for (size_t i = 0; i < allBooks.size(); i++) {
        categoryTree.insert(allBooks[i].category, allBooks[i]);
    }
    saveToDisk();
    cout << "Book deleted successfully!" << endl;
    return true;
}

bool DatabaseManager::updateBook(string isbn, string title, string author, string category, double price) {
    std::lock_guard<std::mutex> lock(dbMutex);
    Book* hashBook = isbnIndex.search(isbn);
    if (hashBook == nullptr) {
        cout << "Book with ISBN " << isbn << " not found!" << endl;
        return false;
    }
    Book* book = nullptr;
    for (size_t i = 0; i < allBooks.size(); i++) {
        if (allBooks[i].isbn == isbn) {
            book = &allBooks[i];
            break;
        }
    }
    if (book == nullptr) {
        cout << "Book with ISBN " << isbn << " not found in main storage!" << endl;
        return false;
    }
    double oldPrice = book->price;
    string oldCategory = book->category;
    book->title = title;
    book->author = author;
    book->category = category;
    book->price = price;
    hashBook->title = title;
    hashBook->author = author;
    hashBook->category = category;
    hashBook->price = price;
    if (oldPrice != price) {
        priceIndex.remove(isbn, oldPrice);
        priceIndex.insert(book);
    }
    if (oldCategory != category) {
        categoryTree = BTree(3);
        for (size_t i = 0; i < allBooks.size(); i++) {
            categoryTree.insert(allBooks[i].category, allBooks[i]);
        }
    }
    saveToDisk();
    cout << "Book updated successfully!" << endl;
    return true;
}

bool DatabaseManager::processSale(string isbn, int quantity) {
    std::lock_guard<std::mutex> lock(dbMutex);
    Book* book = isbnIndex.search(isbn);
    if (book == nullptr) {
        cout << "Book with ISBN " << isbn << " not found!" << endl;
        return false;
    }
    if (book->stock < quantity) {
        cout << "Insufficient stock! Available: " << book->stock << ", Requested: " << quantity << endl;
        return false;
    }
    book->stock -= quantity;
    book->salesCount += quantity;
    lowStockHeap.updateStock(isbn, book->stock);
    if (bestSellersHeap.contains(isbn)) {
        bestSellersHeap.updateSales(isbn, book->salesCount);
    } 
    else {
        bestSellersHeap.insert(isbn, book->salesCount);
    }
    saveToDisk();
    return true;
}

bool DatabaseManager::updateStock(string isbn, int newStock) {
    std::lock_guard<std::mutex> lock(dbMutex);
    Book* book = isbnIndex.search(isbn);
    if (book == nullptr) {
        cout << "Book with ISBN " << isbn << " not found!" << endl;
        return false;
    }
    book->stock = newStock;
    lowStockHeap.updateStock(isbn, newStock);
    saveToDisk();
    return true;
}

Book* DatabaseManager::searchByISBN(string isbn) {
    return isbnIndex.search(isbn);
}

vector<Book*> DatabaseManager::searchByCategory(string category) {
    vector<Book> books;
    categoryTree.traverse(books);
    vector<Book*> result;
    for (size_t i = 0; i < books.size(); i++) {
        if (books[i].category == category) {
            for (size_t j = 0; j < allBooks.size(); j++) {
                if (allBooks[j].isbn == books[i].isbn) {
                    result.push_back(&allBooks[j]);
                    break;
                }
            }
        }
    }
    return result;
}

vector<Book*> DatabaseManager::searchByPrice(double min, double max) {
    return priceIndex.findBooksInRange(min, max);
}
vector<Book> DatabaseManager::getAllBooks() {
    return allBooks;
}
void DatabaseManager::displayDashboard() {
    cout << endl << string(80, '=') << endl;
    cout << "                    BOOKSTORE DASHBOARD" << endl;
    cout << string(80, '=') << endl;
    cout << endl << "LOW STOCK ALERT - Items Requiring Immediate Restocking" << endl;
    cout << string(80, '-') << endl;
    vector<BookStat> lowStock = getLowStockBooks(5);
    for (size_t i = 0; i < lowStock.size(); i++) {
        Book* book = searchByISBN(lowStock[i].isbn);
        if (book != nullptr) {
            cout << (i + 1) << ". " << book->title << " (ISBN: " << book->isbn 
                 << ") - Only " << lowStock[i].count << " copies remaining" << endl;
        }
    }
    cout << endl << "BEST SELLERS - Top Performing Books" << endl;
    cout << string(80, '-') << endl;
    vector<BookStat> bestSellers = getBestSellers(5);
    for (size_t i = 0; i < bestSellers.size(); i++) {
        Book* book = searchByISBN(bestSellers[i].isbn);
        if (book != nullptr) {
            cout << (i + 1) << ". " << book->title << " (ISBN: " << book->isbn 
                 << ") - " << bestSellers[i].count << " copies sold" << endl;
        }
    }
    cout << endl << string(80, '=') << endl;
}
vector<BookStat> DatabaseManager::getLowStockBooks(int n) {
    vector<BookStat> result;
    auto heapResult = lowStockHeap.getTopNLowStock(n);
    for (size_t i = 0; i < heapResult.size(); i++) {
        BookStat stat;
        stat.isbn = heapResult[i].first;
        stat.count = heapResult[i].second;
        result.push_back(stat);
    }
    return result;
}

vector<BookStat> DatabaseManager::getBestSellers(int n) {
    vector<BookStat> result;
    auto heapResult = bestSellersHeap.getTopNBestSellers(n);
    for (size_t i = 0; i < heapResult.size(); i++) {
        BookStat stat;
        stat.isbn = heapResult[i].first;
        stat.count = heapResult[i].second;
        result.push_back(stat);
    }
    return result;
}

void DatabaseManager::saveToDisk() {
    ofstream out(dbFile);
    for (size_t i = 0; i < allBooks.size(); i++) {
        out << allBooks[i].serialize() << endl;
    }
    
    out.close();
}
bool DatabaseManager::takeBook(string isbn) {
    std::lock_guard<std::mutex> lock(dbMutex);
    Book* book = isbnIndex.search(isbn);
    if (book == nullptr) {
        return false;
    }
    if (book->stock < 1) {
        return false;
    }
    book->stock -= 1;
    book->salesCount += 1;
    lowStockHeap.updateStock(isbn, book->stock);
    if (bestSellersHeap.contains(isbn)) {
        bestSellersHeap.updateSales(isbn, book->salesCount);
    } 
    else {
        bestSellersHeap.insert(isbn, book->salesCount);
    }
    saveToDisk();
    return true;
}

bool DatabaseManager::dropBook(string isbn) {
    std::lock_guard<std::mutex> lock(dbMutex);
    Book* book = isbnIndex.search(isbn);
    if (book == nullptr) {
        return false;
    }
    book->stock += 1;
    lowStockHeap.updateStock(isbn, book->stock);
    saveToDisk();
    return true;
}
void DatabaseManager::loadFromDisk() {
    ifstream in(dbFile);
    if (!in.is_open()) {
        return;
    } 
    allBooks.clear();
    isbnIndex = HashTable();
    categoryTree = BTree(3);
    priceIndex = AVLTree();
    lowStockHeap.clear();
    bestSellersHeap.clear();
    allBooks.reserve(15000);
    string line;
    line.reserve(256); 
    string isbn, title, author, category, priceStr, stockStr, salesCountStr;
    while (getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        size_t pos1 = line.find('|');
        if (pos1 == string::npos) {
            continue;
        }
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos2 == string::npos) {
            continue;
        }
        size_t pos3 = line.find('|', pos2 + 1);
        if (pos3 == string::npos) {
            continue;
        }
        size_t pos4 = line.find('|', pos3 + 1);
        if (pos4 == string::npos) {
            continue;
        }
        size_t pos5 = line.find('|', pos4 + 1);
        if (pos5 == string::npos) {
            continue;
        }
        size_t pos6 = line.find('|', pos5 + 1);
        isbn = line.substr(0, pos1);
        title = line.substr(pos1 + 1, pos2 - pos1 - 1);
        author = line.substr(pos2 + 1, pos3 - pos2 - 1);
        category = line.substr(pos3 + 1, pos4 - pos3 - 1);
        priceStr = line.substr(pos4 + 1, pos5 - pos4 - 1);
        int salesCount = 0;
        if (pos6 != string::npos) {
            stockStr = line.substr(pos5 + 1, pos6 - pos5 - 1);
            salesCountStr = line.substr(pos6 + 1);
            salesCount = stoi(salesCountStr);
        }
         else {
            stockStr = line.substr(pos5 + 1);
        }
        double price = stod(priceStr);
        int stock = stoi(stockStr);
        Book newBook(isbn, title, author, category, price, stock, salesCount);
        allBooks.push_back(newBook);
        Book* bookPtr = &allBooks.back();
        isbnIndex.insert(newBook);
        categoryTree.insert(category, newBook);
        priceIndex.insert(bookPtr);
        lowStockHeap.insert(isbn, stock);
        bestSellersHeap.insert(isbn, salesCount);
    }
    in.close();
}