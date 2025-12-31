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

void HashTable::display() {
    int totalBooks = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (!table[i].empty()) {
            cout << "Slot " << i << " has " << table[i].size() << " books:" << endl;
            for (auto &book : table[i]) {
                cout << "  - " << book.title << " by " << book.author << " (ISBN: " << book.isbn << ")" << endl;
            }
            totalBooks += table[i].size();
        }
    }
    cout << "Total books in hash table: " << totalBooks << endl;
}

int HashTable::count() {
    int total = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        total += table[i].size();
    }
    return total;
}

bool HashTable::isEmpty() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (!table[i].empty()) {
            return false;
        }
    }
    return true;
}

void HashTable::clear() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i].clear();
    }
}