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

#include "CReferencedMemBlock.h"

using namespace libredaq::utils;

/*---------------------------------------------------------------
						constructor
---------------------------------------------------------------*/
CReferencedMemBlock::CReferencedMemBlock(size_t mem_block_size) :
	base_t( new std::vector<char>(mem_block_size) )
{
}

CReferencedMemBlock::~CReferencedMemBlock()
{
}

/*---------------------------------------------------------------
						resize
---------------------------------------------------------------*/
void CReferencedMemBlock::resize(size_t mem_block_size)
{
	this->operator ->()->resize(mem_block_size);
}

