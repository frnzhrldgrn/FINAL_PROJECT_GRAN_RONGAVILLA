//FRANZHAROLDGRAN
//CHRISTIANRONGAVILLA


#include <iostream>
#include <vector>
#include <string>
#include <memory>      
#include <stdexcept>    
#include <limits>       

using namespace std;

// -------- STRATEGY PATTERN --------
// Abstract Base Class for Charge Calculation Strategy
class ChargeStrategy {
public:
    // Pure virtual method: derived classes MUST implement this to calculate charge.
    virtual double calculateCharge(int duration) const = 0;
    // Virtual destructor: ensures proper cleanup for derived objects when deleted polymorphically.
    virtual ~ChargeStrategy() = default;
};

// Concrete Strategy: Calculates charge based on hours used
class PerHourStrategy : public ChargeStrategy {
public:
    double calculateCharge(int hours) const override {
        // Example rate: 100 per hour
        return hours * 100.0;
    }
};

// Concrete Strategy: Calculates charge based on kilometers traveled
class PerKmStrategy : public ChargeStrategy {
public:
    double calculateCharge(int km) const override {
        // Example rate: 10 per kilometer
        return km * 10.0;
    }
};

// -------- USER CLASS --------
class User {
private:
    string username; // Stores the user's chosen username
    string password; // Stores the user's password

public:
    // Constructor: Initializes a new user account
    User(string uname, string pwd) {
        // Basic validation: ensure username and password are not empty
        if (uname.empty() || pwd.empty()) {
            throw invalid_argument("Username or password cannot be empty.");
        }
        username = uname;
        password = pwd;
    }

    // Getter method to retrieve the username
    string getUsername() const { return username; }

    // Method to verify login credentials
    bool login(string uname, string pwd) const {
        return username == uname && password == pwd;
    }
};

// -------- VEHICLE BASE CLASS --------
class Vehicle {
protected:
    int id;                               // Unique identifier for the vehicle
    string type;                          // Describes the vehicle type (e.g., "Car", "Motorcycle")
    bool available;                       // Current availability status (true if available)
    // unique_ptr: Manages the lifetime of the associated ChargeStrategy object.
    // This is the "Context" that uses the "Strategy".
    unique_ptr<ChargeStrategy> strategy;

public:
    // Constructor: Initializes a Vehicle with an ID, type, and its specific charging strategy.
    Vehicle(int id, string type, unique_ptr<ChargeStrategy> strat)
        : id(id), type(type), available(true), strategy(move(strat)) {}

    // Virtual destructor: Essential for polymorphism, ensuring correct cleanup
    // of derived class objects when deleted via a base class pointer.
    virtual ~Vehicle() = default;

    // Virtual method to display vehicle details. Can be overridden by derived classes.
    virtual void display() const {
        cout << "Vehicle ID: " << id << ", Type: " << type
             << ", Available: " << (available ? "Yes" : "No") << endl;
    }

    // Getters for vehicle properties
    int getId() const { return id; }
    bool isAvailable() const { return available; }

    // Setter for vehicle availability status
    void setAvailable(bool status) { available = status; }

    // Method to calculate rental charge by delegating to the assigned strategy.
    double calculateCharge(int input) const {
        // This call is polymorphic: it invokes the calculateCharge method
        // specific to the concrete strategy (PerHourStrategy or PerKmStrategy)
        // associated with this particular vehicle.
        return strategy->calculateCharge(input);
    }
};

// -------- VEHICLE TYPES --------
// Car class: Inherits from Vehicle, specifically for cars
class Car : public Vehicle {
public:
    // Car constructor: Calls the Vehicle base constructor, setting type "Car"
    // and assigning a PerHourStrategy for its charge calculation.
    Car(int id) : Vehicle(id, "Car", make_unique<PerHourStrategy>()) {}
};

// Motorcycle class: Inherits from Vehicle, specifically for motorcycles
class Motorcycle : public Vehicle {
public:
    // Motorcycle constructor: Calls the Vehicle base constructor, setting type "Motorcycle"
    // and assigning a PerKmStrategy for its charge calculation.
    Motorcycle(int id) : Vehicle(id, "Motorcycle", make_unique<PerKmStrategy>()) {}
};

// -------- RENTAL SYSTEM --------
class RentalSystem {
private:
    vector<User> users;                     // Stores all registered user accounts
    vector<unique_ptr<Vehicle>> vehicles;   // Stores all vehicles managed by the system (using smart pointers for ownership)
    User* currentUser = nullptr;            // Raw pointer to the currently logged-in user (nullptr if no one logged in)

