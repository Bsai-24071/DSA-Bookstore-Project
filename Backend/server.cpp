#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include "../DataStructures/DatabaseManager.h"
#include "../DataStructures/UserManager.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

using namespace std;

DatabaseManager* db;
UserManager* um;

string urlDecode(const string& str) {
    string decoded;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            istringstream is(str.substr(i + 1, 2));
            if (is >> hex >> value) {
                decoded += static_cast<char>(value);
                i += 2;
            }
        } else if (str[i] == '+') {
            decoded += ' ';
        } else {
            decoded += str[i];
        }
    }
    return decoded;
}

string jsonEscape(const string& str) {
    string escaped;
    for (char c : str) {
        if (c == '"') escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else if (c == '\n') escaped += "\\n";
        else if (c == '\r') escaped += "\\r";
        else if (c == '\t') escaped += "\\t";
        else escaped += c;
    }
    return escaped;
}

string createHTTPResponse(int statusCode, const string& contentType, const string& body) {
    ostringstream response;
    string statusText = (statusCode == 200) ? "OK" : (statusCode == 404) ? "Not Found" : "Error";
    
    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
    response << "Access-Control-Allow-Headers: Content-Type\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;
    
    return response.str();
}

string getAllBooksJSON() {
    vector<Book> books = db->getAllBooks();
    ostringstream json;
    json << "[";
    for (size_t i = 0; i < books.size(); i++) {
        if (i > 0) json << ",";
        json << "{";
        json << "\"isbn\":\"" << jsonEscape(books[i].isbn) << "\",";
        json << "\"title\":\"" << jsonEscape(books[i].title) << "\",";
        json << "\"author\":\"" << jsonEscape(books[i].author) << "\",";
        json << "\"category\":\"" << jsonEscape(books[i].category) << "\",";
        json << "\"price\":" << books[i].price << ",";
        json << "\"stock\":" << books[i].stock << ",";
        json << "\"salesCount\":" << books[i].salesCount;
        json << "}";
    }
    json << "]";
    return json.str();
}

string getBookByISBNJSON(const string& isbn) {
    Book* book = db->searchByISBN(isbn);
    if (!book) {
        return "{\"error\":\"Book not found\"}";
    }
    ostringstream json;
    json << "{";
    json << "\"isbn\":\"" << jsonEscape(book->isbn) << "\",";
    json << "\"title\":\"" << jsonEscape(book->title) << "\",";
    json << "\"author\":\"" << jsonEscape(book->author) << "\",";
    json << "\"category\":\"" << jsonEscape(book->category) << "\",";
    json << "\"price\":" << book->price << ",";
    json << "\"stock\":" << book->stock << ",";
    json << "\"salesCount\":" << book->salesCount;
    json << "}";
    return json.str();
}

string getBooksByCategoryJSON(const string& category) {
    vector<Book*> books = db->searchByCategory(category);
    ostringstream json;
    json << "[";
    for (size_t i = 0; i < books.size(); i++) {
        if (i > 0) json << ",";
        json << "{";
        json << "\"isbn\":\"" << jsonEscape(books[i]->isbn) << "\",";
        json << "\"title\":\"" << jsonEscape(books[i]->title) << "\",";
        json << "\"author\":\"" << jsonEscape(books[i]->author) << "\",";
        json << "\"category\":\"" << jsonEscape(books[i]->category) << "\",";
        json << "\"price\":" << books[i]->price << ",";
        json << "\"stock\":" << books[i]->stock << ",";
        json << "\"salesCount\":" << books[i]->salesCount;
        json << "}";
    }
    json << "]";
    return json.str();
}

string getBooksByPriceRangeJSON(double minPrice, double maxPrice) {
    vector<Book*> books = db->searchByPrice(minPrice, maxPrice);
    ostringstream json;
    json << "[";
    for (size_t i = 0; i < books.size(); i++) {
        if (i > 0) json << ",";
        json << "{";
        json << "\"isbn\":\"" << jsonEscape(books[i]->isbn) << "\",";
        json << "\"title\":\"" << jsonEscape(books[i]->title) << "\",";
        json << "\"author\":\"" << jsonEscape(books[i]->author) << "\",";
        json << "\"category\":\"" << jsonEscape(books[i]->category) << "\",";
        json << "\"price\":" << books[i]->price << ",";
        json << "\"stock\":" << books[i]->stock << ",";
        json << "\"salesCount\":" << books[i]->salesCount;
        json << "}";
    }
    json << "]";
    return json.str();
}

