import json
import os
import time
import datetime

import bcrypt as bc
import pandas as pd
import watchdog.observers as wd
import watchdog.events as we
import colorama
from colorama import Fore, Back, Style
colorama.init(autoreset=True)

DateAndTime = datetime.datetime.now().strftime("%x") + ' ' + datetime.datetime.now().strftime("%X")
print(f"{Style.BRIGHT}{DateAndTime}")
