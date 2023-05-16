#include "include/utils.h"

// Fonction qui calcule la norme d'un vecteur
float calcul_norme_vecteur(struct vecteur V)
{
    float norme = ((V.x)*(V.x)) + ((V.y)*(V.y));
    norme = sqrtf(norme);
    return norme;
}


// Fonction qui calcule l'angle entre le Nord, le bateau et la bouée :
// [IN] struc point waypoint : coordonnées de la bouée 
// [IN] struc point bateau : les coordonnées du bateau 
// [OUT] float NordMatWaypoint : angle Nord Bateau Bouée
float calcul_angle_NordMatWaypoint(struct point waypoint, struct point bateau)
{
    struct vecteur MatNord = {0, 1};
    struct vecteur MatWaypoint;
    MatWaypoint.x = waypoint.x - bateau.x;
    MatWaypoint.y = waypoint.y - bateau.y;

    float NordMatWaypoint;
    float scalaire = (MatNord.x * MatWaypoint.x) + (MatNord.y * MatWaypoint.y);

    float normeMB = calcul_norme_vecteur(MatWaypoint);
    float normeMN = calcul_norme_vecteur(MatNord);

    NordMatWaypoint = scalaire/(normeMB * normeMN);
    NordMatWaypoint = acosf(NordMatWaypoint)*180/M_PI;

    float produit_vectoriel = (MatNord.x * MatWaypoint.y) - (MatNord.y * MatWaypoint.x);

    if(produit_vectoriel < 0)
    {
        NordMatWaypoint = 360 - NordMatWaypoint;
    }

    // printf("NordMatWaypoint = %f\n", NordMatWaypoint);

    return NordMatWaypoint;
}