#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <sqlite3.h>
#include <vector>
#include <string>
#include <limits>  // for numeric_limits
#include<sstream>
#include<map>

using namespace std;

// ---------------- Records ----------------
struct SenderRecord {
    string name;
    string address;
    string phone;
    string total_price;
    string tracking_id;
    string priority;
};

struct ReceiverRecord {
    string name;
    string address;
    string phone;
    string sender_name;
    string tracking_id;
};

struct LogRecord{
    string user_type;
    string action;
    string time;
};

// --------------- DB Manager ---------------
class DatabaseManager {
public:
    DatabaseManager() : db(nullptr) {}
    ~DatabaseManager() { close(); }

    bool open(const string& path = "admin.db");
    void close();

    bool createTable(const string& table_name);        // single-table create
    bool createAllTables();                            // convenience

    bool insertSender(const SenderRecord& srec);
    bool insertReceiver(const ReceiverRecord& rrec);
    bool insertLog(const LogRecord& lrec);

    bool deleteSenderByTrackingId(const string& tid);
    bool selectSenderByAddress(const string& addr);
    bool selectReceiverByAddress(const string& addr);
    string readFile(const string &filename);
    void replaceplaceholders(string& str, const map<string, string>& replacements);
    void sendEmail(const SenderRecord& srec, const std::string& recipient_email);





    bool loadOrderDetails(unordered_map<string, string> &order_details,
                          vector<pair<int, string> >& order_vector);

private:
    sqlite3 *db;
};

// ---------------- Parcel ------------------
class Parcel {
public:
    unordered_map<string, string> weight_map;
    unordered_map<string, string> location_map;
    unordered_map<string, string> priority_map;

    string sender_name;
    string tracking_id;

    ifstream weight_fin;
    ifstream location_fin;
    ifstream priority_fin;

    ofstream fout;

    string package_weight;
    string package_weight_price;

    string location;
    string location_price;

    string ask_package_weight;
    string ask_location;

    string priority_number;  // numeric in string form
    string priority_order;
    string priority_price;

    int total_price;

    void package_details();
};

// Priority comparator (lower number = higher priority)
struct PriorityGreater {
    bool operator()(const pair<int,string>& a, const pair<int,string>& b) const {
        return a.first < b.first;
    }
};

// ---------------- User --------------------
class User {
private:
    int pin = 1234;

public:
    ifstream fin;
    ofstream fout;

    Parcel P;

    unordered_map<string, string> order_details;

    priority_queue<pair<int, string>, vector<pair<int,string> >, PriorityGreater> order_priority;
    vector<pair<int, string> > order_vector;

    string sender_name;
    string sender_address;
    string sender_phone_number;
    string total_price;

    string receiver_name;
    string receiver_phone_number;
    string receiver_address;

    string tracking_id;

    void Sender();
    void Receiver();
    void Admin();


    int ask_pin = 0;
    int admin_options = 0;
    char admin_sub_options = 0;
};

// --------------- Menu ---------------------
class Menu {
public:
    void menu();
};

#endif