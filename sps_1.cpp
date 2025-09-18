#include <iostream>
#include<string>
#include<limits>
#include<unordered_map>
#include<ctime>
#include<fstream>
#include<sstream>
using namespace std;

enum class vehicleType{cycle,fiveSeaterCar,sevenSeaterCar,motorcycle,EV,disabled,bus,truck};
enum class spotStatus {free,reserved,occupied,maintenance};
<<<<<<< HEAD
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
    int score;
    int useCount;
    int rating; 
    string review;

public:
    user(string name, string userID, long long contact, string email = "not_provided@mail.com", 
         vehicleType Type = vehicleType::car, parkingSpotType spotType = parkingSpotType::standard) {
        this->name=name; 
        this->userID=userID; 
        this->contact=contact; 
        this->email=email; 
        this->Type=Type; 
        this->score=0;
        this->useCount=0;
        this->rating=0;
        userCount++; 
        entryTime=time(NULL);
    }  

    void showInfo() const { 
        cout << "User: " << name << " | ID: " << userID << " | Contact: " << contact
             << " | Email: " << email << " | Vehicle Type: " << (int)Type
             << " | Rating: " << (rating==0?"Not rated":to_string(rating)+" star(s)") << "\n";
    }

    static int getUserCount() { 
        return userCount; 
    }
    string getID() const { 
        return userID; 
    }

    void saveToFile(const string &filename) const {
        ofstream fout(filename, ios::app);
        fout << name << " | " << userID << " | " << contact
             << " | " << email << " | VehicleType: " << (int)Type
             << " | Entry: " << ctime(&entryTime)
             << " | Rating: " << (rating==0?"Not rated":to_string(rating)+" star(s)")
             << " | Review: " << (review.empty()?"No review":review)

             << "\n";
        fout.close();
    }

    void recordUsage() {
        useCount++;
        score += 5;  
        cout << name << " used the system. Usage Count: " << useCount << " | Score: " << score << "\n";
    }

    int getDiscountedRate(int baseRate) {
        if(score >= 50) { // Example loyalty discount
            cout << "Discount applied! 10% off.\n";
            return baseRate * 0.9;
        }
        return baseRate;
    }

    void setRating(int r) {
        if(r < 1 || r > 5) {
            cout << "Invalid rating! Must be 1-5 stars.\n";
            return;
        }
        rating = r;
        cout << "Rating recorded: " << rating << " star(s)\n";
        if(rating==1 || rating==2) {
            cout<<"Will try better next time. ";
    }
    cout<<"Thanks for choosing us!";
}

void setReview(const string &r) {
    review = r;
    cout << "Review recorded!\n";
}



    friend void displayContact(const user& u);
};

