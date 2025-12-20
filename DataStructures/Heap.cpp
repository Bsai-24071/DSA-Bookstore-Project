#include "Heap.h"

// ==================== MinHeapStock Implementation ====================

int MinHeapStock::findIndexMapping(const string& isbn) const {
    for (size_t i = 0; i < indexMap.size(); i++) {
        if (indexMap[i].isbn == isbn) {
            return indexMap[i].index;
        }
    }
    return -1;
}

void MinHeapStock::updateIndexMapping(const string& isbn, int newIndex) {
    for (size_t i = 0; i < indexMap.size(); i++) {
        if (indexMap[i].isbn == isbn) {
            indexMap[i].index = newIndex;
            return;
        }
    }
    indexMap.push_back(IndexMapping(isbn, newIndex));
}

void MinHeapStock::removeIndexMapping(const string& isbn) {
    for (size_t i = 0; i < indexMap.size(); i++) {
        if (indexMap[i].isbn == isbn) {
            indexMap.erase(indexMap.begin() + i);
            return;
        }
    }
}

void MinHeapStock::heapifyUp(int index) {
    while (index > 0 && heap[getParentIndex(index)].priority > heap[index].priority) {
        swap(index, getParentIndex(index));
        index = getParentIndex(index);
    }
}

void MinHeapStock::heapifyDown(int index) {
    int minIndex = index;
    int left = getLeftChildIndex(index);
    int right = getRightChildIndex(index);

    if (left < heap.size() && heap[left].priority < heap[minIndex].priority) {
        minIndex = left;
    }
    if (right < heap.size() && heap[right].priority < heap[minIndex].priority) {
        minIndex = right;
    }

    if (minIndex != index) {
        swap(index, minIndex);
        heapifyDown(minIndex);
    }
}

void MinHeapStock::swap(int i, int j) {
    updateIndexMapping(heap[i].isbn, j);
    updateIndexMapping(heap[j].isbn, i);
    
    HeapNode<int> temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void MinHeapStock::insert(const string& isbn, int stock) {
    if (contains(isbn)) {
        updateStock(isbn, stock);
        return;
    }
    
    heap.push_back(HeapNode<int>(isbn, stock));
    int index = heap.size() - 1;
    updateIndexMapping(isbn, index);
    heapifyUp(index);
}

void MinHeapStock::updateStock(const string& isbn, int newStock) {
    if (!contains(isbn)) {
        throw runtime_error("Book not found in heap");
    }

    int index = findIndexMapping(isbn);
    int oldStock = heap[index].priority;
    heap[index].priority = newStock;

    if (newStock < oldStock) {
        heapifyUp(index);
    } else {
        heapifyDown(index);
    }
}

string MinHeapStock::getMinStockBook() const {
    if (isEmpty()) {
        throw runtime_error("Heap is empty");
    }
    return heap[0].isbn;
}

int MinHeapStock::getMinStock() const {
    if (isEmpty()) {
        throw runtime_error("Heap is empty");
    }
    return heap[0].priority;
}

string MinHeapStock::extractMin() {
    if (isEmpty()) {
        throw runtime_error("Heap is empty");
    }

    string minIsbn = heap[0].isbn;
    removeIndexMapping(minIsbn);

    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty()) {
        updateIndexMapping(heap[0].isbn, 0);
        heapifyDown(0);
    }

    return minIsbn;
}

vector<pair<string, int>> MinHeapStock::getTopNLowStock(int n) const {
    vector<pair<string, int>> result;
    MinHeapStock tempHeap = *this; 
    
    n = min(n, (int)heap.size());
    for (int i = 0; i < n; i++) {
        string isbn = tempHeap.getMinStockBook();
        int stock = tempHeap.getMinStock();
        result.push_back({isbn, stock});
        tempHeap.extractMin();
    }

    return result;
}

bool MinHeapStock::contains(const string& isbn) const {
    return findIndexMapping(isbn) != -1;
}

void MinHeapStock::remove(const string& isbn) {
    if (!contains(isbn)) {
        return;
    }

    int index = findIndexMapping(isbn);
    removeIndexMapping(isbn);

    heap[index] = heap.back();
    heap.pop_back();

    if (index < heap.size()) {
        updateIndexMapping(heap[index].isbn, index);

        heapifyUp(index);
        if (findIndexMapping(heap[index].isbn) == index) {
            heapifyDown(index);
        }
    }
}

void MinHeapStock::clear() {
    heap.clear();
    indexMap.clear();
}

// ==================== MaxHeapSales Implementation ====================

int MaxHeapSales::findIndexMapping(const string& isbn) const {
    for (size_t i = 0; i < indexMap.size(); i++) {
        if (indexMap[i].isbn == isbn) {
            return indexMap[i].index;
        }
    }
    return -1;
}

