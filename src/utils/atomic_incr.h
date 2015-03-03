/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  |                                                                         |
  | NOTE: This class is derived from CSerialPort from the MRPT project:     |
  |                     Mobile Robot Programming Toolkit (MRPT)             |
  |                          http://www.mrpt.org/                           |
  |                                                                         |
  | Copyright (c) 2005-2015, Individual contributors, see AUTHORS file      |
  | See: http://www.mrpt.org/Authors - All rights reserved.                 |
  | Released under BSD License. See details in http://www.mrpt.org/License  |
  +-------------------------------------------------------------------------+  */
#pragma once

namespace libredaq
{
namespace synch
{

/** This class acts exactly as an int (or long) variable, but with atomic increment and decrement operators.
  *   This is a useful component of thread-safe smart pointers.
  * \note Based on code from the Boost library.
  * \ingroup synch_grp
  */
class CAtomicCounter
{
public:
#ifdef _WIN32
	typedef long atomic_num_t;
#else
	typedef int atomic_num_t;
#endif

	explicit CAtomicCounter( long v ): m_value( static_cast<atomic_num_t>(v) )
	{ }

	void operator++();  //!< Atomic increment of value.
	atomic_num_t operator--(); 	//!< Atomic decrement of value and return new value.
	operator atomic_num_t() const; //!< Get current value

private:
	mutable atomic_num_t m_value;

	CAtomicCounter( CAtomicCounter const & );	//!< Forbidden method
	CAtomicCounter & operator=( CAtomicCounter const & ); 	//!< Forbidden method
}; // end of CAtomicCounter


} // End of namespace
} // End of namespace

