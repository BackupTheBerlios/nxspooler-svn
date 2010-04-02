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
   \class TDebug
   \brief To help developers to know when a method is started and finished, using debug messages.

   An object of the class TDebug uses qDebug() in its constructor to tell when a 
   method is started, and in its destructor to automatically tell when it is finished.
*/

TDebugIndentation TDebug::indentation;


//! Constructor. It must write debug information about the method it's going to be executed.
/*!
  \param methodName The name of the (original) current method.
*/
TDebug::TDebug(const char *methodName) : originalMethodName(methodName)
{
   qDebug() << indentation << "/ " << originalMethodName;

   indentation++;
}


//! Destructor. It must write debug information about the method it's finishing.
/*!
*/
TDebug::~TDebug()
{
   indentation--;

   qDebug() << indentation << "\\ " << originalMethodName;
}
