import json
import time
import paho.mqtt.client as mqtt

ID = '0f261a58-e137-4fc6-8a89-7d9c2881f7e0'

CLIENT_DATA_TOPIC = ID + '/data'
CLIENT_NAME = ID + 'team_name'
SERVER_COMMAND_TOPIC = ID + '/commands'

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(CLIENT_DATA_TOPIC)
        print(f"Subscribed to topic: {CLIENT_DATA_TOPIC}")
    else:
        print(f"Failed to connect, return code {rc}")

def handle_telemetry(client, userdata, message):
    # parse the message
    payload = json.loads(message.payload.decode())
    print("Message received:", payload)

    # send command to the server
    command = { 'led_on' : payload['data'] < 300 }
    print("Sending message:", command)

    client.publish(SERVER_COMMAND_TOPIC, json.dumps(command))

mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, CLIENT_NAME)
mqtt_client.on_connect = on_connect
mqtt_client.on_message = handle_telemetry

print("Connecting to MQTT Broker...")
mqtt_client.connect('test.mosquitto.org', 1883, 60)
mqtt_client.loop_start()

while True:
    # print('Waiting for messages...')
    # send command to the server
    command = { 'led_on' : True }
    print("Sending message:", command)

    mqtt_client.publish(SERVER_COMMAND_TOPIC, json.dumps(command))

    time.sleep(2)