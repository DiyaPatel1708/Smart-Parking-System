#include<iostream>
#include<string>
#include<ctime>
#include<vector>
#include<fstream>
using namespace std;

enum class vehicleType{car,motorcycle,EV,disabled};
enum class spotStatus {free,reserved,occupied,maintenance};
<<<<<<< HEAD
enum class parkingSpotType {standard,premium,ev_charging,motorcycle, disabled};
=======
enum class parkingSpotType {
    standard,    
    premium,     
    ev_charging, 
    disabled,    
    motorcycle  
};
string to_string(vehicleType type) {
    switch (type) {
        case vehicleType::car: return "Car";
        case vehicleType::motorcycle: return "Motorcycle";
        case vehicleType::EV: return "EV";
        case vehicleType::disabled: return "Disabled";
        default: return "Unknown";
    }
}
string to_string(spotStatus status) {
    switch (status) {
        case spotStatus::free: return "Free";
        case spotStatus::reserved: return "Reserved";
        case spotStatus::occupied: return "Occupied";
        case spotStatus::maintenance: return "Maintenance";
        default: return "Unknown";
    }
}
string to_string(parkingSpotType type) {
    switch (type) {
        case parkingSpotType::standard: return "Standard";
        case parkingSpotType::premium: return "Premium";
        case parkingSpotType::ev_charging: return "EV Charging";
        case parkingSpotType::disabled: return "Disabled";
        case parkingSpotType::motorcycle: return "Motorcycle";
        default: return "Unknown";
    }
}

