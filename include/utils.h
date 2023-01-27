#ifndef _STRUCT_H_
#define _STRUCT_H_

#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

struct point{
    float x;
    float y;
};

struct vecteur{
    float x;
    float y;
};

struct cmd{
    float safran;
    float voile;
};

float calcul_norme_vecteur(struct vecteur V);


#endif