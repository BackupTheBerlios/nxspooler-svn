/*****************************************************************************
*  This file is part of NxSpooler.
*
*  Copyright (C) 2009-2010 by Creación y Diseño Ibense S.L., Arón Galdón Ginés, Toni Asensi Esteve.
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
#include "tsystem.h"
#include "tdebugstartend.h"
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

   QSettings m_settings; //!< Object that manages (loads and saves automatically) the configuration of the program.
   TSysTrayIcon m_sys_tray_icon; //!< Object that manages the system tray icon.
   int m_default_interval; //!< Default quantity of seconds to wait.
   QString m_special_extension; //!< A dot followed by the special extension for text files that contain a path inside.
   QStringList m_default_formats; //!< The files to detect will have extensions; this is a list of default extensions to identify.
   QString m_default_shared_resource; //!< Constant string of the name of the default shared resource.
   QString m_default_folder; //!< Constant string of the name of the default local folder to monitor.
   QTimer m_timer; //!< Every X seconds this object will try to detect new files and open them.

signals:
   void settingsRestored();
};

#endif
