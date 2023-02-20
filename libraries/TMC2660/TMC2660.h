/*
 * TMC2660.h
 *
 *  Created on: 06.07.2017
 *      Author: LK
 */

#ifndef TMC_IC_TMC2660_H_
#define TMC_IC_TMC2660_H_

#include <stdint.h>
#include "TMC2660_Register.h"
#include "TMC2660_Constants.h"
#include "TMC2660_Fields.h"

#define TMC_ACCESS_NONE        0x00

#define TMC_ACCESS_READ        0x01
#define TMC_ACCESS_WRITE       0x02

typedef enum {
    TMC_ERROR_NONE      = 0x00,
    TMC_ERROR_GENERIC   = 0x01,
    TMC_ERROR_FUNCTION  = 0x02,
    TMC_ERROR_MOTOR     = 0x08,
    TMC_ERROR_VALUE     = 0x10,
    TMC_ERROR_CHIP      = 0x40
} TMCError;

typedef enum {
    TMC_COMM_DEFAULT,
    TMC_COMM_SPI,
    TMC_COMM_UART
} TMC_Comm_Mode;

// Callback functions have IC-dependent parameters
// To store the function pointers we use this dummy type, which is never
// called without casting it to the IC-specific type first.
// (Casting between function pointers is allowed by the C standard)
typedef void (*tmc_callback_config)(void);

// States of a configuration
typedef enum {
    CONFIG_READY,
    CONFIG_RESET,
    CONFIG_RESTORE
} ConfigState;

// structure for configuration mechanism
typedef struct
{
    ConfigState          state;
    uint8_t                configIndex;
    int32_t                shadowRegister[TMC_REGISTER_COUNT];
    uint8_t (*reset)       (void);
    uint8_t (*restore)     (void);
    tmc_callback_config  callback;
    uint8_t                   channel;
} ConfigurationTypeDef;

#define TMC2660_FIELD_READ(motor, address, mask, shift)           FIELD_READ(tmc2660_readInt, motor, address, mask, shift)
#define TMC2660_FIELD_WRITE(motor, address, mask, shift, value)   FIELD_WRITE(tmc2660_writeInt, motor, address, mask, shift, value)
#define TMC2660_FIELD_UPDATE(motor, address, mask, shift, value)  FIELD_UPDATE(tmc2660_readInt, tmc2660_writeInt, motor, address | TMC2660_WRITE_BIT, mask, shift, value)

// Usage note: use 1 TypeDef per IC
typedef struct {
	uint8_t standStillCurrentScale;
	uint32_t standStillTimeout;
	uint8_t isStandStillOverCurrent;
	uint8_t isStandStillCurrentLimit;
	uint8_t continuousModeEnable;
	uint8_t runCurrentScale;
	uint8_t coolStepInactiveValue;
	uint8_t coolStepActiveValue;
	uint32_t coolStepThreshold;
	int velocity;
	int oldX;
	uint32_t oldTick;
	uint8_t registerAccess[TMC2660_REGISTER_COUNT];
	int32_t registerResetState[TMC2660_REGISTER_COUNT];
} TMC2660TypeDef;

void tmc2660_initConfig(TMC2660TypeDef *TMC2660);
void tmc2660_periodicJob(uint8_t motor, uint32_t tick, TMC2660TypeDef *TMC2660, ConfigurationTypeDef *TMC2660_config);
uint8_t tmc2660_reset(TMC2660TypeDef *TMC2660, ConfigurationTypeDef *TMC2660_config);
uint8_t tmc2660_restore(ConfigurationTypeDef *TMC2660_config);

#endif /* TMC_IC_TMC2660_H_ */
