
function hashPassword(password) {
    let hash = 0;
    for (let i = 0; i < password.length; i++) {
        hash ^= password.charCodeAt(i);
        hash *= 31;
    }
    return hash.toString();
}

const DatabaseStore = {
    books: [],
    
    users: [
        { username: "admin", hashedPassword: "-23211549491", isAdmin: true, myBooks: [] },
        { username: "john", hashedPassword: "-26618169685", isAdmin: false, myBooks: [] }
    ],
    
    currentUser: null,

    async searchByISBN(isbn) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.searchByISBN(isbn));
            if (response.ok) {
                const book = await response.json();
                if (book.error) return null;
                return book;
            }
        } catch (error) {
            console.error('Search by ISBN failed:', error);
        }
        return null;
    },

    async searchByCategory(category) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.searchByCategory(category));
            if (response.ok) {
                const books = await response.json();
                return books;
            }
        } catch (error) {
            console.error('Search by category failed:', error);
        }
        return [];
    },

    async searchByPrice(minPrice, maxPrice) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.searchByPrice(minPrice, maxPrice));
            if (response.ok) {
                const books = await response.json();
                return books;
            }
        } catch (error) {
            console.error('Search by price failed:', error);
        }
        return [];
    },

    getLowStockBooks(limit = 10) {
        return [...this.books]
            .sort((a, b) => a.stock - b.stock)
            .slice(0, limit);
    },

    getBestSellers(limit = 10) {
        return [...this.books]
            .sort((a, b) => b.salesCount - a.salesCount)
            .slice(0, limit);
    },

    async addBook(isbn, title, author, category, price, stock) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.getAll, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ isbn, title, author, category, price, stock })
            });
            if (response.ok) {
                const result = await response.json();
                await this.loadBooksFromBackend();
                return { success: true, message: "Book added successfully!" };
            }
        } catch (error) {
            console.error('Add book failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
        return { success: false, message: "Failed to add book" };
    },

    async updateBook(isbn, title, author, category, price) {
        try {
            const response = await fetch(`${window.API_CONFIG.BASE_URL}/api/books/${isbn}`, {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ title, author, category, price })
            });
            if (response.ok) {
                await this.loadBooksFromBackend();
                return { success: true, message: "Book updated successfully!" };
            }
        } catch (error) {
            console.error('Update book failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
        return { success: false, message: "Book not found!" };
    },

    async deleteBook(isbn) {
        try {
            const response = await fetch(`${window.API_CONFIG.BASE_URL}/api/books/${isbn}`, {
                method: 'DELETE'
            });
            if (response.ok) {
                await this.loadBooksFromBackend();
                return { success: true, message: "Book deleted successfully!" };
            }
        } catch (error) {
            console.error('Delete book failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
        return { success: false, message: "Book not found!" };
    },

    async processSale(isbn, quantity) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.sales, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ isbn, quantity })
            });
            if (response.ok) {
                const result = await response.json();
                if (result.success) {
                    await this.loadBooksFromBackend();
                    const book = this.books.find(b => b.isbn === isbn);
                    return { success: true, message: result.message, total: book ? book.price * quantity : 0 };
                }
            }
        } catch (error) {
            console.error('Process sale failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
        return { success: false, message: "Insufficient stock or book not found" };
    },

    async updateStock(isbn, newStock) {
        try {
            const payload = { stock: parseInt(newStock) };
            console.log('Updating stock for', isbn, 'with payload:', payload);
            
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.updateStock(isbn), {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });
            
            console.log('Stock update response status:', response.status);
            
            if (response.ok) {
                const result = await response.json();
                await this.loadBooksFromBackend();
                return { success: true, message: result.message || "Stock updated successfully" };
            } else {
                const errorText = await response.text();
                console.error('Stock update failed with status:', response.status, 'Body:', errorText);
                return { success: false, message: `Stock update failed (${response.status})` };
            }
        } catch (error) {
            console.error('Update stock failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
    },

    async takeBook(isbn) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.take(isbn), {
                method: 'POST'
            });
            if (response.ok) {
                const result = await response.json();
                if (result.success) {
                    await this.loadBooksFromBackend();
                    this.currentUser.myBooks.push(isbn);
                    // Also update in users array
                    const userInArray = this.users.find(u => u.username === this.currentUser.username);
                    if (userInArray) {
                        userInArray.myBooks = [...this.currentUser.myBooks];
                    }
                    this.saveToLocalStorage();
                    return { success: true, message: result.message };
                }
            }
        } catch (error) {
            console.error('Take book failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
        return { success: false, message: "Book out of stock or not found" };
    },

    async dropBook(isbn) {
        const index = this.currentUser.myBooks.indexOf(isbn);
        if (index === -1) {
            return { success: false, message: "You don't have this book!" };
        }
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.drop(isbn), {
                method: 'POST'
            });
            if (response.ok) {
                const result = await response.json();
                if (result.success) {
                    await this.loadBooksFromBackend();
                    this.currentUser.myBooks.splice(index, 1);
                    // Also update in users array
                    const userInArray = this.users.find(u => u.username === this.currentUser.username);
                    if (userInArray) {
                        userInArray.myBooks = [...this.currentUser.myBooks];
                    }
                    this.saveToLocalStorage();
                    return { success: true, message: result.message };
                }
            }
        } catch (error) {
            console.error('Drop book failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
        return { success: false, message: "Book not found!" };
    },

    async login(username, password) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.auth.login, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ username, password })
            });
            if (response.ok) {
                const result = await response.json();
                if (result.success) {
                    // Preserve myBooks from localStorage if this user was logged in before
                    const existingUser = this.users.find(u => u.username === result.user.username);
                    const myBooks = existingUser?.myBooks || [];
                    
                    // If user not in local array, add them
                    if (!existingUser) {
                        this.users.push({
                            username: result.user.username,
                            hashedPassword: '',
                            isAdmin: result.user.isAdmin,
                            myBooks: []
                        });
                    }
                    
                    this.currentUser = { ...result.user, myBooks };
                    this.saveToLocalStorage();
                    return { success: true, user: this.currentUser };
                }
            }
        } catch (error) {
            console.error('Login failed:', error);
        }
        return { success: false, message: "Invalid credentials!" };
    },

    async register(username, password) {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.auth.register, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ username, password })
            });
            if (response.ok) {
                const result = await response.json();
                if (result.success) {
                    // Add new user to local users array with myBooks initialized
                    this.users.push({
                        username: username,
                        hashedPassword: hashPassword(password),
                        isAdmin: false,
                        myBooks: []
                    });
                    this.saveToLocalStorage();
                    return { success: true, message: "Registration successful! Please login." };
                }
            }
            const errorResult = await response.json();
            return { success: false, message: errorResult.message || "Registration failed" };
        } catch (error) {
            console.error('Registration failed:', error);
            return { success: false, message: "Failed to connect to server" };
        }
    },

    logout() {
        // Make sure current user's books are saved to users array before logout
        if (this.currentUser) {
            const userInArray = this.users.find(u => u.username === this.currentUser.username);
            if (userInArray) {
                userInArray.myBooks = [...this.currentUser.myBooks];
            }
        }
        this.currentUser = null;
        this.saveToLocalStorage();
    },

    saveToLocalStorage() {
        localStorage.setItem('bookstore_data', JSON.stringify({
            books: this.books,
            users: this.users,
            currentUser: this.currentUser
        }));
    },

    loadFromLocalStorage() {
        const data = localStorage.getItem('bookstore_data');
        if (data) {
            const parsed = JSON.parse(data);
            this.books = parsed.books || this.books;
            this.users = parsed.users || this.users;
            this.currentUser = parsed.currentUser || null;
            
            const adminUser = this.users.find(u => u.username === "admin");
            if (!adminUser) {
                this.users.unshift({ username: "admin", hashedPassword: "-23211549491", isAdmin: true, myBooks: [] });
            } else if (adminUser.hashedPassword !== "-23211549491") {
                adminUser.hashedPassword = "-23211549491";
            }
            
            return true;
        }
        return false;
    },

    async loadBooksFromBackend() {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.books.getAll);
            if (response.ok) {
                this.books = await response.json();
                this.saveToLocalStorage();
                return true;
            }
        } catch (error) {
            console.error('Failed to load books from backend:', error);
            showToast('Failed to connect to backend server', 'error');
        }
        return false;
    },

    async loadDashboardData() {
        try {
            const response = await fetch(window.API_CONFIG.ENDPOINTS.dashboard);
            if (response.ok) {
                const data = await response.json();
                return data;
            }
        } catch (error) {
            console.error('Failed to load dashboard data:', error);
        }
        return null;
    }
};

