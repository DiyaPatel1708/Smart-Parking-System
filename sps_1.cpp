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
#include <algorithm>

using namespace std;

// Vehicle types supported by the system
enum class VehicleCategory {
    BICYCLE,
    CAR_5SEATER, 
    CAR_7SEATER,
    MOTORCYCLE,
    ELECTRIC_VEHICLE,
    DISABLED_VEHICLE
};

// Possible states of a parking spot
enum class SpotState { 
    AVAILABLE, 
    BOOKED, 
    TAKEN, 
    UNDER_MAINTENANCE 
};

// Different types of parking spots available
enum class ParkingSpotCategory {
    BICYCLE_SPOT,
    REGULAR_SPOT,
    PREMIUM_SPOT,
    EV_CHARGING_SPOT,
    MOTORCYCLE_SPOT,
    COMPACT_SPOT,
    LARGE_SPOT,
    SMALL_SPOT,
    FAMILY_SPOT,
    DISABLED_SPOT
};

// Forward declarations
class ParkingSpot;
class ParkingLayoutManager;

// Utility functions
bool parseIntInRange(const string& str, int& result, int minVal, int maxVal) {
    try {
        size_t pos;
        result = stoi(str, &pos);
        return pos == str.length() && result >= minVal && result <= maxVal;
    } catch (...) {
        return false;
    }
}

bool isBlank(const string& str) {
    return all_of(str.begin(), str.end(), [](char c) { return isspace(c); });
}

bool isValidName(const string& name) {
    if (name.empty()) return false;
    for (char c : name) {
        if (!isalpha(c) && c != ' ' && c != '-') return false;
    }
    return true;
}

bool isValidPassword(const string& password) {
    if (password.length() < 6) {
        cout << "Password must be at least 6 characters long!\n";
        return false;
    }
    return true;
}

bool isAllDigits(const string& str) {
    if (str.empty()) return false;
    for (char c : str)
        if (!isdigit((unsigned char)c)) return false;
    return true;
}

bool isValidIndianMobile(const string& mobile) {
    return mobile.size() == 10 && isAllDigits(mobile) &&
           (mobile[0] == '6' || mobile[0] == '7' || mobile[0] == '8' || mobile[0] == '9');
}

bool isEmailValid(const string& email) {
    size_t atpos = email.find('@');
    size_t dotpos = email.find('.', atpos + 1);

    if (atpos == string::npos || dotpos == string::npos) return false;
    if (atpos == 0 || dotpos == 0) return false;
    if (atpos == email.size() - 1 || dotpos == email.size() - 1) return false;
    if (atpos > dotpos) return false;
    if (email.find(' ') != string::npos) return false;

    return true;
}

// File handling class for data persistence
class DataStorage {
private:
    static const string USER_DATA_FILE;
    static const string TRANSACTION_LOG_FILE;
    static const string REVENUE_DATA_FILE;
    static const string SPOT_STATUS_FILE;
    static const string SPOT_DATABASE_FILE;

public:
    // Save user information to file
     static void storeUserData(const string &userName, const string &userId, const string &userPassword,
                              long long userContact, const string &userEmail, VehicleCategory userVehicle, 
                              int userRating, const string &userFeedback, const string &securityQuestion = "", 
                              const string &securityAnswer = "") {
        ofstream outputFile(USER_DATA_FILE, ios::app);
        if (outputFile.is_open()) {
            time_t currentTime = time(0);
            char timeBuffer[80];
            strftime(timeBuffer, sizeof(timeBuffer), "%a %b %d %H:%M:%S %Y", localtime(&currentTime));
            
            outputFile << userName << "|" << userId << "|" << userPassword << "|" << userContact << "|" << userEmail 
                 << "|Vehicle:" << static_cast<int>(userVehicle) 
                 << "|Joined:" << timeBuffer
                 << "|Rating:" << userRating
                 << "|Feedback:" << userFeedback;
            
            // Only add security questions if they are provided
            if (!securityQuestion.empty()) {
                outputFile << "|SecurityQ:" << securityQuestion;
            }
            if (!securityAnswer.empty()) {
                outputFile << "|SecurityA:" << securityAnswer;
            }
            
            outputFile << "\n";
            outputFile.close();
            cout << "User data saved to file with security questions.\n";
        } else {
            cout << "Error: Could not open user data file for writing!\n";
        }
    }

    // Update user password in file
static bool updateUserPasswordInFile(const string &userId, const string &newPassword) {
    ifstream inputFile(USER_DATA_FILE);
    if (!inputFile) {
        cout << "No user records found!\n";
        return false;
    }

    vector<string> allLines;
    string dataLine;
    bool userFound = false;
    int updateCount = 0;

    while (getline(inputFile, dataLine)) {
        // More precise matching - look for "|userId|" pattern
        size_t userPos = dataLine.find("|" + userId + "|");
        if (userPos != string::npos) {
            // Found the user - update the password
            vector<string> fields;
            stringstream ss(dataLine);
            string field;
            
            while (getline(ss, field, '|')) {
                fields.push_back(field);
            }
            
            if (fields.size() >= 3) {
                fields[2] = newPassword; // Update password field
                
                // Reconstruct the line
                string updatedLine;
                for (size_t i = 0; i < fields.size(); i++) {
                    updatedLine += fields[i];
                    if (i < fields.size() - 1) updatedLine += "|";
                }
                allLines.push_back(updatedLine);
                userFound = true;
                updateCount++;
            } else {
                allLines.push_back(dataLine);
            }
        } else {
            allLines.push_back(dataLine);
        }
    }
    inputFile.close();

    if (!userFound) {
        cout << "User ID not found in records!\n";
        return false;
    }

    // Only show message once, not for every duplicate
    if (updateCount > 0) {
        cout << "Password updated in file for user: " << userId << "\n";
    }

    // Rewrite file with updated password
    ofstream outputFile(USER_DATA_FILE);
    if (outputFile.is_open()) {
        for (const string &line : allLines) {
            outputFile << line << "\n";
        }
        outputFile.close();
        return true;
    } else {
        cout << "Error updating user records!\n";
        return false;
    }
}
    // Read user data silently during startup
    static void loadUserDataSilently() {
    ifstream inputFile(USER_DATA_FILE);
    if (!inputFile) {
        // Create empty file if it doesn't exist - ADD THIS
        ofstream createFile(USER_DATA_FILE);
        if (createFile.is_open()) {
            createFile.close();
        }
        return;
    }
    string dataLine;
    while (getline(inputFile, dataLine)) {}
    inputFile.close();
}

    // Save transaction details
    static void recordTransaction(const string &userId, VehicleCategory vehicleType, ParkingSpotCategory spotCategory,
                               const string &spotIdentifier, int timeDuration, int paymentAmount, const string &paymentType) {
        ofstream outputFile(TRANSACTION_LOG_FILE, ios::app);
        if (outputFile.is_open()) {
            time_t currentTime = time(0);
            outputFile << "Time:" << ctime(&currentTime)
                 << "User:" << userId 
                 << "|Vehicle:" << static_cast<int>(vehicleType)
                 << "|SpotType:" << static_cast<int>(spotCategory)
                 << "|Spot:" << spotIdentifier
                 << "|Hours:" << timeDuration << " hours"
                 << "|Payment:Rs " << paymentAmount
                 << "|Method:" << paymentType << "\n\n";
            outputFile.close();
        }
    }

    // Load transactions silently
    static void loadTransactionsSilently() {
    ifstream inputFile(TRANSACTION_LOG_FILE);
    if (!inputFile) {
        // Create empty file if it doesn't exist - ADD THIS
        ofstream createFile(TRANSACTION_LOG_FILE);
        if (createFile.is_open()) {
            createFile.close();
        }
        return;
    }
    string dataLine;
    while (getline(inputFile, dataLine)) {}
    inputFile.close();
}
    // Generate revenue report
    static void generateRevenueReport(int bikeRevenue, int carRevenue, int disabledRevenue, int evRevenue) {
        ofstream outputFile(REVENUE_DATA_FILE, ios::app);
        if (outputFile.is_open()) {
            time_t currentTime = time(0);
            outputFile << "=== REVENUE SUMMARY ===\n";
            outputFile << "Date: " << ctime(&currentTime);
            outputFile << "Two-Wheeler Income: Rs " << bikeRevenue << "\n";
            outputFile << "Four-Wheeler Income: Rs " << carRevenue << "\n";
            outputFile << "Disabled Vehicle Income: Rs " << disabledRevenue << "\n";
            outputFile << "EV Income: Rs " << evRevenue << "\n";
            outputFile << "TOTAL INCOME: Rs " << (bikeRevenue + carRevenue + disabledRevenue + evRevenue) << "\n";
            outputFile << "=======================\n\n";
            outputFile.close();
        }
    }

    // Load revenue data silently
    static void loadRevenueDataSilently() {
    ifstream inputFile(REVENUE_DATA_FILE);
    if (!inputFile) {
        // Create empty file if it doesn't exist - ADD THIS
        ofstream createFile(REVENUE_DATA_FILE);
        if (createFile.is_open()) {
            createFile.close();
        }
        return;
    }
    string dataLine;
    while (getline(inputFile, dataLine)) {}
    inputFile.close();
}

    // Save current spot status
    static void saveSpotStatus(const vector<vector<string>> &spotLayout) {
        ofstream outputFile(SPOT_STATUS_FILE);
        if (outputFile.is_open()) {
            time_t currentTime = time(0);
            outputFile << "=== CURRENT SPOT STATUS ===\n";
            outputFile << "Last Update: " << ctime(&currentTime);
            outputFile << "Format: SpotID|Status|Category\n";
            
            for (const auto &row : spotLayout) {
                for (const string &spot : row) {
                    outputFile << spot << "\n";
                }
            }
            outputFile.close();
        }
    }

    // Load spot status silently
    static void loadSpotStatusSilently() {
    ifstream inputFile(SPOT_STATUS_FILE);
    if (!inputFile) {
        // Create empty file if it doesn't exist - ADD THIS
        ofstream createFile(SPOT_STATUS_FILE);
        if (createFile.is_open()) {
            createFile.close();
        }
        return;
    }
    string dataLine;
    while (getline(inputFile, dataLine)) {}
    inputFile.close();
}

    // Save permanent spot layout to file
    static void saveSpotDatabase(ParkingLayoutManager& layoutManager);
    
    // Load permanent spot layout from file
    static bool loadSpotDatabase(ParkingLayoutManager& layoutManager);

    // Save CURRENT spot states (status, occupant, etc.)
    static void saveSpotStates(ParkingLayoutManager& layoutManager);
    
    // Load spot states from file
    static bool loadSpotStates(ParkingLayoutManager& layoutManager);

    // Delete user account from file
    static bool deleteUserAccount(const string &userId) {
        ifstream inputFile(USER_DATA_FILE);
        if (!inputFile) {
            cout << "No user records found!\n";
            return false;
        }

        vector<string> allLines;
        string dataLine;
        bool userFound = false;

        // Read all lines and exclude the user to delete
        while (getline(inputFile, dataLine)) {
            if (dataLine.find("|" + userId + "|") == string::npos) {
                allLines.push_back(dataLine);
            } else {
                userFound = true;
                cout << "User account found and marked for deletion.\n";
            }
        }
        inputFile.close();

        if (!userFound) {
            cout << "User ID not found in records!\n";
            return false;
        }

        // Rewrite file without the deleted user
        ofstream outputFile(USER_DATA_FILE);
        if (outputFile.is_open()) {
            for (const string &line : allLines) {
                outputFile << line << "\n";
            }
            outputFile.close();
            cout << "User account successfully deleted from records.\n";
            return true;
        } else {
            cout << "Error updating user records!\n";
            return false;
        }
    }

    // Delete user transactions
    static bool deleteUserTransactions(const string &userId) {
        ifstream inputFile(TRANSACTION_LOG_FILE);
        if (!inputFile) {
            return true; // No transactions to delete
        }

        vector<string> allLines;
        string dataLine;
        bool inUserTransaction = false;
        bool transactionsFound = false;

        while (getline(inputFile, dataLine)) {
            if (dataLine.find("User:" + userId) != string::npos) {
                inUserTransaction = true;
                transactionsFound = true;
                continue; // Skip this line
            }
            
            if (inUserTransaction) {
                if (dataLine.empty()) {
                    inUserTransaction = false;
                }
                continue; // Skip transaction lines
            }
            
            allLines.push_back(dataLine);
        }
        inputFile.close();

        if (transactionsFound) {
            ofstream outputFile(TRANSACTION_LOG_FILE);
            if (outputFile.is_open()) {
                for (const string &line : allLines) {
                    outputFile << line << "\n";
                }
                outputFile.close();
                cout << "User transactions deleted successfully.\n";
            }
        }

        return true;
    }

