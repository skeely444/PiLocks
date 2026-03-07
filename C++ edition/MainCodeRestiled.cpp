#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>
#include <filesystem>
#include "bcrypt.h"
using namespace std;
namespace fs = std::filesystem;



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
const string BRIGHTBLUE = "\033[94m";
const string BRIGHTCYAN = "\033[96m";
const string YELLOW = "\033[33m";
const string CYAN = "\033[36m";
const string MAGENTA = "\033[35m";
const string BOLD = "\033[1m";
const string DIM = "\033[2m";
const string UNDERLINE = "\033[4m";
const string ITALIC = "\033[3m";

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
    bool change = false;
    string loggedInUser = "";  // Add this member variable
    string clearLoggedInUser = "";
    
    void setLoggedInUser(string user) {
        loggedInUser = user;
    }
    void setClearLoggedInUser(string user) {
        clearLoggedInUser = user;
    }
    void addUser(string name, string role, string password, int id) {
        Usernames.push_back(name);
        Userroles.push_back(role);
        UserIds.push_back(id);
        Userpasswords.push_back(password);
    }
    bool checkIfUserOrOwnerExistAlready(string name, string role) {
        bool Exists = false;
        for (int i = 0; i < Usernames.size(); i++) {
            if (name == Usernames[i]) {
                cout << RED << "Sorry, this user already exists. Please use a different name." << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
                Exists = true;
            }
            if (role == "Owner") {
                cout << RED << "Sorry, there is already a user with the owner role. Please use a different role." << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
                Exists = true;
            }
        }
        return Exists;
    }
    bool checkOwnerPrivileges() {
        bool hasOwnerRole = false;
        for (int i = 0; i < Usernames.size(); i++) {
            if ((Usernames[i] == loggedInUser) && (Userroles[i] == "Owner")) {
                hasOwnerRole = true;
                return hasOwnerRole;
                break;
            }
            else
            {
                hasOwnerRole = false;
            }
        }
        return hasOwnerRole;
    }
    string hashUsername(string username) {
        vector <int> individualHash;
        for (int i = 0; i < username.length(); i++) {
            int AsciiChar = username[i];
            int hashResult = (((AsciiChar * AsciiChar) - (AsciiChar + AsciiChar) * (AsciiChar % 2) + AsciiChar) / AsciiChar) * (AsciiChar * AsciiChar + AsciiChar);
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
        return hashedPassword;
    }
    void returnAll() {
        for (int i = 0; i < Usernames.size(); i++)
            cout << Usernames[i] << " " << UserIds[i] << " " << Userroles[i] << " " << endl;
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
            if (attemptedUser == Usernames[i] && bcrypt::validatePassword(attemptedPassword, Userpasswords[i]) && (Userroles[i] == "Admin" || Userroles[i] == "Owner")) {
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
        if (correctUserLogin) {
            UserAlreadySignedIn = true;
        }
        return UserAlreadySignedIn;
    }
    void removeUser(string removing) {
        for (int i = 0; i < Usernames.size(); i++) {
            if (removing == Usernames[i]) {
                if (Userroles[i] != "Owner") {
                    RemovedUsers.push_back(removing);
                    Usernames.erase(Usernames.begin() + i);
                    Userpasswords.erase(Userpasswords.begin() + i);
                    Userroles.erase(Userroles.begin() + i);
                    UserIds.erase(UserIds.begin() + i);
                    break;  // Exit after removing the first match
                }
                else if (Userroles[i] == "Owner") {
                    cout << RED << "The user with the role 'Owner' cannot be removed." << RESET << endl;
                    this_thread::sleep_for(chrono::seconds(5));
                    break;
                }
            }
        }
    }
    bool antiAdminAbuse(string trying) {
        bool abused = false;
        for (int i = 0; i < RemovedUsers.size(); i++) {
            if (trying == RemovedUsers[i]) {
                abused = true;
                break;
            }
            else {
                abused = false;
            }
        }
        return abused;
    }
    void promoteUser(string promoting) {
        for (int i = 0; i < Usernames.size(); i++) {
            if (promoting == Usernames[i] && Userroles[i] == "User") {
                Userroles[i] = "Admin";
                break;
            }
            else if (promoting == Usernames[i] && (Userroles[i] == "Admin" || Userroles[i] == "Owner")) {
                cout << RED << "This person already has admin permissions." << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
            }
        }
    }
    void saveToLog(string action) {  // Remove user parameter
        ActionsPerformed.push_back(action);
        UserPerformed.push_back(clearLoggedInUser);  // Use member variable
        ActionTimestamps.push_back(getTimestamp());

        change = true;

        ofstream file("userLogs.json");

        file << "{\n";
        file << " \"Actions\": [\n";

        for (int i = 0; i < ActionsPerformed.size(); i++) {
            file << "    {\n";
            file << "      \"user\": \"" << UserPerformed[i] << "\",\n";
            file << "      \"action\": \"" << ActionsPerformed[i] << "\",\n";
            file << "      \"time\": \"" << ActionTimestamps[i] << "\"\n";

            if (i == ActionsPerformed.size() - 1)
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
                time.erase(time.size() - 1, 1);
                ActionTimestamps.push_back(time);
            }
        }

        for (int i = 0; i < ActionsPerformed.size(); i++) {
            string timestamp = ActionTimestamps[i];  // Use stored timestamp
            if (ActionsPerformed[i] == "Add") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << GREEN << " added" << RESET << " a new user." << endl;
            }
            else if (ActionsPerformed[i] == "View") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << BLUE << " viewed" << RESET << " all users." << endl;
            }
            else if (ActionsPerformed[i] == "Remove") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << RED << " removed" << RESET << " a user." << endl;
            }
            else if (ActionsPerformed[i] == "Promote") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << CYAN << " promoted" << RESET << " a user." << endl;
            }
            else if (ActionsPerformed[i] == "Late") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << " will be " << YELLOW << "late." << RESET << endl;
            }
            else if (ActionsPerformed[i] == "Appointment") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << " made an " << MAGENTA << "appointment." << RESET << endl;
            }
            else if (ActionsPerformed[i] == "Approve") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << BRIGHTBLUE << " approved" << RESET << " an appointment." << endl;
            }
            else if (ActionsPerformed[i] == "Clear") {
                cout << "[" << timestamp << "]" << " " << UserPerformed[i] << BRIGHTCYAN << " cleared" << RESET << " the logs." << endl;
            }
        }
    }
    void clearLogs(string confirmation) {
        if (filesystem::exists("userLogs.json")) {
            if (confirmation == "Yes" || confirmation == "yes") {
                filesystem::remove("userLogs.json");
                cout << GREEN << "Logs removed successfully." << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
            }
            else {
                cout << RED << "Action canceled." << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
                return;
            }
        }
        else {
            cout << RED << "There are no logs." << RESET << endl;
            this_thread::sleep_for(chrono::seconds(5));
            return;
        }
    }
    void makeAppointment(string who, string when) {
        UserAppointments.push_back(who);
        UsersWithAppointments.push_back(clearLoggedInUser);
        UserAppointmentsTimes.push_back(when);
        AppointmentStatus.push_back("Pending");

        ofstream file("userAppointments.json");

        file << "{\n";
        file << " \"Appointments\": [\n";

        for (int i = 0; i < UserAppointments.size(); i++) {
            file << "    {\n";
            file << "      \"user\": \"" << UsersWithAppointments[i] << "\",\n";
            file << "      \"type\": \"" << UserAppointments[i] << "\",\n";
            file << "      \"time\": \"" << UserAppointmentsTimes[i] << "\",\n";
            file << "      \"status\": \"" << AppointmentStatus[i] << "\"\n";

            if (i == UserAppointments.size() - 1) {
                file << "    }\n";
            }
            else
                file << "    },\n";
            
        }
        file << "  ]\n";
        file << "}\n";

        file.close();
    }
    void saveAppointment() {
        ofstream file("userAppointments.json");

        file << "{\n";
        file << " \"Appointments\": [\n";

        for (int i = 0; i < UserAppointments.size(); i++) {
            file << "    {\n";
            file << "      \"user\": \"" << UsersWithAppointments[i] << "\",\n";
            file << "      \"type\": \"" << UserAppointments[i] << "\",\n";
            file << "      \"time\": \"" << UserAppointmentsTimes[i] << "\",\n";
            file << "      \"status\": \"" << AppointmentStatus[i] << "\"\n";

            if (i == UserAppointments.size() - 1) {
                file << "    }\n";
            }
            else
                file << "    },\n";

        }
        file << "  ]\n";
        file << "}\n";

        file.close();
    }
    void getAppointments() {
        ifstream file("userAppointments.json");

        if (!file.is_open()) {
            return;
        }

        UserAppointments.clear();
        UsersWithAppointments.clear();
        AppointmentStatus.clear();
        UserAppointmentsTimes.clear();

        string word;
        while (file >> word) {
            if (word == "\"user\":") {
                string user;
                file >> user;
                user.erase(0, 1);
                user.erase(user.size() - 2, 2);
                UsersWithAppointments.push_back(user);
            }
            else if (word == "\"type\":") {
                string type;
                file >> type;
                type.erase(0, 1);
                type.erase(type.size() - 2, 2);
                UserAppointments.push_back(type);
            }
            else if (word == "\"time\":") {
                string time;
                file >> time;
                time.erase(0, 1);
                time.erase(time.size() - 2, 2);
                UserAppointmentsTimes.push_back(time);
            }
            else if (word == "\"status\":") {
                string status;
                file >> status;
                status.erase(0, 1);
                status.erase(status.size() - 1, 1);
                AppointmentStatus.push_back(status);
            }
        }
        file.close();


        for (int i = 0; i < UserAppointments.size(); i++) {
            if (AppointmentStatus[i] == "Pending") {
                cout << " Pending:" << endl;
                cout << " " << UsersWithAppointments[i] << " has an appointment with " << UserAppointments[i] << " at " << UserAppointmentsTimes[i] << ". This appointment has not been approved yet." << endl;
            }
        }
        for (int i = 0; i < UserAppointments.size(); i++) {
            if (AppointmentStatus[i] == "Approved") {
                cout << endl << " Approved:" << endl;
                cout << " " << UsersWithAppointments[i] << " has an appointment with " << UserAppointments[i] << " at " << UserAppointmentsTimes[i] << ". This appointment is approved." << endl;
            }
        }
        for (int i = 0; i < UserAppointments.size(); i++) {
            if (AppointmentStatus[i] == "Unapproved") {
                cout << endl << " Unapproved:" << endl;
                cout << " " << UsersWithAppointments[i] << " has an appointment with " << UserAppointments[i] << " at " << UserAppointmentsTimes[i] << ". This appointment is unapproved." << endl;
            }
        }
    }
    void approveAppointment(string whoApprove) {
        if (whoApprove == "Cancel") {
            return;
        }
        for (int i = 0; i < UsersWithAppointments.size(); i++) {
            if (UsersWithAppointments[i] == whoApprove) {
                AppointmentStatus[i] = "Approved";
                cout << "Appointment approved.";
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
    vector <string> UsersWithAppointments;
    vector <string> UserAppointments;
    vector <string> UserAppointmentsTimes;
    vector <string> AppointmentStatus;
    vector <string> RemovedUsers;
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
    bool ownerPrivileges = false;
    while (use) {
        obj.loadData();
        bool exists = false;
        checkAdminLoggedInState = obj.checkIfAdminLoggedIn();
        checkUserLoggedInState = obj.checkIfUserLoggedIn();
        definedUser = obj.checkIfUsers();
        string userLoggingIn;
        string passwordLoggingIn;
        if (!definedUser) {
            string firstUserName;
            int firstUserId = 1;
            string firstUserRole = "Owner";
            string firstUserPassword;
            cout << "Welcome to PiLocks. To start, please create a PiLocks account." << endl;
            cout << "How would you like to be called?" << endl;
            printPrompt();
            cin >> firstUserName;
            cout << "Which password would you like to use?" << endl;
            printPrompt();
            cin >> firstUserPassword;
            string hashedFirstUserName = obj.hashUsername(firstUserName);
            cout << "Processing..." << endl;
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
            cout << "Logged in as " << obj.clearLoggedInUser << endl << endl;
            cout << "Logs:" << endl;
            obj.getActivityLogs();
            cout << endl << "Appointments:" << endl;
            obj.getAppointments();
            cout << endl << "Available actions:" << endl;
            cout << " 1) " << BOLD << "Add " << RESET << "User" << endl
                << " 2) " << BOLD << "Remove " << RESET << "User" << endl
                << " 3) " << BOLD << "View " << RESET << "Users" << endl
                << " 4) " << BOLD << "Approve " << RESET << "Appointment" << endl
                << " 5) " << BOLD << "Monitor " << RESET << "Live logs" << endl
                << " 6) " << BOLD << "Promote " << RESET << "User (Owner only)" << endl
                << " 7) " << BOLD << "Clear " << RESET << "Logs (Owner only)" << endl
                << " 8) " << BOLD << "Exit" << RESET << endl
                << " 9) " << BOLD << "Logout" << RESET << endl;
            string choice;
            printPrompt();
            cin >> choice;
            if (choice == "Add") {
                string userNameAdding;
                string userRoleAdding;
                string userPasswordAdding;
                int userIdAdding;
                bool antiRaidTriggered = false;
                clearScreen();
                cout << "Please enter the details of the new user below:" << endl
                    << " Username:" << endl;
                printPrompt();
                cin >> userNameAdding;
                cout << " Role (Admin or User):" << endl;
                printPrompt();
                cin >> userRoleAdding;
                cout << " Password:" << endl;
                printPrompt();
                cin >> userPasswordAdding;
                cout << " ID:" << endl;
                printPrompt();
                cin >> userIdAdding;
                string hashedUser = obj.hashUsername(userNameAdding);
                cout << "Processing..." << endl;
                string hashedPassword = obj.hashPassword(userPasswordAdding);
                antiRaidTriggered = obj.antiAdminAbuse(hashedUser);
                if (!antiRaidTriggered) {
                    exists = obj.checkIfUserOrOwnerExistAlready(hashedUser, userRoleAdding);
                    if (!exists) {
                        obj.addUser(hashedUser, userRoleAdding, hashedPassword, userIdAdding);
                        obj.saveData();
                        obj.saveToLog(choice);
                    }
                }
                else if (antiRaidTriggered) {
                    cout << RED << "The user you are trying to add has been removed recently. To prevent abuse, this user cannot be added within a chosen timeframe." << RESET << endl;
                    this_thread::sleep_for(chrono::seconds(5));
                }
            }
            else if (choice == "View") {
                clearScreen();
                obj.returnAll();
                obj.saveToLog(choice);
                this_thread::sleep_for(chrono::seconds(5));
            }
            else if (choice == "Remove") {
                clearScreen();
                cout << "Which user would you like to remove?" << endl;
                string userToRemove;
                printPrompt();
                cin >> userToRemove;
                string hashedUserToRemove = obj.hashUsername(userToRemove);
                obj.removeUser(hashedUserToRemove);
                obj.saveData();
                obj.saveToLog(choice);
            }
            else if (choice == "Promote") {
                ownerPrivileges = obj.checkOwnerPrivileges();
                if (ownerPrivileges) {
                    clearScreen();
                    string userToPromote;
                    cout << "Which user would you like to promote to Admin?" << endl;
                    printPrompt();
                    cin >> userToPromote;
                    string hashedUserToPromote = obj.hashUsername(userToPromote);
                    obj.promoteUser(hashedUserToPromote);
                    obj.saveToLog(choice);
                    obj.saveData();
                }
                else {
                    cout << RED << "Sorry, this function is owner only." << RESET << endl;
                    this_thread::sleep_for(chrono::seconds(5));
                }
            }
            else if (choice == "Approve") {
                cout << endl << "Which appointment would you like to approve?" << endl;
                printPrompt();
                string toApprove;
                cin >> toApprove;
                obj.approveAppointment(toApprove);
                obj.saveAppointment();
                obj.saveToLog(choice);
            }
            else if (choice == "Monitor") {
                bool yes = true;
                auto lastModified = fs::last_write_time("userLogs.json");
                while (yes) {
                    auto currentModifiedTime = fs::last_write_time("userLogs.json");
                    if (lastModified != currentModifiedTime) {
                        clearScreen();
                        cout << "Logs:" << endl;
                        obj.getActivityLogs();
                        lastModified = currentModifiedTime;
                    }
                    else {
                        cout << RED << "Waiting for changes." << RESET << endl;
                    }
                    this_thread::sleep_for(chrono::seconds(5));
                }
            }
            else if (choice == "Clear") {
                ownerPrivileges = obj.checkOwnerPrivileges();
                if (ownerPrivileges) {
                    string sure;
                    cout << "Do you really want to delete the logs? This action cannot be undone." << endl;
                    printPrompt();
                    cin >> sure;
                    obj.clearLogs(sure);
                    obj.saveToLog(choice);
                }
                else {
                    cout << RED << "Sorry, this function is owner only." << RESET << endl;
                    this_thread::sleep_for(chrono::seconds(5));
                }
            }
            else if (choice == "Logout") {
                checkAdminLoggedInState = false;
                correctLoginAdmin = false;
                obj.correctLoginAdmin = false;
                clearScreen();
            }
            else if (choice == "Exit") {
                use = false;
            }
            else {
                cout << RED << "No valid choice detected." << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
            }
        }
        else if (correctUserLogin || checkUserLoggedInState) {
            clearScreen();
            string choice;
            cout << R"(
                     --------------------------------------------------------------------- 
                      _    _                 _____            _                         _ 
                     | |  | |               |  __ \          | |                       | |
                     | |  | |___  ___ _ __  | |  | | __ _ ___| |__   ___   __ _ _ __ __| |
                     | |  | / __|/ _ \ '__| | |  | |/ _` / __| '_ \ / _ \ / _` | '__/ _` |
                     | |__| \__ \  __/ |    | |__| | (_| \__ \ |_) | (_) | (_| | | | (_| |
                      \____/|___/\___|_|    |_____/ \__,_|___/_.__/ \___/ \__,_|_|  \__,_|
                     ---------------------------------------------------------------------
            )" << endl << endl;
            cout << "Available actions:" << endl;
            cout << " 1) Being " << BOLD << "Late" << RESET << endl
                << " 2) Make " << BOLD << "Appointment" << RESET << endl
                << " 3) " << BOLD << "Exit" << RESET << endl
                << " 4) " << BOLD << "Logout" << RESET << endl;
            printPrompt();
            cin >> choice;
            if (choice == "Late") {
                clearScreen();
                int minutes;
                cout << "How late will you be?" << endl;
                printPrompt();
                cin >> minutes;
                obj.saveToLog(choice);
            }
            else if (choice == "Appointment") {
                clearScreen();
                string whoAppointment;
                string whenAppointment;
                cout << "Who do you have an appointment with?" << endl;
                printPrompt();
                cin >> whoAppointment;
                cout << "When is your appointment? [hh:mm]" << endl;
                printPrompt();
                cin >> whenAppointment;
                obj.makeAppointment(whoAppointment, whenAppointment);
                obj.saveToLog(choice);
            }
            else if (choice == "Logout") {
                checkUserLoggedInState = false;
                correctUserLogin = false;
                obj.correctUserLogin = false;
                clearScreen();
            }
            else if (choice == "Exit") {
                use = false;
            }
            else {
                cout << RED << "No valid choice detected." << RESET << endl;
                this_thread::sleep_for(chrono::seconds(5));
            }
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
            cout << "Processing..." << endl;
            correctLoginAdmin = obj.checkAdminAuthentication(hashedUserLoggingIn, passwordLoggingIn);
            correctUserLogin = obj.checkUserAuthentication(hashedUserLoggingIn, passwordLoggingIn);
            if (correctLoginAdmin || correctUserLogin) {
                obj.setLoggedInUser(hashedUserLoggingIn);
                obj.setClearLoggedInUser(userLoggingIn);
            }
            clearScreen();
        }
        else {
            cout << RED << "Something went wrong." << RESET << endl;
            this_thread::sleep_for(chrono::seconds(5));
        }
    }
}