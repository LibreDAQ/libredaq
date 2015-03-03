/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <stdint.h>

#if !defined(__AVR_MEGA__)
#	pragma pack(push, 1) // exact fit - no padding
#endif

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
	FRAMECMD_ENC_START    = 0x98,
	FRAMECMD_ENC_STOP,
	// ----
	
};

#define LDAQ_FRAME_START  0x69
#define LDAQ_FRAME_END    0x96

template <uint8_t NUM_ADCS, typename ADC_VALUE_TYPE>
struct TFrameDAQ_ADC
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	timestamp_t     time;
	ADC_VALUE_TYPE  adcs[NUM_ADCS];
	// -------------------------------
	const uint8_t tail;
	
	TFrameDAQ_ADC(const uint8_t opcode_) :
		header(LDAQ_FRAME_START),
		opcode(opcode_),
		len(sizeof(TFrameDAQ_ADC<NUM_ADCS,ADC_VALUE_TYPE>)-4),
		tail(LDAQ_FRAME_END)
	{
	}
};

template <uint8_t NUM_ENCODERS, typename TICKCOUNT_VALUE_TYPE>
struct TFrameDAQ_ENCODERS
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	timestamp_t           time;
	TICKCOUNT_VALUE_TYPE  tickpos[NUM_ENCODERS];
	// -------------------------------
	const uint8_t tail;
	
	TFrameDAQ_ENCODERS(const uint8_t opcode_) :
		header(LDAQ_FRAME_START),
		opcode(opcode_),
		len(sizeof(TFrameDAQ_ENCODERS)-4),
		tail(LDAQ_FRAME_END)
	{
	}
};

struct TFrameDAQ_CPULoad
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	timestamp_t     time;
	uint8_t         cpu_load_percent;
	// -------------------------------
	const uint8_t tail;
	
	TFrameDAQ_CPULoad() :
		header(LDAQ_FRAME_START),
		opcode(FRAME_CPU_LOAD),
		len(sizeof(TFrameDAQ_CPULoad)-4),
		tail(LDAQ_FRAME_END)
	{
	}
};

struct TFrameDAQ_ADC_Start
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	uint8_t  sampling_rate_khz;         //!< Desired ADC sampling rate (in kHz)
	// -------------------------------
	const uint8_t tail;
	
	TFrameDAQ_ADC_Start() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ADC_START),
		len(sizeof(TFrameDAQ_ADC_Start)-4),
		tail(LDAQ_FRAME_END)
	{
	}
};

struct TFrameDAQ_ADC_Stop
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	
	// -------------------------------
	const uint8_t tail;
	
	TFrameDAQ_ADC_Stop() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ADC_STOP),
		len(sizeof(TFrameDAQ_ADC_Stop)-4),
		tail(LDAQ_FRAME_END)
	{
	}
};

struct TFrameDAQ_ENC_Start
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	uint8_t  sampling_rate_khz;         //!< Desired sampling rate (in kHz)
	// -------------------------------
	const uint8_t tail;
	
	TFrameDAQ_ENC_Start() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ENC_START),
		len(sizeof(TFrameDAQ_ENC_Start)-4),
		tail(LDAQ_FRAME_END)
	{
	}
};

struct TFrameDAQ_ENC_Stop
{
	const uint8_t header;
	const uint8_t opcode;
	const uint8_t len;
	// ----------- Payload -----------
	
	// -------------------------------
	const uint8_t tail;
	
	TFrameDAQ_ENC_Stop() :
		header(LDAQ_FRAME_START),
		opcode(FRAMECMD_ENC_STOP),
		len(sizeof(TFrameDAQ_ENC_Stop)-4),
		tail(LDAQ_FRAME_END)
	{
	}
};

#if !defined(__AVR_MEGA__)
#	pragma pack(pop)
#endif
