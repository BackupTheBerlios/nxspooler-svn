/*****************************************************************************
*  This file is part of NxSpooler.
*
*  Copyright (C) 2009 by Creación y Diseño Ibense S.L., Arón Galdón Ginés, Toni Asensi Esteve.
*
*  NxSpooler is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  NxSpooler is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with NxSpooler. If not, see http://www.gnu.org/copyleft/gpl.html.
*****************************************************************************/

#include "tdebug.h"

/*!
   \class TDebugStartEnd
   \brief To tell, using debug messages, when a method is started and finished. To keep the debug indentation level, etc.

   An object of the class TDebugStartEnd uses qDebug() in its constructor to tell when a
   method is started, and in its destructor to automatically tell when the
   method is finished. Also the developer, when writing other messages to qDebug, can
   use a consistent indentation level thanks to a TDebugIndentation object in the
   TDebugStartEnd class.
*/

// Initialize this static object
TDebugIndentation TDebugStartEnd::indentation;


//! Constructor. It must write debug information about the method it's going to be executed.
/*!
  \param methodName The name of the (original) current method.
*/
TDebugStartEnd::TDebugStartEnd(const char *methodName) : originalMethodName(methodName)
{
   qDebug() << indentation << "/ " << originalMethodName;

   indentation++;
}


//! Destructor. It must write debug information about the method that it's finishing.
/*!
*/
TDebugStartEnd::~TDebugStartEnd()
{
   indentation--;

   qDebug() << indentation << "\\ " << originalMethodName;
}
