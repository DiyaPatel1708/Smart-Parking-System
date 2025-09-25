#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

enum class vehicleType { cycle, fiveSeaterCar, sevenSeaterCar, motorcycle, EV, disabled, bus, truck };
enum class spotStatus { free, reserved, occupied, maintenance };
enum class parkingSpotType {cycle,standard,premium,ev_charging,motorcycle,compact,large,small,family,disabled,bus};

class user {
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
    user(string name, string userID, long long contact, vehicleType Type, string email = "not_provided"){
        this->name=name; 
        this->userID=userID; 
        this->contact=contact; 
        this->email=email; 
        this->Type=Type; 
        userCount++; 
        this->score = 0;
        this->useCount = 0;
        this->rating = 0;

        entryTime=time(NULL);
    }  
    void showInfo() const {
        cout << "User: " << name << " | ID: " << userID << " | Contact: " << contact
             << " | Email: " << email << " | Vehicle Type: " << (int)Type
             << " | Rating: " << (rating==0 ? "Not rated" : to_string(rating)+" star(s)")
             << " | Review: " << (review.empty() ? "No review" : review)
             << "\n";
    }

    static int getUserCount() { 
        return userCount; 
    }


    void saveToFile(const string &filename) const {
        ofstream fout(filename, ios::app);
        fout << name << " | " << userID << " | " << contact
             << " | " << email << " | VehicleType: " << (int)Type
             << " | Entry: " << ctime(&entryTime)
             << " | Rating: " << (rating==0 ? "Not rated" : to_string(rating)+" star(s)")
             << " | Review: " << (review.empty() ? "No review" : review)
             << "\n";
        fout.close();
    }

    void recordUsage() {
        useCount++;
        score += 5;
    }

    void showScore() {
        cout << name << " used the system. Usage Count: " << useCount << " | Score: " << score << "\n";
    }


