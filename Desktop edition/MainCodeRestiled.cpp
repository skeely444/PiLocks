#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>
#include "bcrypt.h"
using namespace std;



void printPrompt() {
    cout << "\033[36m> \033[0m";
}
void clearScreen() {
    cout << "\033[2J\033[H";
}
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string BLUE = "\033[34m";
const string BOLD = "\033[1m";
const string DIM = "\033[2m";
const string UNDERLINE = "\033[4m";

string getTimestamp()
{
    auto now = chrono::system_clock::now();
    std::time_t now_c = chrono::system_clock::to_time_t(now);

    tm localTime{};
    localtime_s(&localTime, &now_c); // Windows-safe

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &localTime);

    return buffer;
}

class UserProfile {
public:
    string loggedInUser = "";  // Add this member variable
    
    void setLoggedInUser(string user) {
        loggedInUser = user;
    }
    
    void addUser(string name, string role, string password, int id) {
        Usernames.push_back(name);
        Userroles.push_back(role);
        UserIds.push_back(id);
        Userpasswords.push_back(password);
    }
    string hashUsername(string username) {
        vector <int> individualHash;
        for (int i = 0; i < username.length(); i++) {
            int AsciiChar = username[i];
            int hashResult = (AsciiChar * AsciiChar) - (AsciiChar + AsciiChar) * (AsciiChar % 2) + AsciiChar;
            individualHash.push_back(hashResult);
        }
        string result = "";
        for (int i = 0; i < individualHash.size(); i++) {
            result += to_string(individualHash[i]);
        }
        return result;
    }
    string hashPassword(string password) {
        string hashedPassword = bcrypt::generateHash(password);
        cout << hashedPassword << endl;
        return hashedPassword;
    }
    void returnAll() {
        for (int i = 0; i < Usernames.size(); i++)
            cout << Usernames[i] << endl << UserIds[i] << endl << Userroles[i] << endl;
    }
    bool checkIfUsers() {
        bool areUsers;
        if (Usernames.size() != 0) {
            areUsers = true;
        }
        else {
            areUsers = false;
        }
        return areUsers;
    }
    bool correctLoginAdmin = false;
    bool correctUserLogin = false;
    bool checkAdminAuthentication(string attemptedUser, string attemptedPassword) {
        for (int i = 0; i < Usernames.size(); i++) {
            if (attemptedUser == Usernames[i] && bcrypt::validatePassword(attemptedPassword, Userpasswords[i]) && Userroles[i] == "Admin") {
                correctLoginAdmin = true;
            }
        }
        return correctLoginAdmin;
    }
    bool checkUserAuthentication(string attemptedUser, string attemptedPassword) {
        for (int i = 0; i < Usernames.size(); i++) {
            if (attemptedUser == Usernames[i] && bcrypt::validatePassword(attemptedPassword, Userpasswords[i]) && Userroles[i] == "User") {
                correctUserLogin = true;
            }
        }
        return correctUserLogin;
    }
    bool checkIfAdminLoggedIn() {
        bool AdminAlreadySignedIn = false;
        if (correctLoginAdmin) {
            AdminAlreadySignedIn = true;
        }
        return AdminAlreadySignedIn;
    }
    bool checkIfUserLoggedIn() {
        bool UserAlreadySignedIn;
        UserAlreadySignedIn = false;
        if (correctLoginAdmin) {
            UserAlreadySignedIn = true;
        }
        return UserAlreadySignedIn;
    }
    void removeUser(string removing) {
        for (int i = 0; i < Usernames.size(); i++) {
            if (removing == Usernames[i]) {
                Usernames.erase(Usernames.begin() + i);
                Userpasswords.erase(Userpasswords.begin() + i);
                Userroles.erase(Userroles.begin() + i);
                UserIds.erase(UserIds.begin() + i);
                cout << "Removed user: " << removing << endl;  // Use the parameter, not the array
                break;  // Exit after removing the first match
            }
        }
    }
    void saveToLog(string action) {  // Remove user parameter
        ActionsPerformed.push_back(action);
        UserPerformed.push_back(loggedInUser);  // Use member variable
        ActionTimestamps.push_back(getTimestamp());

        ofstream file("userLogs.json");

        file << "{\n";
        file << " \"Actions\": [\n";

        for (int i = 0; i < ActionsPerformed.size(); i++) {
            file << "    {\n";
            file << "      \"user\": \"" << UserPerformed[i] << "\",\n";
            file << "      \"action\": \"" << ActionsPerformed[i] << "\",\n";
            file << "      \"time\": \"" << ActionTimestamps[i] << "\"\n";

            if (i == Usernames.size() - 1)
                file << "    }\n";
            else
                file << "    },\n";
        }
        file << "  ]\n";
        file << "}\n";

        file.close();
    }
    void getActivityLogs() {
        ifstream file("userLogs.json");
        if (!file.is_open()) {
            return;
        }

        UserPerformed.clear();
        ActionsPerformed.clear();
        ActionTimestamps.clear();

        string word;
        while (file >> word) {
            if (word == "\"user\":") {
                string name;
                file >> name;
                name.erase(0, 1);
                name.erase(name.size() - 2, 2);
                UserPerformed.push_back(name);
            }
            else if (word == "\"action\":") {
                string action;
                file >> action;
                action.erase(0, 1);
                action.erase(action.size() - 2, 2);
                ActionsPerformed.push_back(action);
            }
            else if (word == "\"time\":") {
                string time;
                file >> time;
                time.erase(0, 1);
                time.erase(time.size() - 2, 2);
                ActionTimestamps.push_back(time);
            }
        }

        for (int i = 0; i < ActionsPerformed.size(); i++) {
            string timestamp = ActionTimestamps[i];  // Use stored timestamp
            if (ActionsPerformed[i] == "Add") {
                cout << timestamp << " " << UserPerformed[i] << GREEN << " added" << RESET << " a new user" << endl;
            }
            else if (ActionsPerformed[i] == "View") {
                cout << timestamp << " " << UserPerformed[i] << BLUE << " viewed" << RESET << " all the Users" << endl;
            }
            else if (ActionsPerformed[i] == "Remove") {
                cout << timestamp << " " << UserPerformed[i] << RED << " removed" << RESET << " a user" << endl;
            }
        }
    }
    void saveData() {
        ofstream file("userdata.json");

        file << "{\n";
        file << "  \"users\": [\n";

        for (int i = 0; i < Usernames.size(); i++) {
            file << "    {\n";
            file << "      \"name\": \"" << Usernames[i] << "\",\n";
            file << "      \"role\": \"" << Userroles[i] << "\",\n";
            file << "      \"id\": " << UserIds[i] << ",\n";
            file << "      \"password\": \"" << Userpasswords[i] << "\"\n";

            if (i == Usernames.size() - 1)
                file << "    }\n";
            else
                file << "    },\n";
        }

        file << "  ]\n";
        file << "}\n";

        file.close();
    }

