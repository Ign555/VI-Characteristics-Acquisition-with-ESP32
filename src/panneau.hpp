/****************************************************************
*
*
* Bibliothèque panneau solaire pour D2E Laos 
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

//Constantes PWM
#define __CANAL_PWM__ 0 //Canal PWM 0
#define __FREQUENCE_PWM_CARTE_VI__ 78000 // Fréquence PWM de la carte à 78 kHz
#define __RESOLUTION_PWM__ 10 //10 bits de résolution => 1024 valeurs possibles


//Constantes matérielle 
#define __VALEUR_R0__ 22 //Résistance de chage (R0) de 22 Ohm

/****************************************************************
*
* Classe panneau solaire
*
****************************************************************/

class Panneau{

    public: 
        Panneau();
        ~Panneau();
        void caracterisation_VI();
        void lecture_temperature();
};

#endif