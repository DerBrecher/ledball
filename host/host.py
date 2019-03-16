from flask import Flask, jsonify, abort, request, render_template, redirect, url_for
import diytools

app = Flask(__name__)

ledState = {
    "state":True,
    "color":{
      "red":255,
      "green":255,
      "blue":255
    },
    "brightness":255,
    "fadeSpeed":100,
    "effectSpeed":1000,
    "effectNo":13,
    "randomEffect":False,
    "randomColor":False,
    "rainbow":True
  }

@app.route('/')
def index():
    global ledState
    return render_template("index.html", currentColor = diytools.jsontohex(ledState["color"]))

@app.route("/updateState", methods=["POST"])
def updateState():
    global ledState
    if request.method == "POST":
        ledState["color"] = diytools.hextojson(request.form["ledColor"])
        print(ledState)
        return redirect(url_for("index"))

@app.route('/api/ledstate', methods=['GET'])
def returnLedState():
    global ledState
    return jsonify(ledState)

@app.route('/colortest')
def colorTest():
    global ledState
    return render_template("colorTest.html", currentColor = diytools.jsontohex(ledState["color"]))

if __name__ == '__main__':
    app.run(debug = True, host= "0.0.0.0")


#
# @app.route('/api/newdevice', methods=['POST'])
# def addNewDevice():
#     if not request.json or not 'mac' in request.json:
#         print("No or invalide json request!")
#         abort(400)
#     print(request.json['mac'])
#     return jsonify({'State':"Okay"}),200
#
# @app.route('/api/heartbeat', methods=['POST'])
# def deviceHeartbeat():
#     if not request.json or not 'mac' in request.json:
#         print("No or invalide json request!")
#         abort(400)
#     print(request.json)
#     return jsonify({'State':"Okay"}),200
1
