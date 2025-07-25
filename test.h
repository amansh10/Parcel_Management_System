#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <sqlite3.h>

using namespace std;

class Parcel {
public:
    unordered_map<string, string> weight_map;
    unordered_map<string, string> location_map;

    ifstream weight_fin;
    ifstream location_fin;

    ofstream fout;

    string package_weight;
    string package_weight_price;

    string location;
    string location_price;

    string ask_package_weight;
    string ask_location;

    int total_price;

    void package_details();
};

class User {
public:
    ifstream fin;
    ofstream fout;

    unordered_map<string, string> order_details;

    string sender_name;
    string sender_address;
    string sender_phone_number;
    string total_price; // Added for Total_Price column

    string receiver_name; // Fixed typo
    string receiver_phone_number; // Fixed typo
    string receiver_address; // Fixed typo

    string tracking_id;

    void Sender();
    void Receiver(); // Fixed typo
    void Admin();
};

class Menu {
public:
    void menu();
};

#endif