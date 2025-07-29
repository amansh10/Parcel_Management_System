#include "test.h"

string Date(){

    time_t timestamp;
    time(&timestamp);

    return string(ctime(&timestamp));
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

    while (priority_fin >> priority_order >> priority_price) {
        priority_map[priority_order] = priority_price;
    }

    weight_fin.close();
    location_fin.close();
    priority_fin.close();

    cout << "What is the weight of the package? ";
    cin >> ask_package_weight;

    cout << "What is the location of the place? ";
    cin >> ask_location;

    cout << "How urgent is this order? (use priority key from file) ";
    cin >> priority_number;

    try {
        total_price = (stoi(weight_map.at(ask_package_weight)) +
                       stoi(location_map.at(ask_location)) +
                       stoi(priority_map.at(priority_number))) / 3;
    } catch (const exception& e) {
        cerr << "Bad input while computing total_price: " << e.what() << "\n";
        total_price = 0;
    }

    fout.open(sender_name + "_" + "bill.txt");
    fout << "Total Price Is " << total_price << endl;
    fout.close();
}


void User::Sender() {
    cout << "What is your name? ";
    cin >> sender_name;

    cout << "What is your address? ";
    cin >> sender_address;

    cout << "What is your phone number? ";
    cin >> sender_phone_number;

    srand(static_cast<unsigned>(time(nullptr)));
    tracking_id = to_string(rand());

    P.sender_name = sender_name;
    P.tracking_id = tracking_id;
    P.package_details();

    total_price = to_string(P.total_price);

    // Store in-memory priority vector
    try {
        order_vector.push_back({stoi(P.priority_number), tracking_id});
    } catch (...) {
        cerr << "Invalid priority_number, defaulting to 999\n";
        order_vector.push_back({999, tracking_id});
    }

    DatabaseManager dbm;
    if (!dbm.open()) return;
    dbm.createAllTables();

    SenderRecord srec {
            sender_name,
            sender_address,
            sender_phone_number,
            total_price,
            tracking_id,
            P.priority_number
    };

    LogRecord lrec {
       "Sender",
       "Order Placed",
       Date()
    };

    if (!dbm.insertSender(srec) ) {
        cerr << "Failed to insert sender.\n";
    }

    if(!dbm.insertLog(lrec)) {
        cerr << "Failed to insert log.\n";
    }

    order_details[sender_name] = tracking_id;

    dbm.sendEmail(srec,"cyco.dud1@gmail.com");
    dbm.close();
}

