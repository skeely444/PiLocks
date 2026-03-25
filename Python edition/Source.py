from base64 import encode
import json
import os
import time
import datetime
import customtkinter
from pathlib import Path

import bcrypt as bc
import pandas as pd
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import colorama
from colorama import Fore, Back, Style
colorama.init(autoreset=True)

class MyHandler(FileSystemEventHandler):
    def on_modified(self, event):
        if event.src_path.endswith("Logs.json"):
            clearScreen()
            print("Press any key to exit\n")
            loadAction()
            time.sleep(5.0)

DateAndTime = datetime.datetime.now().strftime("%X")

UserList = []
Actions = []
JSONifiedUser = ''
authorized = False
succes = False
userNameLoggingIn = ''
userPasswordLoggingIn = ''

def clearScreen():
    os.system('cls' if os.name == 'nt' else 'clear')

def createUser(name, password, role):
    global succes
    exist = False
    for index in range(len(UserList)):
        if name == UserList[index]["Name"] or (UserList[index]["Role"] == "Owner" and role == "Owner"):
            exist = True
            break
        else:
            exist = False
    if exist:
        print(f"{Fore.RED}Sorry, this user already exists or an owner has already been created. Please use a different name or different role.")
        time.sleep(1.0)
        succes = False
    else:
        if len(UserList) == 0:
            id = 0
        else:
            id = UserList[-1]["Id"] + 1
        bytes = password.encode('utf-8')
        salt = bc.gensalt()
        hashedPassword = bc.hashpw(bytes, salt)
        decodedHashedPassword = hashedPassword.decode("utf-8")
        UserList.append({"Name": name, "Password": decodedHashedPassword, "Role": role, "Id": id})
        saveUsersToDataFile()
        print(f"{Fore.GREEN}User created succesfully.{Fore.RESET}")
        time.sleep(1.0)
        succes = True

def removeUser(toRemove):
    global succes
    global found
    found = False
    for index in range(len(UserList)):
        if UserList[index]["Name"] == toRemove and not UserList[index]["Role"] == "Owner":
            UserList.pop(index)
            saveUsersToDataFile()
            succes = True
            found = True
            print(f"{Fore.GREEN}User removed succesfully.")
            time.sleep(1.0)
            return True
    if not found:
        print(f"{Fore.RED}Sorry, this user doesn't exist or to owner account has been provided. Please try again...")
        time.sleep(1.0)
        succes = False
    else:
        return

def saveAction(action, preformer):
    global Actions
    Actions.append({"Action": action, "User": preformer, "Time": DateAndTime})
    with open("Logs.json", "w") as json_file:
        json.dump(Actions, json_file, indent=2)

def loadAction():
    global Actions
    Actions.clear()
    try:
        with open("Logs.json", "r") as json_file:
            Actions = json.load(json_file)
    except:
        return
    for index in range(len(Actions)):
        if Actions[index]["Action"] == "Add":
            print(f" [{Actions[index]["Time"]}] {Actions[index]["User"]} {Fore.GREEN}added{Fore.RESET} a user.")
        elif Actions[index]["Action"] == "Remove":
            print(f" [{Actions[index]["Time"]}] {Actions[index]["User"]} {Fore.RED}removed{Fore.RESET} a user.")

def saveUsersToDataFile():
    with open("Users.json", "w") as json_file:
        json.dump(UserList, json_file, indent=2)

def loadUsersToDataFile():
    global UserList
    UserList.clear()
    try:
        with open("Users.json", "r") as json_file:
           UserList = (json.load(json_file))
    except:
        return
       
def checkAuthentication(username, password):
    global authorized
    for index in range(len(UserList)):
        encodedCorrectPassword = UserList[index]["Password"].encode('utf-8')
        encodedTriedPassword = password.encode('utf-8')
        if UserList[index]["Name"] == username and bc.checkpw(encodedTriedPassword, encodedCorrectPassword):
            authorized = True
            return authorized
    print(f"{Fore.RED}Sorry, this info is incorrect")
    time.sleep(1.0)
    return
def getRole(user):
    for index in range(len(UserList)):
        if UserList[index]["Name"] == user:
            role = UserList[index]["Role"]
            return role

