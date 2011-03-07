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

#include <QDir>
#include <QTranslator>
#include <QCoreApplication>
#include <QLibraryInfo>
#include "tdebugstartend.h"

class TTranslator : public QTranslator
{
   Q_OBJECT

private:
   QTranslator m_translatorStandardQtItems; //!< A simple, particular QTranslator for already translated, standard Qt items like "Restore defaults" buttons, "Cancel" buttons, etc. This particular QTranslator must not be destroyed in the constructor of this class.
   QCoreApplication &m_a;  //!< A reference to the main QCoreApplication, for a faster and somewhat clearer access to it.

   QDir folderWhereTheExecutableIs() const;
   void searchAndLoadTranslation(QTranslator &,
                                 const QString &,
                                 const QString &,
                                 const QString &,
                                 const QString &,
                                 const QString &,
                                 const QString &,
                                 const QString &,
                                 const QString &,
                                 const QString &,
                                 const QString & = "");

public:
    TTranslator(QCoreApplication &);
    ~TTranslator();
};

// Allow access to those external objects that deal with the standard input, output...
extern QTextStream cin, cout, cerr;

#endif

