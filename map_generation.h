#include <stdint.h>
#include <stdlib.h>

#ifndef MAP_GENERATION_H
#define MAP_GENERATION_H

typedef struct {
    char** map;
    __int8_t top_path, bottom_path, left_path, right_path;
} Map;

Map* generate_map(__int8_t top_path, __int8_t bottom_path, __int8_t left_path, __int8_t right_path);

#endif