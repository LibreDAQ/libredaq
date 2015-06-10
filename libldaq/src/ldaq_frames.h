/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <stdint.h>

//#if !defined(__AVR_MEGA__) && !defined(__arm__)
#pragma pack(push, 1) // exact fit - no padding
//#endif

enum ldaq_frame_opcodes_t
{
	// ======================== BOARD -> PC  ==========================
	FRAME_CPU_LOAD    = 0x10,
	// ----
	FRAME_ADC16b_x8   = 0x20,     // 8 x 16-bit ADC readings
	FRAME_ADC16b_x16,             // 16 x 16-bit ADC readings
	FRAME_ADC16b_x24,             // 24 x 16-bit ADC readings
	FRAME_ADC16b_x32,             // 32 x 16-bit ADC readings
	FRAME_ADC16b_x40,             // 40 x 16-bit ADC readings
	// ----
	FRAME_ENC32b_x2   = 0x30,     // 2 x 32-bit encoder tick count
	FRAME_ENC32b_x4,              // 4 x 32-bit encoder tick count
	FRAME_ENC32b_x6,              // 6 x 32-bit encoder tick count
	
	// ======================== PC -> BOARD  ==========================
	FRAMECMD_ADC_START    = 0x90,
	FRAMECMD_ADC_STOP,
	// ----
	FRAMECMD_ENC_START,
	FRAMECMD_ENC_STOP,
	// ----
	FRAMECMD_STOP_ALL
	
};

#define LDAQ_FRAME_START  0x69
#define LDAQ_FRAME_END    0x96

struct TFrameDAQ_ADC
{
	uint8_t header;
	uint8_t opcode;
	uint8_t len;
	// ----------- Payload -----------
	uint32_t     time;
	int16_t      adcs[8]; // ADC_VALUE_TYPE, NUM_ADCS
	// -------------------------------
	uint8_t tail;
	
#ifdef __cplusplus  // C++ version
	TFrameDAQ_ADC(const uint8_t opcode_) :
		header(LDAQ_FRAME_START),
		opcode(opcode_),
		len(sizeof(TFrameDAQ_ADC)-4), // <NUM_ADCS,ADC_VALUE_TYPE>
		tail(LDAQ_FRAME_END)
	{
	}
};
#else  // C version
};
static inline void TFrameDAQ_ADC_init(struct TFrameDAQ_ADC *f, const uint8_t opcode) {
	f->header = LDAQ_FRAME_START;
	f->opcode = opcode;
	f->len = sizeof(struct TFrameDAQ_ADC)-4;  // ADC_VALUE_TYPE, NUM_ADCS
	f->tail = LDAQ_FRAME_END;
}
#endif

struct TFrameDAQ_ENC
{
	uint8_t header;
	uint8_t opcode;
	uint8_t len;
	// ----------- Payload -----------
	uint32_t     time;
	uint32_t     tickpos[4];
	// -------------------------------
	uint8_t tail;
	
	#ifdef __cplusplus  // C++ version
	TFrameDAQ_ENC(const uint8_t opcode_) :
	header(LDAQ_FRAME_START),
	opcode(opcode_),
	len(sizeof(TFrameDAQ_ENC)-4),
	tail(LDAQ_FRAME_END)
	{
	}
};
#else  // C version
};
static inline void TFrameDAQ_ENC_init(struct TFrameDAQ_ENC *f, const uint8_t opcode) {
	f->header = LDAQ_FRAME_START;
	f->opcode = opcode;
	f->len = sizeof(struct TFrameDAQ_ENC)-4;
	f->tail = LDAQ_FRAME_END;
}
#endif


struct TFrameDAQ_CPULoad
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	uint32_t     time;
	uint8_t         cpu_load_percent;
	// -------------------------------
	const uint8_t tail;
	
#ifdef __cplusplus
	TFrameDAQ_CPULoad() :
		header(LDAQ_FRAME_START),
		opcode(FRAME_CPU_LOAD),
		len(sizeof(TFrameDAQ_CPULoad)-4),
		tail(LDAQ_FRAME_END)
	{
	}
#endif
};

struct TFrameDAQ_ADC_Start
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	uint32_t  sampling_rate_hz;         //!< Desired ADC sampling rate (in Hz)
	// -------------------------------
	const uint8_t tail;
	
#ifdef __cplusplus
	TFrameDAQ_ADC_Start() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ADC_START),
		len(sizeof(TFrameDAQ_ADC_Start)-4),
		tail(LDAQ_FRAME_END)
	{
	}
#endif
};

struct TFrameDAQ_ADC_Stop
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	
	// -------------------------------
	const uint8_t tail;
	
#ifdef __cplusplus
	TFrameDAQ_ADC_Stop() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ADC_STOP),
		len(sizeof(TFrameDAQ_ADC_Stop)-4),
		tail(LDAQ_FRAME_END)
	{
	}
#endif
};

struct TFrameDAQ_ENC_Start
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	uint32_t  sampling_rate_hz;         //!< Desired ADC sampling rate (in Hz)
	// -------------------------------
	const uint8_t tail;
	
#ifdef __cplusplus
	TFrameDAQ_ENC_Start() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ENC_START),
		len(sizeof(TFrameDAQ_ENC_Start)-4),
		tail(LDAQ_FRAME_END)
	{
	}
#endif
};

struct TFrameDAQ_ENC_Stop
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	
	// -------------------------------
	const uint8_t tail;
	
#ifdef __cplusplus
	TFrameDAQ_ENC_Stop() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ENC_STOP),
		len(sizeof(TFrameDAQ_ENC_Stop)-4),
		tail(LDAQ_FRAME_END)
	{
	}
#endif
};

struct TFrameDAQ_StopAllTasks
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	
	// -------------------------------
	const uint8_t tail;
	
#ifdef __cplusplus
	TFrameDAQ_StopAllTasks() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_STOP_ALL),
		len(sizeof(TFrameDAQ_StopAllTasks)-4),
		tail(LDAQ_FRAME_END)
	{
	}
#endif
};


//#if !defined(__AVR_MEGA__) && !defined(__arm__)
#	pragma pack(pop)
//#endif