    // Initialize all data files
    static void initializeAllData(ParkingLayoutManager& layoutManager) {
        loadUserDataSilently();
        loadTransactionsSilently();
        loadRevenueDataSilently();
        loadSpotDatabase(layoutManager);
        loadSpotStates(layoutManager);
    }

    // Display stored users
    static void showUserData() {
        ifstream inputFile(USER_DATA_FILE);
        if (!inputFile) {
            cout << "No user records found!\n";
            return;
        }
        cout << "\n=== REGISTERED USERS ===\n";
        string dataLine;
        while (getline(inputFile, dataLine)) {
            cout << dataLine << endl;
        }
        inputFile.close();
    }

    // Display transaction history
    static void showTransactionHistory(const string &filterDate = "") {
        ifstream inputFile(TRANSACTION_LOG_FILE);
        if (!inputFile) {
            cout << "No transaction records found!\n";
            return;
        }

        cout << "\n=== TRANSACTION LOG ===\n";
        string dataLine;
        bool matchesFound = false;
        
        while (getline(inputFile, dataLine)) {
            if (dataLine.find("Time:") != string::npos) {
                string transactionTime = dataLine.substr(5);
                if (filterDate.empty() || transactionTime.find(filterDate) != string::npos) {
                    matchesFound = true;
                    cout << dataLine << endl;
                    while (getline(inputFile, dataLine) && !dataLine.empty()) {
                        cout << dataLine << endl;
                    }
                    cout << "------------------------\n";
                }
            }
        }
        
        if (!matchesFound && !filterDate.empty()) {
            cout << "No transactions on: " << filterDate << endl;
        }
        inputFile.close();
    }

    // Display spot status
    static void showSpotStatus() {
        ifstream inputFile(SPOT_STATUS_FILE);
        if (!inputFile) {
            cout << "No spot data available!\n";
            return;
        }
        
        cout << "\n=== PARKING SPOT STATUS ===\n";
        string dataLine;
        while (getline(inputFile, dataLine)) {
            cout << dataLine << endl;
        }
        inputFile.close();
    }

    // Generate daily revenue report
    static void createDailyRevenueReport() {
        time_t currentTime = time(0);
        tm *localTime = localtime(&currentTime);
        string todayDate = to_string(localTime->tm_mday) + "/" + to_string(1 + localTime->tm_mon) + "/" + to_string(1900 + localTime->tm_year);
        
        ifstream inputFile(TRANSACTION_LOG_FILE);
        if (!inputFile) {
            cout << "No data for revenue report!\n";
            return;
        }

        int todayRevenue = 0;
        string dataLine;
        bool hasTransactions = false;
        
        while (getline(inputFile, dataLine)) {
            if (dataLine.find("Time:") != string::npos) {
                string transactionTime = dataLine.substr(5);
                if (transactionTime.find(to_string(localTime->tm_mday)) != string::npos && 
                    transactionTime.find(to_string(1 + localTime->tm_mon)) != string::npos) {
                    hasTransactions = true;
                    while (getline(inputFile, dataLine) && !dataLine.empty()) {
                        if (dataLine.find("Payment:Rs") != string::npos) {
                            size_t amountPos = dataLine.find("Payment:Rs");
                            if (amountPos != string::npos) {
                                string amountText = dataLine.substr(amountPos + 10);
                                string cleanAmount;
                                for (char digit : amountText) {
                                    if (isdigit(digit)) cleanAmount += digit;
                                }
                                if (!cleanAmount.empty()) {
                                    try {
                                        todayRevenue += stoi(cleanAmount);
                                    } catch (...) {}
                                }
                            }
                        }
                    }
                }
            }
        }
        inputFile.close();

        cout << "\n=== TODAY'S REVENUE ===\n";
        cout << "Date: " << todayDate << endl;
        if (hasTransactions) {
            cout << "Total Revenue: Rs " << todayRevenue << endl;
        } else {
            cout << "No transactions today.\n";
        }
        cout << "=======================\n";
    }

    // Get current date string
    static string getTodaysDate() {
        time_t currentTime = time(0);
        tm *localTime = localtime(&currentTime);
        return to_string(localTime->tm_mday) + "/" + to_string(1 + localTime->tm_mon) + "/" + to_string(1900 + localTime->tm_year);
    }

    // Get security question and answer for a user
   static pair<string, string> getSecurityQA(const string &userId) {
        ifstream inputFile(USER_DATA_FILE);
        if (!inputFile) {
            return make_pair("", "");
        }

        string dataLine;
        while (getline(inputFile, dataLine)) {
            // Look for the specific user
            size_t userPos = dataLine.find("|" + userId + "|");
            if (userPos != string::npos) {
                // Found the user line
                vector<string> fields;
                stringstream ss(dataLine);
                string field;
                
                while (getline(ss, field, '|')) {
                    fields.push_back(field);
                }
                
                string securityQ = "", securityA = "";
                for (const string &f : fields) {
                    if (f.find("SecurityQ:") == 0) {
                        securityQ = f.substr(10);
                    } else if (f.find("SecurityA:") == 0) {
                        securityA = f.substr(10);
                    }
                }
                
                inputFile.close();
                
                // Clean up any whitespace
                securityQ.erase(remove(securityQ.begin(), securityQ.end(), '\n'), securityQ.end());
                securityQ.erase(remove(securityQ.begin(), securityQ.end(), '\r'), securityQ.end());
                securityA.erase(remove(securityA.begin(), securityA.end(), '\n'), securityA.end());
                securityA.erase(remove(securityA.begin(), securityA.end(), '\r'), securityA.end());
                
                return make_pair(securityQ, securityA);
            }
        }
        inputFile.close();
        return make_pair("", "");
    }
};

// Initialize file paths
const string DataStorage::USER_DATA_FILE = "user_records.txt";
const string DataStorage::TRANSACTION_LOG_FILE = "transaction_history.txt";
const string DataStorage::REVENUE_DATA_FILE = "revenue_data.txt";
const string DataStorage::SPOT_STATUS_FILE = "spot_status.txt";
const string DataStorage::SPOT_DATABASE_FILE = "parking_spots_db.txt";

// Individual parking spot class
class ParkingSpot {
 private:
  string spotIdentifier;
  SpotState currentStatus;
  ParkingSpotCategory spotCategory;
  time_t reservationTimestamp;
  int reservationLength;
  string occupiedByUserId;

 public:
  ParkingSpot(const string &id, ParkingSpotCategory category) {
    spotIdentifier = id;
    spotCategory = category;
    currentStatus = SpotState::AVAILABLE;
    reservationTimestamp = 0;
    reservationLength = 0;
    occupiedByUserId = "";
  }

  string getSpotId() const { return spotIdentifier; }
  SpotState getCurrentStatus() const { return currentStatus; }
  ParkingSpotCategory getSpotCategory() const { return spotCategory; }
  string getOccupantId() const { return occupiedByUserId; }
  time_t getReservationTimestamp() const { return reservationTimestamp; }
  
  void setCurrentStatus(SpotState status) { currentStatus = status; }
  void setOccupantId(const string& occupant) { occupiedByUserId = occupant; }
  void setReservationTimestamp(time_t timestamp) { reservationTimestamp = timestamp; }
  
  // Check if vehicle can park in this spot
  bool canVehicleParkHere(VehicleCategory vehicle, ParkingSpotCategory spot) {
    switch (spot) {
      case ParkingSpotCategory::REGULAR_SPOT:
        return (vehicle == VehicleCategory::CAR_5SEATER ||
                vehicle == VehicleCategory::CAR_7SEATER ||
                vehicle == VehicleCategory::ELECTRIC_VEHICLE);
      case ParkingSpotCategory::PREMIUM_SPOT:
        return (vehicle == VehicleCategory::CAR_5SEATER ||
                vehicle == VehicleCategory::CAR_7SEATER ||
                vehicle == VehicleCategory::ELECTRIC_VEHICLE);
      case ParkingSpotCategory::EV_CHARGING_SPOT:
        return (vehicle == VehicleCategory::ELECTRIC_VEHICLE);
      case ParkingSpotCategory::DISABLED_SPOT:
        return (vehicle == VehicleCategory::DISABLED_VEHICLE);
      case ParkingSpotCategory::MOTORCYCLE_SPOT:
        return (vehicle == VehicleCategory::MOTORCYCLE);
      case ParkingSpotCategory::BICYCLE_SPOT:
        return (vehicle == VehicleCategory::BICYCLE);
      case ParkingSpotCategory::LARGE_SPOT:
        return (vehicle == VehicleCategory::CAR_7SEATER);
      case ParkingSpotCategory::SMALL_SPOT:
        return (vehicle == VehicleCategory::CAR_5SEATER);
      case ParkingSpotCategory::COMPACT_SPOT:
        return (vehicle == VehicleCategory::MOTORCYCLE ||
                vehicle == VehicleCategory::BICYCLE);
      case ParkingSpotCategory::FAMILY_SPOT:
        return true;
      default:
        return false;
    }
  }
  
  // Reserve spot for parking
  bool reserveSpot(VehicleCategory vehicle, int durationHours = 1) {
    if (currentStatus != SpotState::AVAILABLE) {
      cout << "Spot " << spotIdentifier << " is not available!\n";
      return false;
    }
    
    // Family spots require minimum 8-hour booking
    if (spotCategory == ParkingSpotCategory::FAMILY_SPOT && durationHours < 8) {
      cout << "Family spots require minimum 8-hour booking!\n";
      cout << "You requested " << durationHours << " hours. Please book for 8+ hours.\n";
      return false;
    }
    
    if (!canVehicleParkHere(vehicle, spotCategory)) {
      cout << "Vehicle type not suitable for spot " << spotIdentifier << "!\n";
      cout << "This spot is for: ";
      switch(spotCategory) {
        case ParkingSpotCategory::REGULAR_SPOT: cout << "Regular Cars/EVs"; break;
        case ParkingSpotCategory::PREMIUM_SPOT: cout << "Premium Cars/EVs"; break;
        case ParkingSpotCategory::EV_CHARGING_SPOT: cout << "EV Charging"; break;
        case ParkingSpotCategory::DISABLED_SPOT: cout << "Disabled Vehicles"; break;
        case ParkingSpotCategory::MOTORCYCLE_SPOT: cout << "Motorcycles"; break;
        case ParkingSpotCategory::BICYCLE_SPOT: cout << "Bicycles"; break;
        case ParkingSpotCategory::LARGE_SPOT: cout << "Large Cars (7-seater)"; break;
        case ParkingSpotCategory::SMALL_SPOT: cout << "Small Cars (5-seater)"; break;
        case ParkingSpotCategory::COMPACT_SPOT: cout << "Compact Vehicles"; break;
        case ParkingSpotCategory::FAMILY_SPOT: cout << "Family Vehicles"; break;
      }
      cout << "\n";
      return false;
    }
    
    currentStatus = SpotState::BOOKED;
    reservationTimestamp = time(0);
    reservationLength = durationHours;
    cout << "Spot " << spotIdentifier << " reserved successfully";
    
    if (spotCategory == ParkingSpotCategory::FAMILY_SPOT) {
      cout << " (Family spot - " << durationHours << " hour booking)";
    } else {
      cout << " for " << durationHours << " hours";
    }
    cout << ".\n";
    
    return true;
  }
  
  // Occupy the spot
  bool occupySpot(const string& userId) {
    if (currentStatus != SpotState::AVAILABLE && currentStatus != SpotState::BOOKED) {
      return false;
    }
    currentStatus = SpotState::TAKEN;
    occupiedByUserId = userId;
    return true;
  }
  
  // Release the spot
  bool releaseSpot() {
    if (currentStatus == SpotState::AVAILABLE) return false;
    currentStatus = SpotState::AVAILABLE;
    reservationTimestamp = 0;
    reservationLength = 0;
    occupiedByUserId = "";
    cout << "Spot " << spotIdentifier << " is now available.\n";
    return true;
  }
  
  // Put spot under maintenance
  bool setMaintenanceMode() {
    if (currentStatus == SpotState::TAKEN || currentStatus == SpotState::BOOKED) {
        cout << "Cannot maintain spot " << spotIdentifier << "! Currently ";
        switch(currentStatus) {
            case SpotState::TAKEN: cout << "occupied"; break;
            case SpotState::BOOKED: cout << "reserved"; break;
            default: cout << "in use";
        }
        cout << ". Wait until spot is free.\n";
        return false;
    }
    
    if (currentStatus == SpotState::UNDER_MAINTENANCE) {
        cout << "Spot " << spotIdentifier << " is already under maintenance!\n";
        return false;
    }
    
    currentStatus = SpotState::UNDER_MAINTENANCE;
    return true;
  }
  
