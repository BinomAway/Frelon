import numpy as np
from scipy.signal import iirfilter

# Paramètres du filtre
order = 2  # Ordre du filtre
low_freq = 50  # Fréquence basse (Hz)
high_freq = 300  # Fréquence haute (Hz)
sampling_rate = 1000  # Fréquence d'échantillonnage (Hz)

# Normaliser les fréquences (par rapport à la fréquence de Nyquist)
nyquist = 0.5 * sampling_rate
omega_low = 2 * np.pi * low_freq / sampling_rate
omega_high = 2 * np.pi * high_freq / sampling_rate
omega_center = (omega_low + omega_high) / 2
bandwidth = omega_high - omega_low

# **1. Calcul des coefficients par approche mathématique**
tan_bandwidth = np.tan(bandwidth / 2)
cos_center = np.cos(omega_center)
alpha = tan_bandwidth / (1 + tan_bandwidth)

b_math = [
    alpha,            # b[0]
    0,                # b[1]
    -alpha            # b[2]
]
a_math = [
    1.0,                                  # a[0]
    -2.0 * cos_center / (1 + tan_bandwidth),  # a[1]
    (1.0 - tan_bandwidth) / (1.0 + tan_bandwidth)  # a[2]
]

# **2. Calcul des coefficients avec scipy.signal.iirfilter**
low = low_freq / nyquist
high = high_freq / nyquist
b_scipy, a_scipy = iirfilter(order, [low, high], btype='band', ftype='butter')

# Afficher les résultats
print("=== Coefficients calculés manuellement ===")
print("b_math:", b_math)
print("a_math:", a_math)

print("\n=== Coefficients calculés avec scipy.signal.iirfilter ===")
print("b_scipy:", b_scipy)
print("a_scipy:", a_scipy)

