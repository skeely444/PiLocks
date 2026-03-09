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
    with open("Users.json", "r") as json_file:
       UserList = (json.load(json_file))  

print(r"""
                                             ___       _                    _            
                                            (  _`\  _ ( )                  ( )
                                            | |_) )(_)| |       _      ___ | |/')   ___  
                                            | ,__/'| || |  _  /'_`\  /'___)| , <  /',__) 
                                            | |    | || |_( )( (_) )( (___ | |\`\ \__, \ 
                                            (_)    (_)(____/'`\___/'`\____)(_) (_)(____/ 
""")
use = True