function showToast(message, type = 'success') {
    const container = document.getElementById('toastContainer');
    const toast = document.createElement('div');
    const bgColor = type === 'success' ? 'bg-emerald-600' : type === 'error' ? 'bg-red-600' : 'bg-blue-600';
    const icon = type === 'success' ? 'check-circle' : type === 'error' ? 'x-circle' : 'info';
    
    toast.className = `toast ${bgColor} text-white px-6 py-4 rounded-lg shadow-lg flex items-center gap-3 min-w-[300px]`;
    toast.innerHTML = `
        <i data-lucide="${icon}" class="w-5 h-5"></i>
        <span class="font-medium">${message}</span>
    `;
    
    container.appendChild(toast);
    lucide.createIcons();
    
    setTimeout(() => {
        toast.style.animation = 'slideIn 0.3s ease-out reverse';
        setTimeout(() => toast.remove(), 300);
    }, 3000);
}

function showModal(title, content, actions = []) {
    const modalContainer = document.getElementById('modalContainer');
    modalContainer.innerHTML = `
        <div class="modal-backdrop fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-40">
            <div class="bg-slate-800 rounded-xl shadow-2xl max-w-2xl w-full mx-4 max-h-[90vh] overflow-y-auto">
                <div class="sticky top-0 bg-slate-800 border-b border-slate-700 px-6 py-4 flex justify-between items-center">
                    <h2 class="text-2xl font-bold text-emerald-500">${title}</h2>
                    <button onclick="closeModal()" class="text-slate-400 hover:text-white">
                        <i data-lucide="x" class="w-6 h-6"></i>
                    </button>
                </div>
                <div class="p-6">
                    ${content}
                </div>
                <div class="sticky bottom-0 bg-slate-800 border-t border-slate-700 px-6 py-4 flex justify-end gap-3">
                    ${actions.map(action => `
                        <button onclick="${action.onclick}" class="${action.class}">
                            ${action.label}
                        </button>
                    `).join('')}
                </div>
            </div>
        </div>
    `;
    lucide.createIcons();
}

function closeModal() {
    document.getElementById('modalContainer').innerHTML = '';
}

