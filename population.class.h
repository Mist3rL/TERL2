#ifndef POPULATION_CLASS_H
#define POPULATION_CLASS_H

#include <iostream>
#include <cstdio>
#include <vector>
#include <math.h>
#include <fstream>
#include <string>
#include "personne.class.h"
#include "ascenseur.class.h"
#include "config.h"
class Population{
    private:
        Personne *listes;
    public:
        Population(size_t, int, std::string);
        ~Population();
        Personne* getListe();
        void gerer(int, std::vector<Ascenseur>&, LoaderObject&); /// Donnes des "ordres" à la population, et aux ascenseurs
        void save();
};
void gererPersonnesAscenseur(Ascenseur&, Population&, LoaderObject&, int);
void gererAscenseurs(std::vector<Ascenseur>&, Population&); /// Permet de donner des ordres aux ascenseurs pour savoir s'ils doivent monter ou descendre (en fonction de leur passagers)
bool getPath(std::vector<Ascenseur>&, int, int, std::vector<int>&);

#endif // POPULATION_CLASS_H
