#include<iostream>
#include<string>
#include<ctime>
#include<vector>
#include<fstream>
using namespace std;

enum class vehicleType{car,motorcycle,EV,disabled};
enum class spotStatus {free,reserved,occupied,maintenance};
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

class user{
private:
    string name;
    string userID;
    long long contact;
    string email;
    vehicleType Type;
    static int userCount;
    time_t entryTime;
public:
    user(string name, string userID, long long contact, string email, vehicleType Type){
        this->name=name; this->userID=userID; this->contact=contact; this->email=email; this->Type=Type; userCount++; entryTime=time(NULL);
    }  
    void showInfo() const{ 
        cout<<"User: "<<name
    <<" | ID: "<<userID
    <<" | Contact: "<<contact
    <<" | Email: "<<email
    <<" | Vehicle Type: "<<to_string(Type)<<"\n";
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
        this->name=name; this->location=location; totalGarages++;
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
        spotID=id; type=t; status=spotStatus::free; reservationTime=0;
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
            cout<<"Spot "<<spotID<<" is not free!\n"; return false; 
        }
        if (!isVehicleCompatibleWithSpot(vehicle, type)){
            cout<<"Vehicle type not suitable for spot "<<spotID<<"!\n"; return false; 
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
        if(!reserve(vehicle)){ return false; }
        reservationTime=reservedUntil;
        cout<<"Reservation for spot "<<spotID<<" is valid until "<<ctime(&reservedUntil);
        return true;
    }
    bool release(){
        if (status==spotStatus::free) { return false; }
        status=spotStatus::free; reservationTime=0;
        cout<<"Spot "<<spotID<<" has been released.\n";
        return true;
    }
    friend void checkSpotDetails(const parkingSpot &spot){
        cout<<"[Friend Function] Spot ID: "<<spot.spotID<<", Status: "<<(int)spot.status<<"\n";
    }
};
class dynamicPrice {
public:
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
        return mRate;
    }
};
class Payment{
    static int totalTwoWheelerRevenue;
    static int totalFourWheelerRevenue;
    static int totalTruckRevenue;
    static int totalEVRevenue;
private:
    Payment() {}
public:
    inline static void addRevenue(int amt,string type){
        if(type=="Bike") totalTwoWheelerRevenue+=amt;
        else if(type=="Car") totalFourWheelerRevenue+=amt;
        else if(type=="Truck") totalTruckRevenue+=amt;
        else if(type=="EV") totalEVRevenue+=amt;
    }
    inline static void showRevenue(){
        cout<<"Two-Wheeler Revenue: Rs "<<totalTwoWheelerRevenue<<"\n";
        cout<<"Four-Wheeler Revenue: Rs "<<totalFourWheelerRevenue<<"\n";
        cout<<"Truck Revenue: Rs "<<totalTruckRevenue<<"\n";
        cout<<"EV Revenue: Rs "<<totalEVRevenue<<"\n";
    }
    static void paym(int amt){
        cout<<"Successfully Paid Rs."<<amt<<" in cash.\n";
    }
    static void pay(int amt,string card){ 
        cout<<"Paid Rs."<<amt<<" using card "<<card<<"\n"; 
    }
    static void pay(int amt,string upi,bool check){ 
        cout<<"Paid Rs."<<amt<<" using UPI ID "<<upi<<"\n"; 
    }
};
int Payment::totalTwoWheelerRevenue=0;
int Payment::totalFourWheelerRevenue=0;
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
    Payment::pay(rate,"1234-5678-9012");
    Payment::pay(rate,"naman@upi",true);
    return 0;
}