function renderSidebar() {
    const userInfo = document.getElementById('userInfo');
    const nav = document.getElementById('sidebarNav');
    const logoutBtn = document.getElementById('logoutBtn');
    
    if (DatabaseStore.currentUser) {
        userInfo.innerHTML = `
            <div class="flex items-center gap-3">
                <div class="w-10 h-10 bg-emerald-600 rounded-full flex items-center justify-center">
                    <i data-lucide="user" class="w-5 h-5"></i>
                </div>
                <div>
                    <div class="font-medium">${DatabaseStore.currentUser.username}</div>
                    <div class="text-xs text-emerald-500">${DatabaseStore.currentUser.isAdmin ? 'Admin' : 'User'}</div>
                </div>
            </div>
        `;
        logoutBtn.classList.remove('hidden');
        logoutBtn.textContent = 'Logout';
        logoutBtn.className = 'w-full px-4 py-2 bg-red-600 hover:bg-red-700 rounded-lg text-sm font-medium transition';
        
        if (DatabaseStore.currentUser.isAdmin) {
            nav.innerHTML = `
                <button onclick="renderView('dashboard')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="layout-dashboard" class="w-5 h-5"></i>
                    Dashboard
                </button>
                <button onclick="renderView('allBooks')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="book" class="w-5 h-5"></i>
                    All Books
                </button>
                <button onclick="renderView('addBook')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="plus-circle" class="w-5 h-5"></i>
                    Add Book
                </button>
                <button onclick="renderView('search')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="search" class="w-5 h-5"></i>
                    Search Books
                </button>
                <button onclick="renderView('processSale')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="shopping-cart" class="w-5 h-5"></i>
                    Process Sale
                </button>
            `;
        } else {
            nav.innerHTML = `
                <button onclick="renderView('search')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="search" class="w-5 h-5"></i>
                    Search Books
                </button>
                <button onclick="renderView('takeBook')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="book-plus" class="w-5 h-5"></i>
                    Take a Book
                </button>
                <button onclick="renderView('dropBook')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="book-minus" class="w-5 h-5"></i>
                    Drop a Book
                </button>
                <button onclick="renderView('profile')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                    <i data-lucide="user-circle" class="w-5 h-5"></i>
                    My Profile
                </button>
            `;
        }
    } else {
        userInfo.innerHTML = '<div class="text-sm text-slate-400">Guest Mode</div>';
        logoutBtn.classList.remove('hidden');
        logoutBtn.textContent = 'Back to Login';
        logoutBtn.className = 'w-full px-4 py-2 bg-emerald-600 hover:bg-emerald-700 rounded-lg text-sm font-medium transition';
        nav.innerHTML = `
            <button onclick="renderView('search')" class="nav-item w-full text-left px-4 py-3 rounded-lg hover:bg-slate-700 flex items-center gap-3 text-slate-300 hover:text-white">
                <i data-lucide="search" class="w-5 h-5"></i>
                Search Books
            </button>
        `;
    }
    
    lucide.createIcons();
}

function renderView(viewName) {
    const content = document.getElementById('mainContent');
    content.className = 'p-8 view-transition';
    
    switch(viewName) {
        case 'auth':
            renderAuthView();
            break;
        case 'dashboard':
            renderDashboardView();
            break;
        case 'allBooks':
            renderAllBooksView();
            break;
        case 'addBook':
            renderAddBookView();
            break;
        case 'search':
            renderSearchView();
            break;
        case 'processSale':
            renderProcessSaleView();
            break;
        case 'takeBook':
            renderTakeBookView();
            break;
        case 'dropBook':
            renderDropBookView();
            break;
        case 'profile':
            renderProfileView();
            break;
        default:
            renderWelcomeView();
    }
    
    lucide.createIcons();
}

function renderAuthView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <div class="max-w-md mx-auto">
            <div class="bg-slate-800 rounded-xl p-8 shadow-xl border border-slate-700">
                <h1 class="text-3xl font-bold text-emerald-500 mb-2 flex items-center gap-2">
                    <i data-lucide="shield-check" class="w-8 h-8"></i>
                    Authentication
                </h1>
                <p class="text-slate-400 mb-6">Login or register to continue</p>
                
                <div class="space-y-4">
                    <button onclick="showLoginModal()" class="w-full px-6 py-3 bg-emerald-600 hover:bg-emerald-700 rounded-lg font-medium flex items-center justify-center gap-2">
                        <i data-lucide="log-in" class="w-5 h-5"></i>
                        Login
                    </button>
                    <button onclick="showRegisterModal()" class="w-full px-6 py-3 bg-slate-700 hover:bg-slate-600 rounded-lg font-medium flex items-center justify-center gap-2">
                        <i data-lucide="user-plus" class="w-5 h-5"></i>
                        Register
                    </button>
                    <button onclick="continueAsGuest()" class="w-full px-6 py-3 bg-slate-700 hover:bg-slate-600 rounded-lg font-medium flex items-center justify-center gap-2">
                        <i data-lucide="eye" class="w-5 h-5"></i>
                        Continue as Guest
                    </button>
                </div>
            </div>
        </div>
    `;
}

function renderWelcomeView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <div class="text-center py-20">
            <i data-lucide="book-open" class="w-24 h-24 mx-auto text-emerald-500 mb-6"></i>
            <h1 class="text-5xl font-bold mb-4">Welcome to Bookstore</h1>
            <p class="text-xl text-slate-400 mb-8">Management System</p>
            
            <div class="grid grid-cols-1 md:grid-cols-3 gap-6 max-w-4xl mx-auto mt-12">
                <div class="bg-slate-800 p-6 rounded-xl border border-slate-700">
                    <i data-lucide="hash" class="w-12 h-12 text-emerald-500 mx-auto mb-3"></i>
                    <h3 class="font-semibold mb-2">Hash Table</h3>
                    <p class="text-sm text-slate-400">O(1) ISBN Lookups</p>
                </div>
                <div class="bg-slate-800 p-6 rounded-xl border border-slate-700">
                    <i data-lucide="git-branch" class="w-12 h-12 text-emerald-500 mx-auto mb-3"></i>
                    <h3 class="font-semibold mb-2">B-Tree & AVL</h3>
                    <p class="text-sm text-slate-400">Category & Price Search</p>
                </div>
                <div class="bg-slate-800 p-6 rounded-xl border border-slate-700">
                    <i data-lucide="layers" class="w-12 h-12 text-emerald-500 mx-auto mb-3"></i>
                    <h3 class="font-semibold mb-2">Min/Max Heaps</h3>
                    <p class="text-sm text-slate-400">Stock & Sales Tracking</p>
                </div>
            </div>
        </div>
    `;
}

