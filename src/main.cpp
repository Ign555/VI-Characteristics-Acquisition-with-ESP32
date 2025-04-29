#include <Arduino.h>

//*******************************************Initialisation***********************************************************

int pwmChannel = 0;          // Selects channel 0
const int frequence = 78000; // PWM frequency of 1 KHz
const int resolution = 10;   // 10-bit resolution, 1024 possible values
const int pwmPin = 19;       // Pin du changement du rapport cyclique de decoupage de la resistance 22 ohm
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
void caracterisationPV(void);

//uint8_t getDIPSwitchInfo(void);

void setup()
{
  Serial.begin(115200);
  Serial.println("CARTE VI Mesure Lineaire\n");

  pinMode(Pinrelay, OUTPUT); // Configuration du pรฉriphรฉrique du pinrelay
  pinMode(pwmPin, OUTPUT);   // Configuaration du pรฉriphรฉrique du Pwm

  ledcSetup(pwmChannel, frequence, resolution); // Assigns the PWM channel to pin 19
  ledcAttachPin(pwmPin, pwmChannel);            // Create the selected output voltage
  ledcAttachPin(pwmChannel, 0);                 // ledcAttachPin(pwmChannel, 0); // 3,3V
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
      caracterisationPV();
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
 //************************************Initialisation de la Fonction de caractérisation************************************
void caracterisationPV(void) //Fonction de la carctérisation
{

  int i; // boucle for

  float alphaIcc = 1024; // transistor close
  float alphaVoc = 0;    // transsitor open
  float deltaI, deltaV;

  digitalWrite(Pinrelay, HIGH); // signal relais active, disconnection PV

  //**************************************************  Mesure Icc **************************

  ledcWrite(pwmChannel, alphaIcc);
  delay(100);
  Vcourant_ampli = 0;
  for (i = 0; i < 1000; i++)
    Vcourant_ampli = (float)analogReadMilliVolts(33) + Vcourant_ampli; // mesure de la tension Ushunt amplifiรฉe

  Vcourant_ampli = Vcourant_ampli / 1000;
  Icc = (Vcourant_ampli / 319.48); // mise à l'échelle

  //**************************************************  Mesure Voc **************************

  ledcWrite(pwmChannel, alphaVoc);
  delay(100);
  Vmesure = 0;
  for (i = 0; i < 1000; i++)
  {
    Vmesure = (float)analogReadMilliVolts(32) + Vmesure; // mesure de la tension Ushunt amplifiรฉe
  }
  Vmesure = Vmesure / 1000; // mesure de la tension de sortie du pont diviseur
  Voc = (Vmesure / 91.56);  // mesure de la tension + calibration

  Serial.print("Icc: ");
  Serial.println(Icc);

  Serial.print("Voc: ");
  Serial.println(Voc);

  Iinf = Voc / R0; // calcul de la valeur minimal du courant

  // For the V constance zone
  for (numPt = 1; numPt <= nbPtI; numPt++)
  {

    // CoefLog[numPt] = 1 + ((float(numPt - 1) * 9) / (nbPtI - 1));
    deltaI = (Icc - Iinf) / (nbPtI - 1);
    // Serial.printf("CoefLog[%d]=%f\n",numPt,CoefLog [numPt]);

//*******************************************Mesure résistances*************************************************************** 
    I[numPt] = Iinf + (numPt - 1) * deltaI;
    V[numPt] = Voc;
    Req[numPt] = Voc / I[numPt];
    if (Req[numPt] > 22)
      Req[numPt] = 22;

    dty[numPt] = 1 - Req[numPt] / 22;
  
       /*Affichage pour debug
        Serial.print("numPt : ");
        Serial.print(numPt);
        Serial.print(" V : ");
        Serial.print(V[numPt]);
        Serial.print(" I : ");
        Serial.print(I[numPt]);

        Serial.print(" R : ");
        Serial.println(Req[numPt]);
        */
    
  }

  // For I constance zone
  for (numPt = nbPtI + 1; numPt <= (nbPtV + nbPtI) - 1; numPt++)
  {

    // Coef[numPt] = 1 + (float(numPt - 1) * 9 / (nbPtV - 1));
    deltaV = Voc / (nbPtV - 1);
    I[numPt] = Icc;
    V[numPt] = Voc - (numPt - nbPtI) * deltaV;
    Req[numPt] = V[numPt] / Icc;
     if (Req[numPt] > 22)
      Req[numPt] = 22;

    dty[numPt] = 1 - Req[numPt] / 22;
   

    /*Affichage pour debug
        Serial.print("numPt : ");
        Serial.print(numPt);

        Serial.print(" V : ");
        Serial.print(V[numPt]);

        Serial.print(" I : ");
        Serial.print(I[numPt]);

        Serial.print(" R : ");
        Serial.println(Req[numPt]);
        */ 
      
  }

  /***************************************** Mesure des points de la caracteristique  *************************************/

  for (numPt = 1; numPt <= nbPtI + nbPtV - 1; numPt++)
  {

    ledcWrite(pwmChannel, (int)(dty[numPt]*1023));
    delay(100);

    Vmesure = 0;
    Vcourant_ampli = 0;
    conv = 1;
    int ii;
    for (ii = 0; ii < 1000; ii++)
    { // acquisition 100

      Vmesure = (float)analogReadMilliVolts(32) / 1000 + Vmesure;
      Vcourant_ampli = (float)analogReadMilliVolts(33) / 1000 + Vcourant_ampli;
    }
  
    tab_curr[numPt] = (Vcourant_ampli / 319.48); // mesure du courant moyen + calibration
    tab_volt[numPt] = (Vmesure / 91.56);         // mesure de la tension moyenne + calibration
   if (tab_volt[numPt]<=1.555)
   {
     tab_volt[numPt]=0;
   }
    conv = 0;

    Serial.printf("VI %d %3.3f %3.3f  \r\n", numPt, tab_volt[numPt], tab_curr[numPt]);
    
  }


  digitalWrite(Pinrelay, LOW);
}
