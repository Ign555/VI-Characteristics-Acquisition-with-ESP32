/****************************************************************
*
*
* Bibliothèque panneau solaire pour les cartes VI D2E Laos 
* Created by Rayan Grand
* Version : v1.0
* File Creation : 29/04/2025
*
*
****************************************************************/

//#define __DEBUG__ //Décommenter pour débugger

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

    this->_nbr_mesure = nbr_ptI + nbr_ptV;

    digitalWrite(__PIN_RELAI__, HIGH); // Mise des pins commandes des relais à l'état haut, déconnexion PV
    
    this->_mesuse_Icc();
    this->_mesure_Voc();

    //Impression de Voc et Icc
    Serial.printf("Icc: %f \n", this->Icc);
    Serial.printf("Voc: %f \n", this->Voc);

    //Calcul des résistances équivalentes pour chaque point de la courbe
    this->_zone_V_constante(nbr_ptI);
    this->_zone_I_constant(nbr_ptI, nbr_ptV);

    //Mesure des points de la caractéristique V(I)
    this->_mesure_point_caracteristique();

    digitalWrite(__PIN_RELAI__, LOW); // Mise des pins commandes des relais à l'état haut, déconnexion PV

}

/******************************Fonctions "getter" ( public )******************************/

int Panneau::get_nombre_de_mesures(){

    return this->_nbr_mesure;

}

float Panneau::get_mesure_V(uint8_t index){

    return this->VI_V[index];

}

float Panneau::get_mesure_I(uint8_t index){

    return this->VI_I[index];

}

/******************************Fonctions d'acquisition des données du panneau ( privé )******************************/

void Panneau::_mesuse_Icc(){

    float S_Vcourant_ampli = 0, Vcourant_ampli = 0;
    float Vcourant_ampli_adc = 0; //Variable pour stocker temporairement les mesures de tensions image du courant sortant du panneau 

    ledcWrite(__CANAL_PWM__, __DUTY_ICC__); // Mise en court-circuit du PV
    
    delay(100); // Attente du 100 ms ( pour donner le temps au transistor ?? )

    //Somme des mesures de la tension Ushunt amplifiée
    for (int i = 0; i < __N_MESURE__; i++){
        Vcourant_ampli_adc = (float)analogReadMilliVolts(__PIN_MESURE_COURANT__); // mesure de la tension Ushunt amplifiée 
        
        //On vérifie que la mesure ne corresponde pas au seuil minimum de la fonction analogReadMilliVolts
        if(Vcourant_ampli_adc > __SEUIL_ANALOGREADMILLIVOLT_ESP32__){
            S_Vcourant_ampli += Vcourant_ampli_adc; 
        }
    }

    //Calcul de la moyenne des mesures 
    Vcourant_ampli = S_Vcourant_ampli / __N_MESURE__;

    //Mise à l'échelle de Vcourant_ampli pour obtenir Icc
    this->Icc = (Vcourant_ampli / __FACTEUR_ECHELLE_COURANT__); 
       
}

void Panneau::_mesure_Voc(){

    float S_Vmesure = 0, Vmesure = 0; //S_Mesure => Somme des mesures effectuées, Vmesure => Moyennes des mesures
    float Vmesure_adc = 0; //Variable pour stocker temporairement les mesures de tensions

    ledcWrite(__CANAL_PWM__, __DUTY_VOC__); // Connexion de R0 = 22 Ohn entre les bornes du panneau

    delay(100); // Attente du 100 ms ( pour donner le temps au transistor ?? )
    
    //Somme des mesures de la tension à la sortie du pont diviseur
    for (int i = 0; i < __N_MESURE__; i++)
    {
        Vmesure_adc = (float)analogReadMilliVolts(__PIN_MESURE_TENSION__); // mesure de la tension en sortie du pont diviseur
        
        //On vérifie que la mesure ne corresponde pas au seuil minimum de la fonction analogReadMilliVolts
        if(Vmesure_adc > __SEUIL_ANALOGREADMILLIVOLT_ESP32__){
            S_Vmesure += Vmesure_adc; 
        }
    }

    //Calcul de la moyenne des mesures
    Vmesure = S_Vmesure / __N_MESURE__;

    //Mise à l'échelle de Vmesure pour obtenir Voc
    this->Voc = (Vmesure / __FACTEUR_ECHELLE_TENSION__); 
    
}

