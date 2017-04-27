#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_ttf.h"

#define XRES 1200
#define YRES 600
#define MINX 0
#define MINY 0
#define MAXX 800

typedef struct
{
    double x,y;
} ControlPoint;

void drawLine32(SDL_Surface* surface, Uint32 pixel, SDL_Rect pos);
void ajoutInfos(float timestamp, float dureeMoyenne, ControlPoint* Clist, int& nombresInfos);
void ShowControlPoints(SDL_Surface* screen, ControlPoint Clist[], int& nombresInfos);
int Re_to_EcrX(double r);
int Re_to_EcrY(double r);
void creerGraphique(ControlPoint listMax[], ControlPoint listMin[], ControlPoint listMoy[], int& nombresInfosMax, int& nombresInfosMin, int& nombresInfosMoy);
void definirPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void drawLine(SDL_Surface* surface, Uint8 r, Uint8 v, Uint8 b, SDL_Rect pos);
Uint32 SDL_GetPixel32(SDL_Surface *surface, int x, int y);
void SDL_PutPixel32(SDL_Surface *surface, int x, int y, Uint32 pixel);
void Line(SDL_Surface* surf,int x1,int y1, int x2,int y2,Uint32 couleur);  // Bresenham
void ShowBezier(SDL_Surface* screen,double tstep,unsigned long couleur, ControlPoint Clist[], int& nombresInfos);
void Casteljau(double t,ControlPoint* tab,int nbpoints,double* rx,double* ry);
void ShowGraphic(SDL_Surface* screen,unsigned long couleur, ControlPoint Clist[], int& nombresInfos);

#endif
