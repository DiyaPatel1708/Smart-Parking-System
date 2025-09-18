#include<iostream>
#include<string>
#include<ctime>
#include<vector>
#include<fstream>
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