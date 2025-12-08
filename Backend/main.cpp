#include "httplib.h"
#include "DatabaseManager.h"
#include <iostream>
#include <mutex>
#include <vector>
#include <string>

using namespace std;

mutex dbMutex;

int main() {
    DatabaseManager manager("bookstore_data.txt");
    httplib::Server server;

    cout << "Starting Distributed Bookstore Server on Port 9000..." << endl;

    server.Get("/add", [&](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(dbMutex);

        string isbnVal = req.get_param_value("isbn");
        string titleVal = req.get_param_value("title");
        string authorVal = req.get_param_value("author");
        string categoryVal = req.get_param_value("cat");
        string priceVal = req.get_param_value("price");
        string stockVal = req.get_param_value("stock");

        if (isbnVal.empty() || titleVal.empty() || priceVal.empty() || stockVal.empty()) {
            res.set_content("Error: Missing parameters.", "text/plain");
            return;
        }

        try {
            double price = stod(priceVal);
            int stock = stoi(stockVal);

            manager.addBook(isbnVal, titleVal, authorVal, categoryVal, price, stock);
            
            cout << "Added: " << titleVal << endl;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content("Book Added Successfully", "text/plain");
        } catch (...) {
            res.set_content("Error: Invalid input format.", "text/plain");
        }
    });

    server.Get("/search", [&](const httplib::Request& req, httplib::Response& res) {
        string isbnVal = req.get_param_value("isbn");
        Book* book = manager.searchByISBN(isbnVal);

        res.set_header("Access-Control-Allow-Origin", "*");
        if (book) {
            res.set_content(book->toJson(), "application/json");
        } else {
            res.set_content("{}", "application/json");
        }
    });

    server.Get("/search_price", [&](const httplib::Request& req, httplib::Response& res) {
        string minPriceStr = req.get_param_value("min");
        string maxPriceStr = req.get_param_value("max");

        double minPrice = minPriceStr.empty() ? 0.0 : stod(minPriceStr);
        double maxPrice = maxPriceStr.empty() ? 100000.0 : stod(maxPriceStr);

        vector<Book*> books = manager.searchByPrice(minPrice, maxPrice);

        string jsonResponse = "[";
        for (size_t i = 0; i < books.size(); i++) {
            jsonResponse += books[i]->toJson();
            if (i < books.size() - 1) {
                jsonResponse += ",";
            }
        }
        jsonResponse += "]";

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(jsonResponse, "application/json");
    });

    server.Get("/inventory", [&](const httplib::Request& req, httplib::Response& res) {
        vector<Book> books = manager.getAllBooks();
        string jsonResponse = "[";
        for (size_t i = 0; i < books.size(); i++) {
            jsonResponse += books[i].toJson();
            if (i < books.size() - 1) {
                jsonResponse += ",";
            }
        }
        jsonResponse += "]";

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(jsonResponse, "application/json");
    });

    if (!server.listen("0.0.0.0", 9000)) {
        cerr << "Error: Could not start server on port 9000." << endl;
        return 1;
    }

    return 0;
}