#ifndef SIMULATION_H
#define SIMULATION_H

#include <cstdlib>
#include <iostream>
#include <time.h>
#include "config.h"
#include "loader.class.h"
#include "gui.h"
#include "moteur.h"
#include "population.class.h"
#include "souris.struct.h"
#include "load_config.h"
#include "graphique.h"

void simulation(LoaderObject&, Immeuble&, std::vector<Ascenseur>&, Population&);

#endif // SIMULATION_H
