from flask import jsonify

def hextojson(hexIn):
    hexIn = hexIn.lower()
    if(len(hexIn)==7):
        n = 1
    else:
        n = 0
    color ={}
    color["red"] = int(hexIn[n:n+2], 16)
    color["green"] = int(hexIn[n+2:n+4], 16)
    color["blue"] = int(hexIn[n+4:n+6], 16)
    return color

def jsontohex(color):
    hexOut = "#"
    hexOut += "{:02X}".format(color["red"])
    hexOut += "{:02X}".format(color["green"])
    hexOut += "{:02X}".format(color["blue"])
    return hexOut
