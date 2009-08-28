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
   \class TSystem
   \brief Ofrece funciones que pueden usarse desde cualquier objecto del programa.
*/

#include "tsystem.h"

//! Constructor.
/*!
  Será un objeto global.
*/
TSystem::TSystem()
{
   qDebug() << "___" << metaObject()->className() << ":: TSystem()";

   qDebug() << "END" << metaObject()->className() << ":: TSystem()";
}


//! Destructor.
/*!
*/
TSystem::~TSystem()
{
   qDebug() << "___" << metaObject()->className() << ":: ~TSystem()";

   qDebug() << "END" << metaObject()->className() << ":: ~TSystem()";
}


//! Muestra una pregunta y pide que al usuario que acepte o cancele.
/*!
   \param message Pregunta para el usuario
   \return Verdadero si el usuario pulsa el botón de aceptar
*/
bool TSystem::confirm(const QString &message) const
{
   qDebug() << "___" << metaObject()->className() << ":: confirm()";

   QMessageBox msgBox(QMessageBox::Question, tr("Confirm"), message, QMessageBox::Ok|QMessageBox::Cancel);

   int bt = msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: confirm()";
   return (bt == QMessageBox::Ok);
}


//! Devuelve true si existe el programa cuyo nombre de ejecutable se recibe.
/*!
   Si se recibe una ruta, se comprueba su existencia.
   Si se recibe un nombre de ejecutable, se comprueba si está en el PATH.
   Como NxSpooler permite almacenar el programa como una cadena vacía,
   con un nombre vacío se retornará true.
   \param name Ruta o nombre de un ejecutable
   \return Valor true si el programa existe en la ruta o está en el PATH
*/
bool TSystem::existsProgram(const QString &name) const
{
   qDebug() << "___" << metaObject()->className() << ":: existsProgram";

   if (name.isEmpty())
   {
      qDebug() << "END" << metaObject()->className() << ":: existsProgram AHEAD";
      return true;
   }

   // Según la plataforma, usar un comando u otro para realizar la detección
   QString comando;
#ifdef Q_WS_WIN
   comando = QString("dir \"%1\" >nul").arg(name);
#else
   comando = QString("which \"%1\" >/dev/null").arg(name);
#endif

   int resultado;
   // Según la plataforma, ejecutar el comando de detección con una u otra codificación
#ifdef Q_WS_WIN
   resultado = system(comando.toLatin1());
#else
   resultado = system(comando.toUtf8());
#endif

   qDebug() << "END" << metaObject()->className() << ":: existsProgram";
   return (resultado == 0);
}


// Muestra un mensaje de aviso al usuario (el usuario sólo puede continuar).
/*!
   \param message Warning message that is shown to the user
   \param windowTitle The title that appears in the window
*/
void TSystem::showWarning(const QString &message, const QString &windowTitle) const
{
   qDebug() << "___" << metaObject()->className() << ":: showWarning";

   QTextStream cerr(stderr);
   cerr << "Warning: " << message << endl;

   QMessageBox msgBox;
   msgBox.setWindowTitle(windowTitle == "" ? tr("Warning") : windowTitle);
   msgBox.setText(message);
   msgBox.setIcon(QMessageBox::Warning);
   msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: showWarning";
}


// Muestra un mensaje de error al usuario.
/*!
   \param message Error message that is shown to the user
   \param windowTitle The title that appears in the window
*/
void TSystem::showError(const QString &message, const QString &windowTitle) const
{
   qDebug() << "___" << metaObject()->className() << ":: showError()";

   QTextStream cerr(stderr);
   cerr << "Error: " << message << endl;

   QMessageBox msgBox;
   msgBox.setWindowTitle(windowTitle == "" ? tr("Error"):windowTitle);
   msgBox.setText(message);
   msgBox.setIcon(QMessageBox::Critical);
   msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: showError()";
}


//! Se sale del programa debido a una excepción
/*!
*/
void
TSystem::exitBecauseException(std::exception &excep)
{
    QApplication auxiliary(); // It's needed to avoid the error of showing a Qt dialog without a QApplication
    syst.showError(QString(excep.what()) + ".", tr("Error - ") + qApp->applicationName());

    exit(EXIT_FAILURE);
}


//! Se sale del programa debido a una excepción
/*!
*/
void
TSystem::exitBecauseException()
{
    QApplication auxiliary(); // It's needed to show a Qt dialog later
    syst.showError(tr("An unidentified problem has happened and %1 must be closed.").arg(qApp->applicationName())
                        , tr("Error - ") + qApp->applicationName());

    exit(EXIT_FAILURE);
}