void displayContact(const user& u) {
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
            case vehicleType::car:
                if(spot==parkingSpotType::standard) mRate=100;
                else if(spot==parkingSpotType::ev_charging) mRate=150;
                else if(spot==parkingSpotType::premium) mRate=175;
                break;
            case vehicleType::disabled: mRate = 30; break;
            case vehicleType::EV: mRate = 120; if(isCharging) mRate *= 1.2; break;
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
        else if(vehicle==vehicleType::car) totalFourWheelerRevenue+=amt;
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


int main(){
    int choice;
                string id;
            string rev;
    unordered_map<string, user*> users;

    do {
    cout << "\n========== SMART PARKING MENU ==========\n";
    cout << "1. Register User\n";
    cout << "2. Book Parking Spot\n";
    cout << "3. Show Total Revenue\n";
    cout << "4. Rate System (1-5 stars)\n";
    cout << "5. Add Review\n";
    cout << "6. Exit\n";
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

    // Automatically calculate base rate
    int capacity = 100; // total spots
    int occupied = 40;  // currently occupied
    vehicleType type = vehicleType::car;
    parkingSpotType spot = parkingSpotType::standard;
    bool isCharging = false;

    int rate = dynamicPrice::calRate(occupied, capacity, type, spot, isCharging);
    cout << "Parking Rate: Rs " << rate << "\n";

    u->recordUsage();
    int finalRate = u->getDiscountedRate(rate);

    Payment::addRevenue(finalRate, type);

    // Payment Method Selection
    int payChoice;
    cout << "Choose Payment Method:\n";
    cout << "1. Cash\n2. Card\n3. UPI\n4. Netbanking\n";
    cout << "Enter choice: "; cin >> payChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch(payChoice){
        case 1:
            Payment::pay(finalRate); // cash
            break;
        case 2: {
            string card;
            cout << "Enter Card Number: "; getline(cin, card);
            Payment::pay(finalRate, card); // card
            break;
        }
        case 3: {
            string upi;
            cout << "Enter UPI ID: "; getline(cin, upi);
            Payment::pay(finalRate, upi, true); // UPI
            break;
        }
        case 4: {
            string acc, bank;
            cout << "Enter Bank Name: "; getline(cin, bank);
            cout << "Enter Account Number: "; getline(cin, acc);
            Payment::pay(finalRate, acc, bank); // Netbanking
            break;
        }
        default:
            cout << "Invalid payment choice! Paying by cash by default.\n";
            Payment::pay(finalRate);
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

        default:
            cout << "Invalid choice!\n";
    }
} while(choice != 6);

}
=======
enum class parkingSpotType {
    standard,    
    premium,     
    ev_charging, 
    disabled,    
    motorcycle  
};
class parkingGarage {
private:
    string name;
    string location;
    static int totalGarages;
public: 
    parkingGarage(const string &name, const string &location){
        this->name=name; 
        this->location=location; 
        totalGarages++;
    }
    int getTotalGarages(){
        return totalGarages; 
    }
    void displayInfo() const{
        cout<<"Garage: "<<name<<" at "<<location<<"\n";
    }
    ~parkingGarage(){
        totalGarages--;
    }
};
int parkingGarage::totalGarages=0;
class parkingSpot{
private:
    string spotID;
    spotStatus status;
    parkingSpotType type;
    time_t reservationTime;
public:
    parkingSpot(const string& id, parkingSpotType t){
        spotID=id; type=t; 
        status=spotStatus::free; 
        reservationTime=0;
    }
    friend void debugSpot(const parkingSpot& ps);

    string getSpotId() const{ 
        return spotID; 
    }
    spotStatus getStatus() const{ 
        return status; 
    }
    bool isVehicleCompatibleWithSpot(vehicleType vehicle,parkingSpotType spot){
        switch(spot){
            case parkingSpotType::standard:
                return (vehicle == vehicleType::car || vehicle == vehicleType::disabled);
            case parkingSpotType::premium:
                return (vehicle == vehicleType::car);
            case parkingSpotType::ev_charging:
                return (vehicle == vehicleType::EV);
            case parkingSpotType::disabled:
                return (vehicle == vehicleType::disabled);
            case parkingSpotType::motorcycle:
                return (vehicle == vehicleType::motorcycle);
            default:
                return false;
        }
    }
    bool reserve(vehicleType vehicle) {  
        if (status != spotStatus::free) {
            cout<<"Spot "<<spotID<<" is not free!\n"; 
            return false; 
        }
        if (!isVehicleCompatibleWithSpot(vehicle, type)){
            cout<<"Vehicle type not suitable for spot "<<spotID<<"!\n"; 
            return false; 
        }
        status=spotStatus::reserved;
        cout<<"Spot "<<spotID<<" reserved successfully.\n";
        return true;  
    }
    string getSpotSymbol(parkingSpotType type) {
        switch (type){
            case parkingSpotType::standard: 
            return 'S';
            case parkingSpotType::premium: 
            return 'P';
            case parkingSpotType::ev_charging: 
            return 'E';
            case parkingSpotType::disabled: 
            return 'D';
            case parkingSpotType::motorcycle:
            return 'M';
            default: return '?';
        }
    } 
    bool reserve(vehicleType vehicle,time_t reservedUntil){
        if(!reserve(vehicle)) return false; 
        reservationTime=reservedUntil;
        cout<<"Reservation for spot "<<spotID<<" is valid until "<<ctime(&reservedUntil);
        return true;
    }
    bool release(){
        if (status==spotStatus::free) return false; 
        status=spotStatus::free; reservationTime=0;
        cout<<"Spot "<<spotID<<" has been released.\n";
        return true;
    }
    friend void checkSpotDetails(const parkingSpot &spot){
        cout<<"[Friend Function] Spot ID: "<<spot.spotID<<", Status: ";
        cout<<"\n";
    }
};
int main(){

    return 0;
}
>>>>>>> 9d1b1d59743e0b575879bc465a3635a3331d301f
