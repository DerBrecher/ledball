from flask import Flask, jsonify, abort, request, render_template, redirect, url_for
from app.ledball import ledBallClass

app = Flask(__name__)

ledBall = ledBallClass()

@app.route('/')
def index():
    global ledBall
    return render_template("index.html", currentBallState = ledBall)

@app.route("/updateState", methods=["POST"])
def updateState():
    global ledBall
    if request.method == "POST":
        ledBall.handleHtmlForm(request.form)
        return redirect(url_for("index"))

@app.route('/api/ledstate', methods=['GET'])
def returnLedState():
    global ledBall
    return jsonify(ledState)

@app.route('/colortest')
def colorTest():
    global ledBall
    return render_template("colorTest.html", currentBallState = ledBall)

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
