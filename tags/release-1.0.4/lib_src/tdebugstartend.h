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

#ifndef T_DEBUG_START_END_H
#define T_DEBUG_START_END_H

#include "tdebugindentation.h"


class TDebugStartEnd
{
   const char *originalMethodName; //!< The name of the (original) current method.

public:

   static TDebugIndentation indentation; //!<  Object that knows the level of indentation that must be used in the debug messages.

   TDebugStartEnd(const char *);
   ~TDebugStartEnd();
};

#ifdef QT_DEBUG
   /*!  Create a TDebugStartEnd. This action also writes, in qDebug(), information about the name of the current method. */
   #define QDEBUG_METHOD_NAME TDebugStartEnd methodName(Q_FUNC_INFO);
#else // We don't execute any statement
   #define QDEBUG_METHOD_NAME
#endif

#endif
