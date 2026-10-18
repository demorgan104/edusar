#ifndef PDUR_TYPES_H
#define PDUR_TYPES_H
#include "Std_Types.h"
typedef struct { PduIdType source; PduIdType destination; } PduR_RouteType;
typedef struct {
 const PduR_RouteType *txRoutes; size_t txCount;
 const PduR_RouteType *rxRoutes; size_t rxCount;
} PduR_ConfigType;
#endif
