/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_mod_CY8C9540.h"
#if HAVE_LDAQ_SLOT_BUS


// ----------- Software I2C driver -----------
static void sw_i2c_clear_SDA(void) { 
	PWM_CYPRESS_SDA_PIO->PIO_OER  = 1 << PWM_CYPRESS_SDA_IDX;  // Set as output
	PWM_CYPRESS_SDA_PIO->PIO_CODR = 1 << PWM_CYPRESS_SDA_IDX;
}
static void sw_i2c_set_SDA(void) { 
	PWM_CYPRESS_SDA_PIO->PIO_OER  = 1 << PWM_CYPRESS_SDA_IDX;  // Set as output
	PWM_CYPRESS_SDA_PIO->PIO_SODR = 1 << PWM_CYPRESS_SDA_IDX;
}
static bool sw_i2c_read_SDA(void) {
	PWM_CYPRESS_SDA_PIO->PIO_ODR  = 1 << PWM_CYPRESS_SDA_IDX;  // Set as input
	//	PWM_CYPRESS_SDA_PIO->PIO_PER  = 1 << PWM_CYPRESS_SDA_IDX;
	PWM_CYPRESS_SDA_PIO->PIO_SODR = 1 << PWM_CYPRESS_SDA_IDX;
	NOP_DELAY_4();
	return (PWM_CYPRESS_SDA_PIO->PIO_PDSR & (1 << PWM_CYPRESS_SDA_IDX));
}

static void sw_i2c_clear_SCL(void) {
	PWM_CYPRESS_SCL_PIO->PIO_OER  = 1 << PWM_CYPRESS_SCL_IDX;
//	PWM_CYPRESS_SCL_PIO->PIO_PER  = 1 << PWM_CYPRESS_SCL_IDX;
	PWM_CYPRESS_SCL_PIO->PIO_CODR = 1 << PWM_CYPRESS_SCL_IDX;
}
static void sw_i2c_set_SCL(void) { 
	PWM_CYPRESS_SCL_PIO->PIO_OER  = 1 << PWM_CYPRESS_SCL_IDX;
//	PWM_CYPRESS_SCL_PIO->PIO_PER  = 1 << PWM_CYPRESS_SCL_IDX;
	PWM_CYPRESS_SCL_PIO->PIO_SODR = 1 << PWM_CYPRESS_SCL_IDX;
}
static bool sw_i2c_read_SCL(void) {
	PWM_CYPRESS_SCL_PIO->PIO_ODR  = 1 << PWM_CYPRESS_SCL_IDX;  // Set as input
	//	PWM_CYPRESS_SCL_PIO->PIO_PER  = 1 << PWM_CYPRESS_SCL_IDX;
	PWM_CYPRESS_SCL_PIO->PIO_SODR = 1 << PWM_CYPRESS_SCL_IDX;
	NOP_DELAY_4();
	return (PWM_CYPRESS_SCL_PIO->PIO_PDSR & (1 << PWM_CYPRESS_SCL_IDX));
}
	
static void sw_i2c_delay(void) { delay_us(5); }

bool private_sw_i2c_started = false;

#define I2C_DO_CLOCK_STRETCHING 0

static void sw_i2c_init(void)
{
	pio_configure_pin(PWM_CYPRESS_SCL_GPIO, PIO_OUTPUT_1 | PIO_PULLUP );
	pio_configure_pin(PWM_CYPRESS_SDA_GPIO, PIO_OUTPUT_1 | PIO_PULLUP | PIO_OPENDRAIN);

	// Bus free + minimum bus-free time:
	sw_i2c_set_SDA();
	sw_i2c_set_SCL();
	delay_us(5);
}

static void sw_i2c_start_condition(void)
{
	if (private_sw_i2c_started) // if started, do a restart condition
	{
		sw_i2c_read_SDA();
		sw_i2c_delay();
		while (sw_i2c_read_SCL() == 0) {  // Clock stretching
			// TODO: Timeout!
		}
		// Repeated start setup time, minimum 4.7us
		sw_i2c_delay();
	}
	if (sw_i2c_read_SDA() == 0) {
		//arbitration_lost();
		return;
	}
	// Start condition:
	sw_i2c_clear_SDA(); sw_i2c_delay();
	sw_i2c_clear_SCL();
	
	private_sw_i2c_started = true;
}

static void sw_i2c_stop_condition(void)
{
	// set SDA to 0
	sw_i2c_clear_SDA(); sw_i2c_delay();
	// Clock stretching
#if I2C_DO_CLOCK_STRETCHING
	while (sw_i2c_read_SCL() == 0) {
		// TODO: timeout
	}
#else
	sw_i2c_read_SCL(); sw_i2c_delay();
#endif
	
	// Stop bit setup time, minimum 4us
	sw_i2c_delay();
	// SCL is high, set SDA from 0 to 1
	if (sw_i2c_read_SDA() == 0) {
		//arbitration_lost();
		return;
	}
	sw_i2c_delay();
	private_sw_i2c_started = false;	
}

static void sw_i2c_write_bit(const bool bit) 
{
	if (bit) sw_i2c_read_SDA();
	else     sw_i2c_clear_SDA();
	// Delay:
	sw_i2c_delay();
	
#if I2C_DO_CLOCK_STRETCHING
	while (sw_i2c_read_SCL() == 0) {
		// TODO: timeout
	}
#else
	sw_i2c_read_SCL(); sw_i2c_delay();
#endif
	// SCL is high, now data is valid
	// If SDA is high, check that nobody else is driving SDA
	if (bit && sw_i2c_read_SDA() == 0) {
		//arbitration_lost();
		return;
	}
	sw_i2c_delay();
	sw_i2c_clear_SCL();
}

