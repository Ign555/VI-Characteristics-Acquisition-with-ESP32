/****************************************************************
*
*
* Fonctions outils pour les cartes du D2E Laos
* Created by Rayan Grand
* Version : v1.0
* File Creation : 08/05/2025
*
*
****************************************************************/

#include "utils.h"

/****************************************************************
*
* Fonctions de convertion
*
****************************************************************/

void bytes_to_float(uint8_t *bytes, float *a){

    
}

void float_to_bytes(float *a, uint8_t *bytes){

    memcpy(bytes, a, sizeof(float));
    
}