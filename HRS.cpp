#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Strategy Pattern for Bill Calculation
class BillStrategy {
public:
    virtual double calculateBill(int nights, double rate) = 0;
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
            return nights * rate * 0.9; // 10% discount
        return nights * rate;
    }
};

// Room class
class Room {
private:
    int roomNumber;
    bool available;
    double rate;

public:
    Room(int num, double r) : roomNumber(num), available(true), rate(r) {}

    int getRoomNumber() const { return roomNumber; }
    bool isAvailable() const { return available; }
    double getRate() const { return rate; }

    void book() { available = false; }
    void release() { available = true; }
    void setRate(double r) { rate = r; }
};

// Base User class
class User {
protected:
    string username, password;

public:
    User(string u, string p) : username(u), password(p) {}
    virtual void menu() = 0;

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

// Reservation class
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

// System class
class HotelSystem {
private:
    vector<Room> rooms;
    vector<User*> users;
    vector<Reservation> reservations;
    BillStrategy* billStrategy;
    User* currentUser = nullptr;

    Room* findRoom(int num) {
        for (auto& room : rooms)
            if (room.getRoomNumber() == num)
                return &room;
        return nullptr;
    }

    int getInt(const string& prompt) {
        int val;
        cout << prompt;
        while (!(cin >> val)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Try again.\n" << prompt;
        }
        cin.ignore(1000, '\n');
        return val;
    }

    double getDouble(const string& prompt) {
        double val;
        cout << prompt;
        while (!(cin >> val) || val < 0) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Try again.\n" << prompt;
        }
        cin.ignore(1000, '\n');
        return val;
    }

public:
    HotelSystem() {
        rooms.emplace_back(101, 100.0);
        rooms.emplace_back(102, 120.0);
        rooms.emplace_back(103, 150.0);
        users.push_back(new Admin("admin", "admin123"));
        billStrategy = new StandardBill();
    }

    ~HotelSystem() {
        delete billStrategy;
        for (auto user : users)
            delete user;
    }

    void mainMenu() {
        int choice;
        do {
            cout << "Welcome to Hotel Reservation System\n";
            cout << "\nMain Menu\n";
            cout << "\nSelect Role:\n";
            cout << "1. Admin\n2. Customer\n0. Exit\nChoice: ";
            choice = getInt("");

            if (choice == 1) {
                if (adminLogin()) userMenu();
            } else if (choice == 2) {
                customerSubmenu();
            }
        } while (choice != 0);
    }

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
                cout << "Login successful!\n";
                currentUser->menu();
                return true;
            }
        }
        cout << "Invalid Admin credentials.\n";
        return false;
    }

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
                cout << "Login successful!\n";
                currentUser->menu();
                return true;
            }
        }
        cout << "Invalid Customer credentials.\n";
        return false;
    }

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

    void userMenu() {
        if (dynamic_cast<Admin*>(currentUser)) {
            adminMenu();
        } else {
            customerMenu();
        }
    }

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
            cout << "0. Logout\n";
            choice = getInt("Choice: ");
            if (choice == 1) showReservations();
            else if (choice == 2) showRooms();
            else if (choice == 3) changeBillStrategy();
            else if (choice == 4) addRoom();
            else if (choice == 5) editRoomRate();
            else if (choice == 6) editRoomAvailability();
            else if (choice == 7) cancelReservation();
        } while (choice != 0);
    }

    void customerMenu() {
        int choice;
        do {
            cout << "\nCustomer Menu:\n";
            cout << "1. Book Room\n";
            cout << "2. My Room Reservations\n";
            cout << "3. Cancel My Reservation\n";
            cout << "0. Logout\n";
            choice = getInt("Choice: ");
            if (choice == 1) bookRoom();
            else if (choice == 2) myReservations();
            else if (choice == 3) cancelReservation();
        } while (choice != 0);
    }

    void showRooms() {
        cout << "\nRooms:\n";
        for (const auto& room : rooms) {
            cout << "Room " << room.getRoomNumber()
                 << " | Rate: $" << room.getRate()
                 << " | " << (room.isAvailable() ? "Available" : "Booked") << endl;
        }
    }

    void bookRoom() {
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
        double bill = billStrategy->calculateBill(nights, room->getRate());
        room->book();
        reservations.emplace_back(currentUser->getUsername(), roomNum, nights, bill);
        cout << "Room booked! Total bill: $" << bill << endl;
    }

    void myReservations() {
        cout << "\nYour Reservations:\n";
        for (const auto& res : reservations) {
            if (res.getCustomerName() == currentUser->getUsername())
                res.show();
        }
    }

    void showReservations() {
        cout << "\nAll Reservations:\n";
        for (const auto& res : reservations)
            res.show();
    }

    void cancelReservation() {
        cout << "\nCancel Reservation:\n";
        int roomNum = getInt("Enter room number of reservation to cancel: ");
        bool found = false;

        for (auto it = reservations.begin(); it != reservations.end(); ++it) {
            if (it->getRoomNumber() == roomNum &&
                (dynamic_cast<Admin*>(currentUser) || it->getCustomerName() == currentUser->getUsername())) {
                Room* room = findRoom(roomNum);
                if (room) room->release();
                reservations.erase(it);
                cout << "Reservation cancelled successfully.\n";
                found = true;
                break;
            }
        }

        if (!found)
            cout << "Reservation not found or you do not have permission to cancel it.\n";
    }

    void changeBillStrategy() {
        cout << "1. Standard Bill\n2. Discount Bill (10% off for 5+ nights)\n";
        int c = getInt("Choose strategy: ");
        delete billStrategy;
        if (c == 2)
            billStrategy = new DiscountBill();
        else
            billStrategy = new StandardBill();
        cout << "Bill strategy changed.\n";
    }

    void addRoom() {
        int roomNum = getInt("Enter new room number: ");
        for (const auto& room : rooms) {
            if (room.getRoomNumber() == roomNum) {
                cout << "Room number already exists.\n";
                return;
            }
        }
        double rate = getDouble("Enter room rate: ");
        rooms.push_back(Room(roomNum, rate));
        cout << "Room added successfully.\n";
    }

    void editRoomRate() {
        int roomNum = getInt("Enter room number to edit rate: ");
        Room* room = findRoom(roomNum);
        if (!room) {
            cout << "Room not found.\n";
            return;
        }
        double newRate = getDouble("Enter new rate: ");
        room->setRate(newRate);
        cout << "Room rate updated.\n";
    }

    void editRoomAvailability() {
        int roomNum = getInt("Enter room number to change availability: ");
        Room* room = findRoom(roomNum);
        if (!room) {
            cout << "Room not found.\n";
            return;
        }
        cout << "1. Set as Available\n2. Set as Booked\n";
        int choice = getInt("Choice: ");
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
    HotelSystem system;
    system.mainMenu();
    return 0;
}
