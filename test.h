#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <sqlite3.h>
#include<vector>
#include<string>

using namespace std;

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

    string priority_number;
    string priority_order;
    string priority_price;

    int total_price;

    void package_details();
};

class User {
public:
    ifstream fin;
    ofstream fout;

    Parcel P;

    unordered_map<string, string> order_details;

    priority_queue<pair<int, string> > order_priority;
    vector<pair<int, string> >order_vector;

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

    // Admin Credentials;

    void Admin();

    int pin=1234; // put in the private class
    int ask_pin;
    int admin_options;
    char admin_sub_options;

};

class Menu {
public:
    void menu();
};

#endif


// Fill the vector using the database.

