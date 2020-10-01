# are-you-still-here-python

# a project by aaron montoya-moraga
# github.com/montoyamoraga/are-you-still-here

# started in september 2020
# this is the Python flavor of this project
# v0.0.1

# instructions:
# 0. install script requirements
# 1. select dif 
# 2. run this script on a computer with internet connection

# import included Python modules
import os
import json
import time
import datetime

# import Python modules that need installation
import requests

# variables defined by user

# name of organization
# replace with the institution you want
userOrganization = "National_Rifle_Association"

# time to wait, measured in seconds
# 1 minute
userInterval = 60;
# for 1 hour replace with
userInterval = 60 * 60;
# for 1 day replace with
# userInterval = 60 * 60 * 24;

# print timestamp or not
# True: prints the timestamp every time this script checks
# False: does not print the timestamp
userPrintTimeStamp = True;

# print positive results
# True: script prints
userPrintPositive = True;

# clear terminal screen
os.system('cls' if os.name == 'nt' else 'clear')

# API request to Wikipedia
# request the first 128 characters of the first paragraph
urlFull = "https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&explaintext&exchars=128&titles=" + userOrganization

# do the request
result = requests.get(urlFull)

# words to check if organization still exists or not
# yes words are in present tense
wordsExistYes = [" is ", " are "]
# no words are in past tense
wordsExistNo = [" was ", " were "]

# boolean to signal if the organization still exists
# default value is True
stillExists = True

# infinite loop
while True:

  # iterate over every character in the wikipedia article
  for i in range(len(result.text)):

      # go through every word in no
      for no in wordsExistNo:
          # check if the word in no matches the Wikipedia text
          if (no == result.text[i:i+len(no)]):
            # if it does, declare the non existence of the institution
            stillExists = False

  # print the existence or non existence of the institution
  if stillExists:
    
    if userPrintPositive:
      # check if the user wants the time stamp
      if userPrintTimeStamp:
        # print the timestamp, omit newline character
        print(datetime.datetime.now().isoformat(timespec="seconds"), end=" ")
      print("it is still here")
  else:
      # check if the user wants the time stamp
      if userPrintTimeStamp:
        # print the timestamp, omit newline character
        print(datetime.datetime.now().isoformat(timespec="seconds"), end=" ")
      print("it is no more")

  # wait for one cycle
  time.sleep(userInterval)
