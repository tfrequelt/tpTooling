#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

// Fonction pour lire un fichier en tant que string
string lireFichier(const string& chemin) {
    ifstream fichier(chemin);
    if (!fichier) {
        cerr << "Erreur : Impossible de lire le fichier " << chemin << endl;
        exit(1);
    }

    stringstream buffer;
    buffer << fichier.rdbuf();
    return buffer.str();
}

// Fonction pour extraire une valeur JSON basique (parsing simple sans bibliothèque externe)
string extraireValeurJSON(const string& json, const string& cle) {
    size_t pos = json.find("\"" + cle + "\"");
    if (pos == string::npos) return "Inconnu";

    size_t debut = json.find(":", pos) + 1;
    size_t fin = json.find_first_of(",}", debut);
    string valeur = json.substr(debut, fin - debut);

    // Nettoyage des guillemets et espaces
    valeur.erase(remove(valeur.begin(), valeur.end(), '\"'), valeur.end());
    valeur.erase(remove(valeur.begin(), valeur.end(), ' '), valeur.end());

    return valeur;
}

// Fonction pour extraire le nom du projet depuis "Modules"
string extraireNomProjet(const string& json, const string& cheminFichier) {
    size_t pos = json.find("\"Modules\"");
    if (pos != string::npos) {
        size_t debut = json.find("\"Name\"", pos);
        if (debut != string::npos) {
            return extraireValeurJSON(json, "Name");
        }
    }

    // Si aucun nom trouvé dans "Modules", utiliser le nom du fichier
    size_t debutNomFichier = cheminFichier.find_last_of("/\\") + 1;
    size_t finNomFichier = cheminFichier.find(".uproject");
    return cheminFichier.substr(debutNomFichier, finNomFichier - debutNomFichier);
}

// Fonction pour afficher les infos du projet
void afficherInfos(const string& cheminUproject) {
    string contenu = lireFichier(cheminUproject);

    string nomJeu = extraireNomProjet(contenu, cheminUproject);
    string versionUnreal = extraireValeurJSON(contenu, "EngineAssociation");

    cout << "Nom du jeu : " << nomJeu << endl;
    cout << "Version Unreal : " << versionUnreal << endl;

    size_t posPlugins = contenu.find("\"Plugins\"");
    if (posPlugins != string::npos) {
        cout << "Plugins activés :" << endl;
        size_t debut = contenu.find("[", posPlugins);
        size_t fin = contenu.find("]", debut);
        string plugins = contenu.substr(debut, fin - debut);

        size_t posNom = plugins.find("\"Name\"");
        while (posNom != string::npos) {
            size_t debutNom = plugins.find(":", posNom) + 1;
            size_t finNom = plugins.find(",", debutNom);
            string nomPlugin = plugins.substr(debutNom, finNom - debutNom);

            nomPlugin.erase(remove(nomPlugin.begin(), nomPlugin.end(), '\"'), nomPlugin.end());
            cout << " - " << nomPlugin << endl;

            posNom = plugins.find("\"Name\"", finNom);
        }
    } else {
        cout << "Aucun plugin activé." << endl;
    }
}

// Fonction pour exécuter une commande
void executerCommande(const string& commande) {
    int retour = system(commande.c_str());
    if (retour != 0) {
        cerr << "Erreur : La commande a échoué avec le code " << retour << endl;
    }
}

// Fonction pour compiler le projet Unreal
void compilerProjet(const string& cheminUproject) {
    string contenu = lireFichier(cheminUproject);
    string nomProjet = extraireNomProjet(contenu, cheminUproject);

    string commande = "call./Engine/Build/BatchFiles/Build.bat " + nomProjet + "Editor Win64 Development \"" + cheminUproject + "\"";
    cout << "Exécution de la commande : " << commande << endl;
    executerCommande(commande);
}

// Fonction pour packager le projet Unreal
void packagerProjet(const string& cheminUproject, const string& cheminPackage) {
    string commande = "call./Engine/Build/BatchFiles/RunUAT.bat BuildCookRun -project=\"" + cheminUproject + "\" -stagingdirectory=\"" + cheminPackage + "\"";
    cout << "Exécution de la commande : " << commande << endl;
    executerCommande(commande);
}

// Point d'entrée du programme
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage : " << argv[0] << " [CHEMIN_DU_UPROJECT] [COMMAND] [OPTIONS]" << endl;
        return 1;
    }

    string cheminUproject = argv[1];
    string commande = argv[2];

    if (commande == "show-infos") {
        afficherInfos(cheminUproject);
    } else if (commande == "build") {
        compilerProjet(cheminUproject);
    } else if (commande == "package") {
        if (argc < 4) {
            cerr << "Erreur : Vous devez spécifier un chemin de package." << endl;
            return 1;
        }
        string cheminPackage = argv[3];
        packagerProjet(cheminUproject, cheminPackage);
    } else {
        cerr << "Commande inconnue : " << commande << endl;
        return 1;
    }

    return 0;
}
