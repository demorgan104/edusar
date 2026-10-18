#include <assert.h>
#include <stdio.h>
#include "Com.h"
#include "PduR.h"
#include "CanIf.h"
static unsigned calls;
static Std_ReturnType driver(uint16_t id, const PduInfoType *info) {
 assert(id == 0x123u);
 assert(info->SduLength == 2u);
 assert(info->SduDataPtr[0] == 0x34u && info->SduDataPtr[1] == 0x12u);
 ++calls;
 CanIf_RxIndication(id, info);
 return E_OK;
}
static Std_ReturnType fail(uint16_t id, const PduInfoType *info) {
 (void)id; (void)info; return E_NOT_OK;
}
int main(void) {
 uint16_t value = 0x1234u, result = 0;
 uint8_t bytes[9] = { 0x78, 0x56 };
 PduInfoType info = { bytes, 2u };
 assert(Com_SendSignal(0, &value) == E_NOT_OK);
 assert(CanIf_Transmit(20, &info) == E_NOT_OK);
 assert(PduR_ComTransmit(10, &info) == E_NOT_OK);
 Com_Init(&Com_Config); PduR_Init(&PduR_Config); CanIf_Init(&CanIf_Config, driver);
 assert(Com_ReceiveSignal(0, &result) == E_NOT_OK);
 assert(Com_SendSignal(0, &value) == E_OK);
 assert(calls == 1u && Com_GetTxConfirmationCount() == 1u);
 assert(Com_ReceiveSignal(0, &result) == E_OK && result == value);
 assert(Com_SendSignal(999, &value) == E_NOT_OK);
 assert(Com_SendSignal(0, NULL) == E_NOT_OK);
 assert(Com_ReceiveSignal(0, NULL) == E_NOT_OK);
 assert(Com_ReceiveSignal(999, &result) == E_NOT_OK);
 assert(PduR_ComTransmit(999, &info) == E_NOT_OK);
 assert(PduR_ComTransmit(10, NULL) == E_NOT_OK);
 assert(CanIf_Transmit(999, &info) == E_NOT_OK);
 assert(CanIf_Transmit(20, NULL) == E_NOT_OK);
 CanIf_RxIndication(0x777, &info);
 PduR_CanIfRxIndication(999, &info);
 CanIf_RxIndication(0x123, NULL);
 info.SduLength = 9;
 assert(CanIf_Transmit(20, &info) == E_NOT_OK);
 CanIf_RxIndication(0x123, &info);
 info.SduLength = 1;
 assert(CanIf_Transmit(20, &info) == E_NOT_OK);
 CanIf_RxIndication(0x123, &info);
 assert(Com_ReceiveSignal(0, &result) == E_OK && result == value);
 info.SduLength = 2;
 CanIf_RxIndication(0x123, &info);
 assert(Com_ReceiveSignal(0, &result) == E_OK && result == 0x5678u);
 info.SduDataPtr = NULL;
 assert(PduR_ComTransmit(10, &info) == E_NOT_OK);
 CanIf_RxIndication(0x123, &info);
 CanIf_Init(&CanIf_Config, fail);
 assert(Com_SendSignal(0, &value) == E_NOT_OK);
 assert(Com_GetTxConfirmationCount() == 1u);
 Com_Init(&Com_Config);
 assert(Com_GetTxConfirmationCount() == 0u);
 assert(Com_ReceiveSignal(0, &result) == E_NOT_OK);
 Com_Init(NULL); PduR_Init(NULL); CanIf_Init(NULL, NULL);
 assert(Com_SendSignal(0, &value) == E_NOT_OK);
 puts("Stack tests passed");
 return 0;
}
