#include "Com.h"
#include "PduR.h"
#define COM_MAX_SIGNALS 16u
static const Com_ConfigType *active;
static uint16_t received[COM_MAX_SIGNALS];
static uint8_t valid[COM_MAX_SIGNALS];
static uint32_t confirmations;
void Com_Init(const Com_ConfigType *config) {
 size_t i;
 active = config && config->signals && config->count <= COM_MAX_SIGNALS ? config : NULL;
 confirmations = 0;
 for (i = 0; i < COM_MAX_SIGNALS; ++i) { received[i] = 0; valid[i] = 0; }
}
Std_ReturnType Com_SendSignal(uint16_t id, const uint16_t *value) {
 size_t i;
 if (!active || !value) return E_NOT_OK;
 for (i = 0; i < active->count; ++i)
  if (active->signals[i].signalId == id) {
   uint8_t bytes[2] = { (uint8_t)*value, (uint8_t)(*value >> 8) };
   PduInfoType info = { bytes, 2u };
   return PduR_ComTransmit(active->signals[i].txPdu, &info);
  }
 return E_NOT_OK;
}
Std_ReturnType Com_ReceiveSignal(uint16_t id, uint16_t *value) {
 size_t i;
 if (!active || !value) return E_NOT_OK;
 for (i = 0; i < active->count; ++i)
  if (active->signals[i].signalId == id && valid[i]) { *value = received[i]; return E_OK; }
 return E_NOT_OK;
}
void Com_RxIndication(PduIdType id, const PduInfoType *info) {
 size_t i;
 if (!active || !info || !info->SduDataPtr || info->SduLength != 2u) return;
 for (i = 0; i < active->count; ++i)
  if (active->signals[i].rxPdu == id) {
   received[i] = (uint16_t)((uint16_t)info->SduDataPtr[0] | ((uint16_t)info->SduDataPtr[1] << 8));
   valid[i] = 1; return;
  }
}
void Com_TxConfirmation(PduIdType id) {
 size_t i;
 if (!active) return;
 for (i = 0; i < active->count; ++i)
  if (active->signals[i].txPdu == id) { ++confirmations; return; }
}
uint32_t Com_GetTxConfirmationCount(void) { return confirmations; }
