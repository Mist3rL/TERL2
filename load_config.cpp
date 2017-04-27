#include "load_config.h"

using namespace std;

void loadConfig(vector<string>& contenu_file, Immeuble& immeuble, vector<Ascenseur>& ascenseurs)
{
    string etages;
    for (size_t i=0;i<contenu_file.size();i++)
    {
        ascenseurs.push_back(i+1);
        istringstream  iss(contenu_file[i]);
        while (getline(iss, etages, ' '))
        {
            immeuble.getListesEtages()[atoi(etages.c_str())].addAscenseur(i+1, ascenseurs);
        }
    }
}
