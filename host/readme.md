# How to use
This is the host part of a wifi enabled LED Controller and is made to run on a raspberry pi zero w.

It uses to two Docker containers:
* An Apache Webserver for the frontend
* A MQTT Server as a backend and communication interface to the ESP8266 LED controllers

You can use to supplyed systemctl service to start these containers on the bootup of the raspberry. Name the Docker containers as following when you want to use these scripts.
* Apache Webserver: "frontend"
* MQTT Server: "mqttserver"