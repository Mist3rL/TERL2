#include "gui.h"
using namespace std;
void afficherImmeuble(LoaderObject& objects, Immeuble& immeuble, vector<Ascenseur>& ascenseurs, SDL_Rect camera, SDL_Surface *texte, TTF_Font* police)
{
    SDL_Color couleurNoire = {0, 0, 0};
    SDL_Rect pos;
    unsigned int etage_hauteur=objects.getSurface(ASCENSEUR)->h;
    unsigned int cols_largeur=objects.getSurface(ASCENSEUR)->w;
    char etage_number[4];
    for (size_t i=0;i<immeuble.getEtagesCount();i++)
    {
        for (size_t j=0;j<ascenseurs.size();j++)
        {
            pos.y=objects.getSurface(ECRAN)->h-(i+1)*etage_hauteur+camera.y;
            pos.x=j*cols_largeur-camera.x;
            if (immeuble.getListesEtages()[i].haveAscenseurAtCols(j,ascenseurs))
            {
                SDL_BlitSurface(objects.getSurface(ASCENSEUR), NULL, objects.getSurface(ECRAN), &pos);
            }
            else
            {
                SDL_BlitSurface(objects.getSurface(VIDE), NULL, objects.getSurface(ECRAN), &pos);
            }
        }
        sprintf(etage_number, "%d", i);
        texte=TTF_RenderText_Solid(police, etage_number, couleurNoire);
        pos.y=objects.getSurface(ECRAN)->h-(i+1)*etage_hauteur+objects.getSurface(ASCENSEUR)->h/2-texte->h/2+camera.y;
        pos.x=1-camera.x;
        SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
        SDL_FreeSurface(texte);
    }
}

void afficherAscenseurs(LoaderObject& objects, vector<Ascenseur>& ascenseurs, SDL_Rect camera, sourisTooltip& sInfos)
{
    SDL_Rect pos;
    for (size_t j=0;j<ascenseurs.size();j++)
    {
        pos.x=j*objects.getSurface(ASCENSEUR)->w-camera.x;
        if (ascenseurs[j].getVitesse()==0)
        {
            ascenseurs[j].setPosition(objects.getSurface(ECRAN)->h-(ascenseurs[j].getCurrentEtages()+1)*objects.getSurface(ASCENSEUR)->h);
            pos.y=ascenseurs[j].getPosition()+camera.y;
            if (!sInfos.onTooltip && sInfos.x>=pos.x && sInfos.x<=pos.x+objects.getSurface(ASCENSEUR_OPEN)->w && sInfos.y>=pos.y && sInfos.y<=pos.y+objects.getSurface(ASCENSEUR_OPEN)->h)
            {
                sInfos.ascenseursHover=j;
                sInfos.isHover=true;
            }
            SDL_BlitSurface(objects.getSurface(ASCENSEUR_OPEN), NULL, objects.getSurface(ECRAN), &pos);
        }
        else
        {
            pos.y=ascenseurs[j].getPosition()+camera.y;
            if (!sInfos.onTooltip && sInfos.x>=pos.x && sInfos.x<=pos.x+objects.getSurface(ASCENSEUR_OPEN)->w && sInfos.y>=pos.y && sInfos.y<=pos.y+objects.getSurface(ASCENSEUR_OPEN)->h)
            {
                sInfos.ascenseursHover=j;
                sInfos.isHover=true;
            }
            SDL_BlitSurface(objects.getSurface(ASCENSEUR_REAL), NULL, objects.getSurface(ECRAN), &pos);
        }
    }
}

