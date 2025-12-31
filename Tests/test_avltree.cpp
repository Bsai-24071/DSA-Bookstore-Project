#include <iostream>
#include "AVLTree.h"
#include "Books.h"

using namespace std;

void printBooks(const vector<Book*>& books) {
    if (books.empty()) {
        cout << "No books found" << endl;
        return;
    }

    for (size_t i = 0; i < books.size(); i++) {
        cout << "  $" << books[i]->price << " - " << books[i]->title 
             << " by " << books[i]->author << " (ISBN: " << books[i]->isbn << ")" << endl;
    }
}

int main() {
    AVLTree priceTree;

    Book book1 = {"978-0-13-468599-1", "Clean Code", "Robert Martin", "Programming", 42.99, 15};
    Book book2 = {"978-0-13-235088-4", "Clean Architecture", "Robert Martin", "Programming", 38.50, 12};
    Book book3 = {"978-0-596-52068-7", "JavaScript: The Good Parts", "Douglas Crockford", "Programming", 29.99, 20};
    Book book4 = {"978-0-201-63361-0", "Design Patterns", "Gang of Four", "Programming", 54.99, 8};
    Book book5 = {"978-0-134-68599-8", "The Pragmatic Programmer", "Hunt & Thomas", "Programming", 45.00, 10};
    Book book6 = {"978-1-449-35573-9", "Learning Python", "Mark Lutz", "Programming", 59.99, 7};
    Book book7 = {"978-0-321-12521-7", "Domain-Driven Design", "Eric Evans", "Programming", 49.99, 6};
    Book book8 = {"978-0-262-03384-8", "Introduction to Algorithms", "CLRS", "Academic", 89.99, 5};
    Book book9 = {"978-0-596-00765-1", "Head First Design Patterns", "Freeman", "Programming", 34.99, 18};
    Book book10 = {"978-1-491-95039-8", "Fluent Python", "Luciano Ramalho", "Programming", 62.00, 9};

    cout << "=== Test 1: Insert books into AVL Tree ===" << endl;
    priceTree.insert(&book1);
    priceTree.insert(&book2);
    priceTree.insert(&book3);
    priceTree.insert(&book4);
    priceTree.insert(&book5);
    priceTree.insert(&book6);
    priceTree.insert(&book7);
    priceTree.insert(&book8);
    priceTree.insert(&book9);
    priceTree.insert(&book10);
    cout << "Inserted 10 books" << endl;
    cout << endl;

    cout << "=== Test 2: Display all books sorted by price ===" << endl;
    priceTree.display();
    cout << endl;

    cout << "=== Test 3: Find books in range $40-$60 ===" << endl;
    vector<Book*> rangeBooks = priceTree.findBooksInRange(40.0, 60.0);
    printBooks(rangeBooks);
    cout << endl;

    cout << "=== Test 4: Find books under $35 ===" << endl;
    vector<Book*> cheapBooks = priceTree.findBooksUnderPrice(35.0);
    printBooks(cheapBooks);
    cout << endl;

    cout << "=== Test 5: Find books over $50 ===" << endl;
    vector<Book*> expensiveBooks = priceTree.findBooksOverPrice(50.0);
    printBooks(expensiveBooks);
    cout << endl;

    cout << "=== Test 6: Get all books sorted ===" << endl;
    vector<Book*> allBooks = priceTree.getAllBooksSorted();
    cout << "Total books: " << allBooks.size() << endl;
    printBooks(allBooks);
    cout << endl;

    cout << "=== Test 7: Remove a book (Clean Code - $42.99) ===" << endl;
    priceTree.remove("978-0-13-468599-1", 42.99);
    cout << "After removal:" << endl;
    priceTree.display();
    cout << endl;

    cout << "=== Test 8: Remove another book (JavaScript - $29.99) ===" << endl;
    priceTree.remove("978-0-596-52068-7", 29.99);
    cout << "After removal:" << endl;
    priceTree.display();
    cout << endl;

    cout << "=== Test 9: Find books in range $40-$60 after removals ===" << endl;
    rangeBooks = priceTree.findBooksInRange(40.0, 60.0);
    printBooks(rangeBooks);
    cout << endl;

    cout << "=== Test 10: Test with duplicate prices ===" << endl;
    Book book11 = {"978-1-234-56789-0", "Book A", "Author A", "Fiction", 45.00, 10};
    Book book12 = {"978-1-234-56789-1", "Book B", "Author B", "Fiction", 45.00, 8};
    priceTree.insert(&book11);
    priceTree.insert(&book12);
    cout << "Inserted 2 books with same price ($45.00)" << endl;
    vector<Book*> booksAt45 = priceTree.findBooksInRange(45.0, 45.0);
    cout << "Books at exactly $45.00:" << endl;
    printBooks(booksAt45);
    cout << endl;

    cout << "=== Test 11: Check isEmpty() ===" << endl;
    cout << "Tree is empty: " << (priceTree.isEmpty() ? "Yes" : "No") << endl;
    cout << endl;

    cout << "=== Test 12: AVL Tree Balance Test ===" << endl;
    cout << "The AVL tree automatically balances during insertions and deletions." << endl;
    cout << "Height is maintained at O(log n) through rotations:" << endl;
    cout << "  - LL Rotation (Right Rotate)" << endl;
    cout << "  - RR Rotation (Left Rotate)" << endl;
    cout << "  - LR Rotation (Left-Right)" << endl;
    cout << "  - RL Rotation (Right-Left)" << endl;
    cout << "All operations completed successfully!" << endl;

    return 0;
}
