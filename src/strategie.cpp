#include "include/strategie.h"

int decision_strategie(struct point Bouee, struct point Mat, float VMA, float NMA)
{
    int retour = 0;

    struct vecteur MatNord = {0, 1};
    struct vecteur MatBouee;
    MatBouee.x = Bouee.x - Mat.x;
    MatBouee.y = Bouee.y - Mat.y;

    float NordMatBouee;
    float scalaire = (MatNord.x * MatBouee.x) + (MatNord.y * MatBouee.y);

    float normeMB = calcul_norme_vecteur(MatBouee);
    float normeMN = calcul_norme_vecteur(MatNord);

    NordMatBouee = scalaire/(normeMB * normeMN);
    NordMatBouee = acosf(NordMatBouee)*180/M_PI;

    // printf("NordMatBouee = %f\n", NordMatBouee);
    float VentMatBouee;
    VentMatBouee = VMA - (NordMatBouee - NMA);

    if(VentMatBouee > 360)
    {
        VentMatBouee = VentMatBouee - 360;
    }

    // printf("VentMatBouee = %f\n", VentMatBouee);
    if(VentMatBouee < 30 || VentMatBouee > 330)
    {
        retour = 1;
    }
    else if(VentMatBouee >=30 && VentMatBouee <=330)
    {
        retour = 2;
    }

    return retour;
}