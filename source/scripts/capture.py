# Import required modules
from picamera2 import Picamera2  # A Python interface to the Pi Camera Module
from libcamera import (
    controls,
)  # The controls module of libcamera provides support for manipulating camera parameters
import time  # Provides various time-related functions
import os  # Provides functions for interacting with the operating system

# Create an instance of the Picamera2
picam2 = Picamera2()

picam2.options[
    "compress_level"
] = 9  # PNG compression level, where 0 gives no compression, 1 is the fastest that actually does any compression, and 9 is the slowest.

# Set camera controls for automatic white balance and automatic exposure to True
picam2.set_controls({"AwbEnable": True})
picam2.set_controls({"AeEnable": True})

# Set the Noise Reduction Mode to High Quality
picam2.set_controls(
    {"NoiseReductionMode": controls.draft.NoiseReductionModeEnum.HighQuality}
)

# Start the camera
picam2.start()

# Check if a directory named 'images' exists, if not, create it
if not os.path.exists("/home/pi/images"):
    os.makedirs("/home/pi/images")

# Create a timestamp for uniquely identifying the photos captured
timestamp = time.strftime("%Y%m%d-%H%M%S")

# Capture the photo and save it with a unique name (timestamp)
picam2.capture_file(f"/home/pi/images/{timestamp}.png")
