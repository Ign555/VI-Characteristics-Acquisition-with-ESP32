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

//#define DEBUG //Décommenter pour activer le mode debug

#include <Arduino.h>

#include "Panneau.hpp"


int nbPtI = 12; // number of points lower part
int nbPtV = 15; // number of points upper part


/* DIP switch entrées:
int BP1 = 25;
int BP2 = 26;
int BP3 = 27;
int BP4 = 14;
*/

/*void onReceive(int packetSize);
void FloatToBytes(float buffercase, int &quotient, int &reste);
void sendValues(float *buffer, int start, int end, uint8_t CAN_ID);
*/

//uint8_t getDIPSwitchInfo(void);

//Prototype de fonction
void reception(char ch); //Fonction de callback exécuté lors d'une réception via le port série

//Déclaration des périphériques
Panneau panneau;

void setup()
{

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
      panneau.caracterisation_VI(15, 12);
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