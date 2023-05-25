#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>


//Structures 
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
float calcul_norme_vecteur(struct vecteur vector)
{
    float norme = ((vector.x)*(vector.x)) + ((vector.y)*(vector.y));
    norme = sqrtf(norme);
    return norme;
}

// Fonction pour calculer la distance entre deux points GPS
float distance(struct point p1, struct point p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dist = sqrt(dx*dx + dy*dy);
    return dist;
}

// Fonction pour convertir la valeur d'un angle entre [0; 360] sur [-180; 180]
float conversion_angle_360_180(float angle)
{
    float angle_conv;
    if(angle < 180)
    {
        angle_conv = angle;
    }
    else if(angle > 180)
    {
        angle_conv = -(360 - angle);
    }
    else
    {
        angle_conv = 180;
    }

    return angle_conv;
}

// Fonction qui vérifie la distance avec la bouée
// [IN] struct point bouee : coordonnees de la Bouee
// [IN] struct point bateau : les coordonnees du Bateau
// [IN] float seuil : seuil où on considère que le bateau est arrivé à la bouée
// [OUT] bool : true si la distance est inférieure au seuil, false sinon
bool verif_distance_bouee(struct point bouee, struct point bateau, float* seuil)
{
    bool distance_bouee = false;
    float dist = distance(bouee, bateau);
    if(dist <= *seuil)
    {
        distance_bouee = true;
    }
    else
    {
        *seuil += 0.2;
    }
    return distance_bouee;
}


// Fonction qui calcule l'angle entre le Nord, le bateau et la bouee :
// [IN] struct point waypoint : coordonnees de la Bouee 
// [IN] struct point bateau : les coordonnees du Bateau 
// [OUT] float NordMatWaypoint : angle Nord Bateau Bouee
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

// Fonction qui calcule l'angle entre le Est, le Bateau et l'Avant :
// [IN] float nordMatAvant : angle Nord Mat Avant
// [OUT] float NordMatWaypoint : angle Est Mat Avant
float calcul_angle_EstMatAvant(float nordMatAvant)
{
    float EstMatAvant;

    if(nordMatAvant == 270)
    {
        EstMatAvant = 0;
    }
    else if(nordMatAvant < 270)
    {
        EstMatAvant = nordMatAvant + 90;
    }
    else if(nordMatAvant > 270)
    {
        EstMatAvant = 90 - nordMatAvant;
    }

    return EstMatAvant;
}

// Fonction qui calcule l'angle entre le Vent, le Bateau et la Bouee :
// [IN] float ventMatAvant : angle donnee par la girouette
// [IN] float nordMatAvant : angle entre le Nord, le Mat et Avant du bateau
// [IN] struct point bateau : les coordonnees du Bateau 
// [IN] struct point bouee : les coordonnees de la Bouee 
// [OUT] float VentMatBouee : angle entre le Vent, le Mat et la Bouee
float calcul_angle_VentMatBouee(struct point bouee, struct point bateau, float ventMatAvant, float nordMatAvant)
{   
    float VentMatBouee; 

    float NordMatBouee = calcul_angle_NordMatWaypoint(bouee, bateau);
    // printf("nordMatAvant = %f\n", nordMatAvant);

    float AvantMatBouee = 0;
    if(nordMatAvant > NordMatBouee)
    {
        AvantMatBouee = (360 - nordMatAvant) + NordMatBouee;
    }
    else
    {
        AvantMatBouee = NordMatBouee - nordMatAvant;
    }
    // printf("AvantMatBouee = %f\n", AvantMatBouee);

    // Convertion de l'angle donne par la girouette sur -180/180 par rapport a la direction du bateau
    float AvantMatVent = conversion_angle_360_180(ventMatAvant);
    // printf("AvantMatVent = %f\n", AvantMatVent);

    if(AvantMatVent < 0)
    {
        if( (-AvantMatVent) < AvantMatBouee)
        {
            VentMatBouee = AvantMatBouee - (-AvantMatVent);
        }
        else 
        {
            VentMatBouee = 360 -(-AvantMatVent) + AvantMatBouee; 
        }
    }
    else 
    {
        if( (360-AvantMatVent) > AvantMatBouee)
        {
            VentMatBouee = AvantMatVent + AvantMatBouee;
        }
        else
        {
            VentMatBouee = AvantMatBouee - (360 - AvantMatVent);
        }
    }
    // printf("VentMatBouee = %f\n", VentMatBouee);

    return VentMatBouee;
}

