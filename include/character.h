#ifndef TRAINER_H
#define TRAINER_H

typedef struct {
    int bldr, tree, path, pmart, pcntr, tgrass, sgrass, mtn, forest, water, gate;
    int x, y;
} Trainer;

typedef enum {
    NONE,
    PC,
    HIKER,
    RIVAL,
    SWIMMER,
    OTHER
} TrainerType;

Trainer* create_pc();
Trainer* create_hiker();
Trainer* create_rival();
Trainer* create_swimmer();
Trainer* create_other();

#endif // TRAINER_H