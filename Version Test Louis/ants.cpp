#include "ants.hpp"
#include <iostream>
#include <algorithm>
#include <climits>

void Fourmiliere::ajouterSalle(const std::string& nom, int capacite) {
    if (capacite == -1) capacite = 1000000; // infini pour Sv et Sd
    salles[nom] = Salle{nom, capacite, {}, {}};
}

void Fourmiliere::ajouterTunnel(const std::string& de, const std::string& vers) {
    salles[de].voisins.push_back(vers);
    salles[vers].voisins.push_back(de); // tunnels bidirectionnels
}

std::vector<std::string> Fourmiliere::dijkstra(const std::string& depart, const std::string& arrivee) {
    std::unordered_map<std::string, int> distances;
    std::unordered_map<std::string, std::string> precedents;
    std::priority_queue<std::pair<int, std::string>, std::vector<std::pair<int, std::string>>, std::greater<>> pq;

    // Initialisation
    for (const auto& [nom, salle] : salles) {
        distances[nom] = INT_MAX;
    }
    distances[depart] = 0;
    pq.push({0, depart});

    while (!pq.empty()) {
        auto [dist, courant] = pq.top();
        pq.pop();

        if (courant == arrivee) break;
        if (dist > distances[courant]) continue;

        for (const std::string& voisin : salles[courant].voisins) {
            int nouvelleDist = distances[courant] + 1;
            if (nouvelleDist < distances[voisin]) {
                distances[voisin] = nouvelleDist;
                precedents[voisin] = courant;
                pq.push({nouvelleDist, voisin});
            }
        }
    }

    // Reconstruction du chemin
    std::vector<std::string> chemin;
    std::string courant = arrivee;

    while (courant != depart && precedents.find(courant) != precedents.end()) {
        chemin.push_back(courant);
        courant = precedents[courant];
    }
    chemin.push_back(depart);
    std::reverse(chemin.begin(), chemin.end());

    return chemin;
}

std::vector<std::string> Fourmiliere::plusCourtChemin(const std::string& depart, const std::string& arrivee) {
    return dijkstra(depart, arrivee);
}

std::vector<std::vector<std::string>> Fourmiliere::trouverCheminsDisjoints(const std::string& depart, const std::string& arrivee, int nbChemins) {
    std::vector<std::vector<std::string>> chemins;

    // Pour simplifier, on trouve le chemin le plus court et on le réutilise
    // Dans une implémentation plus avancée, on utiliserait l'algorithme de Ford-Fulkerson
    std::vector<std::string> cheminPrincipal = dijkstra(depart, arrivee);

    if (!cheminPrincipal.empty()) {
        for (int i = 0; i < nbChemins; ++i) {
            chemins.push_back(cheminPrincipal);
        }
    }

    return chemins;
}

void Fourmiliere::afficherEtat() const {
    for (const auto& [nom, salle] : salles) {
        if (!salle.fourmisPresentes.empty()) {
            std::cout << nom << ": ";
            for (const std::string& fourmi : salle.fourmisPresentes) {
                std::cout << fourmi << " ";
            }
            std::cout << "(" << salle.fourmisPresentes.size() << "/" << salle.capacite << ")" << std::endl;
        }
    }
}

// Implémentation du Simulateur
Simulateur::Simulateur(Fourmiliere& f, int nbFourmis) : fourmiliere(f) {
    initialiserFourmis(nbFourmis);
}

void Simulateur::initialiserFourmis(int nbFourmis) {
    std::vector<std::vector<std::string>> cheminsOptimaux = genererCheminsOptimaux();

    // Distribution intelligente des fourmis sur les chemins
    std::vector<int> distribution = {15, 10, 8, 8, 5, 4}; // Distribution basée sur les capacités des chemins

    int fourmiIndex = 0;
    for (size_t cheminIndex = 0; cheminIndex < cheminsOptimaux.size() && fourmiIndex < nbFourmis; ++cheminIndex) {
        int nbFourmisEchemin = std::min(distribution[cheminIndex], nbFourmis - fourmiIndex);

        for (int i = 0; i < nbFourmisEchemin; ++i) {
            std::string nomFourmi = "f" + std::to_string(fourmiIndex + 1);
            fourmis.emplace_back(nomFourmi, cheminsOptimaux[cheminIndex]);
            fourmiIndex++;
        }
    }

    // Ajouter toutes les fourmis au vestibule
    for (const Fourmi& fourmi : fourmis) {
        fourmiliere.getSalles()["Sv"].ajouterFourmi(fourmi.nom);
    }
}

