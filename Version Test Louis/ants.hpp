#ifndef ANTS_HPP
#define ANTS_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <set>
#include <algorithm>
#include <climits>

struct Salle {
    std::string nom;
    int capacite; // -1 pour illimitée (Sv et Sd)
    std::vector<std::string> voisins;
    std::vector<std::string> fourmisPresentes; // fourmis actuellement dans cette salle

    bool peutAccueillir() const {
        if (capacite == -1) return true; // Sv et Sd illimitées
        return fourmisPresentes.size() < static_cast<size_t>(capacite);
    }

    void ajouterFourmi(const std::string& fourmi) {
        fourmisPresentes.push_back(fourmi);
    }

    void retirerFourmi(const std::string& fourmi) {
        auto it = std::find(fourmisPresentes.begin(), fourmisPresentes.end(), fourmi);
        if (it != fourmisPresentes.end()) {
            fourmisPresentes.erase(it);
        }
    }
};

class Fourmiliere {
public:
    void ajouterSalle(const std::string& nom, int capacite = 1);
    void ajouterTunnel(const std::string& de, const std::string& vers);
    std::vector<std::string> plusCourtChemin(const std::string& depart, const std::string& arrivee);
    std::vector<std::vector<std::string>> trouverCheminsDisjoints(const std::string& depart, const std::string& arrivee, int nbChemins);
    void afficherEtat() const;
    std::unordered_map<std::string, Salle>& getSalles() { return salles; }

private:
    std::unordered_map<std::string, Salle> salles;
    std::vector<std::string> dijkstra(const std::string& depart, const std::string& arrivee);
};

struct Fourmi {
    std::string nom;
    std::vector<std::string> chemin;
    int position; // index dans le chemin
    bool arrivee = false;

    Fourmi(std::string id, std::vector<std::string> c)
        : nom(id), chemin(c), position(0) {}

    std::string positionActuelle() const {
        if (position < chemin.size()) return chemin[position];
        return "";
    }

    std::string prochaineSalle() const {
        return (position + 1 < chemin.size()) ? chemin[position + 1] : "";
    }

    void avancer() {
        if (position + 1 < chemin.size()) {
            ++position;
            if (position == chemin.size() - 1) arrivee = true;
        }
    }

    bool peutAvancer(Fourmiliere& fourmiliere) const {
        if (arrivee || position + 1 >= chemin.size()) return false;

        std::string prochaine = chemin[position + 1];
        auto& salles = fourmiliere.getSalles();

        // Peut toujours aller vers Sd (dortoir)
        if (prochaine == "Sd") return true;

        return salles[prochaine].peutAccueillir();
    }
};

class Simulateur {
public:
    Simulateur(Fourmiliere& f, int nbFourmis);
    void executerSimulation();

private:
    Fourmiliere& fourmiliere;
    std::vector<Fourmi> fourmis;
    int etapeActuelle = 0;

    void initialiserFourmis(int nbFourmis);
    bool executerEtape();
    void afficherEtape(const std::vector<std::string>& mouvements);
    std::vector<std::vector<std::string>> genererCheminsOptimaux();
};

#endif