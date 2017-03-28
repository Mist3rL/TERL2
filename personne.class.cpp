#include "personne.class.h"
using namespace std;
unsigned int Personne::countPersonnes=0;
float Personne::minTime=-1;
float Personne::maxTime=-1;

Personne::Personne()
{
    aim_etage=-1;
    nombreVoyages=0;
    attente=0;
    attente_total=0;
    ascenseur_id=-1;
    isHome=false;
    current_etage=-1;
    waitElevators=false;
    busy=false;
    posX=0;
    vitesse=0;
}
void Personne::init(int nombres_etages)
{
    vector<int> temps;
    temps.push_back(4820);temps.push_back(3630); temps.push_back(4520); temps.push_back(4220); temps.push_back(4370); temps.push_back(4280);
    temps.push_back(4400); temps.push_back(5000); temps.push_back(3640); temps.push_back(4320);
    random_shuffle(temps.begin(), temps.end()); /// On mélange le vecteur à propos du temps
    countPersonnes++;
    id=countPersonnes;
    visible=false;
    posX=0;
    posY=0;
    time_arrive=rand()%(TEMPS_ARRIVE_SECONDE-2)+2;
    int nombres_destination_up=rand()%2+3;
    vector<int> etages;
    int temp=0;
    for (int i=0;i<nombres_destination_up;i++)
    {
        do{
            temp=rand()%(nombres_etages-1)+1;
        }while(vectorHave(temp, etages));
        etages.push_back(temp);
    }
    sort(etages.begin(), etages.end());
    for (size_t i=0;i<etages.size();i++)
    {
        listes_montant.push_back(Decision(etages[i], temps.back()));
        temps.pop_back();
    }
    int nombres_destination_down=10-nombres_destination_up;
    etages.clear();
    for (int i=0;i<nombres_destination_down;i++)
    {
        do{
            temp=rand()%(nombres_etages-1)+1; /// On ne veut pas que cela amène à l'étage 0
        }while(vectorHave(temp, etages) || temp==listes_montant[listes_montant.size()-1].getEtages()); /// On ne veut pas qu'il descende à l'étage où il est 8->9->9-7 par exemple
        etages.push_back(temp);
    }
    sort(etages.begin(), etages.end(), greater<int>());
    for (size_t i=0;i<etages.size();i++)
    {
        listes_descendant.push_back(Decision(etages[i], temps.back()));
        temps.pop_back();
    }
    listes_descendant.push_back(Decision(0, 99999));
}
void Personne::init(string contenu)
{
    countPersonnes++;
    id=countPersonnes;
    visible=false;
    posX=0;
    posY=0;
    istringstream iss(contenu);
    string decisions;
    string block;
    bool montant;
    int last;
    bool time_arriveFile=false;
    while (getline(iss, decisions, ' '))
    {
        if (!time_arriveFile)
        {
            time_arrive=atoi(decisions.c_str());
            time_arriveFile=true;
        }
        else
        {
            montant=true;
            last=-1;
            istringstream  iss2(decisions);
            getline(iss2, block, ':');
            if (atoi(block.c_str())<last)
                montant=false;

            last=atoi(block.c_str());
            getline(iss2, block, ':');
            if (montant)
            {
                listes_montant.push_back(Decision(last, atoi(block.c_str())));
            }
            else
            {
                listes_descendant.push_back(Decision(last, atoi(block.c_str())));
            }
        }
    }
}
Personne::~Personne()
{

}
vector<int> Personne::getListesEtages()
{
    vector<int> listes;
    for (size_t i=0;i<listes_montant.size();i++)
    {
        listes.push_back(listes_montant[i].getEtages());
    }
    for (size_t i=0;i<listes_descendant.size();i++)
    {
        listes.push_back(listes_descendant[i].getEtages());
    }
    return listes;
}
int Personne::getNextEtages()
{
    if (listes_montant.size()>0)
    {
        return listes_montant[0].getEtages();
    }
    else if (listes_descendant.size()>0)
    {
        return listes_descendant[0].getEtages();
    }
    return 0;
}
void Personne::setPosition(int x, int y)
{
    posX=x;
    posY=y;
}
void Personne::setVisible(bool v)
{
    visible=v;
}
bool vectorHave(int val, vector<int> vec)
{
    for (size_t i=0;i<vec.size();i++)
    {
        if (vec[i]==val)
            return true;
    }
    return false;
}
bool vectorHave(int val, vector<unsigned int> vec)
{
    for (size_t i=0;i<vec.size();i++)
    {
        if (vec[i]==(unsigned int)val)
            return true;
    }
    return false;
}
vector<int> vectorGetCommun(vector<unsigned int> a, vector<unsigned int> b)
{
    vector<int> c;
    for (size_t i=0;i<a.size();i++)
    {
        for (size_t j=0;j<b.size();j++)
        {
            if (a[i]==b[j])
            {
                c.push_back(a[i]);
            }
        }
    }
    return c;
}
void Personne::setVitesse(float v)
{
    vitesse=v;
    if (v<0)
    {
        cols_aim=-1;
    }
}
bool Personne::getVisible()
{
    return visible;
}
float Personne::getVitesse()
{
    return vitesse;
}
int Personne::getPositionX()
{
    return posX;
}
int Personne::getPositionY()
{
    return posY;
}
void Personne::setColsAim(int c)
{
    cols_aim=c;
}
int Personne::getColsAim()
{
    return cols_aim;
}
float Personne::getTimeArrive()
{
    return time_arrive;
}
void Personne::setTimeArrive(float t)
{
    time_arrive=t;
}
void Personne::setBusy(bool b)
{
    busy=b;
}
bool Personne::getBusy()
{
    return busy;
}
void Personne::setWaitElevators(bool wE)
{
    waitElevators=wE;
}
bool Personne::getWaitElevators()
{
    return waitElevators;
}
Decision& Personne::getNextDecision()
{
    if (listes_montant.size()>0)
        return listes_montant[0];

    return listes_descendant[0];
}
void Personne::removeDecision()
{
    if (listes_montant.size()>0)
    {
        listes_montant.erase(listes_montant.begin());
    }
    else
    {
        listes_descendant.erase(listes_descendant.begin());
    }
}
vector<Decision> Personne::getAllDecisions()
{
    vector<Decision> temp;
    for (size_t i=0;i<listes_montant.size();i++)
        temp.push_back(listes_montant[i]);

    for (size_t i=0;i<listes_descendant.size();i++)
        temp.push_back(listes_descendant[i]);

    return temp;
}
void Personne::setCurrentEtage(int ce)
{
    current_etage=ce;
}
int Personne::getCurrentEtage()
{
    return current_etage;
}
bool Personne::getIsHome()
{
    return isHome;
}
void Personne::setIsHome(bool ih)
{
    isHome=ih;
}
int Personne::getAscenseurId()
{
    return ascenseur_id;
}
void Personne::setAscenseur(int id)
{
    ascenseur_id=id;
}
float Personne::getTempsAttente()
{
    return attente;
}
void Personne::setTempsAttente(float ta)
{
    if (ta==0)
    {
        attente_total+=attente;
        nombreVoyages++;
        if (minTime==-1 || minTime>attente)
        {
            minTime=attente;
        }
        if (maxTime<attente)
        {
            maxTime=attente;
        }
    }

    attente=ta;
}
unsigned int Personne::getNombreVoyages()
{
    return nombreVoyages;
}
float Personne::getTempsTotal()
{
    return attente_total;
}
void Personne::setAimEtage(int ae)
{
    aim_etage=ae;
}
int Personne::getAimEtage()
{
    return aim_etage;
}
