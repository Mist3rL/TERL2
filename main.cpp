#include <cstdlib>
#include <iostream>
#include <time.h>
#include <sstream>
#include <string>
#include <fstream>
#include "config.h"
#include "loader.class.h"
#include "gui.h"
#include "moteur.h"
#include "population.class.h"
#include "souris.struct.h"
#include "load_config.h"
#include "simulation.h"
#include "edit_config_immeuble.h"
using namespace std;
int main (int argc, char** argv)
{
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO)<0)
    {
        cout << "Unable to init SDL " << SDL_GetError() << endl;
        return 1;
    }
    TTF_Init();
    vector<Ascenseur> ascenseurs;
    vector<string> load_config;
    ifstream fichier("immeuble.config", ios::in);
    int max_etages=1;
    if(fichier)
    {
            string ligne;
            string etages;
            while (getline(fichier, ligne))
            {
                load_config.push_back(ligne);
                istringstream iss(ligne);
                while (getline(iss, etages, ' '))
                {
                    if (max_etages<atoi(etages.c_str())+1)
                        max_etages=atoi(etages.c_str())+1;
                }
            }
            fichier.close();
    }
    Immeuble immeubles(max_etages);
    loadConfig(load_config, immeubles, ascenseurs); /// Chargement de l'immeuble en fonction de l'image
    atexit(SDL_Quit);
    LoaderObject surfaces(ascenseurs.size(), immeubles.getEtagesCount()); /// Chargement des objets ect...
    Population pop(NOMBRE_PERSONNES, immeubles.getEtagesCount(), string("population.config"));
    simulation(surfaces, immeubles, ascenseurs, pop);
    TTF_Quit();
    SDL_Quit();
    float temps_moyen=0;
    float temps_total=0;
    for (size_t i=0;i<NOMBRE_PERSONNES;i++)
    {
        temps_total+=pop.getListe()[i].getTempsTotal();
        if (pop.getListe()[i].getNombreVoyages()==0)
            temps_moyen+=pop.getListe()[i].getTempsTotal();
        else
            temps_moyen+=pop.getListe()[i].getTempsTotal()/pop.getListe()[i].getNombreVoyages();
    }
    temps_moyen=temps_moyen/(float)NOMBRE_PERSONNES;
    cout << "Le temps d'attente total est de " << temps_total << " secondes." << endl;
    cout << "Le temps d'attente moyen est de " << temps_moyen << " secondes." << endl;
    cout << "Immeuble de " << immeubles.getEtagesCount() << " etages. " << endl;
    cout << "Nombre de personnes : " << NOMBRE_PERSONNES << endl;
    cout << "Nombre de personns maximum par ascenseur : " << NOMBRE_PERSONNES_MAX << endl;
    cout << "Nombre d'ascenseurs : " << ascenseurs.size() << endl;
    cout << "Temps d'attente minimum : " << Personne::minTime << endl;
    cout << "Temps d'attente maximum : " << Personne::maxTime << endl;
    return 0;
}
