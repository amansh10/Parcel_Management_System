#include "test.h"
using namespace std;

int load_callback(void *data, int argc, char **argv, char **) {
    unordered_map<string, string> *order_details = (unordered_map<string, string>*)data;
    (*order_details)[argv[1]] = argv[5]; // Name, Tracking_ID
    return 0;
}

void load_order_details(unordered_map<string, string> &order_details) {
    sqlite3 *db;
    sqlite3_open("sender.db", &db);
    sqlite3_exec(db, "SELECT * FROM sender;", load_callback, &order_details, nullptr);
    sqlite3_close(db);
}


void User::Sender() {
    Parcel P;

    cout << "What is your name?" << endl;
    cin >> sender_name;

    cout << "What is your address?" << endl;
    cin >> sender_address;

    cout << "What is your phone number? " << endl;
    cin >> sender_phone_number;

    srand(time(0));
    tracking_id = to_string(rand());

    P.sender_name = sender_name;
    P.tracking_id=tracking_id;

    P.package_details();

    total_price = to_string(P.total_price); // Set total_price from Parcel

    sqlite3 *db;
    char *errMsg = nullptr;

    int rc = sqlite3_open("sender.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open sender.db: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    string sender_command = "CREATE TABLE IF NOT EXISTS sender("
                                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "Name TEXT NOT NULL, "
                                 "Address TEXT NOT NULL, "
                                 "Phone_Number TEXT NOT NULL, "
                                 "Total_Price TEXT NOT NULL, "
                                 "Tracking_ID TEXT NOT NULL);";

    rc = sqlite3_exec(db, sender_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (create table): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    string sender_insert_command = "INSERT INTO sender (Name, Address, Phone_Number, Total_Price, Tracking_ID) VALUES ('" + sender_name + "', '" + sender_address + "', '" + sender_phone_number + "', '" + total_price + "', '" + tracking_id + "');";

    rc = sqlite3_exec(db, sender_insert_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (insert): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);

    order_details[sender_name] = tracking_id;

// for admin

     rc = sqlite3_open("admin.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open sender.db: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

   sender_command = "CREATE TABLE IF NOT EXISTS admin_sender("
                                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "Name TEXT NOT NULL, "
                                 "Address TEXT NOT NULL, "
                                 "Phone_Number TEXT NOT NULL, "
                                 "Total_Price TEXT NOT NULL, "
                                 "Tracking_ID TEXT NOT NULL);";

    rc = sqlite3_exec(db, sender_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (create table): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

     sender_insert_command = "INSERT INTO admin_sender (Name, Address, Phone_Number, Total_Price, Tracking_ID) VALUES ('" + sender_name + "', '" + sender_address + "', '" + sender_phone_number + "', '" + total_price + "', '" + tracking_id + "');";

    rc = sqlite3_exec(db, sender_insert_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (insert): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);


}

void User::Receiver() {
    cout << "Tell me your name? " << endl;
    cin >> receiver_name;

    cout << "What is your phone number? " << endl;
    cin >> receiver_phone_number;

    cout << "What is your address? " << endl;
    cin >> receiver_address;

    cout << "What is the name of the person whose order you are going to receive? " << endl;
    cin >> sender_name;

    cout << "What is the tracking ID? " << endl;
    cin >> tracking_id;

    if (order_details[sender_name] == tracking_id) {
        cout << "Here is your order" << endl;
    } else {
        cout << "Sorry, there is no parcel for you!" << endl;
    }

    sqlite3 *db;
    char *errMsg = nullptr;

    int rc = sqlite3_open("receiver.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

   string receiver_command = "CREATE TABLE IF NOT EXISTS receiver("
                                   "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "Name TEXT NOT NULL, "
                                   "Address TEXT NOT NULL, "
                                   "Phone_Number TEXT NOT NULL, "
                                   "Sender_Name TEXT NOT NULL, "
                                   "Tracking_ID TEXT NOT NULL);";

    rc = sqlite3_exec(db, receiver_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (create table): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    string receiver_insert_command = "INSERT INTO receiver (Name, Address, Phone_Number, Sender_Name, Tracking_ID) VALUES ('" + receiver_name + "', '" + receiver_address + "', '" + receiver_phone_number + "', '" + sender_name + "', '" + tracking_id + "');";

    rc = sqlite3_exec(db, receiver_insert_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (insert): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);

     rc = sqlite3_open("admin.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

     receiver_command = "CREATE TABLE IF NOT EXISTS admin_reciever("
                              "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "Name TEXT NOT NULL, "
                              "Address TEXT NOT NULL, "
                              "Phone_Number TEXT NOT NULL, "
                              "Sender_Name TEXT NOT NULL, "
                              "Tracking_ID TEXT NOT NULL);";

    rc = sqlite3_exec(db, receiver_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (create table): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

     receiver_insert_command = "INSERT INTO admin_reciever (Name, Address, Phone_Number, Sender_Name, Tracking_ID) VALUES ('" + receiver_name + "', '" + receiver_address + "', '" + receiver_phone_number + "', '" + sender_name + "', '" + tracking_id + "');";

    rc = sqlite3_exec(db, receiver_insert_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (insert): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);
}

void User::Admin(priority_queue<pair<string,int> >&orderprint) {
    while(!orderprint.empty()){
     pair<string , int> order=orderprint.top();
     cout<<order.first<<" "<<order.second<<endl;
     orderprint.pop();
    };
};

void Parcel::package_details() {
    User U;
    string name=U.sender_name;


    weight_fin.open("weight.txt");
    location_fin.open("location.txt");
    priority_fin.open("priority_price.txt");


    while (weight_fin >> package_weight >> package_weight_price) {
        weight_map[package_weight] = package_weight_price;
    }

    while (location_fin >> location >> location_price) {
        location_map[location] = location_price;
    }

    while(priority_fin>>priority_order>>priority_price){
        priority_map[priority_order]=priority_price;
    }

    weight_fin.close();
    location_fin.close();

    cout << "What is the weight of the package? " << endl;
    cin >> ask_package_weight;

    cout << "What is the location of the place? " << endl;
    cin >> ask_location;

    cout<<"How urgent is this order? "<<endl;
    cin>>priority_number;

    total_price = (stoi(weight_map[ask_package_weight]) + stoi(location_map[ask_location])+stoi(priority_map[priority_order])) / 3;

    order_priority.push(make_pair(tracking_id,stoi(priority_number)));
    U.Admin(order_priority);

    fout.open(sender_name+"bill.txt");

    fout << "Total Price Is " << total_price << endl;
    fout.close();
}

void Menu::menu() {
    int options;
    char sub_options;

    User U;
    Parcel P;
    priority_queue<pair<string, int> > orderprint;
    load_order_details(U.order_details);


    while (options != 3) {
        cout << "1. Users" << endl;
        cout << "2. Package" << endl;
        cout << "3. Exit" << endl;

        cin >> options;

        if (options == 1) {
            cout << "Are you the sender/receiver/admin?" << endl;
            cin >> sub_options;

            if (sub_options == 's') {
                U.Sender();
            }
            else if (sub_options == 'r') {
                U.Receiver();
            }
            else if (sub_options == 'a') {
                U.Admin(orderprint);
            }
        }
        else if (options == 2) {
            if(P.sender_name.empty()){
                cout<<"Are you the sender or reciever? "<<endl;
                cin>>sub_options;

                if(sub_options=='s'){
                    U.Sender();
                }
                else if(sub_options=='r'){
                    U.Receiver();
                }
            }

        }
    }
}