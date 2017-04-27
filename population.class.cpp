#include "population.class.h"
using namespace std;


Population::Population(size_t n, int nombres_etages, string filename)
{
    ifstream fichier(filename.c_str(), ios::in);
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
void getTemps(float& temps_moyen, float& temps_total, Population& pop)
{
    for (size_t i=0;i<NOMBRE_PERSONNES;i++)
    {
        temps_total+=pop.getListe()[i].getTempsTotal();
        if (pop.getListe()[i].getNombreVoyages()==0)
            temps_moyen+=pop.getListe()[i].getTempsTotal();
        else
            temps_moyen+=pop.getListe()[i].getTempsTotal()/pop.getListe()[i].getNombreVoyages();
    }
    temps_moyen=temps_moyen/(float)NOMBRE_PERSONNES;
}
void Population::gerer(int tempsEcoule, vector<Ascenseur>& listes, LoaderObject& objects)
{
    unsigned int id_pers;
    vector<int> listes_etages;
    vector<size_t> listes_ascenseurAble;
    vector< vector<int> > listesTempT;
    vector<int> listesTemp;
    vector< vector<int> > listesAscenseurMoved;
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
                    // Si un chemin a été trouvée
                    if (getPath(listes, this->listes[i].getNextEtages(), this->listes[i].getCurrentEtage(), listesTempT))
                    {
                        for (size_t w=0;w<listesTempT.size() && !find;w++)
                        {
                            listesTemp=listesTempT[w];
                            if (listes[listesTemp[listesTemp.size()-1]].canAcceptPeople())
                            {
                                int etages_commun=this->listes[i].getNextEtages();
                                if (listesTemp.size()>1)
                                {
                                    etages_commun=vectorGetCommun(listes[listesTemp[listesTemp.size()-1]].getEtages(), listes[listesTemp[listesTemp.size()-2]].getEtages())[0];
                                }
                                // On récupère l'étage qu'il y a en commun entre les 2 ascenseurs qu'on a eu grâce à getPath
                                // Puis on gère comme d'habitude l'ascenseur....
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
                                    if (best_index==-1 || best_indice>listes[listesTemp[listesTemp.size()-1]].getDiffEtages(this->listes[i].getCurrentEtage(), etages_commun))
                                    {
                                        if (listes[listesTemp[listesTemp.size()-1]].getNbrPersonnes()>0)
                                        {
                                            temp_etages=listes[listesTemp[listesTemp.size()-1]].seekEtagesAim(this->listes);
                                            if ((temp_etages>listes[listesTemp[listesTemp.size()-1]].getCurrentEtages() && temp_etages>=this->listes[i].getCurrentEtage() && this->listes[i].getCurrentEtage()>=listes[listesTemp[listesTemp.size()-1]].getCurrentEtages()) ||
                                                    (temp_etages<listes[listesTemp[listesTemp.size()-1]].getCurrentEtages() && temp_etages<=this->listes[i].getCurrentEtage() && this->listes[i].getCurrentEtage()<=listes[listesTemp[listesTemp.size()-1]].getCurrentEtages())) /// L'ascenseur va descendre
                                            {
                                                if (((listes[listesTemp[listesTemp.size()-1]].getNbrPersonnes()!=0 && listes[listesTemp[listesTemp.size()-1]].getVitesse()) || !listes[listesTemp[listesTemp.size()-1]].getHadToWait()) && this->listes[i].getCurrentEtage()!=listes[listesTemp[listesTemp.size()-1]].getCurrentEtages()) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
                                                {
                                                    best_index=listesTemp[listesTemp.size()-1];
                                                    best_indice=listes[listesTemp[listesTemp.size()-1]].getDiffEtages(this->listes[i].getCurrentEtage(), etages_commun);
                                                }
                                            }
                                        }
                                        else if (!listes[listesTemp[listesTemp.size()-1]].getHadToWait() && this->listes[i].getCurrentEtage()!=listes[listesTemp[listesTemp.size()-1]].getCurrentEtages() &&
                                                 (!listes[listesTemp[listesTemp.size()-1]].getVitesse() ||
                                                  (listes[listesTemp[listesTemp.size()-1]].getEtagesAim()>=listes[listesTemp[listesTemp.size()-1]].getCurrentEtages() && this->listes[i].getCurrentEtage()>=listes[listesTemp[listesTemp.size()-1]].getEtagesAim())
                                                  || (listes[listesTemp[listesTemp.size()-1]].getEtagesAim()<=listes[listesTemp[listesTemp.size()-1]].getCurrentEtages() && this->listes[i].getCurrentEtage()<=listes[listesTemp[listesTemp.size()-1]].getEtagesAim()))) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
                                        {
                                            best_index=listesTemp[listesTemp.size()-1];
                                            best_indice=listes[listesTemp[listesTemp.size()-1]].getDiffEtages(this->listes[i].getCurrentEtage(), etages_commun);
                                        }
                                    }
                                }
                            }
                            listesTemp.clear(); // On vide notre vecteur de getPath
                        }
                        if (!find && best_index!=-1)
                        {
                            vector<int> subList;
                            subList.push_back(best_index);
                            subList.push_back(i);
                            listesAscenseurMoved.push_back(subList);
                        }
                    }
                    listesTempT.clear();
                    best_index=-1;
                    find=false;
                }
            }
        }
    }
    for (size_t i=0;i<listesAscenseurMoved.size();i++)
    {
        int idAsc=listesAscenseurMoved[i][0];
        int idPeople=listesAscenseurMoved[i][1];
        if (listes[idAsc].getNbrPersonnes()>0)
        {
            temp_etages=listes[idAsc].seekEtagesAim(this->listes);
            if ((temp_etages>listes[idAsc].getCurrentEtages() && temp_etages>=this->listes[idPeople].getCurrentEtage() && this->listes[idPeople].getCurrentEtage()>=listes[idAsc].getCurrentEtages()) ||
                    (temp_etages<listes[idAsc].getCurrentEtages() && temp_etages<=this->listes[idPeople].getCurrentEtage() && this->listes[idPeople].getCurrentEtage()<=listes[idAsc].getCurrentEtages())) /// L'ascenseur va descendre
            {
                if (((listes[idAsc].getNbrPersonnes()!=0 && listes[idAsc].getVitesse()) || !listes[idAsc].getHadToWait()) && this->listes[idPeople].getCurrentEtage()!=listes[idAsc].getCurrentEtages()) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
                {
                    listes[idAsc].setEtagesAim(this->listes[idPeople].getCurrentEtage());
                }
            }
        }
        else if (!listes[idAsc].getHadToWait() && this->listes[idPeople].getCurrentEtage()!=listes[idAsc].getCurrentEtages() &&
                 (!listes[idAsc].getVitesse() ||
                  (listes[idAsc].getEtagesAim()>=listes[idAsc].getCurrentEtages() && this->listes[idPeople].getCurrentEtage()>=listes[idAsc].getEtagesAim())
                  || (listes[idAsc].getEtagesAim()<=listes[idAsc].getCurrentEtages() && this->listes[idPeople].getCurrentEtage()<=listes[idAsc].getEtagesAim()))) /// Il est en mouvement Ou immobile mais n'attends personne, on lui indique un nouvel arrêt
        {
            listes[idAsc].setEtagesAim(this->listes[idPeople].getCurrentEtage());
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

/**** Retourne true si un chemin a été trouvé
Les arguments sont un vector d'ascenseurs, l'étage qu'on souhaite atteindre, l'étage d'où l'on commence et un vector d'int vide
Le vector est rangé dans l'ordre inverse, exemple :
Si l'on souhaite allez à l'étage 10, et que pour cela nous devons prendre l'ascenseur 0 -> 1 -> 3, le vector sera : 3 -> 1 -> 0
Il faut donc utiliser v.size()-1 et v.size()-2
****/
bool getPath(vector<Ascenseur>& listes, int etages_aim, int etages_start, vector< vector<int> >& v)
{
    if (etages_aim==etages_start)
        return false;

    bool returnV=false;
    vector<int> tempBoth;
    vector<int> tempOne;
    for (size_t i=0;i<listes.size();i++)
    {
        if (vectorHave(etages_aim, listes[i].getEtages()))
        {
            if (vectorHave(etages_start, listes[i].getEtages()))
            {
                tempBoth.push_back(i); // tempBoth on ajoute tempBoth à v
                v.push_back(tempBoth);
                returnV=true;
                tempBoth.clear();
            }
            else
            {
                tempOne.push_back(i);
            }
        }
    }
    if (returnV)
        return true;

    vector<unsigned int> listesEtages;
    vector<int> final;
    vector<int> addV;
    bool etat;
    for (size_t i=0;i<tempOne.size();i++)
    {
        listesEtages=listes[tempOne[i]].getEtages();
        for (size_t j=0;j<listesEtages.size();j++)
        {
            if (listesEtages[j]!=etages_aim)
            {
                etat=getPathSub(listes, listesEtages[j], etages_start, final);
                if (etat)
                {
                    addV.push_back(tempOne[i]);
                    for (size_t w=0;w<final.size();w++)
                    {
                        addV.push_back(final[w]);
                    }
                    returnV=true;
                    v.push_back(addV);
                    addV.clear();
                }
                final.clear();
            }
        }
    }
    return returnV;
}

bool getPathSub(vector<Ascenseur>& listes, int etages_aim, int etages_start, vector<int>& v)
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
                    v.push_back(i); // tempBoth on ajoute tempBoth à v
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
                etat=getPathSub(listes, listesEtages[j], etages_start, final);
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
