import numpy as np
import matplotlib.pyplot as plt
import argparse
import librosa
import scipy
import subprocess
import os
import soundfile as sf

def separate_harmonic(input_file):
    """Return path to harmonic component of audio"""
    y, sr = librosa.load(input_file, sr=None)
    y_harmonic, _ = librosa.effects.hpss(y)

    base_name = os.path.splitext(os.path.basename(input_file))[0]
    harmonic_path = f"output/{base_name}_harmonic.wav"
    sf.write(harmonic_path, y_harmonic, sr)
    print(f"Harmonic component saved to: {harmonic_path}")
    return harmonic_path

# Parse command line arguments
parser = argparse.ArgumentParser(description="Computes chromagram/harmonic pitch class profile")
parser.add_argument("filename", type=str, help="Path to the .wav file")
parser.add_argument("--enhanced", action="store_true", help="Enhance chromagram")
parser.add_argument("--hpss", action="store_true", help="Use librosa's harmonic-percussive source separation to preprocess audio")
args = parser.parse_args()

# Apply HPSS if requested
input_wav = args.filename
if args.hpss:
    os.makedirs("output", exist_ok=True)
    input_wav = separate_harmonic(args.filename)

# Run compiled HPCP extractor3
os.makedirs("output", exist_ok=True)
subprocess.run(["./build/test", input_wav], check=True)

# Load CSV
chroma = np.loadtxt("output/hpcp.csv", delimiter=",")

# Apply enhancement if requested
if args.enhanced:
    chroma /= np.max(chroma) # normalize
    chroma = np.log(1000*chroma + 1.0) # convert to log scale

    global_threshold = 0.2 * np.log(1000*1.0 + 1.0)
    chroma[chroma < global_threshold] = 0 # drop all entries below global threshold

    col_sums = np.max(chroma, axis=0)
    column_threshold = 0.2 * np.log(1000*1.0 + 1.0)
    col_sums[col_sums < column_threshold] = np.inf
    chroma /= col_sums # normalize columns or set to zero below threshold
    
    chroma_filter = np.minimum(chroma, librosa.decompose.nn_filter(chroma, aggregate=np.median, metric='cosine'))
    chroma = scipy.ndimage.median_filter(chroma_filter, size=(1, 9)) # smooth out using filter
    

# Plot
plt.figure(figsize=(10, 6))
plt.imshow(chroma, aspect="auto", origin="lower", cmap="magma")
plt.xlabel("Frame")
plt.ylabel("Pitch Class (C to B)")
plt.title(f"Essentia HPCP for {os.path.basename(input_wav)}")
plt.colorbar(label="Energy")
plt.yticks(ticks=np.arange(12), labels=["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"])
plt.tight_layout()
plt.savefig("output/hpcp.png")
print("Saved visualization to output/essentia_hpcp.png")
