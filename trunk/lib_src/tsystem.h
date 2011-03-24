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

#ifndef T_SYSTEM_H
#define T_SYSTEM_H

#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtGui/QAbstractButton>
#include <QtGui/QMessageBox>
#include <QApplication>
#include <QProcess>
#include <stdexcept>

#ifdef Q_WS_WIN
   #include <QtCore/qt_windows.h>
#endif

#include "tdebugstartend.h"

using std::runtime_error;

class TSystem : public QObject
{
   Q_OBJECT

private:
   QString m_applicationName;  //!<  The name of the program.

   void showMsgBoxWhenHavingQApp(const QString &, const QString &,
                                 QMessageBox::Icon = QMessageBox::NoIcon) const;


public:
   TSystem();
   ~TSystem();
   bool confirm(const QString &) const;
   bool existsProgram(const QString &) const;
   void showMsgBox(const QString &, const QString &,
                   QMessageBox::Icon = QMessageBox::NoIcon) const;
   void showWarning(const QString &, const QString & = "") const;
   void showError(const QString &, const QString & = "") const;
   int execute(const QString &) const;
   int execute(const QString &, const QStringList &) const;
   void wait(int) const;

   QString setApplicationName(const QString &);
   QString applicationName() const;

   void exitBecauseException(std::exception &);
   void exitBecauseException();
};


extern TSystem syst; //!< Extern object to achieve that any object that has access to this header can use the global object that will be created in the main.cpp file.

// Allow access to those external objects that deal with the standard input, output...
extern QTextStream cin, cout, cerr;

#endif
