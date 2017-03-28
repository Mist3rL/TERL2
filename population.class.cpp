#include "population.class.h"
using namespace std;


Population::Population(size_t n, int nombres_etages, string filename)
{
    ifstream fichier(filename, ios::in);
    int nbr_personnes=0;
    vector<string> contenu;
    if(fichier)
    {
            string ligne;
            while (getline(fichier, ligne))
            {
                nbr_personnes++;
                contenu.push_back(ligne);
            }
            fichier.close();
            listes=new Personne[nbr_personnes];
            for (size_t i=0;i<nbr_personnes;i++)
            {
                listes[i].init(contenu[i]);
            }
    }
    else
    {
        listes=new Personne[n];
        for (size_t i=0;i<n;i++)
            listes[i].init(nombres_etages);

        save();
    }
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
    vector<int> listesTemp;
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

                        }
                    }
                    /// On parcours la liste des ascenseurs qui nous intéresse
                    for (size_t j=0;j<listes_ascenseurAble.size() && !find;j++)
                    {
                        if (listes[listes_ascenseurAble[j]].getCurrentEtages()==this->listes[i].getCurrentEtage() && listes[listes_ascenseurAble[j]].getVitesse()==0)
                        {
                            /// TODO : Prendre l'ascensuer le plus rapide !!!!
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
                            else if (temp_etages<listes[best_index].getCurrentEtages() && temp_etages>this->listes[i].getCurrentEtage()) /// L'ascenseur va monter
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
                            if ((listes[best_index].getVitesse() || !listes[best_index].getHadToWait()) && this->listes[i].getCurrentEtage()!=listes[best_index].getCurrentEtages()) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
                            {
                                listes[best_index].setEtagesAim(this->listes[i].getCurrentEtage());
                            }
                        }
                    }
                    listes_ascenseurAble.clear();
                    best_index=-1;
                    if (!find)
                    {
                        if (getPath(listes, this->listes[i].getNextEtages(), this->listes[i].getCurrentEtage(), listesTemp))
                        {
                            if (listesTemp.size()>1)
                            {
                                cout << listesTemp[listesTemp.size()-1] << " et " << listesTemp[listesTemp.size()-2] << endl;
                                int etages_commun=vectorGetCommun(listes[listesTemp[listesTemp.size()-1]].getEtages(), listes[listesTemp[listesTemp.size()-2]].getEtages())[0];
                                cout << "=" << etages_commun << endl;
                                if (listes[listesTemp[listesTemp.size()-1]].getCurrentEtages()==this->listes[i].getCurrentEtage() && listes[listesTemp[listesTemp.size()-1]].getVitesse()==0)
                                {
                                    listes[listesTemp[listesTemp.size()-1]].increaseWait();
                                    this->listes[i].setColsAim(listes[listesTemp[listesTemp.size()-1]].getCols()-1);
                                    this->listes[i].setAscenseur(listesTemp[listesTemp.size()-1]);
                                    this->listes[i].setTempsAttente(0);
                                    this->listes[i].setBusy(true);
                                    this->listes[i].setVitesse(VITESSE_PERSONNE);
                                    this->listes[i].setAimEtage(etages_commun);
                                    find=true;
                                }
                                else
                                {
                                    if (listes[0].getNbrPersonnes()>0)
                                    {
                                        temp_etages=listes[listesTemp[listesTemp.size()-1]].seekEtagesAim(this->listes);
                                        if (temp_etages>listes[listesTemp[listesTemp.size()-1]].getCurrentEtages() && temp_etages<this->listes[i].getCurrentEtage()) /// L'ascenseur va descendre
                                        {
                                            find=true;
                                        }
                                        else if (temp_etages<listes[listesTemp[listesTemp.size()-1]].getCurrentEtages() && temp_etages>this->listes[i].getCurrentEtage()) /// L'ascenseur va monter
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
                                        if ((listes[listesTemp[listesTemp.size()-1]].getVitesse() || !listes[listesTemp[listesTemp.size()-1]].getHadToWait()) &&
                                            this->listes[i].getCurrentEtage()!=listes[listesTemp[listesTemp.size()-1]].getCurrentEtages()) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
                                        {
                                            listes[listesTemp[listesTemp.size()-1]].setEtagesAim(this->listes[i].getCurrentEtage());
                                        }
                                    }
                                }
                            }
                            listesTemp.clear();
                        }
                    }
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

void Population::save()
{
    ofstream fichier("population.config", ios::out | ios::trunc); // ouverture en écriture avec effacement du fichier ouvert
    vector<Decision> a;
    if(fichier)
    {
        for (size_t i=0;i<NOMBRE_PERSONNES;i++)
        {
            fichier << listes[i].getTimeArrive();
            a=listes[i].getAllDecisions();
            for (size_t j=0;j<a.size();j++)
            {
                 fichier << ' ' << a[j].getEtages() << ':' << a[j].getDuree();
            }
            fichier << endl;
        }
        fichier.close();
    }
}

bool getPath(vector<Ascenseur>& listes, int etages_aim, int etages_start, vector<int>& v)
{
    if (etages_aim==etages_start)
        return false;

    vector<int> tempBoth;
    vector<int> tempOne;
    for (size_t i=0;i<listes.size();i++)
    {
        if (!vectorHave(i, v))
        {
            if (vectorHave(etages_aim, listes[i].getEtages()))
            {
                if (vectorHave(etages_start, listes[i].getEtages()))
                {
                    v.push_back(i);
                    return true;
                }
                else
                {
                    tempOne.push_back(i);
                }
            }
        }
    }
    vector<unsigned int> listesEtages;
    vector<int> final;
    bool etat;
    for (size_t i=0;i<tempOne.size();i++)
    {
        listesEtages=listes[tempOne[i]].getEtages();
        for (size_t j=0;j<listesEtages.size();j++)
        {
            if (listesEtages[j]!=etages_aim)
            {
                etat=getPath(listes, listesEtages[j], etages_start, final);
                if (etat)
                {
                    v.push_back(tempOne[i]);
                    for (size_t w=0;w<final.size();w++)
                    {
                        v.push_back(final[w]);
                    }
                    return true;
                }
            }
        }
    }
    return false;
}
