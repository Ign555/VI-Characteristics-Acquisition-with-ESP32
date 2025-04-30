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

void Panneau::caracterisation_VI(int nbr_ptV, int nbr_ptI){

    digitalWrite(__PIN_RELAI__, HIGH); // Mise des pins commandes des relais à l'état haut, déconnexion PV
    
    this->_mesuse_Icc();
    this->_mesure_Voc();

    //Impression de Voc et Icc
    Serial.printf("Icc: %f \n", this->Icc);
    Serial.printf("Voc: %f \n", this->Voc);

    //Calcul des résistances équivalentes pour chaque point de la courbe
    this->_zone_I_constant(nbr_ptV);
    this->_zone_V_constante(nbr_ptI);

    digitalWrite(__PIN_RELAI__, LOW); // Mise des pins commandes des relais à l'état haut, déconnexion PV

}

void Panneau::lecture_temperature(){



}

/******************************Fonctions d'acquisition des données du panneau ( privé )******************************/

void Panneau::_mesuse_Icc(){

    float S_Vcourant_ampli = 0, Vcourant_ampli = 0;

    ledcWrite(__CANAL_PWM__, __DUTY_ICC__); // Mise en court-circuit du PV
    
    delay(100); // Attente du 100 ms ( pour donner le temps au transistor ?? )

    //Somme des mesures de la tension Ushunt amplifiée
    for (int i = 0; i < 1000; i++){ 
        S_Vcourant_ampli = (float)analogReadMilliVolts(__PIN_MESURE_COURANT__) + S_Vcourant_ampli;
    }

    //Calcul de la moyenne des mesures 
    Vcourant_ampli = S_Vcourant_ampli / 1000;

    //Mise à l'échelle de Vcourant_ampli pour obtenir Icc
    this->Icc = (Vcourant_ampli / 319.48); 
       
}

void Panneau::_mesure_Voc(){

    float S_Vmesure = 0, Vmesure = 0;

    ledcWrite(__CANAL_PWM__, __DUTY_VOC__); // Connexion de R0 = 22 Ohn entre les bornes du panneau

    delay(100); // Attente du 100 ms ( pour donner le temps au transistor ?? )
    
    //Somme des mesures de la tension à la sortie du pont diviseur
    for (int i = 0; i < 1000; i++)
    {
        S_Vmesure = (float)analogReadMilliVolts(__PIN_MESURE_TENSION__) + S_Vmesure; // mesure de la tension Ushunt amplifiรฉe
    }

    //Calcul de la moyenne des mesures
    Vmesure = S_Vmesure / 1000;

    //Mise à l'échelle de Vmesure pour obtenir Voc
    this->Voc = (Vmesure / 91.56); 
    
}

void Panneau::_zone_V_constante(int nbr_ptI){

    float Imin, deltaI;

    //Calcul de la valeur minimal du courant
    Imin = this->Voc / __VALEUR_R0__;

    //Stockage / Calcul des différents points caractéristique dans la zone où la tension est constante ( V, I, Req, dty )
    for(this->_num_pt = 1; this->_num_pt <= nbr_ptI; this->_num_pt++){

        //à commenter
        deltaI = (this->Icc - Imin) / (nbr_ptI - 1);
        
        //Mesure des résistance équivalentes
        this->I[this->_num_pt] = Imin + (this->_num_pt - 1) * deltaI; //Calcul de I ( Formule à mettre )
        this->V[this->_num_pt] = Voc; //On est dans la zone où V est constant donc c'est Voc

        this->Req[this->_num_pt] = Voc / I[this->_num_pt]; //Résistance équivalente pour notre I

        //Plafonnement de la résistance équivatente 
        if(this->Req[this->_num_pt] > __VALEUR_R0__){
            this->Req[this->_num_pt] = __VALEUR_R0__;
        }

        dty[this->_num_pt] = 1 - Req[this->_num_pt] / __VALEUR_R0__; //Calcul du rapport cyclique associé à la résistance

    }

}

void Panneau::_zone_I_constant(int nbr_ptV){

    float deltaV;
    int nbr_prev = this->_num_pt; //Stockage du nombre de panneau déjà fait ( correspondant normalement à nbr_ptI)

    for(this->_num_pt = this->_num_pt + 1; this->_num_pt < (nbr_ptV + nbr_prev); this->_num_pt++){

        //à commenter
        deltaV = this->Voc / (nbr_ptV - 1);
        
        //Mesure des résistance équivalentes
        this->I[this->_num_pt] = Icc; //On est dans la zone où I est constant donc c'est Icc
        this->V[this->_num_pt] = Voc - (this->_num_pt - nbr_prev) * deltaV; //Calcul de V ( Formule à mettre )

        this->Req[this->_num_pt] = this->V[this->_num_pt] / Icc; //Résistance équivalente pour notre V

        //Plafonnement de la résistance équivatente 
        if(this->Req[this->_num_pt] > __VALEUR_R0__){
            this->Req[this->_num_pt] = __VALEUR_R0__;
        }

        dty[this->_num_pt] = 1 - Req[this->_num_pt] / __VALEUR_R0__; //Calcul du rapport cyclique associé à la résistance
        
    }

}

void Panneau::_mesure_point_caracteristique(int nbr_ptI, int nbr_ptV){

    



}