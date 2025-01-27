import matplotlib.pyplot as plt

def read_results(file_path):
    """
    Lit les résultats d'énergie depuis un fichier et retourne un dictionnaire.

    Parameters:
        file_path (str): Chemin vers le fichier contenant les résultats.

    Returns:
        dict: Un dictionnaire avec les noms des fichiers comme clés et les énergies comme valeurs.
    """
    results = {}
    with open(file_path, 'r') as file:
        lines = file.readlines()
        i = 0
        while i < len(lines):
            # Vérifie si la ligne contient un nom de fichier
            if lines[i].strip().endswith(":"):
                name = lines[i].strip().replace('./ProjetSTM-main/', '').replace(':', '')

                # Récupère la ligne suivante pour l'énergie
                if i + 1 < len(lines) and "Énergie en Bande" in lines[i + 1]:
                    energy_line = lines[i + 1].strip()
                    try:
                        energy = float(energy_line.split(': ')[-1])  # Extraire la valeur
                        results[name] = energy
                    except ValueError:
                        print(f"Erreur de conversion pour {name}.")
                i += 1
            i += 1
    return results

def plot_bar_chart(results):
    """
    Trace un diagramme au bâtonnets des énergies pour chaque fichier.

    Parameters:
        results (dict): Un dictionnaire contenant les noms des fichiers et leurs énergies.
    """
    files = list(results.keys())
    energies = list(results.values())

    plt.figure(figsize=(16, 10))
    plt.bar(files, energies, color='skyblue')
    plt.xlabel('Fichiers WAV')
    plt.ylabel('Énergie (110–140 Hz)')
    plt.title('Énergie pour chaque fichier WAV')
    plt.xticks(rotation=90, fontsize=8, ha='right')  # Rotation des étiquettes pour une meilleure lisibilité
    plt.tight_layout()

    # Enregistrer le graphique si trop de fichiers à afficher à l'écran
    if len(files) > 20:
        plt.savefig("energy_bar_chart.png")
        print("Le graphique a été sauvegardé dans 'energy_bar_chart.png'")
    else:
        plt.show()

# Exemple d'utilisation
if __name__ == "__main__":
    results_file = "results.txt"  # Chemin vers le fichier des résultats
    results = read_results(results_file)
    
    if not results:
        print("Aucun résultat trouvé dans le fichier.")
    else:
        plot_bar_chart(results)

