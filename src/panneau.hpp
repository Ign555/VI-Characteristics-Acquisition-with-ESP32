/****************************************************************
*
*
* Bibliothèque panneau solaire pour les cartes VI du D2E Laos 
* Created by Rayan Grand
* Version : v1.0
* File Creation : 29/04/2025
*
*
****************************************************************/

#include <Arduino.h>

#ifndef __PANNEAU_H__
#define __PANNEAU_H__

/****************************************************************
*
* Constantes liées à la carte VI
*
****************************************************************/

//Broche de la carte VI
#define __PIN_RELAI__ 18
#define __PIN_PWM__ 19 //Pin de la PWM lié au découpage de la résistance de chagre ( R0 )
#define __PIN_MESURE_TENSION__ 32
#define __PIN_MESURE_COURANT__ 33


//Constantes PWM
#define __CANAL_PWM__ 0 //Canal PWM 0
#define __FREQUENCE_PWM_CARTE_VI__ 78000 // Fréquence PWM de la carte à 78 kHz
#define __RESOLUTION_PWM__ 10 //10 bits de résolution => 1024 valeurs possibles pour le rapport cyclique

//Valeur rapport cyclique PWM 
#define __DUTY_ICC__ 1023 //Rapport cyclique maximum => mise en court-circuit du panneau
#define __DUTY_VOC__ 0 //Rapport cyclique minimum =>  imposer une charge de 22 Ohm

//Constante lié à la mesure 
#define __N_MESURE__ 1000

//Constantes matérielle 
#define __VALEUR_R0__ 22 //Résistance de chage (R0) de 22 Ohm
#define __FACTEUR_ECHELLE_TENSION__ 91.56 //Facteur de mise à l'échelle pour obtenir Voc
#define __FACTEUR_ECHELLE_COURANT__ 319.48 //Facteur de mise à l'échelle pour obtenir ICC
#define __SEUIL_ANALOGREADMILLIVOLT_ESP32__ 142 // Valeur minimum donnée par la fonction analogReadMillivolt ( voir vidéo Discours )

/****************************************************************
*
* Classe panneau solaire
*
****************************************************************/

class Panneau{

    public: 

        Panneau();
        ~Panneau();
        void caracterisation_VI(int nbr_ptI, int nbrPtI);

        //Getter
        int get_nombre_de_mesures(); //Fonction qui renvoie le nombre de mesure effectuée lors de la dernière caractérisation

        float get_mesure_V(uint8_t index);
        float get_mesure_I(uint8_t index);

    private:

        float Icc, Voc; // _S_Vcourant_ampli => Somme des tensions relevées en sortie de l'Ampli ( panneau en court-circuit ), 
                                    // Icc => Courant moyen relevée ( panneau en court-circuit )

        float I[60], V[60]; 
        float VI_I[60], VI_V[60]; //Tableaux stockant les points caractéristiques
        float Req[60], dty[60]; //Tableau stockant les résistances équivatentes / Tableau stockant les rapports cyliques associés 

        int _nbr_mesure; //Nombre de mesure effectuée lors de la dernière caractérisation
        int _num_pt;

        void _mesuse_Icc();
        void _mesure_Voc();

        void _zone_V_constante(int nbr_ptI);
        void _zone_I_constant(int nbr_ptI, int nbr_ptV);

        void _mesure_point_caracteristique();

};

#endif