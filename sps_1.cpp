#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

enum class vehicleType { cycle, fiveSeaterCar, sevenSeaterCar, motorcycle, EV, disabled, bus, truck };
enum class spotStatus { free, reserved, occupied, maintenance };
enum class parkingSpotType {standard,premium,ev_charging,motorcycle, disabled};


class user {
    string name;
    string id;
    long long contact;
    string email;
    vehicleType vType;
    int rating;
    string review;

public:
    user(string n, string i, long long c, string e, vehicleType v) {
        name = n;
        id = i;
        contact = c;
        email = e;
        vType = v;
        rating = 0;
    }

    string getId() { return id; }
    string getName() { return name; }
    vehicleType getVehicleType() { return vType; }
    void setRating(int r) { rating = r; }
    void setReview(string rv) { review = rv; }

    void saveToFile(const string &filename) {
        ofstream fout(filename, ios::app);
        fout << name << " " << id << " " << contact << " " << email << " "
             << (int)vType << " " << rating << " " << review << endl;
        fout.close();
    }

    static void readFromFile(const string &filename) {
        ifstream fin(filename);
        string line;
        while (getline(fin, line)) {
            cout << line << endl;
        }
        fin.close();
    }
};

class Payment {
    static int totalTwoWheelerRevenue;
    static int totalFourWheelerRevenue;
    static int totalDisabledRevenue;
    static int totalEVRevenue;

public:
    static void processPayment(user* u, int amount) {
        vehicleType vt = u->getVehicleType();
        if (vt == vehicleType::motorcycle || vt == vehicleType::cycle)
            totalTwoWheelerRevenue += amount;
        else if (vt == vehicleType::fiveSeaterCar || vt == vehicleType::sevenSeaterCar || vt == vehicleType::truck || vt == vehicleType::bus)
            totalFourWheelerRevenue += amount;
        else if (vt == vehicleType::disabled)
            totalDisabledRevenue += amount;
        else if (vt == vehicleType::EV)
            totalEVRevenue += amount;
    }

    static void showRevenue() {
        cout << "Revenue Report:" << endl;
        cout << "Two-Wheeler Revenue: Rs " << totalTwoWheelerRevenue << endl;
        cout << "Four-Wheeler Revenue: Rs " << totalFourWheelerRevenue << endl;
        cout << "Disabled Vehicle Revenue: Rs " << totalDisabledRevenue << endl;
        cout << "EV Revenue: Rs " << totalEVRevenue << endl;
    }

    static void saveRevenueToFile(const string &filename) {
        ofstream fout(filename, ios::app);
        time_t now = time(0);
        fout << "---- Revenue Report (" << ctime(&now) << ") ----\n";
        fout << "Two-Wheeler Revenue: Rs " << totalTwoWheelerRevenue << "\n";
        fout << "Four-Wheeler Revenue: Rs " << totalFourWheelerRevenue << "\n";
        fout << "Disabled Vehicle Revenue: Rs " << totalDisabledRevenue << "\n";
        fout << "EV Revenue: Rs " << totalEVRevenue << "\n\n";
        fout.close();
        cout << "Revenue saved to file: " << filename << "\n";
    }
};

int Payment::totalTwoWheelerRevenue = 0;
int Payment::totalFourWheelerRevenue = 0;
int Payment::totalDisabledRevenue = 0;
int Payment::totalEVRevenue = 0;

void loadUsers(unordered_map<string, user*> &users, const string &filename) {
    ifstream fin(filename);
    if(!fin) return;

    string line;
    while(getline(fin, line)) {
        cout << "[INFO] Skipping load from file for now: " << line << "\n";
    }
    fin.close();
}

int main() {
    unordered_map<string, user*> users;
    loadUsers(users, "users.txt");

    int choice;
    do {
        cout << "\nSmart Parking System Menu:\n";
        cout << "1. Register User\n";
        cout << "2. Book Parking Spot\n";
        cout << "3. Show Revenue\n";
        cout << "4. Give Rating & Review\n";
        cout << "5. Save Users to File\n";
        cout << "6. Load Users from File\n";
        cout << "7. View Stored Users\n";
        cout << "8. View Transactions\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if(choice == 1) {
            string name, id, email;
            long long contact;
            int vtype;
            cout << "Enter Name, ID, Contact, Email, VehicleType(0-7): ";
            cin >> name >> id >> contact >> email >> vtype;
            user* u = new user(name, id, contact, email, (vehicleType)vtype);
            users[id] = u;
            cout << "User Registered.\n";
        }
        else if(choice == 2) {
            string id;
            cout << "Enter User ID: ";
            cin >> id;
            if(users.find(id) == users.end()) {
                cout << "User not found.\n";
            } else {
                int rate = 50;
                Payment::processPayment(users[id], rate);
                cout << "Booking successful. Amount: Rs " << rate << endl;

                ofstream fout("transactions.txt", ios::app);
                time_t now = time(0);
                fout << "UserID: " << id 
                     << " | Amount: Rs " << rate 
                     << " | DateTime: " << ctime(&now);
                fout.close();
            }
        }
        else if(choice == 3) {
            Payment::showRevenue();
            Payment::saveRevenueToFile("revenue.txt");
        }
        else if(choice == 4) {
            string id;
            cout << "Enter User ID: ";
            cin >> id;
            if(users.find(id) == users.end()) {
                cout << "User not found.\n";
            } else {
                int rating;
                string review;
                cout << "Enter rating (1-5): ";
                cin >> rating;
                cin.ignore();
                cout << "Enter review: ";
                getline(cin, review);
                users[id]->setRating(rating);
                users[id]->setReview(review);
            }
        }
        else if(choice == 5) {
            for(auto &p : users) {
                p.second->saveToFile("users.txt");
            }
            cout << "Users saved to file.\n";
        }
        else if(choice == 6) {
            loadUsers(users, "users.txt");
        }
        else if(choice == 7) {
            user::readFromFile("users.txt");
        }
        else if(choice == 8) {
            ifstream fin("transactions.txt");
            if(!fin) { cout<<"No transactions found!\n"; }
            else {
                string line;
                cout << "\n--- Transaction History ---\n";
                while(getline(fin, line)) {
                    cout << line << "\n";
                }
                fin.close();
            }
        }
    } while(choice != 0);

    return 0;
}
