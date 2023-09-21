#include "character.h"
#include <limits.h>
#include <stdlib.h>

Trainer* create_pc(int x, int y) {
    Trainer* pc = (Trainer*)malloc(sizeof(Trainer));
    pc->x = x;
    pc->y = y;
    pc->bldr = INT_MAX;
    pc->tree = INT_MAX;
    pc->path = 10;
    pc->pmart = 10;
    pc->pcntr = 10;
    pc->tgrass = 20;
    pc->sgrass = 10;
    pc->mtn = INT_MAX;
    pc->forest = INT_MAX;
    pc->water = INT_MAX;
    pc->gate = 10;

    return pc;
}

Trainer* create_hiker(int x, int y){
    Trainer* hiker = (Trainer*)malloc(sizeof(Trainer));
    hiker->x = x;
    hiker->y = y;
    hiker->bldr = INT_MAX;
    hiker->tree = INT_MAX;
    hiker->path = 10;
    hiker->pmart = 50;
    hiker->pcntr = 50;
    hiker->tgrass = 15;
    hiker->sgrass = 10;
    hiker->mtn = 15;
    hiker->forest = 15;
    hiker->water = INT_MAX;
    hiker->gate = INT_MAX;

    return hiker;
}

Trainer* create_rival(int x, int y){
    Trainer* rival = (Trainer*)malloc(sizeof(Trainer));
    rival->x = x;
    rival->y = y;
    rival->bldr = INT_MAX;
    rival->tree = INT_MAX;
    rival->path = 10;
    rival->pmart = 50;
    rival->pcntr = 50;
    rival->tgrass = 20;
    rival->sgrass = 10;
    rival->mtn = INT_MAX;
    rival->forest = INT_MAX;
    rival->water = INT_MAX;
    rival->gate = INT_MAX;

    return rival;
}

Trainer* create_swimmer(int x, int y){
    Trainer* swimmer = (Trainer*)malloc(sizeof(Trainer));
    swimmer->x = x;
    swimmer->y = y;
    swimmer->bldr = INT_MAX;
    swimmer->tree = INT_MAX;
    swimmer->path = INT_MAX;
    swimmer->pmart = INT_MAX;
    swimmer->pcntr = INT_MAX;
    swimmer->tgrass = INT_MAX;
    swimmer->sgrass = INT_MAX;
    swimmer->mtn = INT_MAX;
    swimmer->forest = INT_MAX;
    swimmer->water = 7;
    swimmer->gate = INT_MAX;

    return swimmer;
}

Trainer* create_other(int x, int y){
    Trainer* other = (Trainer*)malloc(sizeof(Trainer));
    other->x = x;
    other->y = y;
    other->bldr = INT_MAX;
    other->tree = INT_MAX;
    other->path = 10;
    other->pmart = 50;
    other->pcntr = 50;
    other->tgrass = 20;
    other->sgrass = 10;
    other->mtn = INT_MAX;
    other->forest = INT_MAX;
    other->water = INT_MAX;
    other->gate = INT_MAX;

    return other;
}