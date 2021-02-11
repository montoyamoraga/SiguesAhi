#######
# about
#######

# are you still here?
# a project by aaron montoya-moraga
# available at 
# github.com/montoyamoraga/are-you-still-here

# are-you-still-here.py
# this is the desktop Python flavor
# started in september 2020
# modified in october 2020
# v0.0.1

##############
# instructions
##############

# 0. install script requirements
# 1. upgrade your Python installation if needed
# 2. modify in this script the user defined variables
# 3. run this script on a computer with internet connection

################
# import modules
################

# import Python modules
import os
import json
import time
import datetime

# import Python modules that need installation
import requests

########################
# user defined variables
########################

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

# messages of existence
userExistPositive = "it is still here"
userExistNegative = "it is no more"

#####################
# setup initial state
#####################

# API request to Wikipedia
# request the first 128 characters of the first paragraph
urlFull = "https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&explaintext&exchars=128&titles=" + userOrganization

# do the request and store the result
result = requests.get(urlFull)

# words to check if organization still exists or not
# yes words are in present tense
wordsExistYes = [" is ", " are "]
# no words are in past tense
wordsExistNo = [" was ", " were "]

# boolean to signal if the organization still exists
# default value is True
stillExists = True

###############
# infinite loop
###############

while True:

  # iterate over every character in the wikipedia article
  for i in range(len(result.text)):

      # go through every word in no
      for no in wordsExistNo:
          # check if the word in no matches the Wikipedia text
          if (no == result.text[i:i+len(no)]):
            # if it does, declare the non existence of the institution
            stillExists = False

  # retrieve timestamp
  if userPrintTimeStamp:
    # if user wants a timestamp, calculate it
    timeStamp = datetime.datetime.now().isoformat(timespec="seconds") + " "
  else:
    # if user wants no timestamp, it is empty
    timeStamp = ""

  # if the institution still exists
  if stillExists:
    # print positive existence to terminal
    print(timeStamp + userExistPositive)
  
  # if the institution is no more
  else:
    # print negative existence to terminal
    print(timeStamp + currentuserExistNegative)

  # sleep for one cycle
  time.sleep(userInterval)
