#include "CanIf.h"
#include "PduR.h"
static const CanIf_ConfigType *active;
static CanIf_DriverTransmitType transmit;
void CanIf_Init(const CanIf_ConfigType *config, CanIf_DriverTransmitType driver) {
 active = config; transmit = driver;
}
Std_ReturnType CanIf_Transmit(PduIdType id, const PduInfoType *info) {
 size_t i;
 if (!active || !transmit || !info || !info->SduDataPtr || info->SduLength > 8u) return E_NOT_OK;
 for (i = 0; i < active->txCount; ++i) {
  const CanIf_PduType *pdu = &active->txPdus[i];
  if (pdu->pduId == id) {
   if (info->SduLength != pdu->length || transmit(pdu->canId, info) != E_OK) return E_NOT_OK;
   PduR_CanIfTxConfirmation(id);
   return E_OK;
  }
 }
 return E_NOT_OK;
}
void CanIf_RxIndication(uint16_t canId, const PduInfoType *info) {
 size_t i;
 if (!active || !info || !info->SduDataPtr || info->SduLength > 8u) return;
 for (i = 0; i < active->rxCount; ++i)
  if (active->rxPdus[i].canId == canId && active->rxPdus[i].length == info->SduLength) {
   PduR_CanIfRxIndication(active->rxPdus[i].pduId, info); return;
  }
}
