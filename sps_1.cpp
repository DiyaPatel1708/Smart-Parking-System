#include <cctype>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

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

 public:
  parkingSpot(const string &id, parkingSpotType t) {
    spotID = id;
    type = t;
    status = spotStatus::free;
    reservationTime = 0;
  }
  friend void debugSpot(const parkingSpot &ps);

  string getSpotId() const { return spotID; }
  spotStatus getStatus() const { return status; }
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
        return (vehicle == vehicleType::sevenSeaterCar ||
                vehicle == vehicleType::cycle ||
                vehicle == vehicleType::motorcycle ||
                vehicle == vehicleType::fiveSeaterCar ||
                vehicle == vehicleType::disabled || vehicle == vehicleType::EV);
      default:
        return false;
    }
  }
  bool reserve(vehicleType vehicle) {
    if (status != spotStatus::free) {
      cout << "Spot " << spotID << " is not free!\n";
      return false;
    }
    if (!isVehicleCompatibleWithSpot(vehicle, type)) {
      cout << "Vehicle type not suitable for spot " << spotID << "!\n";
      return false;
    }
    status = spotStatus::reserved;
    cout << "Spot " << spotID << " reserved successfully.\n";
    return true;
  }
  string getSpotSymbol(parkingSpotType type) {
    switch (type) {
      case parkingSpotType::standard:
        return "Stdrd";
      case parkingSpotType::premium:
        return "Prmium";
      case parkingSpotType::ev_charging:
        return "EV";
      case parkingSpotType::disabled:
        return "Disabld";
      case parkingSpotType::motorcycle:
        return "Mtrcycl";
      case parkingSpotType::large:
        return "Lrge";
      case parkingSpotType::compact:
        return "Compact";
      case parkingSpotType::family:
        return "Fmly";
      case parkingSpotType::cycle:
        return "Cycl";
      case parkingSpotType::small:
        return "Sml";
      default:
        return "?";
    }
  }
  bool reserve(vehicleType vehicle, time_t reservedUntil) {
    if (!reserve(vehicle)) return false;
    reservationTime = reservedUntil;
    cout << "Reservation for spot " << spotID << " is valid until "
         << ctime(&reservedUntil);
    return true;
  }
  bool release() {
    if (status == spotStatus::free) return false;
    status = spotStatus::free;
    reservationTime = 0;
    cout << "Spot " << spotID << " has been released.\n";
    return true;
  }
  void checkSpotDetails(const parkingSpot &spot) {
    cout << "Spot ID: " << spot.spotID << ", Status: " << (int)spot.status
         << "\n";
  }
  void displayGarage() {
    for (auto &spot : spotID) {
      string symbol = getSpotSymbol(type);
      cout << "[" << symbol << "] ";
    }
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
      cout << "Spot " << spotID << " is now free.\n";
    }
    return *this;
  }
};
void debugSpot(const parkingSpot &ps) {
  cout << "[DEBUG] Spot " << ps.spotID << " | Status: " << (int)ps.status
       << "\n";
}

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
        cout << "[INFO] No user file found. Starting fresh.\n";
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
        if (line.empty()) continue; // skip empty lines

        vector<string> fields;
        string temp;
        stringstream ss(line);

        // Split line by '|'
        while (getline(ss, temp, '|')) {
            trim(temp);
            fields.push_back(temp);
        }

        if (fields.size() < 5) {
            cout << "[WARN] Skipping malformed line: " << line << "\n";
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

        // Extract VehicleType number
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

        // Check if user already exists
        if (users.find(userID) != users.end()) {
            cout << "[INFO] Duplicate UserID skipped: " << userID << "\n";
            continue;
        }

        // Create correct derived object
        switch (vTypeInt) {
            case 0: users[userID] = new CycleUser(name, userID, contact, email); break;
            case 1: users[userID] = new FCarUser(name, userID, contact, email); break;
            case 2: users[userID] = new SCarUser(name, userID, contact, email); break;
            case 3: users[userID] = new MotorcycleUser(name, userID, contact, email); break;
            case 4: users[userID] = new EVUser(name, userID, contact, email); break;
            case 5: users[userID] = new DisabledUser(name, userID, contact, email); break;
            default:
                cout << "[WARN] Unknown vehicle type in line: " << line << "\n";
                continue;
        }

        cout << "[INFO] Loaded user: " << name << " (" << userID << ")\n";
    }

    fin.close();
}



