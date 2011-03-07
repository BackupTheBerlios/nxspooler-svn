/*****************************************************************************
*  This file is part of NxSpooler.
*
*  Copyright (C) 2009-2011 by Creación y Diseño Ibense S.L., Arón Galdón Ginés, Toni Asensi Esteve.
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

#include "lib_src/tsystrayicon.h"
#include "lib_src/tsystem.h"
#include "lib_src/tdebugstartend.h"
#include "toptions.h"

#include "ui_aboutDialog.h"
#include "ui_helpDialog.h"
#include "ui_nxspooler.h"

class TNxSpooler : public QDialog, private Ui::NxSpooler
{
   Q_OBJECT

public:

   //! This enum describes different results of trying to open a file.
   enum ResultOfOpening {
         OpeningOk = 0, //!< There was no error found opening the file.
         OpeningError //!< There was an error opening the file.
         // In the past it also was: OpeningNotShouldBeDone //!< The NxSpooler was configured so that the file should not be opened.
         };

   //! This enum describes different situations where a "path" to open is found
   enum SituationOfAPathToOpen {
         PathFoundInsideAContainerFile = 0, //!< The path was found inside a container file.
         PathNotFoundInsideAContainerFile //!< The path was not found inside a container file.
         };

   TNxSpooler(QWidget * = 0);
   ~TNxSpooler();

private slots:
   void detectFilesAndOpen();
   void openAboutNxSpooler();
   void openOptions();
   void openHelp();
   void show();
   void showOrHide(QSystemTrayIcon::ActivationReason);
   void hide();
   void restoreSettings();

private:
   bool filterAndSortFolder(QDir &) const;
   void initializeSettings();
   void prepareTrayIconOrShowProgram();
   void prepareSharedFolder() const;
   void prepareTimer();
   ResultOfOpening openPath(QFileInfo &, const QString &,
                            TNxSpooler::SituationOfAPathToOpen = PathNotFoundInsideAContainerFile) const;
   ResultOfOpening openPathWrittenInside(const QString &);
   QString getDefaultProgram() const;
   QString getDefaultProgramInLinux() const;
   void removeExtensionsThatDoNotMustBeOpened(QStringList &extsToDetect) const;

   TSysTrayIcon m_sys_tray_icon; //!< Object that manages the system tray icon.
   QTimer m_timer; //!< Every X seconds this object will try to detect new files and open them.
   const QString m_special_extension; //!< The special extension for  text files that contain a path inside to be opened by NxSpooler.
   const QString m_usual_shared_resource; //!< The usual name of the shared resource in the client computer.
   QSettings m_settings; //!< Object that manages (loads and saves automatically) the configuration of NxSpooler.

   // The default values for the settings of NxSpooler
   const int m_default_interval; //!< The default quantity of seconds to wait between examinations.
   const QStringList m_default_formats; //!< The files to detect will have extensions; this is a list of default extensions to identify.
   const QString m_default_folder; //!< The name of the default local folder to monitor.

signals:
   void settingsRestored();
};

#endif
