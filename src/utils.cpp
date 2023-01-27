#include "include/utils.h"

float calcul_norme_vecteur(struct vecteur V)
{
    float norme = ((V.x)*(V.x)) + ((V.y)*(V.y));
    norme = sqrtf(norme);
    return norme;
}