void ecrireHeure(LoaderObject& objects, long timeL, SDL_Surface *texte, TTF_Font *police, int SPEED_SIMULATION)
{
    SDL_Color couleurNoire = {0, 0, 0};
    SDL_Rect pos;
    pos.x=20;
    pos.y=objects.getSurface(ECRAN)->h-20;
    float timestamp=(float)timeL/1000;
    int h=7,m=59,s=58;
    h+=timestamp/3600;
    timestamp-=(h-7)*3600;
    m+=timestamp/60;
    timestamp-=(m-59)*60;
    s+=timestamp;
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
    char texte_heure[10];
    sprintf(texte_heure, "%d:%d:%d", h,m,s);
    texte=TTF_RenderText_Solid(police, texte_heure, couleurNoire);
    pos.y-=texte->h;
    SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
    pos.x+=texte->w+5;
    SDL_FreeSurface(texte);
    sprintf(texte_heure, "x%d", SPEED_SIMULATION);
    texte=TTF_RenderText_Solid(police, texte_heure, couleurNoire);
    SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
    SDL_FreeSurface(texte);
}
void afficherPersonnes(LoaderObject& objects, Population& population, SDL_Rect camera, sourisTooltip& sInfos, int nbrCols)
{
    SDL_Rect pos;
    for (size_t i=0;i<NOMBRE_PERSONNES;i++)
    {
        if (population.getListe()[i].getVisible())
        {
            pos.x=population.getListe()[i].getPositionX()-camera.x;
            pos.y=population.getListe()[i].getPositionY()+camera.y;
            if (!sInfos.onTooltip && sInfos.x>=pos.x && sInfos.x<=pos.x+objects.getSurface(PERSONNE)->w && sInfos.y>=pos.y && sInfos.y<=pos.y+objects.getSurface(PERSONNE)->h)
            {
                sInfos.isHover=true;
                sInfos.personnesHover.push_back(i);
            }
            if (population.getListe()[i].getVitesse()>0 && population.getListe()[i].getColsAim()>=nbrCols) // entrain de partir aux "courses"
                SDL_BlitSurface(objects.getSurface(PERSONNE_HOVER), NULL, objects.getSurface(ECRAN), &pos);
            else
                SDL_BlitSurface(objects.getSurface(PERSONNE), NULL, objects.getSurface(ECRAN), &pos);
        }
    }
}
void afficherTooltip(LoaderObject& objects, Population& population, vector<Ascenseur>& listes, SDL_Surface* texte, TTF_Font* police, sourisTooltip& sInfos)
{
    SDL_Color couleurNoire = {0, 0, 0}, couleurRouge = {255, 0, 0};
    SDL_Rect pos;
    if (sInfos.isHover)
    {
        if (!sInfos.onTooltip)
        {
            sInfos.xTool=sInfos.x;
            sInfos.yTool=sInfos.y-objects.getSurface(TOOLTIP_SCREEN)->h;
            if (sInfos.x+objects.getSurface(TOOLTIP_SCREEN)->w>objects.getSurface(ECRAN)->w)
                sInfos.xTool-=(sInfos.x+objects.getSurface(TOOLTIP_SCREEN)->w-objects.getSurface(ECRAN)->w);
            if (sInfos.yTool<0)
                sInfos.yTool+=objects.getSurface(TOOLTIP_SCREEN)->h;
        }
        pos.x=sInfos.xTool;
        pos.y=sInfos.yTool;
        SDL_BlitSurface(objects.getSurface(TOOLTIP_SCREEN), NULL, objects.getSurface(ECRAN), &pos);
        if (sInfos.personnesHover.size()>0) /// On doit afficher des personnes
        {
            sInfos.isHoverPageN=false;
            sInfos.isHoverPageP=false;
            int per_ligne=objects.getSurface(TOOLTIP_SCREEN)->w/(objects.getSurface(PERSONNE)->w+3);
            int per_col=objects.getSurface(TOOLTIP_SCREEN)->h/(objects.getSurface(PERSONNE)->h+3)-1;
            size_t index=(sInfos.page-1)*per_ligne*per_col;
            if (sInfos.personnesHover.size()==1)
            {
                sInfos.personneSelected=0;
            }
            if (sInfos.personneSelected==-1)
            {
                sInfos.personneHover=-1;
                for (size_t i=0;i<per_col && index<sInfos.personnesHover.size();i++)
                {
                    for (size_t j=0;j<per_ligne && index<sInfos.personnesHover.size();j++)
                    {
                        index++;
                        pos.x+=3;
                        if (sInfos.x>=pos.x && sInfos.x<=pos.x+objects.getSurface(PERSONNE)->w &&
                           sInfos.y>=pos.y && sInfos.y<=pos.y+objects.getSurface(PERSONNE)->h)
                        {
                           sInfos.personneHover=index-1;
                           SDL_BlitSurface(objects.getSurface(PERSONNE_HOVER), NULL, objects.getSurface(ECRAN), &pos);
                        }
                        else
                            SDL_BlitSurface(objects.getSurface(PERSONNE), NULL, objects.getSurface(ECRAN), &pos);
                        pos.x+=objects.getSurface(PERSONNE)->w;
                    }
                    pos.y+=objects.getSurface(PERSONNE)->h+3;
                    pos.x=sInfos.xTool;
                }
                if (per_ligne*per_col<sInfos.personnesHover.size()) /// Il y a trop de personnes en dessous de la souris, on va devoir paginer le tout
                {
                    if (sInfos.page*per_ligne*per_col<sInfos.personnesHover.size())
                    {
                        texte=TTF_RenderText_Solid(police, "->", couleurNoire);
                        pos.x=sInfos.xTool+objects.getSurface(TOOLTIP_SCREEN)->w-texte->w;
                        pos.y=sInfos.yTool+objects.getSurface(TOOLTIP_SCREEN)->h-texte->h;
                        if (sInfos.x>=pos.x && sInfos.x<=pos.x+texte->w && sInfos.y>=pos.y && sInfos.y<=pos.y+texte->h)
                        {
                            sInfos.isHoverPageN=true;
                            SDL_FreeSurface(texte);
                            texte=TTF_RenderText_Solid(police, "->", couleurRouge);
                        }
                        SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                        SDL_FreeSurface(texte);
                    }
                    if (sInfos.page>1)
                    {
                        texte=TTF_RenderText_Solid(police, "<-", couleurNoire);
                        pos.x=sInfos.xTool;
                        pos.y=sInfos.yTool+objects.getSurface(TOOLTIP_SCREEN)->h-texte->h;
                        if (sInfos.x>=pos.x && sInfos.x<=pos.x+texte->w && sInfos.y>=pos.y && sInfos.y<=pos.y+texte->h)
                        {
                            sInfos.isHoverPageP=true;
                            SDL_FreeSurface(texte);
                            texte=TTF_RenderText_Solid(police, "<-", couleurRouge);
                        }
                        SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                        SDL_FreeSurface(texte);
                    }
                }
            }
            else
            {
                Personne p=population.getListe()[sInfos.personnesHover[sInfos.personneSelected]];
                char texteI[255];
                sprintf(texteI, "Personne : %d", sInfos.personnesHover[sInfos.personneSelected]);
                texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
                pos.x+=15;
                pos.y+=15;
                SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                pos.y+=texte->h;
                SDL_FreeSurface(texte);

                sprintf(texteI, "Sa tournée : ");
                texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
                SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                pos.x+=texte->w;
                SDL_FreeSurface(texte);

                vector<int> listes_etages=p.getListesEtages();
                for (size_t i=0;i<listes_etages.size();i++)
                {
                    sprintf(texteI, "%d,", listes_etages[i]);
                    texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
                    SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                    pos.x+=texte->w;
                    if (pos.x>=sInfos.xTool+objects.getSurface(TOOLTIP_SCREEN)->w)
                    {
                        pos.x=sInfos.xTool+15;
                        pos.y+=texte->h;
                    }
                    if (i+1==listes_etages.size())
                        pos.y+=texte->h;

                    SDL_FreeSurface(texte);
                }
                listes_etages.clear();
                pos.x=sInfos.xTool+15;

                if (p.getVitesse()!=0)
                    sprintf(texteI, "Etat : Mouvement");
                else if (p.getAscenseurId()>=0)
                    sprintf(texteI, "Etat : Ascenseur");
                else if (p.getWaitElevators())
                    sprintf(texteI, "Etat : Attente (%2f)", p.getTempsAttente());
                else if (!p.getVisible())
                    sprintf(texteI, "Etat : Courses");
                else
                    printf(texteI, "Etat : ????");

                texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
                SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                SDL_FreeSurface(texte);
            }
        }
        else /// Un ascenseur
        {
            Ascenseur asc=listes[sInfos.ascenseursHover];
            char texteI[255];
            sprintf(texteI, "Ascenseur : %d", sInfos.ascenseursHover);
            texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
            pos.x+=15;
            pos.y+=15;
            SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
            SDL_FreeSurface(texte);
            sprintf(texteI, "Nombre de personnes dans l'ascenseur : %d", asc.getNbrPersonnes());
            texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
            pos.y+=25;
            SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
            SDL_FreeSurface(texte);
            if (asc.getNbrPersonnes()>0)
            {
                sprintf(texteI, "Voir la/les personne(s)", asc.getNbrPersonnes());
                texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
                pos.x+=20;
                pos.y+=25;
                if (sInfos.x>=pos.x && sInfos.x<=pos.x+texte->w && sInfos.y>=pos.y && sInfos.y<=pos.y+texte->h)
                {
                    sInfos.isHoverPersInAsc=true;
                    SDL_FreeSurface(texte);
                    texte=TTF_RenderText_Solid(police, texteI, couleurRouge);
                }
                SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                SDL_FreeSurface(texte);
                pos.x-=20;
            }
            sprintf(texteI, "Nombre de personnes se dirigeant vers lui : %d", asc.getHadToWaitInt());
            texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
            pos.y+=25;
            SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
            SDL_FreeSurface(texte);
            if (asc.getVitesse())
            {
                sprintf(texteI, "Se dirige vers l'etage :  %d", asc.getEtagesAim());
            }
            else
            {
                sprintf(texteI, "Etage Actuel : %d", asc.getCurrentEtages());
            }
            texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
            pos.y+=25;
            SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
            SDL_FreeSurface(texte);
            sprintf(texteI, "Colonne : %d", asc.getCols());
            texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
            pos.y+=25;
            SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
            SDL_FreeSurface(texte);
            sprintf(texteI, "Peut aller aux etages : ");
            texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
            pos.y+=25;
            SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
            pos.x+=texte->w;
            SDL_FreeSurface(texte);
            for (size_t i=0;i<asc.getEtages().size();i++)
            {
                sprintf(texteI, "%d,", asc.getEtages()[i]);
                texte=TTF_RenderText_Solid(police, texteI, couleurNoire);
                SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
                pos.x+=texte->w;
                SDL_FreeSurface(texte);
            }
        }
    }
}

