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

#ifndef T_TRANSLATOR_H
#define T_TRANSLATOR_H

#include <QtCore/QDebug>
#include <QTranslator>
#include <QApplication>
#include <QLibraryInfo>

#include "tsystem.h"

class TTranslator : public QTranslator
{
   Q_OBJECT

private:
   QTranslator m_translatorStandardItems; //!< A QTranslator for already translated, standard items like "Restore defaults" buttons, "Cancel" buttons, etc.

public:
    TTranslator(QApplication &a);
    ~TTranslator();
};

#endif
