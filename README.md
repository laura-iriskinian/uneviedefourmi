# 🐜 Visualisateur de Fourmilière

## 📋 Liste des fichiers nécessaires

Assure-toi d'avoir tous ces fichiers dans le même dossier :

```
📦 Dossier fourmiliere/
├── 🔧 ants.cpp                    # Programme principal C++
├── 🌐 visualisateur.html          # Interface de visualisation
├── 🚀 run_ant_simulations.sh      # Script d'exécution automatique
├── 📊 fourmiliere_zero.txt        # 2 fourmis, 2 salles
├── 📊 fourmiliere_un.txt          # 5 fourmis, 2 salles (séquentiel)
├── 📊 fourmiliere_deux.txt        # 5 fourmis, 2 salles (raccourci direct)
├── 📊 fourmiliere_trois.txt       # 5 fourmis, 4 salles
├── 📊 fourmiliere_quatre.txt      # 10 fourmis, 6 salles
├── 📊 fourmiliere_cinq.txt        # 50 fourmis, 14 salles
└── 📖 README.md                   # Ce guide
```

## 🚀 Utilisation rapide

### 1. Script automatique (recommandé)
```bash
chmod +x run_ant_simulations.sh
./run_ant_simulations.sh
```

### 2. Interface de visualisation
```bash
# Ouvrir dans le navigateur
open visualisateur.html
# ou double-cliquer sur le fichier HTML
```

### 3. Compilation manuelle
```bash
g++ -o ants ants.cpp -std=c++11
./ants fourmiliere_zero.txt
```

## 🎮 Comment utiliser l'interface

1. **Ouvrir** `visualisateur.html` dans un navigateur
2. **Sélectionner** une fourmilière dans le menu déroulant
3. **Cliquer** sur "Charger"
4. **Naviguer** avec les boutons :
    - `◀ Précédent` : Étape précédente
    - `Suivant ▶` : Étape suivante
    - `▶ Auto` : Lecture automatique
    - `⏸ Stop` : Arrêter l'auto
    - `Reset` : Retour au début

## 📊 Description des fourmilières

| Nom | Fourmis | Étapes | Particularité |
|-----|---------|--------|---------------|
| **Zero** | 2 | 2 | Chemins parallèles optimaux |
| **Un** | 5 | 7 | Goulot d'étranglement séquentiel |
| **Deux** | 5 | 5 | Raccourci direct Sv→Sd |
| **Trois** | 5 | 7 | Chemin optimal trouvé |
| **Quatre** | 10 | 9 | Capacités multiples |
| **Cinq** | 50 | ~25 | Réseau complexe |

## 🔧 Dépannage

### Compilation échoue
```bash
# Vérifier g++
g++ --version

# Essayer avec flags explicites
g++ -o ants ants.cpp -std=c++11 -Wall
```

### Script ne s'exécute pas
```bash
# Donner les permissions
chmod +x run_ant_simulations.sh

# Vérifier bash
which bash
```

### Interface ne fonctionne pas
- Utiliser un navigateur moderne (Chrome, Firefox, Safari)
- Vérifier que JavaScript est activé
- Ouvrir la console (F12) pour voir les erreurs

## 🎯 Résultats attendus

### Fourmilière Zero (2 fourmis)
```
Étape 1: f1→S1, f2→S2
Étape 2: f1→Sd, f2→Sd
```

### Fourmilière Un (5 fourmis)
```
Étape 1: f1→S1
Étape 2: f1→S2, f2→S1
Étape 3: f1→Sd, f2→S2, f3→S1
...
```

## 🌟 Fonctionnalités de l'interface

- ✅ **Visualisation en temps réel** des positions
- ✅ **Navigation étape par étape**
- ✅ **Mode de lecture automatique**
- ✅ **Affichage des mouvements**
- ✅ **Codes couleur** pour les différents types de salles
- ✅ **Statistiques détaillées**
- ✅ **Réseau de tunnels** affiché
- ✅ **Support de 6 fourmilières** différentes

## 📈 Extensions possibles

- Ajouter de nouvelles fourmilières
- Modifier les capacités des salles
- Changer les algorithmes de routage
- Ajouter des métriques de performance

---

Bon amusement avec la simulation ! 🐜✨