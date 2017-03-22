/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <stdint.h>

#pragma pack(push, 1) // exact fit - no padding

/** The numerical values of each OPCODE identifier. See the list of frames below to learn on each OPCODE meaning */
enum ldaq_frame_opcodes_t
{
	// ======================== BOARD -> PC  ==========================
	FRAME_CPU_LOAD    = 0x00,
	// ----
	FRAME_ADC_8bx1    = 0x08,     //!< 1 x 8-bit ADC readings
	FRAME_ADC_16bx1,              //!< 1 x 16-bit ADC readings
	FRAME_ADC_16bx4,              //!< 4 x 16-bit ADC readings
	FRAME_ADC_16bx8,              //!< 8 x 16-bit ADC readings
	FRAME_ADC_24bx1,              //!< 1 x 24-bit ADC readings
	FRAME_ADC_24bx4,              //!< 4 x 24-bit ADC readings
	FRAME_ADC_24bx8,              //!< 4 x 24-bit ADC readings
	// ----
	FRAME_ENC_32bx1   = 0x18,     //!< 1 x 32-bit encoder tick count
	FRAME_ENC_32bx2,              //!< 2 x 32-bit encoder tick count
	FRAME_ENC_32bx4,              //!< 4 x 32-bit encoder tick count
	
	// ======================== PC -> BOARD  ==========================
	FRAMECMD_START_PC2BOARD_CMD_CODES  = 0x50,
	// ------
	FRAMECMD_ADC_START    = 0x50,
	FRAMECMD_ADC_STOP,
	FRAMECMD_ADC_AMP_START,
	FRAMECMD_ADC_AMP_STOP,
	// ----
	FRAMECMD_ENC_START   = 0x70,
	FRAMECMD_ENC_STOP,
	// ----
	FRAMECMD_PWM_INIT    = 0x80,
	FRAMECMD_PWM_CHANGE_PERIOD,
	FRAMECMD_PWM_CHANGE_DUTY,
	FRAMECMD_PWM_STOP,
	// ----
	FRAMECMD_DAC_SET_VALUES  = 0x90,
	// ----
	FRAMECMD_FIRMWARE_MODE = 0xE0,
	// ----
	FRAMECMD_STOP_ALL     = 0xF0
};

/** - SDK: These are the modes available for command FRAMECMD_FIRMWARE_MODE
  * - Firmware: Add new firmware modes here, together with the proper call to ldaq_register_firmware_mode() 
  */
typedef enum
{
	FIRM_MODE_NORMAL = 0,
	FIRM_MODE_HIGHSPEED_ADC,
	
	// === Always leave at the end of the list of modes!! ===
	COUNT_FIRMWARE_MODES
} firmware_mode_t;


#define LDAQ_FRAME_START  0x69
#define LDAQ_FRAME_END    0x96

// ======================== AUXILIARY MACROS  ==========================
#define DECLARE_LDAQ_FRAME_BEGIN(_STRUCT_NAME_)  \
	struct _STRUCT_NAME_  { \
		uint8_t header; \
		uint8_t opcode; \
		uint8_t len;

#ifdef __cplusplus  // C++ version
#	define DECLARE_LDAQ_FRAME_END(_STRUCT_NAME_)  \
		uint8_t tail; \
		_STRUCT_NAME_(const uint8_t opcode_) : header(LDAQ_FRAME_START),opcode(opcode_),len(sizeof(_STRUCT_NAME_)-4), tail(LDAQ_FRAME_END) { } \
	};
#	define DECLARE_LDAQ_FRAME_END_OPCODE(_STRUCT_NAME_,_OPCODE_)  \
		uint8_t tail; \
		_STRUCT_NAME_() : header(LDAQ_FRAME_START),opcode(_OPCODE_),len(sizeof(_STRUCT_NAME_)-4), tail(LDAQ_FRAME_END) { } \
	};
#else  // C version
#	define DECLARE_LDAQ_FRAME_END(_STRUCT_NAME_)  \
		uint8_t tail; \
	}; \
	static inline void _STRUCT_NAME_##_init(struct _STRUCT_NAME_ *f, const uint8_t opcode) { \
		f->header = LDAQ_FRAME_START; \
		f->opcode = opcode; \
		f->len = sizeof(struct _STRUCT_NAME_)-4;\
		f->tail = LDAQ_FRAME_END; \
	}
