# Intro to IoT

## In This Repo
Starter code in `starter` directory \
Complete code for reference in `src` \

## Lets Review
What does IoT stand for? \
What does that actually mean? \
How does IoT differ from treditional physical computing? \

## IoT is a Network of Physical Computing Systems
![PhysCompSystem](assets/PhysCompSystem.png) \
![IoTSystem](assets/IoTSystem.png)

## What We're Building Today: yonolin++
We're gonna build a collaborative music making platform, where each team sends sensor data to a music-making system, which will interprite that data and convert it into music. The system might then send data back to the smart device, where it can use it to control another actuator.

You will each need an ESP8266 and a sensor, plus some wires, resistors, and maybe an LED or two.

You will also need the Arduino IDE installed (preferably version <2.0), and [Python](https://www.python.org/downloads/). You can download the Arduino IDE from the [official Arduino website](https://www.arduino.cc/en/software). 

## The ESP8266

![ESP8266](assets/ESP8266.png)

The ESP8266 is a low-cost Wi-Fi microchip with full TCP/IP stack and microcontroller capability. It is widely used in IoT projects due to its versatility and ease of use. The chip allows microcontrollers to connect to a Wi-Fi network and make simple TCP/IP connections using Hayes-style commands. With its powerful onboard processing and storage capabilities, the ESP8266 can be integrated with sensors and other application-specific devices through its GPIOs, making it an ideal choice for IoT applications.

## Setup

Sending code the the ESP is *slightly* more complicated then sending to a standard arduino, but here are the steps for setting up your IDE to work with the ESP8266:
1. Open the Arduino IDE *from the terminal*
    - getting weird already... the ESP8266 compiler uses python for some reason, and the Arduino IDE has a hard time finding python if its opened from the GUI. You'll have to find out where Arduino.app is installed, and run `open path/to/Arduino.app` in your terminal.
2. Install the ESP8266 library
    - Navigate to the IDE's settings page (ctrl / cmd + , or from the toolbar)
    - Under 'Aditional Boards Manager URLs', paste this link: https://github.com/esp8266/Arduino/releases/download/3.1.1/package_esp8266com_index.json
    - Hit 'ok'
    - Go to `Tools>Board>Board Manager`
    - Search for 'ESP8266'
    - Locate the package and hit install. *This tutorial was made with v3.1.1 (1/31/25)*
    ![espBoardPackage](assets/espBoardPackage.png)
    - You've succesfully installed the ESP8266 package!
3. Install some other packages
    - Check out the `#include` statments at the top of the provided code. Those are libraries that we're importing into our sketch, so you're gonna need to install them. To install a package:
    - Go to `Sketch>Include Library>Manage Libraries...`
    - Search for the package
    - Hit install

## Overview of the Project
Here's what we need our smart devices to be able to do:
1. Connect to the internet
2. Connect to a network where we can send and receive data
3. Generate data to send
4. Send the data
5. Look out for incomming data
6. Do something with the incoming data

## Lets Get Coding

### Connecting to the Internet
This part isn't all that hard, so all of the code is provided in the starter. You'll just have to fill in the name and password of the WiFi network you're using, and call the `setup_wifi` function.

### Connect to a network where we can send a receive data

Stop and think about this step. There are no physical wires between our devices anymore, meaning it's rather difficult to send messages to a specific device and look out for messages from specific devices... What to do... What to do...

### Protocols to the Rescue!

![protocolDiagram](assets/protocolDiagram.png)

### MQTT
The protocol we're gonna be using is MQTT

MQTT (Message Queuing Telemetry Transport) is a lightweight messaging protocol designed for small sensors and mobile devices optimized for high-latency or unreliable networks. It operates on a publish-subscribe model, which decouples the message sender (publisher) from the receiver (subscriber). 

![mqttDiagram](assets/mqttDiagram.png)

In this model, devices (clients) can publish messages to a central broker on specific topics. Other devices can subscribe to these topics to receive messages. The broker is responsible for distributing the messages to the appropriate subscribers. This model is highly efficient for IoT applications as it reduces the complexity of direct device-to-device communication and allows for scalable and flexible message distribution.

#### Connecting to the MQTT Server 

We first have to initialize out MQTT client to be able to talk to the server. We do this via
```cpp
mqtt_client.setServer(MQTT_SERVER, MQTT_SERVER_PORT);
```
which function should this be called in? (setup / loop)

Now have to establish a connection to the MQTT server. We do this by calling 
```cpp
mqtt_client.connect(mqtt_client_id.c_str(), MQTT_USER, MQTT_PASSWORD)
```
Until a connection has been established. the `mqtt_client_id` variable must be unique and configured on the MQTT server ahead of time. We will be using a [cedalo](https://platform.cedalo.cloud) server for this. Note that we would like to repeatadly try connecting until we succeed, so the complete code looks like this:
```cpp
void connect() {
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String mqtt_client_id = "1";
    if (mqtt_client.connect(mqtt_client_id.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      mqtt_client.subscribe(MQTT_TOPIC_IN);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" will try again in 5 seconds");
      delay(5000);
    }
  }
}
```
Note the call to `mqtt_client.subscribe(MQTT_TOPIC_IN);`. This subscribes our device to the chosen topic. (Make sure you know what that means!)

This function is called `connect`, but we could also call it `reconnect` because we can call it if we ever get disconnected from the server. Ie, we can call it like so:
```cpp
if (!mqtt_client.connected()) {
    connect();
}
```
which function should this be called in? (setup / loop)

#### Subscribing
Once a connection has been made, we can subscribe to a topic via
```cpp
mqtt_client.subscribe(MQTT_TOPIC_IN);
```
where `MQTT_TOPIC_IN` is the topic you want to listen for. This is done in the `connect` function.

Now that we're subscribed, we can process incoming messages like so:
```cpp
mqtt_client.loop();
time_client.update();
```

This simply performs the main loop of the mqtt client.


#### Publishing
Finally, we can publish data via
```cpp
mqtt_client.publish(MQTT_TOPIC_OUT, data_string.c_str());
```
where `MQTT_TOPIC_OUT` is the topic assigned to our data, and `data_string` is our data. Note that this data should be a string in this case. 

*a note on C++:*
Unlike other languages, you can't just throw ints into a function that expects a string and hope for the best. To convert an int to a string, you can do the following:
```
int data = analogRead(A0); // read an integer
String data_string = String(data); // convert it into a string data structure
Serial.println(data_string.c_str()); // convert the data structure into a string primitive
```

#### Callbacks
Now that we're subscribing and publishing, we can add callback functions so we can do something with the data we send / receive. 
```cpp
mqtt_client.setCallback(callback);
```
will execute the function `callback` each time a message comes in from the broker. Now if we defined the callback function like so:
```cpp
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived on topic: '");
  Serial.print(topic);
  Serial.print("' with payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
```

we will print the message topic and payload to the serial monitor. Your callback function can contain whatever you want. 

### And That's That!

Hook up your inputs and outputs, and you've got yourself an IoT system. 

## The Other End
The other smart device in our system is a python script running locally on my computer. Note how now that we're working in IoT, we're able to have systems built in different languages and frameworks communicate with each other via a shared 'language' (what do we call this shared language?)

You can check out the code in `src/app.py`