    int getDiscountedRate(int baseRate) {
        if(score >= 50) {
            cout << "Discount applied! 10% off.\n";
            return static_cast<int>(baseRate * 0.9);
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
        if(rating==1 || rating==2) cout<<"Will try better next time. ";
        cout<<"Thanks for choosing us!\n";
    }

    void setReview(const string &r) {
        review = r;
        cout << "Review recorded!\n";
    }

friend void displayContact(const user& u);
    friend vehicleType getVehicleType(const user& u);

    // ---------- Operator Overloading ----------
    friend ostream& operator<<(ostream& os, const user& u) {
        os << "User: " << u.name << " | ID: " << u.userID 
           << " | Contact: " << u.contact 
           << " | Vehicle Type: " << static_cast<int>(u.Type)
           << " | Rating: " << (u.rating==0 ? "Not rated" : to_string(u.rating)+" star(s)")
           << " | Review: " << (u.review.empty() ? "No review" : u.review);
        return os;
    }


    friend bool operator==(const user& u1, const user& u2) {
        return u1.userID == u2.userID;
    }

    friend bool operator<(const user& u1, const user& u2) {
        return u1.name < u2.name;
    }

    friend bool operator!=(const user& u1, const user& u2) {
        return !(u1 == u2);
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
int user::userCount=0;
void displayContact(const user& u) {
    cout << "Contact of " << u.name << " is: " << u.contact << "\n";
}
vehicleType getVehicleType(const user& u) {
    return u.Type;
}


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
            case parkingSpotType::standard: return (vehicle == vehicleType::fiveSeaterCar || vehicle == vehicleType::sevenSeaterCar || vehicle == vehicleType::EV);
            case parkingSpotType::premium: return (vehicle == vehicleType::fiveSeaterCar || vehicle == vehicleType::sevenSeaterCar || vehicle == vehicleType::EV);
            case parkingSpotType::ev_charging: return (vehicle == vehicleType::EV);
            case parkingSpotType::disabled: return (vehicle == vehicleType::disabled);
            case parkingSpotType::motorcycle: return (vehicle == vehicleType::motorcycle);
            case parkingSpotType::cycle: return (vehicle == vehicleType::cycle);
            case parkingSpotType::bus: return (vehicle == vehicleType::bus || vehicle == vehicleType::truck);
            case parkingSpotType::large: return (vehicle == vehicleType::sevenSeaterCar);
            case parkingSpotType::small: return (vehicle == vehicleType::fiveSeaterCar);
            case parkingSpotType::compact: return (vehicle == vehicleType::motorcycle || vehicle == vehicleType::cycle);
            case parkingSpotType::family: return (vehicle == vehicleType::sevenSeaterCar || vehicle == vehicleType::bus || vehicle == vehicleType::truck || vehicle == vehicleType::cycle || vehicle == vehicleType::motorcycle || vehicle == vehicleType::fiveSeaterCar || vehicle == vehicleType::disabled || vehicle == vehicleType::EV);
            default: return false;
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
            case parkingSpotType::standard: return "Stdrd";
            case parkingSpotType::premium: return "Prmium";
            case parkingSpotType::ev_charging: return "EV";
            case parkingSpotType::disabled: return "Disabld";
            case parkingSpotType::motorcycle: return "Mtrcycl";
            case parkingSpotType::large: return "Lrge";
            case parkingSpotType::compact: return "Compact";
            case parkingSpotType::family: return "Fmly";
            case parkingSpotType::cycle: return "Cycl";
            case parkingSpotType::small: return "Sml";
            case parkingSpotType::bus: return "Bus";
            default: return "?";
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
    void checkSpotDetails(const parkingSpot &spot){
        cout<<"Spot ID: "<<spot.spotID<<", Status: "<<(int)spot.status<<"\n";
    }
    void displayGarage() {
        for (auto& spot:spotID){
            string symbol=getSpotSymbol(type); 
            cout<<"["<<symbol<<"] "; 
        }
    }
};
void debugSpot(const parkingSpot& ps){
    cout << "[DEBUG] Spot " << ps.spotID 
         << " | Status: " << (int)ps.status << "\n";
}

class dynamicPrice {
public:
    static int calRate(int occupied,int capacity,vehicleType type, parkingSpotType spot, bool isCharging=false){ 
        int mRate=0;
        switch(type) {
            case vehicleType::cycle:{
                if(spot==parkingSpotType::compact) mRate=20;
                else if(spot==parkingSpotType::cycle) mRate=30;
                else if(spot==parkingSpotType::family) mRate=5;
                break;
            }
            case vehicleType::bus:{
                if(spot==parkingSpotType::bus) mRate=300;
                else if(spot==parkingSpotType::family) mRate=50;
                break;
            }
            case vehicleType::truck:{
                if(spot==parkingSpotType::bus) mRate=350;
                else if(spot==parkingSpotType::family) mRate=55;
                break;
            }
            case vehicleType::motorcycle:{
                if(spot==parkingSpotType::compact) mRate=45;
                else if(spot==parkingSpotType::motorcycle) mRate=50;
                else if(spot==parkingSpotType::family) mRate=10;
                break;
            }
            case vehicleType::fiveSeaterCar:{
                if(spot==parkingSpotType::standard) mRate=100;
                else if(spot==parkingSpotType::ev_charging) mRate=150;
                else if(spot==parkingSpotType::premium) mRate=175;
                else if(spot==parkingSpotType::small) mRate=85;
                else if(spot==parkingSpotType::family) mRate=10;
                break;
            }
            case vehicleType::sevenSeaterCar:{
                if(spot==parkingSpotType::standard) mRate=140;
                else if(spot==parkingSpotType::ev_charging) mRate=190;
                else if(spot==parkingSpotType::premium) mRate=215;
                else if(spot==parkingSpotType::large) mRate=200;
                else if(spot==parkingSpotType::family) mRate=15;
                break;
            }
            case vehicleType::disabled:{
                if(spot==parkingSpotType::disabled) mRate = 30;
                else if(spot==parkingSpotType::family) mRate=5;   
                break;
            }
            case vehicleType::EV:{
                if(spot==parkingSpotType::premium) mRate=175;
                else if(spot==parkingSpotType::standard) mRate=140;
                else if(spot==parkingSpotType::ev_charging) mRate=190;
                else if(spot==parkingSpotType::family) mRate=14;
                if(isCharging) mRate *= 1.2;  
                break;
            }
        }
        double ratio = (double)occupied / capacity;
        if(ratio < 0.3) mRate *= 0.8;     
        else if(ratio > 0.8) mRate *= 1.5;  
        return mRate;
    }
};
class Payment {
    static int totalTwoWheelerRevenue;
    static int totalFourWheelerRevenue;
    static int totalDisabledRevenue;
    static int totalEVRevenue;
    static int totalBusRevenue;
public:
    Payment(){}
    inline static void addRevenue(int amt,vehicleType vehicle){
        if(vehicle==vehicleType::motorcycle || vehicle==vehicleType::cycle) totalTwoWheelerRevenue+=amt;
        else if(vehicle==vehicleType::fiveSeaterCar || vehicle==vehicleType::sevenSeaterCar) totalFourWheelerRevenue+=amt;
        else if(vehicle==vehicleType::disabled) totalDisabledRevenue+=amt;
        else if(vehicle==vehicleType::EV) totalEVRevenue+=amt;
        else if(vehicle==vehicleType::truck|| vehicle==vehicleType::bus) totalBusRevenue+=amt;
    }
    inline static void showRevenue(){
        cout<<"---Vehicles Revenue--- \n";
        cout<<"Two-Wheeler Revenue: Rs "<<totalTwoWheelerRevenue<<"\n";
        cout<<"Four-Wheeler Revenue: Rs "<<totalFourWheelerRevenue<<"\n";
        cout<<"Disabled vehicle Revenue: Rs "<<totalDisabledRevenue<<"\n";
        cout<<"Bus/Truck Revenue: Rs "<<totalBusRevenue<<"\n";
        cout<<"EV Revenue: Rs "<<totalEVRevenue<<"\n";
    }
    static void saveRevenueToFile(const string &filename) {
        ofstream fout(filename, ios::app);
        time_t now = time(0);
        fout << "---- Revenue Report (" << ctime(&now) << ") ----\n";
        fout << "Two-Wheeler Revenue: Rs " << totalTwoWheelerRevenue << "\n";
        fout << "Four-Wheeler Revenue: Rs " << totalFourWheelerRevenue << "\n";
        fout << "Disabled Vehicle Revenue: Rs " << totalDisabledRevenue << "\n";
        fout << "Bus/Truck Revenue: Rs " << totalBusRevenue << "\n";
        fout << "EV Revenue: Rs " << totalEVRevenue << "\n\n";
        fout.close();
        cout << "Revenue saved to file: " << filename << "\n";
    }
    static void pay(){
        cout<<"1. Cash\n"<<"2. Card\n"<<"3. NetBanking\n"<<"4. Upi\n";
        int paymentType;
        cout<<"Select Option from above for Payment: ";
        cin>>paymentType;
        int amt;
        switch(paymentType){
            case 1:{
                cout<<"Enter Amount: ";
                cin>>amt;
                cout<<"Payment Successful\n";
            break;}
            case 2:{
                cout<<"Enter Card Number: ";
                long long cardnum;
                cin>>cardnum;
                cout<<"Enter Amount: ";
                cin>>amt;
                string cardNum=to_string(cardnum);
                if(cardNum.length()>=13 && cardNum.length()<=19) cout<<"Payment Successful\n";
                else cout<<"Please Enter valid card number\n";
            break;}
            case 3:{
                int bankType; 
                cout<<"1. HDFC\n2. SBI\n3. Axis\n4. ICICI\n5. Bank of Baroda\n 6. Punjab National Banking\n7. IndusInd Bank\n";
                cout<<"Select Option from above for Payment: ";
                cin>>bankType;
                cout<<"Enter Amount: ";
                cin>>amt;
                cout<<"Payment Successful\n";  
            break;}
            case 4:{
                string upiID;
                cout<<"Enter UPI ID: ";
                cin>>upiID;
                cout<<"Enter Amount: ";
                cin>>amt;
                cout<<"Payment Successful\n";
            break;}
        }
    }
};

int Payment::totalTwoWheelerRevenue = 0;
int Payment::totalFourWheelerRevenue = 0;
int Payment::totalDisabledRevenue = 0;
int Payment::totalBusRevenue=0;
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
        else if(choice == 7) { // View Stored Users
    if(users.empty()) { cout << "No users registered yet!\n"; break; }
    cout << "\n--- Stored Users ---\n";
    for(auto &p : users) {
        cout << *(p.second) << "\n";  // Using operator<< instead of manual print
    }
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