void afficherEtagesCount(LoaderObject& objects, Population& population, Immeuble& immeuble, SDL_Surface* texte, TTF_Font* police, SDL_Rect camera)
{
    long cntPopEtages[immeuble.getEtagesCount()]={0};
    long notarrived=0;
    for (size_t i=0;i<NOMBRE_PERSONNES;i++)
    {
        if (population.getListe()[i].getTimeArrive()>0)
        {
            notarrived++;
        }
        else if (population.getListe()[i].getAscenseurId()<0 && population.getListe()[i].getCurrentEtage()>=0)
        {
            cntPopEtages[population.getListe()[i].getCurrentEtage()]+=1;
        }
    }
    SDL_Color couleurNoire = {0, 0, 0};
    SDL_Rect pos;
    unsigned int etage_hauteur=objects.getSurface(ASCENSEUR)->h;
    char etage_number[255];
    for (size_t i=0;i<immeuble.getEtagesCount();i++)
    {
        if (i==0)
        {
            sprintf(etage_number, "%d (%d)", cntPopEtages[i], notarrived);
        }
        else
        {
            sprintf(etage_number, "%d", cntPopEtages[i]);
        }
        texte=TTF_RenderText_Solid(police, etage_number, couleurNoire);
        pos.y=objects.getSurface(ECRAN)->h-(i+1)*etage_hauteur+objects.getSurface(ASCENSEUR)->h/2-texte->h/2+camera.y;
        pos.x=objects.getSurface(ECRAN)->w-texte->w;
        SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
        SDL_FreeSurface(texte);
    }
}

