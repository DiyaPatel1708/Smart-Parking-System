#include<iostream>
#include<string>
#include<ctime>
#include<vector>
#include<fstream>
#include<unordered_map>
#include<limits>
using namespace std;

enum class vehicleType{cycle,fiveSeaterCar,sevenSeaterCar,motorcycle,EV,disabled,bus,truck};
enum class spotStatus {free,reserved,occupied,maintenance};
enum class parkingSpotType {standard,premium,ev_charging,motorcycle, disabled};

// ---------------- USER CLASS ----------------
class user {
private:
    string name;
    string userID;
    long long contact;
    string email;
    vehicleType Type;
    parkingSpotType spotType;
    static int userCount;
    time_t entryTime;
    int rating;       // ⭐ added
    string review;    // 📝 added
public:
    user(string name, string userID, long long contact, string email = "not_provided@mail.com", 
         vehicleType Type = vehicleType::fiveSeaterCar, parkingSpotType spotType = parkingSpotType::standard) {
        this->name=name; 
        this->userID=userID; 
        this->contact=contact; 
        this->email=email; 
        this->Type=Type; 
        this->spotType=spotType;
        this->rating = 0;
        this->review = "No review yet.";
        userCount++; 
        entryTime=time(NULL);
    }  
    void showInfo() const{ 
        cout<<"User: "<<name<<" | ID: "<<userID<<" | Contact: "<<contact
            <<" | Email: "<<email<<" | Vehicle Type: "<<(int)Type
            <<" | Rating: "<<rating<<" | Review: "<<review<<"\n";
    }
    static int getUserCount() { 
        return userCount; 
    }

    void setRating(int stars) {
        if(stars < 1 || stars > 5) {
            cout<<"Invalid rating! Must be 1–5 stars.\n";
            return;
        }
        rating = stars;
        cout<<"Thank you! You rated us "<<rating<<" stars.\n";
    }

    void setReview(const string &rev) {
        review = rev;
        cout<<"Review added!\n";
    }

    // Save user info into file (append mode)
    void saveToFile(const string &filename) const {
        ofstream fout(filename, ios::app);
        fout<<name<<" | "<<userID<<" | "<<contact<<" | "<<email
            <<" | VehicleType: "<<(int)Type
            <<" | Entry: "<<ctime(&entryTime)
            <<" | Rating: "<<rating
            <<" | Review: "<<review<<"\n";
        fout.close();
    }

    // Read all stored users from file
    static void readFromFile(const string &filename) {
        ifstream fin(filename);
        if(!fin) {
            cout<<"No data file found!\n";
            return;
        }
        string line;
        cout<<"\n--- Stored User Data ---\n";
        while(getline(fin,line)) {
            cout<<line<<"\n";
        }
        fin.close();
    }

    friend void displayContact(const user& u);
};
void displayContact(const user& u){
    cout << "Contact of " << u.name << " is: " << u.contact << "\n";
}
int user::userCount=0;

// ---------------- PRICING ----------------
class dynamicPrice {
public:
    static int calRate(int occupied,int capacity,vehicleType type, parkingSpotType spot, bool isCharging=false){ 
        int mRate=0;
        switch(type) {
            case vehicleType::motorcycle: mRate=60; break;
            case vehicleType::fiveSeaterCar:
                if(spot==parkingSpotType::standard) mRate=100;
                else if(spot==parkingSpotType::ev_charging) mRate=150;
                else if(spot==parkingSpotType::premium) mRate=175;
                break;
            case vehicleType::disabled: mRate = 30; break;
            case vehicleType::EV: mRate = 120; if(isCharging) mRate *= 1.2; break;
            default: break;
        }
        double ratio = (double)occupied / capacity;
        if(ratio < 0.3) mRate *= 0.8;     
        else if(ratio > 0.8) mRate *= 1.5;  
        return mRate;
    }
};

// ---------------- PAYMENT ----------------
class Payment {
    static int totalTwoWheelerRevenue;
    static int totalFourWheelerRevenue;
    static int totalDisabledRevenue;
    static int totalEVRevenue;
private:
    Payment(){}
public:
    inline static void addRevenue(int amt,vehicleType vehicle){
        if(vehicle==vehicleType::motorcycle) totalTwoWheelerRevenue+=amt;
        else if(vehicle==vehicleType::fiveSeaterCar) totalFourWheelerRevenue+=amt;
        else if(vehicle==vehicleType::disabled) totalDisabledRevenue+=amt;
        else if(vehicle==vehicleType::EV) totalEVRevenue+=amt;
    }
    inline static void showRevenue(){
        cout<<"---Vehicles Revenue--- \n";
        cout<<"Two-Wheeler Revenue: Rs "<<totalTwoWheelerRevenue<<"\n";
        cout<<"Four-Wheeler Revenue: Rs "<<totalFourWheelerRevenue<<"\n";
        cout<<"Disabled vehicle Revenue: Rs "<<totalDisabledRevenue<<"\n";
        cout<<"EV Revenue: Rs "<<totalEVRevenue<<"\n";
    }

