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

enum ldaq_frame_opcodes_t
{
	// ======================== BOARD -> PC  ==========================
	FRAME_CPU_LOAD    = 0x00,
	// ----
	FRAME_ADC16b_x8   = 0x08,     // 8 x 16-bit ADC readings
	FRAME_ADC16b_x16,             // 16 x 16-bit ADC readings
	FRAME_ADC16b_x24,             // 24 x 16-bit ADC readings
	FRAME_ADC16b_x32,             // 32 x 16-bit ADC readings
	FRAME_ADC16b_x40,             // 40 x 16-bit ADC readings
	// ----
	FRAME_ENC32b_x2   = 0x18,     // 2 x 32-bit encoder tick count
	FRAME_ENC32b_x4,              // 4 x 32-bit encoder tick count
	FRAME_ENC32b_x6,              // 6 x 32-bit encoder tick count
	
	// ======================== PC -> BOARD  ==========================
	FRAMECMD_START_PC2BOARD_CMD_CODES  = 0x50,
	// ------
	FRAMECMD_ADC_START    = 0x50,
	FRAMECMD_ADC_STOP,
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

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_ADC) // Payload:
uint32_t     time;
int16_t      adcs[8];
DECLARE_LDAQ_FRAME_END(TFrameDAQ_ADC)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_ENC) // Payload:
uint32_t     time;
uint32_t     tickpos[4];
DECLARE_LDAQ_FRAME_END(TFrameDAQ_ENC)
	
DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_CPULoad) // Payload:
uint32_t     time;
uint8_t         cpu_load_percent;
DECLARE_LDAQ_FRAME_END(TFrameDAQ_CPULoad)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_ADC_Start) // Payload:
uint32_t  sampling_rate_hz;         //!< Desired ADC sampling rate (in Hz)
DECLARE_LDAQ_FRAME_END_OPCODE(TFrameDAQ_ADC_Start,FRAMECMD_ADC_START)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_ADC_Stop) // Payload:
// None
DECLARE_LDAQ_FRAME_END(TFrameDAQ_ADC_Stop)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_ENC_Start) // Payload:
uint32_t  sampling_rate_hz;         //!< Desired sampling rate (in Hz)
DECLARE_LDAQ_FRAME_END_OPCODE(TFrameDAQ_ENC_Start,FRAMECMD_ENC_START)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_ENC_Stop) // Payload:
// None
DECLARE_LDAQ_FRAME_END(TFrameDAQ_ENC_Stop)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_StopAllTasks) // Payload:
// None
DECLARE_LDAQ_FRAME_END_OPCODE(TFrameDAQ_StopAllTasks,FRAMECMD_STOP_ALL)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_DAC_SetValues) // Payload:
uint16_t dac_values[4];
DECLARE_LDAQ_FRAME_END_OPCODE(TFrameDAQ_DAC_SetValues,FRAMECMD_DAC_SET_VALUES)

DECLARE_LDAQ_FRAME_BEGIN(TFrameDAQ_SwitchFirmwareMode) // Payload:
uint8_t new_firmware_mode;
DECLARE_LDAQ_FRAME_END_OPCODE(TFrameDAQ_SwitchFirmwareMode,FRAMECMD_FIRMWARE_MODE)


#pragma pack(pop)

