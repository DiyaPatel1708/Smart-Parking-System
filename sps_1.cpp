#include <iostream>
#include <string>
#include <limits>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <sstream>
#include <cctype>

using namespace std;

enum class vehicleType { car, motorcycle, EV, disabled };
enum class spotStatus { free, reserved, occupied, maintenance };
enum class parkingSpotType { standard, premium, ev_charging, motorcycle, disabled };

// ---------------- utility ----------------
bool parseIntInRange(const string &s, int &out, int minv, int maxv) {
    stringstream ss(s);
    if (!(ss >> out)) return false;
    ss >> ws;
    if (!ss.eof()) return false;
    return out >= minv && out <= maxv;
}

bool isAllDigits(const string &s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit((unsigned char)c)) return false;
    return true;
}

bool isEmailValid(const string &email) {
    size_t atpos=email.find('@');
    size_t dotpos=email.find('.', atpos+1);

    if(atpos==string::npos || dotpos==string::npos) {
        return false;
    }
    if(atpos==0 || dotpos==0) {
        return false;
    }
if(atpos==email.size() -1 || dotpos==email.size() -1) {
    return false;
}

if(atpos>dotpos) {
    return false;
}

if(email.find(' ')!=string::npos) {
    return false;
}

return true;

}

bool isBlank(const string &s) {
    for(char c:s) {
        if(!isspace((unsigned char) c)) {
            return false;
        }
    }
    return true;
}

// ---------------- Base Class ----------------
class user {
protected:
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
         vehicleType Type = vehicleType::car, parkingSpotType spotType = parkingSpotType::standard)
        : name(name), userID(userID), contact(contact), email(email), Type(Type), spotType(spotType),
          score(0), useCount(0), rating(0) {
        userCount++;
        entryTime = time(NULL);
    }

    virtual ~user() { }

    virtual void showInfo() const {
        cout << "User: " << name << " | ID: " << userID << " | Contact: " << contact
             << " | Email: " << email << " | Vehicle Type: " << (int)Type
             << " | Rating: " << (rating==0 ? "Not rated" : to_string(rating)+" star(s)")
             << " | Review: " << (review.empty() ? "No review" : review)
             << "\n";
    }

    static int getUserCount() { return userCount; }
    string getID() const { return userID; }

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

    void showScore() const {
        cout << name << " used the system. Usage Count: " << useCount << " | Score: " << score << "\n";
    }

    virtual int getDiscountedRate(int baseRate) const {
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

    user& operator++() {
        score += 10;
        useCount++;
        cout << name << "'s score incremented."
             << " Score: " << score << ", Uses: " << useCount << "\n";
        return *this;
    }
};

int user::userCount = 0;

void displayContact(const user& u) {
    cout << "Contact of " << u.name << " is: " << u.contact << "\n";
}

vehicleType getVehicleType(const user& u) {
    return u.Type;
}

// ---------------- Derived Classes ----------------
class CarUser : public user {
public:
    CarUser(string name, string userID, long long contact, string email = "not_provided@mail.com")
        : user(name, userID, contact, email, vehicleType::car) {}

    void showInfo() const override {
        cout << "[Car User] ";
        user::showInfo();
    }
};

class MotorcycleUser : public user {
public:
    MotorcycleUser(string name, string userID, long long contact, string email = "not_provided@mail.com")
        : user(name, userID, contact, email, vehicleType::motorcycle) {}

    void showInfo() const override {
        cout << "[Motorcycle User] ";
        user::showInfo();
    }
};

class EVUser : public user {
public:
    EVUser(string name, string userID, long long contact, string email = "not_provided@mail.com")
        : user(name, userID, contact, email, vehicleType::EV) {}

    void showInfo() const override {
        cout << "[EV User] ";
        user::showInfo();
    }

    int getDiscountedRate(int baseRate) const override {
        int rate = user::getDiscountedRate(baseRate);
        cout << "EV special discount applied! 5% off.\n";
        return static_cast<int>(rate * 0.95);
    }
};

class DisabledUser : public user {
public:
    DisabledUser(string name, string userID, long long contact, string email = "not_provided@mail.com")
        : user(name, userID, contact, email, vehicleType::disabled) {}

    void showInfo() const override {
        cout << "[Disabled User] ";
        user::showInfo();
    }
};

