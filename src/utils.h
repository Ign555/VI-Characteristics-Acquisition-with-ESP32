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

#include <Arduino.h>

#ifndef __UTILS_H__
#define __UTILS_H__

/****************************************************************
*
* Fonctions de convertion
*
****************************************************************/

void bytes_to_float(uint8_t *bytes, float *a);
void float_to_bytes(float *a, uint8_t *bytes);

#endif
