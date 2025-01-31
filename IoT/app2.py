import paho.mqtt.client as mqtt
from paho.mqtt.properties import Properties
from paho.mqtt.packettypes import PacketTypes
from datetime import datetime as dt
from paho.mqtt.client import connack_string as ack
import ssl

# Configuration 
version = '5' # or '3'
mytransport = 'websockets' # or 'tcp'

ID = '0f261a58-e137-4fc6-8a89-7d9c2881f7e0'

broker = 'pf-4pbo68mmklw1arbgld3b.cedalo.cloud' # eg. choosen-name-xxxx.cedalo.cloud
myport = 443

sub_topic = 'data'
pub_topic = 'command'

client = None
if version == '5':
    client = mqtt.Client(client_id="home",
                         transport=mytransport,
                         protocol=mqtt.MQTTv5)
if version == '3':
    client = mqtt.Client(client_id="home",
                         transport=mytransport,
                         protocol=mqtt.MQTTv311,
                         clean_session=True)
    
client.username_pw_set("home", "pass")
client.tls_set(certfile=None,
               keyfile=None,
               # If these arguments above are not None then they will
               # be used as client information for TLS based
               # authentication and authorization (depends on broker setup).
               cert_reqs=ssl.CERT_REQUIRED)
               # this makes it mandatory that the broker
               # has a valid certificate

# Callbacks
def on_connect(client, userdata, flags, rc, v5config=None):
    print(dt.now().strftime("%H:%M:%S.%f")[:-2] + " Connection returned result: "+ack(rc))

def on_message(client, userdata, message,tmp=None):
    print(int(message.payload))
    # print(dt.now().strftime("%H:%M:%S.%f")[:-2] + " Received message " + str(message.payload) + " on topic '"
    #     + message.topic + "' with QoS " + str(message.qos))

def on_publish(client, userdata, mid,tmp=None):
    print(dt.now().strftime("%H:%M:%S.%f")[:-2] + " Published message id: "+str(mid))
    
def on_subscribe(client, userdata, mid, qos,tmp=None):
    if isinstance(qos, list):
        qos_msg = str(qos[0])
    else:
        qos_msg = f"and granted QoS {qos[0]}"
    print(dt.now().strftime("%H:%M:%S.%f")[:-2] + " Subscribed " + qos_msg)

client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish
client.on_subscribe = on_subscribe

if version == '5':
    properties=Properties(PacketTypes.CONNECT)
    properties.SessionExpiryInterval=30*60 # in seconds
    client.connect(broker,
                   port=myport,
                   clean_start=mqtt.MQTT_CLEAN_START_FIRST_ONLY,
                   properties=properties,
                   keepalive=60)

if version == '3':
    client.connect(broker, port=myport, keepalive=60)

client.loop_start()

client.subscribe(sub_topic, 2)

properties=Properties(PacketTypes.PUBLISH)
properties.MessageExpiryInterval = 30 # in seconds

try:
    while True:
        client.publish(pub_topic, 'some command', 2, properties=properties)
except KeyboardInterrupt:
    client.unsubscribe(sub_topic)
    client.disconnect()
    print("Disconnected. Goodbye!")
