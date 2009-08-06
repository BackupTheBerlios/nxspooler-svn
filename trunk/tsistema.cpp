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

/*!
   \class TSistema
   \brief Ofrece funciones que pueden usarse desde cualquier objecto del programa.
*/

#include "tsistema.h"

//! Constructor.
/*!
  Será un objeto global.
*/
TSistema::TSistema()
{
   qDebug() << "___" << metaObject()->className() << ":: TSistema()";

   qDebug() << "END" << metaObject()->className() << ":: TSistema()";
}


//! Destructor.
/*!
*/
TSistema::~TSistema()
{
   qDebug() << "___" << metaObject()->className() << ":: ~TSistema()";

   qDebug() << "END" << metaObject()->className() << ":: ~TSistema()";
}


//! Muestra una pregunta y pide que al usuario que acepte o cancele.
/*!
   \param mensaje Pregunta para el usuario
   \return Verdadero si el usuario pulsa el botón de aceptar
*/
bool TSistema::confirmar(const QString &mensaje) const
{
   qDebug() << "___" << metaObject()->className() << ":: confirmar()";

   // Nota: de esta manera aparecían los botones en inglés (ver archivos de traducción, QTranslator, Qt Linguist y demás)
   QMessageBox msgBox(QMessageBox::Question, tr("Confirmación"), mensaje, QMessageBox::Ok|QMessageBox::Cancel);

   int bt = msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: confirmar()";
   return (bt == QMessageBox::Ok);
}


//! Devuelve true si existe el programa cuyo nombre de ejecutable se recibe.
/*!
   Si se recibe una ruta, se comprueba su existencia.
   Si se recibe un nombre de ejecutable, se comprueba si está en el PATH.
   Como NxSpooler permite almacenar el programa como una cadena vacía,
   con un nombre vacío se retornará true.
   \param nombre Ruta o nombre de un ejecutable
   \return Valor true si el programa existe en la ruta o está en el PATH
*/
bool TSistema::existePrograma(const QString &nombre) const
{
   qDebug() << "___" << metaObject()->className() << ":: existePrograma";

   if (nombre.isEmpty())
   {
      qDebug() << "END" << metaObject()->className() << ":: existePrograma AHEAD";
      return true;
   }

   // Según la plataforma, usar un comando u otro para realizar la detección
   QString comando;
#ifdef Q_WS_WIN
   comando = QString("dir \"%1\" >nul").arg(nombre);
#else
   comando = QString("which \"%1\" >/dev/null").arg(nombre);
#endif

   int resultado;
   // Según la plataforma, ejecutar el comando de detección con una u otra codificación
#ifdef Q_WS_WIN
   resultado = system(comando.toLatin1());
#else
   resultado = system(comando.toUtf8());
#endif

   qDebug() << "END" << metaObject()->className() << ":: existePrograma";
   return (resultado == 0);
}


// Muestra un mensaje de aviso al usuario (el usuario sólo puede continuar).
/*!
   \param mensaje Aviso que se le muestra al usuario
*/
void TSistema::mostrarAviso(const QString &mensaje, const QString &titVentana) const
{
   qDebug() << "___" << metaObject()->className() << ":: mostrarAviso";

   QMessageBox msgBox;

   msgBox.setWindowTitle(titVentana == "" ? tr("Aviso"):titVentana);
   msgBox.setText(mensaje);
   msgBox.setIcon(QMessageBox::Warning);
   qDebug() << tr("Aviso: ") << mensaje;
   msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: mostrarAviso";
}


// Muestra un mensaje de error al usuario.
/*!
   \param mensaje Error que se le muestra al usuario
*/
void TSistema::mostrarError(const QString &mensaje, const QString &titVentana) const
{
   qDebug() << "___" << metaObject()->className() << ":: mostrarError()";

   QMessageBox msgBox;

   msgBox.setWindowTitle(titVentana == "" ? tr("Error"):titVentana);
   msgBox.setText(mensaje);
   msgBox.setIcon(QMessageBox::Critical);

   QTextStream cerr(stderr);
   cerr << tr("Error: ") << mensaje << endl;
   msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: mostrarError()";
}
