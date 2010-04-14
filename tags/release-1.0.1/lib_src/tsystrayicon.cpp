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
   \class TSysTrayIcon
   \brief Manages a system tray icon that hides when it's going to be destroyed.

   With Qt4.5, when a program with an icon in the system tray finishes suddenly, it leaves
   a phantom icon in the system tray that uses to disappear when passing the mouse over it.
*/

#include "tsystrayicon.h"

//! Constructor without parameters
/*!
*/
TSysTrayIcon::TSysTrayIcon()
{
   QDEBUG_METHOD_NAME;
}


//! Destructor
/*!
  Hide the icon when finishing.
*/
TSysTrayIcon::~TSysTrayIcon()
{
   QDEBUG_METHOD_NAME;

   if (isSystemTrayAvailable())
   {
      hide();
   }
}
