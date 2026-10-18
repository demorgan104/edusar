#ifndef STD_TYPES_H
#define STD_TYPES_H
#include <stdint.h>
#include <stddef.h>
typedef uint8_t Std_ReturnType;
#define E_OK ((Std_ReturnType)0u)
#define E_NOT_OK ((Std_ReturnType)1u)
typedef uint16_t PduIdType;
typedef uint16_t PduLengthType;
typedef struct { uint8_t *SduDataPtr; PduLengthType SduLength; } PduInfoType;
#endif
