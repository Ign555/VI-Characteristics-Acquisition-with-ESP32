//#define DEBUG //Décommenter pour activer le mode debug

#include <Arduino.h>

#include "Panneau.hpp"

//*******************************************Initialisation***********************************************************

int pwmChannel = 0;          // Selects channel 0
int Pinrelay = 18;           // Pin, du  signal commande relay

float Vcourant_ampli; // Tension amplifier par l'AOP, mesurรฉ sur une entrรฉe esp32
float Vmesure;        // tension de sortie du pont diviseur mesurer sur l'entrรฉe esp32

float Icc;             // transistor fermรฉ alpha=1
float Voc;             // transistor ouvert alpha=0
float Vmin;            // tension minimale
float Imin;            // courant minimale

int nbPtI = 12; // number of points lower part
int nbPtV = 15; // number of points upper part
int numPt;

float Req[60]; // Résistance équivalent de chaque partition
float dty[60]; // Mettre les rapport cycliques dans le tableau
float Iinf;    // Courant inférieurs
float I[60];
float V[60];
float tab_volt[60];
float tab_curr[60];

int conv;
int R0 = 22; // rรฉsistance de puissance ร  22 ohms

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
Panneau panneau;

void setup()
{
  Serial.begin(115200);
  Serial.println("CARTE VI Mesure Lineaire\n");



}

void loop()
{
}

//******************************************Commandes******************************************************

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

void serialEvent()
{
  while (Serial.available() > 0) // tant qu'il y a des caractères à lire
  {
    reception(Serial.read());
  }
}

