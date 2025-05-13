#ifndef __CAN_MESSAGE_H__
#define __CAN_MESSAGE_H__

typedef struct CANMessage
{
  bool extented = false;
  bool RTR = false;
  unsigned int id = 0;
  char len = 0;
  unsigned char data[512] = {0};
} CANMessage;

void onReceive(int packetSize);
void manageCAN();

#endif