async function renderDashboardView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = '<div class="flex justify-center items-center h-64"><div class="text-emerald-500">Loading dashboard...</div></div>';
    
    const dashboardData = await DatabaseStore.loadDashboardData();
    if (!dashboardData) {
        content.innerHTML = '<div class="text-center text-red-500 py-8">Failed to load dashboard data</div>';
        return;
    }
    
    await DatabaseStore.loadBooksFromBackend();
    
    const lowStockBooks = dashboardData.lowStock.map(item => {
        const book = DatabaseStore.books.find(b => b.isbn === item.isbn);
        return book ? { ...book, stock: item.stock } : null;
    }).filter(b => b !== null).slice(0, 5);
    
    const bestSellerBooks = dashboardData.bestSellers.map(item => {
        const book = DatabaseStore.books.find(b => b.isbn === item.isbn);
        return book ? { ...book, salesCount: item.sales } : null;
    }).filter(b => b !== null).slice(0, 5);
    
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="layout-dashboard" class="w-8 h-8"></i>
            Dashboard
        </h1>
        
        <div class="grid grid-cols-1 md:grid-cols-3 gap-6 mb-8">
            <div class="bg-slate-800 p-6 rounded-xl border border-slate-700">
                <div class="flex items-center justify-between">
                    <div>
                        <p class="text-slate-400 text-sm">Total Books</p>
                        <p class="text-3xl font-bold text-emerald-500">${DatabaseStore.books.length}</p>
                    </div>
                    <i data-lucide="book" class="w-12 h-12 text-emerald-500 opacity-20"></i>
                </div>
            </div>
            <div class="bg-slate-800 p-6 rounded-xl border border-slate-700">
                <div class="flex items-center justify-between">
                    <div>
                        <p class="text-slate-400 text-sm">Total Stock</p>
                        <p class="text-3xl font-bold text-blue-500">${DatabaseStore.books.reduce((sum, b) => sum + b.stock, 0)}</p>
                    </div>
                    <i data-lucide="package" class="w-12 h-12 text-blue-500 opacity-20"></i>
                </div>
            </div>
            <div class="bg-slate-800 p-6 rounded-xl border border-slate-700">
                <div class="flex items-center justify-between">
                    <div>
                        <p class="text-slate-400 text-sm">Total Revenue</p>
                        <p class="text-3xl font-bold text-purple-500">$${DatabaseStore.books.reduce((sum, b) => sum + (b.salesCount * b.price), 0).toLocaleString('en-US', {minimumFractionDigits: 2, maximumFractionDigits: 2})}</p>
                        <p class="text-xs text-slate-500 mt-1">${DatabaseStore.books.reduce((sum, b) => sum + b.salesCount, 0).toLocaleString()} books sold</p>
                    </div>
                    <i data-lucide="trending-up" class="w-12 h-12 text-purple-500 opacity-20"></i>
                </div>
            </div>
        </div>
        
        <div class="grid grid-cols-1 lg:grid-cols-2 gap-6">
            <div class="bg-slate-800 rounded-xl border border-slate-700">
                <div class="p-6 border-b border-slate-700">
                    <h2 class="text-xl font-bold text-red-500 flex items-center gap-2">
                        <i data-lucide="alert-triangle" class="w-6 h-6"></i>
                        Low Stock Alert (Min-Heap)
                    </h2>
                </div>
                <div class="p-6 space-y-3">
                    ${lowStockBooks.map(book => `
                        <div class="flex items-center justify-between p-3 bg-slate-700 rounded-lg">
                            <div class="flex-1">
                                <p class="font-medium">${book.title}</p>
                                <p class="text-sm text-slate-400">${book.isbn}</p>
                            </div>
                            <div class="text-right">
                                <p class="text-red-500 font-bold">Only ${book.stock} left</p>
                            </div>
                        </div>
                    `).join('')}
                </div>
            </div>
            
            <div class="bg-slate-800 rounded-xl border border-slate-700">
                <div class="p-6 border-b border-slate-700">
                    <h2 class="text-xl font-bold text-emerald-500 flex items-center gap-2">
                        <i data-lucide="trophy" class="w-6 h-6"></i>
                        Best Sellers (Max-Heap)
                    </h2>
                </div>
                <div class="p-6 space-y-3">
                    ${bestSellerBooks.map(book => `
                        <div class="flex items-center justify-between p-3 bg-slate-700 rounded-lg">
                            <div class="flex-1">
                                <p class="font-medium">${book.title}</p>
                                <p class="text-sm text-slate-400">${book.isbn}</p>
                            </div>
                            <div class="text-right">
                                <p class="text-emerald-500 font-bold">${book.salesCount} sold</p>
                            </div>
                        </div>
                    `).join('')}
                </div>
            </div>
        </div>
    `;
}

function renderAllBooksView(page = 1) {
    const itemsPerPage = 50;
    const totalPages = Math.ceil(DatabaseStore.books.length / itemsPerPage);
    const startIndex = (page - 1) * itemsPerPage;
    const endIndex = startIndex + itemsPerPage;
    const booksToShow = DatabaseStore.books.slice(startIndex, endIndex);
    
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <div class="flex justify-between items-center mb-8">
            <h1 class="text-3xl font-bold text-emerald-500 flex items-center gap-3">
                <i data-lucide="book" class="w-8 h-8"></i>
                All Books (${DatabaseStore.books.length})
            </h1>
            <div class="flex items-center gap-4">
                <span class="text-slate-400">Page ${page} of ${totalPages}</span>
                <div class="flex gap-2">
                    <button onclick="renderAllBooksView(${page - 1})" ${page === 1 ? 'disabled' : ''} class="px-4 py-2 bg-slate-700 hover:bg-slate-600 disabled:opacity-50 disabled:cursor-not-allowed rounded-lg">
                        <i data-lucide="chevron-left" class="w-4 h-4"></i>
                    </button>
                    <button onclick="renderAllBooksView(${page + 1})" ${page === totalPages ? 'disabled' : ''} class="px-4 py-2 bg-slate-700 hover:bg-slate-600 disabled:opacity-50 disabled:cursor-not-allowed rounded-lg">
                        <i data-lucide="chevron-right" class="w-4 h-4"></i>
                    </button>
                </div>
            </div>
        </div>
        
        <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
            ${booksToShow.map(book => `
                <div class="book-card bg-slate-800 rounded-xl border border-slate-700 p-6">
                    <div class="flex justify-between items-start mb-4">
                        <h3 class="text-lg font-bold text-emerald-500">${book.title}</h3>
                        ${DatabaseStore.currentUser?.isAdmin ? `
                            <div class="flex gap-2">
                                <button onclick='showUpdateBookModal("${book.isbn}")' class="text-blue-500 hover:text-blue-400">
                                    <i data-lucide="edit" class="w-4 h-4"></i>
                                </button>
                                <button onclick='deleteBookConfirm("${book.isbn}")' class="text-red-500 hover:text-red-400">
                                    <i data-lucide="trash-2" class="w-4 h-4"></i>
                                </button>
                            </div>
                        ` : ''}
                    </div>
                    <p class="text-sm text-slate-400 mb-2">by ${book.author}</p>
                    <div class="space-y-2 text-sm">
                        <div class="flex justify-between">
                            <span class="text-slate-400">ISBN:</span>
                            <span>${book.isbn}</span>
                        </div>
                        <div class="flex justify-between">
                            <span class="text-slate-400">Category:</span>
                            <span class="text-emerald-500">${book.category}</span>
                        </div>
                        <div class="flex justify-between">
                            <span class="text-slate-400">Price:</span>
                            <span class="text-emerald-500 font-bold">$${book.price.toFixed(2)}</span>
                        </div>
                        <div class="flex justify-between">
                            <span class="text-slate-400">Stock:</span>
                            <span class="${book.stock < 10 ? 'text-red-500' : 'text-green-500'}">${book.stock}</span>
                        </div>
                        <div class="flex justify-between">
                            <span class="text-slate-400">Sales:</span>
                            <span>${book.salesCount}</span>
                        </div>
                    </div>
                </div>
            `).join('')}
        </div>
    `;
    lucide.createIcons();
}

