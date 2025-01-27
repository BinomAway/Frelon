import wave
import numpy as np
import sys

def process_wav_file_to_c_buffer(wav_file, target_sampling_rate):
    """
    Lit un fichier WAV, l'échantillonne à la fréquence désirée, et retourne un buffer exploitable en C.
    
    Parameters:
        wav_file (str): Chemin vers le fichier WAV.
        target_sampling_rate (int): Fréquence d'échantillonnage cible (en Hz).

    Returns:
        np.ndarray: Signal échantillonné prêt à être utilisé comme buffer.
    """
    # Ouvrir le fichier WAV
    with wave.open(wav_file, 'rb') as wav:
        n_channels = wav.getnchannels()
        sampwidth = wav.getsampwidth()
        framerate = wav.getframerate()
        n_frames = wav.getnframes()

        # Lire les frames du fichier WAV
        raw_audio = wav.readframes(n_frames)

    # Convertir les données audio en tableau numpy
    dtype = np.int16 if sampwidth == 2 else np.uint8
    audio = np.frombuffer(raw_audio, dtype=dtype)

    # Si le fichier est stéréo, convertir en mono
    if n_channels > 1:
        audio = audio[::n_channels]  # Prendre un seul canal

    # Normaliser le signal entre -1 et 1
    audio = audio / np.max(np.abs(audio))

    # Si la fréquence d'échantillonnage d'origine est différente de la cible
    if framerate != target_sampling_rate:
        from scipy.signal import resample
        audio = resample(audio, int(len(audio) * target_sampling_rate / framerate))

    # Convertir le signal en float pour être compatible avec le code C
    buffer = (audio * 32767).astype(np.int16)

    return buffer

def save_c_buffer_to_file(buffer, output_file):
    """
    Sauvegarde un buffer en format texte pour l'utiliser dans le code C.
    
    Parameters:
        buffer (np.ndarray): Tableau des échantillons du signal.
        output_file (str): Chemin vers le fichier texte de sortie.
    """
    with open(output_file, 'w') as f:
        for value in buffer:
            f.write(f"{value},\n")

# Exemple d'utilisation
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 WAVtoBUFF.py <fichier_WAV>")
        sys.exit(1)

    wav_file = sys.argv[1]  # Premier argument : le fichier WAV
    target_sampling_rate = 8000  # Fréquence d'échantillonnage cible (en Hz)
    output_file = "buffer.txt"  # Fichier texte pour stocker le buffer

    buffer = process_wav_file_to_c_buffer(wav_file, target_sampling_rate)
    save_c_buffer_to_file(buffer, output_file)

    print(f"Buffer prêt et sauvegardé dans {output_file}")
