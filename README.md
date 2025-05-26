# 🐜 Une Vie de Fourmi

Ce projet simule le déplacement de fourmis dans une fourmilière représentée sous forme de graphe, dans lequel chaque fourmi doit rejoindre le dortoir (`Sd`) en un **minimum d'étapes**, en respectant certaines contraintes.

---

## 📌 Objectif

Faire transiter toutes les fourmis depuis le vestibule (`Sv`) jusqu'au dortoir (`Sd`), en :

- Respectant les **capacités des salles** (une ou plusieurs fourmis selon la configuration)
- Évitant les collisions (une seule fourmi par salle si capacité = 1)
- Utilisant le **chemin le plus court** à chaque étape
- Respectant les règles de déplacement par **étapes simultanées**

---

## ⚙️ Fichiers du projet

| Fichier       | Rôle |
|---------------|------|
| `main.cpp`    | Code principal de la simulation |
| `input.txt`   | Configuration de la fourmilière et des fourmis |
| *(optionnel)* `output.txt` | Pour enregistrer les étapes (non implémenté ici) |

---

## 📥 Format du fichier `input.txt`

Le fichier contient les informations suivantes dans cet ordre :

- Nombre de fourmis
- Nombre de salles
- Salles (Nom et capacité)
- Nombre de tunnels
- Tunnels (chaque ligne : salle1 salle2)
    - Capacité `-1` = capacité **illimitée** (ex: `Sv`, `Sd`)
    - Le graphe est **non orienté** (les tunnels vont dans les deux sens)

---

## ▶️ Compilation et exécution

### Compilation :

```bash
g++ -o fourmiliere main.cpp

### **Execution** :


./fourmiliere
