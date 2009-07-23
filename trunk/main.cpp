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
         qDebug() << t("FIN main Se ha hallado proceso de NxSpooler ya en ejecución");
         return EXIT_FAILURE;
      }

      a.setOrganizationName(t("Creación y Diseño Ibense"));
      a.setOrganizationDomain(t("cdi-ibense.com"));
      a.setApplicationName(t("NxSpooler"));
      a.setApplicationVersion(t("0.1"));

      // Comprobamos si se le han pasado parámetros al programa
      if (argc - 1 != 0)
      {
         sist.mostrarError(t("El programa NxSpooler no usa parámetros y se le han pasado: ")
             + QString::number(argc - 1) + ".", t("Error - NxSpooler"));
         qDebug() << t("FIN main Se han pasado parámetros a NxSpooler.");
         return EXIT_FAILURE;
      }
   
      TNxSpooler w;
      int resultado = a.exec();

      qDebug() << "FIN main";
      return resultado;
   }
   catch(std::exception &excep)
   {
      sist.mostrarError(t(excep.what()) + ".");
      qDebug() << "FIN main Error " << excep.what();
      return EXIT_FAILURE;
   }
   catch(...)
   {
      sist.mostrarError(t("Ha ocurrido un error no identificado en NxSpooler y debe cerrarse."));

      qDebug() << "FIN main Error desconocido";
      return EXIT_FAILURE;
   }
}
