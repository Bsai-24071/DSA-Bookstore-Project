#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "Books.h"
#include "BTree.h"
#include "HashTable.h"
#include "AVLTree.h" 
#include <mutex>
#include <fstream>
#include <sstream>

using namespace std;

class DatabaseManager {
private:
    HashTable isbnIndex;
    BTree categoryTree;
    AVLTree priceIndex; 
    vector<Book> allBooks;
    string dbFile;
    
    std::mutex dbMutex; 

public:
    DatabaseManager(string filename = "bookstore_data.txt"); 
    void addBook(string isbn, string title, string author, string cat, double price, int stock);
    Book* searchByISBN(string isbn);
    vector<Book*> searchByPrice(double min, double max);
    vector<Book> getAllBooks();
    void saveToDisk();
    void loadFromDisk();
};

#endif