#include "test.h"
using namespace std;

int map_callback(void *data, int argc, char **argv, char **) {
    auto *order_details = static_cast<unordered_map<string, string>*>(data);
    if (order_details && argc > 5 && argv[1] && argv[5]) {
        (*order_details)[argv[1]] = argv[5];
    }
    return 0;
}

int vector_callback(void *data, int argc, char **argv, char **) {
    auto *order_vector = static_cast<vector<pair<int, string> >*>(data);
    if (order_vector && argc > 6 && argv[5] && argv[6]) {
        try {
            order_vector->push_back({stoi(argv[6]), argv[5]});
        } catch (const exception& e) {
            cerr << "Error converting Priority: " << e.what() << endl;
        }
    }
    return 0;
}



void load_order_details(unordered_map<string, string> &order_details, vector<pair<int, string> > &order_vector) {
    sqlite3 *db;
    char *errMsg = nullptr;

    int rc = sqlite3_open("admin.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Populate order_details
    rc = sqlite3_exec(db, "SELECT * FROM admin_sender;", map_callback, &order_details, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (order_details): " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    // Populate order_vector
    rc = sqlite3_exec(db, "SELECT * FROM admin_sender;", vector_callback, &order_vector, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (order_vector): " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

void fillpriority(priority_queue<pair<int, string> > &orderprint,vector<pair<int,string> >&order_vector) {
    for(auto C:order_vector){
        orderprint.push(make_pair(C.first,C.second));
    }

    while (!orderprint.empty()) {;
        pair<int, string> order = orderprint.top();
        cout << order.second << " " << order.first << endl;
        orderprint.pop();
    }
};

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
    P.tracking_id = tracking_id;
    P.package_details();

    total_price = to_string(P.total_price);

    order_vector.push_back(make_pair(stoi(P.priority_number),tracking_id ));


    sqlite3 *db;
    char *errMsg = nullptr;

    int rc = sqlite3_open("admin.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open sender.db: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    string sender_command = "CREATE TABLE IF NOT EXISTS admin_sender("
                            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "Name TEXT NOT NULL, "
                            "Address TEXT NOT NULL, "
                            "Phone_Number TEXT NOT NULL, "
                            "Total_Price TEXT NOT NULL, "
                            "Tracking_ID TEXT NOT NULL, "
                            "Priority TEXT NOT NULL);";

    rc = sqlite3_exec(db, sender_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (create table): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    string sender_insert_command = "INSERT INTO admin_sender (Name, Address, Phone_Number, Total_Price, Tracking_ID, Priority) VALUES ('" + sender_name + "', '" + sender_address + "', '" + sender_phone_number + "', '" + total_price + "', '" + tracking_id + "','" + P.priority_number + "');";

    rc = sqlite3_exec(db, sender_insert_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (insert): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);

    order_details[sender_name] = tracking_id;


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

        for(int i=0;i<order_vector.size();i++){
            if(order_vector[i].second!=tracking_id) {
                continue;
            }
            else{
                order_vector.erase(order_vector.begin() + i);
                cout<<"Thanks for recieving the parcel!: "+tracking_id<<endl;
            }

        };

    sqlite3 *db;
    char *errMsg = nullptr;

    int rc = sqlite3_open("admin.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

   string receiver_command = "CREATE TABLE IF NOT EXISTS admin_reciever("
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

    string receiver_insert_command = "INSERT INTO admin_reciever (Name, Address, Phone_Number, Sender_Name, Tracking_ID) VALUES ('" + receiver_name + "', '" + receiver_address + "', '" + receiver_phone_number + "', '" + sender_name + "', '" + tracking_id + "');";

    rc = sqlite3_exec(db, receiver_insert_command.c_str(), nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (insert): " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return;
    }

    sqlite3_close(db);

}

void User::Admin() {
    cout<<"What is your pin? "<<endl;
    cin>>ask_pin;

    if(ask_pin==pin){
        cout<<"What do you wanna check? "<<endl;
        cout<<"1. Priority Of Order"<<endl;
        cout<<"2. Filter any details "<<endl;
        cout<<"3. Remove any order from queue"<<endl;

        cin>>admin_options;

        if(admin_options==1){
            fillpriority(order_priority,order_vector);
        }
        else if (admin_options == 2) {

            cout << "Do you wanna check from the sender or receiver? (s/r): ";
            cin >> admin_sub_options;

            if (admin_sub_options == 's') {
                string location;
                sqlite3 *db;
                char *errMsg = nullptr;

                int rc = sqlite3_open("admin.db", &db);
                if (rc != SQLITE_OK) {
                    cout << "Cannot open database: " << sqlite3_errmsg(db) << endl;
                    sqlite3_close(db);
                    return;
                }

                cout << "Which ID, you want to check what? ";
                cin >> location;


                string sql = "SELECT * FROM admin_sender WHERE Address='" + location + "'";

                auto callback = [](void* unused, int argc, char** argv, char** colNames) -> int {
                    for (int i = 0; i < argc; ++i) {
                        cout << colNames[i] << ": " << (argv[i] ? argv[i] : "NULL") << "  ";
                    }
                    cout << endl;
                    return 0;
                };

                rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);
                if (rc != SQLITE_OK) {
                    cout << "SQL error: " << errMsg << endl;
                    sqlite3_free(errMsg);
                }
                sqlite3_close(db);
            }
            else if (admin_sub_options == 'r') {
                string location;
                sqlite3 *db;
                char *errMsg = nullptr;

                int rc = sqlite3_open("admin.db", &db);
                if (rc != SQLITE_OK) {
                    cout << "Cannot open database: " << sqlite3_errmsg(db) << endl;
                    sqlite3_close(db);
                    return;
                }

                cout << "Which ID, you want to check what? ";
                cin >> location;


                string sql = "SELECT * FROM admin_reciever WHERE Address='" + location + "'";

                auto callback = [](void* unused, int argc, char** argv, char** colNames) -> int {
                    for (int i = 0; i < argc; ++i) {
                        cout << colNames[i] << ": " << (argv[i] ? argv[i] : "NULL") << "  ";
                    }
                    cout << endl;
                    return 0;
                };

                rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);
                if (rc != SQLITE_OK) {
                    cout << "SQL error: " << errMsg << endl;
                    sqlite3_free(errMsg);
                }
                sqlite3_close(db);
            }
            else {
                cout << "Invalid option. Please enter 's' or 'r'." << endl;
            }
        }
        else if(admin_options==3){

            cout<<"Which Tracking ID do you wanna remove? "<<endl;
            cin>>tracking_id;

            for(int i=0;i<order_vector.size();i++){
                if(order_vector[i].second==tracking_id) {
                    order_vector.erase(order_vector.begin() + i);
                };
            };
        };

    };

};


void Parcel::package_details() {
    weight_fin.open("weight.txt");
    location_fin.open("location.txt");
    priority_fin.open("priority_price.txt");

    while (weight_fin >> package_weight >> package_weight_price) {
        weight_map[package_weight] = package_weight_price;
    }

    while (location_fin >> location >> location_price) {
        location_map[location] = location_price;
    }

    while(priority_fin >> priority_order >> priority_price){
        priority_map[priority_order] = priority_price;
    }

    weight_fin.close();
    location_fin.close();
    priority_fin.close();

    cout << "What is the weight of the package? " << endl;
    cin >> ask_package_weight;

    cout << "What is the location of the place? " << endl;
    cin >> ask_location;

    cout << "How urgent is this order? " << endl;
    cin >> priority_number;

    total_price = (stoi(weight_map[ask_package_weight]) +
                   stoi(location_map[ask_location]) +
                   stoi(priority_map[priority_order])) / 3;

    fout.open(sender_name +"_"+ "bill.txt");
    fout << "Total Price Is " << total_price << endl;
    fout.close();
}

void Menu::menu() {
    int options;
    string sub_options;

    User U;
    Parcel P;

    load_order_details(U.order_details,U.order_vector);


    while (options != 3) {
        cout << "1. Users" << endl;
        cout << "2. Package" << endl;
        cout << "3. Exit" << endl;

        cin >> options;

        if (options == 1) {
            cout << "Are you the sender/receiver/admin?" << endl;
            cin >> sub_options;

            if (sub_options == "s" ||sub_options=="sender") {
                U.Sender();
            }
            else if (sub_options =="r" || sub_options=="receiver") {
                U.Receiver();
            }
            else if (sub_options == "a" || sub_options=="admin"){
               U.Admin();
            }
        }
        else if (options == 2) {
            if(P.sender_name.empty()){
                cout<<"Are you the sender or reciever? "<<endl;
                cin>>sub_options;

                if(sub_options == "s" ||sub_options=="sender"){
                    U.Sender();
                }
                else if(sub_options =="r" || sub_options=="receiver"){
                    U.Receiver();
                }
            }

        }
    }
}