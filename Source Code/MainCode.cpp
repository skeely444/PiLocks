#include <iostream>
#include <string>
#include <vector>
#include <fstream>
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
        cout << action << user << endl;
        ofstream file("Logs.json");

        file << "{\n";
        file << "  \"Logged actions\": [\n";
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
                cout << "Welcome to UserDb. Please log in using your account." << endl
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
                    cout << "Sorry, this user doesn't exist. Please check your password and username again." << endl;
                }
            }
            else if (correctLoginAdmin || checkLoggedInState) {
                cout << "Welcome back. Which action would you like to perform?" << endl
                    << "Add a user" << endl
                    << "Remove a user" << endl
                    << "View all users" << endl
                    << "Exit program" << endl;
                string choice;
                cin >> choice;
                if (choice == "View") {
                    obj.returnAll();
                    
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
                    obj.saveToLog(choice, userLoggingIn);
                }
                else if (choice == "Remove") {
                    cout << "Please enter the name of the user you want to remove." << endl;
                    string toRemove;
                    cin >> toRemove;
                    obj.removeUser(toRemove);
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