    // Helper method for safe integer input validation
    int inputInt(string prompt) {
        int val;
        while (true) {
            cout << prompt;
            cin >> val;
            // Check if input failed (e.g., non-numeric) or if value is negative
            if (cin.fail() || val < 0) {
                cout << "Invalid input. Please enter a non-negative number.\n";
                cin.clear(); // Clear error flags on cin
                // Ignore the rest of the current line to prevent infinite loop on bad input
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else {
                return val; // Valid input received
            }
        }
    }

public:
    // Allows new users to register an account
    void signUp() {
        string uname, pwd;
        cout << "Enter username: ";
        cin >> uname;
        cout << "Enter password: ";
        cin >> pwd;
        try {
            // Emplace directly constructs a User object in the vector, more efficient
            users.emplace_back(uname, pwd);
            cout << "Signup successful.\n";
        } catch (const exception& e) {
            // Catches invalid_argument thrown by User constructor for empty credentials
            cout << "Error: " << e.what() << endl;
        }
    }

    // Authenticates and logs in an existing user
    void login() {
        string uname, pwd;
        cout << "Username: ";
        cin >> uname;
        cout << "Password: ";
        cin >> pwd;
        for (auto& u : users) { // Iterate through all registered users
            if (u.login(uname, pwd)) { // Use the User class's login method
                currentUser = &u; // Set the current user pointer to the logged-in user
                cout << "Login successful!\n";
                return; // Exit after successful login
            }
        }
        cout << "Invalid credentials.\n"; // If loop finishes, no matching user found
    }

    // Allows adding new vehicles (Car or Motorcycle) to the system
    void addVehicle() {
        int id = inputInt("Enter Vehicle ID: "); // Use validated input
        // (Note: In a real system, you'd add logic here to prevent duplicate IDs)

        int choice;
        cout << "1. Car\n2. Motorcycle\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            // Dynamically create a Car and add its unique_ptr to the vector
            vehicles.push_back(make_unique<Car>(id));
            cout << "Car with ID " << id << " added.\n";
        } else if (choice == 2) {
            // Dynamically create a Motorcycle and add its unique_ptr to the vector
            vehicles.push_back(make_unique<Motorcycle>(id));
            cout << "Motorcycle with ID " << id << " added.\n";
        } else {
            cout << "Invalid type.\n";
        }
    }

    // Displays the details of all vehicles in the system
    void viewVehicles() {
        if (vehicles.empty()) {
            cout << "No vehicles in the system yet.\n";
            return;
        }
        cout << "\n--- Current Vehicles ---\n";
        for (const auto& v : vehicles) {
            v->display(); // Polymorphic call: invokes the Vehicle::display() for each type
        }
        cout << "----------------------\n";
    }

    // Allows a user to book an available vehicle
    void bookVehicle() {
        int id = inputInt("Enter Vehicle ID to book: ");
        for (auto& v : vehicles) {
            if (v->getId() == id && v->isAvailable()) { // Find vehicle by ID and check availability
                v->setAvailable(false); // Mark the vehicle as unavailable
                cout << "Vehicle ID " << id << " booked successfully.\n";
                return;
            }
        }
        cout << "Vehicle with ID " << id << " not found or not available.\n";
    }

    // Allows a user to return a booked vehicle and calculates the charge
    void returnVehicle() {
        int id = inputInt("Enter Vehicle ID to return: ");
        for (auto& v : vehicles) {
            if (v->getId() == id && !v->isAvailable()) { // Find vehicle by ID and ensure it's currently booked
                v->setAvailable(true); // Mark the vehicle as available again
                int usage = inputInt("Enter hours/km used: "); // Get usage (hours for Car, km for Motorcycle)
                
                // Polymorphic call: The correct calculateCharge method (PerHourStrategy or PerKmStrategy)
                // is called based on the vehicle's assigned strategy.
                double charge = v->calculateCharge(usage);
                cout << "Vehicle ID " << id << " returned. Total charge: $" << charge << endl;
                return;
            }
        }
        cout << "Invalid vehicle ID or vehicle was not currently booked.\n";
    }

    // Main menu displayed to a logged-in user
    void menu() {
        int choice;
        do {
            cout << "\n--- Vehicle Rental Menu ---\n";
            cout << "1. Add Vehicle\n2. View Vehicles\n3. Book Vehicle\n";
            cout << "4. Return Vehicle\n5. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;
            switch (choice) {
                case 1: addVehicle(); break;
                case 2: viewVehicles(); break;
                case 3: bookVehicle(); break;
                case 4: returnVehicle(); break;
                case 5: currentUser = nullptr; cout << "Logged out successfully.\n"; break; // Logout: set currentUser to nullptr
                default: cout << "Invalid option. Please try again.\n";
            }
        } while (currentUser != nullptr); // Loop as long as a user is logged in
    }

    // Application's entry point, handles initial sign-up/login flow
    void start() {
        int choice;
        do {
            cout << "\n--- Welcome to Vehicle Rental System ---\n";
            cout << "1. Sign Up\n2. Login\n3. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;
            switch (choice) {
                case 1: signUp(); break;
                case 2:
                    login();
                    if (currentUser) { // If login was successful, proceed to the main menu
                        menu();
                    }
                    break;
                case 3: cout << "Goodbye! Thank you for using our system.\n"; break;
                default: cout << "Invalid option. Please try again.\n";
            }
        } while (choice != 3); // Continue looping until the user chooses to exit
    }
};

int main() {
    RentalSystem app; // Create an instance of the RentalSystem class.
    app.start();      // Call the start method to begin the application's main loop.
    return 0;         // Indicate successful program execution.
}****