// Fonction qui calcule l'angle de la voile
// [IN] float ventMatAvant : angle qui donne la provenance du vent par rapport au bateau (180 = vente arriere)
// [OUT] float cmd_voile : commande d'angle a adopter a la voile
float calcul_angle_voile(float ventMatAvant)
{
    float cmd_voile = 0;
    if(ventMatAvant > 180)
    {
        ventMatAvant = 360 - ventMatAvant;
    }

    if(ventMatAvant >= 150)
    {
        cmd_voile = 90;
    }
    else if(ventMatAvant >= 55 && ventMatAvant < 150)
    {
        cmd_voile = (15.0 / 19.0) * ventMatAvant - (540.0 / 19.0);
    }
    else if(ventMatAvant >= 15 && ventMatAvant < 55)
    {
        cmd_voile = 15;
    }
    else if(ventMatAvant >= 0 && ventMatAvant < 15)
    {
        cmd_voile = ventMatAvant;
    }

    return cmd_voile;
}

// Fonction qui calcule l'angle du safran
// [IN] struct point waypoint : coordonnees de la bouee 
// [IN] struct point bateau : les coordonnees du bateau 
// [IN] float bateau_orientation : orientation du bateau en degres par rapport au nord
// [OUT] float cmd_safran : commande d'angle a adopter au safran
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

    return cmd_safran;
}


