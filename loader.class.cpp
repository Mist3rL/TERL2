#include <cstdlib>
#include <iostream>
#include "loader.class.h"
using namespace std;
LoaderObject::LoaderObject(int ascenseurs_count){
    error=false;
    vide_const=IMG_Load("vide.png");
    if (vide_const==NULL)
    {
        cout << "Erreur chargement image vide" << endl;
        error=true;
    }
    ascenseur_const=IMG_Load("ascenseur.png");
    if (ascenseur_const==NULL)
    {
        cout << "Erreur chargement image ascenseur" << endl;
        error=true;
    }
    personne_const=IMG_Load("personne.png");
    if (personne_const==NULL)
    {
        cout << "Erreur chargement image personne" << endl;
        error=true;
    }
    ascenseur_real_const=IMG_Load("ascenseur_real.png");
    if (ascenseur_real_const==NULL)
    {
        cout << "Erreur chargement image ascenseur_real" << endl;
        error=true;
    }
    ascenseur_open_const=IMG_Load("ascenseur_open.png");
    if (ascenseur_open_const==NULL)
    {
        cout << "Erreur chargement image ascenseur_open" << endl;
        error=true;
    }
    personne_hover_const=IMG_Load("personne_hover.png");
    if (personne_hover_const==NULL)
    {
        cout << "Erreur chargement image personne_hover" << endl;
        error=true;
    }
    tooltip_screen=SDL_CreateRGBSurface(0, 600, 200, 32, 0, 0, 0, 0);
    if (tooltip_screen==NULL)
    {
        cout << "Erreur chargement surface tooltip_screen" << endl;
        error=true;
    }
    int w=840;
    int h=630;
    if (ascenseurs_count<6)
        w=ascenseurs_count*ascenseur_const->w;

    if (NOMBRE_ETAGES<10)
        h=NOMBRE_ETAGES*ascenseur_const->h;

    if (w<420)
        w=420;
    if (h<420)
        h=420;

    window = SDL_CreateWindow("Simulation Ascenseur", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, 0);
    if (!window)
    {
        cout << "Unable to set " << w << "x" << h << " video " << SDL_GetError() << endl;
        error=true;
    }
    ecran = SDL_GetWindowSurface(window);
    double width_wanted=((double)ascenseur_const->w/ascenseurs_count)*((double)ecran->w/ascenseur_const->w);
    double scaleX=width_wanted/ascenseur_const->w;
    double height_wanted=((double)ascenseur_const->h/NOMBRE_ETAGES)*((double)ecran->h/ascenseur_const->h);
    double scaleY=height_wanted/ascenseur_const->h;
    if (ascenseurs_count<7)
        scaleX=1;
    if (NOMBRE_ETAGES<7)
        scaleY=1;

    scale(scaleX, scaleY);
    SDL_FillRect(tooltip_screen, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
}

LoaderObject::~LoaderObject(){
    SDL_FreeSurface(ascenseur);
    SDL_FreeSurface(vide);
    SDL_FreeSurface(personne);
    SDL_FreeSurface(ascenseur_real);
    SDL_FreeSurface(ascenseur_open);
    SDL_FreeSurface(tooltip_screen);
    SDL_FreeSurface(personne_hover);
    SDL_FreeSurface(ascenseur_const);
    SDL_FreeSurface(vide_const);
    SDL_FreeSurface(personne_const);
    SDL_FreeSurface(ascenseur_real_const);
    SDL_FreeSurface(ascenseur_open_const);
    SDL_FreeSurface(personne_hover_const);
}

SDL_Surface* LoaderObject::getSurface(int s){
    switch (s){
        default:
            return ecran;
            break;
        case VIDE:
            return vide;
        case ASCENSEUR:
            return ascenseur;
        case PERSONNE:
            return personne;
        case ASCENSEUR_REAL:
            return ascenseur_real;
        case ASCENSEUR_OPEN:
            return ascenseur_open;
        case TOOLTIP_SCREEN:
            return tooltip_screen;
        case PERSONNE_HOVER:
            return personne_hover;
    }
}

bool LoaderObject::getError(){
    return error;
}

SDL_Window* LoaderObject::getWindow(){
    return window;
}

void LoaderObject::scale(double scaleX, double scaleY)
{
    vide=zoomSurface(vide_const, scaleX, scaleY, SMOOTHING_ON);
    ascenseur=zoomSurface(ascenseur_const, scaleX, scaleY, SMOOTHING_ON);
    personne=zoomSurface(personne_const, scaleX, scaleY, SMOOTHING_ON);
    ascenseur_real=zoomSurface(ascenseur_real_const, scaleX, scaleY, SMOOTHING_ON);
    ascenseur_open=zoomSurface(ascenseur_open_const, scaleX, scaleY, SMOOTHING_ON);
    personne_hover=zoomSurface(personne_hover_const, scaleX, scaleY, SMOOTHING_ON);
}

void LoaderObject::scaleFree()
{
    SDL_FreeSurface(vide);
    SDL_FreeSurface(ascenseur);
    SDL_FreeSurface(personne);
    SDL_FreeSurface(ascenseur_real);
    SDL_FreeSurface(ascenseur_open);
    SDL_FreeSurface(personne_hover);
}

