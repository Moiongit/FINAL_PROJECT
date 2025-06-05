#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

// ==============================
// Strategy Pattern for Bill Calculation (Polymorphism + Strategy Pattern)
// ==============================
class BillStrategy {
public:
    virtual double calculateBill(int nights, double rate) = 0; // Pure virtual (abstract method)
    virtual ~BillStrategy() {}
};

class StandardBill : public BillStrategy {
public:
    double calculateBill(int nights, double rate) override {
        return nights * rate;
    }
};

class DiscountBill : public BillStrategy {
public:
    double calculateBill(int nights, double rate) override {
        if (nights >= 5)
            return nights * rate * 0.9; // 10% discount for 5 or more nights
        return nights * rate;
    }
};

// ==============================
// Room class (Encapsulation)
// ==============================
class Room {
private:
    int roomNumber;
    bool available;
    double rate;

public:
    Room(int num, double r) : roomNumber(num), available(true), rate(r) {}

    // Getters - Encapsulation: controlled access to private members
    int getRoomNumber() const { return roomNumber; }
    bool isAvailable() const { return available; }
    double getRate() const { return rate; }

    // Setters and State Modifiers - Encapsulation
    void book() { available = false; }
    void release() { available = true; }
    void setRate(double r) { rate = r; }
};

// ==============================
// Base User class (Inheritance + Polymorphism + Encapsulation)
// ==============================
class User {
protected:
    string username, password;

public:
    User(string u, string p) : username(u), password(p) {}

    virtual void menu() = 0;  // Pure virtual - Abstract class, forces subclasses to implement

    string getUsername() const { return username; }
    bool checkPassword(const string& p) const { return password == p; }
    virtual ~User() {}
};

class Customer : public User {
public:
    Customer(string u, string p) : User(u, p) {}

    void menu() override {
        cout << "Welcome, " << username << "! (Customer)\n";
    }
};

class Admin : public User {
public:
    Admin(string u, string p) : User(u, p) {}

    void menu() override {
        cout << "Welcome, " << username << "! (Admin)\n";
    }
};

// ==============================
// Reservation class (Encapsulation)
// ==============================
class Reservation {
private:
    string customerName;
    int roomNumber;
    int nights;
    double totalBill;

public:
    Reservation(string name, int room, int n, double bill)
        : customerName(name), roomNumber(room), nights(n), totalBill(bill) {}

    void show() const {
        cout << "Customer: " << customerName
             << ", Room: " << roomNumber
             << ", Nights: " << nights
             << ", Bill: $" << totalBill << endl;
    }

    int getRoomNumber() const { return roomNumber; }
    string getCustomerName() const { return customerName; }
};

// ==============================
// HotelSystem class: System Controller (Aggregation, Exception Handling, Encapsulation)
// ==============================
class HotelSystem {
private:
    vector<Room> rooms;                  // Aggregation of Room objects
    vector<User*> users;                 // Aggregation of Users (Admin and Customer)
    vector<Reservation> reservations;   // Aggregation of Reservations
    BillStrategy* billStrategy;          // Strategy Pattern for billing
    User* currentUser = nullptr;         // Pointer to currently logged-in user

    // Utility function for UI separation
    void printSeparator() {
        cout << "------------------------------" << endl;
    }

    // Find room by number (Encapsulation, Helper function)
    Room* findRoom(int num) {
        for (auto& room : rooms)
            if (room.getRoomNumber() == num)
                return &room;
        return nullptr;
    }

    // Input validation with exception handling
    int getInt(const string& prompt) {
        int val;
        while (true) {
            try {
                cout << prompt;
                if (!(cin >> val)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    throw runtime_error("Invalid input. Please enter an integer.");
                }
                cin.ignore(1000, '\n');
                return val;
            } catch (const runtime_error& e) {
                cout << e.what() << endl;
            }
        }
    }

    // Input validation for double values (non-negative)
    double getDouble(const string& prompt) {
        double val;
        while (true) {
            try {
                cout << prompt;
                if (!(cin >> val) || val < 0) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    throw runtime_error("Invalid input. Please enter a non-negative number.");
                }
                cin.ignore(1000, '\n');
                return val;
            } catch (const runtime_error& e) {
                cout << e.what() << endl;
            }
        }
    }

