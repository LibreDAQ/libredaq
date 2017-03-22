/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#ifndef  circular_buffer_fixsize_H
#define  circular_buffer_fixsize_H

#include <asf.h>

/** A circular buffer of fixed size 0x100 of elements of type uint8_t.
  * Methods are NOT "thread-safe", so the user is responsible of disabling interrupts as needed.
  */
struct circular_buffer_fixsize
{
	uint8_t data[0x100];
	uint8_t next_read,next_write;
};

static inline void circular_buffer_fixsize_init(struct circular_buffer_fixsize *CB)
{
	CB->next_read  = 0;
	CB->next_write = 0;
}
	
/** Raw access to internal container */
static inline uint8_t  circular_buffer_fixsize_get(struct circular_buffer_fixsize *CB,int i) {
	return CB->data[i];
}
static inline uint8_t* circular_buffer_fixsize_get_ref(struct circular_buffer_fixsize *CB,int i) {
	return &CB->data[i];
}
	
/** Raw access to number of bytes written */
static inline uint8_t circular_buffer_fixsize_writtenBytesCount(struct circular_buffer_fixsize *CB) {return CB->next_write;}

/** Insert a copy of the given element in the buffer.
	* \exception std::out_of_range If the buffer run out of space.
	*/
static inline void circular_buffer_fixsize_push(struct circular_buffer_fixsize *CB,uint8_t d) 
{
	uint8_t i;
	//irqflags_t irqsave = ldaq_enter_cs();
	{
		i=CB->next_write++;
		//if (CB->next_write==CB->next_read) { LED_ON(4); for(;;) {}  } //throw std::out_of_range("push: circular_buffer_fixsize is full"); }
	}
	//ldaq_leave_cs(irqsave);
	CB->data[i]=d;
}

/** Insert an array of elements in the buffer.
	* \exception std::out_of_range If the buffer run out of space.
	*/
static inline void circular_buffer_fixsize_push_many(struct circular_buffer_fixsize *CB,const uint8_t *array_elements, uint8_t count) 
{
	//irqflags_t irqsave = ldaq_enter_cs();
	{
		for (uint8_t i=0;i<count;i++)
		{
			CB->data[CB->next_write++] = array_elements[i];
			//if (CB->next_write==CB->next_read) { LED_ON(4); for(;;) {}  } //throw std::out_of_range("push: circular_buffer_fixsize is full"); }
		}
	}
	//ldaq_leave_cs(irqsave);
}

/** Retrieve an element from the buffer.
	* \exception std::out_of_range If the buffer is empty.
	*/
static inline uint8_t circular_buffer_fixsize_pop(struct circular_buffer_fixsize *CB) {
	uint8_t ret;
	//irqflags_t irqsave = ldaq_enter_cs();
	{
		ret=CB->data[CB->next_read++];
		//if (...) throw std::out_of_range("push: circular_buffer_fixsize is full"); }
	}
	//ldaq_leave_cs(irqsave);
	return ret;
}

/** Peeks at elements from the buffer at the current read position, but without popping them out.
	*/
static inline uint8_t circular_buffer_fixsize_peek(struct circular_buffer_fixsize *CB, uint8_t idx) {
	idx = (CB->next_read + idx); // & 0x100;
	return CB->data[idx];
}

/** Pop a number of elements into a user-provided array.
	* \exception std::out_of_range If the buffer has less elements than requested.
	*/
static inline void circular_buffer_fixsize_pop_many(struct circular_buffer_fixsize *CB,uint8_t *out_array, uint8_t count)
{
	while (count--)
		*out_array++ = circular_buffer_fixsize_pop(CB);
}

/** Return the number of elements available for read ("pop") in the buffer (this is NOT the maximum size of the internal buffer)
	* \sa capacity
	*/
static inline iram_size_t circular_buffer_fixsize_size(struct circular_buffer_fixsize *CB) {
	if (CB->next_write>=CB->next_read)
			return CB->next_write-CB->next_read;
	else	return CB->next_write + (0x100-CB->next_read);
}
	
/** Is empty? */
static inline bool circular_buffer_fixsize_empty(struct circular_buffer_fixsize *CB) {
	bool ret;
	//irqflags_t irqsave = ldaq_enter_cs();
	{
		ret= CB->next_write==CB->next_read;
	}
	//ldaq_leave_cs(irqsave);
	return ret;
}

/** The maximum number of elements that can be written ("push") without rising an overflow error.
	*/
static inline uint8_t circular_buffer_fixsize_available(struct circular_buffer_fixsize *CB) {
	//irqflags_t irqsave = ldaq_enter_cs();
	{
		return (0x100-circular_buffer_fixsize_size(CB))-1;
	}
	//ldaq_leave_cs(irqsave);
}

/** Delete all the stored data, if any. */
static inline void circular_buffer_fixsize_clear(struct circular_buffer_fixsize *CB) {
	//irqflags_t irqsave = ldaq_enter_cs();
	{
		CB->next_write = CB->next_read = 0;
	}
	//ldaq_leave_cs(irqsave);
}

#endif
