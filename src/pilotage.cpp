#include "include/pilotage.h"


struct cmd pilotage(struct point waypoint, float Vent)
{
    struct cmd pilote;
    pilote.voile = 0;
    pilote.safran = 0;

    //Commande de la voile
    float Vent_Voile;
    if(Vent > 180)
    {
        Vent_Voile = 360 - Vent;
    }
    else 
    {
        Vent_Voile = Vent;
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

    //Commande du safran entre -30° et +30°
    //Utilisation du PID



    return pilote;
}