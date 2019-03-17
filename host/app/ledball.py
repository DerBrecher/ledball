from flask import jsonify

class ledBallClass:
    state = True
    red = 255
    green = 128
    blue = 0
    hexColor = ""
    brightness = 100
    fadeSpeed = 100
    effectSpeed = 100
    effectNo = 1
    randomEffect= False
    randomColor = False
    rainbowColor = False

    def __init__(self):
        initalHex = "#"
        initalHex += "{:02X}".format(self.red)
        initalHex += "{:02X}".format(self.green)
        initalHex += "{:02X}".format(self.blue)
        self.hexColor = initalHex

    def getStateAsJson(self):
        json = {}
        json["state"] = self.state
        json["color"] = {}
        json["color"]["red"] = self.red
        json["color"]["green"] = self.green
        json["color"]["blue"] = self.blue
        json["brightness"] = self.brightness
        json["fadeSpeed"] = self.fadeSpeed
        json["effectSpeed"] = self.effectSpeed
        json["effectNo"] = self.effectNo
        json["randomEffect"] = self.randomEffect
        json["randomColor"] = self.randomColor
        json["rainbowColor"] = self.rainbowColor
        return jsonify(json)

    def getColorHex(self):
        hexOut = "#"
        hexOut += "{:02X}".format(self.red)
        hexOut += "{:02X}".format(self.green)
        hexOut += "{:02X}".format(self.blue)
        return hexOut

    def setColorHex(self, hexIn):
        hexIn = hexIn.lower()
        if(len(hexIn)==7):
            n = 1
        else:
            n = 0
        self.red = int(hexIn[n:n+2], 16)
        self.green = int(hexIn[n+2:n+4], 16)
        self.blue = int(hexIn[n+4:n+6], 16)
        self.hexColor = hexIn

    def handleHtmlForm(self, form):
        self.setColorHex(form["ledColor"]) #Update Manal Color

        if "state" in form: #Update State
            self.state = True
        else:
            self.state = False

        if form["effectInputType"] == "randomEffect": #Update Effect Input Type
            self.randomEffect = True
        else:
            self.randomEffect = False

        self.effectNo = form["effectNo"] #Update Manual Effect number
        self.effectSpeed = form["effectSpeed"] #Update Effect Speed
        self.fadeSpeed = form["fadeSpeed"] #Update Fade Speed

        #Update Color Input Type
        if form["colorInputType"] == "randomColor":
            self.randomColor = True
            self.rainbowColor = False
        if form["colorInputType"] == "rainbowColor":
            self.rainbowColor = True
            self.randomColor = False
        if form["colorInputType"] == "manualColor":
            self.rainbowColor = False
            self.randomColor = False

        self.brightness = form["brightness"]
