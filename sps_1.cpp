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
class Payment{
    static int totalTwoWheelerRevenue;
    static int totalFourWheelerRevenue;
    static int totalDisabledRevenue;
    static int totalEVRevenue;
    static int totalBusRevenue;
private:
    Payment(){}
public:
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
int Payment::totalTwoWheelerRevenue=0;
int Payment::totalFourWheelerRevenue=0;
int Payment::totalDisabledRevenue=0;
int Payment::totalEVRevenue=0;
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
>>>>>>> 9d1b1d59743e0b575879bc465a3635a3331d301f