  // Remove from maintenance
  bool removeMaintenanceMode() {
    if (currentStatus != SpotState::UNDER_MAINTENANCE) {
        cout << "Spot " << spotIdentifier << " is not under maintenance!\n";
        return false;
    }
    currentStatus = SpotState::AVAILABLE;
    return true;
  }
};

// Parking map management system
class ParkingLayoutManager {
private:
    vector<vector<ParkingSpot*>> parkingGrid;
    unordered_map<string, ParkingSpot*> spotDirectory;
    const int GRID_ROWS = 6;
    const int GRID_COLUMNS = 12;

public:
    ParkingLayoutManager() {
        initializeParkingGrid();
    }
    
    ~ParkingLayoutManager() {
        for (auto& row : parkingGrid) {
            for (auto spot : row) {
                delete spot;
            }
        }
    }
    
    void initializeParkingGrid() {
        parkingGrid.resize(GRID_ROWS, vector<ParkingSpot*>(GRID_COLUMNS));
        
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLUMNS; col++) {
                string spotId = generateSpotId(row, col);
                ParkingSpotCategory spotType = determineSpotType(row, col);
                parkingGrid[row][col] = new ParkingSpot(spotId, spotType);
                spotDirectory[spotId] = parkingGrid[row][col];
            }
        }
    }
    
    // Determine spot type based on position
    ParkingSpotCategory determineSpotType(int row, int col) {
        // Elevator locations at columns 1, 6, 11 (0-indexed: 1, 6, 11)
        vector<int> elevatorColumns = {1, 6, 11};
        
        // Premium spots near elevators (first 2 rows)
        for (int elevatorCol : elevatorColumns) {
            if (abs(col - elevatorCol) <= 1 && row < 2) {
                return ParkingSpotCategory::PREMIUM_SPOT;
            }
        }
        
        // Disabled spots near elevators (first 3 rows)
        for (int elevatorCol : elevatorColumns) {
            if (abs(col - elevatorCol) <= 2 && row < 3) {
                return ParkingSpotCategory::DISABLED_SPOT;
            }
        }
        
        // EV charging near elevators (rows 2-3)
        for (int elevatorCol : elevatorColumns) {
            if (abs(col - elevatorCol) <= 1 && row >= 2 && row < 4) {
                return ParkingSpotCategory::EV_CHARGING_SPOT;
            }
        }
        
        // Family spots in central area
        if (row >= 3 && row <= 5 && col >= 4 && col <= 8) {
            return ParkingSpotCategory::FAMILY_SPOT;
        }
        
        // Large spots for 7-seaters
        if (row >= 4 && col >= 2 && col <= 10) {
            return ParkingSpotCategory::LARGE_SPOT;
        }
        
        // Small spots for 5-seaters
        if (row >= 5 && col >= 1 && col <= 11) {
            return ParkingSpotCategory::SMALL_SPOT;
        }
        
        // Motorcycle spots at edges
        if (row >= 5 && (col <= 2 || col >= 10)) {
            return ParkingSpotCategory::MOTORCYCLE_SPOT;
        }
        
        // Bicycle spots
        if (row >= 5 && col >= 3 && col <= 9) {
            return ParkingSpotCategory::BICYCLE_SPOT;
        }
        
        // Compact spots
        if (row >= 4 && (col <= 1 || col >= 11)) {
            return ParkingSpotCategory::COMPACT_SPOT;
        }
        
        // Regular spots for remaining areas
        if (row >= 2 && row <= 4) {
            return ParkingSpotCategory::REGULAR_SPOT;
        }
        
        return ParkingSpotCategory::REGULAR_SPOT;
    }
    
    string generateSpotId(int row, int col) {
        char rowChar = 'A' + row;
        return string(1, rowChar) + to_string(col + 1);
    }
    
    // Display the parking layout
    void displayParkingLayout() {
        cout << "\n" << string(80, '=') << "\n";
        cout << "                      SMART PARKING FACILITY LAYOUT\n";
        cout << string(80, '=') << "\n\n";
        
        cout << "FACILITY FEATURES:\n";
        cout << " ~ Elevators at columns 2, 7, 12 (Premium & Disabled spots nearby)\n";
        cout << " ~ Premium spots: Extra security, CCTV coverage\n";
        cout << " ~ Disabled spots: Wide spaces, ramp access\n";
        cout << " ~ Regular spots: Standard parking spaces\n";
        cout << " ~ EV Charging: Electric vehicle charging stations\n";
        cout << " ~ Family spots: Minimum 8-hour booking required\n";
        cout << " ~ Large spots: For 7-seater vehicles\n";
        cout << " ~ Small spots: For 5-seater vehicles\n";
        cout << " ~ Motorcycle spots: Dedicated motorcycle parking\n";
        cout << " ~ Bicycle spots: Dedicated bicycle parking\n";
        cout << " ~ Compact spots: For motorcycles and bicycles\n\n";
        
        cout << "LEGEND:\n";
        cout << " [ ] Available   [R] Reserved    [X] Occupied     [M] Maintenance\n";
        cout << " PRM-Premium     DIS-Disabled    EV-EV Charging   REG-Regular\n";
        cout << " FAM-Family      LRG-Large       SML-Small        CMP-Compact\n";
        cout << " MC-Motorcycle   CYC-Bicycle     +--+ Elevator Location\n";
        cout << " Note: Family spots require 8+ hour booking\n\n";
        
        // Column headers
        cout << "        ";
        for (int col = 0; col < GRID_COLUMNS; col += 2) {
            cout << "Col " << setw(2) << left << (col+1) << "-" << setw(2) << (col+2) << "  ";
        }
        cout << "\n        ";
        cout << string(80, '-') << "\n";
        
        // Display each row
        for (int row = 0; row < GRID_ROWS; row++) {
            cout << "Row " << char('A' + row) << "  ";
            
            for (int col = 0; col < GRID_COLUMNS; col += 2) {
                ParkingSpot* spot1 = parkingGrid[row][col];
                ParkingSpot* spot2 = parkingGrid[row][col + 1];
                
                string status1 = getStatusSymbol(spot1->getCurrentStatus());
                string status2 = getStatusSymbol(spot2->getCurrentStatus());
                string type1 = getTypeSymbol(spot1->getSpotCategory());
                string type2 = getTypeSymbol(spot2->getSpotCategory());
                string spotId1 = spot1->getSpotId();
                string spotId2 = spot2->getSpotId();
                
                cout << "[" << status1 << "]" << type1 << " " << spotId1 << " " << spotId2 << "  ";
            }
            cout << "\n";
            
            // Show elevator indicators between rows
            if (row < GRID_ROWS - 1) {
                cout << "        ";
                for (int col = 0; col < GRID_COLUMNS; col += 2) {
                    if (col == 1 || col == 6 || col == 11) {
                        cout << "   +--+        ";
                    } else {
                        cout << "--------------";
                    }
                }
                cout << "\n";
            }
        }
        
        cout << "        ";
        cout << string(80, '-') << "\n";
        
        cout << "ELEVATOR LOCATIONS: Between Columns 2-3, 7-8, 12-13\n";
        
        showAvailabilityStatus();
        
        cout << "\n" << string(80, '=') << "\n";
    }
    
    void showAvailabilityStatus() {
        cout << "\nCURRENT AVAILABILITY:\n";
        
        bool allTypesAvailable = true;
        
        vector<VehicleCategory> vehicleTypes = {
            VehicleCategory::BICYCLE, VehicleCategory::MOTORCYCLE, VehicleCategory::CAR_5SEATER,
            VehicleCategory::CAR_7SEATER, VehicleCategory::ELECTRIC_VEHICLE, VehicleCategory::DISABLED_VEHICLE
        };
        
        for (auto vehicle : vehicleTypes) {
            vector<string> availableSpots = findAvailableSpots(vehicle);
            if (availableSpots.empty()) {
                showFullMessage(vehicle);
                allTypesAvailable = false;
            } else {
                cout << " ";
                switch (vehicle) {
                    case VehicleCategory::BICYCLE: cout << "Bicycles: " << availableSpots.size() << " spots"; break;
                    case VehicleCategory::MOTORCYCLE: cout << "Motorcycles: " << availableSpots.size() << " spots"; break;
                    case VehicleCategory::CAR_5SEATER: cout << "5-Seater Cars: " << availableSpots.size() << " spots"; break;
                    case VehicleCategory::CAR_7SEATER: cout << "7-Seater Cars: " << availableSpots.size() << " spots"; break;
                    case VehicleCategory::ELECTRIC_VEHICLE: cout << "EVs: " << availableSpots.size() << " spots"; break;
                    case VehicleCategory::DISABLED_VEHICLE: cout << "Disabled Vehicles: " << availableSpots.size() << " spots"; break;
                }
                cout << " available\n";
            }
        }
        
        if (!allTypesAvailable) {
            cout << "\nSome vehicle types have no available spots.\n";
        }
        
        vector<string> maintenanceSpots = getMaintenanceSpots();
        if (!maintenanceSpots.empty()) {
            cout << "\nMAINTENANCE: " << maintenanceSpots.size() << " spots under repair\n";
        }
    }
    
    void showFullMessage(VehicleCategory vehicle) {
        cout << " [FULL] ";
        switch (vehicle) {
            case VehicleCategory::BICYCLE:
                cout << "BICYCLE PARKING FULL";
                break;
            case VehicleCategory::MOTORCYCLE:
                cout << "MOTORCYCLE PARKING FULL";
                break;
            case VehicleCategory::CAR_5SEATER:
                cout << "5-SEATER CAR PARKING FULL";
                break;
            case VehicleCategory::CAR_7SEATER:
                cout << "7-SEATER CAR PARKING FULL";
                break;
            case VehicleCategory::ELECTRIC_VEHICLE:
                cout << "EV CHARGING FULL";
                break;
            case VehicleCategory::DISABLED_VEHICLE:
                cout << "DISABLED PARKING FULL";
                break;
        }
        cout << "\n";
    }
    
    string getStatusSymbol(SpotState status) {
        switch (status) {
            case SpotState::AVAILABLE: return " ";
            case SpotState::BOOKED: return "R";
            case SpotState::TAKEN: return "X";
            case SpotState::UNDER_MAINTENANCE: return "M";
            default: return "?";
        }
    }
    
    string getTypeSymbol(ParkingSpotCategory type) {
        switch (type) {
            case ParkingSpotCategory::REGULAR_SPOT: return "REG";
            case ParkingSpotCategory::PREMIUM_SPOT: return "PRM";
            case ParkingSpotCategory::EV_CHARGING_SPOT: return "EV ";
            case ParkingSpotCategory::MOTORCYCLE_SPOT: return "MC ";
            case ParkingSpotCategory::COMPACT_SPOT: return "CMP";
            case ParkingSpotCategory::DISABLED_SPOT: return "DIS";
            case ParkingSpotCategory::FAMILY_SPOT: return "FAM";
            case ParkingSpotCategory::LARGE_SPOT: return "LRG";
            case ParkingSpotCategory::SMALL_SPOT: return "SML";
            case ParkingSpotCategory::BICYCLE_SPOT: return "CYC";
            default: return "???";
        }
    }
    
    ParkingSpot* findSpotById(const string& spotId) {
        auto it = spotDirectory.find(spotId);
        if (it != spotDirectory.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    vector<vector<ParkingSpot*>>& getParkingGrid() { return parkingGrid; }
    unordered_map<string, ParkingSpot*>& getSpotDirectory() { return spotDirectory; }
    
    bool reserveParkingSpot(const string& spotId, VehicleCategory vehicle, int durationHours = 1) {
        ParkingSpot* spot = findSpotById(spotId);
        if (!spot) {
            cout << "Spot '" << spotId << "' not found!\n";
            cout << "Valid spots: A1, B2, C3, etc.\n";
            return false;
        }
        
        if (spot->getCurrentStatus() != SpotState::AVAILABLE) {
            cout << "Spot " << spotId << " unavailable! Current: ";
            switch (spot->getCurrentStatus()) {
                case SpotState::BOOKED: cout << "Reserved\n"; break;
                case SpotState::TAKEN: cout << "Occupied\n"; break;
                case SpotState::UNDER_MAINTENANCE: cout << "Maintenance\n"; break;
                default: cout << "Unknown\n";
            }
            return false;
        }
        
        if (!spot->canVehicleParkHere(vehicle, spot->getSpotCategory())) {
            cout << "Vehicle incompatible with spot " << spotId << "!\n";
            cout << "This spot is for " << getTypeSymbol(spot->getSpotCategory()) << " type.\n";
            cout << "Your vehicle: ";
            switch (vehicle) {
                case VehicleCategory::BICYCLE: cout << "Bicycle"; break;
                case VehicleCategory::MOTORCYCLE: cout << "Motorcycle"; break;
                case VehicleCategory::CAR_5SEATER: cout << "5-Seater Car"; break;
                case VehicleCategory::CAR_7SEATER: cout << "7-Seater Car"; break;
                case VehicleCategory::ELECTRIC_VEHICLE: cout << "Electric Vehicle"; break;
                case VehicleCategory::DISABLED_VEHICLE: cout << "Disabled Vehicle"; break;
            }
            cout << "\n";
            return false;
        }
        
        return spot->reserveSpot(vehicle, durationHours);
    }
    
    bool occupyParkingSpot(const string& spotId, const string& userId) {
        ParkingSpot* spot = findSpotById(spotId);
        if (!spot) {
            cout << "Spot '" << spotId << "' not found!\n";
            return false;
        }
        
        if (spot->getCurrentStatus() != SpotState::AVAILABLE && spot->getCurrentStatus() != SpotState::BOOKED) {
            cout << "Spot " << spotId << " not available for parking!\n";
            return false;
        }
        
        return spot->occupySpot(userId);
    }
    
    vector<string> findAvailableSpots(VehicleCategory vehicle) {
        vector<string> availableList;
        for (const auto& spotPair : spotDirectory) {
            if (spotPair.second->getCurrentStatus() == SpotState::AVAILABLE && 
                spotPair.second->canVehicleParkHere(vehicle, spotPair.second->getSpotCategory())) {
                availableList.push_back(spotPair.first);
            }
        }
        return availableList;
    }
    
    bool releaseParkingSpot(const string& spotId) {
        ParkingSpot* spot = findSpotById(spotId);
        if (!spot) {
            cout << "Spot '" << spotId << "' not found!\n";
            cout << "Valid spots: A1, B2, C3, etc.\n";
            return false;
        }
        
        if (spot->getCurrentStatus() == SpotState::AVAILABLE) {
            cout << "Spot " << spotId << " is already available!\n";
            return false;
        }
        
        return spot->releaseSpot();
    }
    
    string getSpotOccupant(const string& spotId) {
        ParkingSpot* spot = findSpotById(spotId);
        if (!spot) {
            return "";
        }
        return spot->getOccupantId();
    }
    
    bool markSpotForMaintenance(const string& spotId) {
        ParkingSpot* spot = findSpotById(spotId);
        if (!spot) {
            cout << "Spot '" << spotId << "' not found!\n";
            cout << "Valid spots: A1, B2, C3, etc.\n";
            return false;
        }
        
        if (spot->setMaintenanceMode()) {
            cout << "Spot " << spotId << " marked for maintenance.\n";
            return true;
        }
        return false;
    }
    
    bool removeSpotMaintenance(const string& spotId) {
        ParkingSpot* spot = findSpotById(spotId);
        if (!spot) {
            cout << "Spot '" << spotId << "' not found!\n";
            cout << "Valid spots: A1, B2, C3, etc.\n";
            return false;
        }
        
        if (spot->removeMaintenanceMode()) {
            cout << "Spot " << spotId << " maintenance completed.\n";
            return true;
        }
        return false;
    }
    
    vector<string> getMaintenanceSpots() {
        vector<string> maintenanceList;
        for (const auto& spotPair : spotDirectory) {
            if (spotPair.second->getCurrentStatus() == SpotState::UNDER_MAINTENANCE) {
                maintenanceList.push_back(spotPair.first);
            }
        }
        return maintenanceList;
    }
    
    void showMaintenanceSpots() {
        vector<string> maintenanceSpots = getMaintenanceSpots();
        if (maintenanceSpots.empty()) {
            cout << "No spots under maintenance.\n";
        } else {
            cout << "\nSPOTS UNDER MAINTENANCE:\n";
            for (const string& spotId : maintenanceSpots) {
                ParkingSpot* spot = findSpotById(spotId);
                cout << "• " << spotId << " (" << getTypeSymbol(spot->getSpotCategory()) << ")\n";
            }
        }
    }
    
    void saveCurrentLayout() {
        vector<vector<string>> layoutGrid(GRID_ROWS, vector<string>(GRID_COLUMNS));
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLUMNS; col++) {
                ParkingSpot* spot = parkingGrid[row][col];
                string statusText;
                switch(spot->getCurrentStatus()) {
                    case SpotState::AVAILABLE: statusText = "Available"; break;
                    case SpotState::BOOKED: statusText = "Reserved"; break;
                    case SpotState::TAKEN: statusText = "Occupied"; break;
                    case SpotState::UNDER_MAINTENANCE: statusText = "Maintenance"; break;
                }
                layoutGrid[row][col] = spot->getSpotId() + "|" + statusText + "|" + getTypeSymbol(spot->getSpotCategory());
            }
        }
        DataStorage::saveSpotStatus(layoutGrid);
    }
};

