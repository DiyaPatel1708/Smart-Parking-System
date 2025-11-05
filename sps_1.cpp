#include <cctype>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <cmath>

using namespace std;

enum class vehicleType {
  cycle,
  fiveSeaterCar,
  sevenSeaterCar,
  motorcycle,
  EV,
  disabled
};
enum class spotStatus { free, reserved, occupied, maintenance };
enum class parkingSpotType {
  cycle,
  standard,
  premium,
  ev_charging,
  motorcycle,
  compact,
  large,
  small,
  family,
  disabled
};

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
  for (char c : s)
    if (!isdigit((unsigned char)c)) return false;
  return true;
}

bool isEmailValid(const string &email) {
  size_t atpos = email.find('@');
  size_t dotpos = email.find('.', atpos + 1);

  if (atpos == string::npos || dotpos == string::npos) {
    return false;
  }
  if (atpos == 0 || dotpos == 0) {
    return false;
  }
  if (atpos == email.size() - 1 || dotpos == email.size() - 1) {
    return false;
  }

  if (atpos > dotpos) {
    return false;
  }

  if (email.find(' ') != string::npos) {
    return false;
  }

  return true;
}

bool isValidIndianMobile(const string &str) {
  return str.size() == 10 && isAllDigits(str) &&
         (str[0] == '6' || str[0] == '7' || str[0] == '8' || str[0] == '9');
}

bool isValidName(const string &name) {
  if (name.empty()) return false;
  for (char c : name) {
    if (!isalpha(c) && c != ' ' && c != '-') return false;
  }
  return true;
}

bool isBlank(const string &s) {
  for (char c : s) {
    if (!isspace((unsigned char)c)) {
      return false;
    }
  }
  return true;
}

// ---------------- Late Fine Calculator ----------------
class LateFineCalculator {
private:
    static const int BASE_FINE_PER_HOUR = 50;
    static const int MAX_FINE_HOURS = 24;
    static const int DAILY_MAX_FINE = 1000;
    
public:
    static int calculateFine(time_t entryTime, time_t exitTime, int hourlyRate, vehicleType vehicle) {
        double parkedHours = difftime(exitTime, entryTime) / 3600.0;
        int allowedHours = getAllowedHours(vehicle);
        
        if (parkedHours <= allowedHours) {
            return 0; // No fine if within allowed time
        }
        
        int extraHours = static_cast<int>(ceil(parkedHours - allowedHours));
        int fine = 0;
        
        // Calculate base fine
        if (extraHours <= MAX_FINE_HOURS) {
            fine = extraHours * BASE_FINE_PER_HOUR;
        } else {
            // For exceeding 24 hours, charge daily maximum
            int extraDays = (extraHours + 23) / 24; // Ceiling division
            fine = extraDays * DAILY_MAX_FINE;
        }
        
        // Additional penalty based on vehicle type
        fine = applyVehicleTypeMultiplier(fine, vehicle);
        
        // Cap the fine to avoid excessive charges
        fine = min(fine, hourlyRate * 10);
        
        return fine;
    }
    
    static void displayFineRules() {
        cout << "\n=== Late Fine Rules ===\n";
        cout << "• Base Fine: Rs " << BASE_FINE_PER_HOUR << " per hour after allowed time\n";
        cout << "• Maximum Fine: Rs " << DAILY_MAX_FINE << " per day after 24 hours\n";
        cout << "• Allowed Parking Hours:\n";
        cout << "  - Cycles/Motorcycles: 12 hours\n";
        cout << "  - Cars/EVs: 8 hours\n";
        cout << "  - Disabled Vehicles: 24 hours\n";
        cout << "• Discounts:\n";
        cout << "  - Disabled: 50% discount on fines\n";
        cout << "  - EVs: 20% discount on fines\n";
        cout << "  - Cycles: 40% discount on fines\n";
        cout << "======================\n";
    }
    
private:
    static int getAllowedHours(vehicleType vehicle) {
        switch (vehicle) {
            case vehicleType::cycle:
            case vehicleType::motorcycle:
                return 12; // 12 hours for two-wheelers
            case vehicleType::fiveSeaterCar:
            case vehicleType::sevenSeaterCar:
            case vehicleType::EV:
                return 8;  // 8 hours for cars
            case vehicleType::disabled:
                return 24; // 24 hours for disabled
            default:
                return 8;
        }
    }
    
    static int applyVehicleTypeMultiplier(int fine, vehicleType vehicle) {
        switch (vehicle) {
            case vehicleType::disabled:
                return static_cast<int>(fine * 0.5); // 50% discount for disabled
            case vehicleType::EV:
                return static_cast<int>(fine * 0.8); // 20% discount for EV
            case vehicleType::cycle:
                return static_cast<int>(fine * 0.6); // 40% discount for cycles
            default:
                return fine;
        }
    }
};

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
  user(string name, string userID, long long contact, vehicleType Type,
       parkingSpotType spotType, string email = "not_provided") {
    this->name = name;
    this->userID = userID;
    this->contact = contact;
    this->email = email;
    this->Type = Type;
    this->spotType = spotType;
    this->score = 0;
    this->rating = 0;
    this->useCount = 0;
    userCount++;
    entryTime = time(NULL);
  }

  virtual void greet() const {
    cout << "Hello " << name << "! Welcome back.\n";
  }
  
  virtual void displayParkingMessage() const {
    cout << name << ", choose your spot carefully!\n";
  }

  virtual ~user() {}

  virtual void showInfo() const {
    cout << "User: " << name << " | ID: " << userID << " | Contact: " << contact
         << " | Email: " << email << " | Vehicle Type: " << (int)Type
         << " | Rating: "
         << (rating == 0 ? "Not rated" : to_string(rating) + " star(s)")
         << " | Review: " << (review.empty() ? "No review" : review) << "\n";
  }

  static int getUserCount() { return userCount; }
  string getID() const { return userID; }

  void saveToFile(const string &filename) const {
    ofstream fout(filename, ios::app);
    fout << name << " | " << userID << " | " << contact << " | " << email
         << " | VehicleType: " << (int)Type << " | Entry: " << ctime(&entryTime)
         << " | Rating: "
         << (rating == 0 ? "Not rated" : to_string(rating) + " star(s)")
         << " | Review: " << (review.empty() ? "No review" : review) << "\n";
    fout.close();
  }

  static void readFromFile(const string &filename) {
    ifstream fin(filename);
    string line;
    while (getline(fin, line)) {
      cout << line << endl;
    }
    fin.close();
  }

  void recordUsage() {
    useCount++;
    score += 5;
  }

  void showScore() const {
    cout << name << " used the system. Usage Count: " << useCount
         << " | Score: " << score << "\n";
  }

  virtual int getDiscountedRate(int baseRate) const {
    if (score >= 50) {
      cout << "Discount applied! 10% off.\n";
      return static_cast<int>(baseRate * 0.9);
    }
    return baseRate;
  }

  void setRating(int r) {
    if (r < 1 || r > 5) {
      cout << "Invalid rating! Must be 1-5 stars.\n";
      return;
    }
    rating = r;
    cout << "Rating recorded: " << rating << " star(s)\n";
    if (rating == 1 || rating == 2) cout << "Will try better next time. ";
    cout << "Thanks for choosing us!\n";
  }

  void setReview(const string &r) {
    review = r;
    cout << "Review recorded!\n";
  }

  void calculateParkingFee(int baseRate = 100) const {
    time_t exitTime = time(NULL);
    double parkedHours = difftime(exitTime, entryTime) / 3600.0;
    
    int fine = LateFineCalculator::calculateFine(entryTime, exitTime, baseRate, Type);
    
    cout << "\n--- Parking Fee Calculation ---\n";
    cout << "Entry Time: " << ctime(&entryTime);
    cout << "Exit Time: " << ctime(&exitTime);
    cout << "Parked Duration: " << fixed << setprecision(1) << parkedHours << " hours\n";
    cout << "Base Rate: Rs " << baseRate << "\n";
    
    if (fine > 0) {
        cout << "Late Fine: Rs " << fine << "\n";
        cout << "Total Amount: Rs " << (baseRate + fine) << "\n";
        cout << "NOTE: You exceeded the allowed parking time!\n";
    } else {
        cout << "Late Fine: Rs 0 (No fine)\n";
        cout << "Total Amount: Rs " << baseRate << "\n";
    }
    cout << "-------------------------------\n";
  }

  friend void displayContact(const user &u);
  friend vehicleType getVehicleType(const user &u);

  user &operator++() {
    score += 10;
    useCount++;
    cout << name << "'s score incremented." << " Score: " << score
         << ", Uses: " << useCount << "\n";
    return *this;
  }
};

