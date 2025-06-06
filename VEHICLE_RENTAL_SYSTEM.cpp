#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <limits>
using namespace std;

// -------- STRATEGY PATTERN --------
class ChargeStrategy {
public:
    virtual double calculateCharge(int duration) const = 0;
    virtual ~ChargeStrategy() = default;
};

class PerHourStrategy : public ChargeStrategy {
public:
    double calculateCharge(int hours) const override {
        return hours * 100.0; // Example: 100 per hour
    }
};

class PerKmStrategy : public ChargeStrategy {
public:
    double calculateCharge(int km) const override {
        return km * 10.0; // Example: 10 per km
    }
};

// -------- USER CLASS --------
class User {
private:
    string username;
    string password;

public:
    User(string uname, string pwd) {
        if (uname.empty() || pwd.empty()) {
            throw invalid_argument("Username or password cannot be empty.");
        }
        username = uname;
        password = pwd;
    }

    string getUsername() const { return username; }

    bool login(string uname, string pwd) const {
        return username == uname && password == pwd;
    }
};

// -------- VEHICLE BASE CLASS --------
class Vehicle {
protected:
    int id;
    string type;
    bool available;
    unique_ptr<ChargeStrategy> strategy;

public:
    Vehicle(int id, string type, unique_ptr<ChargeStrategy> strat)
        : id(id), type(type), available(true), strategy(move(strat)) {}

    virtual ~Vehicle() = default;

    virtual void display() const {
        cout << "Vehicle ID: " << id << ", Type: " << type
             << ", Available: " << (available ? "Yes" : "No") << endl;
    }

    int getId() const { return id; }
    bool isAvailable() const { return available; }
    void setAvailable(bool status) { available = status; }

    double calculateCharge(int input) const {
        return strategy->calculateCharge(input);
    }
};

// -------- VEHICLE TYPES --------
class Car : public Vehicle {
public:
    Car(int id) : Vehicle(id, "Car", make_unique<PerHourStrategy>()) {}
};

class Motorcycle : public Vehicle {
public:
    Motorcycle(int id) : Vehicle(id, "Motorcycle", make_unique<PerKmStrategy>()) {}
};

// -------- RENTAL SYSTEM --------
class RentalSystem {
private:
    vector<User> users;
    vector<unique_ptr<Vehicle>> vehicles;
    User* currentUser = nullptr;

    int inputInt(string prompt) {
        int val;
        while (true) {
            cout << prompt;
            cin >> val;
            if (cin.fail() || val < 0) {
                cout << "Invalid input. Try again.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else {
                return val;
            }
        }
    }

public:
    void signUp() {
        string uname, pwd;
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        cin >> pwd;
        try {
            users.emplace_back(uname, pwd);
            cout << "Signup successful.\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void login() {
        string uname, pwd;
        cout << "Username: ";
        cin >> uname;
        cout << "Password: ";
        cin >> pwd;
        for (auto& u : users) {
            if (u.login(uname, pwd)) {
                currentUser = &u;
                cout << "Login successful!\n";
                return;
            }
        }
        cout << "Invalid credentials.\n";
    }

    void addVehicle() {
        int id = inputInt("Enter Vehicle ID: ");
        int choice;
        cout << "1. Car\n2. Motorcycle\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            vehicles.push_back(make_unique<Car>(id));
        } else if (choice == 2) {
            vehicles.push_back(make_unique<Motorcycle>(id));
        } else {
            cout << "Invalid type.\n";
        }
    }

    void viewVehicles() {
        for (const auto& v : vehicles) {
            v->display();
        }
    }

    void bookVehicle() {
        int id = inputInt("Enter Vehicle ID to book: ");
        for (auto& v : vehicles) {
            if (v->getId() == id && v->isAvailable()) {
                v->setAvailable(false);
                cout << "Vehicle booked successfully.\n";
                return;
            }
        }
        cout << "Vehicle not available.\n";
    }

    void returnVehicle() {
        int id = inputInt("Enter Vehicle ID to return: ");
        for (auto& v : vehicles) {
            if (v->getId() == id && !v->isAvailable()) {
                v->setAvailable(true);
                int usage = inputInt("Enter hours/km used: ");
                double charge = v->calculateCharge(usage);
                cout << "Vehicle returned. Charge: $" << charge << endl;
                return;
            }
        }
        cout << "Invalid vehicle or already returned.\n";
    }

    void menu() {
        int choice;
        do {
            cout << "\n--- Vehicle Rental Menu ---\n";
            cout << "1. Add Vehicle\n2. View Vehicles\n3. Book Vehicle\n";
            cout << "4. Return Vehicle\n5. Logout\n";
            cin >> choice;
            switch (choice) {
                case 1: addVehicle(); break;
                case 2: viewVehicles(); break;
                case 3: bookVehicle(); break;
                case 4: returnVehicle(); break;
                case 5: currentUser = nullptr; break;
                default: cout << "Invalid option.\n";
            }
        } while (currentUser != nullptr);
    }

    void start() {
        int choice;
        do {
            cout << "\n--- Welcome to Vehicle Rental System ---\n";
            cout << "1. Sign Up\n2. Login\n3. Exit\n";
            cin >> choice;
            switch (choice) {
                case 1: signUp(); break;
                case 2: login(); if (currentUser) menu(); break;
                case 3: cout << "Goodbye!\n"; break;
                default: cout << "Invalid option.\n";
            }
        } while (choice != 3);
    }
};

int main() {
    RentalSystem app;
    app.start();
    return 0;
}