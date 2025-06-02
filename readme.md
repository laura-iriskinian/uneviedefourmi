# Une Vie de Fourmi 🐜

## Description du Projet

Le projet Une vide de fourmi implémente une simulation d'optimisation de déplacement de fourmis dans une fourmilière. L'objectif est de faire passer toutes les fourmis du vestibule (entrée) au dortoir (sortie) en un minimum d'étapes, en respectant les contraintes de capacité des salles et la structure du réseau de tunnels.

## Problématique

### Contexte
Une colonie de fourmis doit rejoindre le dortoir depuis le vestibule le plus rapidement possible pour éviter les prédateurs. La fourmilière est composée de :
- **Vestibule (Sv)** : Point de départ, peut accueillir toutes les fourmis
- **Dortoir (Sd)** : Point d'arrivée, capacité illimitée comme me vestibule
- **Salles intermédiaires (S1, S2, ...)** : Capacité limitée (par défaut 1 fourmi par salle)
- **Tunnels** : Connexions entre les salles

### Contraintes
1. **Capacité des salles** : Chaque salle a une capacité maximale (sauf Sv et Sd)
2. **Déplacement simultané** : Toutes les fourmis se déplacent à la même vitesse
3. **Tunnels instantanés** : Le passage d'une salle à l'autre se fait en une étape
4. **Occupation exclusive** : Une fourmi ne peut entrer dans une salle que si elle a de la place
5. **Objectif** : Minimiser le nombre d'étapes total

## Architecture du Code

### Fichiers Principaux

#### `ants.h`
Header contenant :
- **Classe Ant** : Représente une fourmi avec son nom, position actuelle et statut (si elle est arrivée au dortoit ou pas)
- **Structure ColonyInfo** : Stocke les informations sur la colonie (nombre de fourmis, capacités des salles, graphe des tunnels)
- **Déclarations des fonctions** : Pathfinding, gestion des mouvements, utilitaires

#### `ants.cpp`
Implémentation des algorithmes principaux :
- **Chargement de fichier** : Parse les fichiers de configuration des fourmilières
- **Algorithme BFS** : Trouve le chemin le plus court entre deux salles
- **Stratégie de mouvement** : Choisit la meilleure salle suivante pour chaque fourmi
- **Gestion des capacités** : Vérifie et met à jour l'occupation des salles

#### `main.cpp`
Programme principal qui :
- Charge la configuration depuis un fichier
- Initialise les fourmis dans le vestibule
- Exécute la simulation étape par étape
- Affiche les mouvements et le résultat final

## Algorithmes Implémentés

### 1. Pathfinding (BFS)
```cpp
vector<string> findShortestPath(string start, string target)
```
Utilise un parcours en largeur pour trouver le chemin le plus court entre deux salles.

### 2. Stratégie de Priorité
Les fourmis sont triées par priorité selon :
1. Distance au dortoir (les plus proches en premier)
2. Ordre lexicographique (f1 avant f2, etc.)

### 3. Sélection de Salle Optimale
```cpp
string chooseBestNextRoom(string currentPos, const map<string, int>& tempOccupancy)
```
Choisit la prochaine salle en privilégiant :
1. Le dortoir (Sd) si accessible directement
2. La salle libre avec le chemin le plus court vers Sd

## Format des Fichiers de Configuration

```
f=5                    # Nombre de fourmis
S1 { 2 }              # Salle S1 avec capacité 2
S2                    # Salle S2 avec capacité par défaut (1)
Sv - S1               # Tunnel entre vestibule et S1
S1 - S2               # Tunnel entre S1 et S2
S2 - Sd               # Tunnel entre S2 et dortoir
```
## Compilation et Exécution

### Compilation  
g++ -o ants main.cpp ants.cpp

### Exécution
./ants

## Résultats et Performance

### Exemple de Sortie
```
+++ Step 1 +++
f1 - Sv - S1

+++ Step 2 +++
f1 - S1 - S2
f2 - Sv - S1

+++ Step 3 +++
f1 - S2 - Sd
f2 - S1 - S2
f3 - Sv - S1
```

### Optimisations Implémentées
1. **Gestion anticipée des capacités** : Prévision des mouvements pour éviter les blocages
2. **Priorisation intelligente** : Les fourmis les plus proches du but bougent en premier
3. **Évitement des deadlocks** : Vérification de disponibilité avant déplacement

## Conclusion

Ce projet démontre l'application d'algorithmes de graphes classiques (BFS, pathfinding, Dijkstra) à un problème d'optimisation combinatoire. La solution implémentée utilise une approche gloutonne avec priorisation qui donne de très bons résultats pratiques, même si elle n'est pas toujours optimale théoriquement.
Le travail de benchmark entre 2 algorithmes montre bien l'efficacité et la rapidité d'exécution de BFS pour ces projets.

L'architecture modulaire du code permet une extension facile pour tester d'autres stratégies et algorithmes.
Le projet illustre également l'importance de la modélisation appropriée d'un problème réel en structures de données informatiques efficaces.

