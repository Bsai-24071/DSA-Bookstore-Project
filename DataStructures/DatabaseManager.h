#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "Books.h"
#include "BTree.h"
#include "HashTable.h"
#include "AVLTree.h" 
#include "Heap.h"
#include <mutex>
#include <fstream>
#include <sstream>

using namespace std;

struct BookStat {
    string isbn;
    int count;
};

class DatabaseManager {
private:
    HashTable isbnIndex;
    BTree categoryTree;
    AVLTree priceIndex; 
    MinHeapStock lowStockHeap;
    MaxHeapSales bestSellersHeap;
    vector<Book> allBooks;
    string dbFile;    
    std::mutex dbMutex; 
public:
    DatabaseManager(string filename = "bookstore_data.txt"); 
    void addBook(string isbn, string title, string author, string cat, double price, int stock);
    bool deleteBook(string isbn);
    bool updateBook(string isbn, string title, string author, string category, double price);
    bool processSale(string isbn, int quantity = 1);
    bool updateStock(string isbn, int newStock);
    Book* searchByISBN(string isbn);
    vector<Book*> searchByCategory(string category);
    vector<Book*> searchByPrice(double min, double max);
    vector<Book> getAllBooks();
    void displayDashboard();
    vector<BookStat> getLowStockBooks(int n);
    vector<BookStat> getBestSellers(int n);
    void saveToDisk();
    void loadFromDisk();
    bool takeBook(string isbn);
    bool dropBook(string isbn);
};

#endif