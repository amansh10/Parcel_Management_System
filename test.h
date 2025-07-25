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
    unordered_map<string,string> priority_map;

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

    priority_queue<pair<string,int> >order_priority;

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
    void Admin(priority_queue<pair<string,int> >&orderprint);
};

class Menu {
public:
    void menu();
};

#endif


/*
 Problem:

 1) Right now it directly prints from he admin, because in parcel, admin is being triggered
 2)It needs to store continously and print
 3) Only the admin should be able to pop the orders.

 */