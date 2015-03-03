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

//#include "types.h"

/** This class implements a high-performance stopwatch.
	*  Typical resolution is about 1e-6 seconds.
	*  \note The class is named after the Spanish equivalent of "Tic-Toc" ;-)
	*/
class CTicTac
{
public:
	/** Default constructor. */
	CTicTac();

	/** Destructor. */
	virtual ~CTicTac();

	/** Starts the stopwatch
		* \sa Tac
		*/
	void	Tic();

	/** Stops the stopwatch
		* \return Returns the ellapsed time in seconds.
		* \sa Tic
		*/
	double	Tac();

private:
	unsigned char largeInts[64];

	// Cannot be copied:
	CTicTac(const CTicTac& o);
	CTicTac & operator =(const CTicTac& o); 

}; // End of class def.

