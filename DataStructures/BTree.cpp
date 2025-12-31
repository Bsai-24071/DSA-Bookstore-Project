#include "BTree.h"
#include <iostream>

BTreeNode::BTreeNode(int _minDegree, bool _leaf) {
    minDegree = _minDegree;
    leaf = _leaf;
    keys = new string[2 * minDegree - 1];
    books = new Book[2 * minDegree - 1];
    C = new BTreeNode *[2 * minDegree];
    numKeys = 0;
}

void BTreeNode::traverse(vector<Book> &result) {
    int i;
    for (i = 0; i < numKeys; i++) {
        if (!leaf) {
            C[i]->traverse(result);
        }
        result.push_back(books[i]);
    }
    if (!leaf) {
        C[i]->traverse(result);
    }
}

Book* BTreeNode::search(string k) {
    int i = 0;
    while (i < numKeys && k > keys[i]) {
        i++;
    }
    if (keys[i] == k) {
        return &books[i];
    }
    if (leaf) {
        return nullptr;
    }
    return C[i]->search(k);
}

void BTreeNode::splitChild(int i, BTreeNode *y) {
    BTreeNode *z = new BTreeNode(y->minDegree, y->leaf);
    z->numKeys = y->minDegree - 1;

    for (int j = 0; j < y->minDegree - 1; j++) {
        z->keys[j] = y->keys[j + y->minDegree];
        z->books[j] = y->books[j + y->minDegree];
    }

    if (!y->leaf) {
        for (int j = 0; j < y->minDegree; j++) {
            z->C[j] = y->C[j + y->minDegree];
        }
    }

    y->numKeys = y->minDegree - 1;
    for (int j = numKeys; j >= i + 1; j--) {
        C[j + 1] = C[j];
    }
    C[i + 1] = z;

    for (int j = numKeys - 1; j >= i; j--) {
        keys[j + 1] = keys[j];
        books[j + 1] = books[j];
    }
    keys[i] = y->keys[y->minDegree - 1];
    books[i] = y->books[y->minDegree - 1];
    numKeys = numKeys + 1;
}

void BTreeNode::insertNonFull(string k, Book b) {
    int i = numKeys - 1;
    if (leaf) {
        while (i >= 0 && keys[i] > k) {
            keys[i + 1] = keys[i];
            books[i + 1] = books[i];
            i--;
        }
        keys[i + 1] = k;
        books[i + 1] = b;
        numKeys = numKeys + 1;
    } else {
        while (i >= 0 && keys[i] > k) {
            i--;
        }
        if (C[i + 1]->numKeys == 2 * minDegree - 1) {
            splitChild(i + 1, C[i + 1]);
            if (keys[i + 1] < k) {
                i++;
            }
        }
        C[i + 1]->insertNonFull(k, b);
    }
}

BTree::BTree(int _minDegree) {
    root = nullptr;
    minDegree = _minDegree;
}

void BTree::traverse(vector<Book> &result) {
    if (root != nullptr) {
        root->traverse(result);
    }
}

Book* BTree::search(string k) {
    return (root == nullptr) ? nullptr : root->search(k);
}

void BTree::insert(string k, Book b) {
    if (root == nullptr) {
        root = new BTreeNode(minDegree, true);
        root->keys[0] = k;
        root->books[0] = b;
        root->numKeys = 1;
    } else {
        if (root->numKeys == 2 * minDegree - 1) {
            BTreeNode *s = new BTreeNode(minDegree, false);
            s->C[0] = root;
            s->splitChild(0, root);
            int i = 0;
            if (s->keys[0] < k) {
                i++;
            }
            s->C[i]->insertNonFull(k, b);
            root = s;
        } else {
            root->insertNonFull(k, b);
        }
    }
}
