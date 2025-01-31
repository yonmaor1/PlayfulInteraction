import paho.mqtt.client as mqtt
from paho.mqtt.properties import Properties
from paho.mqtt.packettypes import PacketTypes
from datetime import datetime as dt
from paho.mqtt.client import connack_string as ack
import ssl
import json
import numpy as np
import random

# music related imports
from pydub import AudioSegment
from pydub.playback import play
import queue
import io
import sys
import concurrent.futures

# music configs
MAX_QUEUE_LENGTH = 10
THRESHOLD = 50  # Change threshold for playing a note

# queues for each client
client_queues = {}

# Pentatonic scale notes (C major pentatonic)
notes = [261.63, 293.66, 329.63, 392.00, 440.00, 523.25, 587.33, 659.25, 783.99, 880.00, 1046.50]

def generate_sine_wave(frequency, duration, sample_rate=44100):
    t = np.linspace(0, duration / 1000, int(sample_rate * duration / 1000), False)
    sine_wave = 0.5 * np.sin(2 * np.pi * frequency * t)
    sine_wave = (sine_wave * 32767).astype(np.int16)  # Convert to 16-bit PCM format
    audio_segment = AudioSegment(
        sine_wave.tobytes(),
        frame_rate=sample_rate,
        sample_width=2,  # 16-bit PCM
        channels=1
    )
    return audio_segment

def play_note_from_value(value):

    # Redirect stdout to suppress pydub playback output
    old_stdout = sys.stdout
    sys.stdout = io.StringIO()

    # Map value to a note in the pentatonic scale
    note_index = int((value / 1024) * (len(notes) - 1))
    frequency = notes[note_index]
    duration = 200  # milliseconds
    sine_wave = generate_sine_wave(frequency, duration)

    with concurrent.futures.ThreadPoolExecutor() as executor:
        pass
        # executor.submit(play, sine_wave)

    # Reset stdout to its original state
    sys.stdout = old_stdout

# End Music Setup

# General Configuration 
version = '5' # or '3'
mytransport = 'websockets' # or 'tcp'


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

def on_message(client, userdata, message, tmp=None):
    # print(message)
    # print("Message received on topic:", message.topic)
    # print("Raw message payload:", message.payload)

    # Parse the message
    payload = json.loads(message.payload.decode())
    client_id = client.username

    if client_id not in client_queues:
        client_queues[client_id] = queue.Queue(maxsize=MAX_QUEUE_LENGTH)

    q = client_queues[client_id]

    if q.full():
        q.get()

    q.put(payload)

    # Calculate the average value in the queue
    avg_value = sum(list(q.queue)) / q.qsize()

    print(list(q.queue))

    # Check if the average value changes significantly
    if abs(avg_value - payload) > THRESHOLD:
        print(avg_value)
        play_note_from_value(avg_value)

def on_publish(client, userdata, mid,tmp=None):
    pass
    # print(dt.now().strftime("%H:%M:%S.%f")[:-2] + " Published message id: "+str(mid))
    
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
        client.publish(pub_topic, random.random() < 0.5, 2, properties=properties)
except KeyboardInterrupt:
    client.unsubscribe(sub_topic)
    client.disconnect()
    print("Disconnected. Goodbye!")
