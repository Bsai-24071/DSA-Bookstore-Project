#ifndef BTREE_H
#define BTREE_H
#include <vector>
#include <string>
#include "Books.h"

using namespace std;

class BTreeNode {
public:
    string *keys;
    Book *books;
    BTreeNode **C;
    int minDegree;
    int numKeys;
    bool leaf;

    BTreeNode(int minDegree, bool leaf);
    void insertNonFull(string k, Book b);
    void splitChild(int i, BTreeNode *y);
    void traverse(vector<Book> &result);
    Book* search(string k);
};

class BTree {
private:
    BTreeNode *root;
    int minDegree;

public:
    BTree(int minDegree);
    void insert(string k, Book b);
    void traverse(vector<Book> &result);
    Book* search(string k);
};

#endif
