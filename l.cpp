#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace std;

class UserInterface {
public:
    virtual void create() = 0;
    virtual bool login() = 0;
    virtual void addBalance(double amount) = 0;
    virtual double getBalance() const = 0;
    virtual bool accountCreated() const = 0;
};

class User : public UserInterface {
    string username;
    string password;
    long long int phone;
    string email;
    double balance;
    bool isCreated;

    void saveUserData() const {
        ofstream outFile(username + "_data.txt");
        outFile << username << endl;
        outFile << password << endl;
        outFile << phone << endl;
        outFile << email << endl;
        outFile << balance << endl;
        outFile.close();
    }

    bool loadUserData(const string& user) {
        ifstream inFile(user + "_data.txt");
        if (!inFile) return false;
        inFile >> username >> password >> phone >> email >> balance;
        inFile.close();
        return true;
    }

public:
    User() : balance(10000), isCreated(false) {}

    void create() override {
        cout << "Enter Your Name: ";
        cin >> username;
        while (true) {
            cout << "Enter Phone Number: ";
            cin >> phone;
            if (to_string(phone).length() == 10) {
                break;
            } else {
                cout << "Invalid phone number! Please enter a 10-digit number." << endl;
            }
        }
        while (true) {
            cout << "Enter Your E-Mail: ";
            cin >> email;
            size_t at_pos = email.find("@");
            size_t dot_pos = email.find(".", at_pos);
            if (at_pos != string::npos && dot_pos != string::npos && dot_pos > at_pos) {
                break;
            } else {
                cout << "Invalid email address! Please enter a valid email." << endl;
            }
        }
        cout << "Enter Password: ";
        cin >> password;
        isCreated = true;
        saveUserData();  
    }

    bool login() override {
        string usernameInput, passwordInput;
        cout << "Enter the UserName: ";
        cin >> usernameInput;

        if (!loadUserData(usernameInput)) {
            cout << "User not found! Please sign up." << endl;
            return false;
        }

        cout << "Enter the Password: ";
        cin >> passwordInput;

        if (password == passwordInput) {
            cout << "Login successful!" << endl;
            return true;
        } else {
            cout << "Invalid Username or Password!" << endl;
            return false;
        }
    }

    bool accountCreated() const override {
        return isCreated;
    }

    void addBalance(double amount) override {
        balance += amount;
        saveUserData();  
    }

    double getBalance() const override {
        return balance;
    }

    bool deductBalance(double amount) {
        if (balance >= amount) {
            balance -= amount;
            saveUserData(); 
            return true;
        } else {
            return false;
        }
    }

    string getUsername() const {
        return username;
    }
    string getEmail() const {
        return email;
    }
    long long int getPhone() const {
        return phone;
    }

    void logout() {
        saveUserData(); 
    }
};

class Item {
public:
    string name;
    string size;
    int quantity;
    double price;

    Item(string n, string s, int q, double p) : name(n), size(s), quantity(q), price(p) {}

    double getTotalPrice() const {
        return price * quantity;
    }
};

class Cart {
    vector<Item> cartItems;
    vector<Item> orderedItems;

    void saveCartData(const string& username) const {
        ofstream outCart(username + "_cart.txt");
        for (const auto& item : cartItems) {
            outCart << item.name << " " << item.size << " " << item.quantity << " " << item.price << endl;
        }
        outCart.close();

        ofstream outOrdered(username + "_ordered.txt");
        for (const auto& item : orderedItems) {
            outOrdered << item.name << " " << item.size << " " << item.quantity << " " << item.price << endl;
        }
        outOrdered.close();
    }

    void loadCartData(const string& username) {
        ifstream inCart(username + "_cart.txt");
        string name, size;
        int quantity;
        double price;
        while (inCart >> name >> size >> quantity >> price) {
            cartItems.emplace_back(name, size, quantity, price);
        }
        inCart.close();

        ifstream inOrdered(username + "_ordered.txt");
        while (inOrdered >> name >> size >> quantity >> price) {
            orderedItems.emplace_back(name, size, quantity, price);
        }
        inOrdered.close();
    }

public:
    void addItemToCart(Item item) {
        cartItems.push_back(item);
    }

    void addItemToOrdered(Item item) {
        orderedItems.push_back(item);
    }

