from base64 import encode
import json
import os
import time
import datetime
import random

import bcrypt as bc
import pandas as pd
import watchdog.observers as wd
import watchdog.events as we
import colorama
from colorama import Fore, Back, Style
colorama.init(autoreset=True)

DateAndTime = datetime.datetime.now().strftime("%x") + ' ' + datetime.datetime.now().strftime("%X")

UserList = []
Actions = []
UserDidAction = []
ActionTimeStamp = []
JSONifiedUser = ''
authorized = False
userNameLoggingIn = ''
userPasswordLoggingIn = ''

def clearScreen():
    os.system('cls' if os.name == 'nt' else 'clear')

def createUser(name, password, role):
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

def removeUser(toRemove):
    found = False
    for index in range(len(UserList)):
        if UserList[index]["Name"] == toRemove and not UserList[index]["Role"] == "Owner":
            UserList.pop(index)
            saveUsersToDataFile()
            return
        else:    
            break
    print(f"{Fore.RED}Sorry, this user doesn't exist or to owner account has been provided. Please try again...")
    time.sleep(1.0)

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
        print('Welcome to the PiLocks dashboard. Please start by making an account.')
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
            input()
    elif len(UserList) > 0:
        print("Welcome back. Please log in using your PiLocks account.")
        userNameLoggingIn = input(f"Username\n{Fore.CYAN}>{Fore.RESET}")
        userPasswordLoggingIn = input(f"Password\n{Fore.CYAN}>{Fore.RESET}")
        checkAuthentication(userNameLoggingIn, userPasswordLoggingIn)