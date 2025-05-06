/****************************************************************
*
*
* Programme mesure caractéristique VI ( linéaire )
* Created by Vorachack Kongphet
* Version : v1.1
* 
*
*
****************************************************************/

//#define __DEBUG__ //Décommenter pour débugger

#include <Arduino.h>
#include <CAN.h>

#include "Panneau.hpp"
#include "canMessage.h"
#include "tc74.h"

#define __ADDRESSE_CAPTEUR_TC74__ 0x48 //Addresse du TC74

//Broche du DIP Switch
#define BP1 25
#define BP2 6
#define BP3 27
#define BP4 14
#define BP5 15

//#define MAIN_MODE //Décommenter pour 'commander' le bus CAN avec la carte

int nbPtI = 12; // number of points lower part
int nbPtV = 15; // number of points upper part

void onReceive(int packetSize);
/*
void FloatToBytes(float buffercase, int &quotient, int &reste);
void sendValues(float *buffer, int start, int end, uint8_t CAN_ID);
*/

//uint8_t getDIPSwitchInfo(void);

//Prototype de fonction
void reception(char ch); //Fonction de callback exécuté lors d'une réception via le port série

//Déclaration des flags
bool canAvailable;


//Déclaration des périphériques
CANMessage rxMsg;
Panneau panneau;

void setup()
{

  //Définition entrée/sortie DIP switch

  
  //Initialisation bus CAN
  lireTemperature(__ADDRESSE_CAPTEUR_TC74__);

  //Initialisation du port série

  Serial.begin(115200);
  Serial.println("CARTE VI Mesure Lineaire");

}

void loop()
{

}

/****************************************************************
*
* Réception et traitement des informations reçues en liason série 
*
****************************************************************/

void serialEvent()
{
  while (Serial.available() > 0) // tant qu'il y a des caractères à lire
  {
    reception(Serial.read());
  }
}

void reception(char ch)
{
  static int i = 0;
  static String chaine = "";
  String commande;
  String valeur;
  int index, length;

  if ((ch == 13) or (ch == 10))
  {
    index = chaine.indexOf(' ');
    length = chaine.length();

    if (index == -1)
    {
      commande = chaine;
      valeur = "";
    }
    else
    {
      commande = chaine.substring(0, index);
      valeur = chaine.substring(index + 1, length);
    }

    if (commande == "M")
    {
      panneau.caracterisation_VI(nbPtV, nbPtI);
    }

    chaine = "";
  }
  else
  {
    chaine += ch;
  }
}

/****************************************************************
*
* Réception et traitement des informations reçues par le BUS CAN
*
****************************************************************/

void initCAN(){

  Serial.println("Initialisation du bus CAN...");

  // On démarre le CAN à une vitesse de 1000 kbps
  if (!CAN.begin(10E3))
  {
      Serial.println("Echec inialisation du CAN!");
      while (1);
  }

  //On calcul le délai d'attente avant le message CAN 

  //Définition de la fonction de réception ( callback )
  CAN.onReceive(onReceive);

}

void onReceive(int packetSize)
{

  rxMsg.id = CAN.packetId();
  rxMsg.len = CAN.packetDlc();

  int i = 0;
  while (CAN.available())
  {
    rxMsg.data[i] = CAN.read();
    i++;
  }

  canAvailable = true;

}

void manageCAN()
{

  #ifdef __DEBUG__
    printf("ID  = %d\n", rxMsg.id);
    printf("Len = %d\n", rxMsg.len);
  #endif

  canAvailable = false;

}