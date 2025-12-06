#include "HashTable.h"
#include <iostream>

int HashTable::hashFunction(string key) {
    int sum = 0;
    for (char c : key) {
        sum += c;
    }
    return sum % TABLE_SIZE;
}

void HashTable::insert(Book b) {
    int index = hashFunction(b.isbn);
    for (auto &book : table[index]) {
        if (book.isbn == b.isbn) {
            book = b;
            return;
        }
    }
    table[index].push_back(b);
}

Book* HashTable::search(string isbn) {
    int index = hashFunction(isbn);
    for (auto &book : table[index]) {
        if (book.isbn == isbn) {
            return &book;
        }
    }
    return nullptr;
}

void HashTable::remove(string isbn) {
    int index = hashFunction(isbn);
    for (auto it = table[index].begin(); it != table[index].end(); ++it) {
        if (it->isbn == isbn) {
            table[index].erase(it);
            return;
        }
    }
}
