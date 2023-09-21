#ifndef TRAINER_H
#define TRAINER_H

typedef struct {
    int bldr, tree, path, pmart, pcntr, tgrass, sgrass, mtn, forrest, water, gate;
} Trainer;

Trainer *create_pc();
Trainer *create_hiker();
Trainer *create_rival();
Trainer *create_swimmer();
Trainer *create_other();

#endif // TRAINER_H