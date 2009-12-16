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

#ifndef T_NXSPOOLER_H
#define T_NXSPOOLER_H

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QMenu>
#include <QtGui/QDialog>

#include <QTranslator>

#include "tsystrayicon.h"
#include "toptions.h"
#include "tsystem.h"

// Si estamos en Windows, usaremos la función system() de la librería stdlib.h
#ifdef Q_WS_WIN
#include <stdlib.h>
#endif

#include "ui_aboutDialog.h"
#include "ui_helpDialog.h"
#include "ui_nxspooler.h"

class TNxSpooler : public QDialog, private Ui::NxSpooler
{
   Q_OBJECT

public:
   TNxSpooler(QWidget *parent = 0);
   ~TNxSpooler();

private slots:
   void open();
   void openAboutNxSpooler();
   void openOptions();
   void openHelp();
   void show();
   void showOrHide(QSystemTrayIcon::ActivationReason reason);
   void hide();
   void restoreSettings();

private:
   bool filterAndSortFolder(QDir &folder) const;
   void initializeSettings();
   void prepareTrayIconOrShowProgram();
   void prepareSharedFolder() const;
   void prepareTimer();
   QString getDefaultProgram() const;
   QString getDefaultProgramInLinux() const;

   QSettings m_settings; //!< Objeto gestor de la configuración del programa (carga y guarda automáticamente)
   TSysTrayIcon m_sys_tray_icon; //!< Gestor del icono de bandeja
   int m_default_interval; //!< Número de segundos del intervalo por defecto
   QString m_special_extension; //!< A dot followed by the special extension for text files that contains a path inside
   QStringList m_default_formats; //!< Lista de extensiones a incluir en la detección
   QString m_default_resource; //!< Cadena que se mantendrá constante con el nombre de recurso por defecto
   QString m_default_folder; //!< Cadena que se mantendrá constante con la ruta por defecto
   QTimer m_timer; //!< Objeto que llamará a abrir() a intervalos de tiempo regulares

signals:
   void settingsRestored();
};

#endif
