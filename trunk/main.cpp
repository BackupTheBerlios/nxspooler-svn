/*****************************************************************************
*  This file is part of NxSpooler.
*
*  Copyright (C) 2009 by Creación y Diseño Ibense S.L.
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

#include <QTranslator>
#include <QLibraryInfo>

#include "tnxspooler.h"
#include "qtsingleapplication/qtsingleapplication.h"
#include "tsistema.h"

// Objeto global para uso de características comunes
TSistema sist;

// Este programa funciona tanto en Linux como en Windows.
int main(int argc, char *argv[])
{
   try
   {
      qDebug() << "___ main";

      QTextStream cerr(stderr);

      QtSingleApplication a(argc, argv);

      if (a.isRunning())
      {
         QString message = a.tr("NxSpooler was already running");
         throw runtime_error(message.toStdString());
      }

      a.setOrganizationName("Creación y Diseño Ibense");
      a.setOrganizationDomain("cdi-ibense.com");
      a.setApplicationName("NxSpooler");
      a.setApplicationVersion("0.2");

      // Ponemos en marcha un QTranslator para que vea si se pueden traducir los mensajes al usuario
      QTranslator translator;

      // The language and country of this locale as a string of the form "language_country", where language is a
      // lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter ISO 3166 country code.
      QString locale = QLocale::system().name();

      // Tries to load a file that contains translations for the source texts used in the program. The program will
      // continue even if the file is not found
      if ( ! translator.load(QString("nxspooler_") + locale ))
         cerr << a.tr("Warning: the file of the NxSpooler translation for your language has not been found.") << endl;

      a.installTranslator(&translator);

      // Launch a QTranslator for the case of already translated, standard items
      // like "Restore defaults" buttons, "Cancel" buttons, etc.
      QString translations_path;
      translations_path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
      QTranslator translatorStandardItems;

      // Note: the program will continue even if the file is not found
      if ( ! translatorStandardItems.load("qt_" + locale, translations_path))
         cerr << a.tr("Warning: the file of the Qt translation for your language has not been found.") << endl;

      a.installTranslator(&translatorStandardItems);

      // Comprobamos si se le han pasado parámetros al programa
      if (argc - 1 != 0)
      {
         QString message = a.tr("NxSpooler does not expect parameters and has been provided with: ")
                       + QString::number(argc - 1);
         throw runtime_error(message.toStdString());
      }

      TNxSpooler nxspooler;

      int resultado = a.exec();

      qDebug() << "END main";
      return resultado;
   }
   catch(std::exception &excep)
   {
      qDebug() << "END main. Error " << excep.what();

      QApplication auxiliar(argc, argv); // It's needed to show a Qt Dialog later
      sist.mostrarError(QString(excep.what()) + ".", QT_TR_NOOP("Error - NxSpooler"));

      return EXIT_FAILURE;
   }
   catch(...)
   {
      qDebug() << "END main. Unknown error";

      QApplication auxiliar(argc, argv); // It's needed to show a Qt Dialog later
      sist.mostrarError(QT_TR_NOOP("An unidentified problem has happened and NxSpooler must be closed.")
                        , QT_TR_NOOP("Error - NxSpooler"));
      return EXIT_FAILURE;
   }
}
