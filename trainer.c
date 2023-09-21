#include "trainer.h"
#include <limits.h>
#include <stdlib.h>

Trainer* create_pc() {
    Trainer* pc = (Trainer*)malloc(sizeof(Trainer));
    pc->bldr = INT_MAX;
    pc->tree = INT_MAX;
    pc->path = 10;
    pc->pmart = 10;
    pc->pcntr = 10;
    pc->tgrass = 20;
    pc->sgrass = 10;
    pc->mtn = INT_MAX;
    pc->forrest = INT_MAX;
    pc->water = INT_MAX;
    pc->gate = 10;

    return pc;
}

Trainer* create_hiker(){
    Trainer* hiker = (Trainer*)malloc(sizeof(Trainer));
    hiker->bldr = INT_MAX;
    hiker->tree = INT_MAX;
    hiker->path = 10;
    hiker->pmart = 50;
    hiker->pcntr = 50;
    hiker->tgrass = 15;
    hiker->sgrass = 10;
    hiker->mtn = 15;
    hiker->forrest = 15;
    hiker->water = INT_MAX;
    hiker->gate = INT_MAX;

    return hiker;
}

Trainer* create_rival(){
    Trainer* rival = (Trainer*)malloc(sizeof(Trainer));
    rival->bldr = INT_MAX;
    rival->tree = INT_MAX;
    rival->path = 10;
    rival->pmart = 50;
    rival->pcntr = 50;
    rival->tgrass = 20;
    rival->sgrass = 10;
    rival->mtn = INT_MAX;
    rival->forrest = INT_MAX;
    rival->water = INT_MAX;
    rival->gate = INT_MAX;

    return rival;
}

Trainer* create_swimmer(){
    Trainer* swimmer = (Trainer*)malloc(sizeof(Trainer));
    swimmer->bldr = INT_MAX;
    swimmer->tree = INT_MAX;
    swimmer->path = INT_MAX;
    swimmer->pmart = INT_MAX;
    swimmer->pcntr = INT_MAX;
    swimmer->tgrass = INT_MAX;
    swimmer->sgrass = INT_MAX;
    swimmer->mtn = INT_MAX;
    swimmer->forrest = INT_MAX;
    swimmer->water = 7;
    swimmer->gate = INT_MAX;

    return swimmer;
}

Trainer* create_other(){
    Trainer* other = (Trainer*)malloc(sizeof(Trainer));
    other->bldr = INT_MAX;
    other->tree = INT_MAX;
    other->path = 10;
    other->pmart = 50;
    other->pcntr = 50;
    other->tgrass = 20;
    other->sgrass = 10;
    other->mtn = INT_MAX;
    other->forrest = INT_MAX;
    other->water = INT_MAX;
    other->gate = INT_MAX;

    return other;
}