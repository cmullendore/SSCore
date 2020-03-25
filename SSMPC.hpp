// SSMPC.hpp
// SSCore
//
// Created by Tim DeBenedictis on 3/24/20.
// Copyright © 2020 Southern Stars. All rights reserved.
// Routines for importing asteroid and comet data from Minor Planet Center export files.

#ifndef SSMPC_hpp
#define SSMPC_hpp

#include "SSPlanet.hpp"

int importMPCComets ( const char *filename, SSObjectVec &comets );
int importMPCAsteroids ( const char *filename, SSObjectVec &comets );

#endif /* SSMPC_hpp */