    // Load all users from JSON back into vectors
    void loadData() {
        ifstream file("userdata.json");
        if (!file.is_open()) {
            // No existing file? Nothing to load (first time running).
            return;
        }

        Usernames.clear();
        Userroles.clear();
        UserIds.clear();
        Userpasswords.clear();

        string word;
        while (file >> word) {
            if (word == "\"name\":") {
                string name;
                file >> name;
                name.erase(0, 1);
                name.erase(name.size() - 2, 2);
                Usernames.push_back(name);
            }
            else if (word == "\"role\":") {
                string role;
                file >> role;
                role.erase(0, 1);
                role.erase(role.size() - 2, 2);
                Userroles.push_back(role);
            }
            else if (word == "\"id\":") {
                string id;
                file >> id;
                if (id.back() == ',') id.pop_back();
                if (id.back() == '}') id.pop_back();
                UserIds.push_back(stoi(id));
            }
            else if (word == "\"password\":") {
                string password;
                file >> password;
                // remove first "
                password.erase(0, 1);
                // if last char is ","
                if (password.back() == ',') {
                    password.pop_back();   // remove comma
                    password.pop_back();   // remove closing "
                }
                // if last char is just "
                else if (password.back() == '"') {
                    password.pop_back();
                }

                Userpasswords.push_back(password);
            }
        }

        file.close();
    }
private:
    vector <string> Usernames;
    vector <string> Userroles;
    vector <int> UserIds;
    vector <string> Userpasswords;
    vector <string> ActionsPerformed;
    vector <string> UserPerformed;
    vector <string> ActionTimestamps;
};