// Implement DataStorage spot database methods
void DataStorage::saveSpotDatabase(ParkingLayoutManager& layoutManager) {
    ofstream outputFile(SPOT_DATABASE_FILE);
    if (outputFile.is_open()) {
        time_t currentTime = time(0);  // Store time first
        outputFile << "=== PARKING SPOT DATABASE ===\n";
        outputFile << "Format: SpotID|Category|Row|Col\n";
        outputFile << "Last Updated: " << ctime(&currentTime) << "\n";   
        auto& parkingGrid = layoutManager.getParkingGrid();
        for (int row = 0; row < 6; row++) {
            for (int col = 0; col < 12; col++) {
                ParkingSpot* spot = parkingGrid[row][col];
                outputFile << spot->getSpotId() << "|" 
                          << static_cast<int>(spot->getSpotCategory()) << "|"
                          << row << "|" << col << "\n";
            }
        }
        outputFile.close();
    }
}

bool DataStorage::loadSpotDatabase(ParkingLayoutManager& layoutManager) {
  ofstream outputFile(SPOT_DATABASE_FILE, ios::app);
    if (outputFile.is_open()) {
        time_t currentTime = time(0);  // Store time first
        outputFile << "\n=== SPOT STATES SNAPSHOT ===\n";
        outputFile << "Timestamp: " << ctime(&currentTime);
    ifstream inputFile(SPOT_DATABASE_FILE);
    if (!inputFile) {
        return false; 
    }
    // Database exists, but we'll still use the programmed layout
    // This ensures consistency with the display logic
    inputFile.close();
    return true;
}
}

void DataStorage::saveSpotStates(ParkingLayoutManager& layoutManager) {
    ofstream outputFile(SPOT_DATABASE_FILE, ios::app);
    if (outputFile.is_open()) {
        time_t currentTime = time(0);  // FIX: Store time in variable first
        outputFile << "\n=== SPOT STATES SNAPSHOT ===\n";
        outputFile << "Timestamp: " << ctime(&currentTime);  // FIX: Use variable
        outputFile << "Format: SpotID|Status|Occupant|ReservationTime\n";
        
        auto& spotDirectory = layoutManager.getSpotDirectory();
        for (const auto& spotPair : spotDirectory) {
            ParkingSpot* spot = spotPair.second;
            if (spot->getCurrentStatus() != SpotState::AVAILABLE) {
                outputFile << spot->getSpotId() << "|"
                          << static_cast<int>(spot->getCurrentStatus()) << "|"
                          << spot->getOccupantId() << "|"
                          << spot->getReservationTimestamp() << "\n";
            }
        }
        outputFile.close();
    }
}

bool DataStorage::loadSpotStates(ParkingLayoutManager& layoutManager) {
    ifstream inputFile(SPOT_DATABASE_FILE);
    if (!inputFile) {
        return false;
    }
    
    string line;
    bool inStatesSection = false;
    
    while (getline(inputFile, line)) {
        if (line.find("=== SPOT STATES SNAPSHOT ===") != string::npos) {
            inStatesSection = true;
            continue;
        }
        
        if (inStatesSection && line.find('|') != string::npos) {
            vector<string> fields;
            stringstream ss(line);
            string field;
            
            while (getline(ss, field, '|')) {
                fields.push_back(field);
            }
            
            if (fields.size() >= 4) {
                // Add validation before conversion
                if (fields[0].empty() || fields[1].empty() || fields[3].empty()) {
                    continue; // Skip invalid lines
                }
                
                try {
                    string spotId = fields[0];
                    int status = stoi(fields[1]);
                    string occupant = fields[2];
                    time_t reservationTime = stoll(fields[3]);
                    
                    ParkingSpot* spot = layoutManager.findSpotById(spotId);
                    if (spot) {
                        spot->setCurrentStatus(static_cast<SpotState>(status));
                        spot->setOccupantId(occupant);
                        spot->setReservationTimestamp(reservationTime);
                    }
                } catch (const std::exception& e) {
                    // Skip lines that can't be parsed
                    continue;
                }
            }
        }
    }
    inputFile.close();
    return true;
}

// Administrator class for system management
class SystemAdministrator {
private:
    string adminUsername;
    string adminPassword;
    static const string DEFAULT_ADMIN_USER;
    static const string DEFAULT_ADMIN_PASS;

public:
    SystemAdministrator() {
        adminUsername = DEFAULT_ADMIN_USER;
        adminPassword = DEFAULT_ADMIN_PASS;
    }

    // Verify admin credentials
    static bool verifyAdminAccess() {
        string inputUser, inputPass;
        cout << "\n=== ADMINISTRATOR LOGIN ===\n";
        cout << "Username: ";
        getline(cin, inputUser);
        cout << "Password: ";
        getline(cin, inputPass);

        string lowercaseUser = inputUser;
        for (char &c : lowercaseUser) {
            c = tolower(c);
        }

        if (lowercaseUser == DEFAULT_ADMIN_USER && inputPass == DEFAULT_ADMIN_PASS) {
            cout << "Access granted! Welcome Administrator.\n";
            return true;
        } else {
            cout << "Invalid credentials! Access denied.\n";
            return false;
        }
    }

    // Display admin menu options
    static void showAdminMenu() {
        cout << "\n=== ADMINISTRATOR PANEL ===\n";
        cout << "1. Mark spot for maintenance\n";
        cout << "2. Remove spot from maintenance\n";
        cout << "3. View maintenance spots\n";
        cout << "4. View revenue statistics\n";
        cout << "5. View transaction history\n";
        cout << "6. View all user data\n";
        cout << "7. View spot status\n";
        cout << "8. Delete user account (Admin)\n";
        cout << "9. Return to main menu\n";
        cout << "Select option: ";
    }

    // Admin function to delete any user account
    static bool adminDeleteUserAccount() {
        cout << "\n=== ADMIN USER ACCOUNT DELETION ===\n";
        cout << "Enter User ID to delete: ";
        string userId;
        getline(cin, userId);
        
        cout << "WARNING: This will permanently delete user '" << userId << "' and all their data!\n";
        cout << "Type 'CONFIRM_DELETE' to proceed: ";
        string confirmation;
        getline(cin, confirmation);
        
        if (confirmation != "CONFIRM_DELETE") {
            cout << "Account deletion cancelled.\n";
            return false;
        }
        
        if (DataStorage::deleteUserAccount(userId)) {
            DataStorage::deleteUserTransactions(userId);
            cout << " User account '" << userId << "' successfully deleted by admin.\n";
            return true;
        }
        
        return false;
    }
};

// Default admin credentials
const string SystemAdministrator::DEFAULT_ADMIN_USER = "sps";
const string SystemAdministrator::DEFAULT_ADMIN_PASS = "464442";

// Late fee calculation system
class LateFeeCalculator {
private:
    static const int HOURLY_FINE_RATE = 50;
    static const int MAX_FINE_HOURS = 24;
    static const int DAILY_MAX_FINE = 1000;
    
public:
    // Calculate late fee based on parking duration
    static int computeLateFee(time_t entryTimestamp, time_t exitTimestamp, int hourlyCharge, VehicleCategory vehicle) {
        double totalHours = difftime(exitTimestamp, entryTimestamp) / 3600.0;
        int allowedHours = getAllowedParkingHours(vehicle);
        
        if (totalHours <= allowedHours) {
            return 0;
        }
        
        int extraHours = static_cast<int>(ceil(totalHours - allowedHours));
        int fineAmount = 0;
        
        if (extraHours <= MAX_FINE_HOURS) {
            fineAmount = extraHours * HOURLY_FINE_RATE;
        } else {
            int extraDays = (extraHours + 23) / 24;
            fineAmount = extraDays * DAILY_MAX_FINE;
        }
        
        fineAmount = applyVehicleDiscount(fineAmount, vehicle);
        fineAmount = min(fineAmount, hourlyCharge * 10);
        
        return fineAmount;
    }
    
