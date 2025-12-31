// ==========================================
// BOOKSTORE API CONFIGURATION
// ==========================================

// STEP 1: Replace 'YOUR_AWS_PUBLIC_IP' with your actual AWS EC2 Public IP
// Example: const API_BASE_URL = 'http://54.123.45.67:9000';

const API_BASE_URL = 'http://3.94.129.50:9000';
// For local testing (Windows machine), use:
// const API_BASE_URL = 'http://localhost:9000';

// ==========================================
// API ENDPOINTS
// ==========================================
// IMPORTANT: These paths match server.cpp exactly
// - /api/books/isbn/ is 16 chars (path.substr(16) extracts ISBN)
// - /api/books/category/ is 20 chars (path.substr(20) extracts category)
// - /api/books/price/ is 17 chars (path.substr(17) extracts price range)
const API_ENDPOINTS = {
    health: `${API_BASE_URL}/api/health`,
    books: {
        getAll: `${API_BASE_URL}/api/books`,
        searchByISBN: (isbn) => `${API_BASE_URL}/api/books/isbn/${isbn}`,
        searchByCategory: (category) => `${API_BASE_URL}/api/books/category/${category}`,
        searchByPrice: (min, max) => `${API_BASE_URL}/api/books/price/${min}/${max}`,
        updateStock: (isbn) => `${API_BASE_URL}/api/books/${isbn}/stock`,
        take: (isbn) => `${API_BASE_URL}/api/books/${isbn}/take`,
        drop: (isbn) => `${API_BASE_URL}/api/books/${isbn}/drop`
    },
    sales: `${API_BASE_URL}/api/sales`,
    auth: {
        login: `${API_BASE_URL}/api/auth/login`,
        register: `${API_BASE_URL}/api/auth/register`
    },
    dashboard: `${API_BASE_URL}/api/dashboard`
};

// Export for use in script.js
window.API_CONFIG = {
    BASE_URL: API_BASE_URL,
    ENDPOINTS: API_ENDPOINTS
};
