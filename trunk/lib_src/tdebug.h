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

#ifndef T_DEBUG_H
#define T_DEBUG_H

#include "tdebugindentation.h"


class TDebug
{
   const char *originalMethodName; //!< The name of the (original) current method.

public:

   static TDebugIndentation indentation; //!<  Object that knows the level of indentation that must be used in the debug messages.

   TDebug(const char *);
   ~TDebug();
};

/*!  Define a TDebug that writes in qDebug() information about the name of the current method. */
#ifdef QT_DEBUG
   #define QDEBUG_METHOD_NAME TDebug methodName(__PRETTY_FUNCTION__)
#else
   #define QDEBUG_METHOD_NAME
#endif

#endif
