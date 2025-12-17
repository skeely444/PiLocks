#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std;

class UserProfile {
public:
    int i = 0;
    void addUser(string name, string role, int id, string password) {
        Usernames.push_back(name);
        Userroles.push_back(role);
        UserIds.push_back(id);
        Userpasswords.push_back(password);
    }
    void returnAll() {
        int i = 0;
        for (; i < Usernames.size(); i++)
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
        int i = 0;
        for (; i < Usernames.size(); i++) {
            if (attemptedUser == Usernames[i] && attemptedPassword == Userpasswords[i] && Userroles[i] == "Admin") {
                correctLoginAdmin = true;
            }
        }
        return correctLoginAdmin;
    }
    bool checkUserAuthentication(string attemptedUser, string attemptedPassword) {
        int i = 0;
        for (; i < Usernames.size(); i++) {
            if (attemptedUser == Usernames[i] && attemptedPassword == Userpasswords[i] && Userroles[i] == "User") {
                correctUserLogin = true;
            }
        }
        return correctUserLogin;
    }
    bool checkIfLoggedIn() {
        bool alreadySignedIn;
        alreadySignedIn = false;
        if (correctLoginAdmin) {
            alreadySignedIn = true;
        }
        return alreadySignedIn;
    }
    void removeUser(string removing) {
        int i = 0;
        for (; i < Usernames.size(); i++) {
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

        ofstream file("Logs.json");

        file << "{\n";
        file << "  \"Logged actions\": [\n";

        for (int i = 0; i < ActionsPerformed.size(); i++) {
            file << "{\n";
            file << "    \"User\": \"" << UserPerformed[i] << "\",\n";
            file << "    \"Action\": \"" << ActionsPerformed[i] << "\"\n";
            file << "}\n";
        }

        file << "  ]\n";
        file << "}\n";
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
    bool checkLoggedInState = false;
    while (use) {
        obj.loadData();
        bool correctLoginAdmin = false;
        bool correctUserLogin = false;
        bool checkLoggedInState = obj.checkIfLoggedIn();
        bool definedUser = obj.checkIfUsers();
        string userLoggingIn;
        string passwordLoggingIn;
        if (definedUser) {
            if (!checkLoggedInState) {
                cout << "Welcome to UserDb. Please log in using your account." <<  endl
                    << "Username: ";
                cin >> userLoggingIn;
                cout << "Password: ";
                cin >> passwordLoggingIn;
                correctLoginAdmin = obj.checkAdminAuthentication(userLoggingIn, passwordLoggingIn);
                if (!correctLoginAdmin && definedUser) {
                    correctUserLogin = obj.checkUserAuthentication(userLoggingIn, passwordLoggingIn);
                    if (correctUserLogin) {
                        cout << "Welcome, User" << endl;
                    }
                    cout << RED << "Sorry, this user doesn't exist. Please check your password and username again." <<  RESET << endl;
                }
            }
            else if (correctLoginAdmin || checkLoggedInState) {
                clearScreen();
                string userPerforming = userLoggingIn;
                cout << "Welcome back. Which action would you like to perform?" << endl
                    << "1) Add user" << endl
                    << "2) Remove user" << endl
                    << "3) View users" << endl
                    << "4) Exit" << endl;
                string choice;
                printPrompt();
                cin >> choice;
                if (choice == "View") {
                    clearScreen;
                    obj.returnAll();
                    obj.saveToLog(choice, userPerforming);
                    this_thread::sleep_for(chrono::seconds(3));
                    clearScreen();
                }
                else if (choice == "Exit") {
                    use = false;
                }
                else if (choice == "Add") {
                    cout << "Please enter the details of the user you want to add." << endl
                        << "Username: ";
                    string userCreating;
                    cin >> userCreating;
                    cout << "Password: ";
                    string passwordCreating;
                    cin >> passwordCreating;
                    cout << "Role: ";
                    string roleCreating;
                    cin >> roleCreating;
                    cout << "Id: ";
                    int idCreating;
                    cin >> idCreating;
                    obj.addUser(userCreating, roleCreating, idCreating, passwordCreating);
                    obj.saveData();
                }
                else if (choice == "Remove") {
                    cout << "Please enter the name of the user you want to remove." << endl;
                    string toRemove;
                    cin >> toRemove;
                    obj.removeUser(toRemove);
                }
                else {
                    cout << RED << "No valid choice detected" << RESET << endl;
                    this_thread::sleep_for(chrono::seconds(3)); this_thread::sleep_for(chrono::seconds(3));
                }
            }
        }
        if (!definedUser) {
            cout << "Welcome to UserDb. Please create an Admin account to access this DB" << endl
                << "Username: ";
            string userSigningIn;
            cin >> userSigningIn;
            cout << "Password: ";
            string passwordSigningIn;
            cin >> passwordSigningIn;
            cout << "Role: ";
            string roleSigningIn;
            cin >> roleSigningIn;
            cout << "Id: ";
            int idSigningIn;
            cin >> idSigningIn;
            obj.addUser(userSigningIn, roleSigningIn, idSigningIn, passwordSigningIn);
            obj.saveData();
        }
    }
}