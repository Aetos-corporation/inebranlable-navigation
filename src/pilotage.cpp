#include "include/pilotage.h"

// Fonction qui calcule l'angle de la voile
// [IN] float VentMatAvant : angle qui donne la provenance du vent par rapport au bateau (180 = vente arrière)
// [OUT] float cmd_voile : commande d'angle à adopter à la voile
float calcul_angle_voile(float VentMatAvant)
{
    float cmd_voile = 0;
    if(VentMatAvant > 180)
    {
        VentMatAvant = 360 - VentMatAvant;
    }
    else 
    {
        VentMatAvant = VentMatAvant;
    }

    if(VentMatAvant >= 150)
    {
        cmd_voile = 90;
    }
    else if(VentMatAvant >= 55 && VentMatAvant < 150)
    {
        cmd_voile = (15.0 / 19.0) * VentMatAvant - (540.0 / 19.0);
    }
    else if(VentMatAvant >= 15 && VentMatAvant < 55)
    {
        cmd_voile = 15;
    }
    else if(VentMatAvant >= 0 && VentMatAvant < 15)
    {
        cmd_voile = VentMatAvant;
    }

    return cmd_voile;
}

// Fonction qui calcule l'angle du safran
// [IN] struc point waypoint : coordonnées de la bouée 
// [IN] struc point bateau : les coordonnées du bateau 
// [IN] float bateau_orientation : orientation du bateau en degrés par rapport au nord
// [OUT] float cmd_safran : commande d'angle à adopter au safran
float calcul_angle_safran(struct point waypoint, struct point bateau, float bateau_orientation)
{

    float NordMatWaypoint = calcul_angle_NordMatWaypoint(waypoint, bateau);

    float AvantMatWaypoint = NordMatWaypoint - bateau_orientation;
    
    // printf("angle_bateau_waypoint = %f\n", AvantMatWaypoint);

    float cmd_safran = 0;

    if(AvantMatWaypoint > 180)
    {
        AvantMatWaypoint = 360 - AvantMatWaypoint;
    }
    else if(AvantMatWaypoint < -180) 
    {
        AvantMatWaypoint = AvantMatWaypoint + 360;
    }

    if(AvantMatWaypoint > -180 && AvantMatWaypoint <= -120 )
    {
        cmd_safran = 15 ;
    }
    else if(AvantMatWaypoint > -120 && AvantMatWaypoint <= -30)
    {
        cmd_safran = 8 ;
    }
    else if(AvantMatWaypoint > -30 && AvantMatWaypoint <= -5)
    {
        cmd_safran = 2 ;
    }
    else if(AvantMatWaypoint > -5 && AvantMatWaypoint < 5)
    {
        cmd_safran = 0 ;
    }
    else if(AvantMatWaypoint > 5 && AvantMatWaypoint <= 30)
    {
        cmd_safran = -2 ;
    }
    else if(AvantMatWaypoint > 30 && AvantMatWaypoint <= 120)
    {
        cmd_safran = -8 ;
    }
    else if(AvantMatWaypoint > 120 && AvantMatWaypoint <= 180)
    {
        cmd_safran = -15 ;
    }

    // Utilisation du PID
    //cmd_safran = pid(cmd_safran, 0.0); // setpoint = 0.0 pour réguler l'erreur à zéro

    return cmd_safran;
}

// Fonction qui assure le pilotage de la voile et du safran du bateau
// [IN] struct point Waypoint : ojectif a atteindre
// [IN] struct point bateau : coordonées du bateau
// [IN] float VMA : angle entre du vent par rapport au bateau
// [IN] float NMA : orientation du bateau par rapport au nord
// [OUT] struct cmd pilote : structure qui contient les commandes pour la voile et le safran
struct cmd pilotage(struct point waypoint, struct point bateau, float VMA, float NMA)
{
    struct cmd pilote;
    pilote.voile = 0;
    pilote.safran = 0;

    //Commande de la voile entre 0° et 90°
    pilote.voile = calcul_angle_voile(VMA);

    //Commande du safran entre -30° et +30°
    pilote.safran = calcul_angle_safran(waypoint, bateau, NMA);

    return pilote;
}