int main() {
    cout << R"(
                                             ___       _                    _           
                                            (  _`\  _ ( )                  ( )          
                                            | |_) )(_)| |       _      ___ | |/')   ___ 
                                            | ,__/'| || |  _  /'_`\  /'___)| , <  /',__)
                                            | |    | || |_( )( (_) )( (___ | |\`\ \__, \
                                            (_)    (_)(____/'`\___/'`\____)(_) (_)(____/                                                                                                                                                                                                                                                                        
    )" << endl;
    UserProfile obj;
    bool use = true;
    bool checkAdminLoggedInState = false;
    bool checkUserLoggedInState = false;
    bool correctLoginAdmin = false;
    bool correctUserLogin = false;
    bool definedUser;
    while (use) {
        obj.loadData();
        checkAdminLoggedInState = obj.checkIfAdminLoggedIn();
        checkUserLoggedInState = obj.checkIfUserLoggedIn();
        definedUser = obj.checkIfUsers();
        string userLoggingIn;
        string passwordLoggingIn;
        if (!definedUser) {
            string firstUserName;
            int firstUserId = 1;
            string firstUserRole = "Admin";
            string firstUserPassword;
            cout << "Welcome to PiLocks. To start, please create a PiLocks account" << endl;
            cout << "How would you like to be called?" << endl;
            printPrompt();
            cin >> firstUserName;
            cout << "Which password would you like to use?" << endl;
            printPrompt();
            cin >> firstUserPassword;
            string hashedFirstUserName = obj.hashUsername(firstUserName);
            string hashedFirstPassword = obj.hashPassword(firstUserPassword);
            obj.addUser(hashedFirstUserName, firstUserRole, hashedFirstPassword, firstUserId);
            obj.saveData();
            clearScreen();
        }
        else if (correctLoginAdmin || checkAdminLoggedInState) {
            clearScreen();
            cout << R"(
                     ------------------------------------------------------------------------------------
                         _       _           _         ____            _     _                         _ 
                        / \   __| |_ __ ___ (_)_ __   |  _ \  __ _ ___| |__ | |__   ___   __ _ _ __ __| |
                       / _ \ / _` | '_ ` _ \| | '_ \  | | | |/ _` / __| '_ \| '_ \ / _ \ / _` | '__/ _` |
                      / ___ \ (_| | | | | | | | | | | | |_| | (_| \__ \ | | | |_) | (_) | (_| | | | (_| |
                     /_/   \_\__,_|_| |_| |_|_|_| |_| |____/ \__,_|___/_| |_|_.__/ \___/ \__,_|_|  \__,_|
                     ------------------------------------------------------------------------------------
            )" << endl;
            cout << "Logs: " << endl;
            obj.getActivityLogs();
            cout << endl << "Available actions: " << endl;
            cout << " 1) " << BOLD << "Add " << RESET << "User" << endl
                << " 2) " << BOLD << "Remove " << RESET << "User" << endl
                << " 3) " << BOLD << "View " << RESET << "Users" << endl
                << " 4) " << BOLD << "Exit " << RESET << endl
                << " 5) " << BOLD << "Logout " << RESET << endl;
            string choice;
            printPrompt();
            cin >> choice;
            if (choice == "Add") {
                string userNameAdding;
                string userRoleAdding;
                string userPasswordAdding;
                int userIdAdding;
                clearScreen();
                cout << "Please enter the details of the new user below" << endl
                    << " Username: " << endl;
                printPrompt();
                cin >> userNameAdding;
                cout << " Role: " << endl;
                printPrompt();
                cin >> userRoleAdding;
                cout << " Password: " << endl;
                printPrompt();
                cin >> userPasswordAdding;
                cout << " Id: " << endl;
                printPrompt();
                cin >> userIdAdding;
                string hashedUser = obj.hashUsername(userNameAdding);
                string hashedPassword = obj.hashPassword(userPasswordAdding);
                obj.addUser(hashedUser, userRoleAdding, hashedPassword, userIdAdding);
                obj.saveData();
                obj.saveToLog(choice);
            }
            else if (choice == "View") {
                clearScreen();
                obj.returnAll();
                obj.saveToLog(choice);
                this_thread::sleep_for(chrono::seconds(10));
            }
            else if (choice == "Remove") {
                clearScreen();
                cout << "Which user would you like to remove?" << endl;
                string userToRemove;
                printPrompt();
                cin >> userToRemove;
                string hashedUserToRemove;
                obj.removeUser(hashedUserToRemove);
                obj.saveData();
                obj.saveToLog(choice);
            }
            else if (choice == "Logout") {
                checkAdminLoggedInState = false;
                correctLoginAdmin = false;
            }
            else if (choice == "Exit") {
                use = false;
            }
            else {
                cout << RED << "No valid choice detected" << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
            }
        }
        else if (correctUserLogin || checkUserLoggedInState) {
            cout << R"(
                     --------------------------------------------------------------------- 
                      _    _                 _____            _                         _ 
                     | |  | |               |  __ \          | |                       | |
                     | |  | |___  ___ _ __  | |  | | __ _ ___| |__   ___   __ _ _ __ __| |
                     | |  | / __|/ _ \ '__| | |  | |/ _` / __| '_ \ / _ \ / _` | '__/ _` |
                     | |__| \__ \  __/ |    | |__| | (_| \__ \ |_) | (_) | (_| | | | (_| |
                      \____/|___/\___|_|    |_____/ \__,_|___/_.__/ \___/ \__,_|_|  \__,_|
                     ---------------------------------------------------------------------
            )";
        }
        else if (definedUser) {
            cout << "Please login using your PiLocks account." << endl;
            cout << "What is your username?" << endl;
            printPrompt();
            cin >> userLoggingIn;
            cout << "What is your password?" << endl;
            printPrompt();
            cin >> passwordLoggingIn;
            string hashedUserLoggingIn = obj.hashUsername(userLoggingIn);
            correctLoginAdmin = obj.checkAdminAuthentication(hashedUserLoggingIn, passwordLoggingIn);
            if (correctLoginAdmin) {
                obj.setLoggedInUser(userLoggingIn);  // Store the username
            }
            correctUserLogin = obj.checkUserAuthentication(hashedUserLoggingIn, passwordLoggingIn);
        }
        else {
            cout << RED << "Something went wrong" << RESET << endl;
            this_thread::sleep_for(chrono::seconds(5));
        }
    }
}