string parseJSONValue(const string& json, const string& key) {
    string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == string::npos) return "";
    
    pos = json.find(":", pos);
    if (pos == string::npos) return "";
    
    pos++;
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    
    if (json[pos] == '"') {
        pos++;
        size_t endPos = json.find('"', pos);
        if (endPos == string::npos) return "";
        return json.substr(pos, endPos - pos);
    } else {
        size_t endPos = pos;
        while (endPos < json.length() && json[endPos] != ',' && json[endPos] != '}' && json[endPos] != ' ') {
            endPos++;
        }
        return json.substr(pos, endPos - pos);
    }
}

string getRequestBody(const string& request) {
    size_t pos = request.find("\r\n\r\n");
    if (pos != string::npos) {
        return request.substr(pos + 4);
    }
    return "";
}

void handleClient(SOCKET clientSocket) {
    char buffer[4096] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0) {
        closesocket(clientSocket);
        return;
    }
    
    string request(buffer);
    istringstream requestStream(request);
    string method, path, version;
    requestStream >> method >> path >> version;
    
    cout << "[REQUEST] " << method << " " << path << endl;
    
    string response;
    
    // Handle CORS preflight
    if (method == "OPTIONS") {
        response = createHTTPResponse(200, "text/plain", "OK");
    }
    // Health check
    else if (path == "/api/health" || path == "/health") {
        response = createHTTPResponse(200, "application/json", 
            "{\"status\":\"ok\",\"message\":\"Bookstore Backend Running\"}");
    }
    // Get all books
    else if (path == "/api/books" && method == "GET") {
        response = createHTTPResponse(200, "application/json", getAllBooksJSON());
    }
    // Search by ISBN
    else if (path.find("/api/books/isbn/") == 0 && method == "GET") {
        string isbn = urlDecode(path.substr(16));
        response = createHTTPResponse(200, "application/json", getBookByISBNJSON(isbn));
    }
    // Search by category
    else if (path.find("/api/books/category/") == 0 && method == "GET") {
        string category = urlDecode(path.substr(20));
        response = createHTTPResponse(200, "application/json", getBooksByCategoryJSON(category));
    }
    // Search by price range: /api/books/price/10/50
    else if (path.find("/api/books/price/") == 0 && method == "GET") {
        string priceStr = path.substr(17);
        size_t slashPos = priceStr.find('/');
        if (slashPos != string::npos) {
            double minPrice = stod(priceStr.substr(0, slashPos));
            double maxPrice = stod(priceStr.substr(slashPos + 1));
            response = createHTTPResponse(200, "application/json", 
                getBooksByPriceRangeJSON(minPrice, maxPrice));
        } else {
            response = createHTTPResponse(400, "application/json", 
                "{\"error\":\"Invalid price range format\"}");
        }
    }
    // Get dashboard stats
    else if (path == "/api/dashboard" && method == "GET") {
        vector<BookStat> lowStock = db->getLowStockBooks(10);
        vector<BookStat> bestSellers = db->getBestSellers(10);
        
        ostringstream json;
        json << "{\"lowStock\":[";
        for (size_t i = 0; i < lowStock.size(); i++) {
            if (i > 0) json << ",";
            json << "{\"isbn\":\"" << jsonEscape(lowStock[i].isbn) << "\",\"stock\":" << lowStock[i].count << "}";
        }
        json << "],\"bestSellers\":[";
        for (size_t i = 0; i < bestSellers.size(); i++) {
            if (i > 0) json << ",";
            json << "{\"isbn\":\"" << jsonEscape(bestSellers[i].isbn) << "\",\"sales\":" << bestSellers[i].count << "}";
        }
        json << "]}";
        
        response = createHTTPResponse(200, "application/json", json.str());
    }
    // Add new book (POST)
    else if (path == "/api/books" && method == "POST") {
        string body = getRequestBody(request);
        string isbn = parseJSONValue(body, "isbn");
        string title = parseJSONValue(body, "title");
        string author = parseJSONValue(body, "author");
        string category = parseJSONValue(body, "category");
        double price = stod(parseJSONValue(body, "price"));
        int stock = stoi(parseJSONValue(body, "stock"));
        
        db->addBook(isbn, title, author, category, price, stock);
        response = createHTTPResponse(200, "application/json", 
            "{\"success\":true,\"message\":\"Book added successfully\"}");
    }
    // Update stock (PUT) - CHECK THIS BEFORE GENERAL UPDATE!
    else if (path.find("/api/books/") == 0 && path.find("/stock") != string::npos && method == "PUT") {
        size_t isbnStart = 11;
        size_t isbnEnd = path.find("/stock");
        string isbn = urlDecode(path.substr(isbnStart, isbnEnd - isbnStart));
        string body = getRequestBody(request);
        
        string stockStr = parseJSONValue(body, "stock");
        if (stockStr.empty()) {
            response = createHTTPResponse(400, "application/json", 
                "{\"success\":false,\"message\":\"Invalid stock value\"}");
        } else {
            try {
                int newStock = stoi(stockStr);
                bool success = db->updateStock(isbn, newStock);
                if (success) {
                    response = createHTTPResponse(200, "application/json", 
                        "{\"success\":true,\"message\":\"Stock updated successfully\"}");
                } else {
                    response = createHTTPResponse(404, "application/json", 
                        "{\"success\":false,\"message\":\"Book not found\"}");
                }
            } catch (...) {
                response = createHTTPResponse(400, "application/json", 
                    "{\"success\":false,\"message\":\"Invalid stock format\"}");
            }
        }
    }
    // Update book (PUT)
    else if (path.find("/api/books/") == 0 && method == "PUT") {
        string isbn = urlDecode(path.substr(11));
        string body = getRequestBody(request);
        string title = parseJSONValue(body, "title");
        string author = parseJSONValue(body, "author");
        string category = parseJSONValue(body, "category");
        
        string priceStr = parseJSONValue(body, "price");
        if (priceStr.empty()) {
            response = createHTTPResponse(400, "application/json", 
                "{\"success\":false,\"message\":\"Invalid price value\"}");
        } else {
            try {
                double price = stod(priceStr);
                bool success = db->updateBook(isbn, title, author, category, price);
                if (success) {
                    response = createHTTPResponse(200, "application/json", 
                        "{\"success\":true,\"message\":\"Book updated successfully\"}");
                } else {
                    response = createHTTPResponse(404, "application/json", 
                        "{\"success\":false,\"message\":\"Book not found\"}");
                }
            } catch (...) {
                response = createHTTPResponse(400, "application/json", 
                    "{\"success\":false,\"message\":\"Invalid price format\"}");
            }
        }
    }
    // Delete book (DELETE)
    else if (path.find("/api/books/") == 0 && method == "DELETE") {
        string isbn = urlDecode(path.substr(11));
        bool success = db->deleteBook(isbn);
        if (success) {
            response = createHTTPResponse(200, "application/json", 
                "{\"success\":true,\"message\":\"Book deleted successfully\"}");
        } else {
            response = createHTTPResponse(404, "application/json", 
                "{\"success\":false,\"message\":\"Book not found\"}");
        }
    }
    // Process sale (POST)
    else if (path == "/api/sales" && method == "POST") {
        string body = getRequestBody(request);
        string isbn = parseJSONValue(body, "isbn");
        int quantity = stoi(parseJSONValue(body, "quantity"));
        
        bool success = db->processSale(isbn, quantity);
        if (success) {
            response = createHTTPResponse(200, "application/json", 
                "{\"success\":true,\"message\":\"Sale processed successfully\"}");
        } else {
            response = createHTTPResponse(400, "application/json", 
                "{\"success\":false,\"message\":\"Insufficient stock or book not found\"}");
        }
    }
    // Take book (POST)
    else if (path.find("/api/books/") == 0 && path.find("/take") != string::npos && method == "POST") {
        size_t isbnStart = 11;
        size_t isbnEnd = path.find("/take");
        string isbn = urlDecode(path.substr(isbnStart, isbnEnd - isbnStart));
        
        bool success = db->takeBook(isbn);
        if (success) {
            response = createHTTPResponse(200, "application/json", 
                "{\"success\":true,\"message\":\"Book taken successfully\"}");
        } else {
            response = createHTTPResponse(400, "application/json", 
                "{\"success\":false,\"message\":\"Book out of stock or not found\"}");
        }
    }
    // Drop/return book (POST)
    else if (path.find("/api/books/") == 0 && path.find("/drop") != string::npos && method == "POST") {
        size_t isbnStart = 11;
        size_t isbnEnd = path.find("/drop");
        string isbn = urlDecode(path.substr(isbnStart, isbnEnd - isbnStart));
        
        bool success = db->dropBook(isbn);
        if (success) {
            response = createHTTPResponse(200, "application/json", 
                "{\"success\":true,\"message\":\"Book returned successfully\"}");
        } else {
            response = createHTTPResponse(404, "application/json", 
                "{\"success\":false,\"message\":\"Book not found\"}");
        }
    }
    // Login (POST)
    else if (path == "/api/auth/login" && method == "POST") {
        string body = getRequestBody(request);
        string username = parseJSONValue(body, "username");
        string password = parseJSONValue(body, "password");
        
        User* user = um->login(username, password);
        if (user) {
            ostringstream json;
            json << "{\"success\":true,\"user\":{";
            json << "\"username\":\"" << jsonEscape(user->username) << "\",";
            json << "\"isAdmin\":" << (user->isAdmin ? "true" : "false");
            json << "}}";
            response = createHTTPResponse(200, "application/json", json.str());
        } else {
            response = createHTTPResponse(401, "application/json", 
                "{\"success\":false,\"message\":\"Invalid credentials\"}");
        }
    }
    // Register (POST)
    else if (path == "/api/auth/register" && method == "POST") {
        string body = getRequestBody(request);
        string username = parseJSONValue(body, "username");
        string password = parseJSONValue(body, "password");
        
        bool success = um->registerUser(username, password, false);
        if (success) {
            response = createHTTPResponse(200, "application/json", 
                "{\"success\":true,\"message\":\"Registration successful\"}");
        } else {
            response = createHTTPResponse(400, "application/json", 
                "{\"success\":false,\"message\":\"Username already exists\"}");
        }
    }
    else {
        response = createHTTPResponse(404, "application/json", 
            "{\"error\":\"Endpoint not found\"}");
    }
    
    send(clientSocket, response.c_str(), response.length(), 0);
    closesocket(clientSocket);
}

int main() {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "WSAStartup failed" << endl;
            return 1;
        }
    #endif
    
    db = new DatabaseManager("bookstore_data.txt");
    um = new UserManager(nullptr, "users_data.txt");
    
    cout << "===========================================\n";
    cout << "  BOOKSTORE HTTP SERVER\n";
    cout << "===========================================\n";
    cout << "Initializing database..." << endl;
    db->loadFromDisk();
    cout << "Books loaded: " << db->getAllBooks().size() << endl;
    
    cout << "Loading users..." << endl;
    um->loadFromDisk();
    cout << "User system initialized" << endl;
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket" << endl;
        return 1;
    }
    
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(9000);
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(serverSocket);
        return 1;
    }
    
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        closesocket(serverSocket);
        return 1;
    }
    
    cout << "\n✓ Server listening on port 9000" << endl;
    cout << "✓ Waiting for connections...\n" << endl;
    
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket == INVALID_SOCKET) {
            continue;
        }
        
        thread(handleClient, clientSocket).detach();
    }
    
    closesocket(serverSocket);
    
    #ifdef _WIN32
        WSACleanup();
    #endif
    
    delete db;
    delete um;
    
    return 0;
}
