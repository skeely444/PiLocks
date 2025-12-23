#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>
using namespace std;

void printPrompt() {
    cout << "\033[36m> \033[0m";
}
void clearScreen() {
    cout << "\033[2J\033[H";
}
const string RESET = "\033[0m";
const string RED = "\033[31m";
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
    void addUser(string name, string role, string password, int id) {
        Usernames.push_back(name);
        Userroles.push_back(role);
        UserIds.push_back(id);
        Userpasswords.push_back(password);
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
            if (attemptedUser == Usernames[i] && attemptedPassword == Userpasswords[i] && Userroles[i] == "Admin") {
                correctLoginAdmin = true;
            }
        }
        return correctLoginAdmin;
    }
    bool checkUserAuthentication(string attemptedUser, string attemptedPassword) {
        for (int i = 0; i < Usernames.size(); i++) {
            if (attemptedUser == Usernames[i] && attemptedPassword == Userpasswords[i] && Userroles[i] == "User") {
                correctUserLogin = true;
            }
        }
        return correctUserLogin;
    }
    bool checkIfAdminLoggedIn() {
        bool AdminAlreadySignedIn;
        AdminAlreadySignedIn = false;
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
                cout << "Removed user: " << Usernames[i];
            }
        }
    }
    void saveToLog(string action, string user) {
        ActionsPerformed.push_back(action);
        UserPerformed.push_back(user);
    }
    void getActivityLogs() {
        string buffer = getTimestamp();
        for (int i = 0; i < ActionsPerformed.size(); i++) {
            cout << UNDERLINE << buffer << ActionsPerformed[i] << UserPerformed[i] << RESET << endl;
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
            cout << "Wich password would you like to use?" << endl;
            printPrompt();
            cin >> firstUserPassword;
            obj.addUser(firstUserName, firstUserRole, firstUserPassword, firstUserId);
            obj.saveData();
            clearScreen();
        }
        else if (correctLoginAdmin) {
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
            string buffer = getTimestamp();
            obj.getActivityLogs();
            cout << endl << "Available actions: " << endl;
            cout << " 1) Add User" << endl
                << " 2) Remove User" << endl
                << " 3) View Users" << endl
                << " 4) Exit Program" << endl;
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
                obj.addUser(userNameAdding, userRoleAdding, userPasswordAdding, userIdAdding);
                obj.saveData();
                obj.saveToLog(choice, userLoggingIn);
            }
            else if (choice == "View") {
                clearScreen();
                obj.returnAll();
                this_thread::sleep_for(chrono::seconds(10));
            }
            else if (choice == "Remove") {
                clearScreen();
                cout << "Wich user would you like to remove?" << endl;
                string userToRemove;
                printPrompt();
                cin >> userToRemove;
                obj.removeUser(userToRemove);
                obj.saveData();
            }
            else if (choice == "Exit") {
                use = false;
            }
            else {
                cout << RED << "No valid choice detected" << RESET << endl;
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
            correctLoginAdmin = obj.checkAdminAuthentication(userLoggingIn, passwordLoggingIn);
            correctUserLogin = obj.checkUserAuthentication(userLoggingIn, passwordLoggingIn);
        }
        else {
            cout << RED << "Something went wrong" << RESET << endl;
        }
    }
}