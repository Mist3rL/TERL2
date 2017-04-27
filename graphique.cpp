#include "graphique.h"

using namespace std;
float MAXY=1;
void ajoutInfos(float timestamp, float dureeMoyenne, ControlPoint* Clist, int& nombresInfos)
{
    float timeL=(float)timestamp/1000;
    int h=7,m=59,s=58;
    h+=timeL/3600;
    timeL-=(h-7)*3600;
    m+=timeL/60;
    timeL-=(m-59)*60;
    s+=timeL;
    if (s>59)
    {
        m+=s/60;
        s=s%60;
    }
    if (m>59)
    {
        h+=m/60;
        m=m%60;
    }
    h-=8;
    if (h*60+m>0)
    {
        Clist[h*60+m].x=h*60+m;
        Clist[h*60+m].y=dureeMoyenne;
        if (dureeMoyenne+50>MAXY)
            MAXY=dureeMoyenne+50;

        nombresInfos=h*60+m+1;
    }
}
void drawLine(SDL_Surface* surface, Uint8 r, Uint8 v, Uint8 b, SDL_Rect pos)
{
    Uint32 pixel;
    for (int x=pos.x;x<pos.x+pos.w;x++)
    {
        for (int y=pos.y;y<pos.y+pos.h;y++)
        {
            pixel=SDL_MapRGBA(surface->format, r, v, b, 1);
            definirPixel(surface,x,y,pixel);
        }
    }
}
void ShowControlPoints(SDL_Surface* screen, ControlPoint Clist[], int& nombresInfos)
{
    TTF_Font *police=TTF_OpenFont("police.otf", 12);
    SDL_Surface *texte=NULL;
    SDL_Rect pos;
    char duree[255];
    SDL_Color couleurNoire = {0, 0, 0};
    int i;
    for(i=0;i<nombresInfos;i++)
    {
        int x = Re_to_EcrX(Clist[i].x);
        int y = Re_to_EcrY(Clist[i].y);
        sprintf(duree, "%d", (int)Clist[i].x);
        texte=TTF_RenderText_Solid(police, duree, couleurNoire);
        pos.x=0;
        pos.y=y;
        SDL_BlitSurface(texte, NULL, screen, &pos);
        SDL_FreeSurface(texte);
        sprintf(duree, "%dh00", (int)(Clist[i].y+1));
        texte=TTF_RenderText_Solid(police, duree, couleurNoire);
        pos.x=x;
        pos.y=screen->h-texte->h;
        SDL_BlitSurface(texte, NULL, screen, &pos);
        SDL_FreeSurface(texte);
    }
}
void creerGraphique(ControlPoint listMax[], ControlPoint listMin[], ControlPoint listMoy[], int& nombresInfosMax, int& nombresInfosMin, int& nombresInfosMoy)
{
    TTF_Font *police=TTF_OpenFont("police.otf", 12);
    SDL_Surface *texte=NULL;
    SDL_Rect pos;
    SDL_Color couleurNoire = {0, 0, 0}, couleurRouge = {255, 0, 0}, couleurVert = {0, 255, 0}, couleurBleu = {0, 255, 255};
    Uint32 pixel;
    Uint8 r,g,b,a;
    int x,y;
    SDL_Surface* surface=SDL_CreateRGBSurface(0, 1200, 600, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
    pos.x=30;
    pos.y=10;
    pos.w=1;
    pos.h=surface->h-10;
    drawLine(surface, 0, 0, 0, pos);
    pos.x=30;
    pos.y=surface->h-10;
    pos.w=surface->w-30;
    pos.h=1;
    drawLine(surface, 0, 0, 0, pos);
    pos.w=NULL;
    pos.h=NULL;
    texte=TTF_RenderText_Solid(police, "Heure", couleurNoire);
    pos.x=surface->w-texte->w;
    pos.y=surface->h-texte->h;
    SDL_BlitSurface(texte, NULL, surface, &pos);
    SDL_FreeSurface(texte);
    texte=TTF_RenderText_Solid(police, "Durée (s)", couleurNoire);
    pos.x=0;
    pos.y=0;
    SDL_BlitSurface(texte, NULL, surface, &pos);
    SDL_FreeSurface(texte);
    char heure[255];
    for (int i=0;i<=12;i++)
    {
        sprintf(heure, "%dh00", (i+8));
        texte=TTF_RenderText_Solid(police, heure, couleurNoire);
        pos.x=30+i*XRES/15;
        pos.y=surface->h-texte->h;
        SDL_BlitSurface(texte, NULL, surface, &pos);
        SDL_FreeSurface(texte);
    }
    for (int i=0;i<MAXY-50;i+=MAXY/12)
    {
        sprintf(heure, "%d", i);
        texte=TTF_RenderText_Solid(police, heure, couleurNoire);
        pos.x=30-texte->w;
        pos.y=Re_to_EcrY(i);
        SDL_BlitSurface(texte, NULL, surface, &pos);
        SDL_FreeSurface(texte);
    }
    texte=TTF_RenderText_Solid(police, "Moyen", couleurRouge);
    pos.x=surface->w-100;
    pos.y=100;
    SDL_BlitSurface(texte, NULL, surface, &pos);
    pos.y+=texte->h;
    SDL_FreeSurface(texte);
    texte=TTF_RenderText_Solid(police, "Min", couleurBleu);
    SDL_BlitSurface(texte, NULL, surface, &pos);
    pos.y+=texte->h;
    SDL_FreeSurface(texte);
    texte=TTF_RenderText_Solid(police, "Max", couleurVert);
    SDL_BlitSurface(texte, NULL, surface, &pos);
    SDL_FreeSurface(texte);
    /*pixel=SDL_MapRGBA(surface->format, r, v, b, a);
    definirPixel(surface,x,y,pixel);*/
    //ShowControlPoints(surface, listMax, nombresInfosMax);
    //ShowBezier(surface,0.002,0x00FF00, listMax, nombresInfosMax); /// vert
    //ShowControlPoints(surface, listMoy, nombresInfosMoy);
    //ShowBezier(surface,0.002,0xFF0000, listMoy, nombresInfosMoy); /// rouge
    //ShowControlPoints(surface, listMin, nombresInfosMin);
    //ShowBezier(surface,0.002,0x00FFFF, listMin, nombresInfosMin); /// bleu
    ShowGraphic(surface,0x00FF00,listMax,nombresInfosMax);
    ShowGraphic(surface,0xFF0000,listMoy,nombresInfosMoy);
    ShowGraphic(surface,0x00FFFF,listMin,nombresInfosMin);
    SDL_SaveBMP(surface,"graphique.bmp");
    TTF_CloseFont(police);
}


/*definirPixel : permet de modifier la couleur d'un pixel
Paramètres d'entrée/sortie :
SDL_Surface *surface : la surface sur laquelle on va modifier la couleur d'un pixel
int x : la coordonnée en x du pixel à modifier
int y : la coordonnée en y du pixel à modifier
Uint32 pixel : le pixel à insérer
*/
void definirPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    /*nbOctetsParPixel représente le nombre d'octets utilisés pour stocker un pixel.
    En multipliant ce nombre d'octets par 8 (un octet = 8 bits), on obtient la profondeur de couleur
    de l'image : 8, 16, 24 ou 32 bits.*/
    int nbOctetsParPixel = surface->format->BytesPerPixel;
    /*Ici p est l'adresse du pixel que l'on veut modifier*/
    /*surface->pixels contient l'adresse du premier pixel de l'image*/
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    /*Gestion différente suivant le nombre d'octets par pixel de l'image*/
    switch(nbOctetsParPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            /*Suivant l'architecture de la machine*/
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

void Casteljau(double t,ControlPoint* tab,int nbpoints,double* rx,double* ry, int& nombresInfos)
{
    ControlPoint subtab[nombresInfos];
    int i;
    if (nbpoints == 1)
    {
        *rx = tab[0].x;
        *ry = tab[0].y;
        return;
    }
    for(i=0;i<nbpoints-1;i++)
    {
        subtab[i].x = t*tab[i].x + (1-t)*tab[i+1].x;
        subtab[i].y = t*tab[i].y + (1-t)*tab[i+1].y;
    }
    Casteljau(t,subtab,nbpoints-1,rx,ry,nombresInfos);
}
void ShowGraphic(SDL_Surface* screen,unsigned long couleur, ControlPoint Clist[], int& nombresInfos)
{
    int lastx=0, lasty=0;
    int x=0,y=0;
    for (int i=0;i<nombresInfos;i++)
    {
        x=30+Clist[i].x*XRES/MAXX;
        y=Re_to_EcrY(Clist[i].y);
        if (i!=0 && x>=0 && x<=XRES && y>=0 && y<=YRES)
            Line(screen, x, y, lastx, lasty, couleur);
        lastx=x;
        lasty=y;
    }

}
void ShowBezier(SDL_Surface* screen,double tstep,unsigned long couleur, ControlPoint Clist[], int& nombresInfos)
{
    double t = 0.0;
    int lastx = 0;
    int lasty = 0;
    int x,y;
    double rx = 0.0;
    double ry = 0.0;
    int i = 0;
    while(t<=1.0)
    {
        Casteljau(t,Clist,nombresInfos,&rx,&ry,nombresInfos);
        x = Re_to_EcrX(rx);
        y = Re_to_EcrY(ry);
        if (i!=0)
            Line(screen,x,y,lastx,lasty,couleur);

        lastx = x;
        lasty = y;
        t+=tstep;
        i++;
    }
}
int Re_to_EcrX(double r)
{
    return (int)30+(XRES*(r-MINX)/(MAXX-MINX));
}

int Re_to_EcrY(double r)
{
    int y = (int)10+(YRES*(r-MINY)/(MAXY-MINY));
    y = YRES - y - 1;
    return y;
}
void Line(SDL_Surface* surf,int x1,int y1, int x2,int y2,Uint32 couleur)  // Bresenham
{
    int x,y;
    int Dx,Dy;
    int xincr,yincr;
    int erreur;
    int i;

    Dx = abs(x2-x1);
    Dy = abs(y2-y1);
    if(x1<x2)
        xincr = 1;
    else
        xincr = -1;
    if(y1<y2)
        yincr = 1;
    else
        yincr = -1;

    x = x1;
    y = y1;
    if(Dx>Dy)
    {
        erreur = Dx/2;
        for(i=0;i<Dx;i++)
        {
            x += xincr;
            erreur += Dy;
            if(erreur>Dx)
            {
                erreur -= Dx;
                y += yincr;
            }
            SDL_PutPixel32(surf,x, y,couleur);
        }
    }
    else
    {
        erreur = Dy/2;
        for(i=0;i<Dy;i++)
        {
            y += yincr;
            erreur += Dx;
            if(erreur>Dy)
            {
                erreur -= Dy;
                x += xincr;
            }
            SDL_PutPixel32(surf,x, y,couleur);
        }
    }
    SDL_PutPixel32(surf,x1,y1,couleur);
    SDL_PutPixel32(surf,x2,y2,couleur);
}

void SDL_PutPixel32(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32*)p = pixel;
}

Uint32 SDL_GetPixel32(SDL_Surface *surface, int x, int y)
{
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * 4;
    return *(Uint32*)p;
}