// ---------------- Dynamic Pricing ----------------
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
            case vehicleType::EV: mRate = 120; if(isCharging) mRate = static_cast<int>(mRate * 1.2); break;
        }
        double ratio = (capacity==0) ? 0.0 : (double)occupied / capacity;
        if(ratio < 0.3) mRate = static_cast<int>(mRate * 0.8);
        else if(ratio > 0.8) mRate = static_cast<int>(mRate * 1.5);
        return mRate;
    }
};

// ---------------- Payment ----------------
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

    static void pay(int amt) { cout<<"Payment Successful || Rs."<<amt<<" in cash.\n"; }
    static void pay(int amt, const string &cardNumber) { cout<<"Payment Successful || Rs."<<amt<<" using Card: "<<cardNumber<<"\n"; }
    static void pay(int amt, const string &upiID, bool isUPI) { if(isUPI) cout<<"Payment Successful || Rs."<<amt<<" using UPI: "<<upiID<<"\n"; }
    static void pay(int amt, const string &accNumber, const string &bankName) { cout<<"Payment Successful || Rs."<<amt<<" using Netbanking | Account: "<<accNumber<<" Bank: "<<bankName<<"\n"; }
};
int Payment::totalTwoWheelerRevenue=0;
int Payment::totalFourWheelerRevenue=0;
int Payment::totalDisabledRevenue=0;
int Payment::totalEVRevenue=0;

