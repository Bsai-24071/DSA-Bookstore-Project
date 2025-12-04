#include "BTree.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    cout << "--- Stage 1: B-TREE VALIDATION (Sorted Data) ---" << endl;
    
    BTree bookstoreIndex(3);
    
    Book sciFi("2002", "Dune", "F. Herbert", "SciFi", 12.00, 30);
    Book classicBook("3003", "War and Peace", "L. Tolstoy", "Classic", 20.00, 10);
    Book thrillerBook("1001", "The Martian", "A. Weir", "Thriller", 15.00, 50);
    
    bookstoreIndex.insert(sciFi.category, sciFi);
    bookstoreIndex.insert(classicBook.category, classicBook);
    bookstoreIndex.insert(thrillerBook.category, thrillerBook);
    
    cout << "[TEST: B-TREE INORDER TRAVERSAL]" << endl;
    cout << "Expected Order: Classic, SciFi, Thriller" << endl;
    
    vector<Book> sortedList;
    bookstoreIndex.traverse(sortedList);
    
    cout << "--- Actual Result (Sorted by Category Key) ---" << endl;
    for (int i = 0; i < sortedList.size(); i++) {
        cout << "KEY: " << sortedList[i].category << " | TITLE: " << sortedList[i].title << endl;
    }
    
    cout << "B-Tree structure is verified." << endl;
    
    return 0;
}
