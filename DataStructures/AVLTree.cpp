#include "AVLTree.h"
#include <iostream>
using namespace std;

AVLNode::AVLNode(double price) {
    priceKey = price;
    left = nullptr;
    right = nullptr;
    height = 1;
}

AVLTree::AVLTree() {
    root = nullptr;
}

AVLTree::~AVLTree() {
    deleteTree(root);
}

void AVLTree::deleteTree(AVLNode* node) {
    if (node == nullptr) {
        return;
    }
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

int AVLTree::getHeight(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

int AVLTree::getBalance(AVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

int AVLTree::max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

AVLNode* AVLTree::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    return x;
}

AVLNode* AVLTree::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    return y;
}

AVLNode* AVLTree::insert(AVLNode* node, double price, Book* book) {
    if (node == nullptr) {
        AVLNode* newNode = new AVLNode(price);
        newNode->books.push_back(book);
        return newNode;
    }
    if (price < node->priceKey) {
        node->left = insert(node->left, price, book);
    }
    else if (price > node->priceKey) {
        node->right = insert(node->right, price, book);
    }
    else {
        node->books.push_back(book);
        return node;
    }
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);
    if (balance > 1 && price < node->left->priceKey) {
        return rightRotate(node);
    }
    if (balance < -1 && price > node->right->priceKey) {
        return leftRotate(node);
    }
    if (balance > 1 && price > node->left->priceKey) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && price < node->right->priceKey) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

void AVLTree::insert(Book* book) {
    root = insert(root, book->price, book);
}

void AVLTree::inorder(AVLNode* node, vector<Book*>& result) {
    if (node == nullptr) {
        return;
    }
    inorder(node->left, result);
    for (size_t i = 0; i < node->books.size(); i++) {
        result.push_back(node->books[i]);
    }
    inorder(node->right, result);
}

void AVLTree::rangeQuery(AVLNode* node, double minPrice, double maxPrice, vector<Book*>& result) {
    if (node == nullptr) {
        return;
    }
    if (node->priceKey > minPrice) {
        rangeQuery(node->left, minPrice, maxPrice, result);
    }
    if (node->priceKey >= minPrice && node->priceKey <= maxPrice) {
        for (size_t i = 0; i < node->books.size(); i++) {
            result.push_back(node->books[i]);
        }
    }
    if (node->priceKey < maxPrice) {
        rangeQuery(node->right, minPrice, maxPrice, result);
    }
}

vector<Book*> AVLTree::findBooksInRange(double minPrice, double maxPrice) {
    vector<Book*> result;
    rangeQuery(root, minPrice, maxPrice, result);
    return result;
}

vector<Book*> AVLTree::findBooksUnderPrice(double maxPrice) {
    return findBooksInRange(0.0, maxPrice);
}

vector<Book*> AVLTree::findBooksOverPrice(double minPrice) {
    return findBooksInRange(minPrice, 999999.0);
}

vector<Book*> AVLTree::getAllBooksSorted() {
    vector<Book*> result;
    inorder(root, result);
    return result;
}

AVLNode* AVLTree::findMin(AVLNode* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

AVLNode* AVLTree::remove(AVLNode* node, double price, string isbn) {
    if (node == nullptr) {
        return nullptr;
    }
    if (price < node->priceKey) {
        node->left = remove(node->left, price, isbn);
    }
    else if (price > node->priceKey) {
        node->right = remove(node->right, price, isbn);
    }
    else {
        for (size_t i = 0; i < node->books.size(); i++) {
            if (node->books[i]->isbn == isbn) {
                node->books.erase(node->books.begin() + i);
                break;
            }
        }
        if (node->books.empty()) {
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            }
            else if (node->left == nullptr) {
                AVLNode* temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr) {
                AVLNode* temp = node->left;
                delete node;
                return temp;
            }
            else {
                AVLNode* minRight = findMin(node->right);
                node->priceKey = minRight->priceKey;
                node->books = minRight->books;
                node->right = remove(node->right, minRight->priceKey, minRight->books[0]->isbn);
            }
        }
    }
    if (node == nullptr) {
        return node;
    }
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);
    if (balance > 1 && getBalance(node->left) >= 0) {
        return rightRotate(node);
    }
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0) {
        return leftRotate(node);
    }
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

void AVLTree::remove(string isbn, double price) {
    root = remove(root, price, isbn);
}

void AVLTree::display() {
    vector<Book*> books = getAllBooksSorted();
    if (books.empty()) {
        cout << "No books in price index" << endl;
        return;
    }
    cout << "Books sorted by price:" << endl;
    for (size_t i = 0; i < books.size(); i++) {
        cout << "$" << books[i]->price << " - " << books[i]->title << " by " << books[i]->author << endl;
    }
}

bool AVLTree::isEmpty() {
    return root == nullptr;
}
