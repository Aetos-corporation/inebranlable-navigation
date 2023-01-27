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

float calcul_norme_vecteur(struct vecteur V)
{
    float norme = ((V.x)*(V.x)) + ((V.y)*(V.y));
    norme = sqrtf(norme);
    return norme;
}

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

struct point navigation(int statregie)
{

}
struct cmd pilotage(struct point waypoint, float Vent)
{
    struct cmd pilote;

    //Commande de la voile
    pilote.voile = 0;
    if(Vent > 180)
    {
        Vent = Vent - 180;
    }

    if(Vent >= 150)
    {
        pilote.voile = 90;
    }
    else if(Vent >= 55 && Vent < 150)
    {
        pilote.voile = (15/19) * Vent -(540/19);
    }
    else if(Vent >= 15 && Vent < 55)
    {
        pilote.voile = 15;
    }
    else if(Vent >= 0 && Vent < 15)
    {
        pilote.voile = Vent;
    }

    //Commande du safran
    pilote.safran = 0;




    

    return pilote;
}

int main(void)
{
    struct point Bouee = {-20, 20};
    struct point Mat = {0, 0};
    //Les angles sont donnés sur [0, 360]
    float VentMatAvant = 60.26;
    float NordMatAvant = 326.54;

    int strategie = decision_strategie(Bouee, Mat, VentMatAvant, NordMatAvant);

    switch (strategie)
    {
    case 0:
        printf("decision_strategie : failed\n");

        break;
    case 1:
        printf("decision_strategie : tirer des bords\n");
        break;
    
    case 2:
        printf("decision_strategie : nominal\n");
        break;
    
    default:
        printf("decision_strategie : valeur de retour inconnue\n");
        break;
    }
}