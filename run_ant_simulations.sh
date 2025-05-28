#!/bin/bash

# Script pour compiler et exécuter ants.cpp avec tous les fichiers de fourmilière
# Usage: ./run_ant_simulations.sh

echo "🐜 SCRIPT D'EXÉCUTION DES SIMULATIONS DE FOURMILIÈRE 🐜"
echo "====================================================="

# Compilation du programme C++
echo "📦 Compilation d'ants.cpp..."
if g++ -o ants ants.cpp -std=c++11; then
    echo "✅ Compilation réussie"
else
    echo "❌ Erreur de compilation"
    exit 1
fi

# Fichiers de fourmilière à tester
files=("fourmiliere_zero.txt" "fourmiliere_un.txt" "fourmiliere_deux.txt" "fourmiliere_trois.txt" "fourmiliere_quatre.txt" "fourmiliere_cinq.txt")

echo
echo "🏃 Exécution des simulations..."
echo

# Boucle pour chaque fichier
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        echo "════════════════════════════════════════════════════"
        echo "📄 Test avec $file"
        echo "════════════════════════════════════════════════════"

        # Exécuter le programme avec le fichier
        ./ants "$file"

        echo
        echo "⏸️  Appuyez sur Entrée pour continuer vers le fichier suivant..."
        read -r
        echo
    else
        echo "⚠️  Fichier $file non trouvé, passage au suivant..."
    fi
done

echo "🎉 Toutes les simulations sont terminées!"
echo
echo "💡 Pour visualiser les résultats de manière interactive,"
echo "   ouvrez le fichier HTML 'visualisateur.html'"
echo "   dans votre navigateur."

# Nettoyage
echo
echo "🧹 Nettoyage..."
rm -f ants
echo "✅ Nettoyage terminé"