var mqtt;
var reconnectTimeout = 2000;
var host = ""; //gets updated to window.location.hostname on MQTT Connect
var port = 9001;

//MQTT Commands Topics
var powerStateTopic = "ledball/command/Power"; //Toggle
var randomColorTopic = "ledball/command/RandomColor"; //Toggle
var rainbowColorTopic = "ledball/command/RainbowColor"; //Toggle
var randomColorSyncTopic = "ledball/command/RandomColorSync"; //Toggle
var filterColorMultiTopic = "ledball/command/FilterColorMulti"; //Toggle
var randomEffectTopic = "ledball/command/RandomEffect"; //Toggle
var filter1Topic = "ledball/command/Filter1"; //Toggle
var filter2Topic = "ledball/command/Filter2"; //Toggle
var filter3Topic = "ledball/command/Filter3"; //Toggle
var filter4Topic = "ledball/command/Filter4"; //Toggle

var filter1ColorTopic = "ledball/command/Filter1/Color"; //ColorPicker
var filter2ColorTopic = "ledball/command/Filter2/Color"; //ColorPicker
var filter3ColorTopic = "ledball/command/Filter3/Color"; //ColorPicker
var filter4ColorTopic = "ledball/command/Filter4/Color"; //ColorPicker
var colorTopic = "ledball/command/Color"; //ColorPicker

var effectNumberTopic = "ledball/command/EffectNumber"; //List
var effectDirectionTopic = "ledball/command/EffectDirection"; //List

var brightnessTopic = "ledball/command/Brightness"; //Slider
var effectSpeedTopic = "ledball/command/EffectSpeed"; //Slider
var fadeSpeedTopic = "ledball/command/FadeSpeed"; //Slider
var randomEffectPowerTopic = "ledball/command/RandomEffectPower"; //Slider

//MQTT State Topics
var powerStateStateTopic = "ledball/state/Power"; //Toggle
var randomColorStateTopic = "ledball/state/RandomColor"; //Toggle
var rainbowColorStateTopic = "ledball/state/RainbowColor"; //Toggle
var randomColorSyncStateTopic = "ledball/state/RandomColorSync"; //Toggle
var filterColorMultiStateTopic = "ledball/state/FilterColorMulti"; //Toggle
var randomEffectStateTopic = "ledball/state/RandomEffect"; //Toggle
var filter1StateTopic = "ledball/state/Filter1"; //Toggle
var filter2StateTopic = "ledball/state/Filter2"; //Toggle
var filter3StateTopic = "ledball/state/Filter3"; //Toggle
var filter4StateTopic = "ledball/state/Filter4"; //Toggle

var filter1ColorStateTopic = "ledball/state/Filter1/Color"; //ColorPicker
var filter2ColorStateTopic = "ledball/state/Filter2/Color"; //ColorPicker
var filter3ColorStateTopic = "ledball/state/Filter3/Color"; //ColorPicker
var filter4ColorStateTopic = "ledball/state/Filter4/Color"; //ColorPicker
var colorStateTopic = "ledball/state/Color"; //ColorPicker

var effectNumberStateTopic = "ledball/state/EffectNumber"; //List
var effectDirectionStateTopic = "ledball/state/EffectDirection"; //List

var brightnessStateTopic = "ledball/state/Brightness"; //Slider
var effectSpeedStateTopic = "ledball/state/EffectSpeed"; //Slider
var fadeSpeedStateTopic = "ledball/state/FadeSpeed"; //Slider
var randomEffectPowerStateTopic = "ledball/state/RandomEffectPower"; //Slider


//------ Test Functions ------
function printTest() {
    console.log("This is a Test!")
}

function testPublish() {
    message = new Paho.MQTT.Message("This is a Test from JS");
    message.destinationName = "foo/bar";
    //console.log(document.getElementById("MainColorPicker").jscolor.toRGBString())
    //console.log(document.getElementById("MainColorPicker").jscolor.fromString("00FF00"))
    mqtt.send(message);
    console.log(message);
}

//------ Functions to update MQTT on HTML Update ------
function handleToggle(oswitch, topic) {
    if (oswitch.checked) {
        message = new Paho.MQTT.Message("1");
    } else {
        message = new Paho.MQTT.Message("0");
    }
    message.retained = true;
    message.destinationName = topic;
    mqtt.send(message);
}

function handleList(oselect, topic) {
    var selectedOption = oselect.options[oselect.selectedIndex].value;
    message = new Paho.MQTT.Message(selectedOption);
    message.retained = true;
    message.destinationName = topic;
    mqtt.send(message);
}

function handleSlider(effectSpeed, topic, htmlID) {
    var value = effectSpeed.value;
    document.getElementById(htmlID).innerHTML = value;
    message = new Paho.MQTT.Message(value);
    message.retained = true;
    message.destinationName = topic;
    mqtt.send(message);
}

