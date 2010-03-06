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

/*
// To avoid errors when compiling in Windows, we have followed what it was told in
// http://forums.codeblocks.org/index.php/topic,10508.msg72241.html#msg72241 and added
// two "ifndef"s and "endif"s in the cwchar file, being the result there like this:
#ifndef __STRICT_ANSI__
  using ::swprintf;
#endif

#ifndef __STRICT_ANSI__
  using ::vswprintf;
#endif
*/

#include <QTranslator>
#include <QLibraryInfo>

#include "qtsingleapplication/qtsingleapplication.h"
#include "tnxspooler.h"
#include "ttranslator.h"

// Feel free to improve NxSpooler and visit http://developer.berlios.de/projects/nxspooler/

// Global object to provide services common to several parts of the program
TSystem syst;

// Objects that deal with the standard input, output...
QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cin(stdin, QIODevice::ReadOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

// NxSpooler works in Linux, in Windows and probably in other operating systems
int main(int argc, char *argv[])
{
   try
   {
      qDebug() << "___ main";

      QtSingleApplication a(argc, argv);
      a.setOrganizationName(QString::fromUtf8("Creación y Diseño Ibense"));
      a.setOrganizationDomain("cdi-ibense.com");
      a.setApplicationName(syst.setApplicationName("NxSpooler"));
      a.setApplicationVersion("0.2");

      // Start a QTranslator to try to translate the next messages that the user sees
      TTranslator translator(a);

      // Check if another NxSpooler instance was running
      if (a.isRunning())
      {
         QString message = TSystem::tr("NxSpooler was already running");
         throw runtime_error(message.toStdString());
      }

      // Check if any argument was passed to NxSpooler
      if (argc - 1 != 0)
      {
         QString message = TSystem::tr("NxSpooler does not expect parameters and has been provided with: ")
                       + QString::number(argc - 1);
         throw runtime_error(message.toStdString());
      }

      TNxSpooler nxspooler;

      int result = a.exec();

      qDebug() << "END main. The result is:" << result; // Note: we put no space after the ":"
      return result;
   }
   catch(std::exception &excep)
   {
       syst.exitBecauseException(excep);
   }
   catch(...)
   {
       syst.exitBecauseException();
   }
}

