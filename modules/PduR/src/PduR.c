#include "PduR.h"
#include "CanIf.h"
#include "Com.h"
static const PduR_ConfigType *active;
void PduR_Init(const PduR_ConfigType *config) { active = config; }
Std_ReturnType PduR_ComTransmit(PduIdType id, const PduInfoType *info) {
 size_t i;
 if (!active || !info || !info->SduDataPtr || info->SduLength > 8u) return E_NOT_OK;
 for (i = 0; i < active->txCount; ++i)
  if (active->txRoutes[i].source == id)
   return CanIf_Transmit(active->txRoutes[i].destination, info);
 return E_NOT_OK;
}
void PduR_CanIfRxIndication(PduIdType id, const PduInfoType *info) {
 size_t i;
 if (!active || !info || !info->SduDataPtr || info->SduLength > 8u) return;
 for (i = 0; i < active->rxCount; ++i)
  if (active->rxRoutes[i].source == id) {
   Com_RxIndication(active->rxRoutes[i].destination, info); return;
  }
}
void PduR_CanIfTxConfirmation(PduIdType id) {
 size_t i;
 if (!active) return;
 for (i = 0; i < active->txCount; ++i)
  if (active->txRoutes[i].destination == id) {
   Com_TxConfirmation(active->txRoutes[i].source); return;
  }
}
