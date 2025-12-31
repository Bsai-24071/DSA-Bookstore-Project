#ifndef AVLTREE_H
#define AVLTREE_H

#include "Books.h"
#include <vector>
using namespace std;

class AVLNode {
public:
    double priceKey;
    vector<Book*> books;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(double price);
};

class AVLTree {
private:
    AVLNode* root;
    int getHeight(AVLNode* node);
    int getBalance(AVLNode* node);
    int max(int a, int b);
    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);
    AVLNode* insert(AVLNode* node, double price, Book* book);
    AVLNode* remove(AVLNode* node, double price, string isbn);
    AVLNode* findMin(AVLNode* node);
    void inorder(AVLNode* node, vector<Book*>& result);
    void rangeQuery(AVLNode* node, double minPrice, double maxPrice, vector<Book*>& result);
    void deleteTree(AVLNode* node);
    
public:
    AVLTree();
    ~AVLTree();
    void insert(Book* book);
    void remove(string isbn, double price);
    vector<Book*> findBooksInRange(double minPrice, double maxPrice);
    vector<Book*> findBooksUnderPrice(double maxPrice);
    vector<Book*> findBooksOverPrice(double minPrice);
    vector<Book*> getAllBooksSorted();
    void display();
    bool isEmpty();
};

#endif
