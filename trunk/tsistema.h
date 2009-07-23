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

// Librería genérica de uso global en todo el proyecto.
// Inclusiones, definiciones y una clase para la gestión errores.

#ifndef TSISTEMA_H
#define TSISTEMA_H

#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtGui/QAbstractButton>
#include <QtGui/QMessageBox>

#include <stdexcept>
using std::runtime_error;

// Convertir a la codificación UTF8 la cadena de texto especificada
#ifndef _T_TR_UFT8_
#define _T_TR_UTF8_
#define t(cadena) (QObject::trUtf8(cadena))
#endif

class TSistema
			: public QObject
{
	Q_OBJECT

public:
   TSistema();
   ~TSistema();
   bool confirmar(const QString &mensaje) const;
   bool existePrograma(const QString &nombre) const;
   void mostrarAviso(const QString &mensaje, const QString &titVentana = "") const;
   void mostrarError(const QString &mensaje, const QString &titVentana = "") const;
};


// Permitir que cualquier objeto que acceda a esta cabecera
// pueda usar el objeto global que crearemos en el fichero main.cpp
extern TSistema sist;

#endif
