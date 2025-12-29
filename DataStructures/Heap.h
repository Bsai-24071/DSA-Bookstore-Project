#ifndef HEAP_H
#define HEAP_H

#include "Books.h"
#include <vector>
#include <stdexcept>

using namespace std;

template <typename T>
struct HeapNode {
    string isbn;
    T priority;
    HeapNode(string i, T p) : isbn(i), priority(p) {}
};

struct IndexEntry {
    string isbn;
    int index;
    IndexEntry(string i = "", int idx = -1) : isbn(i), index(idx) {}
};

class IndexHashTable {
private:
    static const int TABLE_SIZE = 1000;
    vector<IndexEntry> table[TABLE_SIZE];
    
    int hashFunction(const string& key) const {
        int sum = 0;
        for (char c : key) {
            sum += c;
        }
        return sum % TABLE_SIZE;
    }
    
public:
    void insert(const string& isbn, int index) {
        int hash = hashFunction(isbn);
        for (size_t i = 0; i < table[hash].size(); i++) {
            if (table[hash][i].isbn == isbn) {
                table[hash][i].index = index;
                return;
            }
        }
        table[hash].push_back(IndexEntry(isbn, index));
    }
    
    int get(const string& isbn) const {
        int hash = hashFunction(isbn);
        for (size_t i = 0; i < table[hash].size(); i++) {
            if (table[hash][i].isbn == isbn) {
                return table[hash][i].index;
            }
        }
        return -1;
    }
    
    bool contains(const string& isbn) const {
        return get(isbn) != -1;
    }
    
    void remove(const string& isbn) {
        int hash = hashFunction(isbn);
        for (size_t i = 0; i < table[hash].size(); i++) {
            if (table[hash][i].isbn == isbn) {
                table[hash].erase(table[hash].begin() + i);
                return;
            }
        }
    }
    
    void clear() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i].clear();
        }
    }
};

class MinHeapStock {
private:
    vector<HeapNode<int>> heap;
    IndexHashTable indexMap;
    void heapifyUp(int index);
    void heapifyDown(int index);
    void swap(int i, int j);
    int getParentIndex(int i) { return (i - 1) / 2; }
    int getLeftChildIndex(int i) { return 2 * i + 1; }
    int getRightChildIndex(int i) { return 2 * i + 2; }
    int findIndexMapping(const string& isbn) const;
    void updateIndexMapping(const string& isbn, int newIndex);
    void removeIndexMapping(const string& isbn);

public:
    MinHeapStock() {}
    void insert(const string& isbn, int stock);
    void updateStock(const string& isbn, int newStock);
    string getMinStockBook() const;
    int getMinStock() const;
    string extractMin();
    vector<pair<string, int>> getTopNLowStock(int n) const;
    bool contains(const string& isbn) const;
    void remove(const string& isbn);
    int size() const { return heap.size(); }
    bool isEmpty() const { return heap.empty(); }
    void clear();
};

class MaxHeapSales {
private:
    vector<HeapNode<int>> heap;
    IndexHashTable indexMap;
    void heapifyUp(int index);
    void heapifyDown(int index);
    void swap(int i, int j);
    int getParentIndex(int i) { return (i - 1) / 2; }
    int getLeftChildIndex(int i) { return 2 * i + 1; }
    int getRightChildIndex(int i) { return 2 * i + 2; }

public:
    MaxHeapSales() {}
    void insert(const string& isbn, int salesCount);
    void updateSales(const string& isbn, int newSalesCount);
    void incrementSales(const string& isbn, int increment = 1);
    string getMaxSalesBook() const;
    int getMaxSales() const;
    string extractMax();
    vector<pair<string, int>> getTopNBestSellers(int n) const;
    bool contains(const string& isbn) const;
    void remove(const string& isbn);
    int size() const { return heap.size(); }
    bool isEmpty() const { return heap.empty(); }
    void clear();
    int getSalesCount(const string& isbn) const;
};

#endif
