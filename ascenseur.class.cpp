#include "ascenseur.class.h"
using namespace std;
unsigned int Ascenseur::countAscenseur=0;

Ascenseur::Ascenseur()
{
    ++countAscenseur;
    id=countAscenseur;
    current_etages=-1;
    cols=0;
    vitesse=0;
    positionY=0;
    etagesAim=0;
    hadToWait=0;
}
Ascenseur::Ascenseur(unsigned int c)
{
    ++countAscenseur;
    id=countAscenseur;
    current_etages=-1;
    cols=c;
    vitesse=0;
    positionY=0;
    etagesAim=0;
    hadToWait=0;
}
Ascenseur::~Ascenseur(){ }
void Ascenseur::setCurrentEtages(unsigned int current)
{
    current_etages=current;
}
void Ascenseur::setCols(unsigned int c)
{
    cols=c;
}
unsigned int Ascenseur::getCurrentEtages()
{
    return current_etages;
}
unsigned int Ascenseur::getCols()
{
    return cols;
}

unsigned int Ascenseur::getNbrPersonnes()
{
    return personnes.size();
}

void Ascenseur::addPersonnes(int id)
{
    personnes.push_back(id);
}

bool Ascenseur::canAcceptPeople()
{
    if (getNbrPersonnes()+hadToWait<NOMBRE_PERSONNES_MAX)
        return true;

    return false;
}

int Ascenseur::getVitesse()
{
    return vitesse;
}

void Ascenseur::setVitesse(int direction)
{
    switch (direction)
    {
        case TOP:
            vitesse=-1*VITESSE_TOP;
        break;
        case BOTTOM:
            vitesse=VITESSE_BOTTOM;
        break;
        default:
            vitesse=0;
        break;
    }
}

int Ascenseur::getPosition()
{
    return positionY;
}

void Ascenseur::setPosition(int pos)
{
    positionY=pos;
}

int Ascenseur::getEtagesAim()
{
    return etagesAim;
}
void Ascenseur::setEtagesAim(int etage)
{
    etagesAim=etage;
    if (etagesAim!=(int)current_etages)
    {
        if (etagesAim>(int)current_etages)
            setVitesse(TOP);
        else
            setVitesse(BOTTOM);
    }
}
vector<unsigned int> Ascenseur::getPersonnes()
{
    return personnes;
}
bool Ascenseur::getHadToWait()
{
    if (hadToWait==0)
        return false;

    return true;
}
void Ascenseur::increaseWait()
{
    hadToWait++;
}
void Ascenseur::decreaseWait()
{
    hadToWait--;
}
void Ascenseur::addEtages(unsigned int etage)
{
    etages.push_back(etage);
    sort(etages.begin(), etages.end()); /// on tri
}
vector<unsigned int> Ascenseur::getEtages()
{
    return etages;
}
void Ascenseur::removePersonnes(unsigned int id)
{
    vector<unsigned int> temp;
    for (size_t i=0;i<personnes.size();i++)
    {
        if (personnes[i]!=id)
        {
            temp.push_back(personnes[i]);
        }
    }
    personnes=temp;
}

int Ascenseur::seekEtagesAim(Personne *liste)
{
    vector<int> listes_etages;
    /// On parcours la liste des destinations des personnes
    for (size_t j=0;j<getNbrPersonnes();j++)
    {
        if (liste[getPersonnes()[j]].getAimEtage()!=-1)
            listes_etages.push_back(liste[getPersonnes()[j]].getAimEtage());
        else
            listes_etages.push_back(liste[getPersonnes()[j]].getNextEtages());
    }
    sort(listes_etages.begin(), listes_etages.end()); /// on tri
    if ((int)getCurrentEtages()<listes_etages[0]) /// On doit donc monter
    {
        return listes_etages[0]; /// et on prend donc la première destination en croissant
    }
    return listes_etages[listes_etages.size()-1]; /// inversement
}

int Ascenseur::getDiffEtages(int etagesActuel, int etagesVisee)
{
    int diff=0;
    size_t indexEtagesNow=-1, indexEtagesVisee=-1, indexEtagesPersonnes=-1;
    for (size_t i=0;i<etages.size() && (indexEtagesNow==-1 || indexEtagesVisee==-1 || indexEtagesPersonnes==-1);i++)
    {
        if (etages[i]==etagesActuel)
            indexEtagesPersonnes=i;
        else if (etages[i]==etagesVisee)
            indexEtagesVisee=i;
        else if (etages[i]==current_etages)
            indexEtagesNow=i;
    }
    diff=abs((int)(indexEtagesNow-indexEtagesPersonnes))+abs((int)(indexEtagesVisee-indexEtagesNow));
    return diff;
}
int Ascenseur::getHadToWaitInt()
{
    return hadToWait;
}
