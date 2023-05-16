#ifndef _PILOTAGE_H_
#define _PILOTAGE_H_

#include "include/utils.h"


// Fonction qui calcule l'angle de la voile
// [IN] float VentMatAvant : angle qui donne la provenance du vent par rapport au bateau (180 = vente arrière)
// [OUT] float cmd_voile : commande d'angle à adopter à la voile
float calcul_angle_voile(float VentMatAvant);

// Fonction qui calcule l'angle du safran
// [IN] struc point waypoint : coordonnées de la bouée 
// [IN] struc point bateau : les coordonnées du bateau 
// [IN] float bateau_orientation : orientation du bateau en degrés par rapport au nord
// [OUT] float cmd_safran : commande d'angle à adopter au safran
float calcul_angle_safran(struct point waypoint, struct point bateau, float bateau_orientation);

// Fonction qui assure le pilotage de la voile et du safran du bateau
// [IN] struct point Waypoint : ojectif a atteindre
// [IN] struct point bateau : coordonées du bateau
// [IN] float VMA : angle entre du vent par rapport au bateau
// [IN] float NMA : orientation du bateau par rapport au nord
// [OUT] struct cmd pilote : structure qui contient les commandes pour la voile et le safran
struct cmd pilotage(struct point waypoint, struct point bateau, float VMA, float NMA);

#endif