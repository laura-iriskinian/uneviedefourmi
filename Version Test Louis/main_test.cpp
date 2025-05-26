#include "ants.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

void chargerFourmiliere(Fourmiliere& f, const std::string& nomFichier, int& nbFourmis) {
    std::ifstream fichier(nomFichier);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << nomFichier << std::endl;
        nbFourmis = 50; // Valeur par défaut si le fichier n'existe pas
        return;
    }

    std::string ligne;
    bool tunnelsSection = false;

    while (std::getline(fichier, ligne)) {
        // Supprimer les caractères de retour chariot
        if (!ligne.empty() && ligne.back() == '\r') {
            ligne.pop_back();
        }

        if (ligne.empty()) continue;

        // Nombre de fourmis
        if (ligne[0] == 'f' && ligne.find('=') != std::string::npos) {
            nbFourmis = std::stoi(ligne.substr(ligne.find('=') + 1));
            std::cout << "Nombre de fourmis trouvé: " << nbFourmis << std::endl;
            continue;
        }

        // Détection des tunnels
        if (ligne.find(" - ") != std::string::npos) {
            tunnelsSection = true;
            size_t pos = ligne.find(" - ");
            std::string salle1 = ligne.substr(0, pos);
            std::string salle2 = ligne.substr(pos + 3);
            f.ajouterTunnel(salle1, salle2);
            std::cout << "Tunnel ajouté: " << salle1 << " - " << salle2 << std::endl;
        }
        // Salles
        else if (!tunnelsSection) {
            std::string nomSalle;
            int capacite = 1; // capacité par défaut

            if (ligne.find(" { ") != std::string::npos) {
                size_t pos1 = ligne.find(" { ");
                size_t pos2 = ligne.find(" }");
                nomSalle = ligne.substr(0, pos1);
                capacite = std::stoi(ligne.substr(pos1 + 3, pos2 - pos1 - 3));
            } else {
                nomSalle = ligne;
                capacite = 1;
            }

            f.ajouterSalle(nomSalle, capacite);
            std::cout << "Salle ajoutée: " << nomSalle << " (capacité: " << capacite << ")" << std::endl;
        }
    }

    fichier.close();
}

int main() {
    std::cout << "===========================================" << std::endl;
    std::cout << "    UNE VIE DE FOURMI - SIMULATION" << std::endl;
    std::cout << "===========================================" << std::endl;

    Fourmiliere f;
    int nbFourmis = 0;

    // Ajouter manuellement Sv et Sd d'abord
    f.ajouterSalle("Sv", -1);
    f.ajouterSalle("Sd", -1);

    std::cout << "\n=== CHARGEMENT DE LA FOURMILIERE ===" << std::endl;
    chargerFourmiliere(f, "fourmiliere_cinq.txt", nbFourmis);

    // Si le fichier n'existe pas, créer la fourmilière manuellement
    if (nbFourmis == 0) {
        std::cout << "Création manuelle de la fourmilière..." << std::endl;
        nbFourmis = 50;

        // Ajouter les salles
        f.ajouterSalle("S1", 8);
        f.ajouterSalle("S2", 4);
        f.ajouterSalle("S3", 2);
        f.ajouterSalle("S4", 4);
        f.ajouterSalle("S5", 2);
        f.ajouterSalle("S6", 4);
        f.ajouterSalle("S7", 2);
        f.ajouterSalle("S8", 5);
        f.ajouterSalle("S9", 1);
        f.ajouterSalle("S10", 1);
        f.ajouterSalle("S11", 1);
        f.ajouterSalle("S12", 1);
        f.ajouterSalle("S13", 4);
        f.ajouterSalle("S14", 2);

        // Ajouter les tunnels
        f.ajouterTunnel("S1", "S2");
        f.ajouterTunnel("S2", "S3");
        f.ajouterTunnel("S3", "S4");
        f.ajouterTunnel("S4", "Sd");
        f.ajouterTunnel("Sv", "S1");
        f.ajouterTunnel("S2", "S5");
        f.ajouterTunnel("S5", "S4");
        f.ajouterTunnel("S13", "Sd");
        f.ajouterTunnel("S8", "S12");
        f.ajouterTunnel("S12", "S13");
        f.ajouterTunnel("S6", "S7");
        f.ajouterTunnel("S7", "S9");
        f.ajouterTunnel("S9", "S14");
        f.ajouterTunnel("S14", "Sd");
        f.ajouterTunnel("S7", "S10");
        f.ajouterTunnel("S10", "S14");
        f.ajouterTunnel("S1", "S6");
        f.ajouterTunnel("S6", "S8");
        f.ajouterTunnel("S8", "S11");
        f.ajouterTunnel("S11", "S13");
    }

    std::cout << "Fourmiliere chargee avec " << nbFourmis << " fourmis." << std::endl;

    // Tester le chemin le plus court
    auto chemin = f.plusCourtChemin("Sv", "Sd");
    std::cout << "\nChemin le plus court Sv -> Sd : ";
    for (const auto& s : chemin) {
        std::cout << s << " ";
    }
    std::cout << "(" << chemin.size() - 1 << " etapes)" << std::endl;

    std::cout << "\n===========================================" << std::endl;

    // Lancer la simulation
    std::cout << "Lancement de la simulation..." << std::endl;
    Simulateur simulateur(f, nbFourmis);
    simulateur.executerSimulation();

    std::cout << "===========================================" << std::endl;
    std::cout << "         SIMULATION TERMINEE" << std::endl;
    std::cout << "===========================================" << std::endl;

    return 0;
}