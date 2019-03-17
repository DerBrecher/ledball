from flask import jsonify

class ledBallClass:
    state = True
    red = 255
    green = 255
    blue = 255
    hexColor = "#FFFFFF"
    brightness = 255
    fadeSpeed = 100
    effectSpeed = 1000
    effectNo = 13
    randomEffect= False
    randomColor = False
    rainbowColor = False



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

        self.brightness = form["brightness"]