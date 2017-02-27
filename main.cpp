#include <cstdlib>
#include <iostream>
#include <time.h>
#include "config.h"
#include "loader.class.h"
#include "gui.h"
#include "moteur.h"
#include "population.class.h"
#include "souris.struct.h"
using namespace std;
int main (int argc, char** argv)
{
    int SPEED_SIMULATION=1;
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO)<0)
    {
        cout << "Unable to init SDL " << SDL_GetError() << endl;
        return 1;
    }
    TTF_Init();
    vector<Ascenseur> ascenseurs;
    ascenseurs.push_back(Ascenseur(1)); /// On créé un ascenseur en lui associant la colonne 1
    ascenseurs.push_back(Ascenseur(2));
    ascenseurs.push_back(Ascenseur(3));
    ascenseurs.push_back(Ascenseur(4));
    ascenseurs.push_back(Ascenseur(5));
    ascenseurs.push_back(Ascenseur(6));
    ascenseurs.push_back(Ascenseur(7));
    Immeuble immeubles(NOMBRE_ETAGES);
    immeubles.getListesEtages()[0].addAscenseur(2, ascenseurs); /// On associe à l'étage 0 l'ascenseur ayant la 2ème id
    immeubles.getListesEtages()[2].addAscenseur(2, ascenseurs);
    immeubles.getListesEtages()[4].addAscenseur(2, ascenseurs);
   immeubles.getListesEtages()[0].addAscenseur(3, ascenseurs);
    immeubles.getListesEtages()[1].addAscenseur(3, ascenseurs);
    immeubles.getListesEtages()[3].addAscenseur(3, ascenseurs);
    immeubles.getListesEtages()[0].addAscenseur(4, ascenseurs);
    immeubles.getListesEtages()[0].addAscenseur(5, ascenseurs);
    immeubles.getListesEtages()[0].addAscenseur(6, ascenseurs);
    immeubles.getListesEtages()[0].addAscenseur(7, ascenseurs);
    immeubles.getListesEtages()[5].addAscenseur(4, ascenseurs);
    immeubles.getListesEtages()[6].addAscenseur(5, ascenseurs);
    immeubles.getListesEtages()[7].addAscenseur(6, ascenseurs);
    immeubles.getListesEtages()[8].addAscenseur(7, ascenseurs);
    for (size_t i=0;i<NOMBRE_ETAGES;i++)
    {
        immeubles.getListesEtages()[i].addAscenseur(1, ascenseurs);
    }
    atexit(SDL_Quit);
    LoaderObject surfaces(ascenseurs.size()); /// Chargement des objets ect...
    long tempsActuel = 0, tempsPrecedent = 0, timestamp = 0;
    int cameraVx = 0, cameraVy = 0;
    SDL_Rect camera={0};
    Population pop(NOMBRE_PERSONNES); /// On créé la population de NOMBRE_PERSONNES
    bool continuer = !surfaces.getError(), pause=false;
    sourisTooltip sInfos; sInfos.x=0; sInfos.y=0; sInfos.isHover=false; sInfos.ascenseursHover=-1, sInfos.onTooltip=false, sInfos.page=1, sInfos.personneHover=-1, sInfos.personneSelected=-1;
    sInfos.isHoverPageN=false; sInfos.isHoverPageP=false, sInfos.isHoverPersInAsc=false; sInfos.isHoverPlus=false; sInfos.isHoverMinus=false; sInfos.isHoverPause=false;
    TTF_Font *police=TTF_OpenFont("police.otf", 30), *policeLittle=TTF_OpenFont("police.otf", 20);
    SDL_Surface *texte=NULL;
    while (continuer)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                default:break;
                case SDL_QUIT:
                    continuer = false;
                break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        default:break;
                        case SDLK_ESCAPE:
                            continuer=false;
                        break;
                        case SDLK_SPACE:
                            pause=!pause;
                        break;
                    }
                break;
                case SDL_MOUSEMOTION: /// La gestion de la caméra.. on s'en fiche un peu
                    if (event.motion.x>(surfaces.getSurface(ECRAN)->w)-50 && event.motion.x<(surfaces.getSurface(ECRAN)->w))
                        cameraVx=CAMERA_SPEED_X;
                    else if (event.motion.x<50)
                        cameraVx=-1*CAMERA_SPEED_X;
                    else
                        cameraVx=0;

                    if (event.motion.y>surfaces.getSurface(ECRAN)->h-50)
                        cameraVy=-1*CAMERA_SPEED_Y;
                    else if (event.motion.y<50)
                        cameraVy=CAMERA_SPEED_Y;
                    else
                        cameraVy=0;

                    sInfos.x=event.motion.x;
                    sInfos.y=event.motion.y;
                break;
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button)
                    {
                        default:break;
                        case SDL_BUTTON_LEFT:
                            if (sInfos.onTooltip)
                            {
                                if (sInfos.personneHover!=-1)
                                    sInfos.personneSelected=sInfos.personneHover;
                                else if (sInfos.isHoverPageP)
                                    sInfos.page--;
                                else if (sInfos.isHoverPageN)
                                    sInfos.page++;
                                else if (sInfos.isHoverPersInAsc)
                                {
                                    sInfos.personnesHover.clear();
                                    for (size_t i=0;i<ascenseurs[sInfos.ascenseursHover].getPersonnes().size();i++)
                                    {
                                        sInfos.personnesHover.push_back(ascenseurs[sInfos.ascenseursHover].getPersonnes()[i]);
                                    }
                                }
                            }
                            if (sInfos.isHoverMinus && SPEED_SIMULATION>1)
                                SPEED_SIMULATION--;
                            else if (sInfos.isHoverPlus && SPEED_SIMULATION<20)
                                SPEED_SIMULATION++;
                            else if (sInfos.isHoverPause)
                                pause=!pause;
                        break;
                    }
                break;
            }
        }
        /// La gestion de la caméra.. on s'en fiche un peu
        if ((cameraVx>0 && camera.x+cameraVx+(surfaces.getSurface(ECRAN)->w)<=ascenseurs.size()*surfaces.getSurface(ASCENSEUR)->w) || (cameraVx<0 && camera.x+cameraVx>=0))
            camera.x+=cameraVx;
        else
            cameraVx=0;
        if ((cameraVy<0 && camera.y+cameraVy>=0) || (cameraVy>0 && camera.y+cameraVy+surfaces.getSurface(ECRAN)->h<=immeubles.getEtagesCount()*surfaces.getSurface(ASCENSEUR)->h))
            camera.y+=cameraVy;
        else
        {
            if (cameraVy>0)
                camera.y=immeubles.getEtagesCount()*surfaces.getSurface(ASCENSEUR)->h-surfaces.getSurface(ECRAN)->h;
            else if (cameraVy<0)
                camera.y=0;

            cameraVy=0;
        }
        if (sInfos.isHover && (sInfos.x<sInfos.xTool || sInfos.x>sInfos.xTool+surfaces.getSurface(TOOLTIP_SCREEN)->w || sInfos.y<sInfos.yTool || sInfos.y>sInfos.yTool+surfaces.getSurface(TOOLTIP_SCREEN)->h))
        {
            sInfos.onTooltip=false;
            sInfos.isHover=false;
            sInfos.page=1;
            sInfos.personneHover=-1;
            sInfos.personneSelected=-1;
            sInfos.personnesHover.clear();
            sInfos.isHoverPageN=false;
            sInfos.isHoverPageP=false;
            sInfos.isHoverPersInAsc=false;
        }
        else if (sInfos.isHover)
            sInfos.onTooltip=true;

        tempsActuel=SDL_GetTicks(); /// Renvoie le nombre de ms depuis le début du programme
        if (!pause)
        {
            gererAscenseurs(ascenseurs, pop); /// Permet de donner des ordres aux ascenseurs pour savoir s'ils doivent monter ou descendre (en fonction de leur passagers)
            pop.gerer((tempsActuel-tempsPrecedent)*SPEED_SIMULATION, ascenseurs, surfaces); /// Donnes des "ordres" à la population, et aux ascenseurs
            bougerAscenseurs(surfaces, ascenseurs, (tempsActuel-tempsPrecedent)*SPEED_SIMULATION, pop); /// Moteur physique des ascenseurs
            bougerPersonnes(surfaces, pop, (tempsActuel-tempsPrecedent)*SPEED_SIMULATION, ascenseurs); /// Moteur physique des personnes
            timestamp+=(tempsActuel-tempsPrecedent)*SPEED_SIMULATION;
        }
        tempsPrecedent=tempsActuel;
        SDL_FillRect(surfaces.getSurface(ECRAN), 0, SDL_MapRGB(surfaces.getSurface(ECRAN)->format, 0, 0, 0));
            afficherImmeuble(surfaces, immeubles, ascenseurs, camera, texte, police); /// La gui du programme
            afficherAscenseurs(surfaces, ascenseurs, camera, sInfos); /// La gui du programme
            afficherPersonnes(surfaces, pop, camera, sInfos, ascenseurs.size()); /// La gui du programme
            afficherBoutons(surfaces, texte, police, sInfos);
            afficherTooltip(surfaces, pop, ascenseurs, texte, police, sInfos); /// La gui du programme
            ecrireHeure(surfaces, timestamp, texte, police, SPEED_SIMULATION); /// La gui du programme
            afficherEtagesCount(surfaces, pop, immeubles, texte, police, camera);
        SDL_UpdateWindowSurface(surfaces.getWindow());
    }
    TTF_CloseFont(police);
    TTF_CloseFont(policeLittle);
    SDL_FreeSurface(texte);
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
    cout << "Immeuble de " << NOMBRE_ETAGES << " etages. " << endl;
    cout << "Nombre de personnes : " << NOMBRE_PERSONNES << endl;
    cout << "Nombre de personns maximum par ascenseur : " << NOMBRE_PERSONNES_MAX << endl;
    cout << "Nombre d'ascenseurs : " << ascenseurs.size() << endl;
    return 0;
}
