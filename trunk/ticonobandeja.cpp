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
   \class TIconoBandeja
   \brief Gestiona un icono de bandeja forzando que se oculte en su destrucción.

   Con Qt4.5, cuando un programa con icono en bandeja termina abrutamente,
   se queda un icono de bandeja fantasma que suele desaparecer al pasar el ratón sobre él.
*/

#include "ticonobandeja.h"

//! Constructor sin parámetros
/*!
*/
TIconoBandeja::TIconoBandeja()
{
   qDebug()<<"___ TIconoBandeja::TIconoBandeja()";

   qDebug()<<"FIN TIconoBandeja::TIconoBandeja()";
}


//! Destructor
/*!
  Ocultar el icono al terminar.
*/
TIconoBandeja::~TIconoBandeja()
{
   qDebug()<<"___ TIconoBandeja::~TIconoBandeja()";

   if (isSystemTrayAvailable())
   {
      hide();
   }

   qDebug()<<"FIN TIconoBandeja::~TIconoBandeja()";
}
