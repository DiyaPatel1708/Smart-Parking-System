#include<iostream>
#include<string>
#include<ctime>
#include<vector>
#include<fstream>
using namespace std;

enum class vehicleType{car,motorcycle,EV,disabled};
enum class spotStatus {free,reserved,occupied,maintenance};
enum class parkingSpotType {standard,premium,ev_charging,motorcycle, disabled};
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
    user(string name, string userID, long long contact, string email = "not_provided@mail.com", 
     vehicleType Type = vehicleType::car, parkingSpotType spotType = parkingSpotType::standard){
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
};
int user::userCount=0;
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
};
class dynamicPrice {
public:
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
        return mRate;

    }
};
class Payment{
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
    static void pay(int amt){
        cout<<"Payment Successful ||"
            <<" Rs."<<amt<<" in cash.\n";
    }
    static void pay(int amt,string card){
        cout<<"Payment Successful ||" 
            <<" Rs."<<amt<<" using card \n";
    }
    static void pay(int amt,string upi,bool check){ 
        cout<<"Payment Successful ||" 
            <<" Rs."<<amt<<" using UPI \n";
    }
};
int Payment::totalTwoWheelerRevenue=0;
int Payment::totalFourWheelerRevenue=0;
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
    Payment::pay(rate,"1234-5678-9012");
    Payment::pay(rate,"naman@upi",true);
    return 0;
}