#	define DECLARE_LDAQ_FRAME_END_OPCODE(_STRUCT_NAME_,_OPCODE_)  \
		uint8_t tail; \
	}; \
	static inline void _STRUCT_NAME_##_init(struct _STRUCT_NAME_ *f) { \
		f->header = LDAQ_FRAME_START; \
		f->opcode = _OPCODE_; \
		f->len = sizeof(struct _STRUCT_NAME_)-4;\
		f->tail = LDAQ_FRAME_END; \
	}
#endif

// ==============================================================================
// ======================== START OF FRAME DEFINITIONS ==========================
// ==============================================================================
DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQDATA_ADC_16bx8)
uint32_t     time;      //!< Timestamp
uint8_t      src_slot;  //!< Source of this measurements: Slot index (0=first,1=second,...) or 0xff means microcontroller embedded sensors.
int16_t      adcs[8];   //!< ADC data
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQDATA_ADC_16bx8, FRAME_ADC_16bx8)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQDATA_ADC_24bx4)
uint32_t     time;      //!< Timestamp
uint8_t      src_slot;  //!< Source of this measurements: Slot index (0=first,1=second,...) or 0xff means microcontroller embedded sensors.
uint8_t      adcs[4*3]; //!< ADC data
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQDATA_ADC_24bx4, FRAME_ADC_24bx4)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQDATA_ENC_32bx4)
uint32_t     time;
uint8_t      src_slot;  //!< Source of this measurements: Slot index (0=first,1=second,...) or 0xff means microcontroller embedded sensors.
uint32_t     tickpos[4];
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQDATA_ENC_32bx4, FRAME_ENC_32bx4)
	
DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQDATA_CPULoad)
uint32_t     time;
uint8_t      cpu_load_percent;
DECLARE_LDAQ_FRAME_END(TFrame_LDAQDATA_CPULoad)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_ADC_Start)
uint8_t   slot;                  //!< Which slot to head this command to (0xff means embedded)
uint32_t  sampling_rate_hz;      //!< Desired ADC sampling rate (in Hz)
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_ADC_Start,FRAMECMD_ADC_START)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_ADC_Stop)
uint8_t   slot;                  //!< Which slot to head this command to (0xff means embedded)
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_ADC_Stop,FRAMECMD_ADC_STOP)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_ADC_AMP_Start)
uint8_t   slot;                  //!< Which slot to head this command to
uint32_t  sampling_rate_hz;         //!< Desired ADC sampling rate (in Hz)
uint16_t  desired_gain;             //!< Desired gain of the input PGA (amplifier)
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_ADC_AMP_Start,FRAMECMD_ADC_AMP_START)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_ADC_Stop_AMP)
uint8_t   slot;                  //!< Which slot to head this command to
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_ADC_Stop_AMP,FRAMECMD_ADC_AMP_STOP)


DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_ENC_Start)
uint8_t   slot;                  //!< Which slot to head this command to (0xff means embedded)
uint32_t  sampling_rate_hz;         //!< Desired sampling rate (in Hz)
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_ENC_Start,FRAMECMD_ENC_START)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_ENC_Stop)
uint8_t   slot;                  //!< Which slot to head this command to (0xff means embedded)
DECLARE_LDAQ_FRAME_END(TFrame_LDAQCMD_ENC_Stop)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_StopAllTasks)
// None
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_StopAllTasks,FRAMECMD_STOP_ALL)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_DAC_SetValues)
uint8_t   slot;                  //!< Which slot to head this command to
uint16_t  dac_values[4];
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_DAC_SetValues,FRAMECMD_DAC_SET_VALUES)

DECLARE_LDAQ_FRAME_BEGIN(TFrame_LDAQCMD_SwitchFirmwareMode)
uint8_t new_firmware_mode;
DECLARE_LDAQ_FRAME_END_OPCODE(TFrame_LDAQCMD_SwitchFirmwareMode,FRAMECMD_FIRMWARE_MODE)


#pragma pack(pop)