    // Display fine rules to users
    static void displayFinePolicy() {
        cout << "\n=== LATE FEE POLICY ===\n";
        cout << "~ Base Fine: Rs " << HOURLY_FINE_RATE << " per hour after allowed time\n";
        cout << "~ Maximum Fine: Rs " << DAILY_MAX_FINE << " per day after 24 hours\n";
        cout << "~ Allowed Parking Hours:\n";
        cout << "  - Bicycles/Motorcycles: 12 hours\n";
        cout << "  - Cars/EVs: 8 hours\n";
        cout << "  - Disabled Vehicles: 24 hours\n";
        cout << "~ Special Discounts:\n";
        cout << "  - Disabled: 50% discount on fines\n";
        cout << "  - EVs: 20% discount on fines\n";
        cout << "  - Bicycles: 40% discount on fines\n";
        cout << "=======================\n";
    }
    
private:
    // Get allowed parking hours by vehicle type
    static int getAllowedParkingHours(VehicleCategory vehicle) {
        switch (vehicle) {
            case VehicleCategory::BICYCLE:
            case VehicleCategory::MOTORCYCLE:
                return 12;
            case VehicleCategory::CAR_5SEATER:
            case VehicleCategory::CAR_7SEATER:
            case VehicleCategory::ELECTRIC_VEHICLE:
                return 8;
            case VehicleCategory::DISABLED_VEHICLE:
                return 24;
            default:
                return 8;
        }
    }
    
    // Apply discounts based on vehicle type
    static int applyVehicleDiscount(int fine, VehicleCategory vehicle) {
        switch (vehicle) {
            case VehicleCategory::DISABLED_VEHICLE:
                return static_cast<int>(fine * 0.5);
            case VehicleCategory::ELECTRIC_VEHICLE:
                return static_cast<int>(fine * 0.8);
            case VehicleCategory::BICYCLE:
                return static_cast<int>(fine * 0.6);
            default:
                return fine;
        }
    }
};

// Payment processing system
class PaymentProcessor {
  static int bikeRevenueTotal;
  static int carRevenueTotal;
  static int disabledRevenueTotal;
  static int evRevenueTotal;

 private:
  PaymentProcessor() {}

 public:
  // Add revenue by vehicle type
  static void addToRevenue(int amount, VehicleCategory vehicle) {
    if (vehicle == VehicleCategory::MOTORCYCLE || vehicle == VehicleCategory::BICYCLE)
      bikeRevenueTotal += amount;
    else if (vehicle == VehicleCategory::CAR_5SEATER || vehicle == VehicleCategory::CAR_7SEATER)
      carRevenueTotal += amount;
    else if (vehicle == VehicleCategory::DISABLED_VEHICLE)
      disabledRevenueTotal += amount;
    else if (vehicle == VehicleCategory::ELECTRIC_VEHICLE)
      evRevenueTotal += amount;
      
    saveRevenueData();
  }
  
  // Display revenue summary
  static void showRevenueSummary() {
    cout << "--- REVENUE BREAKDOWN --- \n";
    cout << "Two-Wheeler Revenue: Rs " << bikeRevenueTotal << "\n";
    cout << "Car Revenue: Rs " << carRevenueTotal << "\n";
    cout << "Disabled Vehicle Revenue: Rs " << disabledRevenueTotal << "\n";
    cout << "EV Revenue: Rs " << evRevenueTotal << "\n";
    cout << "TOTAL REVENUE: Rs " << (bikeRevenueTotal + carRevenueTotal + disabledRevenueTotal + evRevenueTotal) << "\n";
  }
  
  // Save revenue data to file
  static void saveRevenueData() {
    DataStorage::generateRevenueReport(bikeRevenueTotal, carRevenueTotal, disabledRevenueTotal, evRevenueTotal);
  }

  // Process cash payment
  static void processCashPayment(int amount) {
    cout << "Payment Successful! Amount: Rs." << amount << " (Cash)\n";
  }
  
  // Process card payment
  static void processCardPayment(int amount, const long long &cardNumber) {
    cout << "Payment Successful! Amount: Rs." << amount << " (Card)\n";
  }
  
  // Process UPI payment
  static void processUPIPayment(int amount, const string &upiId) {
    cout << "Payment Successful! Amount: Rs." << amount << " (UPI)\n";
  }
  
  // Process net banking payment
  static void processNetBankingPayment(int amount, const long long &accountNumber, const int &bankCode) {
    cout << "Payment Successful! Amount: Rs." << amount << " (Net Banking)\n";
  }
};

// Initialize revenue counters
int PaymentProcessor::bikeRevenueTotal = 0;
int PaymentProcessor::carRevenueTotal = 0;
int PaymentProcessor::disabledRevenueTotal = 0;
int PaymentProcessor::evRevenueTotal = 0;

// Base user class
class ParkingSystemUser {
 protected:
  string fullName;
  string userId;
  string userPassword;
  long long contactNumber;
  string emailAddress;
  VehicleCategory userVehicleType;
  ParkingSpotCategory preferredSpotType;
  static int totalUserCount;
  time_t parkingStartTime;
  time_t parkingEndTime;
  int loyaltyPoints;
  int usageCount;
  int userRating;
  string userFeedback;
  bool currentlyParked;
  string currentSpotId;
  int bookedHourlyRate;
  int bookedHours;
  string securityQuestion;
  string securityAnswer;

 public:
  ParkingSystemUser(string name, string id, string password, long long contact, VehicleCategory vehicleType,
       ParkingSpotCategory spotType, string email = "not_provided", string secQuestion = "", string secAnswer = "") {
    this->fullName = name;
    this->userId = id;
    this->userPassword = password;
    this->contactNumber = contact;
    this->emailAddress = email;
    this->userVehicleType = vehicleType;
    this->preferredSpotType = spotType;
    this->loyaltyPoints = 0;
    this->userRating = 0;
    this->usageCount = 0;
    this->currentlyParked = false;
    this->currentSpotId = "";
    this->bookedHourlyRate = 0;
    this->bookedHours = 0;
    this->securityQuestion = secQuestion;
    this->securityAnswer = secAnswer;
    totalUserCount++;
    parkingStartTime = 0;
    parkingEndTime = 0;
  }

  virtual void welcomeUser() const {
    cout << "Welcome back, " << fullName << "!\n";
  }
  
  virtual void showParkingTip() const {
    cout << fullName << ", please select your parking spot carefully.\n";
  }

  virtual ~ParkingSystemUser() {}

  virtual void displayUserInfo() const {
    string vehicleDescription;
    switch(userVehicleType) {
        case VehicleCategory::BICYCLE: vehicleDescription = "Bicycle"; break;
        case VehicleCategory::CAR_5SEATER: vehicleDescription = "5-Seater Car"; break;
        case VehicleCategory::CAR_7SEATER: vehicleDescription = "7-Seater Car"; break;
        case VehicleCategory::MOTORCYCLE: vehicleDescription = "Motorcycle"; break;
        case VehicleCategory::ELECTRIC_VEHICLE: vehicleDescription = "Electric Vehicle"; break;
        case VehicleCategory::DISABLED_VEHICLE: vehicleDescription = "Disabled Vehicle"; break;
        default: vehicleDescription = "Unknown";
    }
    
    cout << "User: " << fullName << " | ID: " << userId << " | Contact: " << contactNumber
         << " | Email: " << emailAddress << " | Vehicle: " << vehicleDescription
         << " | Rating: "
         << (userRating == 0 ? "Not rated" : to_string(userRating) + " star(s)")
         << " | Feedback: " << (userFeedback.empty() ? "No feedback" : userFeedback) << "\n";
  }

  // Allow user to change vehicle type
  void updateVehicleType() {
    cout << "\n=== UPDATE VEHICLE TYPE ===\n";
    cout << "Current vehicle: ";
    switch(userVehicleType) {
        case VehicleCategory::BICYCLE: cout << "Bicycle"; break;
        case VehicleCategory::CAR_5SEATER: cout << "5-Seater Car"; break;
        case VehicleCategory::CAR_7SEATER: cout << "7-Seater Car"; break;
        case VehicleCategory::MOTORCYCLE: cout << "Motorcycle"; break;
        case VehicleCategory::ELECTRIC_VEHICLE: cout << "Electric Vehicle"; break;
        case VehicleCategory::DISABLED_VEHICLE: cout << "Disabled Vehicle"; break;
    }
    cout << "\n";
    
    int choice = 0;
    string input;
    do {
      cout << "Select New Vehicle Type:\n1. 5-Seater Car\n2. 7-Seater Car\n3. Motorcycle\n4. Bicycle\n5. Electric Vehicle\n6. Disabled Vehicle\n";
      getline(cin, input);
      if (!parseIntInRange(input, choice, 1, 6)) {
        cout << "Invalid selection! Please choose 1-6: ";
        choice = 0;
      }
    } while (choice < 1 || choice > 6);

    VehicleCategory oldType = userVehicleType;
    ParkingSpotCategory oldSpotType = preferredSpotType;

    switch (choice) {
      case 1:
        userVehicleType = VehicleCategory::CAR_5SEATER;
        preferredSpotType = ParkingSpotCategory::REGULAR_SPOT;
        break;
      case 2:
        userVehicleType = VehicleCategory::CAR_7SEATER;
        preferredSpotType = ParkingSpotCategory::REGULAR_SPOT;
        break;
      case 3:
        userVehicleType = VehicleCategory::MOTORCYCLE;
        preferredSpotType = ParkingSpotCategory::MOTORCYCLE_SPOT;
        break;
      case 4:
        userVehicleType = VehicleCategory::BICYCLE;
        preferredSpotType = ParkingSpotCategory::BICYCLE_SPOT;
        break;
      case 5:
        userVehicleType = VehicleCategory::ELECTRIC_VEHICLE;
        preferredSpotType = ParkingSpotCategory::EV_CHARGING_SPOT;
        break;
      case 6:
        userVehicleType = VehicleCategory::DISABLED_VEHICLE;
        preferredSpotType = ParkingSpotCategory::DISABLED_SPOT;
        break;
    }

    cout << "Vehicle updated from ";
    switch(oldType) {
        case VehicleCategory::BICYCLE: cout << "Bicycle"; break;
        case VehicleCategory::CAR_5SEATER: cout << "5-Seater Car"; break;
        case VehicleCategory::CAR_7SEATER: cout << "7-Seater Car"; break;
        case VehicleCategory::MOTORCYCLE: cout << "Motorcycle"; break;
        case VehicleCategory::ELECTRIC_VEHICLE: cout << "Electric Vehicle"; break;
        case VehicleCategory::DISABLED_VEHICLE: cout << "Disabled Vehicle"; break;
    }
    cout << " to ";
    switch(userVehicleType) {
        case VehicleCategory::BICYCLE: cout << "Bicycle"; break;
        case VehicleCategory::CAR_5SEATER: cout << "5-Seater Car"; break;
        case VehicleCategory::CAR_7SEATER: cout << "7-Seater Car"; break;
        case VehicleCategory::MOTORCYCLE: cout << "Motorcycle"; break;
        case VehicleCategory::ELECTRIC_VEHICLE: cout << "Electric Vehicle"; break;
        case VehicleCategory::DISABLED_VEHICLE: cout << "Disabled Vehicle"; break;
    }
    cout << "!\n";
  }

  // Verify user password
  bool checkPassword(const string &inputPassword) const {
    return userPassword == inputPassword;
  }
  
  // Helper function for password change flow
    bool changePasswordFlow() {
        cout << "Enter new password (min 6 characters): ";
        string newPass;
        getline(cin, newPass);
        
        if (!isValidPassword(newPass)) {
            return false;
        }
        
        // Update in memory
        userPassword = newPass;
        
        // Update in file
        if (DataStorage::updateUserPasswordInFile(userId, newPass)) {
            cout << "✓ Password reset successfully!\n";
            return true;
        } else {
            cout << "✗ Password reset failed!\n";
            return false;
        }
    }

  // Change user password - UPDATED TO PERSIST TO FILE
  void changeUserPassword() {
    string currentPass, newPass;
    cout << "Enter current password: ";
    getline(cin, currentPass);
    
    if (!checkPassword(currentPass)) {
        cout << "Incorrect current password!\n";
        return;
    }
    
    cout << "Enter new password (min 6 characters): ";
    getline(cin, newPass);
    
    if (!isValidPassword(newPass)) {
        return;
    }
    
    // Update in memory
    userPassword = newPass;
    
    // Update in file - FIXED: use newPass instead of newPassword
    if (DataStorage::updateUserPasswordInFile(userId, newPass)) {
        cout << "Password updated successfully and saved to file!\n";
    } else {
        cout << "Password updated in memory but failed to save to file!\n";
    }
  }