void User::Receiver() {
    cout << "Tell me your name? ";
    cin >> receiver_name;

    cout << "What is your phone number? ";
    cin >> receiver_phone_number;

    cout << "What is your address? ";
    cin >> receiver_address;

    cout << "What is the name of the person whose order you are going to receive? ";
    cin >> sender_name;

    cout << "What is the tracking ID? ";
    cin >> tracking_id;

    // Remove from in-memory vector
    bool found = false;
    for (size_t i = 0; i < order_vector.size(); ++i) {
        if (order_vector[i].second == tracking_id) {
            order_vector.erase(order_vector.begin() + i);
            cout << "Thanks for receiving the parcel!: " << tracking_id << endl;
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "Tracking ID not found in in-memory queue (it might still be in DB).\n";
    }

    DatabaseManager dbm;
    if (!dbm.open()) return;
    dbm.createAllTables();

    // Record receiver
    ReceiverRecord rec{
            receiver_name,
            receiver_address,
            receiver_phone_number,
            sender_name,
            tracking_id
    };

    LogRecord lrec{
        "Receiver",
        "Order Recieved",
        Date()
};

    if (!dbm.insertReceiver(rec)) {
        cerr << "Failed to insert receiver.\n";
    }

    if(!dbm.insertLog(lrec)) {
        cerr << "Failed to insert log.\n";
    }

    // Delete from sender table (delivered)
    if (!dbm.deleteSenderByTrackingId(tracking_id)) {
        cerr << "Failed to delete sender row for TID: " << tracking_id << endl;
    }

    dbm.close();
}

void User::Admin() {
    cout << "What is your pin? ";
    cin >> ask_pin;

    if (ask_pin != pin) {
        cout << "Wrong pin.\n";
        return;
    }

    cout << "What do you wanna check? \n";
    cout << "1. Priority Of Order\n";
    cout << "2. Filter any details \n";
    cout << "3. Remove any order from queue\n";
    cin >> admin_options;

    DatabaseManager dbm;
    if (!dbm.open()) return;
    dbm.createAllTables();

    switch (admin_options) {
        case 1: {
            // Reload from DB to be safe
            order_details.clear();
            order_vector.clear();
            dbm.loadOrderDetails(order_details, order_vector);

            // refill priority queue
            while (!order_priority.empty()) order_priority.pop();
            for (auto& p : order_vector) order_priority.push(p);

            cout << "---- Priority Queue (lower number = higher priority) ----\n";
            while (!order_priority.empty()) {
                auto top = order_priority.top();
                order_priority.pop();
                cout << "TrackingID: " << top.second << " | Priority: " << top.first << '\n';
            }

            LogRecord lrec{
                    "Admin",
                    "Order Checkd Priority",
                    Date()
            };

            if(!dbm.insertLog(lrec)) {
                cerr << "Failed to insert log.\n";
            }
            break;
        }
        case 2: {
            cout << "Do you wanna check from the sender or receiver? (s/r): ";
            cin >> admin_sub_options;

            string location;
            cout << "Which Address do you want to check? ";
            cin >> location;

            if (admin_sub_options == 's') {
                dbm.selectSenderByAddress(location);
                LogRecord lrec{
                    "Admin",
                    "Order Checked From Sender",
                    Date()
                };

                if(!dbm.insertLog(lrec)) {
                    cerr << "Failed to insert log.\n";
                }
            } else if (admin_sub_options == 'r') {
                dbm.selectReceiverByAddress(location);
                LogRecord lrec{
                        "Admin",
                        "Order Checked",
                        Date()
                };

                if(!dbm.insertLog(lrec)) {
                    cerr << "Failed to insert log.\n";
                }
            } else {
                cout << "Invalid option. Please enter 's' or 'r'.\n";
            }
            break;
        }
        case 3: {
            cout << "Which Tracking ID do you wanna remove? ";
            cin >> tracking_id;

            // remove from in-memory
            for (size_t i = 0; i < order_vector.size(); ++i) {
                if (order_vector[i].second == tracking_id) {
                    order_vector.erase(order_vector.begin() + i);
                    break;
                }
            }


            // remove from DB
            if (!dbm.deleteSenderByTrackingId(tracking_id)) {
                cerr << "Failed to delete from DB.\n";
            }
            LogRecord lrec{
                "Admin",
                "Order Removed By Admin",
                Date()
            };
            break;
        }
        default:
            cout << "Invalid option.\n";
            break;
    }

    dbm.close();
}


void Menu::menu() {
    int options = 0;
    string sub_options;

    User U;
    Parcel P;

    DatabaseManager dbm;
    if (!dbm.open()) return;
    dbm.createAllTables();
    dbm.loadOrderDetails(U.order_details, U.order_vector);
    dbm.close();

    while (options != 3) {
        cout << "\n========== MENU ==========\n";
        cout << "1. Users\n";
        cout << "2. Package\n";
        cout << "3. Exit\n";
        cout << "Choose: ";
        cin >> options;

        if (options == 1) {
            cout << "Are you the sender/receiver/admin? ";
            cin >> sub_options;

            if (sub_options == "s" || sub_options == "sender") {
                U.Sender();
            } else if (sub_options == "r" || sub_options == "receiver") {
                U.Receiver();
            } else if (sub_options == "a" || sub_options == "admin") {
                U.Admin();
            }
        } else if (options == 2) {
            if (P.sender_name.empty()) {
                cout << "Are you the sender or receiver? ";
                cin >> sub_options;

                if (sub_options == "s" || sub_options == "sender") {
                    U.Sender();
                } else if (sub_options == "r" || sub_options == "receiver") {
                    U.Receiver();
                }
            }
        }
    }
}