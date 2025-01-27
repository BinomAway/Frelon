#!/bin/bash

# Chemin vers le dossier kissfft
KISSFFT_PATH=~/projet_indu/Test_filtre/kissfft

# Dossier contenant les fichiers WAV
WAV_DIR="./ProjetSTM-main"

# Fichier de sortie pour les résultats
OUTPUT_FILE="results.txt"

# Vider le fichier de sortie s'il existe déjà
> $OUTPUT_FILE

# Boucle sur tous les fichiers WAV dans le dossier
for wav_file in "$WAV_DIR"/*.WAV; do
    echo "Traitement du fichier : $wav_file"

    # Exécution du script Python pour convertir le fichier WAV en buffer
    echo "Exécution du script Python pour convertir $wav_file..."
    python3 wav_buff.py "$wav_file"
    if [ $? -ne 0 ]; then
        echo "Erreur : Échec de l'exécution du script WAVtoBUFF.py pour $wav_file."
        exit 1
    fi

    # Compilation du programme C
    echo "Compilation du programme..."
    gcc -o test_filter main.c BandPassFilter.c $KISSFFT_PATH/kiss_fft.c -I$KISSFFT_PATH -lm
    if [ $? -ne 0 ]; then
        echo "Erreur : Échec de la compilation."
        exit 1
    fi

    # Exécution du programme C et redirection des résultats dans le fichier de sortie
    echo "Exécution du programme test_filter pour $wav_file..."
    echo "$wav_file:" >> $OUTPUT_FILE
    ./test_filter >> $OUTPUT_FILE
    if [ $? -ne 0 ]; then
        echo "Erreur : Échec de l'exécution du programme pour $wav_file."
        exit 1
    fi
    echo "" >> $OUTPUT_FILE  # Ajouter une ligne vide entre les résultats
done


echo "Tout s'est bien déroulé ! Les résultats sont enregistrés dans $OUTPUT_FILE."

