/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#pragma once

#include <string>

namespace libredaq
{
	
	
	
	
	
	/** @name Utility functions
	  * @{ */
	
// Define a decl. modifier for printf-like format checks at compile time:
#ifdef __GNUC__
#	define MBDE_printf_format_check(_FMT_,_VARARGS_)  __attribute__ ((__format__ (__printf__, _FMT_,_VARARGS_)))
#else
#	define MBDE_printf_format_check(_FMT_,_VARARGS_)
#endif
	
	/** A sprintf-like function for std::string */
	std::string format(const char *fmt, ...) MBDE_printf_format_check(1,2);
	
	/** @} */
}
