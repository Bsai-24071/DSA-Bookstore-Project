#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <vector>
#include <string>
#include "Books.h"

using namespace std;

class HashTable {
private:
    static const int TABLE_SIZE = 100;
    vector<Book> table[TABLE_SIZE];
    int hashFunction(string key);
public:
    void insert(Book b);
    Book* search(string isbn);
    void remove(string isbn);
};

#endif
