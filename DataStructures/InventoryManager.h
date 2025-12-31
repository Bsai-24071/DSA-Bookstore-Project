#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H
#include "Heap.h"
#include "Books.h"
#include <vector>
#include <mutex>
using namespace std;

struct StockData {
    string isbn;
    int stock;
    StockData(string i = "", int s = 0) : isbn(i), stock(s) {}
};

struct SalesData {
    string isbn;
    int sales;
    SalesData(string i = "", int s = 0) : isbn(i), sales(s) {}
};

class InventoryManager {
private:
    MinHeapStock stockHeap;
    MaxHeapSales salesHeap;
    vector<StockData> currentStock;
    vector<SalesData> salesData;
    mutex inventoryMutex;
    int findStockIndex(const string& isbn) const;
    int findSalesIndex(const string& isbn) const;
public:
    InventoryManager() {}
    void addBook(const string& isbn, int initialStock);
    void removeBook(const string& isbn);
    bool sellBook(const string& isbn, int quantity = 1);
    void restockBook(const string& isbn, int quantity);
    vector<pair<string, int>> getLowStockBooks(int n = 10);
    vector<pair<string, int>> getTopBestSellers(int n = 10);
    int getStock(const string& isbn) const;
    int getSales(const string& isbn) const;
    bool needsRestocking(const string& isbn, int threshold = 5);
    vector<string> getBooksNeedingRestock(int threshold = 5);
    void clear();
    int getTotalBooks() const { return currentStock.size(); }
    int getTotalSales() const;
    int getTotalStockValue() const;
};

#endif
