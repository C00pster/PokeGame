#ifndef TRAINER_H
#define TRAINER_H

typedef enum {
    PC,
    HIKER,
    RIVAL,
    SWIMMER,
    OTHER,
    NUM_TRAINERS
} TrainerType;

typedef struct {
    int* weights;
    int x, y;
    TrainerType type;
} Trainer;

Trainer* create_trainer(TrainerType type, int x, int y);
int* get_trainer_weights(TrainerType type, int* result);
void free_trainer(Trainer* trainer);

#endif // TRAINER_H