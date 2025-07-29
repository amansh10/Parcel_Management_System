#include "test.h"

// Need to understand

static int map_callback(void *data, int argc, char **argv, char **) {
    auto *order_details = static_cast<unordered_map<string, string>*>(data);
    if (order_details && argc > 5 && argv[1] && argv[5]) {
        // argv[1] -> Name, argv[5] -> Tracking_ID (based on your schema order)
        (*order_details)[argv[1]] = argv[5];
    }
    return 0;
}

// Need to understand

static int vector_callback(void *data, int argc, char **argv, char **) {
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


bool DatabaseManager::open(const string& path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        cerr << "Cannot open " << path << ": " << sqlite3_errmsg(db) << endl;
        close();
        return false;
    }
    return true;
}

void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool DatabaseManager::createAllTables() {
    return createTable("admin_sender") && createTable("admin_receiver") && createTable("log_table");
}

bool DatabaseManager::createTable(const string &table_name) {
    if (!db) return false;

    const char* sql_sender =
            "CREATE TABLE IF NOT EXISTS admin_sender ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "Name TEXT NOT NULL, "
            "Address TEXT NOT NULL, "
            "Phone_Number TEXT NOT NULL, "
            "Total_Price TEXT NOT NULL, "
            "Tracking_ID TEXT NOT NULL, "
            "Priority TEXT NOT NULL);";

    const char* sql_receiver =
            "CREATE TABLE IF NOT EXISTS admin_receiver ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "Name TEXT NOT NULL, "
            "Address TEXT NOT NULL, "
            "Phone_Number TEXT NOT NULL, "
            "Sender_Name TEXT NOT NULL, "
            "Tracking_ID TEXT NOT NULL);";

    const char *sql_log=
                        "CREATE TABLE IF NOT EXISTS log_table ("
                        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "User TEXT NOT NULL,"
                        "Action TEXT NOT NULL, "
                        "Date TEXT NOT NULL);";


    const char* sql = nullptr;

    sql=sql_log;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (create table): " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }


    if (table_name == "admin_sender") {
        sql = sql_sender;
    } else if (table_name == "admin_receiver") {
        sql = sql_receiver;
    } else {
        return false;
    }

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL error (create table): " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool DatabaseManager::insertSender(const SenderRecord& rec) {
    if (!db) return false;

    const char* sql =
            "INSERT INTO admin_sender "
            "(Name, Address, Phone_Number, Total_Price, Tracking_ID, Priority) "
            "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, rec.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, rec.address.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, rec.phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, rec.total_price.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, rec.tracking_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, rec.priority.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Insert sender failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::insertLog(const LogRecord& lrec) {
    if(!db) return false;

    const char* sql =
            "INSERT INTO log_table "
            "(User, Action, Date) "
            "VALUES (?, ?, ?);";

        sqlite3_stmt* stmt = nullptr;
        if(sqlite3_prepare(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
            return false;
        }

    sqlite3_bind_text(stmt,1,lrec.user_type.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,2,lrec.action.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,3,lrec.time.c_str(),-1,SQLITE_TRANSIENT);

        int rc=sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Insert sender failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::insertReceiver(const ReceiverRecord& rec) {
    if (!db) return false;

    const char* sql =
            "INSERT INTO admin_receiver "
            "(Name, Address, Phone_Number, Sender_Name, Tracking_ID) "
            "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, rec.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, rec.address.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, rec.phone.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, rec.sender_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, rec.tracking_id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Insert receiver failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::deleteSenderByTrackingId(const string& tid) {
    if (!db) return false;

    const char* sql = "DELETE FROM admin_sender WHERE Tracking_ID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, tid.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Delete failed: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

string DatabaseManager::readFile(const string &filename) {
    ifstream fin;
    fin.open("package.json");
    ostringstream ss;
    ss<< fin.rdbuf();
    return ss.str();
};

void DatabaseManager::replaceplaceholders(string& str, const map<string, string>& replacements){
    for (const auto& pair : replacements) {
        size_t pos;
        while ((pos = str.find(pair.first)) != std::string::npos) {
            str.replace(pos, pair.first.length(), pair.second);
        }
    }
}

void DatabaseManager::sendEmail(const SenderRecord& srec, const std::string& recipient_email) {

    string payload = readFile("package.json");

    map<string, string> replacements = {
            {"[name]", srec.name},
            {"[email]", recipient_email},
            {"[tracking_id]", srec.tracking_id},
            {"[total_price]", srec.total_price},
            {"[priority]", srec.priority}
    };

    replaceplaceholders(payload, replacements);

    std::string cmd = "curl --request POST "
                      "--url https://api.sendgrid.com/v3/mail/send "
                      "--header \"Authorization: Bearer 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\" "
                      "--header \"Content-Type: application/json\" "
                      "--data '" + payload + "'";

    std::cout << "Sending email with command:\n" << cmd << std::endl;

    system(cmd.c_str());

   /* std::string content = "Hello " + srec.name +
                          ",\\n\\nYour parcel with tracking ID " + srec.tracking_id +
                          " has been processed.\\nTotal Price: $" + srec.total_price +
                          "\\nPriority: " + srec.priority +
                          "\\n\\nThank you for using our service!";

    // Construct JSON payload with proper escaping
    std::string jsonPayload = "{"
                              "\\\"personalizations\\\": [{\\\"to\\\": [{\\\"email\\\": \\\"" + recipient_email + "\\\"}]}],"
                                                                                                                  "\\\"from\\\": {\\\"email\\\": \\\"amanshrestha656@gmail.com\\\"},"
                                                                                                                  "\\\"subject\\\": \\\"Parcel Notification\\\","
                                                                                                                  "\\\"content\\\": [{\\\"type\\\": \\\"text/plain\\\", \\\"value\\\": \\\"" + content + "\\\"}]"
                                                                                                                                                                                                         "}";

    std::string cmd = "curl --request POST "
                      "--url https://api.sendgrid.com/v3/mail/send "
                      "--header \"Authorization: Bearer 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\" "
                      "--header \"Content-Type: application/json\" "
                      "--data \"" + jsonPayload + "\"";

    // Debug print to check final curl command
    std::cout << "Sending email with command:\n" << cmd << std::endl;

    system(cmd.c_str());
    */

}

bool DatabaseManager::selectSenderByAddress(const string& addr) {
    if (!db) return false;

    const char* sql = "SELECT * FROM admin_sender WHERE Address = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, addr.c_str(), -1, SQLITE_TRANSIENT);

    // Need to understand this portion

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        for (int i = 0; i < cols; ++i) {
            const char* name = sqlite3_column_name(stmt, i);
            const unsigned char* val = sqlite3_column_text(stmt, i);
            cout << name << ": " << (val ? reinterpret_cast<const char*>(val) : "NULL") << "  ";
        }
        cout << "\n";
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::selectReceiverByAddress(const string& addr) {
    if (!db) return false;

    const char* sql = "SELECT * FROM admin_receiver WHERE Address = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, addr.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        for (int i = 0; i < cols; ++i) {
            const char* name = sqlite3_column_name(stmt, i);
            const unsigned char* val = sqlite3_column_text(stmt, i);
            cout << name << ": " << (val ? reinterpret_cast<const char*>(val) : "NULL") << "  ";
        }
        cout << "\n";
    }

    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseManager::loadOrderDetails(unordered_map<string, string> &order_details,
                                       vector<pair<int, string> >& order_vector) {
    if (!db) return false;

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, "SELECT * FROM admin_sender;", map_callback, &order_details, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (order_details): " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    rc = sqlite3_exec(db, "SELECT * FROM admin_sender;", vector_callback, &order_vector, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (order_vector): " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}