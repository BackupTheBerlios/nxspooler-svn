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

/*!
   \class TDebugIndentation
   \brief Auxiliary class of TDebug, to keep levels of indentations.
*/

#include "tdebugindentation.h"


TDebugIndentation::TDebugIndentation() : indentationLevel (0)
{
}

TDebugIndentation::~TDebugIndentation()
{
}

TDebugIndentation & TDebugIndentation::operator++(int)
{
   indentationLevel++;
   return *this;
}

TDebugIndentation & TDebugIndentation::operator--(int)
{
   indentationLevel--;
   return *this;
}

QDebug operator<<(QDebug dbg, const TDebugIndentation &indentation)
{
    for (int x = 0; x < indentation.indentationLevel; x++)
         dbg.nospace() << "  ";

    return dbg.nospace();
}