int user::userCount = 0;

void displayContact(const user &u) {
  cout << "Contact of " << u.name << " is: " << u.contact << "\n";
}

vehicleType getVehicleType(const user &u) { return u.Type; }

// ---------------- Derived Classes ----------------
class FCarUser : public user {
 public:
  FCarUser(string name, string userID, long long contact,
           string email = "not_provided")
      : user(name, userID, contact, vehicleType::fiveSeaterCar,
             parkingSpotType::standard, email) {}

  void showInfo() const override {
    cout << "[Five Seater Car User] ";
    user::showInfo();
  }
};

class SCarUser : public user {
 public:
  SCarUser(string name, string userID, long long contact,
           string email = "not_provided")
      : user(name, userID, contact, vehicleType::sevenSeaterCar,
             parkingSpotType::standard, email) {}

  void showInfo() const override {
    cout << "[Seven Seater Car User] ";
    user::showInfo();
  }
};

class MotorcycleUser : public user {
 public:
  MotorcycleUser(string name, string userID, long long contact,
                 string email = "not_provided")
      : user(name, userID, contact, vehicleType::motorcycle,
             parkingSpotType::motorcycle, email) {}

  void showInfo() const override {
    cout << "[Motorcycle User] ";
    user::showInfo();
  }
};

class CycleUser : public user {
 public:
  CycleUser(string name, string userID, long long contact,
            string email = "not_provided")
      : user(name, userID, contact, vehicleType::cycle, parkingSpotType::cycle,
             email) {}

  void showInfo() const override {
    cout << "[Cycle User] ";
    user::showInfo();
  }
};

class EVUser : public user {
 public:
  EVUser(string name, string userID, long long contact,
         string email = "not_provided")
      : user(name, userID, contact, vehicleType::EV,
             parkingSpotType::ev_charging, email) {}

  void showInfo() const override {
        cout << "[EV User] ";
        user::showInfo();
  }

  void greet() const override {
        cout << "Hello " << name << "! Enjoy your EV perks today!\n";
  }

  void displayParkingMessage() const override {
        cout << name << ", select EV charging spot for extra benefits!\n";
  }

  int getDiscountedRate(int baseRate) const override {
    int rate = user::getDiscountedRate(baseRate);
    cout << "EV special discount applied! 5% off.\n";
    return static_cast<int>(rate * 0.95);
  }
};

class DisabledUser : public user {
 public:
  DisabledUser(string name, string userID, long long contact,
               string email = "not_provided@mail.com")
      : user(name, userID, contact, vehicleType::disabled,
             parkingSpotType::disabled, email) {}
  void displayParkingMessage() const override {
        cout << name << ", reserved spots for disabled users available!\n";
  }
  
  void showInfo() const override {
    cout << "[Disabled User] ";
    user::showInfo();
  }
};

// ---------------- Parking Garage ----------------
class parkingGarage {
 private:
  string name;
  string location;
  static int totalGarages;

 public:
  parkingGarage(const string &name, const string &location) {
    this->name = name;
    this->location = location;
    totalGarages++;
  }
  int getTotalGarages() { return totalGarages; }
  void operator()() const {
    cout << "=== Garage Information ===\n";
    cout << "Name: " << name << "\n";
    cout << "Location: " << location << "\n";
    cout << "Total Garages in system: " << totalGarages << "\n";
  }
  ~parkingGarage() { totalGarages--; }
};
int parkingGarage::totalGarages = 0;

// ---------------- Parking Spot ----------------
class parkingSpot {
 private:
  string spotID;
  spotStatus status;
  parkingSpotType type;
  time_t reservationTime;
  int reservationDuration; // in hours

 public:
  parkingSpot(const string &id, parkingSpotType t) {
    spotID = id;
    type = t;
    status = spotStatus::free;
    reservationTime = 0;
    reservationDuration = 0;
  }
  friend void debugSpot(const parkingSpot &ps);

  string getSpotId() const { return spotID; }
  spotStatus getStatus() const { return status; }
  parkingSpotType getType() const { return type; }
  
  bool isVehicleCompatibleWithSpot(vehicleType vehicle, parkingSpotType spot) {
    switch (spot) {
      case parkingSpotType::standard:
        return (vehicle == vehicleType::fiveSeaterCar ||
                vehicle == vehicleType::sevenSeaterCar ||
                vehicle == vehicleType::EV);
      case parkingSpotType::premium:
        return (vehicle == vehicleType::fiveSeaterCar ||
                vehicle == vehicleType::sevenSeaterCar ||
                vehicle == vehicleType::EV);
      case parkingSpotType::ev_charging:
        return (vehicle == vehicleType::EV);
      case parkingSpotType::disabled:
        return (vehicle == vehicleType::disabled);
      case parkingSpotType::motorcycle:
        return (vehicle == vehicleType::motorcycle);
      case parkingSpotType::cycle:
        return (vehicle == vehicleType::cycle);
      case parkingSpotType::large:
        return (vehicle == vehicleType::sevenSeaterCar);
      case parkingSpotType::small:
        return (vehicle == vehicleType::fiveSeaterCar);
      case parkingSpotType::compact:
        return (vehicle == vehicleType::motorcycle ||
                vehicle == vehicleType::cycle);
      case parkingSpotType::family:
        // Family spots allow all vehicle types - no error message here
        return true;
      default:
        return false;
    }
  }
  
