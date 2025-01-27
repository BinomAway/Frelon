#!/bin/bash

# Chemin vers le dossier kissfft
KISSFFT_PATH=~/projet_indu/Test_filtre/kissfft

# Exécution du script Python pour convertir WAV en buffer
echo "Exécution du script Python pour convertir le fichier WAV..."
python3 WAVtoBUFF.py
if [ $? -ne 0 ]; then
    echo "Erreur : Échec de l'exécution du script WAVtoBUFF.py."
    exit 1
fi

# Compilation du programme C
echo "Compilation du programme..."
gcc -o test_filter main.c BandPassFilter.c $KISSFFT_PATH/kiss_fft.c -I$KISSFFT_PATH -lm
if [ $? -ne 0 ]; then
    echo "Erreur : Échec de la compilation."
    exit 1
fi

# Exécution du programme C
echo "Exécution du programme test_filter..."
./test_filter
if [ $? -ne 0 ]; then
    echo "Erreur : Échec de l'exécution du programme."
    exit 1
fi

# Exécution du script Python pour générer les graphiques
echo "Exécution du script Python pour les graphiques..."
python3 plot.py
if [ $? -ne 0 ]; then
    echo "Erreur : Échec de l'exécution du script plot.py."
    exit 1
fi



echo "Tout s'est bien déroulé !"
