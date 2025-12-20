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

struct IndexMapping {
    string isbn;
    int index;
    IndexMapping(string i = "", int idx = -1) : isbn(i), index(idx) {}
};

class MinHeapStock {
private:
    vector<HeapNode<int>> heap;
    vector<IndexMapping> indexMap;
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
    vector<IndexMapping> indexMap;
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
