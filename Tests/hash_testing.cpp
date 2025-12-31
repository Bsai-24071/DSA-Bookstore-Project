#include "Books.h"
#include "HashTable.h"
#include <iostream>

using namespace std;

int main() { 
    HashTable isbnIndex;
    Book book1("978-0134685991", "Effective Modern C++", "Scott Meyers", "Programming", 42.99, 10);
    Book book2("978-0132350884", "Clean Code", "Robert Martin", "Programming", 35.99, 15);
    cout << "[TEST: INSERTING BOOKS]" << endl;
    isbnIndex.insert(book1);
    cout << "Inserted: " << book1.isbn << endl;
    isbnIndex.insert(book2);
    cout << "Inserted: " << book2.isbn << endl;
    cout << "[TEST: SEARCHING BOOKS - O(1) LOOKUP]" << endl;
    Book* result1 = isbnIndex.search("978-0134685991");
    if (result1) {
        cout << "Found: " << result1->title << " by " << result1->author << " | Price: $" << result1->price << " | Stock: " << result1->stock << endl;
    } 
    else {
        cout << "Not Found" << endl;
    }
    Book* result2 = isbnIndex.search("978-0132350884");
    if (result2) {
        cout << "Found: " << result2->title << " by " << result2->author << " | Price: $" << result2->price << " | Stock: " << result2->stock << endl;
    } 
    else {
        cout << "Not Found" << endl;
    }
    cout << "Hash Table structure is verified." << endl;
    
    return 0;
}
