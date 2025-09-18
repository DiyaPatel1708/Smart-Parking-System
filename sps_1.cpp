#include<iostream>
#include<string>
#include<ctime>
#include<vector>
#include<fstream>
using namespace std;

enum class vehicleType{cycle,fiveSeaterCar,sevenSeaterCar,motorcycle,EV,disabled,bus,truck};
enum class spotStatus {free,reserved,occupied,maintenance};
<<<<<<< HEAD
enum class parkingSpotType {standard,premium,ev_charging,motorcycle, disabled};

// ---------------- USER CLASS ----------------
class user {
=======
enum class parkingSpotType {cycle,standard,premium,ev_charging,motorcycle,compact,large,small,family,disabled,bus};
class user{
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
private:
    string name;
    string userID;
    long long contact;
    string email;
    vehicleType Type;
    parkingSpotType spotType;
    static int userCount;
    time_t entryTime;
public:
<<<<<<< HEAD
    user(string name, string userID, long long contact, string email = "not_provided@mail.com", 
         vehicleType Type = vehicleType::fiveSeaterCar, parkingSpotType spotType = parkingSpotType::standard) {
=======
    user(string name, string userID, long long contact, vehicleType Type, string email = "not_provided"){
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
        this->name=name; 
        this->userID=userID; 
        this->contact=contact; 
        this->email=email; 
        this->Type=Type; 
        userCount++; 
        entryTime=time(NULL);
    }  
    void showInfo() const{ 
        cout<<"User: "<<name<<" | ID: "<<userID<<" | Contact: "<<contact<<" | Email: "<<email<<" | Vehicle Type: "<<(int)Type<<"\n";
    }
    static int getUserCount() { 
        return userCount; 
    }
    void saveToFile(const string &filename) const {
        ofstream fout(filename, ios::app);
        fout<<name<<" | "<<userID<<" | "<<contact<<" | "<<email<<" | VehicleType: "<<(int)Type<<" | Entry: "<<ctime(&entryTime);
        fout.close();
    }

    friend void displayContact(const user& u);
};
void displayContact(const user& u){
    cout << "Contact of " << u.name << " is: " << u.contact << "\n";
}
int user::userCount=0;
<<<<<<< HEAD

// ---------------- PRICING ----------------
=======
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

>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
class dynamicPrice {
public:
    static int calRate(int occupied,int capacity,vehicleType type, parkingSpotType spot, bool isCharging=false){ 
        int mRate=0;
        switch(type) {
<<<<<<< HEAD
            case vehicleType::motorcycle: mRate=60; break;
            case vehicleType::fiveSeaterCar:
                if(spot==parkingSpotType::standard) mRate=100;
                else if(spot==parkingSpotType::ev_charging) mRate=150;
                else if(spot==parkingSpotType::premium) mRate=175;
                break;
            case vehicleType::disabled: mRate = 30; break;
            case vehicleType::EV: mRate = 120; if(isCharging) mRate *= 1.2; break;
=======
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
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
        }
        double ratio = (double)occupied / capacity;
        if(ratio < 0.3) mRate *= 0.8;     
        else if(ratio > 0.8) mRate *= 1.5;  
        return mRate;
    }
};
<<<<<<< HEAD

// ---------------- PAYMENT ----------------
class Payment {
=======
class Payment{
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
    static int totalTwoWheelerRevenue;
    static int totalFourWheelerRevenue;
    static int totalDisabledRevenue;
    static int totalEVRevenue;
<<<<<<< HEAD
=======
    static int totalBusRevenue;
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
private:
    Payment(){}
public:
    inline static void addRevenue(int amt,vehicleType vehicle){
<<<<<<< HEAD
        if(vehicle==vehicleType::motorcycle) totalTwoWheelerRevenue+=amt;
        else if(vehicle==vehicleType::fiveSeaterCar) totalFourWheelerRevenue+=amt;
        else if(vehicle==vehicleType::disabled) totalDisabledRevenue+=amt;
        else if(vehicle==vehicleType::EV) totalEVRevenue+=amt;
=======
        if(vehicle==vehicleType::motorcycle || vehicle==vehicleType::cycle) totalTwoWheelerRevenue+=amt;
        else if(vehicle==vehicleType::fiveSeaterCar || vehicle==vehicleType::sevenSeaterCar) totalFourWheelerRevenue+=amt;
        else if(vehicle==vehicleType::disabled) totalDisabledRevenue+=amt;
        else if(vehicle==vehicleType::EV) totalEVRevenue+=amt;
        else if(vehicle==vehicleType::truck|| vehicle==vehicleType::bus) totalBusRevenue+=amt;
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
    }
    inline static void showRevenue(){
        cout<<"---Vehicles Revenue--- \n";
        cout<<"Two-Wheeler Revenue: Rs "<<totalTwoWheelerRevenue<<"\n";
        cout<<"Four-Wheeler Revenue: Rs "<<totalFourWheelerRevenue<<"\n";
        cout<<"Disabled vehicle Revenue: Rs "<<totalDisabledRevenue<<"\n";
<<<<<<< HEAD
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

=======
        cout<<"Bus/Truck Revenue: Rs "<<totalBusRevenue<<"\n";
        cout<<"EV Revenue: Rs "<<totalEVRevenue<<"\n";
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
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
int Payment::totalTwoWheelerRevenue=0;
int Payment::totalFourWheelerRevenue=0;
int Payment::totalDisabledRevenue=0;
int Payment::totalEVRevenue=0;
<<<<<<< HEAD


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
    vehicleType type = vehicleType::fiveSeaterCar;
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
int Payment::totalBusRevenue=0;
int main(){
    parkingSpot* s1 = new parkingSpot("A1", parkingSpotType::standard);
    s1->reserve(vehicleType::motorcycle);
    debugSpot(*s1);
    user* u1 = new user("Naman","U123",9876543210,vehicleType::motorcycle);
    u1->showInfo();
    displayContact(*u1);
    u1->saveToFile("users.txt");
    
    int rate=dynamicPrice::calRate(10,50,vehicleType::motorcycle,parkingSpotType::motorcycle,false);
    cout<<"Rate: "<<rate<<"\n";
    Payment::addRevenue(rate,vehicleType::motorcycle);
    Payment::showRevenue();
    Payment::pay();

    parkingGarage* g1 = new parkingGarage("City Center Garage","Downtown");
    g1->displayInfo();
    delete g1;
    delete s1;
    delete u1;

    return 0;
}
>>>>>>> c4c27a8b4907cc3636f449756b038db472ddc416