public:
    // Constructor initializes system with default rooms and admin user
    HotelSystem() {
        rooms.emplace_back(101, 100.0);
        rooms.emplace_back(102, 120.0);
        rooms.emplace_back(103, 150.0);
        users.push_back(new Admin("admin", "admin123"));
        billStrategy = new StandardBill(); // Default billing strategy
    }

    // Destructor to free dynamically allocated memory (Rule of three)
    ~HotelSystem() {
        delete billStrategy;
        for (auto user : users)
            delete user;
    }

    // Main menu, entry point for user roles
    void mainMenu() {
        int choice;
        do {
            printSeparator();
            cout << "Welcome to Hotel Reservation System\n";
            printSeparator();
            cout << "\nMain Menu\n";
            cout << "\nSelect Role:\n";
            cout << "1. Admin\n2. Customer\n0. Exit System\nChoice: ";
            choice = getInt("");

            if (choice == 1) {
                if (adminLogin()) userMenu();
            } else if (choice == 2) {
                customerSubmenu();
            }
        } while (choice != 0);
    }

    // Admin login function (User Authentication)
    bool adminLogin() {
        string uname, pass;
        cout << "[Admin Login]\nUsername: ";
        getline(cin, uname);
        cout << "Password: ";
        getline(cin, pass);

        for (auto user : users) {
            if (dynamic_cast<Admin*>(user) &&
                user->getUsername() == uname && user->checkPassword(pass)) {
                currentUser = user;
                cout << "Login successful!\nRedirecting to menu...\n";
                currentUser->menu();
                return true;
            }
        }
        cout << "Invalid Admin credentials.\n";
        return false;
    }

    // Customer submenu for signup/login
    void customerSubmenu() {
        int choice;
        do {
            cout << "\nCustomer Options:\n1. Signup\n2. Login\n0. Back\nChoice: ";
            choice = getInt("");
            if (choice == 1) signup();
            else if (choice == 2) {
                if (customerLogin()) userMenu();
            }
        } while (choice != 0);
    }

    // Customer login function (User Authentication)
    bool customerLogin() {
        string uname, pass;
        cout << "[Customer Login]\nUsername: ";
        getline(cin, uname);
        cout << "Password: ";
        getline(cin, pass);

        for (auto user : users) {
            if (dynamic_cast<Customer*>(user) &&
                user->getUsername() == uname && user->checkPassword(pass)) {
                currentUser = user;
                cout << "Login successful!\nRedirecting to menu...\n";
                currentUser->menu();
                return true;
            }
        }
        cout << "Invalid Customer credentials.\n";
        return false;
    }

    // Customer signup (Encapsulation + Data validation)
    void signup() {
        string uname, pass;
        cout << "Enter new username: ";
        getline(cin, uname);
        cout << "Enter new password: ";
        getline(cin, pass);

        for (const auto& user : users) {
            if (user->getUsername() == uname) {
                cout << "Username already exists!\n";
                return;
            }
        }
        users.push_back(new Customer(uname, pass));
        cout << "Signup successful! Please login.\n";
    }

    // User menu dispatch based on dynamic type (Polymorphism)
    void userMenu() {
        if (dynamic_cast<Admin*>(currentUser)) {
            adminMenu();
        } else {
            customerMenu();
        }
    }

    // ============================
    // Admin menu options
    // ============================
    void adminMenu() {
        int choice;
        do {
            cout << "\nAdmin Menu:\n";
            cout << "1. Show All Room Reservations\n";
            cout << "2. Show All Rooms\n";
            cout << "3. Change Bill Strategy\n";
            cout << "4. Add New Room\n";
            cout << "5. Edit Room Rate\n";
            cout << "6. Edit Room Availability\n";
            cout << "7. Cancel a Reservation\n";
            cout << "8. Show Available Rooms Only\n";
            cout << "0. Logout\n";
            choice = getInt("Choice: ");
            if (choice == 1) showReservations();
            else if (choice == 2) showRooms();
            else if (choice == 3) changeBillStrategy();
            else if (choice == 4) addRoom();
            else if (choice == 5) editRoomRate();
            else if (choice == 6) editRoomAvailability();
            else if (choice == 7) cancelReservation();
            else if (choice == 8) showAvailableRooms();
        } while (choice != 0);
    }

    // ============================
    // Customer menu options
    // ============================
    void customerMenu() {
        int choice;
        do {
            cout << "\nCustomer Menu:\n";
            cout << "1. Book Room\n";
            cout << "2. My Room Reservations\n";
            cout << "3. Cancel My Reservation\n";
            cout << "4. Show Available Rooms Only\n";
            cout << "0. Logout\n";
            choice = getInt("Choice: ");
            if (choice == 1) bookRoom();
            else if (choice == 2) myReservations();
            else if (choice == 3) cancelReservation();
            else if (choice == 4) showAvailableRooms();
        } while (choice != 0);
    }

    // Show all rooms with status (Information Display)
    void showRooms() {
        cout << "\nRooms:\n";
        cout << "Room #  |  Rate ($)  |  Status\n";
        cout << "-------------------------------\n";
        for (const auto& room : rooms) {
            cout << room.getRoomNumber() << "      |  " << room.getRate() << "       |  "
                 << (room.isAvailable() ? "Available" : "Booked") << endl;
        }
    }

    // Show only available rooms (Information Display)
    void showAvailableRooms() {
        cout << "\nAvailable Rooms:\n";
        bool anyAvailable = false;
        for (const auto& room : rooms) {
            if (room.isAvailable()) {
                cout << "Room " << room.getRoomNumber()
                     << " | Rate: $" << room.getRate() << endl;
                anyAvailable = true;
            }
        }
        if (!anyAvailable) {
            cout << "No rooms available at the moment.\n";
        }
    }

    // Book room for current customer (Business logic + Encapsulation + Error handling)
    void bookRoom() {
        try {
            showRooms();
            int roomNum = getInt("Enter room number to book: ");
            Room* room = findRoom(roomNum);
            if (!room) {
                cout << "Room not found.\n";
                return;
            }
            if (!room->isAvailable()) {
                cout << "Room is already booked.\n";
                return;
            }
            int nights = getInt("How many nights? ");
            if (nights <= 0) {
                cout << "Invalid number of nights.\n";
                return;
            }
            // Calculate bill using strategy pattern
            double bill = billStrategy->calculateBill(nights, room->getRate());
            room->book();
            reservations.emplace_back(currentUser->getUsername(), roomNum, nights, bill);
            cout << "Room booked! Total bill: $" << bill << endl;
        } catch (const exception& e) {
            cout << "An error occurred while booking: " << e.what() << endl;
        }
    }

    // Show reservations for current customer (Information display + Encapsulation)
    void myReservations() {
        cout << "\nYour Reservations:\n";
        cout << "Customer    | Room | Nights | Total Bill\n";
        cout << "----------------------------------------\n";
        for (const auto& res : reservations) {
            if (res.getCustomerName() == currentUser->getUsername())
                res.show();
        }
    }

    // Show all reservations (Admin only) (Information display)
    void showReservations() {
        cout << "\nAll Reservations:\n";
        for (const auto& res : reservations)
            res.show();
    }

    // Cancel reservation (Access control + Business logic + Exception handling)
    void cancelReservation() {
        try {
            cout << "\nCancel Reservation:\n";
            int roomNum = getInt("Enter room number of reservation to cancel: ");
            bool found = false;

            for (auto it = reservations.begin(); it != reservations.end(); ++it) {
                if (it->getRoomNumber() == roomNum) {
                    // Customer can only cancel their own reservation
                    if (dynamic_cast<Customer*>(currentUser)) {
                        if (it->getCustomerName() != currentUser->getUsername()) {
                            cout << "You can only cancel your own reservations.\n";
                            return;
                        }
                    }
                    found = true;
                    Room* room = findRoom(roomNum);
                    if (room) room->release();
                    reservations.erase(it);
                    cout << "Reservation canceled.\n";
                    break;
                }
            }
            if (!found) {
                cout << "Reservation not found.\n";
            }
        } catch (const exception& e) {
            cout << "An error occurred while canceling reservation: " << e.what() << endl;
        }
    }

    // Change billing strategy dynamically (Strategy pattern)
    void changeBillStrategy() {
        cout << "\nSelect Bill Strategy:\n";
        cout << "1. Standard\n2. Discount (10% off for 5+ nights)\nChoice: ";
        int choice = getInt("");
        delete billStrategy;
        if (choice == 1) billStrategy = new StandardBill();
        else billStrategy = new DiscountBill();

        cout << "Bill strategy changed.\n";
    }

    // Admin function: Add new room (Encapsulation + Exception handling)
    void addRoom() {
        try {
            int roomNum = getInt("Enter new room number: ");
            if (findRoom(roomNum)) {
                cout << "Room number already exists.\n";
                return;
            }
            double rate = getDouble("Enter room rate: ");
            rooms.emplace_back(roomNum, rate);
            cout << "Room added successfully.\n";
        } catch (const exception& e) {
            cout << "An error occurred while adding room: " << e.what() << endl;
        }
    }

    // Admin function: Edit room rate (Encapsulation + Exception handling)
    void editRoomRate() {
        try {
            int roomNum = getInt("Enter room number to edit rate: ");
            Room* room = findRoom(roomNum);
            if (!room) {
                cout << "Room not found.\n";
                return;
            }
            double newRate = getDouble("Enter new rate: ");
            room->setRate(newRate);
            cout << "Room rate updated.\n";
        } catch (const exception& e) {
            cout << "An error occurred while editing room rate: " << e.what() << endl;
        }
    }

    // Admin function: Edit room availability (Encapsulation)
    void editRoomAvailability() {
        int roomNum = getInt("Enter room number to change availability: ");
        Room* room = findRoom(roomNum);
        if (!room) {
            cout << "Room not found.\n";
            return;
        }
        cout << "Current availability: " << (room->isAvailable() ? "Available" : "Booked") << endl;
        cout << "1. Make Available\n2. Make Booked\nChoice: ";
        int choice;
        cin >> choice;
        cin.ignore(1000, '\n');
        if (choice == 1) {
            room->release();
            cout << "Room marked as available.\n";
        } else if (choice == 2) {
            room->book();
            cout << "Room marked as booked.\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }
};

int main() {
    HotelSystem hs;
    hs.mainMenu();
    return 0;
}