function handleColorPicker(colorPicker, topic) {
    rgbMessage =
        parseInt(colorPicker.jscolor.rgb[0]) + "," +
        parseInt(colorPicker.jscolor.rgb[1]) + "," +
        parseInt(colorPicker.jscolor.rgb[2]);
    message = new Paho.MQTT.Message(rgbMessage);
    message.retained = true;
    message.destinationName = topic;
    mqtt.send(message);
}

//------ Functions to update HTML on MQTT Message ------
function updateToggle(message, htmlID) {
    if (message.payloadString == "1") {
        document.getElementById(htmlID).checked = true;
    }
    else {
        document.getElementById(htmlID).checked = false;
    }
}

function updateList(message, htmlID) {
    document.getElementById(htmlID).selectedIndex = parseInt(message.payloadString);
}

function updateDirectionList(message, htmlID) {
    var list = document.getElementById(htmlID);
    switch (parseInt(message.payloadString)) {
        case 8: //Up
            list.selectedIndex = 0;
            break;
        case 2: //Down
            list.selectedIndex = 1;
            break;
        case 6: //Right
            list.selectedIndex = 2;
            break;
        case 4: //Left
            list.selectedIndex = 3;
            break;
    }
}

function updateSlider(message, htmlID, displayHtmlID) {
    document.getElementById(htmlID).value = message.payloadString;
    document.getElementById(displayHtmlID).innerHTML = parseInt(message.payloadString);
}

function updateColorPicker(message, htmlID) {
    fields = message.payloadString.split(",");
    r = parseInt(fields[0]);
    g = parseInt(fields[1]);
    b = parseInt(fields[2]);
    document.getElementById(htmlID).jscolor.fromRGB(r, g, b);
}

//------ MQTT Functions ------
function onConnect() {
    console.log("MQTT Connected ");
    document.getElementById("status").innerHTML = "LED Ball Controller: Connected";
    document.getElementById("status").style.backgroundColor = 'green';

    //Subscribe to Mqtt Topics
    //Toggles
    mqtt.subscribe(powerStateStateTopic);
    mqtt.subscribe(randomColorStateTopic);
    mqtt.subscribe(rainbowColorStateTopic);
    mqtt.subscribe(randomColorSyncStateTopic);
    mqtt.subscribe(filterColorMultiStateTopic);
    mqtt.subscribe(randomEffectStateTopic);
    mqtt.subscribe(filter1StateTopic);
    mqtt.subscribe(filter2StateTopic);
    mqtt.subscribe(filter3StateTopic);
    mqtt.subscribe(filter4StateTopic);
    //Lists
    mqtt.subscribe(effectNumberStateTopic);
    mqtt.subscribe(effectDirectionStateTopic);
    //Sliders
    mqtt.subscribe(effectSpeedStateTopic);
    mqtt.subscribe(brightnessStateTopic);
    mqtt.subscribe(fadeSpeedStateTopic);
    mqtt.subscribe(randomEffectPowerStateTopic);
    //ColorPickers
    mqtt.subscribe(colorStateTopic);
    mqtt.subscribe(filter1ColorStateTopic);
    mqtt.subscribe(filter2ColorStateTopic);
    mqtt.subscribe(filter3ColorStateTopic);
    mqtt.subscribe(filter4ColorStateTopic);
}

function onFailure(message) {
    console.log("MQTT Connection Attempt to Host " + host + " Failed");
    setTimeout(MQTTconnect, 2000);
}

function onConnectionLost() {
    console.log("MQTT disconnected");
    document.getElementById("status").innerHTML = "LED Ball Controller: Disconnected";
    document.getElementById("status").style.backgroundColor = 'red';
    setTimeout(MQTTconnect, 2000);
}

function onMessageArrived(message) {
    // outMessage = "MQTT Message received: " + message.payloadString + "<br>";
    // outMessage = outMessage + "Message received Topic: " + message.destinationName;
    // console.log(outMessage);
    console.log("New MQTT Message!")

    switch (message.destinationName) {
        //Toggles
        case powerStateStateTopic:
            updateToggle(message, "powerSwitch");
            break;
        case randomColorStateTopic:
            updateToggle(message, "randomColor");
            break;
        case rainbowColorStateTopic:
            updateToggle(message, "rainbowActive");
            break;
        case randomColorSyncStateTopic:
            updateToggle(message, "randomColorSync");
            break;
        case filterColorMultiStateTopic:
            updateToggle(message, "filterColorMulti");
            break;
        case randomEffectStateTopic:
            updateToggle(message, "randomEffect");
            break;
        case filter1StateTopic:
            updateToggle(message, "filter1");
            break;
        case filter2StateTopic:
            updateToggle(message, "filter2");
            break;
        case filter3StateTopic:
            updateToggle(message, "filter3");
            break;
        case filter4StateTopic:
            updateToggle(message, "filter4");
            break;

        //Lists
        case effectNumberStateTopic:
            updateList(message, "effectList");
            break;
        case effectDirectionStateTopic:
            updateDirectionList(message, "effectDirection");
            break;

        //Sliders
        case effectSpeedStateTopic:
            updateSlider(message, "effectSpeed", "effectSpeedDisplay");
            break;
        case brightnessStateTopic:
            updateSlider(message, "brightness", "brightnessDisplay");
            break;
        case fadeSpeedStateTopic:
            updateSlider(message, "fadeSpeed", "fadeSpeedDisplay");
            break;
        case randomEffectPowerStateTopic:
            updateSlider(message, "randomEffectPower", "randomEffectPowerDisplay");
            break;

        //ColorPickers
        case colorStateTopic:
            updateColorPicker(message, "colorPicker");
            break;
        case filter1ColorStateTopic:
            updateColorPicker(message, "filter1Color");
            break;
        case filter2ColorStateTopic:
            updateColorPicker(message, "filter2Color");
            break;
        case filter3ColorStateTopic:
            updateColorPicker(message, "filter3Color");
            break;
        case filter4ColorStateTopic:
            updateColorPicker(message, "filter4Color");
            break;
    }
}

