from flask import Flask, jsonify, abort, request

app = Flask(__name__)

ledState = [
    {
     'state': True,
     'effectNo': 13,
     'testString': u'This is a Test!'
    }
]


@app.route('/')
def helloWorld():
    return "Hello World6!"

@app.route('/api/ledstate', methods=['GET'])
def returnLedState():
    return jsonify({'ledState': ledState})

@app.route('/api/newdevice', methods=['POST'])
def addNewDevice():
    if not request.json or not 'mac' in request.json:
        print("No json request!")
        abort(400)
    print(request.json['mac'])
    return jsonify({'State':"Okay"}),200


if __name__ == '__main__':
    app.run(debug = True)
