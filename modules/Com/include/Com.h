#ifndef COM_H
#define COM_H
#include "Std_Types.h"
typedef struct { uint16_t signalId; PduIdType txPdu; PduIdType rxPdu; } Com_SignalType;
typedef struct { const Com_SignalType *signals; size_t count; } Com_ConfigType;
extern const Com_ConfigType Com_Config;
/* This educational profile supports one unsigned 16-bit little-endian signal per PDU. */
void Com_Init(const Com_ConfigType *config);
Std_ReturnType Com_SendSignal(uint16_t id, const uint16_t *value);
Std_ReturnType Com_ReceiveSignal(uint16_t id, uint16_t *value);
void Com_RxIndication(PduIdType id, const PduInfoType *info);
void Com_TxConfirmation(PduIdType id);
uint32_t Com_GetTxConfirmationCount(void);
#endif
