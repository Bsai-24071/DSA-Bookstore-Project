#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include "../DataStructures/DatabaseManager.h"
#include "../DataStructures/UserManager.h"

using namespace std;

User* currentUser = nullptr;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void displayHeader(const string& title) {
    cout << "\n" << string(80, '=') << endl;
    cout << "  " << title << endl;
    cout << string(80, '=') << endl;
}

void displayMainMenu() {
    displayHeader("BOOKSTORE MANAGEMENT SYSTEM");
    if (currentUser) {
        cout << "\nLogged in as: " << currentUser->username;
        if (currentUser->isAdmin) {
            cout << " (Admin)";
        } else {
            cout << " (User)";
        }
        cout << endl;
    }
    
    if (currentUser && currentUser->isAdmin) {
        cout << "\n1. Add New Book" << endl;
        cout << "2. Update Book Details" << endl;
        cout << "3. Delete Book" << endl;
        cout << "4. Process Sale" << endl;
        cout << "5. Update Stock" << endl;
        cout << "6. Search Books" << endl;
        cout << "7. View Dashboard" << endl;
        cout << "8. View All Books" << endl;
        cout << "9. Logout" << endl;
        cout << "0. Exit" << endl;
    } else if (currentUser) {
        cout << "\n1. Search Books" << endl;
        cout << "2. Take a Book" << endl;
        cout << "3. Drop a Book" << endl;
        cout << "4. View My Profile" << endl;
        cout << "5. Logout" << endl;
        cout << "0. Exit" << endl;
    } else {
        cout << "\n1. Search Books (Guest)" << endl;
        cout << "0. Exit" << endl;
    }
    cout << "\nEnter your choice: ";
}

void displaySearchMenu() {
    displayHeader("SEARCH MENU");
    cout << "\n1. Search by ISBN (Hash Table - O(1))" << endl;
    cout << "2. Search by Category (B-Tree)" << endl;
    cout << "3. Search by Price Range (AVL Tree)" << endl;
    cout << "4. Back to Main Menu" << endl;
    cout << "\nEnter your choice: ";
}

void addBook(DatabaseManager& db) {
    clearScreen();
    displayHeader("ADD NEW BOOK");
    
    string isbn, title, author, category;
    double price;
    int stock;
    
    cout << "\nEnter ISBN: ";
    cin.ignore();
    getline(cin, isbn);
    
    cout << "Enter Title: ";
    getline(cin, title);
    
    cout << "Enter Author: ";
    getline(cin, author);
    
    cout << "Enter Category: ";
    getline(cin, category);
    
    cout << "Enter Price: $";
    cin >> price;
    
    cout << "Enter Stock Quantity: ";
    cin >> stock;
    
    db.addBook(isbn, title, author, category, price, stock);
    
    cout << "\n[SUCCESS] Book added successfully!" << endl;
    pressEnterToContinue();
}