function renderAddBookView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="plus-circle" class="w-8 h-8"></i>
            Add New Book
        </h1>
        
        <form onsubmit="handleAddBook(event)" class="max-w-2xl bg-slate-800 rounded-xl p-8 border border-slate-700">
            <div class="space-y-4">
                <div>
                    <label class="block text-sm font-medium mb-2">ISBN</label>
                    <input type="text" name="isbn" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div>
                    <label class="block text-sm font-medium mb-2">Title</label>
                    <input type="text" name="title" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div>
                    <label class="block text-sm font-medium mb-2">Author</label>
                    <input type="text" name="author" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div>
                    <label class="block text-sm font-medium mb-2">Category</label>
                    <input type="text" name="category" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div>
                    <label class="block text-sm font-medium mb-2">Price ($)</label>
                    <input type="number" step="0.01" name="price" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div>
                    <label class="block text-sm font-medium mb-2">Stock Quantity</label>
                    <input type="number" name="stock" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <button type="submit" class="w-full px-6 py-3 bg-emerald-600 hover:bg-emerald-700 rounded-lg font-medium flex items-center justify-center gap-2">
                    <i data-lucide="plus" class="w-5 h-5"></i>
                    Add Book
                </button>
            </div>
        </form>
    `;
}

function renderSearchView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="search" class="w-8 h-8"></i>
            Search Books
        </h1>
        
        <div class="grid grid-cols-1 md:grid-cols-3 gap-6 mb-8">
            <div class="bg-slate-800 rounded-xl p-6 border border-slate-700">
                <h3 class="text-lg font-bold mb-4 flex items-center gap-2">
                    <i data-lucide="hash" class="w-5 h-5 text-emerald-500"></i>
                    Search by ISBN (O(1))
                </h3>
                <form onsubmit="searchByISBN(event)">
                    <input type="text" name="isbn" placeholder="Enter ISBN..." class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg mb-3">
                    <button type="submit" class="w-full px-4 py-2 bg-emerald-600 hover:bg-emerald-700 rounded-lg">Search</button>
                </form>
            </div>
            
            <div class="bg-slate-800 rounded-xl p-6 border border-slate-700">
                <h3 class="text-lg font-bold mb-4 flex items-center gap-2">
                    <i data-lucide="folder" class="w-5 h-5 text-blue-500"></i>
                    Search by Category (B-Tree)
                </h3>
                <form onsubmit="searchByCategory(event)">
                    <input type="text" name="category" placeholder="Enter Category..." class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg mb-3">
                    <button type="submit" class="w-full px-4 py-2 bg-blue-600 hover:bg-blue-700 rounded-lg">Search</button>
                </form>
            </div>
            
            <div class="bg-slate-800 rounded-xl p-6 border border-slate-700">
                <h3 class="text-lg font-bold mb-4 flex items-center gap-2">
                    <i data-lucide="dollar-sign" class="w-5 h-5 text-purple-500"></i>
                    Search by Price (AVL Tree)
                </h3>
                <form onsubmit="searchByPrice(event)">
                    <input type="number" step="0.01" name="minPrice" placeholder="Min Price" class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg mb-2">
                    <input type="number" step="0.01" name="maxPrice" placeholder="Max Price" class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg mb-3">
                    <button type="submit" class="w-full px-4 py-2 bg-purple-600 hover:bg-purple-700 rounded-lg">Search</button>
                </form>
            </div>
        </div>
        
        <div id="searchResults"></div>
    `;
}