function MQTTconnect() {
    host = window.location.hostname;
    console.log("MQTT connecting to " + host + " " + port);
    mqtt = new Paho.MQTT.Client(host, port, "web_" + parseInt(Math.random() * 100, 10));
    //document.write("connecting to "+ host);
    var options = {
        timeout: 3,
        onSuccess: onConnect,
        onFailure: onFailure,
    };

    mqtt.onConnectionLost = onConnectionLost;
    mqtt.onMessageArrived = onMessageArrived;
    //mqtt.onConnected = onConnected;

    mqtt.connect(options); //connect
    return false;
}

//------ Init Functions ------
(function (window, document, undefined) {
    window.onload = init;

})(window, document, undefined);

function init() {
    //Toggle
    var powerSwitch = document.getElementById("powerSwitch");
    powerSwitch.onchange = function () { handleToggle(powerSwitch, powerStateTopic); };

    var randomColor = document.getElementById("randomColor");
    randomColor.onchange = function () { handleToggle(randomColor, randomColorTopic); };

    var rainbowActive = document.getElementById("rainbowActive");
    rainbowActive.onchange = function () { handleToggle(rainbowActive, rainbowColorTopic); };

    var randomColorSync = document.getElementById("randomColorSync");
    randomColorSync.onchange = function () { handleToggle(randomColorSync, randomColorSyncTopic); };

    var filterColorMulti = document.getElementById("filterColorMulti");
    filterColorMulti.onchange = function () { handleToggle(filterColorMulti, filterColorMultiTopic); };

    var randomEffect = document.getElementById("randomEffect");
    randomEffect.onchange = function () { handleToggle(randomEffect, randomEffectTopic); };

    var filter1 = document.getElementById("filter1");
    filter1.onchange = function () { handleToggle(filter1, filter1Topic); };

    var filter2 = document.getElementById("filter2");
    filter2.onchange = function () { handleToggle(filter2, filter2Topic); };

    var filter3 = document.getElementById("filter3");
    filter3.onchange = function () { handleToggle(filter3, filter3Topic); };

    var filter4 = document.getElementById("filter4");
    filter4.onchange = function () { handleToggle(filter4, filter4Topic); };

    //Lists
    var effectList = document.getElementById("effectList");
    effectList.onchange = function () { handleList(effectList, effectNumberTopic) };

    var effectDirection = document.getElementById("effectDirection");
    effectDirection.onchange = function () { handleList(effectDirection, effectDirectionTopic) };

    //Sliders
    var effectSpeed = document.getElementById("effectSpeed");
    effectSpeed.onchange = function () { handleSlider(effectSpeed, effectSpeedTopic, "effectSpeedDisplay") };

    var brightness = document.getElementById("brightness");
    brightness.onchange = function () { handleSlider(brightness, brightnessTopic, "brightnessDisplay") };

    var fadeSpeed = document.getElementById("fadeSpeed");
    fadeSpeed.onchange = function () { handleSlider(fadeSpeed, fadeSpeedTopic, "fadeSpeedDisplay") };

    var randomEffectPower = document.getElementById("randomEffectPower");
    randomEffectPower.onchange = function () { handleSlider(randomEffectPower, randomEffectPowerTopic, "randomEffectPowerDisplay") };

    //Color Pickers
    var colorPicker = document.getElementById("colorPicker");
    colorPicker.onchange = function () { handleColorPicker(colorPicker, colorTopic) };

    var filter1Color = document.getElementById("filter1Color");
    filter1Color.onchange = function () { handleColorPicker(filter1Color, filter1ColorTopic) };

    var filter2Color = document.getElementById("filter2Color");
    filter2Color.onchange = function () { handleColorPicker(filter2Color, filter2ColorTopic) };

    var filter3Color = document.getElementById("filter3Color");
    filter3Color.onchange = function () { handleColorPicker(filter3Color, filter3ColorTopic) };

    var filter4Color = document.getElementById("filter4Color");
    filter4Color.onchange = function () { handleColorPicker(filter4Color, filter4ColorTopic) };
}
