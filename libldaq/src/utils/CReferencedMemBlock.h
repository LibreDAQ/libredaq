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

#include <vector>
#include <utility>

// STL+ library:
#if defined(_MSC_VER)
#	pragma warning(disable:4290) // Visual C++ does not implement decl. specifiers: throw(A,B,...)
#endif
#include "stlplus/smart_ptr.hpp"

namespace libredaq
{
	namespace utils
	{
		/** Represents a memory block (via "void*") that can be shared between several objects through copy operator (=).
		  *  It keeps the reference count and only when it comes to zero, the memory block is really freed.
		 * \ingroup mrpt_base_grp
		  */
		class CReferencedMemBlock : public stlplus::smart_ptr< std::vector<char> >
		{
		typedef stlplus::smart_ptr< std::vector<char> > base_t;
		public:
			/** Constructor with an optional size of the memory block */
			CReferencedMemBlock(size_t mem_block_size = 0 );

			/** Destructor, calls dereference_once. */
			virtual ~CReferencedMemBlock();

			/** Resize the shared memory block. */
			void resize(size_t mem_block_size );

			template <class T> T getAs()
			{
				if (!base_t::present())
					throw std::runtime_error("Trying to access to an uninitialized memory block");

				if( base_t::operator ->()->empty() )
					throw std::runtime_error("Trying to access to a memory block of size 0");

				return reinterpret_cast<T>( & base_t::operator ->()->operator [](0) );
			}

			template <class T> T getAs() const
			{
				if (!base_t::present())
					throw std::runtime_error("Trying to access to an uninitialized memory block");

				if( base_t::operator ->()->empty() )
					throw std::runtime_error("Trying to access to a memory block of size 0");

				return reinterpret_cast<const T>( & base_t::operator ->()->operator [](0) );
			}

		}; // End of class

	} // End of namespace
} // End of namespace