use = True
while (use):
    clearScreen()
    print(r"""
                                             ___       _                    _            
                                            (  _`\  _ ( )                  ( )
                                            | |_) )(_)| |       _      ___ | |/')   ___  
                                            | ,__/'| || |  _  /'_`\  /'___)| , <  /',__) 
                                            | |    | || |_( )( (_) )( (___ | |\`\ \__, \ 
                                            (_)    (_)(____/'`\___/'`\____)(_) (_)(____/ 
    """)
    loadUsersToDataFile()
    if len(UserList) == 0:
        print('Welcome to the PiLocks dashboard. Please start by making an account.\n')
        toBeCalled = input(f"Wich name would you like to use? \n{Fore.CYAN}>{Fore.RESET}")
        passwordToUse = input(f"Wich password would you like to use? \n{Fore.CYAN}>{Fore.RESET}")
        userRole = "Owner"
        createUser(toBeCalled, passwordToUse, userRole)
    elif authorized:
        userRole = getRole(userNameLoggingIn)
        clearScreen()
        if userRole == "Admin" or userRole == "Owner":
            print(r"""
                           ------------------------------------------------------------------------------------
                               _       _           _         ____            _     _                         _ 
                              / \   __| |_ __ ___ (_)_ __   |  _ \  __ _ ___| |__ | |__   ___   __ _ _ __ __| |
                             / _ \ / _` | '_ ` _ \| | '_ \  | | | |/ _` / __| '_ \| '_ \ / _ \ / _` | '__/ _` |
                            / ___ \ (_| | | | | | | | | | | | |_| | (_| \__ \ | | | |_) | (_) | (_| | | | (_| |
                           /_/   \_\__,_|_| |_| |_|_|_| |_| |____/ \__,_|___/_| |_|_.__/ \___/ \__,_|_|  \__,_|
                           ------------------------------------------------------------------------------------
            """)
            print(f"Logged in as {userNameLoggingIn}")
            print("\nLogs:")
            loadAction()
            print("\nAvailible actions:")
            print(f" 1) {Style.BRIGHT}Add{Style.RESET_ALL} user")
            print(f" 2) {Style.BRIGHT}Remove{Style.RESET_ALL} user")
            print(f" 3) {Style.BRIGHT}View{Style.RESET_ALL} users")
            print(f" 4) {Style.BRIGHT}Monitor{Style.RESET_ALL} mode")
            print(f" 5) {Style.BRIGHT}Logout{Style.RESET_ALL}")
            print(f" 6) {Style.BRIGHT}Exit{Style.RESET_ALL}")
            action = input(f"\n{Fore.CYAN}>{Fore.RESET}")
            if action == "Add":
                nameToCreate = input(f"\nWhat is the name of the user?\n{Fore.CYAN}>{Fore.RESET}")
                passwordToCreate = input(f"Which password will the user use?\n{Fore.CYAN}>{Fore.RESET}")
                roleToCreate = input(f"Which role(Admin/User) does this user fullfill?\n{Fore.CYAN}>{Fore.RESET}")
                createUser(nameToCreate, passwordToCreate, roleToCreate)
                if succes:
                    saveAction(action, userNameLoggingIn)
            elif action == "Remove":
                nameToRemove = input(f"\nWhich user do you wan't to remove?\n{Fore.CYAN}>{Fore.RESET}")
                removeUser(nameToRemove)
                if succes:
                    saveAction(action, userNameLoggingIn)
            elif action == "View":
                clearScreen()
                for index in range(len(UserList)):
                    print(f"{UserList[index]["Name"]} {UserList[index]["Role"]}")
                time.sleep(5.0)
                saveAction(action, userNameLoggingIn)
            elif action == "Monitor":
                path = Path(".")
                observer = Observer()
                handler = MyHandler()
                observer.schedule(handler, path)
                observer.start()
                wait = input()
                observer.stop()
            elif action == "Logout":
                authorized = False
                userNameLoggingIn = ''
            elif action == "Exit":
                use = False
    elif len(UserList) > 0:
        print("Welcome back. Please log in using your PiLocks account.\n")
        userNameLoggingIn = input(f"Username\n{Fore.CYAN}>{Fore.RESET}")
        userPasswordLoggingIn = input(f"Password\n{Fore.CYAN}>{Fore.RESET}")
        checkAuthentication(userNameLoggingIn, userPasswordLoggingIn)