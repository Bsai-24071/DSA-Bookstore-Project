#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "Books.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

struct UserBookData {
    string username;
    vector<string> bookISBNs;
};

unsigned long long xorHash(const string& str) {
    unsigned long long hash = 0;
    for (size_t i = 0; i < str.length(); i++) {
        hash ^= str[i];
        hash *= 31;
    }
    return hash;
}

class User {
public:
    string username;
    unsigned long long hashedPassword;
    vector<Book*> myBooks;
    bool isAdmin;
    User() : hashedPassword(0), isAdmin(false) {}
    User(string uname, unsigned long long hpass, bool admin = false) 
        : username(uname), hashedPassword(hpass), isAdmin(admin) {}
    void addBook(Book* book) {
        myBooks.push_back(book);
    }
    bool removeBook(string isbn) {
        for (size_t i = 0; i < myBooks.size(); i++) {
            if (myBooks[i]->isbn == isbn) {
                myBooks.erase(myBooks.begin() + i);
                return true;
            }
        }
        return false;
    }

    Book* findBookInMyBooks(string isbn) {
        for (size_t i = 0; i < myBooks.size(); i++) {
            if (myBooks[i]->isbn == isbn) {
                return myBooks[i];
            }
        }
        return nullptr;
    }

    string serialize() const {
        string result = username + "|" + to_string(hashedPassword) + "|" + (isAdmin ? "1" : "0") + "|";
        for (size_t i = 0; i < myBooks.size(); i++) {
            result += myBooks[i]->isbn;
            if (i < myBooks.size() - 1) {
                result += ",";
            }
        }
        return result;
    }
};

class UserManager {
private:
    static const int TABLE_SIZE = 100;
    vector<User> userTable[TABLE_SIZE];
    string userFile;
    vector<UserBookData> userBookISBNs;
    
    int hashUsername(const string& username) {
        int hash = 0;
        for (size_t i = 0; i < username.length(); i++) {
            hash = (hash * 31 + username[i]) % TABLE_SIZE;
        }
        return hash;
    }

public:
    UserManager(void* db = nullptr, string filename = "users_data.txt") : userFile(filename) {
        loadFromDisk();
        bool hasAdmin = false;
        for (int i = 0; i < TABLE_SIZE && !hasAdmin; i++) {
            for (size_t j = 0; j < userTable[i].size(); j++) {
                if (userTable[i][j].username == "admin") {
                    hasAdmin = true;
                    break;
                }
            }
        }
        if (!hasAdmin) {
            registerUser("admin", "admin123", true);
        }
    }
    
    template<typename DBManager>
    void setDatabaseManager(DBManager* db) {
        for (size_t i = 0; i < userBookISBNs.size(); i++) {
            User* user = getUser(userBookISBNs[i].username);
            if (user) {
                user->myBooks.clear();
                for (size_t j = 0; j < userBookISBNs[i].bookISBNs.size(); j++) {
                    Book* book = db->searchByISBN(userBookISBNs[i].bookISBNs[j]);
                    if (book) {
                        user->myBooks.push_back(book);
                    }
                }
            }
        }
        userBookISBNs.clear();
    }
    
    void reloadUserBooks() {
    }

    bool registerUser(string username, string password, bool isAdmin = false) {
    int index = hashUsername(username);
    for (size_t i = 0; i < userTable[index].size(); i++) {
        if (userTable[index][i].username == username) {
            return false;
        }
    }

    unsigned long long hashedPwd = xorHash(password);
    User newUser(username, hashedPwd, isAdmin);
    userTable[index].push_back(newUser);
    saveToDisk();
    return true;
}

User* login(string username, string password) {
    int index = hashUsername(username);
    unsigned long long hashedPwd = xorHash(password);
    for (size_t i = 0; i < userTable[index].size(); i++) {
        if (userTable[index][i].username == username &&
            userTable[index][i].hashedPassword == hashedPwd) {
            return &userTable[index][i];
        }
    }
    return nullptr;
}

User* getUser(string username) {
    int index = hashUsername(username);
    for (size_t i = 0; i < userTable[index].size(); i++) {
        if (userTable[index][i].username == username) {
            return &userTable[index][i];
        }
    }
    return nullptr;
}

void saveToDisk() {
    ofstream out(userFile);
    if (!out.is_open()) {
        return;
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        for (size_t j = 0; j < userTable[i].size(); j++) {
            out << userTable[i][j].serialize() << "\n";
        }
    }
    out.close();
}

bool loadFromDisk() {
    ifstream in(userFile);
    if (!in.is_open()) {
        return false;
    }
    string line;
    while (getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        size_t pos1 = line.find('|');
        if (pos1 == string::npos) {
            continue;
        }
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos2 == string::npos) {
            continue;
        }
        size_t pos3 = line.find('|', pos2 + 1);
        if (pos3 == string::npos) {
            continue;
        }
        string username = line.substr(0, pos1);
        string hashedPwdStr = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string isAdminStr = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string booksStr = line.substr(pos3 + 1);
        unsigned long long hashedPwd = stoull(hashedPwdStr);
        bool isAdmin = (isAdminStr == "1");
        User newUser(username, hashedPwd, isAdmin);
        if (!booksStr.empty()) {
            UserBookData userData;
            userData.username = username;
            stringstream ss(booksStr);
            string isbn;
            while (getline(ss, isbn, ',')) {
                userData.bookISBNs.push_back(isbn);
            }
            userBookISBNs.push_back(userData);
        }
        int index = hashUsername(username);
        userTable[index].push_back(newUser);
    }
    in.close();
    return true;
}

};

#endif
