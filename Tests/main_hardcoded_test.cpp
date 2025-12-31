#include "DatabaseManager.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>

using namespace std;

int main() {
    ofstream clearFile("bookstore_data.txt", ofstream::trunc);
    clearFile.close();
    DatabaseManager manager("bookstore_data.txt");
    cout << "=== STARTING AUTOMATED SYSTEM TESTS ===" << endl;
    cout << endl;
    cout << "TEST: Populating Inventory" << endl;
    cout << "--------------------------------------------------" << endl;
    manager.addBook("978-01", "The Great Gatsby", "F. Scott Fitzgerald", "Fiction", 10.50, 50);
    manager.addBook("978-02", "Data Structures", "Dr. Umar", "Education", 45.00, 20);
    manager.addBook("978-03", "Introduction to Algorithms", "Cormen", "Education", 80.00, 15);
    manager.addBook("978-04", "Clean Code", "Robert Martin", "Technology", 35.00, 30);
    manager.addBook("978-05", "Harry Potter", "J.K. Rowling", "Fiction", 25.00, 100);
    manager.addBook("978-06", "The Design of Everyday Things", "Don Norman", "Design", 18.00, 10);
    cout << "Successfully added 6 books to the system." << endl;
    cout << endl;
    cout << "TEST: Hash Table Lookup (O(1) Search by ISBN)" << endl;
    cout << "--------------------------------------------------" << endl;
    string testIsbn = "978-02";
    cout << "Searching for ISBN: " << testIsbn << "... ";
    Book* bookFound = manager.searchByISBN(testIsbn);
    if (bookFound && bookFound->title == "Data Structures") {
        cout << "[PASS] Found: " << bookFound->title << endl;
    } else {
        cout << "[FAIL] Book not found." << endl;
    }
    cout << "Searching for invalid ISBN: 999-99... ";
    Book* bookMissing = manager.searchByISBN("999-99");
    if (bookMissing == nullptr) {
        cout << "[PASS] Correctly returned null for missing book." << endl;
    } 
    else {
        cout << "[FAIL] Found a book that should not exist." << endl;
    }
    cout << endl;
    cout << "TEST: AVL Tree Query (Price Range Search)" << endl;
    cout << "--------------------------------------------------" << endl;
    double minPrice = 10.00;
    double maxPrice = 40.00;
    cout << "Searching for books between " << minPrice << " and " << maxPrice << "..." << endl;
    vector<Book*> rangeResults = manager.searchByPrice(minPrice, maxPrice);
    if (rangeResults.size() == 4) {
        cout << "[PASS] Found " << rangeResults.size() << " books (Expected 4)." << endl;
        for (size_t i = 0; i < rangeResults.size(); i++) {
            cout << "  - " << rangeResults[i]->price << ": " << rangeResults[i]->title << endl;
        }
    } else {
        cout << "[FAIL] Found " << rangeResults.size() << " books (Expected 4)." << endl;
    }
    cout << endl;

    cout << "TEST: B-Tree Traversal (Full Inventory)" << endl;
    cout << "--------------------------------------------------" << endl;

    vector<Book> allBooks = manager.getAllBooks();
    
    if (allBooks.size() == 6) {
        cout << "[PASS] Total Inventory Count: " << allBooks.size() << endl;
        cout << endl;
        cout << "Full List (Sorted Order):" << endl;
        for (size_t i = 0; i < allBooks.size(); i++) {
            cout << "[" << left << setw(12) << allBooks[i].category << "] " 
                 << left << setw(30) << allBooks[i].title 
                 << " - " << allBooks[i].price << endl;
        }
    } else {
        cout << "[FAIL] Inventory count mismatch." << endl;
    }
    cout << endl;
    cout << "=== ALL TESTS COMPLETED ===" << endl;
    return 0;
}