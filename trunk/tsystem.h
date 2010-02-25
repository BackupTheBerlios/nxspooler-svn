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

// Class to provide services common to several parts of the program, like methods, definitions, a class to manage errors, etc.

#ifndef T_SYSTEM_H
#define T_SYSTEM_H

#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtGui/QAbstractButton>
#include <QtGui/QMessageBox>
#include <QApplication>
#include <QProcess>
#include <stdexcept>

using std::runtime_error;

class TSystem : public QObject
{
   Q_OBJECT

public:
   TSystem();
   ~TSystem();
   bool confirm(const QString &message) const;
   bool existsProgram(const QString &name) const;
   void showWarning(const QString &message, const QString &windowTitle = "") const;
   void showError(const QString &message, const QString &windowTitle = "") const;
   int execute(const QString &program) const;
   int execute(const QString &program, const QStringList &arguments) const;

   void exitBecauseException(std::exception &excep);
   void exitBecauseException();
};


// Allow that any object that has access to this header
// can use the global object that will be created in the main.cpp file
extern TSystem syst;

#endif
