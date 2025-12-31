#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// Book title components
vector<string> titleWords1 = {
    "The", "A", "An", "My", "Our", "Your", "His", "Her", "Last", "First",
    "Secret", "Hidden", "Lost", "Found", "Dark", "Bright", "Silent", "Loud",
    "Ancient", "Modern", "Final", "Great", "Broken", "Perfect", "Eternal"
};

vector<string> titleWords2 = {
    "Journey", "Story", "Tale", "Legend", "Mystery", "Adventure", "Quest",
    "War", "Peace", "Love", "Life", "Death", "Time", "Memory", "Dream",
    "Shadow", "Light", "Night", "Day", "World", "Kingdom", "Empire", "City",
    "Mountain", "Ocean", "Forest", "Desert", "River", "Sky", "Star", "Moon"
};

vector<string> titleWords3 = {
    "Warriors", "Dragons", "Kings", "Queens", "Heroes", "Legends", "Secrets",
    "Mysteries", "Dreams", "Shadows", "Lights", "Souls", "Hearts", "Minds",
    "Wizards", "Knights", "Princes", "Thieves", "Hunters", "Seekers"
};

// Author names
vector<string> firstNames = {
    "John", "Jane", "Michael", "Sarah", "David", "Emily", "James", "Emma",
    "Robert", "Olivia", "William", "Sophia", "Richard", "Isabella", "Thomas",
    "Ava", "Daniel", "Mia", "Joseph", "Charlotte", "Charles", "Amelia",
    "Christopher", "Harper", "Matthew", "Evelyn", "Anthony", "Abigail",
    "Mark", "Elizabeth", "Donald", "Sofia", "Steven", "Avery", "Paul",
    "Ella", "Andrew", "Scarlett", "Joshua", "Grace", "Kevin", "Chloe",
    "Brian", "Victoria", "George", "Madison", "Edward", "Luna", "Ronald",
    "Aria", "Timothy", "Layla", "Jason", "Penelope", "Jeffrey", "Riley"
};

vector<string> lastNames = {
    "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller",
    "Davis", "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez",
    "Wilson", "Anderson", "Thomas", "Taylor", "Moore", "Jackson", "Martin",
    "Lee", "Perez", "Thompson", "White", "Harris", "Sanchez", "Clark",
    "Ramirez", "Lewis", "Robinson", "Walker", "Young", "Allen", "King",
    "Wright", "Scott", "Torres", "Nguyen", "Hill", "Flores", "Green",
    "Adams", "Nelson", "Baker", "Hall", "Rivera", "Campbell", "Mitchell",
    "Carter", "Roberts", "Turner", "Phillips", "Evans", "Collins"
};

// Categories
vector<string> categories = {
    "Fiction", "Non-Fiction", "Science Fiction", "Fantasy", "Mystery",
    "Thriller", "Romance", "Horror", "Biography", "History",
    "Science", "Technology", "Business", "Self-Help", "Cooking",
    "Travel", "Art", "Poetry", "Drama", "Children"
};

// Generate random ISBN
string generateISBN(int bookNum) {
    char isbn[20];
    sprintf(isbn, "ISBN%010d", bookNum);
    return string(isbn);
}

// Generate random book title
string generateTitle(int seed) {
    srand(seed);
    string title = titleWords1[rand() % titleWords1.size()] + " ";
    title += titleWords2[rand() % titleWords2.size()];
    
    if (rand() % 2 == 0) {
        title += " of " + titleWords3[rand() % titleWords3.size()];
    }
    
    return title;
}

// Generate random author name
string generateAuthor(int seed) {
    srand(seed * 2);
    string author = firstNames[rand() % firstNames.size()] + " ";
    author += lastNames[rand() % lastNames.size()];
    return author;
}

// Generate random category
string generateCategory(int seed) {
    srand(seed * 3);
    return categories[rand() % categories.size()];
}

// Generate random price between $5.99 and $99.99
double generatePrice(int seed) {
    srand(seed * 5);
    return 5.99 + (rand() % 9400) / 100.0;
}

// Generate random stock between 1 and 100
int generateStock(int seed) {
    srand(seed * 7);
    return 1 + (rand() % 100);
}

// Generate random sales count between 0 and 500
int generateSales(int seed) {
    srand(seed * 11);
    return rand() % 501;
}

int main() {
    cout << "========================================" << endl;
    cout << "  BOOKSTORE DATA GENERATOR" << endl;
    cout << "========================================" << endl;
    
    const int NUM_BOOKS = 10000;
    string filename = "bookstore_data.txt";
    
    cout << "\nGenerating " << NUM_BOOKS << " book entries..." << endl;
    cout << "Output file: " << filename << endl << endl;
    
    ofstream outFile(filename);
    
    if (!outFile.is_open()) {
        cerr << "Error: Could not create file!" << endl;
        return 1;
    }
    
    // Use time as base seed for more randomness
    int baseSeed = static_cast<int>(time(0));
    
    // Progress indicator
    int progressMarker = NUM_BOOKS / 10;
    
    for (int i = 1; i <= NUM_BOOKS; i++) {
        string isbn = generateISBN(i);
        string title = generateTitle(baseSeed + i);
        string author = generateAuthor(baseSeed + i + 1000);
        string category = generateCategory(baseSeed + i + 2000);
        double price = generatePrice(baseSeed + i + 3000);
        int stock = generateStock(baseSeed + i + 4000);
        int sales = generateSales(baseSeed + i + 5000);
        
        // Write in format: ISBN|Title|Author|Category|Price|Stock|SalesCount
        outFile << isbn << "|" 
                << title << "|" 
                << author << "|" 
                << category << "|" 
                << price << "|" 
                << stock << "|" 
                << sales << endl;
        
        // Show progress
        if (i % progressMarker == 0) {
            cout << "Progress: " << (i * 100 / NUM_BOOKS) << "% (" 
                 << i << "/" << NUM_BOOKS << " books)" << endl;
        }
    }
    
    outFile.close();
    
    cout << "\n========================================" << endl;
    cout << "  GENERATION COMPLETE!" << endl;
    cout << "========================================" << endl;
    cout << "\nGenerated " << NUM_BOOKS << " books" << endl;
    cout << "File: " << filename << endl;
    
    // Show sample entries
    cout << "\nSample entries:" << endl;
    cout << "----------------------------------------" << endl;
    
    ifstream inFile(filename);
    string line;
    int count = 0;
    while (getline(inFile, line) && count < 5) {
        cout << line << endl;
        count++;
    }
    inFile.close();
    
    cout << "..." << endl;
    cout << "----------------------------------------" << endl;
    
    cout << "\nYou can now load this file in your bookstore system!" << endl;
    cout << "Expected load time: < 2 seconds for optimized code" << endl << endl;
    
    return 0;
}
