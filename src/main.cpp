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
#include "utils.h"

#define __ADDRESSE_CAPTEUR_TC74__ 0x48 //Addresse du TC74
#define __N_BP__ 5 //Nombre d'interrupteur connecté ( DIP SWITCH )
#define __DELAI_CAN__ 10  //Délai pour que l'on impose après l'envoie d'un paquet sur le CAN pour être sûr qu'il s'envoie bien
#define __COEFF_TEMPS_REPONSE__ 100 //Delai que prend une carte pour répondre = __COEFF_DELAI_CAN__ * numero_de_carte

//#define MAIN_MODE //Décommenter pour 'commander' le bus CAN avec la carte

int nbPtI = 12; // number of points lower part
int nbPtV = 15; // number of points upper part

//Prototype de fonction communication série
void onReceive(int packetSize);
void reception(char ch); //Fonction de callback exécuté lors d'une réception via le port série

//Prototype de fonction CAN
void initCAN(); //Fonction d'initialisation du CAN
void manageCAN();

//Prototype de fonction de réponse CAN
void envoyer_ping();
void envoyer_temperature();
void envoyer_caracteristique();

//Déclaration des flags
bool canAvailable = false;

//Variables globales
uint8_t num_carte = 0; //Numéro de la carte sur le BUS CAN
uint8_t temp; //Température exterrieur ( lecture avec le TC74 )

//Broche du DIP Switch
uint8_t pin_BP[5] = {25, 26, 27, 14, 13};

//Déclaration des périphériques
CANMessage rxMsg;
Panneau panneau;

/****************************************************************
*
* Programme principal
*
****************************************************************/

void setup()
{

  //Initialisation du port série
  Serial.begin(115200);
  Serial.println("CARTE VI Mesure Lineaire");

  //Initalisation de l'I2C pour le TC74
  Wire.begin();

  //Mise en entrée des broches liées au DIP switch
  for(int i = 0 ; i < __N_BP__ ; i++)
  {
    pinMode(pin_BP[i], INPUT);
  }
  
  //Lecture des broches liées au DIP switch ( détermination du numéro de la carte )
  for(int i = 1 ; i < __N_BP__ ; i++)
  {

    if(digitalRead(pin_BP[i-1]) == LOW){
      num_carte = i;
      break;
    }

  }
  
  //Initialisation bus CAN
  initCAN();

  //Impression de l'ID de la carte
  Serial.printf("Carte n°%d\n", num_carte);

}

void loop()
{

  if (canAvailable == true)
  {
    manageCAN();
  }

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

    if (commande == "T")
    {
      Serial.printf("%d\n", temp);
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

  switch(rxMsg.id){
    
    case 0: //En cas d'ID 0 ( la carte CAN vérifie les cartes présentes ) 
      envoyer_ping();
    break;
    
    case 1:
      envoyer_temperature();
    break;

    case 2:
      envoyer_caracteristique();
    break;

    case 8:
      envoyer_temperature();
      envoyer_caracteristique();
    break;
    
    case 127: //En cas d'ID 127 ( demande de redémarrage )
      ESP.restart();
    break;

    default:
      //On imprime les informations du message
      Serial.print("Message inconnu Data(s) : ");
      for (int i = 0; i < rxMsg.len; i++)
      {
        Serial.print(rxMsg.data[i]);
        Serial.print(" ");
        Serial.println();
      }
    break;
  }  

  canAvailable = false;

}

/****************************************************************
*
* Fonction "réponse" CAN
*
****************************************************************/

//Envoie d'une réponse à la carte CAN pour confirmer la présence de la carte
void envoyer_ping(){

  delay(__COEFF_TEMPS_REPONSE__*num_carte);

  Serial.println("Demande d'identification, envoie de la réponse...");

  //Envoie des paquets à l'id 10
  CAN.beginPacket(0xA);
  CAN.write(num_carte);
  CAN.endPacket();

  delay(__DELAI_CAN__);

}

//Envoie de la température à la carte CAN
void envoyer_temperature(){
  
  //Lecture du capteur de température
  temp = lireTemperature(__ADDRESSE_CAPTEUR_TC74__);

  delay(__COEFF_TEMPS_REPONSE__*num_carte);

  #ifdef __DEBUG__
    Serial.println("Envoie de la température...");
  #endif

  //Envoie des paquets à l'id 11 ( température )
  CAN.beginPacket(0xB);
  CAN.write(temp); //Mise de la valeur de la température dans le paquet
  CAN.endPacket();

  delay(__DELAI_CAN__);

}

//Envoie de la caractéristique VI à la cartes CAN
void envoyer_caracteristique(){

  float I_measure_buffer, V_measure_buffer;
  uint8_t I_buffer[sizeof(float)], V_buffer[sizeof(float)]; //Ces variables stockerons les temporaitements les octets des associés aux valeurs des points de mesure

  //Preparation des informations à envoyer
  panneau.caracterisation_VI(nbPtV, nbPtI);

  delay(__COEFF_TEMPS_REPONSE__*num_carte);

  Serial.println("Envoie de la caractéristique VI...");

  //Pour chaque mesure effectuée
  for(uint8_t i = 0; i < panneau.get_nombre_de_mesures(); i++){

    //Récupération des mesures
    V_measure_buffer = panneau.get_mesure_V(i);
    I_measure_buffer = panneau.get_mesure_I(i);

    //Convertion des mesures
    memcpy(V_buffer, &V_measure_buffer, sizeof(float));
    memcpy(I_buffer, &I_measure_buffer, sizeof(float));

    //Envoie des paquets à l'id 12 ( mesure )
    CAN.beginPacket(0xC);
    CAN.write(panneau.get_nombre_de_mesures());
    CAN.write(i);

    //On insère les octets de la valeur de la tension
    for(int j = 0; j < sizeof(float); j++){
      CAN.write(V_buffer[j]);
    }

    CAN.endPacket();

    delay(__DELAI_CAN__);

    //Envoie des paquets à l'id 13 ( mesure )
    CAN.beginPacket(0xD);
    CAN.write(panneau.get_nombre_de_mesures());
    CAN.write(i);

    //On insère les octets de la valeur du courant
    for(int j = 0; j < sizeof(float); j++){
      CAN.write(I_buffer[j]);
    }

    CAN.endPacket();
    
    delay(__DELAI_CAN__);

  }

  delay(__DELAI_CAN__);

}