  bool reserve(vehicleType vehicle, int durationHours = 1) {
    if (status != spotStatus::free) {
      cout << "Spot " << spotID << " is not free!\n";
      return false;
    }
    
    // Special validation for family spots - minimum 8 hours
    // Check this FIRST before vehicle compatibility
    if (type == parkingSpotType::family && durationHours < 8) {
      cout << "ERROR: Family spots require minimum 8-hour booking!\n";
      cout << "You requested " << durationHours << " hours. Please book for at least 8 hours.\n";
      return false; // Return immediately, don't check vehicle compatibility
    }
    
    // Validate vehicle type compatibility
    if (!isVehicleCompatibleWithSpot(vehicle, type)) {
      cout << "Vehicle type not suitable for spot " << spotID << "!\n";
      cout << "This spot is for: ";
      switch(type) {
        case parkingSpotType::standard: cout << "Standard Cars/EVs"; break;
        case parkingSpotType::premium: cout << "Premium Cars/EVs"; break;
        case parkingSpotType::ev_charging: cout << "EV Charging"; break;
        case parkingSpotType::disabled: cout << "Disabled Vehicles"; break;
        case parkingSpotType::motorcycle: cout << "Motorcycles"; break;
        case parkingSpotType::cycle: cout << "Cycles"; break;
        case parkingSpotType::large: cout << "Large Cars (7-seater)"; break;
        case parkingSpotType::small: cout << "Small Cars (5-seater)"; break;
        case parkingSpotType::compact: cout << "Compact (Motorcycles/Cycles)"; break;
        case parkingSpotType::family: cout << "Family (All vehicles)"; break;
      }
      cout << "\n";
      return false;
    }
    
    status = spotStatus::reserved;
    reservationDuration = durationHours;
    cout << "Spot " << spotID << " reserved successfully";
    
    if (type == parkingSpotType::family) {
      cout << " (Family spot - " << durationHours << " hour booking)";
    } else {
      cout << " for " << durationHours << " hours";
    }
    cout << ".\n";
    
    return true;
  }
  
  bool reserve(vehicleType vehicle, time_t reservedUntil) {
    if (!reserve(vehicle, 1)) return false;
    reservationTime = reservedUntil;
    cout << "Reservation for spot " << spotID << " is valid until "
         << ctime(&reservedUntil);
    return true;
  }
  
  bool release() {
    if (status == spotStatus::free) return false;
    status = spotStatus::free;
    reservationTime = 0;
    reservationDuration = 0;
    cout << "Spot " << spotID << " has been released.\n";
    return true;
  }
  
  bool setMaintenance() {
    if (status == spotStatus::occupied) {
      cout << "Cannot put occupied spot " << spotID << " under maintenance!\n";
      return false;
    }
    status = spotStatus::maintenance;
    cout << "Spot " << spotID << " is now under maintenance.\n";
    return true;
  }
  
  bool removeMaintenance() {
    if (status != spotStatus::maintenance) {
      cout << "Spot " << spotID << " is not under maintenance!\n";
      return false;
    }
    status = spotStatus::free;
    cout << "Spot " << spotID << " maintenance completed and is now available.\n";
    return true;
  }
  
  void checkSpotDetails(const parkingSpot &spot) {
    cout << "Spot ID: " << spot.spotID << ", Status: " << (int)spot.status
         << "\n";
  }
  
  parkingSpot &operator++() {
    if (status == spotStatus::reserved) {
      status = spotStatus::occupied;
      cout << "Spot " << spotID << " is now occupied.\n";
    }
    return *this;
  }
  
  parkingSpot &operator--() {
    if (status == spotStatus::occupied || status == spotStatus::reserved) {
      status = spotStatus::free;
      reservationTime = 0;
      reservationDuration = 0;
      cout << "Spot " << spotID << " is now free.\n";
    }
    return *this;
  }
};

void debugSpot(const parkingSpot &ps) {
  cout << "[DEBUG] Spot " << ps.spotID << " | Status: " << (int)ps.status
       << "\n";
}

// ---------------- Parking Map Manager ----------------
class ParkingMapManager {
private:
    vector<vector<parkingSpot*>> spotsGrid;
    unordered_map<string, parkingSpot*> spotMap;
    const int ROWS = 6;
    const int COLS = 12;

public:
    ParkingMapManager() {
        initializeSpots();
    }
    
    ~ParkingMapManager() {
        for (auto& row : spotsGrid) {
            for (auto spot : row) {
                delete spot;
            }
        }
    }
    