void afficherBoutons(LoaderObject& objects, SDL_Surface* texte, TTF_Font* police, sourisTooltip& sInfos)
{
    SDL_Color couleurNoire = {0, 0, 0}, couleurRouge = {255, 0 ,0};
    SDL_Rect pos;

    if (sInfos.isHoverPause)
        texte=TTF_RenderText_Solid(police, "||", couleurRouge);
    else
        texte=TTF_RenderText_Solid(police, "||", couleurNoire);

    pos.y=objects.getSurface(ECRAN)->h-texte->h-5;
    pos.x=objects.getSurface(ECRAN)->w-texte->w-5;
    SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
    if (sInfos.x>=pos.x && sInfos.x<=pos.x+texte->w && sInfos.y>=pos.y && sInfos.y<=pos.y+texte->h)
        sInfos.isHoverPause=true;
    else
        sInfos.isHoverPause=false;

    SDL_FreeSurface(texte);


    if (sInfos.isHoverPlus)
        texte=TTF_RenderText_Solid(police, "+", couleurRouge);
    else
        texte=TTF_RenderText_Solid(police, "+", couleurNoire);

    pos.x-=texte->w+5;
    SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
    if (sInfos.x>=pos.x && sInfos.x<=pos.x+texte->w && sInfos.y>=pos.y && sInfos.y<=pos.y+texte->h)
        sInfos.isHoverPlus=true;
    else
        sInfos.isHoverPlus=false;

    SDL_FreeSurface(texte);


    if (sInfos.isHoverMinus)
        texte=TTF_RenderText_Solid(police, "-", couleurRouge);
    else
        texte=TTF_RenderText_Solid(police, "-", couleurNoire);

    pos.x-=texte->w+5;
    SDL_BlitSurface(texte, NULL, objects.getSurface(ECRAN), &pos);
    if (sInfos.x>=pos.x && sInfos.x<=pos.x+texte->w && sInfos.y>=pos.y && sInfos.y<=pos.y+texte->h)
        sInfos.isHoverMinus=true;
    else
        sInfos.isHoverMinus=false;

    SDL_FreeSurface(texte);

    if (sInfos.isHoverMinus || sInfos.isHoverPlus || sInfos.isHoverPause)
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
}
