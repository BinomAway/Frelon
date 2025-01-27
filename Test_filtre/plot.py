import matplotlib.pyplot as plt
import numpy as np

# Charger les données
data = np.loadtxt("output.csv", delimiter=",")
input_signal = data[:, 0]
filtered_signal = data[:, 1]
fft_input = data[:, 2]
fft_filtered = data[:, 3]

# Définir les paramètres
SAMPLING_RATE = 8000  # Fréquence d'échantillonnage (Hz)
time = np.linspace(0, len(input_signal) / SAMPLING_RATE, len(input_signal))  # Temps en secondes
freqs = np.linspace(0, SAMPLING_RATE / 2, len(fft_input))  # Fréquence en Hz

# Afficher les signaux et FFT
plt.figure(figsize=(12, 8))

# Signal d'entrée
plt.subplot(4, 1, 1)
plt.plot(time, input_signal, label="Signal d'entrée")
plt.xlabel("Temps (s)")
plt.ylabel("Amplitude")
plt.legend()

# Signal filtré
plt.subplot(4, 1, 2)
plt.plot(time, filtered_signal, label="Signal filtré")
plt.xlabel("Temps (s)")
plt.ylabel("Amplitude")
plt.legend()

# FFT du signal d'entrée
plt.subplot(4, 1, 3)
plt.plot(freqs, fft_input, label="FFT Signal d'entrée")
plt.xlabel("Fréquence (Hz)")
plt.ylabel("Amplitude")
plt.legend()

# FFT du signal filtré
plt.subplot(4, 1, 4)
plt.plot(freqs, fft_filtered, label="FFT Signal filtré")
plt.xlabel("Fréquence (Hz)")
plt.ylabel("Amplitude")
plt.legend()

plt.tight_layout()
plt.show()

