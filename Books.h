#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <iostream>

using namespace std;

class Book {
public:
    string isbn;
    string title;
    string author;
    string category;
    double price;
    int stock;

    Book() {}
    Book(string i, string t, string a, string c, double p, int s) 
        : isbn(i), title(t), author(a), category(c), price(p), stock(s) {}

    string serialize() const {
        return isbn + "|" + title + "|" + author + "|" + category + "|" + to_string(price) + "|" + to_string(stock);
    }
    
    string toJson() const {
        return "{ \"isbn\": \"" + isbn + "\", \"title\": \"" + title + "\", \"author\": \"" + author + 
               "\", \"category\": \"" + category + "\", \"price\": " + to_string(price) + 
               ", \"stock\": " + to_string(stock) + " }";
    }
    
    string getIsbn() const { return isbn; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
};

#endif