// Fonction qui calcule la decision de strategie, entre aller tout droit ou tirer des bords
// [IN] struct point bouee : coordonnees de la bouee a viser
// [IN] struct point mat : coordonees du bateau
// [IN] float ventMatAvant : angle entre le vent, le mat et l'avant du bateau, donne par la girouette 
// [IN] float nordMatAvant : angle entre le nord, le mat et l'avant du bateau, donne par l'IMU
// [OUT] int strategie : strategie a adopter
int decision_strategie(struct point bouee, struct point mat, float ventMatAvant, float nordMatAvant)
{
    int retour = 0;
    float VentMatBouee = calcul_angle_VentMatBouee(bouee, mat, ventMatAvant, nordMatAvant);
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


// Fonction qui donne un objectif (bouee) a atteindre au bateau en fonction de la strategie
// [IN] int strategie : strategie de navigation
// [IN] float ventMatAvant : angle entre le vent, le mat et l'avant du bateau
// [IN] float nordMatAvant : angle entre le nord, le mat et l'avant du bateau
// [IN] struct point bouee : bouee ou point d'arrivee a atteindre par defaut
// [IN] struct point bateau : position du Bateau
// [OUT] struct point waypoint : ojectif a atteindre
struct point navigation(struct point bouee, struct point bateau, int statregie, float ventMatAvant, float nordMatAvant)
{
    // Par defaut, on vise comme objectif la bouee (ou le ponton si sur le retour)
    struct point waypoint = bouee;
    
    if (statregie == 1)
    {
        // Il faut tirer des bords, on doit donc générer des waypoint virtuels pour avancer vers l'objectif
        float dist_bateau_bouee = distance(bateau, bouee);
        // printf("dist_bateau_bouee = %f\n", dist_bateau_bouee);

        // Calculer la droite qui a un angle de 40° par rapport au vent 
        float NordMatBouee = calcul_angle_NordMatWaypoint(bouee, bateau);
        // printf("NordMatBouee = %f\n", NordMatBouee);

        float VentMatBouee = calcul_angle_VentMatBouee(bouee, bateau, ventMatAvant, nordMatAvant);
        // printf("VentMatBouee = %f\n", VentMatBouee);

        float BoueeMatWaypoint = 0;
        if(VentMatBouee < 30)
        {
            BoueeMatWaypoint = 40 - VentMatBouee;
        }
        else if (VentMatBouee > 330)
        {
            BoueeMatWaypoint = 360 - (40 - (360 - VentMatBouee));
        }
        // printf("BoueeMatWaypoint = %f\n", BoueeMatWaypoint);
        
        float NordMatWaypoint = NordMatBouee + BoueeMatWaypoint;
        if(NordMatWaypoint > 360)
        {
            NordMatWaypoint = NordMatBouee - (360 - BoueeMatWaypoint);
        }
        // printf("NordMatWaypoint = %f\n", NordMatWaypoint);

        // Définition de l'angle en fonction de l'axe des abscisses (le Nord étant l'axe des ordonnés)
        float XMatWaypoint = 0;
        if(NordMatWaypoint >= 270)
        {
            XMatWaypoint = NordMatWaypoint - 270;
        }
        else
        {
            XMatWaypoint = NordMatWaypoint  + 90;
        }

        // printf("XMatWaypoint = %f\n", XMatWaypoint);
        // Passage de l'angle en radian
        XMatWaypoint = XMatWaypoint * (M_PI/180);

        waypoint.x = bateau.x + (1.5 * dist_bateau_bouee) * cosf(XMatWaypoint);
        waypoint.y = bateau.y + (1.5 * dist_bateau_bouee) * sinf(XMatWaypoint);
    }

    return waypoint;
}


// Fonction qui assure le pilotage de la voile et du safran du bateau
// [IN] struct point Waypoint : ojectif a atteindre
// [IN] struct point bateau : coordonees du bateau
// [IN] float ventMatAvant : angle entre du vent par rapport au bateau
// [IN] float nordMatAvant : orientation du bateau par rapport au nord
// [OUT] struct cmd pilote : structure qui contient les commandes pour la voile et le safran
struct cmd pilotage(struct point waypoint, struct point bateau, float ventMatAvant, float nordMatAvant)
{
    struct cmd pilote;
    pilote.voile = 0;
    pilote.safran = 0;

    //Commande de la voile entre 0° et 90°
    pilote.voile = calcul_angle_voile(ventMatAvant);

    //Commande du safran entre -30° et +30°
    pilote.safran = calcul_angle_safran(waypoint, bateau, nordMatAvant);

    return pilote;
}


// Fonction main qui efffectue l'ensemble des tests
int main(void)
{
    printf("*******************************************************\n");
    printf("*** Tests de l'agorithme de navigation              ***\n");
    printf("*** Auteurs : Arzhel PUILLANDRE et Hugo COLLINEAU   ***\n");
    printf("*******************************************************\n\n");

    // Positions de l'objectif et du bateau 
    struct point Bouee = {7, 2};
    struct point Mat = {2, 2};

    //Les angles sont donnés sur [0, 360]
    float VentMatAvant = 180.10;
    float NordMatAvant = 296.67;


    // // Test de la fonction decision_strategie
    // printf("**********************************************\n");
    // printf("*** Test de la fonction decision_strategie ***\n");
    // printf("**********************************************\n\n");

    // int strategie = decision_strategie(Bouee, Mat, VentMatAvant, NordMatAvant);

    // printf("decision: %d\n", strategie);
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


    // // Test de la fonction navigation 
    // printf("************************************\n");
    // printf("*** Test de la fonction navigation ***\n");
    // printf("************************************\n\n");

    // // Positions de l'objectif 
    // Bouee.x= 7;
    // Bouee.y = 2;

    // Mat.x= 2;
    // Mat.y = 2;

    // //Les angles sont donnés sur [0, 360]
    // VentMatAvant = 37.98;
    // NordMatAvant = 296.67;

    // int strategie = 1;

    // struct point Waypoint = navigation(Bouee, Mat, strategie, VentMatAvant, NordMatAvant);

    // printf("Waypoint.x : %f\n", Waypoint.x);
    // printf("Waypoint.y : %f\n", Waypoint.y);

    // printf("\n\n");


    // Test de la fonction pilotage
    printf("************************************\n");
    printf("*** Test de la fonction pilotage ***\n");
    printf("************************************\n\n");
  
    // Positions de l'objectif et du bateau 
    Bouee.x= 6;
    Bouee.y = 0;
    
    Mat.x= 0;
    Mat.y = 0;

    //Les angles sont donnés sur [0, 360]
    VentMatAvant = 135;
    NordMatAvant = 180;

    struct cmd pilotage_bateau = pilotage(Bouee, Mat, VentMatAvant, NordMatAvant);

    printf("L'angle de voile a adopter est : %f\n", pilotage_bateau.voile);
    printf("L'angle de safran a adopter est : %f\n", pilotage_bateau.safran);

    printf("\n\n");

}