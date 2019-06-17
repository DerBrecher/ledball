#!/bin/bash

mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Power" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/RandomColor" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/RainbowColor" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/RandomColorSync" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/FilterColorMulti" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/RandomEffect" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter1" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter2" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter3" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter4" -m "0"

mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Color" -m "255,128,0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter1/Color" -m "0,0,0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter2/Color" -m "0,0,0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter3/Color" -m "0,0,0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Filter4/Color" -m "0,0,0"

mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/EffectNumber" -m "0"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/EffectDirection" -m "0"

mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/Brightness" -m "200"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/EffectSpeed" -m "30"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/FadeSpeed" -m "215"
mosquitto_pub -h 127.0.0.1 -r -t "ledball/command/RandomEffectPower" -m "0"