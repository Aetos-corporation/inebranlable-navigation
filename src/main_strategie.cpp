#include "include/strategie.h"
#include "include/pilotage.h"
#include "include/navigation.h"

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

    return 0;
}