void updateBook(DatabaseManager& db) {
    clearScreen();
    displayHeader("UPDATE BOOK DETAILS");
    
    string isbn;
    cout << "\nEnter ISBN of book to update: ";
    cin.ignore();
    getline(cin, isbn);
    
    Book* book = db.searchByISBN(isbn);
    if (book == nullptr) {
        cout << "\n[ERROR] Book not found!" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "\nCurrent Details:" << endl;
    cout << "  Title: " << book->title << endl;
    cout << "  Author: " << book->author << endl;
    cout << "  Category: " << book->category << endl;
    cout << "  Price: $" << fixed << setprecision(2) << book->price << endl;
    
    string title, author, category;
    double price;
    
    cout << "\nEnter New Details:" << endl;
    cout << "Title: ";
    getline(cin, title);
    
    cout << "Author: ";
    getline(cin, author);
    
    cout << "Category: ";
    getline(cin, category);
    
    cout << "Price: $";
    cin >> price;
    
    if (db.updateBook(isbn, title, author, category, price)) {
        cout << "\n[SUCCESS] Book updated successfully!" << endl;
    }
    
    pressEnterToContinue();
}

void deleteBook(DatabaseManager& db) {
    clearScreen();
    displayHeader("DELETE BOOK");
    
    string isbn;
    cout << "\nEnter ISBN of book to delete: ";
    cin.ignore();
    getline(cin, isbn);
    
    Book* book = db.searchByISBN(isbn);
    if (book == nullptr) {
        cout << "\n[ERROR] Book not found!" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "\nBook to Delete:" << endl;
    cout << "  Title: " << book->title << endl;
    cout << "  Author: " << book->author << endl;
    cout << "  ISBN: " << book->isbn << endl;
    
    char confirm;
    cout << "\nAre you sure you want to delete this book? (y/n): ";
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        if (db.deleteBook(isbn)) {
            cout << "\n[SUCCESS] Book deleted!" << endl;
        }
    } else {
        cout << "\n[CANCELLED] Delete operation cancelled." << endl;
    }
    
    pressEnterToContinue();
}

void processSale(DatabaseManager& db) {
    clearScreen();
    displayHeader("PROCESS SALE");
    
    string isbn;
    int quantity;
    
    cout << "\nEnter ISBN: ";
    cin.ignore();
    getline(cin, isbn);
    
    // First, show the book details
    Book* book = db.searchByISBN(isbn);
    if (book == nullptr) {
        cout << "\n[ERROR] Book not found!" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "\nBook Found:" << endl;
    cout << "  Title: " << book->title << endl;
    cout << "  Author: " << book->author << endl;
    cout << "  Price: $" << fixed << setprecision(2) << book->price << endl;
    cout << "  Current Stock: " << book->stock << endl;
    cout << "  Sales Count: " << book->salesCount << endl;
    
    cout << "\nEnter Quantity to Sell: ";
    cin >> quantity;
    
    if (db.processSale(isbn, quantity)) {
        cout << "\n[SUCCESS] Sale processed successfully!" << endl;
        cout << "  Total Amount: $" << fixed << setprecision(2) << (book->price * quantity) << endl;
        cout << "  Remaining Stock: " << book->stock << endl;
        cout << "  Updated Sales Count: " << book->salesCount << endl;
    } else {
        cout << "\n[ERROR] Sale could not be processed!" << endl;
    }
    
    pressEnterToContinue();
}

void updateStock(DatabaseManager& db) {
    clearScreen();
    displayHeader("UPDATE STOCK");
    
    string isbn;
    int newStock;
    
    cout << "\nEnter ISBN: ";
    cin.ignore();
    getline(cin, isbn);
    
    Book* book = db.searchByISBN(isbn);
    if (book == nullptr) {
        cout << "\n[ERROR] Book not found!" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "\nBook: \"" << book->title << "\"" << endl;
    cout << "Current Stock: " << book->stock << endl;
    
    cout << "\nEnter New Stock Quantity: ";
    cin >> newStock;
    
    if (db.updateStock(isbn, newStock)) {
        cout << "\n[SUCCESS] Stock updated successfully!" << endl;
        cout << "  New Stock: " << newStock << endl;
    }
    
    pressEnterToContinue();
}

void displayBookDetails(Book* book) {
    cout << string(80, '-') << endl;
    cout << "ISBN: " << book->isbn << endl;
    cout << "Title: " << book->title << endl;
    cout << "Author: " << book->author << endl;
    cout << "Category: " << book->category << endl;
    cout << "Price: $" << fixed << setprecision(2) << book->price << endl;
    cout << "Stock: " << book->stock << endl;
    cout << "Sales Count: " << book->salesCount << endl;
}

void searchByISBN(DatabaseManager& db) {
    clearScreen();
    displayHeader("SEARCH BY ISBN (Hash Table - O(1))");
    
    string isbn;
    cout << "\nEnter ISBN: ";
    cin.ignore();
    getline(cin, isbn);
    
    Book* book = db.searchByISBN(isbn);
    if (book == nullptr) {
        cout << "\n[ERROR] Book not found!" << endl;
    } else {
        cout << "\n[FOUND] Book Details:" << endl;
        displayBookDetails(book);
    }
    
    pressEnterToContinue();
}

void searchByCategory(DatabaseManager& db) {
    clearScreen();
    displayHeader("SEARCH BY CATEGORY (B-Tree)");
    
    string category;
    cout << "\nEnter Category: ";
    cin.ignore();
    getline(cin, category);
    
    vector<Book*> results = db.searchByCategory(category);
    
    if (results.empty()) {
        cout << "\n[INFO] No books found in category: " << category << endl;
    } else {
        cout << "\n[FOUND] " << results.size() << " book(s) in category \"" << category << "\":" << endl;
        for (size_t i = 0; i < results.size(); i++) {
            cout << "\n" << (i + 1) << ". ";
            displayBookDetails(results[i]);
        }
    }
    
    pressEnterToContinue();
}

void searchByPriceRange(DatabaseManager& db) {
    clearScreen();
    displayHeader("SEARCH BY PRICE RANGE (AVL Tree)");
    
    double minPrice, maxPrice;
    
    cout << "\nEnter Minimum Price: $";
    cin >> minPrice;
    
    cout << "Enter Maximum Price: $";
    cin >> maxPrice;
    
    vector<Book*> results = db.searchByPrice(minPrice, maxPrice);
    
    if (results.empty()) {
        cout << "\n[INFO] No books found in price range: $" << fixed << setprecision(2) 
             << minPrice << " - $" << maxPrice << endl;
    } else {
        cout << "\n[FOUND] " << results.size() << " book(s) in price range $" 
             << fixed << setprecision(2) << minPrice << " - $" << maxPrice << ":" << endl;
        for (size_t i = 0; i < results.size(); i++) {
            cout << "\n" << (i + 1) << ". ";
            displayBookDetails(results[i]);
        }
    }
    
    pressEnterToContinue();
}

void searchBooks(DatabaseManager& db) {
    while (true) {
        clearScreen();
        displaySearchMenu();
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                searchByISBN(db);
                break;
            case 2:
                searchByCategory(db);
                break;
            case 3:
                searchByPriceRange(db);
                break;
            case 4:
                return;
            default:
                cout << "\n[ERROR] Invalid choice! Please try again." << endl;
                pressEnterToContinue();
        }
    }
}

void viewDashboard(DatabaseManager& db) {
    clearScreen();
    db.displayDashboard();
    pressEnterToContinue();
}

void viewAllBooks(DatabaseManager& db) {
    clearScreen();
    displayHeader("ALL BOOKS IN INVENTORY");
    
    vector<Book> books = db.getAllBooks();
    
    if (books.empty()) {
        cout << "\n[INFO] No books in inventory." << endl;
    } else {
        cout << "\n[TOTAL BOOKS: " << books.size() << "]" << endl;
        for (size_t i = 0; i < books.size(); i++) {
            cout << "\n" << (i + 1) << ". ";
            cout << "ISBN: " << books[i].isbn << " | "
                 << "Title: " << books[i].title << " | "
                 << "Category: " << books[i].category << " | "
                 << "Price: $" << fixed << setprecision(2) << books[i].price << " | "
                 << "Stock: " << books[i].stock << " | "
                 << "Sales: " << books[i].salesCount << endl;
        }
    }
    
    pressEnterToContinue();
}

void takeBook(DatabaseManager& db, UserManager& um) {
    clearScreen();
    displayHeader("TAKE A BOOK");
    
    if (!currentUser) {
        cout << "\n[ERROR] You must be logged in!" << endl;
        pressEnterToContinue();
        return;
    }
    
    string isbn;
    cout << "\nEnter ISBN of book to take: ";
    cin.ignore();
    getline(cin, isbn);
    
    Book* book = db.searchByISBN(isbn);
    if (book == nullptr) {
        cout << "\n[ERROR] Book not found!" << endl;
        pressEnterToContinue();
        return;
    }
    
    if (book->stock < 1) {
        cout << "\n[ERROR] Book is out of stock!" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "\nBook Details:" << endl;
    cout << "  Title: " << book->title << endl;
    cout << "  Author: " << book->author << endl;
    cout << "  Price: $" << fixed << setprecision(2) << book->price << endl;
    cout << "  Available Stock: " << book->stock << endl;
    
    char confirm;
    cout << "\nConfirm take this book? (y/n): ";
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        if (db.takeBook(isbn)) {
            currentUser->addBook(book);
            um.saveToDisk();
            cout << "\n[SUCCESS] Book taken successfully!" << endl;
            cout << "  Remaining Stock: " << book->stock << endl;
        } else {
            cout << "\n[ERROR] Failed to take book!" << endl;
        }
    } else {
        cout << "\n[CANCELLED] Operation cancelled." << endl;
    }
    
    pressEnterToContinue();
}

void dropBook(DatabaseManager& db, UserManager& um) {
    clearScreen();
    displayHeader("DROP A BOOK");
    
    if (!currentUser) {
        cout << "\n[ERROR] You must be logged in!" << endl;
        pressEnterToContinue();
        return;
    }
    
    if (currentUser->myBooks.empty()) {
        cout << "\n[INFO] You have no books to drop." << endl;
        pressEnterToContinue();
        return;
    }
    
    string isbn;
    cout << "\nEnter ISBN of book to drop: ";
    cin.ignore();
    getline(cin, isbn);
    
    Book* book = currentUser->findBookInMyBooks(isbn);
    if (book == nullptr) {
        cout << "\n[ERROR] You don't have this book!" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "\nBook to Drop:" << endl;
    cout << "  Title: " << book->title << endl;
    cout << "  Author: " << book->author << endl;
    cout << "  ISBN: " << book->isbn << endl;
    
    char confirm;
    cout << "\nConfirm drop this book? (y/n): ";
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        if (db.dropBook(isbn)) {
            currentUser->removeBook(isbn);
            um.saveToDisk();
            cout << "\n[SUCCESS] Book returned to inventory!" << endl;
        } else {
            cout << "\n[ERROR] Failed to return book!" << endl;
        }
    } else {
        cout << "\n[CANCELLED] Operation cancelled." << endl;
    }
    
    pressEnterToContinue();
}

void viewProfile() {
    clearScreen();
    displayHeader("MY PROFILE");
    
    if (!currentUser) {
        cout << "\n[ERROR] You must be logged in!" << endl;
        pressEnterToContinue();
        return;
    }
    
    cout << "\nUsername: " << currentUser->username << endl;
    cout << "Account Type: " << (currentUser->isAdmin ? "Admin" : "User") << endl;
    cout << "\nBooks I Currently Have: " << currentUser->myBooks.size() << endl;
    
    if (currentUser->myBooks.empty()) {
        cout << "  (No books)" << endl;
    } else {
        for (size_t i = 0; i < currentUser->myBooks.size(); i++) {
            Book* book = currentUser->myBooks[i];
            cout << "\n" << (i + 1) << ". ";
            cout << "ISBN: " << book->isbn << endl;
            cout << "   Title: " << book->title << endl;
            cout << "   Author: " << book->author << endl;
            cout << "   Category: " << book->category << endl;
            cout << "   Price: $" << fixed << setprecision(2) << book->price << endl;
        }
    }
    
    pressEnterToContinue();
}

void displayAuthMenu() {
    displayHeader("AUTHENTICATION");
    cout << "\n1. Login" << endl;
    cout << "2. Register" << endl;
    cout << "3. Continue as Guest" << endl;
    cout << "4. Exit" << endl;
    cout << "\nEnter your choice: ";
}

bool loginUser(UserManager& um) {
    clearScreen();
    displayHeader("USER LOGIN");
    
    string username, password;
    
    cout << "\nUsername: ";
    cin.ignore();
    getline(cin, username);
    
    cout << "Password: ";
    getline(cin, password);
    
    User* user = um.login(username, password);
    if (user) {
        currentUser = user;
        cout << "\n[SUCCESS] Login successful!" << endl;
        cout << "Welcome, " << currentUser->username << "!" << endl;
        pressEnterToContinue();
        return true;
    } else {
        cout << "\n[ERROR] Invalid username or password!" << endl;
        pressEnterToContinue();
        return false;
    }
}

bool registerUser(UserManager& um) {
    clearScreen();
    displayHeader("USER REGISTRATION");
    
    string username, password;
    
    cout << "\nEnter Username: ";
    cin.ignore();
    getline(cin, username);
    
    cout << "Enter Password: ";
    getline(cin, password);
    
    if (um.registerUser(username, password, false)) {
        cout << "\n[SUCCESS] Registration successful!" << endl;
        cout << "You can now login with your credentials." << endl;
        pressEnterToContinue();
        return true;
    } else {
        cout << "\n[ERROR] Username already exists!" << endl;
        pressEnterToContinue();
        return false;
    }
}

void authenticationScreen(UserManager& um) {
    while (true) {
        clearScreen();
        displayAuthMenu();
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                loginUser(um);
                if (currentUser) return;
                break;
            case 2:
                registerUser(um);
                break;
            case 3:
                currentUser = nullptr;
                cout << "\n[INFO] Continuing as Guest (limited access)" << endl;
                pressEnterToContinue();
                return;
            case 4:
                exit(0);
            default:
                cout << "\n[ERROR] Invalid choice!" << endl;
                pressEnterToContinue();
        }
    }
}

int main() {
    DatabaseManager db("bookstore_data.txt");
    UserManager um(nullptr, "users_data.txt");
    um.setDatabaseManager(&db);
    
    cout << "\n========================================" << endl;
    cout << "   Welcome to Bookstore Management      " << endl;
    cout << "========================================" << endl;
    cout << "Initializing system..." << endl;
    cout << "All data structures synchronized!" << endl;
    cout << "  - Hash Table (ISBN Lookups)" << endl;
    cout << "  - B-Tree (Category Sorting)" << endl;
    cout << "  - AVL Tree (Price Range Queries)" << endl;
    cout << "  - Min-Heap (Low Stock Tracking)" << endl;
    cout << "  - Max-Heap (Best Sellers)" << endl;
    cout << "  - User Authentication System" << endl;
    
    pressEnterToContinue();
    
    authenticationScreen(um);
    
    while (true) {
        clearScreen();
        displayMainMenu();
        
        int choice;
        cin >> choice;
        
        if (currentUser && currentUser->isAdmin) {
            switch (choice) {
                case 1:
                    addBook(db);
                    break;
                case 2:
                    updateBook(db);
                    break;
                case 3:
                    deleteBook(db);
                    break;
                case 4:
                    processSale(db);
                    break;
                case 5:
                    updateStock(db);
                    break;
                case 6:
                    searchBooks(db);
                    break;
                case 7:
                    viewDashboard(db);
                    break;
                case 8:
                    viewAllBooks(db);
                    break;
                case 9:
                    currentUser = nullptr;
                    cout << "\n[INFO] Logged out successfully!" << endl;
                    pressEnterToContinue();
                    authenticationScreen(um);
                    break;
                case 0:
                    clearScreen();
                    cout << "\n========================================" << endl;
                    cout << "   Thank you for using Bookstore       " << endl;
                    cout << "   Management System!                  " << endl;
                    cout << "========================================" << endl;
                    return 0;
                default:
                    cout << "\n[ERROR] Invalid choice! Please try again." << endl;
                    pressEnterToContinue();
            }
        } else if (currentUser) {
            switch (choice) {
                case 1:
                    searchBooks(db);
                    break;
                case 2:
                    takeBook(db, um);
                    break;
                case 3:
                    dropBook(db, um);
                    break;
                case 4:
                    viewProfile();
                    break;
                case 5:
                    currentUser = nullptr;
                    cout << "\n[INFO] Logged out successfully!" << endl;
                    pressEnterToContinue();
                    authenticationScreen(um);
                    break;
                case 0:
                    clearScreen();
                    cout << "\n========================================" << endl;
                    cout << "   Thank you for using Bookstore       " << endl;
                    cout << "   Management System!                  " << endl;
                    cout << "========================================" << endl;
                    return 0;
                default:
                    cout << "\n[ERROR] Invalid choice! Please try again." << endl;
                    pressEnterToContinue();
            }
        } else {
            switch (choice) {
                case 1:
                    searchBooks(db);
                    break;
                case 0:
                    clearScreen();
                    cout << "\n========================================" << endl;
                    cout << "   Thank you for using Bookstore       " << endl;
                    cout << "   Management System!                  " << endl;
                    cout << "========================================" << endl;
                    return 0;
                default:
                    cout << "\n[ERROR] Invalid choice! Please try again." << endl;
                    pressEnterToContinue();
            }
        }
    }
    
    return 0;
}