function renderProcessSaleView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="shopping-cart" class="w-8 h-8"></i>
            Process Sale
        </h1>
        
        <form onsubmit="handleProcessSale(event)" class="max-w-2xl bg-slate-800 rounded-xl p-8 border border-slate-700">
            <div class="space-y-4">
                <div>
                    <label class="block text-sm font-medium mb-2">ISBN</label>
                    <input type="text" name="isbn" id="saleISBN" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div>
                    <label class="block text-sm font-medium mb-2">Quantity</label>
                    <input type="number" name="quantity" min="1" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div id="saleBookInfo" class="hidden p-4 bg-slate-700 rounded-lg"></div>
                <button type="submit" class="w-full px-6 py-3 bg-emerald-600 hover:bg-emerald-700 rounded-lg font-medium flex items-center justify-center gap-2">
                    <i data-lucide="check-circle" class="w-5 h-5"></i>
                    Process Sale
                </button>
            </div>
        </form>
    `;
    
    document.getElementById('saleISBN').addEventListener('input', async (e) => {
        const book = await DatabaseStore.searchByISBN(e.target.value);
        const infoDiv = document.getElementById('saleBookInfo');
        if (book) {
            infoDiv.classList.remove('hidden');
            infoDiv.innerHTML = `
                <p class="font-bold mb-2">${book.title}</p>
                <p class="text-sm text-slate-400">Author: ${book.author}</p>
                <p class="text-sm text-slate-400">Price: $${book.price.toFixed(2)}</p>
                <p class="text-sm ${book.stock < 10 ? 'text-red-500' : 'text-green-500'}">Current Stock: ${book.stock}</p>
            `;
        } else {
            infoDiv.classList.add('hidden');
        }
    });
}

function renderTakeBookView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="book-plus" class="w-8 h-8"></i>
            Take a Book
        </h1>
        
        <form onsubmit="handleTakeBook(event)" class="max-w-2xl bg-slate-800 rounded-xl p-8 border border-slate-700">
            <div class="space-y-4">
                <div>
                    <label class="block text-sm font-medium mb-2">ISBN</label>
                    <input type="text" name="isbn" id="takeISBN" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
                </div>
                <div id="takeBookInfo" class="hidden p-4 bg-slate-700 rounded-lg"></div>
                <button type="submit" class="w-full px-6 py-3 bg-emerald-600 hover:bg-emerald-700 rounded-lg font-medium flex items-center justify-center gap-2">
                    <i data-lucide="check-circle" class="w-5 h-5"></i>
                    Take Book
                </button>
            </div>
        </form>
    `;
    
    document.getElementById('takeISBN').addEventListener('input', async (e) => {
        const book = await DatabaseStore.searchByISBN(e.target.value);
        const infoDiv = document.getElementById('takeBookInfo');
        if (book) {
            infoDiv.classList.remove('hidden');
            infoDiv.innerHTML = `
                <p class="font-bold mb-2">${book.title}</p>
                <p class="text-sm text-slate-400">Author: ${book.author}</p>
                <p class="text-sm text-slate-400">Price: $${book.price.toFixed(2)}</p>
                <p class="text-sm ${book.stock < 1 ? 'text-red-500' : 'text-green-500'}">Available: ${book.stock}</p>
            `;
        } else {
            infoDiv.classList.add('hidden');
        }
    });
}

async function renderDropBookView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="book-minus" class="w-8 h-8"></i>
            Drop a Book
        </h1>
        <div class="flex justify-center py-8">
            <div class="animate-spin rounded-full h-12 w-12 border-b-2 border-emerald-500"></div>
        </div>
    `;
    
    const myBookPromises = DatabaseStore.currentUser.myBooks.map(isbn => DatabaseStore.searchByISBN(isbn));
    const myBooks = await Promise.all(myBookPromises);
    const validBooks = myBooks.filter(book => book !== null);
    
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="book-minus" class="w-8 h-8"></i>
            Drop a Book
        </h1>
        
        ${validBooks.length === 0 ? `
            <div class="text-center py-12 text-slate-400">
                <i data-lucide="inbox" class="w-16 h-16 mx-auto mb-4 opacity-50"></i>
                <p>You have no books to return</p>
            </div>
        ` : `
            <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                ${validBooks.map(book => `
                    <div class="bg-slate-800 rounded-xl p-6 border border-slate-700">
                        <h3 class="text-lg font-bold text-emerald-500 mb-2">${book.title}</h3>
                        <p class="text-sm text-slate-400 mb-4">by ${book.author}</p>
                        <div class="text-sm text-slate-400 mb-4">
                            <p>ISBN: ${book.isbn}</p>
                            <p>Category: ${book.category}</p>
                        </div>
                        <button onclick='handleDropBook("${book.isbn}")' class="w-full px-4 py-2 bg-red-600 hover:bg-red-700 rounded-lg flex items-center justify-center gap-2">
                            <i data-lucide="x-circle" class="w-4 h-4"></i>
                            Return Book
                        </button>
                    </div>
                `).join('')}
            </div>
        `}
    `;
    lucide.createIcons();
}

