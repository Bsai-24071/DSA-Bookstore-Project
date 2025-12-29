#include "InventoryManager.h"
#include <algorithm>

int InventoryManager::findStockIndex(const string& isbn) const {
    for (size_t i = 0; i < currentStock.size(); i++) {
        if (currentStock[i].isbn == isbn) {
            return i;
        }
    }
    return -1;
}

int InventoryManager::findSalesIndex(const string& isbn) const {
    for (size_t i = 0; i < salesData.size(); i++) {
        if (salesData[i].isbn == isbn) {
            return i;
        }
    }
    return -1;
}

void InventoryManager::addBook(const string& isbn, int initialStock) {
    lock_guard<mutex> lock(inventoryMutex);
    if (findStockIndex(isbn) != -1) {
        return;
    }
    currentStock.push_back(StockData(isbn, initialStock));
    salesData.push_back(SalesData(isbn, 0));
    
    stockHeap.insert(isbn, initialStock);
    salesHeap.insert(isbn, 0);
}

void InventoryManager::removeBook(const string& isbn) {
    lock_guard<mutex> lock(inventoryMutex);
    stockHeap.remove(isbn);
    salesHeap.remove(isbn);
    int stockIdx = findStockIndex(isbn);
    if (stockIdx != -1) {
        currentStock.erase(currentStock.begin() + stockIdx);
    }
    
    int salesIdx = findSalesIndex(isbn);
    if (salesIdx != -1) {
        salesData.erase(salesData.begin() + salesIdx);
    }
}

bool InventoryManager::sellBook(const string& isbn, int quantity) {
    lock_guard<mutex> lock(inventoryMutex);
    int stockIdx = findStockIndex(isbn);
    if (stockIdx == -1) {
        return false;
    }
    
    if (currentStock[stockIdx].stock < quantity) {
        return false;
    }
    currentStock[stockIdx].stock -= quantity;
    stockHeap.updateStock(isbn, currentStock[stockIdx].stock);
    int salesIdx = findSalesIndex(isbn);
    salesData[salesIdx].sales += quantity;
    salesHeap.incrementSales(isbn, quantity);
    
    return true;
}

void InventoryManager::restockBook(const string& isbn, int quantity) {
    lock_guard<mutex> lock(inventoryMutex);
    int stockIdx = findStockIndex(isbn);
    if (stockIdx == -1) {
        currentStock.push_back(StockData(isbn, quantity));
        salesData.push_back(SalesData(isbn, 0));
        stockHeap.insert(isbn, quantity);
        salesHeap.insert(isbn, 0);
    } else {
        currentStock[stockIdx].stock += quantity;
        stockHeap.updateStock(isbn, currentStock[stockIdx].stock);
    }
}

vector<pair<string, int>> InventoryManager::getLowStockBooks(int n) {
    lock_guard<mutex> lock(inventoryMutex);
    return stockHeap.getTopNLowStock(n);
}

vector<pair<string, int>> InventoryManager::getTopBestSellers(int n) {
    lock_guard<mutex> lock(inventoryMutex);
    return salesHeap.getTopNBestSellers(n);
}

int InventoryManager::getStock(const string& isbn) const {
    int idx = findStockIndex(isbn);
    if (idx == -1) {
        return 0;
    }
    return currentStock[idx].stock;
}

int InventoryManager::getSales(const string& isbn) const {
    int idx = findSalesIndex(isbn);
    if (idx == -1) {
        return 0;
    }
    return salesData[idx].sales;
}

bool InventoryManager::needsRestocking(const string& isbn, int threshold) {
    lock_guard<mutex> lock(inventoryMutex);
    int idx = findStockIndex(isbn);
    if (idx == -1) {
        return false;
    }
    return currentStock[idx].stock <= threshold;
}

vector<string> InventoryManager::getBooksNeedingRestock(int threshold) {
    lock_guard<mutex> lock(inventoryMutex);
    vector<string> result;
    for (size_t i = 0; i < currentStock.size(); i++) {
        if (currentStock[i].stock <= threshold) {
            result.push_back(currentStock[i].isbn);
        }
    }
    return result;
}

void InventoryManager::clear() {
    lock_guard<mutex> lock(inventoryMutex);
    stockHeap.clear();
    salesHeap.clear();
    currentStock.clear();
    salesData.clear();
}

int InventoryManager::getTotalSales() const {
    int total = 0;
    for (const auto& sales : salesData) {
        total += sales.sales;
    }
    return total;
}

int InventoryManager::getTotalStockValue() const {
    int total = 0;
    for (const auto& stock : currentStock) {
        total += stock.stock;
    }
    return total;
}