    void initializeSpots() {
        // Create a 6x12 grid with ALL spot types distributed properly
        spotsGrid.resize(ROWS, vector<parkingSpot*>(COLS));
        
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                string spotID = generateSpotID(i, j);
                parkingSpotType type = determineSpotType(i, j);
                spotsGrid[i][j] = new parkingSpot(spotID, type);
                spotMap[spotID] = spotsGrid[i][j];
            }
        }
    }
    
    parkingSpotType determineSpotType(int row, int col) {
        // Define elevator locations (columns 2, 7, 12)
        vector<int> elevatorColumns = {1, 6, 11}; // 0-based indexing
        
        // Premium spots near elevators (first 2 rows)
        for (int elevatorCol : elevatorColumns) {
            if (abs(col - elevatorCol) <= 1 && row < 2) {
                return parkingSpotType::premium;
            }
        }
        
        // Disabled spots prioritized near elevators (first 3 rows)
        for (int elevatorCol : elevatorColumns) {
            if (abs(col - elevatorCol) <= 2 && row < 3) {
                return parkingSpotType::disabled;
            }
        }
        
        // EV charging spots near elevators
        for (int elevatorCol : elevatorColumns) {
            if (abs(col - elevatorCol) <= 1 && row >= 2 && row < 4) {
                return parkingSpotType::ev_charging;
            }
        }
        
        // Family spots (dedicated area with minimum 8-hour booking)
        if (row >= 3 && row <= 5 && col >= 4 && col <= 8) {
            return parkingSpotType::family;
        }
        
        // Large spots for 7-seater cars
        if (row >= 4 && col >= 2 && col <= 10) {
            return parkingSpotType::large;
        }
        
        // Small spots for 5-seater cars
        if (row >= 5 && col >= 1 && col <= 11) {
            return parkingSpotType::small;
        }
        
        // Motorcycle spots
        if (row >= 5 && (col <= 2 || col >= 10)) {
            return parkingSpotType::motorcycle;
        }
        
        // Cycle spots
        if (row >= 5 && col >= 3 && col <= 9) {
            return parkingSpotType::cycle;
        }
        
        // Compact spots
        if (row >= 4 && (col <= 1 || col >= 11)) {
            return parkingSpotType::compact;
        }
        
        // Standard spots for remaining areas
        if (row >= 2 && row <= 4) {
            return parkingSpotType::standard;
        }
        
        // Default to standard
        return parkingSpotType::standard;
    }
    
    string generateSpotID(int row, int col) {
        char rowChar = 'A' + row;
        return string(1, rowChar) + to_string(col + 1);
    }
    
    void displayParkingMap() {
        cout << "\n" << string(100, '=') << "\n";
        cout << "                           SMART PARKING MAP - COMPACT LAYOUT\n";
        cout << string(100, '=') << "\n\n";
        
        // Display garage features
        cout << " GARAGE FEATURES:\n";
        cout << " • +--+ Elevators located near columns 2, 7, 12 (Premium & Disabled spots prioritized)\n";
        cout << " •  Premium spots: Enhanced security, CCTV coverage, near elevators\n";
        cout << " •  Disabled spots: Extra wide, ramps, closest to elevators\n";
        cout << " •  Standard spots: Better lighting, more space than normal\n";
        cout << " •  EV Charging: Near elevators with charging stations\n";
        cout << " •  Family spots: Minimum 8-hour booking, central location\n";
        cout << " •  Large spots: For 7-seater cars, extra width\n";
        cout << " •  Small spots: For 5-seater cars, optimized space\n";
        cout << " •  Motorcycle spots: Dedicated for motorcycles\n";
        cout << " •  Cycle spots: Dedicated for cycles\n";
        cout << " •  Compact spots: For motorcycles and cycles\n\n";
        
        cout << " LEGEND:\n";
        cout << " [ ] Free        [R] Reserved     [X] Occupied     [M] Maintenance\n";
        cout << " PRM-Premium     DIS-Disabled     EV-EV Charging   STD-Standard\n";
        cout << " FAM-Family      LRG-Large        SML-Small        CMP-Compact\n";
        cout << " MC-Motorcycle   CYC-Cycle        +--+ Elevator\n";
        cout << " Note: Family spots require minimum 8-hour booking\n\n";
        
        // Display COMPACT header with 6 columns (pairs of original columns)
        cout << "        ";
        cout << setw(13) << left << "Col 1 & 2";
        cout << setw(13) << "Col 3 & 4"; 
        cout << setw(13) << "Col 5 & 6";
        cout << setw(14) << "Col 7 & 8";
        cout << setw(15) << "Col 9 & 10";
        cout << setw(16) << "Col 11 & 12";
        cout << "\n";
        
        cout << "        ";
        cout << string(100, '-') << "\n";
        
        // Display each row with spots grouped in pairs
        for (int i = 0; i < ROWS; i++) {
            cout << "Row " << char('A' + i) << "  ";
            
            // Display spots in pairs (2 spots per group)
            for (int group = 0; group < 6; group++) {
                int col1 = group * 2;
                int col2 = group * 2 + 1;
                
                parkingSpot* spot1 = spotsGrid[i][col1];
                parkingSpot* spot2 = spotsGrid[i][col2];
                
                string status1 = getStatusSymbol(spot1->getStatus());
                string status2 = getStatusSymbol(spot2->getStatus());
                string type1 = getShortTypeSymbol(spot1->getType());
                string type2 = getShortTypeSymbol(spot2->getType());
                string spotId1 = spot1->getSpotId();
                string spotId2 = spot2->getSpotId();
                
                // Format: [Status]Type SpotID1 SpotID2
                cout << "[" << status1 << "]" << type1 << " " << spotId1 << " " << spotId2 << "  ";
            }
            cout << "\n";
            
            // Add separator between rows with elevator markers
            if (i < ROWS - 1) {
                cout << "        ";
                for (int group = 0; group < 6; group++) {
                    int col1 = group * 2;
                    // Check if this group contains an elevator column (2, 7, 12)
                    if (col1 == 1 || col1 == 6 || col1 == 11) {
                        cout << "_______+--+_______ ";
                    }
                }
                cout << "\n";
            }
        }
        
        cout << "        ";
        cout << string(100, '-') << "\n";
        
        // Mark elevator locations clearly
        cout << "+--+ ELEVATOR LOCATIONS: Between Columns 1-2, 6-7, 11-12\n";
        
        // Display parking availability status
        displayParkingAvailability();
        
        cout << "\n" << string(100, '=') << "\n";
    }
    
    void displayParkingAvailability() {
        cout << "\nPARKING AVAILABILITY STATUS:\n";
        
        bool allFull = true;
        
        // Check availability for each vehicle type
        vector<vehicleType> vehicleTypes = {
            vehicleType::cycle, vehicleType::motorcycle, vehicleType::fiveSeaterCar,
            vehicleType::sevenSeaterCar, vehicleType::EV, vehicleType::disabled
        };
        
        for (auto vehicle : vehicleTypes) {
            vector<string> availableSpots = getAvailableSpotsForVehicle(vehicle);
            if (availableSpots.empty()) {
                displayParkingFullMessage(vehicle);
            } else {
                allFull = false;
                cout << " ";
                switch (vehicle) {
                    case vehicleType::cycle: cout << "Cycles: " << availableSpots.size() << " spots available"; break;
                    case vehicleType::motorcycle: cout << "Motorcycles: " << availableSpots.size() << " spots available"; break;
                    case vehicleType::fiveSeaterCar: cout << "5-Seater Cars: " << availableSpots.size() << " spots available"; break;
                    case vehicleType::sevenSeaterCar: cout << "7-Seater Cars: " << availableSpots.size() << " spots available"; break;
                    case vehicleType::EV: cout << "EVs: " << availableSpots.size() << " spots available"; break;
                    case vehicleType::disabled: cout << "Disabled: " << availableSpots.size() << " spots available"; break;
                }
                cout << "\n";
            }
        }
        
        if (allFull) {
            cout << "\nXXX PARKING GARAGE COMPLETELY FULL XXX!\n";
            cout << "No available spots for any vehicle type. Please check back later.\n";
        }
        
        // Display maintenance spots
        int maintenanceCount = getMaintenanceSpots().size();
        if (maintenanceCount > 0) {
            cout << "\n MAINTENANCE: " << maintenanceCount << " spots under repair\n";
        }
    }
    
    void displayParkingFullMessage(vehicleType vehicle) {
        cout << "XXX ";
        switch (vehicle) {
            case vehicleType::cycle:
                cout << "CYCLE PARKING FULL: No available cycle spots";
                break;
            case vehicleType::motorcycle:
                cout << "MOTORCYCLE PARKING FULL: No available motorcycle spots";
                break;
            case vehicleType::fiveSeaterCar:
                cout << "5-SEATER CAR PARKING FULL: No available spots for 5-seater cars";
                break;
            case vehicleType::sevenSeaterCar:
                cout << "7-SEATER CAR PARKING FULL: No available spots for 7-seater cars";
                break;
            case vehicleType::EV:
                cout << "EV PARKING FULL: No available EV charging spots";
                break;
            case vehicleType::disabled:
                cout << "DISABLED PARKING FULL: No available disabled spots";
                break;
        }
        cout << "\n";
    }
    
    string getStatusSymbol(spotStatus status) {
        switch (status) {
            case spotStatus::free: return " ";
            case spotStatus::reserved: return "R";
            case spotStatus::occupied: return "X";
            case spotStatus::maintenance: return "M";
            default: return "?";
        }
    }
    
    string getShortTypeSymbol(parkingSpotType type) {
        switch (type) {
            case parkingSpotType::standard: return "STD";
            case parkingSpotType::premium: return "PRM";
            case parkingSpotType::ev_charging: return "EV ";
            case parkingSpotType::motorcycle: return "MC ";
            case parkingSpotType::compact: return "CMP";
            case parkingSpotType::disabled: return "DIS";
            case parkingSpotType::family: return "FAM";
            case parkingSpotType::large: return "LRG";
            case parkingSpotType::small: return "SML";
            case parkingSpotType::cycle: return "CYC";
            default: return "???";
        }
    }
    
    parkingSpot* getSpotById(const string& spotId) {
        auto it = spotMap.find(spotId);
        if (it != spotMap.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    bool reserveSpot(const string& spotId, vehicleType vehicle, int durationHours = 1) {
        parkingSpot* spot = getSpotById(spotId);
        if (!spot) {
            cout << "ERROR: Spot ID '" << spotId << "' not found!\n";
            cout << "Please enter a valid Spot ID from the map (e.g., A1, B2, C3)\n";
            return false;
        }
        
        if (spot->getStatus() != spotStatus::free) {
            cout << "ERROR: Spot " << spotId << " is not available! Current status: ";
            switch (spot->getStatus()) {
                case spotStatus::reserved: cout << "Reserved\n"; break;
                case spotStatus::occupied: cout << "Occupied\n"; break;
                case spotStatus::maintenance: cout << "Maintenance\n"; break;
                default: cout << "Unknown\n";
            }
            return false;
        }
        
        if (!spot->isVehicleCompatibleWithSpot(vehicle, spot->getType())) {
            cout << "ERROR: Your vehicle type is not compatible with spot " << spotId << "!\n";
            cout << "This spot is for " << getShortTypeSymbol(spot->getType()) << " type vehicles.\n";
            cout << "Your vehicle type: ";
            switch (vehicle) {
                case vehicleType::cycle: cout << "Cycle"; break;
                case vehicleType::motorcycle: cout << "Motorcycle"; break;
                case vehicleType::fiveSeaterCar: cout << "5-Seater Car"; break;
                case vehicleType::sevenSeaterCar: cout << "7-Seater Car"; break;
                case vehicleType::EV: cout << "Electric Vehicle"; break;
                case vehicleType::disabled: cout << "Disabled Vehicle"; break;
            }
            cout << "\n";
            return false;
        }
        
        // The family spot duration validation will happen inside spot->reserve()
        return spot->reserve(vehicle, durationHours);
    }
    
    vector<string> getAvailableSpotsForVehicle(vehicleType vehicle) {
        vector<string> availableSpots;
        for (const auto& pair : spotMap) {
            if (pair.second->getStatus() == spotStatus::free && 
                pair.second->isVehicleCompatibleWithSpot(vehicle, pair.second->getType())) {
                availableSpots.push_back(pair.first);
            }
        }
        return availableSpots;
    }
    
    bool releaseSpot(const string& spotId) {
        parkingSpot* spot = getSpotById(spotId);
        if (!spot) {
            cout << "ERROR: Spot ID '" << spotId << "' not found!\n";
            cout << "Please enter a valid Spot ID from the map (e.g., A1, B2, C3)\n";
            return false;
        }
        
        if (spot->getStatus() == spotStatus::free) {
            cout << "Spot " << spotId << " is already free!\n";
            return false;
        }
        
        return spot->release();
    }
    
    bool setSpotMaintenance(const string& spotId) {
        parkingSpot* spot = getSpotById(spotId);
        if (!spot) {
            cout << "ERROR: Spot ID '" << spotId << "' not found!\n";
            cout << "Please enter a valid Spot ID from the map (e.g., A1, B2, C3)\n";
            return false;
        }
        
        return spot->setMaintenance();
    }
    
    bool removeSpotMaintenance(const string& spotId) {
        parkingSpot* spot = getSpotById(spotId);
        if (!spot) {
            cout << "ERROR: Spot ID '" << spotId << "' not found!\n";
            cout << "Please enter a valid Spot ID from the map (e.g., A1, B2, C3)\n";
            return false;
        }
        
        return spot->removeMaintenance();
    }
    
    vector<string> getMaintenanceSpots() {
        vector<string> maintenanceSpots;
        for (const auto& pair : spotMap) {
            if (pair.second->getStatus() == spotStatus::maintenance) {
                maintenanceSpots.push_back(pair.first);
            }
        }
        return maintenanceSpots;
    }
    
    void displayMaintenanceSpots() {
        vector<string> maintenanceSpots = getMaintenanceSpots();
        if (maintenanceSpots.empty()) {
            cout << "No spots are currently under maintenance.\n";
        } else {
            cout << "\n SPOTS UNDER MAINTENANCE:\n";
            for (const string& spotId : maintenanceSpots) {
                parkingSpot* spot = getSpotById(spotId);
                cout << "• " << spotId << " (" << getShortTypeSymbol(spot->getType()) << ")\n";
            }
        }
    }
};

// ---------------- Dynamic Pricing ----------------
class dynamicPrice {
 public:
  static int calRate(int occupied, int capacity, vehicleType type,
                     parkingSpotType spot, bool isCharging = false) {
    int mRate = 0;
    switch (type) {
      case vehicleType::cycle: {
        if (spot == parkingSpotType::compact)
          mRate = 20;
        else if (spot == parkingSpotType::cycle)
          mRate = 30;
        else if (spot == parkingSpotType::family)
          mRate = 5;
        break;
      }
      case vehicleType::motorcycle: {
        if (spot == parkingSpotType::compact)
          mRate = 45;
        else if (spot == parkingSpotType::motorcycle)
          mRate = 50;
        else if (spot == parkingSpotType::family)
          mRate = 10;
        break;
      }
      case vehicleType::fiveSeaterCar: {
        if (spot == parkingSpotType::standard)
          mRate = 100;
        else if (spot == parkingSpotType::ev_charging)
          mRate = 150;
        else if (spot == parkingSpotType::premium)
          mRate = 175;
        else if (spot == parkingSpotType::small)
          mRate = 85;
        else if (spot == parkingSpotType::family)
          mRate = 10;
        break;
      }
      case vehicleType::sevenSeaterCar: {
        if (spot == parkingSpotType::standard)
          mRate = 140;
        else if (spot == parkingSpotType::ev_charging)
          mRate = 190;
        else if (spot == parkingSpotType::premium)
          mRate = 215;
        else if (spot == parkingSpotType::large)
          mRate = 200;
        else if (spot == parkingSpotType::family)
          mRate = 15;
        break;
      }
      case vehicleType::disabled: {
        if (spot == parkingSpotType::disabled)
          mRate = 30;
        else if (spot == parkingSpotType::family)
          mRate = 5;
        break;
      }
      case vehicleType::EV: {
        if (spot == parkingSpotType::premium)
          mRate = 175;
        else if (spot == parkingSpotType::standard)
          mRate = 140;
        else if (spot == parkingSpotType::ev_charging)
          mRate = 190;
        else if (spot == parkingSpotType::family)
          mRate = 14;
        if (isCharging) mRate = static_cast<int>(mRate * 1.2);
        break;
      }
    }
    double ratio = (capacity == 0) ? 0.0 : (double)occupied / capacity;
    if (ratio < 0.3)
      mRate = static_cast<int>(mRate * 0.8);
    else if (ratio > 0.8)
      mRate = static_cast<int>(mRate * 1.5);
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
  Payment() {}

 public:
  inline static void addRevenue(int amt, vehicleType vehicle) {
    if (vehicle == vehicleType::motorcycle || vehicle == vehicleType::cycle)
      totalTwoWheelerRevenue += amt;
    else if (vehicle == vehicleType::fiveSeaterCar ||
             vehicle == vehicleType::sevenSeaterCar)
      totalFourWheelerRevenue += amt;
    else if (vehicle == vehicleType::disabled)
      totalDisabledRevenue += amt;
    else if (vehicle == vehicleType::EV)
      totalEVRevenue += amt;
  }
  inline static void showRevenue() {
    cout << "---Vehicles Revenue--- \n";
    cout << "Two-Wheeler Revenue: Rs " << totalTwoWheelerRevenue << "\n";
    cout << "Seven/Five Seater Car Revenue: Rs " << totalFourWheelerRevenue
         << "\n";
    cout << "Disabled vehicle Revenue: Rs " << totalDisabledRevenue << "\n";
    cout << "EV Revenue: Rs " << totalEVRevenue << "\n";
  }
  static void saveRevenueToFile(const string &filename) {
    ofstream fout(filename, ios::app);
    time_t now = time(0);
    fout << "---- Revenue Report (" << ctime(&now) << ") ----\n";
    fout << "Two-Wheeler Revenue: Rs " << totalTwoWheelerRevenue << "\n";
    fout << "Seven/Five Seater Car Revenue: Rs " << totalFourWheelerRevenue
         << "\n";
    fout << "Disabled Vehicle Revenue: Rs " << totalDisabledRevenue << "\n";
    fout << "EV Revenue: Rs " << totalEVRevenue << "\n\n";
    fout.close();
  }

  static void pay(int amt) {
    cout << "Payment Successful || Rs." << amt << " in cash.\n";
  }
  static void pay(int amt, const long long &cardNumber) {
    cout << "Payment Successful || Rs." << amt << "\n";
  }
  static void pay(int amt, const string &upiID, bool isUPI) {
    if (isUPI) cout << "Payment Successful || Rs." << amt << " using UPI\n";
  }
  static void pay(int amt, const long long &accNumber, const int &bankName) {
    cout << "Payment Successful || Rs." << amt
         << " using Netbanking | Bank: " << bankName << "\n";
  }
};
int Payment::totalTwoWheelerRevenue = 0;
int Payment::totalFourWheelerRevenue = 0;
int Payment::totalDisabledRevenue = 0;
int Payment::totalEVRevenue = 0;

void loadUsers(unordered_map<string, user *> &users, const string &filename) {
    ifstream fin(filename);
    if (!fin) {
        return;
    }

    auto trim = [](string &s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        size_t end = s.find_last_not_of(" \t\n\r");
        if (start == string::npos || end == string::npos)
            s = "";
        else
            s = s.substr(start, end - start + 1);
    };

    string line;
    while (getline(fin, line)) {
        trim(line);
        if (line.empty()) continue;

        vector<string> fields;
        string temp;
        stringstream ss(line);

        while (getline(ss, temp, '|')) {
            trim(temp);
            fields.push_back(temp);
        }

        if (fields.size() < 5) {
            continue;
        }

        string name = fields[0];
        string userID = fields[1];
        long long contact = 0;

        try {
            contact = stoll(fields[2]);
        } catch (...) {
            cout << "[WARN] Invalid contact in line: " << line << "\n";
            continue;
        }

        string email = fields[3];

        int vTypeInt = -1;
        size_t pos = fields[4].find(':');
        if (pos != string::npos) {
            try {
                vTypeInt = stoi(fields[4].substr(pos + 1));
            } catch (...) {
                cout << "[WARN] Invalid vehicle type in line: " << line << "\n";
                continue;
            }
        } else {
            cout << "[WARN] Vehicle type missing in line: " << line << "\n";
            continue;
        }

        if (users.find(userID) != users.end()) {
            continue;
        }

        switch (vTypeInt) {
            case 0: users[userID] = new CycleUser(name, userID, contact, email); break;
            case 1: users[userID] = new FCarUser(name, userID, contact, email); break;
            case 2: users[userID] = new SCarUser(name, userID, contact, email); break;
            case 3: users[userID] = new MotorcycleUser(name, userID, contact, email); break;
            case 4: users[userID] = new EVUser(name, userID, contact, email); break;
            case 5: users[userID] = new DisabledUser(name, userID, contact, email); break;
            default:
                continue;
        }
    }

    fin.close();
}

// ---------------- MENU ----------------
int main() {
  unordered_map<string, user *> users;
  loadUsers(users, "users.txt");  
  ParkingMapManager parkingMap;
  
  int choice = 0;

  do {
    cout << "\n========== SMART PARKING MENU ==========\n";
    cout << "1. Register User\n2. Book Parking Spot\n3. Show Total Revenue\n";
    cout << "4. Rate System (1-5 stars)\n5. Add Review\n6. Show User Score\n";
    cout << "7. View Stored Users\n8. View Transactions\n9. View Parking Map\n";
    cout << "10. Calculate Parking Fee with Late Fine\n11. View Fine Rules\n";
    cout << "12. Release Spot\n13. Maintenance Management\n14. Exit\n";
    cout << "Enter choice: ";
    string line;
    if (!getline(cin, line)) break;

    if (!parseIntInRange(line, choice, 1, 14)) {
      cout << "Invalid choice! Please enter a number 1-14.\n";
      continue;
    }

    switch (choice) {
      case 1: {  // Register user
        string name;

        while (true) {
          cout << "Enter Name: ";
          getline(cin, name);
          if (name.empty() || isBlank(name)) {
            cout << "Please enter a valid name!\n";
          }
          if (isValidName(name))
            break;
          else
            cout << "Invalid name! Only letters, spaces, and hyphens are "
                    "allowed.\n";
        }
        string id;

        while (true) {
          cout << "Enter User ID: ";
          getline(cin, id);
          if (id.empty() || isBlank(id)) {
            cout << "Please enter a valid user ID!\n";

          } else {
            break;
          }
        }

        string contactStr;
        while (true) {
          cout << "Enter Contact (10 digits): ";
          getline(cin, contactStr);
          if (contactStr.size() == 10 && isAllDigits(contactStr) && isValidIndianMobile(contactStr)) break;
          cout << "Invalid contact! Must be exactly 10 digits starting with 6, "
                  "7, 8, or 9.\n";
        }
        long long contact = stoll(contactStr);

        string email;
        while (true) {
          cout << "Enter Email: ";
          getline(cin, email);
          if (email.empty()) {
            email = "not_provided";
            break;
          }
          if (isEmailValid(email)) break;
          cout << "Invalid email format! Please enter a valid email\n";
        }
        int vChoice = 0;
        string vline;

        do {
          cout << "Select Vehicle Type:\n1. Five Seater Car\n2. Seven Seater "
                  "Car\n3. Motorcycle\n4. Cycle\n5. EV\n6. Disabled\n";
          getline(cin, vline);
          if (!parseIntInRange(vline, vChoice, 1, 6)) {
            cout << "Invalid choice! Please enter a number between 1 and 6: ";
            vChoice = 0;
          }
        } while (vChoice < 1 || vChoice > 6);

        if (users.find(id) != users.end()) {
          cout << "UserID already exists!\n";
          break;
        }

        switch (vChoice) {
          case 1:
            users[id] = new FCarUser(name, id, contact, email);
            cout << "User Registered Successfully!\n";
            break;
          case 2:
            users[id] = new SCarUser(name, id, contact, email);
            cout << "User Registered Successfully!\n";
            break;
          case 3:
            users[id] = new MotorcycleUser(name, id, contact, email);
            cout << "User Registered Successfully!\n";
            break;
          case 4:
            users[id] = new CycleUser(name, id, contact, email);
            cout << "User Registered Successfully!\n";
            break;
          case 5:
            users[id] = new EVUser(name, id, contact, email);
            cout << "User Registered Successfully!\n";
            break;
          case 6:
            users[id] = new DisabledUser(name, id, contact, email);
            cout << "User Registered Successfully!\n";
            break;
        }

        users[id]->saveToFile("users.txt");
        break;
      }

      case 2: {  // Book parking
        if (users.empty()) {
          cout << "No users registered yet!\n";
          break;
        }
        cout << "Enter User ID: ";
        string id;
        getline(cin, id);
        auto it = users.find(id);
        if (it == users.end()) {
          cout << "User not found!\n";
          break;
        }
        user *u = it->second;
        u->greet();
        u->displayParkingMessage();
        
        // Show parking map first
        parkingMap.displayParkingMap();
        
        vehicleType type = getVehicleType(*u);
        
        // Show available spots for this vehicle type
        vector<string> availableSpots = parkingMap.getAvailableSpotsForVehicle(type);
        
        if (availableSpots.empty()) {
            cout << "\nXXX PARKING FULL for your vehicle type XXX!\n";
            parkingMap.displayParkingFullMessage(type);
            cout << "Please check back later or try a different vehicle type.\n";
            break;
        }
        
        cout << "\nAvailable spots for your vehicle type: ";
        for (size_t i = 0; i < availableSpots.size(); i++) {
            cout << availableSpots[i];
            if (i < availableSpots.size() - 1) cout << ", ";
        }
        cout << "\n";
        
        // Get spot ID from user
        string spotId;
        cout << "\nEnter the Spot ID you want to book (e.g., A1, B3, C5): ";
        getline(cin, spotId);
        
        // Convert to uppercase for consistency
        for (char &c : spotId) {
            c = toupper(c);
        }
        
        // Validate spot ID format (letter followed by number)
        if (spotId.length() < 2 || !isalpha(spotId[0]) || !isdigit(spotId[1])) {
            cout << "ERROR: Invalid spot ID format! Please use format like A1, B2, etc.\n";
            break;
        }
        
        // Check if spot exists
        parkingSpot* selectedSpot = parkingMap.getSpotById(spotId);
        if (!selectedSpot) {
            cout << "ERROR: Spot ID '" << spotId << "' not found!\n";
            cout << "Please enter a valid Spot ID from the map (e.g., A1, B2, C3)\n";
            break;
        }
        
        // Get booking duration with special handling for family spots
        int durationHours = 1;
        if (selectedSpot->getType() == parkingSpotType::family) {
            cout << "NOTE: This is a FAMILY spot. Minimum booking duration is 8 hours.\n";
            cout << "Enter booking duration (hours): ";
            string durationStr;
            getline(cin, durationStr);
            
            if (!parseIntInRange(durationStr, durationHours, 8, 24*7)) { // Max 1 week
                cout << "Invalid duration! Family spots require 8-168 hours (1 week).\n";
                break;
            }
        } else {
            cout << "Enter booking duration (hours, default 1): ";
            string durationStr;
            getline(cin, durationStr);
            
            if (!durationStr.empty()) {
                if (!parseIntInRange(durationStr, durationHours, 1, 24*7)) {
                    cout << "Invalid duration! Using default 1 hour.\n";
                    durationHours = 1;
                }
            }
        }
        
        // Try to reserve the spot
        if (!parkingMap.reserveSpot(spotId, type, durationHours)) {
            cout << "Failed to reserve spot " << spotId << "!\n";
            break;
        }
        
        cout << "Successfully reserved spot " << spotId << " for " << durationHours << " hours!\n";
        
        // Calculate rate based on duration
        int capacity = 72; // 6x12 grid
        int occupied = capacity - availableSpots.size();
        parkingSpotType spotType = selectedSpot->getType();
        bool isCharging = (spotType == parkingSpotType::ev_charging);
        
        int hourlyRate = dynamicPrice::calRate(occupied, capacity, type, spotType, isCharging);
        int totalRate = hourlyRate * durationHours;
        
        cout << "Hourly Rate: Rs " << hourlyRate << "\n";
        cout << "Total Rate for " << durationHours << " hours: Rs " << totalRate << "\n";

        u->recordUsage();
        int finalRate = u->getDiscountedRate(totalRate);
        Payment::addRevenue(finalRate, type);

        // ----- Payment -----
        string payLine;
        int payChoice;
        cout << "Choose Payment Method:\n1. Cash\n2. Card\n3. UPI\n4. "
                "Netbanking\nEnter choice: ";
        getline(cin, payLine);
        if (!parseIntInRange(payLine, payChoice, 1, 4)) {
            cout << "Invalid choice! Defaulting to Cash.\n";
            payChoice = 1;
        }
        switch (payChoice) {
            case 1:
                Payment::pay(finalRate);
                break;
            case 2: {
                cout << "Enter Card Number: ";
                long long cardnum;
                cin >> cardnum;
                string cardNum = to_string(cardnum);
                if (cardNum.length() >= 13 && cardNum.length() <= 19){
                    Payment::pay(finalRate, cardnum);
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else
                    cout << "Please Enter valid card number\n";
                break;
            }
            case 3: {
                cout << "Enter UPI ID: ";
                string upi;
                getline(cin, upi);
                Payment::pay(finalRate, upi, true);
                break;
            }
            case 4: {
                int bankType;
                cout << "1. HDFC\n2. SBI\n3. Axis\n4. ICICI\n5. Bank of Baroda\n "
                        "6. Punjab National Banking\n7. IndusInd Bank\n";
                cout << "Select Option from above for Payment: ";
                cin >> bankType;
                cout << "Enter Account Number: ";
                long long acc;
                cin >> acc;
                Payment::pay(finalRate, acc, bankType);
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }
        
        // ----- Save Transaction -----
        ofstream fout("transactions.txt", ios::app);
        time_t now = time(0);
        fout << "UserID: " << u->getID() << " | VehicleType: " << (int)type
            << " | SpotType: " << (int)spotType << " | SpotID: " << spotId 
            << " | Duration: " << durationHours << " hours"
            << " | Amount: Rs " << finalRate
            << " | Time: " << ctime(&now);
        fout.close();

        // ----- Save Revenue -----
        Payment::saveRevenueToFile("revenue.txt");
        break;
      }

      case 3:
        Payment::showRevenue();
        break;

      case 4: {  // Rating
        if (users.empty()) {
          cout << "No users registered yet!\n";
          break;
        }
        cout << "Enter User ID: ";
        string id;
        getline(cin, id);
        auto it = users.find(id);
        if (it == users.end()) {
          cout << "User not found!\n";
          break;
        }
        user *u = it->second;
        cout << "Enter rating (1-5 stars): ";
        string rline;
        getline(cin, rline);
        int r;
        if (!parseIntInRange(rline, r, 1, 5)) {
          cout << "Invalid rating. Skipping.\n";
          break;
        }
        u->setRating(r);
        u->saveToFile("users.txt");
        break;
      }

      case 5: {  // Review
        if (users.empty()) {
          cout << "No users registered yet!\n";
          break;
        }
        cout << "Enter User ID: ";
        string id;
        getline(cin, id);
        auto it = users.find(id);
        if (it == users.end()) {
          cout << "User not found!\n";
          break;
        }
        user *u = it->second;
        cout << "Enter your review: ";
        string rev;
        getline(cin, rev);
        u->setReview(rev);
        u->saveToFile("users.txt");
        break;
      }

      case 6: {  // Show score
        if (users.empty()) {
          cout << "No users registered yet!\n";
          break;
        }
        cout << "Enter User ID: ";
        string id;
        getline(cin, id);
        auto it = users.find(id);
        if (it == users.end()) {
          cout << "User not found!\n";
          break;
        }
        user *u = it->second;
        u->showScore();
        break;
      }
      case 7: {
        user::readFromFile("users.txt");
        break;
      }
      case 8: {
        ifstream fin("transactions.txt");
        if (!fin) {
          cout << "No transactions found!\n";
        } else {
          string line;
          cout << "\n--- Transaction History ---\n";
          while (getline(fin, line)) {
            cout << line << "\n";
          }
          fin.close();
        }
        break;
      }
      case 9: {
        parkingMap.displayParkingMap();
        break;
      }
      case 10: {  // Calculate parking fee with late fine
        if (users.empty()) {
          cout << "No users registered yet!\n";
          break;
        }
        cout << "Enter User ID: ";
        string id;
        getline(cin, id);
        auto it = users.find(id);
        if (it == users.end()) {
          cout << "User not found!\n";
          break;
        }
        user *u = it->second;
        
        int baseRate;
        cout << "Enter base parking rate (or press Enter for default Rs 100): ";
        string rateInput;
        getline(cin, rateInput);
        if (rateInput.empty()) {
            baseRate = 100;
        } else {
            if (!parseIntInRange(rateInput, baseRate, 1, 10000)) {
                cout << "Invalid rate! Using default Rs 100.\n";
                baseRate = 100;
            }
        }
        
        u->calculateParkingFee(baseRate);
        break;
      }
      case 11: {
        LateFineCalculator::displayFineRules();
        break;
      }
      case 12: {  
        cout << "Enter Spot ID to release: ";
        string spotId;
        getline(cin, spotId);
        
        // Convert to uppercase for consistency
        for (char &c : spotId) {
            c = toupper(c);
        }
        
        // Validate spot ID format
        if (spotId.length() < 2 || !isalpha(spotId[0]) || !isdigit(spotId[1])) {
            cout << "ERROR: Invalid spot ID format! Please use format like A1, B2, etc.\n";
            break;
        }
        
        // Try to release the spot
        if (parkingMap.releaseSpot(spotId)) {
            cout << "Spot " << spotId << " has been successfully released!\n";
            cout << "The spot is now available for other users.\n";
        } else {
            cout << "Failed to release spot " << spotId << "!\n";
        }
        break;
      }
      case 13: {  // Maintenance Management
        cout << "\n=== MAINTENANCE MANAGEMENT ===\n";
        cout << "1. Put spot under maintenance\n";
        cout << "2. Remove spot from maintenance\n";
        cout << "3. View spots under maintenance\n";
        cout << "Enter choice: ";
        string maintChoiceStr;
        getline(cin, maintChoiceStr);
        int maintChoice;
        
        if (!parseIntInRange(maintChoiceStr, maintChoice, 1, 3)) {
            cout << "Invalid choice!\n";
            break;
        }
        
        switch (maintChoice) {
            case 1: {
                cout << "Enter Spot ID to put under maintenance: ";
                string spotId;
                getline(cin, spotId);
                
                for (char &c : spotId) {
                    c = toupper(c);
                }
                
                if (spotId.length() < 2 || !isalpha(spotId[0]) || !isdigit(spotId[1])) {
                    cout << "ERROR: Invalid spot ID format!\n";
                    break;
                }
                
                if (parkingMap.setSpotMaintenance(spotId)) {
                    cout << "Spot " << spotId << " is now under maintenance.\n";
                } else {
                    cout << "Failed to put spot " << spotId << " under maintenance.\n";
                }
                break;
            }
            case 2: {
                cout << "Enter Spot ID to remove from maintenance: ";
                string spotId;
                getline(cin, spotId);
                
                for (char &c : spotId) {
                    c = toupper(c);
                }
                
                if (spotId.length() < 2 || !isalpha(spotId[0]) || !isdigit(spotId[1])) {
                    cout << "ERROR: Invalid spot ID format!\n";
                    break;
                }
                
                if (parkingMap.removeSpotMaintenance(spotId)) {
                    cout << "Spot " << spotId << " maintenance completed and is now available.\n";
                } else {
                    cout << "Failed to remove spot " << spotId << " from maintenance.\n";
                }
                break;
            }
            case 3: {
                parkingMap.displayMaintenanceSpots();
                break;
            }
        }
        break;
      }
    }

  } while (choice != 14);

  for (auto &p : users) delete p.second;
  cout << "Thank you for using Smart Parking System!\n";
  return 0;
}