void MaxHeapSales::updateIndexMapping(const string& isbn, int newIndex) {
    for (size_t i = 0; i < indexMap.size(); i++) {
        if (indexMap[i].isbn == isbn) {
            indexMap[i].index = newIndex;
            return;
        }
    }
    indexMap.push_back(IndexMapping(isbn, newIndex));
}

void MaxHeapSales::removeIndexMapping(const string& isbn) {
    for (size_t i = 0; i < indexMap.size(); i++) {
        if (indexMap[i].isbn == isbn) {
            indexMap.erase(indexMap.begin() + i);
            return;
        }
    }
}

void MaxHeapSales::heapifyUp(int index) {
    while (index > 0 && heap[getParentIndex(index)].priority < heap[index].priority) {
        swap(index, getParentIndex(index));
        index = getParentIndex(index);
    }
}

void MaxHeapSales::heapifyDown(int index) {
    int maxIndex = index;
    int left = getLeftChildIndex(index);
    int right = getRightChildIndex(index);

    if (left < heap.size() && heap[left].priority > heap[maxIndex].priority) {
        maxIndex = left;
    }
    if (right < heap.size() && heap[right].priority > heap[maxIndex].priority) {
        maxIndex = right;
    }

    if (maxIndex != index) {
        swap(index, maxIndex);
        heapifyDown(maxIndex);
    }
}

void MaxHeapSales::swap(int i, int j) {
    updateIndexMapping(heap[i].isbn, j);
    updateIndexMapping(heap[j].isbn, i);
    
    HeapNode<int> temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void MaxHeapSales::insert(const string& isbn, int salesCount) {
    if (contains(isbn)) {
        updateSales(isbn, salesCount);
        return;
    }

    heap.push_back(HeapNode<int>(isbn, salesCount));
    int index = heap.size() - 1;
    updateIndexMapping(isbn, index);
    heapifyUp(index);
}

void MaxHeapSales::updateSales(const string& isbn, int newSalesCount) {
    if (!contains(isbn)) {
        throw runtime_error("Book not found in heap");
    }

    int index = findIndexMapping(isbn);
    int oldSales = heap[index].priority;
    heap[index].priority = newSalesCount;

    if (newSalesCount > oldSales) {
        heapifyUp(index);
    } else {
        heapifyDown(index);
    }
}

void MaxHeapSales::incrementSales(const string& isbn, int increment) {
    if (!contains(isbn)) {
        insert(isbn, increment);
    } else {
        int index = findIndexMapping(isbn);
        heap[index].priority += increment;
        heapifyUp(index);
    }
}

string MaxHeapSales::getMaxSalesBook() const {
    if (isEmpty()) {
        throw runtime_error("Heap is empty");
    }
    return heap[0].isbn;
}

int MaxHeapSales::getMaxSales() const {
    if (isEmpty()) {
        throw runtime_error("Heap is empty");
    }
    return heap[0].priority;
}

string MaxHeapSales::extractMax() {
    if (isEmpty()) {
        throw runtime_error("Heap is empty");
    }

    string maxIsbn = heap[0].isbn;
    removeIndexMapping(maxIsbn);

    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty()) {
        updateIndexMapping(heap[0].isbn, 0);
        heapifyDown(0);
    }

    return maxIsbn;
}

vector<pair<string, int>> MaxHeapSales::getTopNBestSellers(int n) const {
    vector<pair<string, int>> result;
    MaxHeapSales tempHeap = *this;

    n = min(n, (int)heap.size());
    for (int i = 0; i < n; i++) {
        string isbn = tempHeap.getMaxSalesBook();
        int sales = tempHeap.getMaxSales();
        result.push_back({isbn, sales});
        tempHeap.extractMax();
    }

    return result;
}

bool MaxHeapSales::contains(const string& isbn) const {
    return findIndexMapping(isbn) != -1;
}

void MaxHeapSales::remove(const string& isbn) {
    if (!contains(isbn)) {
        return;
    }

    int index = findIndexMapping(isbn);
    removeIndexMapping(isbn);

    heap[index] = heap.back();
    heap.pop_back();

    if (index < heap.size()) {
        updateIndexMapping(heap[index].isbn, index);

        heapifyUp(index);
        if (findIndexMapping(heap[index].isbn) == index) {
            heapifyDown(index);
        }
    }
}

void MaxHeapSales::clear() {
    heap.clear();
    indexMap.clear();
}

int MaxHeapSales::getSalesCount(const string& isbn) const {
    if (!contains(isbn)) {
        return 0;
    }
    return heap[findIndexMapping(isbn)].priority;
}
