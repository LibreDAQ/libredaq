/*+-------------------------------------------------------------------------+
  |          Multibody Dynamics Estimators C++ libraries (libmbde)          |
  |                                                                         |
  | Copyright (C) 2014-2015   Individual contributors (see AUTHORS files)   |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include "libredaq.h"

#include <cstdarg>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace libredaq;

/*---------------------------------------------------------------
				my_vsnprintf
---------------------------------------------------------------*/
int my_vsnprintf(char *buf, size_t bufSize, const char *format, va_list args)
{
#if defined(_MSC_VER)
	#if (_MSC_VER>=1400)
		// Use a secure version in Visual Studio 2005:
		return ::vsnprintf_s (buf, bufSize, _TRUNCATE, format, args);
	#else
		return ::vsprintf(buf,format, args);
	#endif
#else
	// Use standard version:
	return ::vsnprintf(buf, bufSize,format, args);
#endif
}

// A sprintf-like function for std::string
string libredaq::format(const char *fmt, ...)
{
	if (!fmt) return string("");

	int   result = -1, length = 1024;
	vector<char> buffer;
	while (result == -1)
	{
		buffer.resize(length + 10);

		va_list args;  // This must be done WITHIN the loop
		va_start(args,fmt);
		result = my_vsnprintf(&buffer[0], length, fmt, args);
		va_end(args);

		// Truncated?
		if (result>=length) result=-1;
		length*=2;
	}
	string s(&buffer[0]);
	return s;
}
