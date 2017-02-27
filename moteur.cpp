#include <iostream>
#include "moteur.h"

using namespace std;
void bougerAscenseurs(LoaderObject& objects, vector<Ascenseur>& ascenseurs, long tempsEcoule, Population& population)
{
    float mouvementPx=0;
    long vitesse=0;
    int currentPos=0;
    int aimPx=0;
    int newPos;
    for (size_t i=0;i<ascenseurs.size();i++)
    {
        vitesse=ascenseurs[i].getVitesse();
        if (vitesse!=0) /// L'ascenseur est en mouvement
        {
            currentPos=ascenseurs[i].getPosition();
            aimPx=objects.getSurface(ECRAN)->h-(ascenseurs[i].getEtagesAim()+1)*objects.getSurface(ASCENSEUR)->h;
            mouvementPx=(vitesse*((float)tempsEcoule)/1000)*objects.getSurface(ASCENSEUR)->h/2.5;
            if (vitesse>0)
                newPos=ceil((double)currentPos+mouvementPx);
            else
                newPos=floor((double)currentPos+mouvementPx);
            if ((vitesse>0 && currentPos+mouvementPx>=aimPx) || (vitesse<0 && currentPos+mouvementPx<=aimPx)) /// S'il est arrivé à destination (ou un peu plus loin)
            {
                mouvementPx=aimPx-currentPos; /// On le positionne au bon endroit
                ascenseurs[i].setVitesse(NONE);
                ascenseurs[i].setCurrentEtages(ascenseurs[i].getEtagesAim()); /// On indique qu'il est arrivé à  destination
                gererPersonnesAscenseur(ascenseurs[i], population, objects, ascenseurs.size()); /// Et on gère les personnes qui sont à l'intérieur
            }
            ascenseurs[i].setPosition(newPos);
        }
        else
        {
            gererPersonnesAscenseur(ascenseurs[i], population, objects, ascenseurs.size());
        }
    }
}
void bougerPersonnes(LoaderObject& objects, Population& population, long tempsEcoule, vector<Ascenseur>& ascenseurs)
{
    float mouvementPx=0;
    long vitesse=0;
    int currentPosX, currentPosY;
    int aimPx=0;
    for (size_t i=0;i<NOMBRE_PERSONNES;i++)
    {
        vitesse=population.getListe()[i].getVitesse();
        if (population.getListe()[i].getVisible() && vitesse!=0) /// Si on est visible et qu'on bouge
        {
            currentPosX=population.getListe()[i].getPositionX();
            currentPosY=population.getListe()[i].getPositionY();
            aimPx=population.getListe()[i].getColsAim()*objects.getSurface(ASCENSEUR)->w;
            if (population.getListe()[i].getWaitElevators()) /// On se dirige vers la porte de l'ascenseur
            {
                aimPx+=(objects.getSurface(ASCENSEUR)->w/2)-10; /// c'est juste du graphique ! Pour donner l'impression qu'on va bien dans l'ascenseur
            }
            mouvementPx=(vitesse*((float)tempsEcoule)/1000)*objects.getSurface(ASCENSEUR)->h/2.5;
            if ((vitesse<0 && currentPosX+mouvementPx<=aimPx) || (vitesse>0 && currentPosX+mouvementPx>=aimPx)) /// On est arrivé à destination
            {
                mouvementPx=aimPx-currentPosX;
                if (vitesse>0 && population.getListe()[i].getColsAim()>=ascenseurs.size()) /// on sort de l'écran
                {
                    population.getListe()[i].setVisible(false);
                    if (population.getListe()[i].getCurrentEtage()==0) /// Il rentre chez lui
                    {
                        population.getListe()[i].setIsHome(true);
                    }
                }
                else
                {
                    if (population.getListe()[i].getWaitElevators()) /// On attends un ascenseur et on a bougé vers celui ci, on est donc arrivé à la porte de l'ascenseur voulu
                    {
                        population.getListe()[i].setWaitElevators(false);
                        population.getListe()[i].setVisible(false); /// On est dans l'ascenseur, on n'est plus visible
                        ascenseurs[population.getListe()[i].getAscenseurId()].addPersonnes(i);
                        ascenseurs[population.getListe()[i].getAscenseurId()].decreaseWait();
                    }
                    else
                    {
                        population.getListe()[i].setWaitElevators(true); /// On est au "bouton" d'ascenseurs, on attends
                    }
                }
                population.getListe()[i].setVitesse(0);
                population.getListe()[i].setBusy(false); /// On est plus en mouvement, on peut donc lui donner de nouvelles directives
            }
            if (mouvementPx>0 && mouvementPx<1)
                mouvementPx=ceil(mouvementPx); /// Bon la c'est un peu de la triche, on ne peut pas positionner les surfaces en float, uniquement en int, malheureusement, on obtiens souvent un float, donc on ceil pour arrondir au supérieur
            else if (mouvementPx<0)
                mouvementPx=floor(mouvementPx); /// Idem mais en inférieur

            currentPosX=currentPosX+mouvementPx;
            population.getListe()[i].setPosition(currentPosX, currentPosY);
        }
    }
}
