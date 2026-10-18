#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Com.h"
#include "PduR.h"
#include "CanIf.h"
/* Store the frame between sequential ECU lifecycles, emulating a CAN bus. */
static uint8_t frame[8];
static PduLengthType frameLength;
static uint16_t frameId;
static Std_ReturnType busTransmit(uint16_t canId, const PduInfoType *info) {
 if (!info || info->SduLength > sizeof frame) return E_NOT_OK;
 frameId = canId; frameLength = info->SduLength;
 memcpy(frame, info->SduDataPtr, frameLength);
 return E_OK;
}
int main(int argc, char **argv) {
 uint16_t sent = 1234u, received = 0;
 uint32_t confirmations;
 int json = argc > 1 && strcmp(argv[1], "--json") == 0;
 PduInfoType incoming = { frame, 0u };
 if (argc > 3 || (argc > 1 && !json)) return 2;
 if (argc == 3) {
  char *end;
  unsigned long value;
  errno = 0; value = strtoul(argv[2], &end, 10);
  if (errno || !argv[2][0] || *end || value > 65535ul) return 2;
  sent = (uint16_t)value;
 }
 /* ECU 1: application -> COM -> PduR -> CanIf -> simulated CAN. */
 CanIf_Init(&CanIf_Config, busTransmit); PduR_Init(&PduR_Config); Com_Init(&Com_Config);
 if (Com_SendSignal(0u, &sent) != E_OK) return 1;
 confirmations = Com_GetTxConfirmationCount();
 /* ECU 2: a fresh module lifecycle, with independent receive state. */
 CanIf_Init(&CanIf_Config, busTransmit); PduR_Init(&PduR_Config); Com_Init(&Com_Config);
 incoming.SduLength = frameLength;
 CanIf_RxIndication(frameId, &incoming);
 if (Com_ReceiveSignal(0u, &received) != E_OK || received != sent) return 1;
 if (json) {
  printf("{\"sent\":%u,\"received\":%u,\"canId\":%u,\"bytes\":[%u,%u],\"confirmations\":%u}\n",
   (unsigned)sent, (unsigned)received, (unsigned)frameId,
   (unsigned)frame[0], (unsigned)frame[1], (unsigned)confirmations);
 } else {
  printf("COM sends VehicleSpeed=%u\n", (unsigned)sent);
  printf("CAN TX id=0x%03X data=%02X %02X\n", (unsigned)frameId, (unsigned)frame[0], (unsigned)frame[1]);
  printf("COM receives VehicleSpeed=%u; confirmations=%u\n", (unsigned)received, (unsigned)confirmations);
 }
 return 0;
}