void Panneau::_zone_V_constante(int nbr_ptI){

    float Imin, deltaI;

    //Calcul de la valeur minimal du courant
    Imin = this->Voc / __VALEUR_R0__;

    //Stockage / Calcul des différentes caractéristique dans la zone où la tension est constante ( V, I, Req, dty )
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

        #ifdef __DEBUG__
            Serial.printf("numPt : %d;", this->_num_pt);
            Serial.printf("V : %f;", this->V[this->_num_pt]);
            Serial.printf("I : %f;", this->I[this->_num_pt]);
            Serial.printf("R : %f;\n", this->Req[this->_num_pt]);
        #endif
        
    }

}

void Panneau::_zone_I_constant(int nbr_ptI, int nbr_ptV){

    float deltaV;

    for(this->_num_pt =  this->_num_pt; this->_num_pt < (nbr_ptV + nbr_ptI); this->_num_pt++){

        //à commenter
        deltaV = this->Voc / (nbr_ptV - 1);
        
        //Mesure des résistance équivalentes
        this->I[this->_num_pt] = Icc; //On est dans la zone où I est constant donc c'est Icc
        this->V[this->_num_pt] = Voc - (this->_num_pt - nbr_ptI) * deltaV; //Calcul de V ( Formule à mettre )

        this->Req[this->_num_pt] = this->V[this->_num_pt] / Icc; //Résistance équivalente pour notre V

        //Plafonnement de la résistance équivatente 
        if(this->Req[this->_num_pt] > __VALEUR_R0__){
            this->Req[this->_num_pt] = __VALEUR_R0__;
        }

        dty[this->_num_pt] = 1 - Req[this->_num_pt] / __VALEUR_R0__; //Calcul du rapport cyclique associé à la résistance
        
        #ifdef __DEBUG__
            Serial.printf("numPt : %d;", this->_num_pt);
            Serial.printf("V : %f;", this->V[this->_num_pt]);
            Serial.printf("I : %f;", this->I[this->_num_pt]);
            Serial.printf("R : %f\n;", this->Req[this->_num_pt]);
        #endif

    }

}

void Panneau::_mesure_point_caracteristique(){

    float Vmesure = 0, Vcourant_ampli = 0;
    float Vcourant_ampli_adc = 0, Vmesure_adc = 0; //Variables pour stocker temporairement les mesures de tensions

    //Mesure des points de la caractéristique pour chaque résistances trouvées
    for (this->_num_pt = 1; this->_num_pt < this->_nbr_mesure; this->_num_pt++)
    {

        ledcWrite(__CANAL_PWM__, (int)(dty[this->_num_pt]*__DUTY_ICC__)); // Connexion du panneau à la résistance + hachage de cette dernière pour moduler le courant

        delay(100); // Attente du 100 ms ( pour donner le temps au transistor ?? )

        Vmesure = 0;
        Vcourant_ampli = 0;

        for (int i = 0; i < __N_MESURE__; i++)
        { 
            Vmesure_adc = (float)analogReadMilliVolts(__PIN_MESURE_TENSION__);
            Vcourant_ampli_adc = (float)analogReadMilliVolts(__PIN_MESURE_COURANT__);

            //On vérifie que la mesure ne corresponde pas au seuil minimum de la fonction analogReadMilliVolts
            if(Vmesure_adc > __SEUIL_ANALOGREADMILLIVOLT_ESP32__){
                Vmesure += Vmesure_adc/__N_MESURE__; //Pondération pour que la somme fasse une moyenne
            }

            //On vérifie que la mesure ne corresponde pas au seuil minimum de la fonction analogReadMilliVolts
            if(Vcourant_ampli_adc > __SEUIL_ANALOGREADMILLIVOLT_ESP32__){
                Vcourant_ampli += Vcourant_ampli_adc/__N_MESURE__; //Pondération pour que la somme fasse une moyenne
            }
        }
  
        this->VI_I[this->_num_pt] = (Vcourant_ampli / __FACTEUR_ECHELLE_COURANT__);  //Calibration du courant
        
        this->VI_V[this->_num_pt] = (Vmesure / __FACTEUR_ECHELLE_TENSION__);         //Calibration de la tension
        
        //Affichage des mesures effectuées
        Serial.printf("VI %d %3.3f %3.3f  \r\n", this->_num_pt, this->VI_V[this->_num_pt], this->VI_I[this->_num_pt]);
    
    }

}