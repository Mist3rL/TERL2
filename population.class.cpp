#include "population.class.h"
using namespace std;
Population::Population(size_t n)
{
    listes=new Personne[n];
}
Population::~Population()
{
    delete listes;
}
Personne* Population::getListe()
{
    return listes;
}
void Population::gerer(int tempsEcoule, vector<Ascenseur>& listes, LoaderObject& objects)
{
    unsigned int id_pers;
    vector<int> listes_etages;
    vector<size_t> listes_ascenseurAble;
    vector< vector<int> > listes_ascenseurAble2;
    vector<int> c;
    float ta;
    bool find=false;
    int best_index=-1;
    int temp_etages;
    int best_indice=-1;
    for (size_t i=0;i<NOMBRE_PERSONNES;i++)
    {
        if (!this->listes[i].getIsHome()) /// Si la personne n'est toujours pas rentré chez elle
        {
            ta=this->listes[i].getTimeArrive();
            if (ta>0) /// Si elle n'est pas arrivé encore
            {
                ta=ta-((float)tempsEcoule/1000);
                if (ta<=0) /// Elle vient d'arriver, on lui dit d'aller au "bouton"
                {
                    this->listes[i].setCurrentEtage(0);
                    this->listes[i].setColsAim(0);
                    this->listes[i].setVitesse(VITESSE_PERSONNE);
                    this->listes[i].setVisible(true);
                    this->listes[i].setPosition(-1*objects.getSurface(PERSONNE)->w,objects.getSurface(ECRAN)->h-objects.getSurface(PERSONNE)->h);
                    this->listes[i].setBusy(true);
                }
                this->listes[i].setTimeArrive(ta);
            }
            else if (ta<=0) /// La personne est déjà la
            {
                float temp=0;
                if (!this->listes[i].getVisible() && this->listes[i].getAscenseurId()==-1) /// Si on est pas visible et pas dans un ascenseur, on fait des courses
                {
                    temp=this->listes[i].getNextDecision().getDuree()-((float)tempsEcoule/1000);
                    if (temp<=0)
                    {
                        this->listes[i].removeDecision();
                        this->listes[i].setColsAim(0);
                        this->listes[i].setVitesse(VITESSE_PERSONNE);
                        this->listes[i].setVisible(true);
                        this->listes[i].setBusy(true);
                    }
                    else
                    {
                        this->listes[i].getNextDecision().setDuree(temp);
                    }
                }
                else if (this->listes[i].getWaitElevators() && this->listes[i].getVitesse()==0) /// elle attends un ascenseur et n'est pas en direction d'un
                {
                    /// On va donc lui en attribuer un
                    this->listes[i].setTempsAttente(this->listes[i].getTempsAttente()+((float)tempsEcoule/1000));
                    for (size_t j=0;j<listes.size();j++)
                    {
                        /// L'ascenseur peut venir à l'étage actuel de la personne
                        if (vectorHave(this->listes[i].getCurrentEtage(), listes[j].getEtages()))
                        {
                            /// peut aller à l'étage voulu de la personne
                            if (vectorHave(this->listes[i].getNextEtages(), listes[j].getEtages()))
                            {
                                /// Soit l'ascenseur est vide soit il se dirige vers la bonne direction
                                /// Il faut vérifier que l'ascenseur passe devant notre étage
                                /// Il faut vérifier que l'ascenseur se dirige vers notre étage
                                if (listes[j].getNbrPersonnes()==0 ||
                                    (listes[j].seekEtagesAim(this->listes)>listes[j].getCurrentEtages()
                                     && this->listes[i].getNextEtages()>this->listes[i].getCurrentEtage()
                                     && this->listes[i].getCurrentEtage()>=listes[j].getCurrentEtages()) ||
                                    (listes[j].seekEtagesAim(this->listes)<listes[j].getCurrentEtages()
                                     && this->listes[i].getNextEtages()<this->listes[i].getCurrentEtage()
                                     && this->listes[i].getCurrentEtage()<=listes[j].getCurrentEtages()))
                                {
                                    /// Il peut accepter du monde ?
                                    if (listes[j].canAcceptPeople())
                                    {
                                        listes_ascenseurAble.push_back(j);
                                    }
                                }
                            }
                            else /// On regarde s'il l'ascenseur peut nous amener à un ascenseur qui lui peut aller à l'étage voulu de la personne
                            {
                                for (size_t w=0;w<listes.size();w++)
                                {
                                    if (w!=j && vectorHave(this->listes[i].getNextEtages(), listes[w].getEtages()) && !vectorHave(this->listes[i].getCurrentEtage(), listes[w].getEtages()))
                                    {
                                        /// Donc, on a tous ce que l'on veut, mais l'ascenseur j peut il aller à un étage en commun de w ??
                                        vector<int> tempV=vectorGetCommun(listes[j].getEtages(), listes[w].getEtages());
                                        if (tempV.size()>0)
                                        {
                                            int etageAim=abs(tempV[0]-this->listes[i].getCurrentEtage());
                                            for (size_t h=1;h<tempV.size();h++)
                                            {
                                                if (abs(tempV[h]-this->listes[i].getCurrentEtage())<etageAim)
                                                    etageAim=abs(tempV[h]-this->listes[i].getCurrentEtage());
                                            }
                                            vector<int> tempSub;
                                            tempSub.push_back(j);
                                            tempSub.push_back(etageAim);
                                            listes_ascenseurAble2.push_back(tempSub);
                                        }
                                    }
                                }
                            }

                        }
                    }
                    /// On parcours la liste des ascenseurs qui nous intéresse
                    for (size_t j=0;j<listes_ascenseurAble.size() && !find;j++)
                    {
                        if (listes[listes_ascenseurAble[j]].getCurrentEtages()==this->listes[i].getCurrentEtage())
                        {
                            listes[listes_ascenseurAble[j]].increaseWait();
                            this->listes[i].setColsAim(listes[listes_ascenseurAble[j]].getCols()-1);
                            this->listes[i].setAscenseur(listes_ascenseurAble[j]);
                            this->listes[i].setTempsAttente(0);
                            this->listes[i].setBusy(true);
                            this->listes[i].setVitesse(VITESSE_PERSONNE);
                            find=true;
                        }
                        else
                        {
                            if (best_index==-1 || best_indice>listes[listes_ascenseurAble[j]].getDiffEtages(this->listes[i].getCurrentEtage(), this->listes[i].getNextEtages()))
                            {
                                best_index=listes_ascenseurAble[j];
                                best_indice=listes[listes_ascenseurAble[j]].getDiffEtages(this->listes[i].getCurrentEtage(), this->listes[i].getNextEtages());
                            }
                        }
                    }
                    if (!find && best_index!=-1)
                    {
                        if (listes[best_index].getNbrPersonnes()>0)
                        {
                            temp_etages=listes[best_index].seekEtagesAim(this->listes);
                            if (temp_etages>listes[best_index].getCurrentEtages() && temp_etages<this->listes[i].getCurrentEtage()) /// L'ascenseur va descendre
                            {
                                find=true;
                            }
                            else if (temp_etages>this->listes[i].getCurrentEtage()) /// L'ascenseur va monter
                            {
                                find=true;
                            }
                        }
                        else
                        {
                            find=true;
                        }
                        if (find)
                        {
                            if (listes[best_index].getVitesse() || !listes[best_index].getHadToWait()) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
                            {
                                listes[best_index].setEtagesAim(this->listes[i].getCurrentEtage());
                            }
                        }
                    }
                    listes_ascenseurAble.clear();
                    best_index=-1;
                    if (!find)
                    {
                        for (size_t j=0;j<listes_ascenseurAble2.size();j++)
                        {
                            if (listes[listes_ascenseurAble2[j][0]].getCurrentEtages()==this->listes[i].getCurrentEtage())
                            {
                                listes[listes_ascenseurAble2[j][0]].increaseWait();
                                this->listes[i].setColsAim(listes[listes_ascenseurAble2[j][0]].getCols()-1);
                                this->listes[i].setAscenseur(listes_ascenseurAble2[j][0]);
                                this->listes[i].setTempsAttente(0);
                                this->listes[i].setBusy(true);
                                this->listes[i].setVitesse(VITESSE_PERSONNE);
                                this->listes[i].setAimEtage(listes_ascenseurAble2[j][1]);
                                find=true;
                            }
                            else
                            {
                                if (best_index==-1 || best_indice>listes[listes_ascenseurAble2[j][0]].getDiffEtages(this->listes[i].getCurrentEtage(), listes_ascenseurAble2[j][1]))
                                {
                                    best_index=listes_ascenseurAble2[j][0];
                                    best_indice=listes[listes_ascenseurAble2[j][0]].getDiffEtages(this->listes[i].getCurrentEtage(), listes_ascenseurAble2[j][1]);
                                }
                            }
                        }
                        if (!find && best_index!=-1)
                        {
                            if (listes[best_index].getNbrPersonnes()>0)
                            {
                                temp_etages=listes[best_index].seekEtagesAim(this->listes);
                                if (temp_etages>listes[best_index].getCurrentEtages() && temp_etages<this->listes[i].getCurrentEtage()) /// L'ascenseur va descendre
                                {
                                    find=true;
                                }
                                else if (temp_etages>this->listes[i].getCurrentEtage()) /// L'ascenseur va monter
                                {
                                    find=true;
                                }
                            }
                            else
                            {
                                find=true;
                            }
                            if (find)
                            {
                                if (listes[best_index].getVitesse() || !listes[best_index].getHadToWait()) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
                                {
                                    listes[best_index].setEtagesAim(this->listes[i].getCurrentEtage());
                                }
                            }
                        }
                    }
                    listes_ascenseurAble2.clear();
                    best_index=-1;
                    find=false;
                }
            }
        }
    }
}
void gererPersonnesAscenseur(Ascenseur& ascenseur, Population& pop, LoaderObject& objects, int nombreCols)
{
    SDL_Rect p;
    vector <unsigned int> had_to_remove;
    for (size_t i=0;i<ascenseur.getPersonnes().size();i++)
    {
        /// L'ascenseur est arrivé à l'étage où l'on souhaite descendre pour nos courses
        if (pop.getListe()[ascenseur.getPersonnes()[i]].getNextEtages()==(int)ascenseur.getCurrentEtages())
        {
            /// De ce fait on met à jours positions, vitesse ect...
            p.y=objects.getSurface(ECRAN)->h-(ascenseur.getCurrentEtages())*objects.getSurface(ASCENSEUR)->h-objects.getSurface(PERSONNE)->h;
            p.x=ascenseur.getCols()*objects.getSurface(ASCENSEUR)->w-objects.getSurface(ASCENSEUR)->w/2-objects.getSurface(PERSONNE)->w/2;
            pop.getListe()[ascenseur.getPersonnes()[i]].setPosition(p.x, p.y);
            pop.getListe()[ascenseur.getPersonnes()[i]].setVitesse(VITESSE_PERSONNE);
            pop.getListe()[ascenseur.getPersonnes()[i]].setVisible(true);
            pop.getListe()[ascenseur.getPersonnes()[i]].setColsAim(nombreCols);
            pop.getListe()[ascenseur.getPersonnes()[i]].setBusy(true);
            pop.getListe()[ascenseur.getPersonnes()[i]].setCurrentEtage(ascenseur.getCurrentEtages());
            pop.getListe()[ascenseur.getPersonnes()[i]].setAscenseur(-1);
            pop.getListe()[ascenseur.getPersonnes()[i]].setAimEtage(-1);
            had_to_remove.push_back(ascenseur.getPersonnes()[i]);
        }
        else if (pop.getListe()[ascenseur.getPersonnes()[i]].getAimEtage()==(int)ascenseur.getCurrentEtages())
        {
            /// Ici l'ascenseur est arrivé à un étage qui nous permet de prendre un autre ascenseur
            p.y=objects.getSurface(ECRAN)->h-(ascenseur.getCurrentEtages())*objects.getSurface(ASCENSEUR)->h-objects.getSurface(PERSONNE)->h;
            p.x=ascenseur.getCols()*objects.getSurface(ASCENSEUR)->w-objects.getSurface(ASCENSEUR)->w/2-objects.getSurface(PERSONNE)->w/2;
            pop.getListe()[ascenseur.getPersonnes()[i]].setPosition(p.x, p.y);
            pop.getListe()[ascenseur.getPersonnes()[i]].setVitesse(-1*VITESSE_PERSONNE);
            pop.getListe()[ascenseur.getPersonnes()[i]].setVisible(true);
            pop.getListe()[ascenseur.getPersonnes()[i]].setColsAim(0);
            pop.getListe()[ascenseur.getPersonnes()[i]].setBusy(true);
            pop.getListe()[ascenseur.getPersonnes()[i]].setCurrentEtage(ascenseur.getCurrentEtages());
            pop.getListe()[ascenseur.getPersonnes()[i]].setAscenseur(-1);
            pop.getListe()[ascenseur.getPersonnes()[i]].setAimEtage(-1);
            had_to_remove.push_back(ascenseur.getPersonnes()[i]);
        }
    }
    /// On stock les personnes qui sortent de l'ascenseur dans une variable vecteur pour éviter de
    /// ne pas faire le tour du for au dessus.
    for (size_t i=0;i<had_to_remove.size();i++)
    {
        ascenseur.removePersonnes(had_to_remove[i]);
    }
}
void gererAscenseurs(vector<Ascenseur>& liste, Population& population)
{
    unsigned int id_pers;

    for (size_t i=0;i<liste.size();i++)
    {
        if (liste[i].getVitesse()==0) /// Si l'ascenseur est immobile
        {
            /// S'il ne doit pas attendre une personne et qu'il y a du monde dedans
            if (!liste[i].getHadToWait() && liste[i].getNbrPersonnes()>0)
            {
                liste[i].setEtagesAim(liste[i].seekEtagesAim(population.getListe())); /// et on prend donc la première destination en croissant
            }
        }
    }
}
