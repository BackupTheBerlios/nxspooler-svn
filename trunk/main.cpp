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

#include <QTranslator>
#include <QLibraryInfo>

#include "tnxspooler.h"
#include "qtsingleapplication/qtsingleapplication.h"

// Note: Although the names of files, variables and classes are in English
// and the source code is pretty self-explanatory, most of the remarks are
// still in Spanish due to the origin of this program. Feel free to
// translate to English and change what you want and send the modified files
// to the project; to do this you can visit http://developer.berlios.de/projects/nxspooler/

// Objeto global para uso de características comunes
TSystem syst;

// Este programa funciona en Linux, en Windows y posiblemente en otros sistemas operativos
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

      a.setOrganizationName(QString::fromUtf8("Creación y Diseño Ibense"));
      a.setOrganizationDomain("cdi-ibense.com");
      a.setApplicationName("NxSpooler");
      a.setApplicationVersion("0.2");

      // Ponemos en marcha un QTranslator para que vea si se pueden traducir los mensajes al usuario
      QTranslator translator;

      // The language and country of this locale as a string of the form "language_country", where language is a
      // lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter ISO 3166 country code.
      QString locale = QLocale::system().name();
      QString current_language = locale.section('_', 0, 0);

      // Tries to load a file that contains translations for the source texts used in the program. The program will
      // continue even if the file is not found
      if (!translator.load(QString("nxspooler_") + locale ))
         if (current_language != "en")
             cerr << a.tr("Warning: the file of the NxSpooler translation for your language has not been found.") << endl;

      a.installTranslator(&translator);

      // Launch a QTranslator for the case of already translated, standard items
      // like "Restore defaults" buttons, "Cancel" buttons, etc.
      QString translations_path;
      translations_path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
      QTranslator translatorStandardItems;
      // Note: the program will continue even if the file is not found
      if (!translatorStandardItems.load("qt_" + locale, translations_path))
         if (current_language != "en")
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

      int result = a.exec();

      qDebug() << "END main. The result is:" << result; // Note: we put no space after the ":"
      return result;
   }
   catch(std::exception &excep)
   {
      TSystem::exitBecauseException(excep);
   }
   catch(...)
   {
      TSystem::exitBecauseException();
   }
}