  // Reset password using security question
    bool resetPasswordWithSecurityQuestion() {
        cout << "\n=== PASSWORD RESET ===\n";
        
        // First check in-memory security questions
        if (!securityQuestion.empty() && !securityAnswer.empty()) {
            cout << "Security Question (from memory): " << securityQuestion << "\n";
            cout << "Enter your answer: ";
            string answer;
            getline(cin, answer);
            
            if (answer == securityAnswer) {
                cout << "✓ Security answer correct!\n";
                return changePasswordFlow();
            } else {
                cout << "✗ Incorrect security answer!\n";
                return false;
            }
        }
        
        // If not in memory, check file
        cout << "Checking file for security questions...\n";
        auto qa = DataStorage::getSecurityQA(userId);
        
        cout << "DEBUG: Retrieved Q: '" << qa.first << "' A: '" << qa.second << "'\n";
        
        if (qa.first.empty() || qa.second.empty()) {
            cout << "✗ No security question found for user: " << userId << "\n";
            cout << "Please register security questions in User Settings.\n";
            return false;
        }
        
        cout << "Security Question (from file): " << qa.first << "\n";
        cout << "Enter your answer: ";
        string answer;
        getline(cin, answer);
        
        if (answer == qa.second) {
            cout << "✓ Security answer correct!\n";
            return changePasswordFlow();
        } else {
            cout << "✗ Incorrect security answer!\n";
            return false;
        }
    }
  // Delete user account
  bool deleteUserAccount(unordered_map<string, ParkingSystemUser*> &userDatabase) {
    // Check if user is currently parked
    if (currentlyParked) {
        cout << "Cannot delete account while vehicle is parked!\n";
        cout << "Please release spot " << currentSpotId << " first.\n";
        return false;
    }

    cout << "\n=== ACCOUNT DELETION CONFIRMATION ===\n";
    cout << "User: " << fullName << " (ID: " << userId << ")\n";
    cout << "This action will:\n";
    cout << "• Permanently delete your profile\n";
    cout << "• Remove all your transaction history\n";
    cout << "• Delete your loyalty points and ratings\n";
    cout << "• This action cannot be undone!\n\n";

    cout << "Type 'DELETE' to confirm: ";
    string confirmation;
    getline(cin, confirmation);

    if (confirmation != "DELETE") {
        cout << "Account deletion cancelled.\n";
        return false;
    }

    // Remove from file storage first
    if (DataStorage::deleteUserAccount(userId)) {
        DataStorage::deleteUserTransactions(userId);
        
        // Then remove from memory
        userDatabase.erase(userId);
        
        cout << "\n Account successfully deleted!\n";
        cout << "We're sorry to see you go. Thank you for using our parking system.\n";
        return true;
    }

    return false;
  }

  // Display account deletion option in user settings
  void showAccountDeletionMenu() {
    cout << "\n=== ACCOUNT MANAGEMENT ===\n";
    cout << "1. Change Vehicle Type\n2. Change Password\n";
    cout << "3. View Profile\n4. Delete Account\n5. Reset Password (Security Question)\n6. Back to Main Menu\n";
    cout << "Enter choice: ";
  }

  static int getTotalUsers() { return totalUserCount; }
  string getUserId() const { return userId; }
  string getUserPassword() const { return userPassword; }
  bool isCurrentlyParked() const { return currentlyParked; }
  string getCurrentSpotId() const { return currentSpotId; }
  time_t getParkingStartTime() const { return parkingStartTime; }
  VehicleCategory getVehicleType() const { return userVehicleType; }
  ParkingSpotCategory getSpotType() const { return preferredSpotType; }
  int getBookedHourlyRate() const { return bookedHourlyRate; }
  int getBookedHours() const { return bookedHours; }
  string getSecurityQuestion() const { return securityQuestion; }
  string getSecurityAnswer() const { return securityAnswer; }

  void setParkingStatus(bool parked, const string& spotId = "", int hourlyRate = 0, int duration = 0) {
    currentlyParked = parked;
    if (parked) {
        currentSpotId = spotId;
        parkingStartTime = time(NULL);
        bookedHourlyRate = hourlyRate;
        bookedHours = duration;
    } else {
        currentSpotId = "";
        bookedHourlyRate = 0;
        bookedHours = 0;
    }
  }

  void saveUserData() const {
    DataStorage::storeUserData(fullName, userId, userPassword, contactNumber, emailAddress, userVehicleType, userRating, userFeedback, securityQuestion, securityAnswer);
  }

  static void loadUserDataFromFile() {
    DataStorage::showUserData();
  }

  void recordParkingUsage() {
    usageCount++;
    loyaltyPoints += 5;
  }

  void showLoyaltyInfo() const {
    cout << fullName << " usage count: " << usageCount
         << " | Loyalty points: " << loyaltyPoints << "\n";
  }

  virtual int applyLoyaltyDiscount(int baseAmount) const {
    if (loyaltyPoints >= 50) {
      cout << "Loyalty discount applied! 10% off.\n";
      return static_cast<int>(baseAmount * 0.9);
    }
    return baseAmount;
  }

  void setUserRating(int rating) {
    if (rating < 1 || rating > 5) {
      cout << "Rating must be 1-5 stars!\n";
      return;
    }
    userRating = rating;
    cout << "Rating recorded: " << userRating << " star(s)\n";
    if (rating == 1 || rating == 2) cout << "We'll improve! ";
    cout << "Thank you for your feedback!\n";
  }

  void setUserFeedback(const string &feedback) {
    userFeedback = feedback;
    cout << "Feedback recorded!\n";
  }

  int calculateParkingFee() {
    if (!currentlyParked) {
        cout << "User is not currently parked!\n";
        return 0;
    }
    
    if (bookedHourlyRate == 0) {
        cout << "Error: Booking rate missing!\n";
        return 0;
    }
    
    parkingEndTime = time(NULL);
    double actualHours = difftime(parkingEndTime, parkingStartTime) / 3600.0;
    
    int baseAmount = bookedHourlyRate * bookedHours;
    
    int lateFee = 0;
    if (actualHours > bookedHours) {
        int extraHours = static_cast<int>(ceil(actualHours - bookedHours));
        lateFee = extraHours * (bookedHourlyRate * 2);
    }
    
    int totalAmount = baseAmount + lateFee;
    
    cout << "\n=== PARKING FEE CALCULATION ===\n";
    cout << "Booked Hours: " << bookedHours << " hours\n";
    cout << "Actual Hours: " << fixed << setprecision(2) << actualHours << " hours\n";
    cout << "Hourly Rate: Rs " << bookedHourlyRate << "\n";
    cout << "Base Amount: Rs " << baseAmount << " (" << bookedHours << " hours × Rs " << bookedHourlyRate << ")\n";
    
    if (lateFee > 0) {
        cout << "Late Fee: Rs " << lateFee << " (" << (actualHours - bookedHours) << " extra hours)\n";
        cout << "TOTAL: Rs " << totalAmount << "\n";
    } else {
        cout << "Late Fee: Rs 0 (No overtime)\n";
        cout << "TOTAL: Rs " << totalAmount << "\n";
    }
    cout << "===============================\n";
    
    return totalAmount;
  }

