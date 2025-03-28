from pydub import AudioSegment
from pydub.playback import play

MAX_QUEUE_LENGTH = 10
THRESHOLD = 50  # Change threshold for playing a note

# Queues for each client
client_queues = {}

# Pentatonic scale notes (C major pentatonic)
notes = [261.63, 293.66, 329.63, 392.00, 440.00, 523.25, 587.33, 659.25, 783.99, 880.00, 1046.50]

