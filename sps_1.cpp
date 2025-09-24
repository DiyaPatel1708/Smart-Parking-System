class user {
private:
    string name;
    string userID;
    long long contact;
    string email;
    vehicleType Type;
    parkingSpotType spotType;
    static int userCount;
    time_t entryTime;
    int rating;          // ⭐ New field
    string review;       // 📝 New field

public:
    user(string name, string userID, long long contact, 
         vehicleType Type = vehicleType::fiveSeaterCar, 
         string email = "not_provided@mail.com", 
         parkingSpotType spotType = parkingSpotType::standard) 
    {
        this->name=name; 
        this->userID=userID; 
        this->contact=contact; 
        this->email=email; 
        this->Type=Type; 
        this->spotType=spotType;
        this->rating = 0; 
        this->review = "No review yet.";
        userCount++; 
        entryTime=time(NULL);
    }  

    void showInfo() const { 
        cout<<"User: "<<name<<" | ID: "<<userID<<" | Contact: "<<contact
            <<" | Email: "<<email<<" | Vehicle Type: "<<(int)Type
            <<" | Rating: "<<rating<<" | Review: "<<review<<"\n";
    }

    static int getUserCount() { return userCount; }

    void setRating(int stars) {
        if(stars < 1 || stars > 5) {
            cout<<"Invalid rating! Must be 1–5 stars.\n";
            return;
        }
        rating = stars;
        cout<<"Thank you! You rated us "<<rating<<" stars.\n";
    }

    void setReview(const string &rev) {
        review = rev;
        cout<<"Review added!\n";
    }

    void saveToFile(const string &filename) const {
        ofstream fout(filename, ios::app);
        fout<<name<<" | "<<userID<<" | "<<contact<<" | "<<email
            <<" | VehicleType: "<<(int)Type
            <<" | Entry: "<<ctime(&entryTime)
            <<" | Rating: "<<rating
            <<" | Review: "<<review<<"\n";
        fout.close();
    }

    static void readFromFile(const string &filename) {
        ifstream fin(filename);
        if(!fin) {
            cout<<"No data file found!\n";
            return;
        }
        string line;
        cout<<"\n--- Stored User Data ---\n";
        while(getline(fin,line)) {
            cout<<line<<"\n";
        }
        fin.close();
    }

    friend void displayContact(const user& u);
};
int user::userCount=0;

void displayContact(const user& u){
    cout << "Contact of " << u.name << " is: " << u.contact << "\n";
}