// Read a bit from I2C bus
static bool  sw_i2c_read_bit(void) 
{
	bool bit;
	// Let the slave drive data
	sw_i2c_read_SDA();
	sw_i2c_delay();
	while (sw_i2c_read_SCL() == 0) {
		// TODO: timeout
	}
	// SCL is high, now data is valid
	bit = sw_i2c_read_SDA();
	sw_i2c_delay();
	sw_i2c_clear_SCL();
	return bit;
}

// Write a byte to I2C bus. Return 0 if ack by the slave.
static bool i2c_write_byte(bool send_start, bool send_stop, uint8_t byte) 
{
	unsigned int bit;
	bool nack;
	if (send_start) {
		sw_i2c_start_condition();
	}
	for (bit = 0; bit < 8; bit++) {
		sw_i2c_write_bit((byte & 0x80) != 0);
		byte <<= 1;
	}
	nack = sw_i2c_read_bit();
	if (send_stop) {
		sw_i2c_stop_condition();
	}
	return nack;
}

// Read a byte from I2C bus
static uint8_t i2c_read_byte(bool nack, bool send_stop) 
{
	unsigned char byte = 0;
	unsigned bit;
	for (bit = 0; bit < 8; bit++) {
		byte = (byte << 1) | sw_i2c_read_bit();
	}
	sw_i2c_write_bit(nack);
	if (send_stop) {
		sw_i2c_stop_condition();
	}
	return byte;
}

// ==================================



/** Reads one register. Returns false on comms error. */
static bool mod_cy8c9540_i2c_read_register(const uint8_t register_addr, uint8_t *out_value)
{
	bool nack1 = i2c_write_byte(true /*start*/, false /*stop*/, 0x40 | 0x00);  // 0:Write
	if (nack1) return false;
	bool nack2 = i2c_write_byte(false /*start*/, false /*stop*/, register_addr);
	if (nack2) return false;
	bool nack3 = i2c_write_byte(true /*start*/, false /*stop*/, 0x40 | 0x01);  // 1:Read
	if (nack3) return false;
	*out_value = i2c_read_byte(true,true);
	sw_i2c_delay();
	return true;
}

/** Writes one register. Returns false on comms error. */
static bool mod_cy8c9540_i2c_write_register(const uint8_t register_addr, const uint8_t value)
{
	bool nack1 = i2c_write_byte(true /*START*/, false /*stop*/, 0x40 | 0x00);  // 0:Write
	if (nack1) return false;
	bool nack2 = i2c_write_byte(false /*start*/, false /*stop*/, register_addr);
	if (nack2) return false;
	bool nack3 = i2c_write_byte(false /*start*/, true /*STOP*/, value);
	if (nack3) return false;
	sw_i2c_delay();
	return true;
}

/* 
 
*/
void mod_cy8c9540_init()
{
	sw_i2c_init();

	// Send reset pulse:
	// Already done in global reset of all slots.
	
#warning TODO: Handle this init code
	uint8_t chip_id;
	if (!mod_cy8c9540_i2c_read_register(0x2E, &chip_id))
	{
		// Error reading from chip!!
	}
	else
	{
		// Chip ID tells us the number of IO lines:
	}

#if 1
	// Activate PWM:
	mod_cy8c9540_i2c_write_register(0x18, 0x00); // Port Select Register: Select GP0
	mod_cy8c9540_i2c_write_register(0x1C, 0x00); // Port direction Register: Set as outputs
	mod_cy8c9540_i2c_write_register(0x08, 0xff); // Output Port 0: 
	mod_cy8c9540_i2c_write_register(0x1a, 0xff); // Select PWM register: 

	// Setup PWM:
	mod_cy8c9540_i2c_write_register(0x28, 0x07); // Select PWM
	mod_cy8c9540_i2c_write_register(0x29, 0x02); // Config PWM register: Clock = 1.5 MHz
	mod_cy8c9540_i2c_write_register(0x2B, 0x10); // PWM Pulse Width register

	while (1) {
		for (uint8_t i=15; i<200; i+=2)
		{
			// Change PWM pulse width:
			mod_cy8c9540_i2c_write_register(0x28, 0x07); // Select PWM
			mod_cy8c9540_i2c_write_register(0x2B, i); // PWM Pulse Width register
			delay_ms(20);
		}
		for (uint8_t i=200; i>15; i-=2)
		{
			// Change PWM pulse width:
			mod_cy8c9540_i2c_write_register(0x28, 0x07); // Select PWM
			mod_cy8c9540_i2c_write_register(0x2B, i); // PWM Pulse Width register
			delay_ms(20);
		}
	}
#endif
}


#if 0
// Set digital outputs:
{
	mod_cy8c9540_i2c_write_register(0x18, 0x00); // Select GP0
	mod_cy8c9540_i2c_write_register(0x1C, 0x00); // Set as outputs
	mod_cy8c9540_i2c_write_register(0x08, 0x00); // Set low levels
}
#endif


/** Sets one output as PWM */
void mod_cy8c9540_set_pwm(int pin)
{
	//i2c.start
  //i2c.putbyte($42, $18, $00)
  //
  //i2c.start
  //i2c.putbyte($42, $1c, $00)
  //
  //i2c.start
  //i2c.putbyte($42, $08, $00)
	
}


#endif //HAVE_LDAQ_SLOT_BUS

