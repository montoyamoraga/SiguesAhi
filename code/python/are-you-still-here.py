# are-you-still-here-python

# a project by aaron montoya-moraga
# github.com/montoyamoraga/are-you-still-here

# started in september 2020
# alpha state
# it is working but it is messy

# instructions:
# run on terminal

# import included Python modules
import os
import json
import time
import datetime

# import Python modules that need installation
import requests

# variables defined by user
# name of organization
organization = "National_Rifle_Association"
# time to wait, measured in seconds
interval = 60;
# print timestamp
printTimeStamp = True;
# print still exists
printExistence = True;

# clear screen
os.system('cls' if os.name == 'nt' else 'clear')

urlFull = "https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&explaintext&titles=" + organization + "&exchars=128"

x = requests.get(urlFull)

# if x.status_code == 200:
#     print("request happened")

wordsExistYes = [" is ", " are "]
wordsExistNo = [" was ", " were "]

stillExists = True

while True:
  # go through every character in the wikipedia article
  for i in range(len(x.text)):

      # go through every word in no
      for no in wordsExistNo:
          # if the no word is in the wikipedia text
          if (no == x.text[i:i+len(no)]):
            stillExists = False

  # after for loop is over declare result

  if printTimeStamp:
    print(datetime.datetime.now().isoformat(timespec="seconds"), end=" ")

  if stillExists:
    if printExistence:
      print("it is still here")
  else:
      print("it is no more")

  time.sleep(interval)