// ---------------- MENU ----------------
int main(){
    unordered_map<string, user*> users;
    int choice = 0;

    do {
        cout << "\n========== SMART PARKING MENU ==========\n";
        cout << "1. Register User\n2. Book Parking Spot\n3. Show Total Revenue\n";
        cout << "4. Rate System (1-5 stars)\n5. Add Review\n6. Show User Score\n7. Exit\n";
        cout << "Enter choice: ";
        string line;
        if(!getline(cin, line)) break; 

        if(!parseIntInRange(line, choice, 1, 7)) {
            cout << "Invalid choice! Please enter a number 1-7.\n";
            continue;
        }

        switch(choice) {
            case 1: { // Register user
                string name; 
                
                                while(true) {

                cout << "Enter Name: ";
                getline(cin, name);
                if (name.empty() || isBlank(name)) {
    cout << "Please enter a valid name!\n";
    
}
else{
    break;
}

                }
                string id; 

                while(true) {

                
  
                cout << "Enter User ID: ";
                getline(cin, id);
                if(id.empty() || isBlank(id)) {
                        cout << "Please enter a valid user ID!\n";

                }
                else{
                    break;
                }
            }
                string contactStr;
                while(true){
                    cout << "Enter Contact (10 digits): ";
                    getline(cin, contactStr);
                    if(contactStr.size()==10 && isAllDigits(contactStr)) break;
                    cout << "Invalid contact! Must be exactly 10 digits.\n";
                }
                long long contact = stoll(contactStr);

                string email;
                while (true) {
                cout << "Enter Email: ";
                getline(cin, email);
                if (email.empty()) {
                email = "not_provided@mail.com";
                break;
    }
    if (isEmailValid(email)) break;
    cout << "Invalid email format! Please enter a valid email\n";
}


                cout << "Select Vehicle Type:\n1. Car\n2. Motorcycle\n3. EV\n4. Disabled\n";
                string vline; getline(cin, vline); int vChoice;
                if(!parseIntInRange(vline,vChoice,1,4)) vChoice=1;

                if(users.find(id)!=users.end()){ cout<<"UserID already exists!\n"; break; }

                switch(vChoice){
                    case 1: users[id]=new CarUser(name,id,contact,email); break;
                    case 2: users[id]=new MotorcycleUser(name,id,contact,email); break;
                    case 3: users[id]=new EVUser(name,id,contact,email); break;
                    case 4: users[id]=new DisabledUser(name,id,contact,email); break;
                    default: users[id]=new CarUser(name,id,contact,email); break;
                }
                cout<<"User Registered Successfully!\n";
                users[id]->saveToFile("users.txt");
                break;
            }

            case 2:{ // Book parking
                if(users.empty()){ cout<<"No users registered yet!\n"; break; }
                cout<<"Enter User ID: ";
                string id; getline(cin,id);
                auto it=users.find(id);
                if(it==users.end()){ cout<<"User not found!\n"; break; }
                user* u = it->second;

                int capacity=100, occupied=40;
                vehicleType type = getVehicleType(*u);

                cout<<"\n--- Available Spot Types & Rates ---\n";
                for(int s=1;s<=5;s++){
                    parkingSpotType tempSpot; string spotName; bool isChargingPreview=false;
                    switch(s){
                        case 1: tempSpot=parkingSpotType::standard; spotName="Standard"; break;
                        case 2: tempSpot=parkingSpotType::premium; spotName="Premium"; break;
                        case 3: tempSpot=parkingSpotType::ev_charging; spotName="EV Charging"; isChargingPreview=true; break;
                        case 4: tempSpot=parkingSpotType::motorcycle; spotName="Motorcycle"; break;
                        case 5: tempSpot=parkingSpotType::disabled; spotName="Disabled"; break;
                    }
                    int previewRate=dynamicPrice::calRate(occupied,capacity,type,tempSpot,isChargingPreview);
                    cout<<s<<". "<<spotName<<" Spot -> Rs "<<previewRate<<"\n";
                }

                cout<<"Choose your Spot Type: ";
                string sline; getline(cin,sline); int sChoice;
                parkingSpotType spot = parkingSpotType::standard; bool isCharging=false;
                if(parseIntInRange(sline,sChoice,1,5)){
                    switch(sChoice){
                        case 1: spot=parkingSpotType::standard; break;
                        case 2: spot=parkingSpotType::premium; break;
                        case 3: spot=parkingSpotType::ev_charging; isCharging=true; break;
                        case 4: spot=parkingSpotType::motorcycle; break;
                        case 5: spot=parkingSpotType::disabled; break;
                    }
                } else cout<<"Invalid choice! Defaulting to Standard spot.\n";

                int rate=dynamicPrice::calRate(occupied,capacity,type,spot,isCharging);
                cout<<"Parking Rate for chosen spot: Rs "<<rate<<"\n";

                u->recordUsage();
                int finalRate=u->getDiscountedRate(rate);
                Payment::addRevenue(finalRate,type);

                cout<<"Choose Payment Method:\n1. Cash\n2. Card\n3. UPI\n4. Netbanking\nEnter choice: ";
                string payLine; getline(cin,payLine); int payChoice;
                if(!parseIntInRange(payLine,payChoice,1,4)) { Payment::pay(finalRate); break; }
                switch(payChoice){
                    case 1: Payment::pay(finalRate); break;
                    case 2: { cout<<"Enter Card Number: "; string card; getline(cin,card); Payment::pay(finalRate,card); break; }
                    case 3: { cout<<"Enter UPI ID: "; string upi; getline(cin,upi); Payment::pay(finalRate,upi,true); break; }
                    case 4: { cout<<"Enter Bank Name: "; string bank; getline(cin,bank); cout<<"Enter Account Number: "; string acc; getline(cin,acc); Payment::pay(finalRate,acc,bank); break; }
                }
                break;
            }

            case 3: Payment::showRevenue(); break;

            case 4:{ // Rating
                if(users.empty()){ cout<<"No users registered yet!\n"; break; }
                cout<<"Enter User ID: "; string id; getline(cin,id);
                auto it=users.find(id); if(it==users.end()){ cout<<"User not found!\n"; break; }
                user* u = it->second;
                cout<<"Enter rating (1-5 stars): "; string rline; getline(cin,rline); int r;
                if(!parseIntInRange(rline,r,1,5)){ cout<<"Invalid rating. Skipping.\n"; break; }
                u->setRating(r); u->saveToFile("users.txt"); break;
            }

            case 5:{ // Review
                if(users.empty()){ cout<<"No users registered yet!\n"; break; }
                cout<<"Enter User ID: "; string id; getline(cin,id);
                auto it=users.find(id); if(it==users.end()){ cout<<"User not found!\n"; break; }
                user* u = it->second;
                cout<<"Enter your review: "; string rev; getline(cin,rev);
                u->setReview(rev); u->saveToFile("users.txt"); break;
            }

            case 6:{ // Show score
                if(users.empty()){ cout<<"No users registered yet!\n"; break; }
                cout<<"Enter User ID: "; string id; getline(cin,id);
                auto it=users.find(id); if(it==users.end()){ cout<<"User not found!\n"; break; }
                user* u = it->second;
                u->showScore(); break;
            }

            case 7: cout<<"Exiting... Goodbye!\n"; break;
        }

    } while(choice!=7);

    for(auto &p:users) delete p.second;
    return 0;
}