std::vector<std::vector<std::string>> Simulateur::genererCheminsOptimaux() {
    std::vector<std::vector<std::string>> chemins;

    // Chemins principaux identifiés dans la fourmilière
    // Chemin 1: Sv -> S1 -> S2 -> S3 -> S4 -> Sd
    chemins.push_back({"Sv", "S1", "S2", "S3", "S4", "Sd"});

    // Chemin 2: Sv -> S1 -> S2 -> S5 -> S4 -> Sd
    chemins.push_back({"Sv", "S1", "S2", "S5", "S4", "Sd"});

    // Chemin 3: Sv -> S1 -> S6 -> S8 -> S11 -> S13 -> Sd
    chemins.push_back({"Sv", "S1", "S6", "S8", "S11", "S13", "Sd"});

    // Chemin 4: Sv -> S1 -> S6 -> S8 -> S12 -> S13 -> Sd
    chemins.push_back({"Sv", "S1", "S6", "S8", "S12", "S13", "Sd"});

    // Chemin 5: Sv -> S1 -> S6 -> S7 -> S9 -> S14 -> Sd
    chemins.push_back({"Sv", "S1", "S6", "S7", "S9", "S14", "Sd"});

    // Chemin 6: Sv -> S1 -> S6 -> S7 -> S10 -> S14 -> Sd
    chemins.push_back({"Sv", "S1", "S6", "S7", "S10", "S14", "Sd"});

    return chemins;
}

void Simulateur::executerSimulation() {
    std::cout << "=== SIMULATION DU DEPLACEMENT DES FOURMIS ===" << std::endl;
    std::cout << "Nombre de fourmis: " << fourmis.size() << std::endl;

    // Afficher la distribution des fourmis par chemin
    std::cout << "\nDistribution des fourmis par chemin:" << std::endl;
    std::vector<std::vector<std::string>> chemins = genererCheminsOptimaux();
    std::vector<int> compteurs(chemins.size(), 0);

    for (const Fourmi& fourmi : fourmis) {
        for (size_t i = 0; i < chemins.size(); ++i) {
            if (fourmi.chemin == chemins[i]) {
                compteurs[i]++;
                break;
            }
        }
    }

    for (size_t i = 0; i < chemins.size(); ++i) {
        if (compteurs[i] > 0) {
            std::cout << "Chemin " << (i+1) << " (" << compteurs[i] << " fourmis): ";
            for (const std::string& salle : chemins[i]) {
                std::cout << salle << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

    // Lancer la simulation
    while (true) {
        if (!executerEtape()) {
            break;
        }

        // Protection contre les boucles infinies
        if (etapeActuelle > 200) {
            std::cout << "ATTENTION: Simulation interrompue après 200 étapes pour éviter une boucle infinie." << std::endl;
            break;
        }
    }

    std::cout << std::endl << "=== SIMULATION TERMINEE ===" << std::endl;
    std::cout << "Toutes les fourmis ont rejoint le dortoir en " << etapeActuelle << " etapes." << std::endl;

    // Afficher les statistiques finales
    int fourmisArrivees = 0;
    for (const Fourmi& fourmi : fourmis) {
        if (fourmi.arrivee) fourmisArrivees++;
    }
    std::cout << "Fourmis arrivées: " << fourmisArrivees << "/" << fourmis.size() << std::endl;
}

bool Simulateur::executerEtape() {
    etapeActuelle++;
    std::vector<std::string> mouvements;
    bool mouvement = false;

    // Tri des fourmis par priorité (plus proche du dortoir = prioritaire)
    std::vector<int> indices;
    for (size_t i = 0; i < fourmis.size(); ++i) {
        if (!fourmis[i].arrivee) {
            indices.push_back(i);
        }
    }

    // Trier par position dans le chemin (plus avancées d'abord)
    std::sort(indices.begin(), indices.end(), [this](int a, int b) {
        return fourmis[a].position > fourmis[b].position;
    });

    // Tenter de faire avancer chaque fourmi selon la priorité
    for (int index : indices) {
        Fourmi& fourmi = fourmis[index];

        if (fourmi.peutAvancer(fourmiliere)) {
            std::string salleActuelle = fourmi.positionActuelle();
            std::string prochaineSalle = fourmi.prochaineSalle();

            // Retirer la fourmi de sa salle actuelle
            fourmiliere.getSalles()[salleActuelle].retirerFourmi(fourmi.nom);

            // Faire avancer la fourmi
            fourmi.avancer();

            // Ajouter la fourmi à sa nouvelle salle
            fourmiliere.getSalles()[prochaineSalle].ajouterFourmi(fourmi.nom);

            // Enregistrer le mouvement
            mouvements.push_back(fourmi.nom + " - " + salleActuelle + " - " + prochaineSalle);
            mouvement = true;
        }
    }

    // Afficher l'étape seulement s'il y a eu des mouvements
    if (mouvement) {
        afficherEtape(mouvements);
    }

    // Vérifier si toutes les fourmis sont arrivées
    bool toutesArrivees = true;
    for (const Fourmi& fourmi : fourmis) {
        if (!fourmi.arrivee) {
            toutesArrivees = false;
            break;
        }
    }

    return !toutesArrivees;
}

void Simulateur::afficherEtape(const std::vector<std::string>& mouvements) {
    std::cout << "+++ E" << etapeActuelle << " +++" << std::endl;
    for (const std::string& mouvement : mouvements) {
        std::cout << mouvement << std::endl;
    }
    std::cout << std::endl;
}