    static void pay(int amt) { 
        cout<<"Payment Successful || Rs."<<amt<<" in cash.\n";
    }
    static void pay(int amt, string cardNumber) { 
        cout<<"Payment Successful || Rs."<<amt<<" using Card: "<<cardNumber<<"\n";
    }
    static void pay(int amt, string upiID, bool isUPI) { 
        cout<<"Payment Successful || Rs."<<amt<<" using UPI: "<<upiID<<"\n";
    }
    static void pay(int amt, string accNumber, string bankName) { 
        cout<<"Payment Successful || Rs."<<amt<<" using Netbanking | Account: "<<accNumber<<" Bank: "<<bankName<<"\n";
    }
};

int Payment::totalTwoWheelerRevenue=0;
int Payment::totalFourWheelerRevenue=0;
int Payment::totalDisabledRevenue=0;
int Payment::totalEVRevenue=0;


// ---------------- MAIN ----------------
int main(){
    int choice;
    unordered_map<string, user*> users;

    do {
        cout << "\n========== SMART PARKING MENU ==========\n";
        cout << "1. Register User\n";
        cout << "2. Book Parking Spot\n";
        cout << "3. Show Total Revenue\n";
        cout << "4. Rate System (1-5 stars)\n";
        cout << "5. Add Review\n";
        cout << "6. Exit\n";
        cout << "7. View All Users (from file)\n";   // NEW MENU
        cout << "Enter choice: ";

        cin >> choice;
        while (cin.fail()) {
            cout << "Invalid input. Please enter a valid choice: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> choice;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch(choice) {
            case 1: {
                string name, id; 
                long long contact;
                cout << "Enter Name: "; cin >> name;
                cout << "Enter User ID: "; cin >> id;

                bool validContact = false;
                while (!validContact) {
                    cout << "Enter Contact (10 digits): "; 
                    cin >> contact;

                    if(cin.fail() || contact < 1000000000LL || contact > 9999999999LL) {
                        cout << "Invalid contact! Must be a 10-digit number.\n\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    } else validContact = true;
                }

                if(users.find(id) != users.end()) {
                    cout << "UserID already exists!\n";
                } else {
                    users[id] = new user(name, id, contact);
                    cout << "User Registered Successfully!\n";
                }
                break;
            }

            case 2: {
                if(users.empty()) { cout << "No users registered yet!\n"; break; }

                string id; 
                cout << "Enter User ID: "; cin >> id;
                auto it = users.find(id);
                if(it == users.end()) { cout << "User not found!\n"; break; }

                user* u = it->second;

                int capacity = 100; // total spots
                int occupied = 40;  // currently occupied
                vehicleType type = vehicleType::fiveSeaterCar;
                parkingSpotType spot = parkingSpotType::standard;
                bool isCharging = false;

                int rate = dynamicPrice::calRate(occupied, capacity, type, spot, isCharging);
                cout << "Parking Rate: Rs " << rate << "\n";

                Payment::addRevenue(rate, type);

                int payChoice;
                cout << "Choose Payment Method:\n";
                cout << "1. Cash\n2. Card\n3. UPI\n4. Netbanking\n";
                cout << "Enter choice: "; cin >> payChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch(payChoice){
                    case 1:
                        Payment::pay(rate); // cash
                        break;
                    case 2: {
                        string card;
                        cout << "Enter Card Number: "; getline(cin, card);
                        Payment::pay(rate, card); // card
                        break;
                    }
                    case 3: {
                        string upi;
                        cout << "Enter UPI ID: "; getline(cin, upi);
                        Payment::pay(rate, upi, true); // UPI
                        break;
                    }
                    case 4: {
                        string acc, bank;
                        cout << "Enter Bank Name: "; getline(cin, bank);
                        cout << "Enter Account Number: "; getline(cin, acc);
                        Payment::pay(rate, acc, bank); // Netbanking
                        break;
                    }
                    default:
                        cout << "Invalid payment choice! Paying by cash by default.\n";
                        Payment::pay(rate);
                }
                break;
            }

            case 3:
                Payment::showRevenue();
                break;

            case 4: {
                if(users.empty()) { cout << "No users registered yet!\n"; break; }

                string id; 
                cout << "Enter User ID: "; cin >> id;
                auto it = users.find(id);
                if(it == users.end()) { cout << "User not found!\n"; break; }

                user* u = it->second;
                int stars; 
                cout << "Enter rating (1-5 stars): "; cin >> stars;
                u->setRating(stars);
                u->saveToFile("users.txt");
                break;
            }

            case 5: {
                if(users.empty()) { cout << "No users registered yet!\n"; break; }

                string id;
                cout << "Enter User ID: "; cin >> id;
                auto it = users.find(id);
                if(it == users.end()) { cout << "User not found!\n"; break; }

                user* u = it->second;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

                string rev;
                cout << "Enter your review: ";
                getline(cin, rev);

                u->setReview(rev);
                u->saveToFile("users.txt"); 
                break;
            }

            case 6:
                cout << "Exiting... Goodbye!\n";
                break;

            case 7: {
                cout << "Reading all stored users from file...\n";
                user::readFromFile("users.txt");
                break;
            }

            default:
                cout << "Invalid choice!\n";
        }
    } while(choice != 6);

    return 0;
}
