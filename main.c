#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>


// Constantes pour le PID du Safran
#define Kp 1.0 // coefficient proportionnel
#define Ki 0.1 // coefficient intégral
#define Kd 0.01 // coefficient dérivé
#define OUTPUT_MIN -30.0
#define OUTPUT_MAX 30.0

float pid_error = 0.0; // erreur cumulée pour le PID
float pid_last_error = 0.0; // dernière erreur pour le PID

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
float calcul_norme_vecteur(struct vecteur V)
{
    float norme = ((V.x)*(V.x)) + ((V.y)*(V.y));
    norme = sqrtf(norme);
    return norme;
}

// Fonction qui calcule la commande du PID pour une erreur donnée en entrée. 
// Les coefficients proportionnel, intégral et dérivé sont définis en tant que constantes.
float pid(float input, float setpoint)
{
    float error = setpoint - input;
    pid_error += error;
    float d_error = error - pid_last_error;
    float output = Kp * error + Ki * pid_error + Kd * d_error;
    pid_last_error = error;

    return output;
}


// Fonction qui calcule l'angle du safran à l'aide des paramètres suivants :
// struc point waypoint : coordonnées de la bouée 
// struc point bateau : les coordonnées du bateau 
// float bateau_orientation : orientation du bateau en degrés par rapport au nord
float calcul_angle_safran(struct point waypoint, struct point bateau, float bateau_orientation)
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

    // printf("NordMatBouee = %f\n", NordMatBouee);
    float AvantMatWaypoint = NordMatWaypoint - bateau_orientation;
    printf("angle_bateau_waypoint = %f\n", AvantMatWaypoint);

    // Calcul des écarts en x et en y entre la bouée et le bateau
    float delta_x = waypoint.x - bateau.x;
    float delta_y = waypoint.y - bateau.y;

    // Calcul de l'angle entre le bateau et la bouée
    float angle_bateau_waypoint = atan2(delta_y, delta_x) * 180.0 / M_PI; // convertir en degrés
    float erreur_angle = angle_bateau_waypoint - bateau_orientation;

    printf("angle_bateau_waypoint = %f\n", angle_bateau_waypoint);
    printf("erreur_angle = %f\n", erreur_angle);

    // Utilisation du PID
    float angle_safran = pid(erreur_angle, 0.0); // setpoint = 0.0 pour réguler l'erreur à zéro

    return angle_safran;
}



// Fonction qui caclule la décision de stratégie, entre aller tout droit ou tirer des bords
// struct point Bouee : coordonnées de la bouée à viser
// struct point Mat : coordonées du bateau
// float VMA : angle entre le vent, le mat et l'avant du bateau, donné par la girouette 
// float NMA : angle entre le nord, le mat et l'avant du bateau, donné par l'IMU
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
struct cmd pilotage(struct point waypoint, struct point bateau, float VMA, float NMA)
{
    struct cmd pilote;
    pilote.voile = 0;
    pilote.safran = 0;

    //Commande de la voile
    float Vent_Voile;
    if(VMA > 180)
    {
        Vent_Voile = 360 - VMA;
    }
    else 
    {
        Vent_Voile = VMA;
    }

    if(VMA >= 150)
    {
        pilote.voile = 90;
    }
    else if(VMA >= 55 && VMA < 150)
    {
        pilote.voile = (15/19) * VMA -(540/19);
    }
    else if(VMA >= 15 && VMA < 55)
    {
        pilote.voile = 15;
    }
    else if(VMA >= 0 && VMA < 15)
    {
        pilote.voile = VMA;
    }

    //Commande du safran entre -30° et +30°
    pilote.safran = calcul_angle_safran(waypoint, bateau, NMA);

    return pilote;
}

int main(void)
{
    printf("*******************************************************\n");
    printf("*** Tests de l'agorithme de navigation              ***\n");
    printf("*** Auteurs : Arzhel PUILLANDRE et Hugo COLLINEAU   ***\n");
    printf("*******************************************************\n\n");

    // Positions de l'objectif et du bateau 
    struct point Bouee = {-20, 20};
    struct point Mat = {0, 0};

    //Les angles sont donnés sur [0, 360]
    float VentMatAvant = 60.26;
    float NordMatAvant = 326.54;



    // // Test de la fonction decision_strategie
    // printf("**********************************************\n");
    // printf("*** Test de la fonction decision_strategie ***\n");
    // printf("**********************************************\n\n");

    // int strategie = decision_strategie(Bouee, Mat, VentMatAvant, NordMatAvant);

    // switch (strategie)
    // {
    // case 0:
    //     printf("decision_strategie : failed\n");

    //     break;
    // case 1:
    //     printf("decision_strategie : tirer des bords\n");
    //     break;
    
    // case 2:
    //     printf("decision_strategie : nominal\n");
    //     break;
    
    // default:
    //     printf("decision_strategie : valeur de retour inconnue\n");
    //     break;
    // }

    // printf("\n\n");

    // Test de la fonction pilotage
    printf("************************************\n");
    printf("*** Test de la fonction pilotage ***\n");
    printf("************************************\n\n");
  
    // Positions de l'objectif et du bateau 
    Bouee.x= 0;
    Bouee.y = 20;
    
    Mat.x= 0;
    Mat.y = 0;

    //Les angles sont donnés sur [0, 360]
    VentMatAvant = 180;
    NordMatAvant = 0;

    struct cmd pilotage_bateau = pilotage(Bouee, Mat, VentMatAvant, NordMatAvant);

    printf("L'angle de voile a adopter est : %f\n", pilotage_bateau.voile);
    printf("L'angle de safran a adopter est : %f\n", pilotage_bateau.safran);

    printf("\n\n");

}