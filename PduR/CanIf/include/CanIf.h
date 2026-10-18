#ifndef CANIF_H
#define CANIF_H
#include "Std_Types.h"
typedef struct { PduIdType pduId; uint16_t canId; uint8_t length; } CanIf_PduType;
/* Driver must consume/copy the payload before returning; E_OK confirms synchronous completion. */
typedef Std_ReturnType (*CanIf_DriverTransmitType)(uint16_t canId, const PduInfoType *info);
typedef struct {
 const CanIf_PduType *txPdus; size_t txCount;
 const CanIf_PduType *rxPdus; size_t rxCount;
} CanIf_ConfigType;
extern const CanIf_ConfigType CanIf_Config;
void CanIf_Init(const CanIf_ConfigType *config, CanIf_DriverTransmitType driver);
Std_ReturnType CanIf_Transmit(PduIdType id, const PduInfoType *info);
void CanIf_RxIndication(uint16_t canId, const PduInfoType *info);
#endif