    void displayCart() const {
        cout << left << setw(30) << "Item" << setw(10) << "Size" << setw(10) << "Quantity" << setw(15) << "Price (Each)" << setw(15) << "Total Price" << endl;
        cout << "-------------------------------------------------------------------------" << endl;
        for (const auto& item : cartItems) {
            cout << left << setw(30) << item.name << setw(10) << item.size << setw(10) << item.quantity
                 << setw(15) << item.price << setw(15) << item.getTotalPrice() << endl;
        }
    }

    void displayOrderedItems() const {
        cout << left << setw(30) << "Item" << setw(10) << "Size" << setw(10) << "Quantity" << setw(15) << "Price (Each)" << setw(15) << "Total Price" << endl;
        cout << "--------------------------------------------------------------------------" << endl;
        double totalMoney = 0;
        for (const auto& item : orderedItems) {
            cout << left << setw(30) << item.name << setw(10) << item.size << setw(10) << item.quantity
                 << setw(15) << item.price << setw(15) << item.getTotalPrice() << endl;
            totalMoney += item.getTotalPrice();
        }
        cout << "Total Money of All Items: " << totalMoney << endl;
    }

    bool purchaseFromCart(const string& itemName, User& user) {
        for (auto it = cartItems.begin(); it != cartItems.end(); ++it) {
            if (it->name == itemName) {
                double totalPrice = it->getTotalPrice();
                if (user.deductBalance(totalPrice)) {
                    orderedItems.push_back(*it);  
                    cartItems.erase(it);          
                    saveCartData(user.getUsername()); 
                    cout << "Item " << itemName << " has been purchased!" << endl;
                } else {
                    cout << "Balance not sufficient to purchase " << itemName << "!" << endl;
                }
                return true;
            }
        }
        cout << "Item " << itemName << " not found in cart!" << endl;
        return false;
    }

    void logout(const string& username) {
        saveCartData(username);  
    }

    void login(const string& username) {
        loadCartData(username); 
    }
};

int main() {
    int choice = 0, choice1 = 0;
    User u;
    Cart cart;

    while (true) {
        cout << "1. Login" << endl;
        cout << "2. Sign Up" << endl;
        cout << "3. Exit" << endl;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: {
            if (u.login()) {
                cart.login(u.getUsername());  
                while (true) {
                    cout << "1. Add Item to Cart" << endl;
                    cout << "2. View Cart" << endl;
                    cout << "3. Purchase Item" << endl;
                    cout << "4. View Ordered List" << endl;
                    cout << "5. View Balance" << endl;
                    cout << "6. Add Balance" << endl;
                    cout << "7. Logout" << endl;
                    cin >> choice1;
                    cin.ignore();

                    switch (choice1) {
                    case 1: {
                        string name, size;
                        int quantity;
                        double price;
                        cout << "Enter Item Name: ";
                        getline(cin, name);
                        cout << "Enter Item Size: ";
                        getline(cin, size);
                        cout << "Enter Quantity: ";
                        cin >> quantity;
                        cout << "Enter Price of Each Item: ";
                        cin >> price;

                        cart.addItemToCart(Item(name, size, quantity, price));
                        break;
                    }
                    case 2:
                        cart.displayCart();
                        break;
                    case 3: {
                        string itemName;
                        cout << "Enter the Item Name to Purchase: ";
                        getline(cin, itemName);
                        cart.purchaseFromCart(itemName, u);
                        break;
                    }
                    case 4:
                        cart.displayOrderedItems();
                        break;
                    case 5:
                        cout << "Current Balance: " << u.getBalance() << endl;
                        break;
                    case 6: {
                        double amount;
                        cout << "Enter amount to add to balance: ";
                        cin >> amount;
                        u.addBalance(amount);
                        cout << "Balance added successfully!" << endl;
                        break;
                    }
                    case 7:
                        cart.logout(u.getUsername()); 
                        u.logout();
                        cout << "Logged out successfully!" << endl;
                        return 0;
                    default:
                        cout << "Invalid choice!" << endl;
                        break;
                    }
                }
            }
            break;
        }
        case 2:
            u.create();
            break;
        case 3:
            cout << "Exiting..." << endl;
            return 0;
        default:
            cout << "Invalid choice!!!" << endl;
        }
    }
    return 0;
}