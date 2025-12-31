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
    int salesCount;

    Book() : salesCount(0) {}
    Book(string i, string t, string a, string c, double p, int s, int sc = 0) 
        : isbn(i), title(t), author(a), category(c), price(p), stock(s), salesCount(sc) {}

    string serialize() const {
        return isbn + "|" + title + "|" + author + "|" + category + "|" + to_string(price) + "|" + to_string(stock) + "|" + to_string(salesCount);
    }
    
    string toJson() const {
        return "{ \"isbn\": \"" + isbn + "\", \"title\": \"" + title + "\", \"author\": \"" + author + 
               "\", \"category\": \"" + category + "\", \"price\": " + to_string(price) + 
               ", \"stock\": " + to_string(stock) + ", \"salesCount\": " + to_string(salesCount) + " }";
    }
    
    string getIsbn() const { return isbn; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    int getSalesCount() const { return salesCount; }
};

#endif