// ---------------- MENU ----------------
int main() {
  unordered_map<string, user *> users;
  loadUsers(users, "users.txt");  

  int choice = 0;

  do {
    cout << "\n========== SMART PARKING MENU ==========\n";
    cout << "1. Register User\n2. Book Parking Spot\n3. Show Total Revenue\n";
    cout << "4. Rate System (1-5 stars)\n5. Add Review\n6. Show User "
            "Score\n";
    cout << "7. View Stored Users\n";
    cout << "8. View Transactions\n0. Exit\n";
    cout << "Enter choice: ";
    string line;
    if (!getline(cin, line)) break;

    if (!parseIntInRange(line, choice, 0, 8)) {
      cout << "Invalid choice! Please enter a number 0-8.\n";
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
        int capacity = 100, occupied = 40;
        vehicleType type = getVehicleType(*u);

        cout << "\n--- Available Spot Types & Rates ---\n";
        for (int s = 1; s <= 10; s++) {
          parkingSpotType tempSpot;
          string spotName;
          bool isChargingPreview = false;
          switch (s) {
            case 1:
              tempSpot = parkingSpotType::standard;
              spotName = "Standard";
              break;
            case 2:
              tempSpot = parkingSpotType::premium;
              spotName = "Premium";
              break;
            case 3:
              tempSpot = parkingSpotType::ev_charging;
              spotName = "EV Charging";
              isChargingPreview = true;
              break;
            case 4:
              tempSpot = parkingSpotType::motorcycle;
              spotName = "Motorcycle";
              break;
            case 5:
              tempSpot = parkingSpotType::disabled;
              spotName = "Disabled";
              break;
            case 6:
              tempSpot = parkingSpotType::cycle;
              spotName = "Cycle";
              break;
            case 7:
              tempSpot = parkingSpotType::small;
              spotName = "Small";
              break;
            case 8:
              tempSpot = parkingSpotType::compact;
              spotName = "Compact";
              break;
            case 9:
              tempSpot = parkingSpotType::large;
              spotName = "Large";
              break;
            case 10:
              tempSpot = parkingSpotType::family;
              spotName = "Family";
              break;
          }
          parkingSpot ps("dummy", tempSpot);
          if (!ps.isVehicleCompatibleWithSpot(type, tempSpot)){
            continue;
          }
          int previewRate = dynamicPrice::calRate(occupied, capacity, type,tempSpot, isChargingPreview);
          cout << s << ". " << spotName << " Spot -> Rs " << previewRate
               << "\n";
        }

        cout << "Choose your Spot Type: ";
        string sline;
        getline(cin, sline);
        int sChoice;
        parkingSpotType spot = parkingSpotType::standard;
        bool isCharging = false;
        if (parseIntInRange(sline, sChoice, 1, 10)) {
          switch (sChoice) {
            case 1:
              spot = parkingSpotType::standard;
              break;
            case 2:
              spot = parkingSpotType::premium;
              break;
            case 3:
              spot = parkingSpotType::ev_charging;
              isCharging = true;
              break;
            case 4:
              spot = parkingSpotType::motorcycle;
              break;
            case 5:
              spot = parkingSpotType::disabled;
              break;
            case 6:
              spot = parkingSpotType::cycle;
              break;
            case 7:
              spot = parkingSpotType::small;
              break;
            case 8:
              spot = parkingSpotType::compact;
              break;
            case 9:
              spot = parkingSpotType::large;
              break;
            case 10:
              spot = parkingSpotType::family;
              break;
          }
        } else
          cout << "Invalid choice! Defaulting to Standard spot.\n";

        int rate =
            dynamicPrice::calRate(occupied, capacity, type, spot, isCharging);
        cout << "Parking Rate for chosen spot: Rs " << rate << "\n";

        u->recordUsage();
        int finalRate = u->getDiscountedRate(rate);
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
          case 0: {
            cout << "Exiting... Goodbye!\n";
            choice = 5;
            break;
      }
      }
      // ----- Save Transaction -----
      ofstream fout("transactions.txt", ios::app);
      time_t now = time(0);
      fout << "UserID: " << u->getID() << " | VehicleType: " << (int)type
         << " | SpotType: " << (int)spot << " | Amount: Rs " << finalRate
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
      case 0: {
        cout << "Exiting... Goodbye!\n";
        choice = 11;
        break;
      }
    }

  } while (choice != 0 && choice != 11);

  for (auto &p : users) delete p.second;
  return 0;
}