>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
class user{
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
    user(string name, string userID, long long contact, string email, vehicleType Type, parkingSpotType spotType){
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
=======
    user(string name, string userID, long long contact, string email, vehicleType Type){
        this->name=name; this->userID=userID; this->contact=contact; this->email=email; this->Type=Type; userCount++; entryTime=time(NULL);
    }  
    void showInfo() const{ 
        cout<<"User: "<<name
    <<" | ID: "<<userID
    <<" | Contact: "<<contact
    <<" | Email: "<<email
    <<" | Vehicle Type: "<<to_string(Type)<<"\n";
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
    }
    static int getUserCount() { 
        return userCount; 
    }
    void saveToFile(const string &filename) const {
        ofstream fout(filename, ios::app);
        fout<<name<<" | "<<userID<<" | "<<contact<<" | "<<email<<" | VehicleType: "<<(int)Type<<" | Entry: "<<ctime(&entryTime);
        fout.close();
    }
};
int user::userCount=0;
class parkingGarage {
private:
    string name;
    string location;
    static int totalGarages;
public: 
    parkingGarage(const string &name, const string &location){
<<<<<<< HEAD
        this->name=name; 
        this->location=location; 
        totalGarages++;
=======
        this->name=name; this->location=location; totalGarages++;
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
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
<<<<<<< HEAD
        spotID=id; type=t; 
        status=spotStatus::free; 
        reservationTime=0;
=======
        spotID=id; type=t; status=spotStatus::free; reservationTime=0;
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
    }
    string getSpotId() const{ 
        return spotID; 
    }
    spotStatus getStatus() const{ 
        return status; 
    }
    bool isVehicleCompatibleWithSpot(vehicleType vehicle,parkingSpotType spot){
        switch(spot){
            case parkingSpotType::standard: return (vehicle == vehicleType::car || vehicle == vehicleType::disabled);
            case parkingSpotType::premium: return (vehicle == vehicleType::car);
            case parkingSpotType::ev_charging: return (vehicle == vehicleType::EV);
            case parkingSpotType::disabled: return (vehicle == vehicleType::disabled);
            case parkingSpotType::motorcycle: return (vehicle == vehicleType::motorcycle);
            default: return false;
        }
    }
    bool reserve(vehicleType vehicle) {  
        if (status != spotStatus::free) {
<<<<<<< HEAD
            cout<<"Spot "<<spotID<<" is not free!\n"; 
            return false; 
        }
        if (!isVehicleCompatibleWithSpot(vehicle, type)){
            cout<<"Vehicle type not suitable for spot "<<spotID<<"!\n"; 
            return false; 
=======
            cout<<"Spot "<<spotID<<" is not free!\n"; return false; 
        }
        if (!isVehicleCompatibleWithSpot(vehicle, type)){
            cout<<"Vehicle type not suitable for spot "<<spotID<<"!\n"; return false; 
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
        }
        status=spotStatus::reserved;
        cout<<"Spot "<<spotID<<" reserved successfully.\n";
        return true;  
    }
    char getSpotSymbol(parkingSpotType type) {
        switch (type){
            case parkingSpotType::standard: return 'S';
            case parkingSpotType::premium: return 'P';
            case parkingSpotType::ev_charging: return 'E';
            case parkingSpotType::disabled: return 'D';
            case parkingSpotType::motorcycle: return 'M';
            default: return '?';
        }
    } 
    bool reserve(vehicleType vehicle,time_t reservedUntil){
<<<<<<< HEAD
        if(!reserve(vehicle)) return false; 
=======
        if(!reserve(vehicle)){ return false; }
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
        reservationTime=reservedUntil;
        cout<<"Reservation for spot "<<spotID<<" is valid until "<<ctime(&reservedUntil);
        return true;
    }
    bool release(){
<<<<<<< HEAD
        if (status==spotStatus::free) return false; 
=======
        if (status==spotStatus::free) { return false; }
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
        status=spotStatus::free; reservationTime=0;
        cout<<"Spot "<<spotID<<" has been released.\n";
        return true;
    }
<<<<<<< HEAD
    void checkSpotDetails(const parkingSpot &spot){
        cout<<"Spot ID: "<<spot.spotID<<", Status: "<<(int)spot.status<<"\n";
=======
    friend void checkSpotDetails(const parkingSpot &spot){
        cout<<"[Friend Function] Spot ID: "<<spot.spotID<<", Status: "<<(int)spot.status<<"\n";
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
    }
};
class dynamicPrice {
public:
<<<<<<< HEAD
    static int calRate(int occupied,int capacity,vehicleType type, parkingSpotType spot, bool isCharging=false){ 
        int mRate=0;
        switch(type) {
            case vehicleType::motorcycle:{
                mRate=60;
                break;
            }
            case vehicleType::car:{
                if(spot==parkingSpotType::standard) mRate=100;
                else if(spot==parkingSpotType::ev_charging) mRate=150;
                else if(spot==parkingSpotType::premium) mRate=175;
                break;
            }
            case vehicleType::disabled:{
                int mRate=30;
                break;
            }
        }
        double ratio = (double)occupied / capacity;
        if(ratio < 0.3) mRate *= 0.8;     
        else if(ratio > 0.8) mRate *= 1.5; 
        if(type==vehicleType::EV && isCharging) mRate *= 1.2; 
=======
    static int calRate(int occupied, int capacity, string type, bool isCharging=false) {
        int mRate;
        if(type=="Bike") { mRate=20; } 
        else if(type=="Car") { mRate=40; }
        else if(type=="EV") { mRate=60; }
        else if(type=="Truck") { mRate = 100; }
        else { mRate=50; }
        double ratio = (double)occupied / capacity;
        if(ratio < 0.3) { mRate *= 0.8; }     
        else if(ratio > 0.8) { mRate *= 1.5; }
        if(type=="EV" && isCharging) { mRate *= 1.2; }
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
        return mRate;

    }
};
class Payment{
    static int totalTwoWheelerRevenue;
    static int totalFourWheelerRevenue;
    static int totalDisabledRevenue;
    static int totalEVRevenue;
private:
<<<<<<< HEAD
    Payment(){}
public:
    inline static void addRevenue(int amt,vehicleType vehicle){
        if(vehicle==vehicleType::motorcycle) totalTwoWheelerRevenue+=amt;
        else if(vehicle==vehicleType::car) totalFourWheelerRevenue+=amt;
        else if(vehicle==vehicleType::disabled) totalDisabledRevenue+=amt;
        else if(vehicle==vehicleType::EV) totalEVRevenue+=amt;
=======
    Payment() {}
public:
    inline static void addRevenue(int amt,string type){
        if(type=="Bike") totalTwoWheelerRevenue+=amt;
        else if(type=="Car") totalFourWheelerRevenue+=amt;
        else if(type=="Truck") totalTruckRevenue+=amt;
        else if(type=="EV") totalEVRevenue+=amt;
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
    }
    inline static void showRevenue(){
        cout<<"---Vehicles Revenue--- \n";
        cout<<"Two-Wheeler Revenue: Rs "<<totalTwoWheelerRevenue<<"\n";
        cout<<"Four-Wheeler Revenue: Rs "<<totalFourWheelerRevenue<<"\n";
        cout<<"Disabled vehicle Revenue: Rs "<<totalDisabledRevenue<<"\n";
        cout<<"EV Revenue: Rs "<<totalEVRevenue<<"\n";
    }
    static void pay(int amt){
        cout<<"Payment Successful ||"
            <<" Rs."<<amt<<" in cash.\n";
    }
    static void pay(int amt,string card){
        cout<<"Payment Successful ||" 
            <<" Rs."<<amt<<" using card \n";
    }
    static void pay(int amt,string upi,bool check){ 
<<<<<<< HEAD
        cout<<"Payment Successful ||" 
            <<" Rs."<<amt<<" using UPI \n";
=======
        cout<<"Paid Rs."<<amt<<" using UPI ID "<<upi<<"\n"; 
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
    }
};
int Payment::totalTwoWheelerRevenue=0;
int Payment::totalFourWheelerRevenue=0;
<<<<<<< HEAD
int Payment::totalDisabledRevenue=0;
int Payment::totalEVRevenue=0;

int main(){
    parkingSpot s1("A1", parkingSpotType::standard);
    s1.reserve(vehicleType::motorcycle);
    user u1("Naman","U123",9876543210,"naman@mail.com",vehicleType::motorcycle,parkingSpotType::motorcycle);
    u1.showInfo();
    u1.saveToFile("users.txt");
    int rate=dynamicPrice::calRate(10,50,vehicleType::motorcycle,parkingSpotType::motorcycle,false);
    cout<<"Rate: "<<rate<<"\n";
    Payment::addRevenue(rate,vehicleType::motorcycle);
    Payment::showRevenue();
    Payment::pay(rate);
=======
int Payment::totalTruckRevenue=0;
int Payment::totalEVRevenue=0;
int main(){
    parkingSpot s1("A1", parkingSpotType::standard);
    s1.reserve(vehicleType::car);
    s1.release();
    checkSpotDetails(s1);
    user u1("Naman","U123",9876543210,"naman@mail.com",vehicleType::car);
    u1.showInfo();
    u1.saveToFile("users.txt");
    int rate=dynamicPrice::calRate(10,50,"Car");
    cout<<"Rate: "<<rate<<"\n";
    Payment::addRevenue(rate,"Car");
    Payment::showRevenue();
    Payment::paym(rate);
>>>>>>> 035fddfe51d810a456c63c7522dc91de66da7e46
    Payment::pay(rate,"1234-5678-9012");
    Payment::pay(rate,"naman@upi",true);
    return 0;
}
