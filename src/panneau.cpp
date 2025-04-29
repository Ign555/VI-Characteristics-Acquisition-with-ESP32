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

#include "panneau.hpp"

/****************************************************************
*
* Méthode du panneau 
*
****************************************************************/

/******************************Panneau solaire constructteur & destructeurs******************************/

Panneau::Panneau(void){

    //Définition du sens des sorties du panneau 
    pinMode(__PIN_RELAI__, OUTPUT); // Broche relié aux commandes des relais en sortie
    pinMode(__PIN_PWM__, OUTPUT);   // Broche relié à la gate ( PWM ) de l'IRL510 en sortie 


    // Initalisation du PWM
    ledcSetup(__CANAL_PWM__, __FREQUENCE_PWM_CARTE_VI__, __RESOLUTION_PWM__); 
    ledcAttachPin(__PIN_PWM__, __CANAL_PWM__);  //Attribution du canal à une pin ( voir .h )

}

Panneau::~Panneau(void){



}

/******************************Fonctions d'acquisition des données du panneau ( public )******************************/

void Panneau::caracterisation_VI(){

    digitalWrite(__PIN_RELAI__, HIGH); // Mise des pins commandes des relais à l'état haut, déconnexion PV
    
    digitalWrite(__PIN_RELAI__, LOW); // Mise des pins commandes des relais à l'état haut, déconnexion PV

}

void Panneau::lecture_temperature(){



}

/******************************Fonctions d'acquisition des données du panneau ( privé )******************************/

