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

// Fonction qui calcule la norme d'un vecteur
float calcul_norme_vecteur(struct vecteur V);

// Fonction qui calcule l'angle entre le Nord, le bateau et la bouée :
// [IN] struc point waypoint : coordonnées de la bouée 
// [IN] struc point bateau : les coordonnées du bateau 
// [OUT] float NordMatWaypoint : angle Nord Bateau Bouée
float calcul_angle_NordMatWaypoint(struct point waypoint, struct point bateau);

#endif