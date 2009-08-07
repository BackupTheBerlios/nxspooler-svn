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

      QtSingleApplication a(argc, argv);

      if (a.isRunning())
      {
         qDebug() << "END main. NxSpooler was already running";
         return EXIT_FAILURE;
      }

      // Nota: estas cadenas no se traducirán
      a.setOrganizationName("Creación y Diseño Ibense");
      a.setOrganizationDomain("cdi-ibense.com");
      a.setApplicationName("NxSpooler");
      a.setApplicationVersion("0.2");

      // Comprobamos si se le han pasado parámetros al programa
      if (argc - 1 != 0)
      {
	 sist.mostrarError(a.tr("NxSpooler does not expect parameters and has been provided with: ") 
                       + QString::number(argc - 1) + ".", a.tr("Error - NxSpooler"));
         qDebug() << "END main. NxSpooler was provided one or several parameters, but none was expected.";
         return EXIT_FAILURE;
      }

      // Ponemos en marcha un QTranslator para que vea si se pueden traducir los mensajes al usuario
      QTranslator translator;

      // The language and country of this locale as a string of the form "language_country", where language is a
      // lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter ISO 3166 country code.
      QString locale = QLocale::system().name();

      // Tries to load a file that contains translations for the source texts used in the program. No error will occur if the
      // file is not found.
      translator.load(QString("nxspooler_") + locale );

      a.installTranslator(&translator);

      TNxSpooler w;

      int resultado = a.exec();

      qDebug() << "END main";
      return resultado;
   }
   catch(std::exception &excep)
   {
      sist.mostrarError(QString(excep.what()) + ".");
      qDebug() << "END main. Error " << excep.what();
      return EXIT_FAILURE;
   }
   catch(...)
   {
      sist.mostrarError(QT_TR_NOOP("An unidentified problem has happened and NxSpooler must be closed."));
      qDebug() << "END main. Unknown error";
      return EXIT_FAILURE;
   }
}
