#include "DatabaseManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

DatabaseManager::DatabaseManager(string filename) : categoryTree(3) { 
    dbFile = filename;
    allBooks.reserve(1000);
    loadFromDisk(); 
}

void DatabaseManager::addBook(string isbn, string title, string author, string cat, double price, int stock) {
    Book newBook(isbn, title, author, cat, price, stock);
    allBooks.push_back(newBook);
    Book* bookPtr = &allBooks.back(); 
    isbnIndex.insert(newBook);          
    categoryTree.insert(cat, newBook);   
    priceIndex.insert(bookPtr);
    
    saveToDisk();
}

Book* DatabaseManager::searchByISBN(string isbn) {
    return isbnIndex.search(isbn);
}

vector<Book*> DatabaseManager::searchByPrice(double min, double max) {
    return priceIndex.findBooksInRange(min, max);
}

vector<Book> DatabaseManager::getAllBooks() {
    vector<Book> result;
    categoryTree.traverse(result); 
    return result;
}

void DatabaseManager::saveToDisk() {
    ofstream out(dbFile);
    for (size_t i = 0; i < allBooks.size(); i++) {
        out << allBooks[i].serialize() << endl;
    }
    
    out.close();
}

void DatabaseManager::loadFromDisk() {
    ifstream in(dbFile);
    string line;
    if (!in.is_open()) {
        return;
    }
    allBooks.clear();
    isbnIndex = HashTable();
    categoryTree = BTree(3);
    priceIndex = AVLTree();
    while (getline(in, line)) {
        stringstream ss(line);
        string segment;
        vector<string> data;
        
        while (getline(ss, segment, '|')) {
            data.push_back(segment);
        }

        if (data.size() == 6) {
            double price = stod(data[4]);
            int stock = stoi(data[5]);      
            Book newBook(data[0], data[1], data[2], data[3], price, stock);
            allBooks.push_back(newBook);
            Book* bookPtr = &allBooks.back();
            isbnIndex.insert(newBook);
            categoryTree.insert(data[3], newBook);
            priceIndex.insert(bookPtr);
        }
    }
    
    in.close();
}