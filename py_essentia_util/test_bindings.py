# test_bindings.py
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "build"))
import py_essentia_util

if len(sys.argv) < 2:
    print("Usage: python test_bindings.py <audio.wav>")
    sys.exit(1)

filename = sys.argv[1]
key = py_essentia_util.compute_key_from_file(filename)
print("Detected key:", key)