async function renderProfileView() {
    const content = document.getElementById('mainContent');
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="user-circle" class="w-8 h-8"></i>
            My Profile
        </h1>
        <div class="flex justify-center py-8">
            <div class="animate-spin rounded-full h-12 w-12 border-b-2 border-emerald-500"></div>
        </div>
    `;
    
    const myBookPromises = DatabaseStore.currentUser.myBooks.map(isbn => DatabaseStore.searchByISBN(isbn));
    const myBooks = await Promise.all(myBookPromises);
    const validBooks = myBooks.filter(book => book !== null);
    
    content.innerHTML = `
        <h1 class="text-3xl font-bold text-emerald-500 mb-8 flex items-center gap-3">
            <i data-lucide="user-circle" class="w-8 h-8"></i>
            My Profile
        </h1>
        
        <div class="max-w-2xl">
            <div class="bg-slate-800 rounded-xl p-8 border border-slate-700 mb-6">
                <div class="flex items-center gap-4 mb-6">
                    <div class="w-20 h-20 bg-emerald-600 rounded-full flex items-center justify-center">
                        <i data-lucide="user" class="w-10 h-10"></i>
                    </div>
                    <div>
                        <h2 class="text-2xl font-bold">${DatabaseStore.currentUser.username}</h2>
                        <p class="text-emerald-500">${DatabaseStore.currentUser.isAdmin ? 'Administrator' : 'User'}</p>
                    </div>
                </div>
                
                <div class="border-t border-slate-700 pt-6">
                    <h3 class="text-lg font-semibold mb-4">Books I Have (${validBooks.length})</h3>
                    ${validBooks.length === 0 ? `
                        <p class="text-slate-400 text-sm">No books currently borrowed</p>
                    ` : `
                        <div class="space-y-3">
                            ${validBooks.map(book => `
                                <div class="bg-slate-700 p-4 rounded-lg">
                                    <p class="font-medium">${book.title}</p>
                                    <p class="text-sm text-slate-400">by ${book.author}</p>
                                    <p class="text-xs text-slate-500 mt-1">ISBN: ${book.isbn}</p>
                                </div>
                            `).join('')}
                        </div>
                    `}
                </div>
            </div>
        </div>
    `;
    lucide.createIcons();
}

function showLoginModal() {
    showModal('User Login', `
        <form onsubmit="handleLogin(event)" class="space-y-4">
            <div>
                <label class="block text-sm font-medium mb-2">Username</label>
                <input type="text" name="username" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <div>
                <label class="block text-sm font-medium mb-2">Password</label>
                <input type="password" name="password" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <button type="submit" class="w-full px-6 py-3 bg-emerald-600 hover:bg-emerald-700 rounded-lg font-medium">
                Login
            </button>
        </form>
    `, []);
}

function showRegisterModal() {
    showModal('User Registration', `
        <form onsubmit="handleRegister(event)" class="space-y-4">
            <div>
                <label class="block text-sm font-medium mb-2">Username</label>
                <input type="text" name="username" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <div>
                <label class="block text-sm font-medium mb-2">Password</label>
                <input type="password" name="password" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <button type="submit" class="w-full px-6 py-3 bg-emerald-600 hover:bg-emerald-700 rounded-lg font-medium">
                Register
            </button>
        </form>
    `, []);
}

async function handleLogin(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const result = await DatabaseStore.login(formData.get('username'), formData.get('password'));
    
    if (result.success) {
        showToast('[SUCCESS] Login successful!', 'success');
        closeModal();
        renderSidebar();
        renderView('dashboard');
    } else {
        showToast('[ERROR] ' + result.message, 'error');
    }
}

async function handleRegister(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const result = await DatabaseStore.register(formData.get('username'), formData.get('password'));
    
    if (result.success) {
        showToast('[SUCCESS] ' + result.message, 'success');
        closeModal();
    } else {
        showToast('[ERROR] ' + result.message, 'error');
    }
}

function continueAsGuest() {
    showToast('[INFO] Continuing as Guest (limited access)', 'info');
    renderSidebar();
    renderView('search');
}

async function handleAddBook(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const result = await DatabaseStore.addBook(
        formData.get('isbn'),
        formData.get('title'),
        formData.get('author'),
        formData.get('category'),
        parseFloat(formData.get('price')),
        parseInt(formData.get('stock'))
    );
    
    if (result.success) {
        showToast('[SUCCESS] ' + result.message, 'success');
        event.target.reset();
        renderView('allBooks');
    } else {
        showToast('[ERROR] ' + result.message, 'error');
    }
}

async function showUpdateBookModal(isbn) {
    const book = await DatabaseStore.searchByISBN(isbn);
    if (!book) {
        showToast('[ERROR] Book not found', 'error');
        return;
    }
    showModal('Update Book Details', `
        <form onsubmit="handleUpdateBook(event, '${book.isbn}')" class="space-y-4">
            <div>
                <label class="block text-sm font-medium mb-2">Title</label>
                <input type="text" name="title" value="${book.title}" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <div>
                <label class="block text-sm font-medium mb-2">Author</label>
                <input type="text" name="author" value="${book.author}" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <div>
                <label class="block text-sm font-medium mb-2">Category</label>
                <input type="text" name="category" value="${book.category}" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <div>
                <label class="block text-sm font-medium mb-2">Price ($)</label>
                <input type="number" step="0.01" name="price" value="${book.price}" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <div>
                <label class="block text-sm font-medium mb-2">Stock</label>
                <input type="number" name="stock" value="${book.stock}" required class="w-full px-4 py-2 bg-slate-700 border border-slate-600 rounded-lg">
            </div>
            <button type="submit" class="w-full px-6 py-3 bg-emerald-600 hover:bg-emerald-700 rounded-lg font-medium">
                Update Book
            </button>
        </form>
    `, []);
}

async function handleUpdateBook(event, isbn) {
    event.preventDefault();
    const formData = new FormData(event.target);
    
    // Update book details (title, author, category, price)
    const result = await DatabaseStore.updateBook(
        isbn,
        formData.get('title'),
        formData.get('author'),
        formData.get('category'),
        parseFloat(formData.get('price'))
    );
    
    console.log('Book update result:', result);
    
    // Update stock separately (different endpoint)
    const stockResult = await DatabaseStore.updateStock(isbn, parseInt(formData.get('stock')));
    
    console.log('Stock update result:', stockResult);
    
    if (result.success && stockResult.success) {
        showToast('[SUCCESS] Book updated successfully!', 'success');
        closeModal();
        renderView('allBooks');
    } else {
        const errorMsg = !result.success ? 'Book update failed' : 'Stock update failed';
        console.error(errorMsg, { result, stockResult });
        showToast(`[ERROR] ${errorMsg}`, 'error');
    }
}

async function deleteBookConfirm(isbn) {
    const book = await DatabaseStore.searchByISBN(isbn);
    if (!book) {
        showToast('[ERROR] Book not found', 'error');
        return;
    }
    showModal('Delete Book', `
        <div class="text-center">
            <i data-lucide="alert-triangle" class="w-16 h-16 text-red-500 mx-auto mb-4"></i>
            <p class="text-lg mb-2">Are you sure you want to delete this book?</p>
            <p class="text-slate-400">${book.title}</p>
            <p class="text-sm text-slate-500">ISBN: ${isbn}</p>
        </div>
    `, [
        { label: 'Cancel', onclick: 'closeModal()', class: 'px-6 py-2 bg-slate-700 hover:bg-slate-600 rounded-lg' },
        { label: 'Delete', onclick: `deleteBookExecute('${isbn}')`, class: 'px-6 py-2 bg-red-600 hover:bg-red-700 rounded-lg' }
    ]);
    lucide.createIcons();
}

async function deleteBookExecute(isbn) {
    const result = await DatabaseStore.deleteBook(isbn);
    if (result.success) {
        showToast('[SUCCESS] ' + result.message, 'success');
        closeModal();
        renderView('allBooks');
    } else {
        showToast('[ERROR] ' + result.message, 'error');
    }
}

async function handleProcessSale(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const isbn = formData.get('isbn');
    const quantity = parseInt(formData.get('quantity'));
    
    // Get book details first to calculate total
    const book = await DatabaseStore.searchByISBN(isbn);
    if (!book) {
        showToast('[ERROR] Book not found', 'error');
        return;
    }
    
    const result = await DatabaseStore.processSale(isbn, quantity);
    
    if (result.success) {
        const total = book.price * quantity;
        showToast(`[SUCCESS] Sale processed! Total: $${total.toFixed(2)}`, 'success');
        event.target.reset();
        document.getElementById('saleBookInfo').classList.add('hidden');
    } else {
        showToast('[ERROR] ' + result.message, 'error');
    }
}

async function handleTakeBook(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const result = await DatabaseStore.takeBook(formData.get('isbn'));
    
    if (result.success) {
        showToast('[SUCCESS] ' + result.message, 'success');
        event.target.reset();
        document.getElementById('takeBookInfo').classList.add('hidden');
    } else {
        showToast('[ERROR] ' + result.message, 'error');
    }
}

async function handleDropBook(isbn) {
    const result = await DatabaseStore.dropBook(isbn);
    if (result.success) {
        showToast('[SUCCESS] ' + result.message, 'success');
        renderView('dropBook');
    } else {
        showToast('[ERROR] ' + result.message, 'error');
    }
}

async function searchByISBN(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const resultsDiv = document.getElementById('searchResults');
    resultsDiv.innerHTML = '<div class="text-center py-8 text-emerald-500">Searching...</div>';
    
    const book = await DatabaseStore.searchByISBN(formData.get('isbn'));
    
    if (book) {
        resultsDiv.innerHTML = `
            <div class="bg-slate-800 rounded-xl p-6 border border-emerald-500">
                <h3 class="text-2xl font-bold text-emerald-500 mb-4">[FOUND] Book Details</h3>
                <div class="space-y-2">
                    <p><span class="text-slate-400">ISBN:</span> ${book.isbn}</p>
                    <p><span class="text-slate-400">Title:</span> ${book.title}</p>
                    <p><span class="text-slate-400">Author:</span> ${book.author}</p>
                    <p><span class="text-slate-400">Category:</span> ${book.category}</p>
                    <p><span class="text-slate-400">Price:</span> $${book.price.toFixed(2)}</p>
                    <p><span class="text-slate-400">Stock:</span> ${book.stock}</p>
                    <p><span class="text-slate-400">Sales:</span> ${book.salesCount}</p>
                </div>
            </div>
        `;
        showToast('[FOUND] Book found!', 'success');
    } else {
        resultsDiv.innerHTML = `<div class="text-center py-8 text-red-500">[ERROR] Book not found!</div>`;
        showToast('[ERROR] Book not found!', 'error');
    }
}

async function searchByCategory(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const resultsDiv = document.getElementById('searchResults');
    resultsDiv.innerHTML = '<div class="text-center py-8 text-blue-500">Searching...</div>';
    
    const results = await DatabaseStore.searchByCategory(formData.get('category'));
    
    if (results.length > 0) {
        resultsDiv.innerHTML = `
            <h3 class="text-2xl font-bold text-blue-500 mb-4">[FOUND] ${results.length} book(s) in category "${formData.get('category')}"</h3>
            <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                ${results.map(book => `
                    <div class="bg-slate-800 rounded-xl p-6 border border-blue-500">
                        <h4 class="text-lg font-bold text-blue-500 mb-2">${book.title}</h4>
                        <p class="text-sm text-slate-400 mb-4">by ${book.author}</p>
                        <div class="text-sm space-y-1">
                            <p>ISBN: ${book.isbn}</p>
                            <p>Price: $${book.price.toFixed(2)}</p>
                            <p>Stock: ${book.stock}</p>
                        </div>
                    </div>
                `).join('')}
            </div>
        `;
        showToast(`[FOUND] ${results.length} books found`, 'success');
    } else {
        resultsDiv.innerHTML = `<div class="text-center py-8 text-slate-400">[INFO] No books found in this category</div>`;
        showToast('[INFO] No books found', 'info');
    }
}

async function searchByPrice(event) {
    event.preventDefault();
    const formData = new FormData(event.target);
    const resultsDiv = document.getElementById('searchResults');
    resultsDiv.innerHTML = '<div class="text-center py-8 text-purple-500">Searching...</div>';
    
    const results = await DatabaseStore.searchByPrice(
        parseFloat(formData.get('minPrice')),
        parseFloat(formData.get('maxPrice'))
    );
    
    if (results.length > 0) {
        resultsDiv.innerHTML = `
            <h3 class="text-2xl font-bold text-purple-500 mb-4">[FOUND] ${results.length} book(s) in price range</h3>
            <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
                ${results.map(book => `
                    <div class="bg-slate-800 rounded-xl p-6 border border-purple-500">
                        <h4 class="text-lg font-bold text-purple-500 mb-2">${book.title}</h4>
                        <p class="text-sm text-slate-400 mb-4">by ${book.author}</p>
                        <div class="text-sm space-y-1">
                            <p>ISBN: ${book.isbn}</p>
                            <p class="text-purple-500 font-bold">Price: $${book.price.toFixed(2)}</p>
                            <p>Stock: ${book.stock}</p>
                        </div>
                    </div>
                `).join('')}
            </div>
        `;
        showToast(`[FOUND] ${results.length} books found`, 'success');
    } else {
        resultsDiv.innerHTML = `<div class="text-center py-8 text-slate-400">[INFO] No books found in this price range</div>`;
        showToast('[INFO] No books found', 'info');
    }
}

document.addEventListener('DOMContentLoaded', async () => {
    DatabaseStore.loadFromLocalStorage();
    
    await DatabaseStore.loadBooksFromBackend();
    
    document.getElementById('logoutBtn').addEventListener('click', () => {
        DatabaseStore.logout();
        showToast('[INFO] Logged out successfully!', 'info');
        renderSidebar();
        renderView('auth');
    });
    
    // If user is already logged in from previous session, go to dashboard
    if (DatabaseStore.currentUser) {
        renderView('dashboard');
    } else {
        renderView('auth');
    }
    renderSidebar();
    lucide.createIcons();
});