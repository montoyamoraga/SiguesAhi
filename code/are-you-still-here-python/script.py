# libraries
import requests
import json
import os

# clear screen
os.system('cls' if os.name == 'nt' else 'clear')

urlFull = "https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&exintro&explaintext&redirects=1&titles=National_Rifle_Association&exchars=128"

x = requests.get(urlFull)

# if x.status_code == 200:
#     print("request happened")

wordsExistYes = [" is ", " are "]
wordsExistNo = [" was ", " were "]

stillExists = True

# go through every character in the wikipedia article
for i in range(len(x.text)):

    # go through every word in no
    for no in wordsExistNo:
        # if the no word is in the wikipedia text
        if (no == x.text[i:i+len(no)]):
           stillExists = False

# after for loop is over declare result

if stillExists:
    print("oh no NRA still exists")
else:
    print("yay NRA is over")

# print whitespace
for i in range(24):
    print("")