  void processParkingPayment(int amount, VehicleCategory vehicle, ParkingSpotCategory spotType, const string& spotId, int duration) {
    string paymentInput;
    int paymentMethod;
    string methodUsed;
    cout << "Select Payment Method:\n1. Cash\n2. Card\n3. UPI\n4. Net Banking\nChoice: ";
    getline(cin, paymentInput);
    if (!parseIntInRange(paymentInput, paymentMethod, 1, 4)) {
        cout << "Invalid choice! Using Cash.\n";
        paymentMethod = 1;
    }
    
    switch (paymentMethod) {
        case 1:
            PaymentProcessor::processCashPayment(amount);
            methodUsed = "Cash";
            break;
        case 2: {
            cout << "Enter Card Number: ";
            long long cardNumber;
            cin >> cardNumber;
            string cardStr = to_string(cardNumber);
            if (cardStr.length() >= 13 && cardStr.length() <= 19){
                PaymentProcessor::processCardPayment(amount, cardNumber);
                methodUsed = "Card";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            else
                cout << "Invalid card number!\n";
            break;
        }
        case 3: {
            cout << "Enter UPI ID: ";
            string upiId;
            getline(cin, upiId);
            PaymentProcessor::processUPIPayment(amount, upiId);
            methodUsed = "UPI";
            break;
        }
        case 4: {
            int bankSelection;
            cout << "1. HDFC\n2. SBI\n3. Axis\n4. ICICI\n5. Bank of Baroda\n6. Punjab National Bank\n7. IndusInd Bank\n";
            cout << "Select Bank: ";
            cin >> bankSelection;
            cout << "Enter Account Number: ";
            long long accountNo;
            cin >> accountNo;
            PaymentProcessor::processNetBankingPayment(amount, accountNo, bankSelection);
            methodUsed = "Net Banking";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    
    PaymentProcessor::addToRevenue(amount, vehicle);
    DataStorage::recordTransaction(userId, vehicle, spotType, spotId, duration, amount, methodUsed);
  }

  friend void showUserContact(const ParkingSystemUser &user);

  ParkingSystemUser &operator++() {
    loyaltyPoints += 10;
    usageCount++;
    cout << fullName << "'s loyalty increased. Points: " << loyaltyPoints
         << ", Usage: " << usageCount << "\n";
    return *this;
  }
};

int ParkingSystemUser::totalUserCount = 0;

void showUserContact(const ParkingSystemUser &user) {
  cout << "Contact: " << user.fullName << " - " << user.contactNumber << "\n";
}

// User authentication function
ParkingSystemUser* authenticateParkingUser(unordered_map<string, ParkingSystemUser*> &users) {
    string userId, password;
    cout << "Enter User ID: ";
    getline(cin, userId);
    
    // Trim and validate input - ADD THIS VALIDATION
    userId.erase(0, userId.find_first_not_of(" \t\n\r"));
    userId.erase(userId.find_last_not_of(" \t\n\r") + 1);
    
    if (userId.empty()) {
        cout << "User ID cannot be empty!\n";
        return nullptr;
    }
    
    auto userIt = users.find(userId);
    if (userIt == users.end()) {
        cout << "User not found!\n";
        return nullptr;
    }
    
    cout << "Enter Password: ";
    getline(cin, password);
    
    ParkingSystemUser* user = userIt->second;
    if (user->checkPassword(password)) {
        cout << "Welcome " << user->getUserId() << "!\n";
        return user;
    } else {
        cout << "Invalid password!\n";
        return nullptr;
    }
}

// Password reset function using security question
ParkingSystemUser* resetPasswordWithSecurityQuestion(unordered_map<string, ParkingSystemUser*> &users) {
    string userId;
    cout << "Enter User ID for password reset: ";
    getline(cin, userId);
    
    // Trim and validate input
    userId.erase(0, userId.find_first_not_of(" \t\n\r"));
    userId.erase(userId.find_last_not_of(" \t\n\r") + 1);
    
    if (userId.empty()) {
        cout << "User ID cannot be empty!\n";
        return nullptr;
    }
    
    auto userIt = users.find(userId);
    if (userIt == users.end()) {
        cout << "User not found!\n";
        return nullptr;
    }
    
    ParkingSystemUser* user = userIt->second;
    if (user->resetPasswordWithSecurityQuestion()) {
        return user;
    }
    
    return nullptr;
}

// Derived user classes for different vehicle types
class Car5SeaterUser : public ParkingSystemUser {
 public:
  Car5SeaterUser(string name, string userId, string password, long long contact,
           string email = "not_provided", string secQuestion = "", string secAnswer = "")
      : ParkingSystemUser(name, userId, password, contact, VehicleCategory::CAR_5SEATER,
             ParkingSpotCategory::REGULAR_SPOT, email, secQuestion, secAnswer) {}

  void displayUserInfo() const override {
    cout << "[5-Seater Car User] ";
    ParkingSystemUser::displayUserInfo();
  }
};

class Car7SeaterUser : public ParkingSystemUser {
 public:
  Car7SeaterUser(string name, string userId, string password, long long contact,
           string email = "not_provided", string secQuestion = "", string secAnswer = "")
      : ParkingSystemUser(name, userId, password, contact, VehicleCategory::CAR_7SEATER,
             ParkingSpotCategory::REGULAR_SPOT, email, secQuestion, secAnswer) {}

  void displayUserInfo() const override {
    cout << "[7-Seater Car User] ";
    ParkingSystemUser::displayUserInfo();
  }
};

class MotorcycleUser : public ParkingSystemUser {
 public:
  MotorcycleUser(string name, string userId, string password, long long contact,
                 string email = "not_provided", string secQuestion = "", string secAnswer = "")
      : ParkingSystemUser(name, userId, password, contact, VehicleCategory::MOTORCYCLE,
             ParkingSpotCategory::MOTORCYCLE_SPOT, email, secQuestion, secAnswer) {}

  void displayUserInfo() const override {
    cout << "[Motorcycle User] ";
    ParkingSystemUser::displayUserInfo();
  }
};

class BicycleUser : public ParkingSystemUser {
 public:
  BicycleUser(string name, string userId, string password, long long contact,
            string email = "not_provided", string secQuestion = "", string secAnswer = "")
      : ParkingSystemUser(name, userId, password, contact, VehicleCategory::BICYCLE, ParkingSpotCategory::BICYCLE_SPOT,
             email, secQuestion, secAnswer) {}

  void displayUserInfo() const override {
    cout << "[Bicycle User] ";
    ParkingSystemUser::displayUserInfo();
  }
};

class ElectricVehicleUser : public ParkingSystemUser {
 public:
  ElectricVehicleUser(string name, string userId, string password, long long contact,
         string email = "not_provided", string secQuestion = "", string secAnswer = "")
      : ParkingSystemUser(name, userId, password, contact, VehicleCategory::ELECTRIC_VEHICLE,
             ParkingSpotCategory::EV_CHARGING_SPOT, email, secQuestion, secAnswer) {}

  void displayUserInfo() const override {
        cout << "[EV User] ";
        ParkingSystemUser::displayUserInfo();
  }

  void welcomeUser() const override {
        cout << "Hello " << fullName << "! Enjoy EV charging benefits today!\n";
  }

  void showParkingTip() const override {
        cout << fullName << ", choose EV charging spots for best experience!\n";
  }

  int applyLoyaltyDiscount(int baseAmount) const override {
    int discounted = ParkingSystemUser::applyLoyaltyDiscount(baseAmount);
    cout << "EV user discount applied! 5% off.\n";
    return static_cast<int>(discounted * 0.95);
  }
};

class DisabledVehicleUser : public ParkingSystemUser {
 public:
  DisabledVehicleUser(string name, string userId, string password, long long contact,
               string email = "not_provided@mail.com", string secQuestion = "", string secAnswer = "")
      : ParkingSystemUser(name, userId, password, contact, VehicleCategory::DISABLED_VEHICLE,
             ParkingSpotCategory::DISABLED_SPOT, email, secQuestion, secAnswer) {}
  
  void showParkingTip() const override {
        cout << fullName << ", accessible spots are available near elevators!\n";
  }
  
  void displayUserInfo() const override {
    cout << "[Disabled Vehicle User] ";
    ParkingSystemUser::displayUserInfo();
  }
};

// Dynamic pricing calculation
class PricingCalculator {
 public:
  static int calculateRate(int occupiedSpots, int totalSpots, VehicleCategory vehicleType,
                     ParkingSpotCategory spotType, bool chargingRequired = false) {
    int baseRate = 0;
    switch (vehicleType) {
      case VehicleCategory::BICYCLE: {
        if (spotType == ParkingSpotCategory::COMPACT_SPOT)
          baseRate = 20;
        else if (spotType == ParkingSpotCategory::BICYCLE_SPOT)
          baseRate = 30;
        else if (spotType == ParkingSpotCategory::FAMILY_SPOT)
          baseRate = 5;
        break;
      }
      case VehicleCategory::MOTORCYCLE: {
        if (spotType == ParkingSpotCategory::COMPACT_SPOT)
          baseRate = 45;
        else if (spotType == ParkingSpotCategory::MOTORCYCLE_SPOT)
          baseRate = 50;
        else if (spotType == ParkingSpotCategory::FAMILY_SPOT)
          baseRate = 10;
        break;
      }
      case VehicleCategory::CAR_5SEATER: {
        if (spotType == ParkingSpotCategory::REGULAR_SPOT)
          baseRate = 100;
        else if (spotType == ParkingSpotCategory::EV_CHARGING_SPOT)
          baseRate = 150;
        else if (spotType == ParkingSpotCategory::PREMIUM_SPOT)
          baseRate = 175;
        else if (spotType == ParkingSpotCategory::SMALL_SPOT)
          baseRate = 85;
        else if (spotType == ParkingSpotCategory::FAMILY_SPOT)
          baseRate = 10;
        break;
      }
      case VehicleCategory::CAR_7SEATER: {
        if (spotType == ParkingSpotCategory::REGULAR_SPOT)
          baseRate = 140;
        else if (spotType == ParkingSpotCategory::EV_CHARGING_SPOT)
          baseRate = 190;
        else if (spotType == ParkingSpotCategory::PREMIUM_SPOT)
          baseRate = 215;
        else if (spotType == ParkingSpotCategory::LARGE_SPOT)
          baseRate = 200;
        else if (spotType == ParkingSpotCategory::FAMILY_SPOT)
          baseRate = 15;
        break;
      }
      case VehicleCategory::DISABLED_VEHICLE: {
        if (spotType == ParkingSpotCategory::DISABLED_SPOT)
          baseRate = 30;
        else if (spotType == ParkingSpotCategory::FAMILY_SPOT)
          baseRate = 5;
        break;
      }
      case VehicleCategory::ELECTRIC_VEHICLE: {
        if (spotType == ParkingSpotCategory::PREMIUM_SPOT)
          baseRate = 175;
        else if (spotType == ParkingSpotCategory::REGULAR_SPOT)
          baseRate = 140;
        else if (spotType == ParkingSpotCategory::EV_CHARGING_SPOT)
          baseRate = 190;
        else if (spotType == ParkingSpotCategory::FAMILY_SPOT)
          baseRate = 14;
        if (chargingRequired) baseRate = static_cast<int>(baseRate * 1.2);
        break;
      }
    }
    
    // Dynamic pricing based on occupancy
    double occupancyRatio = (totalSpots == 0) ? 0.0 : (double)occupiedSpots / totalSpots;
    if (occupancyRatio < 0.3)
      baseRate = static_cast<int>(baseRate * 0.8);  // 20% discount when empty
    else if (occupancyRatio > 0.8)
      baseRate = static_cast<int>(baseRate * 1.5);  // 50% premium when busy
    
    return baseRate;
  }
};

// Load user data from file
void loadUserRecords(unordered_map<string, ParkingSystemUser *> &userDatabase, const string &filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cout << "No existing user data found. Starting fresh.\n";
        return;
    }

    auto trimSpaces = [](string &text) {
        size_t start = text.find_first_not_of(" \t\n\r");
        size_t end = text.find_last_not_of(" \t\n\r");
        if (start == string::npos || end == string::npos)
            text = "";
        else
            text = text.substr(start, end - start + 1);
    };

    string fileLine;
    int loadedCount = 0;
    
    while (getline(inputFile, fileLine)) {
        trimSpaces(fileLine);
        if (fileLine.empty()) continue;

        vector<string> dataFields;
        string field;
        stringstream lineStream(fileLine);

        // Split by pipe character
        while (getline(lineStream, field, '|')) {
            trimSpaces(field);
            dataFields.push_back(field);
        }

        if (dataFields.size() < 6) {
            continue; // Skip invalid lines
        }

        string userName = dataFields[0];
        string userId = dataFields[1];
        string userPassword = dataFields[2];
        long long userContact = 0;

        try {
            userContact = stoll(dataFields[3]);
        } catch (...) {
            continue; // Skip if contact is invalid
        }

        string userEmail = dataFields[4];

        // Extract vehicle type
        int vehicleTypeCode = -1;
        for (const string &field : dataFields) {
            if (field.find("Vehicle:") == 0) {
                try {
                    vehicleTypeCode = stoi(field.substr(8));
                    break;
                } catch (...) {
                    continue;
                }
            }
        }

        if (vehicleTypeCode == -1) {
            continue; // Skip if no vehicle type found
        }

        // Extract security question and answer - FIXED
        string securityQ = "", securityA = "";
        for (const string &f : dataFields) {
            if (f.find("SecurityQ:") == 0) {
                securityQ = f.substr(10);
            } else if (f.find("SecurityA:") == 0) {
                securityA = f.substr(10);
            }
        }

        // Clean up security data
        securityQ.erase(remove(securityQ.begin(), securityQ.end(), '\n'), securityQ.end());
        securityQ.erase(remove(securityQ.begin(), securityQ.end(), '\r'), securityQ.end());
        securityA.erase(remove(securityA.begin(), securityA.end(), '\n'), securityA.end());
        securityA.erase(remove(securityA.begin(), securityA.end(), '\r'), securityA.end());

        // Skip if user already loaded
        if (userDatabase.find(userId) != userDatabase.end()) {
            continue;
        }

        // Create appropriate user type with security questions
        switch (vehicleTypeCode) {
            case 0: 
                userDatabase[userId] = new BicycleUser(userName, userId, userPassword, userContact, userEmail, securityQ, securityA);
                break;
            case 1: 
                userDatabase[userId] = new Car5SeaterUser(userName, userId, userPassword, userContact, userEmail, securityQ, securityA);
                break;
            case 2: 
                userDatabase[userId] = new Car7SeaterUser(userName, userId, userPassword, userContact, userEmail, securityQ, securityA);
                break;
            case 3: 
                userDatabase[userId] = new MotorcycleUser(userName, userId, userPassword, userContact, userEmail, securityQ, securityA);
                break;
            case 4: 
                userDatabase[userId] = new ElectricVehicleUser(userName, userId, userPassword, userContact, userEmail, securityQ, securityA);
                break;
            case 5: 
                userDatabase[userId] = new DisabledVehicleUser(userName, userId, userPassword, userContact, userEmail, securityQ, securityA);
                break;
            default:
                continue;
        }
        
        loadedCount++;
    }

    inputFile.close();
    cout << "Loaded " << loadedCount << " users from file.\n";
}
// Main application function
int main() {

     try {
        // Initialize data storage
        ParkingLayoutManager parkingSystem;
        DataStorage::initializeAllData(parkingSystem);
        
        unordered_map<string, ParkingSystemUser *> userDatabase;
        loadUserRecords(userDatabase, "user_records.txt");  

    int choice = 0;

    do {
        cout << "\n========== SMART PARKING SYSTEM ==========\n";
        cout << "1. Register User\n2. Book Parking Spot\n";
        cout << "3. Admin Login\n4. Rate System (1-5 stars)\n";
        cout << "5. Add Review\n6. Show User Score\n";
        cout << "7. Release Spot & Calculate Payment\n8. View Parking Map\n";
        cout << "9. View Fine Rules\n10. User Settings\n11. Reset Password (Security Question)\n12. Exit\n";
        cout << "Enter choice: ";
        string line;
        if (!getline(cin, line)) break;

        if (!parseIntInRange(line, choice, 1, 12)) {
            cout << "Invalid choice! Please enter a number 1-12.\n";
            continue;
        }

        switch (choice) {
            case 1: {  // Register User
                string name;
                while (true) {
                    cout << "Enter Name: ";
                    getline(cin, name);
                    if (name.empty() || isBlank(name)) {
                        cout << "Please enter a valid name!\n";
                        continue;
                    }
                    if (isValidName(name))
                        break;
                    else
                        cout << "Invalid name! Only letters, spaces, and hyphens allowed.\n";
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

                string password;
                while (true) {
                    cout << "Enter Password (min 6 characters): ";
                    getline(cin, password);
                    if (isValidPassword(password)) break;
                }

                string contactStr;
                while (true) {
                    cout << "Enter Contact (10 digits): ";
                    getline(cin, contactStr);
                    if (contactStr.size() == 10 && isAllDigits(contactStr) && isValidIndianMobile(contactStr)) break;
                    cout << "Invalid contact! Must be exactly 10 digits starting with 6,7,8,9.\n";
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
                    cout << "Invalid email format! Please enter valid email\n";
                }
                
                // Security question setup
                cout << "Set up security question for password recovery:\n";
                string securityQuestion;
                cout << "Enter security question: ";
                getline(cin, securityQuestion);
                
                string securityAnswer;
                cout << "Enter answer to security question: ";
                getline(cin, securityAnswer);
                
                int vChoice = 0;
                string vline;
                do {
                    cout << "Select Vehicle Type:\n1. Five Seater Car\n2. Seven Seater Car\n";
                    cout << "3. Motorcycle\n4. Bicycle\n5. EV\n6. Disabled\n";
                    getline(cin, vline);
                    if (!parseIntInRange(vline, vChoice, 1, 6)) {
                        cout << "Invalid choice! Please enter 1-6: ";
                        vChoice = 0;
                    }
                } while (vChoice < 1 || vChoice > 6);

                if (userDatabase.find(id) != userDatabase.end()) {
                    cout << "UserID already exists!\n";
                    break;
                }

                switch (vChoice) {
                    case 1:
                        userDatabase[id] = new Car5SeaterUser(name, id, password, contact, email, securityQuestion, securityAnswer);
                        break;
                    case 2:
                        userDatabase[id] = new Car7SeaterUser(name, id, password, contact, email, securityQuestion, securityAnswer);
                        break;
                    case 3:
                        userDatabase[id] = new MotorcycleUser(name, id, password, contact, email, securityQuestion, securityAnswer);
                        break;
                    case 4:
                        userDatabase[id] = new BicycleUser(name, id, password, contact, email, securityQuestion, securityAnswer);
                        break;
                    case 5:
                        userDatabase[id] = new ElectricVehicleUser(name, id, password, contact, email, securityQuestion, securityAnswer);
                        break;
                    case 6:
                        userDatabase[id] = new DisabledVehicleUser(name, id, password, contact, email, securityQuestion, securityAnswer);
                        break;
                }

                userDatabase[id]->saveUserData();
                cout << "User Registered Successfully with security question!\n";
                break;
            }

            case 2: {  // Book Parking Spot
                if (userDatabase.empty()) {
                    cout << "No users registered yet!\n";
                    break;
                }
                
                ParkingSystemUser* user = authenticateParkingUser(userDatabase);
                if (!user) break;
                
                if (user->isCurrentlyParked()) {
                    cout << "User " << user->getUserId() << " is already parked at spot " << user->getCurrentSpotId() << "!\n";
                    cout << "Please release current spot before booking new one.\n";
                    break;
                }
                
                user->welcomeUser();
                user->showParkingTip();
                
                parkingSystem.displayParkingLayout();
                
                VehicleCategory type = user->getVehicleType();
                vector<string> availableSpots = parkingSystem.findAvailableSpots(type);
                
                if (availableSpots.empty()) {
                    cout << "\nXXX PARKING FULL for your vehicle type XXX!\n";
                    cout << "Please check back later or try different vehicle type.\n";
                    break;
                }
                
                cout << "\nAvailable spots for your vehicle type: ";
                for (size_t i = 0; i < availableSpots.size(); i++) {
                    cout << availableSpots[i];
                    if (i < availableSpots.size() - 1) cout << ", ";
                }
                cout << "\n";
                
                string spotId;
                cout << "\nEnter Spot ID to book (e.g., A1, B3, C5): ";
                getline(cin, spotId);
                
                for (char &c : spotId) {
                    c = toupper(c);
                }
                
                if (spotId.length() < 2 || !isalpha(spotId[0]) || !isdigit(spotId[1])) {
                    cout << "ERROR: Invalid spot ID format! Use format like A1, B2, etc.\n";
                    break;
                }
                
                ParkingSpot* selectedSpot = parkingSystem.findSpotById(spotId);
                if (!selectedSpot) {
                    cout << "ERROR: Spot ID '" << spotId << "' not found!\n";
                    cout << "Please enter valid Spot ID from map (e.g., A1, B2, C3)\n";
                    break;
                }
                
                int durationHours = 1;
                if (selectedSpot->getSpotCategory() == ParkingSpotCategory::FAMILY_SPOT) {
                    cout << "NOTE: This is FAMILY spot. Minimum booking duration is 8 hours.\n";
                    cout << "Enter booking duration (hours): ";
                    string durationStr;
                    getline(cin, durationStr);
                    
                    if (!parseIntInRange(durationStr, durationHours, 8, 24*7)) {
                        cout << "Invalid duration! Family spots require 8-168 hours.\n";
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
                
                if (!parkingSystem.reserveParkingSpot(spotId, type, durationHours)) {
                    cout << "Failed to reserve spot " << spotId << "!\n";
                    break;
                }
                
                if (!parkingSystem.occupyParkingSpot(spotId, user->getUserId())) {
                    cout << "Failed to occupy spot " << spotId << "!\n";
                    parkingSystem.releaseParkingSpot(spotId);
                    break;
                }
                
                // Calculate rate
                int capacity = 72;
                int occupied = capacity - availableSpots.size();
                ParkingSpotCategory spotType = selectedSpot->getSpotCategory();
                bool isCharging = (spotType == ParkingSpotCategory::EV_CHARGING_SPOT);
                
                int hourlyRate = PricingCalculator::calculateRate(occupied, capacity, type, spotType, isCharging);
                int totalRate = hourlyRate * durationHours;
                
                cout << "Hourly Rate: Rs " << hourlyRate << "\n";
                cout << "Total Rate for " << durationHours << " hours: Rs " << totalRate << "\n";

                user->recordParkingUsage();
                int finalRate = user->applyLoyaltyDiscount(totalRate);

                user->setParkingStatus(true, spotId, hourlyRate, durationHours);
                cout << "Booking confirmed! Payment processed when you release spot.\n";
                
                parkingSystem.saveCurrentLayout();
                DataStorage::saveSpotStates(parkingSystem);
                break;
            }

            case 3: {  // Admin Login
                if (!SystemAdministrator::verifyAdminAccess()) {
                    break;
                }
                
                int adminChoice = 0;
                do {
                    SystemAdministrator::showAdminMenu();
                    string adminLine;
                    if (!getline(cin, adminLine)) break;

                    if (!parseIntInRange(adminLine, adminChoice, 1, 10)) {
                        cout << "Invalid choice! Please enter 1-10.\n";
                        continue;
                    }

                    switch (adminChoice) {
                        case 1: {
                        cout << "Enter Spot ID for maintenance: ";
                        string spotId;
                        getline(cin, spotId);
                        for (char &c : spotId) c = toupper(c);
    
                        if (spotId.length() < 2 || !isalpha(spotId[0]) || !isdigit(spotId[1])) {
                          cout << "ERROR: Invalid spot ID format!\n";
                          break;
                        }
    
                        ParkingSpot* spot = parkingSystem.findSpotById(spotId);
                          if (spot && (spot->getCurrentStatus() == SpotState::BOOKED || 
                            spot->getCurrentStatus() == SpotState::TAKEN)) {
                            cout << "Cannot put spot " << spotId << " under maintenance! ";
                            cout << "It is currently ";
                            if (spot->getCurrentStatus() == SpotState::BOOKED) 
                              cout << "RESERVED";
                            else 
                              cout << "OCCUPIED";
                            cout << " by user: " << spot->getOccupantId() << "\n";
                            break;
                          }
    
                          if (parkingSystem.markSpotForMaintenance(spotId)) {
                            parkingSystem.saveCurrentLayout();
                            DataStorage::saveSpotStates(parkingSystem);
                          }
                          break;
                        }
                        case 2: {
                            cout << "Enter Spot ID to remove maintenance: ";
                            string spotId;
                            getline(cin, spotId);
                            for (char &c : spotId) c = toupper(c);
                            
                            if (spotId.length() < 2 || !isalpha(spotId[0]) || !isdigit(spotId[1])) {
                                cout << "ERROR: Invalid spot ID format!\n";
                                break;
                            }
                            
                            if (parkingSystem.removeSpotMaintenance(spotId)) {
                                parkingSystem.saveCurrentLayout();
                                DataStorage::saveSpotStates(parkingSystem);
                            }
                            break;
                        }
                        case 3:
                            parkingSystem.showMaintenanceSpots();
                            break;
                        case 4:
                            PaymentProcessor::showRevenueSummary();
                            break;
                        case 5: {
                            cout << "Press Enter for transaction history: ";
                            string date;
                            getline(cin, date);
                            DataStorage::showTransactionHistory(date);
                            break;
                        }
                        case 6:
                            DataStorage::showUserData();
                            break;
                        case 7:
                            DataStorage::showSpotStatus();
                            break;
                        case 8:
                            SystemAdministrator::adminDeleteUserAccount();
                            break;
                        case 9:
                            cout << "Returning to main menu...\n";
                            break;
                    }
                } while (adminChoice != 9);
                break;
            }

            case 4: {  // Rate System
                if (userDatabase.empty()) {
                    cout << "No users registered yet!\n";
                    break;
                }
                
                ParkingSystemUser* user = authenticateParkingUser(userDatabase);
                if (!user) break;
                
                cout << "Enter rating (1-5 stars): ";
                string rline;
                getline(cin, rline);
                int r;
                if (!parseIntInRange(rline, r, 1, 5)) {
                    cout << "Invalid rating. Skipping.\n";
                    break;
                }
                user->setUserRating(r);
                user->saveUserData();
                break;
            }

            case 5: {  // Add Review
                if (userDatabase.empty()) {
                    cout << "No users registered yet!\n";
                    break;
                }
                
                ParkingSystemUser* user = authenticateParkingUser(userDatabase);
                if (!user) break;
                
                cout << "Enter your review: ";
                string review;
                getline(cin, review);
                user->setUserFeedback(review);
                user->saveUserData();
                break;
            }

            case 6: {  // Show User Score
                if (userDatabase.empty()) {
                    cout << "No users registered yet!\n";
                    break;
                }
                
                ParkingSystemUser* user = authenticateParkingUser(userDatabase);
                if (!user) break;
                
                user->showLoyaltyInfo();
                break;
            }

            case 7: {  // Release Spot & Calculate Payment
                if (userDatabase.empty()) {
                    cout << "No users registered yet!\n";
                    break;
                }
                
                ParkingSystemUser* user = authenticateParkingUser(userDatabase);
                if (!user) break;
                
                if (!user->isCurrentlyParked()) {
                    cout << "User " << user->getUserId() << " is not currently parked!\n";
                    break;
                }
                
                string parkedSpotId = user->getCurrentSpotId();
                VehicleCategory vehicle = user->getVehicleType();
                ParkingSpotCategory spotType = user->getSpotType();
                
                int totalAmount = user->calculateParkingFee();
                totalAmount = user->applyLoyaltyDiscount(totalAmount);
                
                cout << "Final amount after discounts: Rs " << totalAmount << "\n";
                
                int duration = user->getBookedHours();
                user->processParkingPayment(totalAmount, vehicle, spotType, parkedSpotId, duration);
                
                if (parkingSystem.releaseParkingSpot(parkedSpotId)) {
                    user->setParkingStatus(false);
                    cout << "Spot " << parkedSpotId << " successfully released!\n";
                    parkingSystem.saveCurrentLayout();
                    DataStorage::saveSpotStates(parkingSystem);
                } else {
                    cout << "Error releasing spot " << parkedSpotId << "!\n";
                }
                break;
            }

            case 8:  // View Parking Map
                parkingSystem.displayParkingLayout();
                break;

            case 9:  // View Fine Rules
                LateFeeCalculator::displayFinePolicy();
                break;

            case 10: {  // User Settings
                if (userDatabase.empty()) {
                    cout << "No users registered yet!\n";
                    break;
                }
                
                ParkingSystemUser* user = authenticateParkingUser(userDatabase);
                if (!user) break;
                
                int settingsChoice = 0;
                bool accountDeleted = false;
                do {
                    user->showAccountDeletionMenu();
                    string settingsLine;
                    getline(cin, settingsLine);
                    
                    if (!parseIntInRange(settingsLine, settingsChoice, 1, 6)) {
                        cout << "Invalid choice! Please enter 1-6.\n";
                        continue;
                    }
                    
                    switch (settingsChoice) {
                        case 1:
                            user->updateVehicleType();
                            user->saveUserData();
                            break;
                        case 2:
                            user->changeUserPassword();
                            break;
                        case 3:
                            user->displayUserInfo();
                            break;
                        case 4:
                            if (user->deleteUserAccount(userDatabase)) {
                                accountDeleted = true;
                                settingsChoice = 6; // Exit the menu
                            }
                            break;
                        case 5:
                            user->resetPasswordWithSecurityQuestion();
                            break;
                        case 6:
                            cout << "Returning to main menu...\n";
                            break;
                    }
                    
                    // If account was deleted, break completely
                    if (accountDeleted) {
                        break;
                    }
                } while (settingsChoice != 6);
                break;
            }

            case 11: {  // Reset Password with Security Question
                if (userDatabase.empty()) {
                    cout << "No users registered yet!\n";
                    break;
                }
                
                ParkingSystemUser* user = resetPasswordWithSecurityQuestion(userDatabase);
                if (user) {
                    cout << "Password reset successful! You can now login with your new password.\n";
                } else {
                    cout << "Password reset failed!\n";
                }
                break;
            }

            case 12: {  // Exit
                cout << "Exiting Smart Parking System. Goodbye!\n";
                parkingSystem.saveCurrentLayout();
                DataStorage::saveSpotDatabase(parkingSystem);
                DataStorage::saveSpotStates(parkingSystem);
                
                // Clean up memory
                for (auto &pair : userDatabase) {
                    delete pair.second;
                }
                break;
            }
        }
    } while (choice != 12);
  } catch (const std::exception& e) {
        cout << "Error: " << e.what() << endl;
        cout << "The program will now exit." << endl;
        return 1;
  }

    return 0;
}