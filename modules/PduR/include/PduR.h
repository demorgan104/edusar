#ifndef PDUR_H
#define PDUR_H
#include "PduR_Types.h"
extern const PduR_ConfigType PduR_Config;
void PduR_Init(const PduR_ConfigType *config);
Std_ReturnType PduR_ComTransmit(PduIdType id, const PduInfoType *info);
void PduR_CanIfRxIndication(PduIdType id, const PduInfoType *info);
void PduR_CanIfTxConfirmation(PduIdType id);
#endif
