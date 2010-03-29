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
   \class TNxSpooler
   \brief Regularly opens and deletes files with specific extensions that appear in a path.

   An object of the class TNxSpooler has the aim of checking every X seconds
   if files with specific extensions appear in a concrete path, to open the files
   and later delete them.
   This path is normally shared so that a FreeNX connection can use it. This way,
   files that are generated by the server in the concrete path will be opened in
   the computer of the user.
*/

#include "tnxspooler.h"

//! Builds a TNxSpooler object, attaching it to a parent.
/*!
   If there are no exceptions to stop it, the TNxSpooler object is constructed.
   \param qwidgetParent If "qwidgetParent" is not specified, the TNxSpooler will be an independent window.
*/
TNxSpooler::TNxSpooler(QWidget *qwidgetParent)
   : QDialog(qwidgetParent)
{
   QDEBUG_METHOD_NAME;

   // "Adopt" member objects that need this
   m_settings.setParent(this);
   m_sys_tray_icon.setParent(this);
   m_timer.setParent(this);

   // Define constant values by default
   m_default_interval = 3;
   m_special_extension = ".nxspooler-open"; // A special extension for files that contain a path to be opened by NxSpooler
   m_default_formats.append("pdf");
   m_default_formats.append("ods");
   m_default_formats.append("sxc");
   m_default_resource = "nxspooler$";
   m_default_folder = QDir::toNativeSeparators(QDir::homePath().append(QDir::separator()).append(".nxspooler"));

   setupUi(this);
   prepareTrayIconOrShowProgram();

   try
   {
      initializeSettings();
      prepareSharedFolder();
   }
   catch (std::exception &excep)
   {
      // If the path did not exist and the path could not be created, show the options dialog
      if (QString(excep.what()).startsWith("2805093"))
      {
         syst.showWarning(excep.what());
         show();
         openOptions();
      }
      else
      {
         throw excep;
      }
   }

   prepareTimer();
}


//! Destroys TNxSpooler.
/*!
   This destructor is used only to debugging purposes, it could be undefined.
*/
TNxSpooler::~TNxSpooler()
{
   QDEBUG_METHOD_NAME;
}


//! In the spool folder, detect files, open and delete them (if possible).
/*!
  Files with the special extensions will contain a path to be open.
*/
void TNxSpooler::detectFilesAndOpen()
{
   // As this is a slot that can be called by Qt code (in response to a call
   // made by m_timer, for example), we don't allow exceptions to go out
   // from here, so we use a "try" block.
   try
   {
      QDEBUG_METHOD_NAME;

      // The spool folder
      QDir folder(m_settings.value("folder").toString());
      // Note: previously we have made sure that existed: m_settings.value("folder")

      if (!filterAndSortFolder(folder))
         return;

      // Exit from this function if nothing must be done
      if (folder.count() == 0)
      {
         qDebug() << TDebug::indentation << "Going to end: " << Q_FUNC_INFO <<  " AHEAD";
         return;
      }

      // Stores the result of some operations
      int op_result = 0;

      // Boolean variables to know what could be done with the file
      bool fileHasBeenOpened;
      bool fileHasBeenDeleted;

      QFileInfoList files = folder.entryInfoList();

      // Try to open the files one by one and delete them
      foreach(QFileInfo file, files)
      {
         // Initialize variables
         fileHasBeenOpened = false;
         fileHasBeenDeleted = false;

         if (file.suffix().prepend(".") == m_special_extension)
         {
            op_result = openPathWrittenInside(file.absoluteFilePath());

            if (op_result == 0)
               fileHasBeenOpened = true;
            else
               fileHasBeenOpened = false;
         }
         else
         {
            op_result = openPath(file, folder.absolutePath());
         }

         // See the result. The opening can fail, for example, if the user didn't specify a
         // valid application to open a file with that extension
         if (op_result != 0)
         {
            syst.showWarning(tr("The file \"%1\" could not be correctly processed. Sometimes this error happens because the system "
                                "cannot find the program specified in the configuration of NxSpooler to open files "
                                "with that extension. The file is going to be deleted when you close this dialog window.")
                                .arg(QDir::toNativeSeparators(file.absoluteFilePath())));
            fileHasBeenOpened = false;
         }
         else
            fileHasBeenOpened = true;

         // Try to delete the file
         fileHasBeenDeleted = folder.remove(file.fileName());
         if (fileHasBeenDeleted == false)
         {
            // If the file couldn't be deleted, there's an external problem and
            // NxSpooler stops trying to open and delete all the files in its folder.
            QString message = tr("2805096 - The file \"%1\" could not be deleted.")
                              .arg(QDir::toNativeSeparators(file.absoluteFilePath()));
            throw runtime_error(message.toStdString());
         }

         // Add the file to the list of opened and deleted files by NxSpooler.
         // Note: if there were problems deleting the file, NxSpooler would have stopped to avoid more problems.
         m_listFiles->addItem(fileHasBeenOpened?file.fileName():file.fileName()+tr(" (errors when opening)"));
      }
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}


//! Show information about the application, authors and license.
/*!
*/
void TNxSpooler::openAboutNxSpooler()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;

       Ui::aboutDialog uiAbout;
       QDialog about(this);
       uiAbout.setupUi(&about);
       uiAbout.m_app_name_and_version->setText(qApp->applicationName() + " " + qApp->applicationVersion());

       about.exec(); // The returned value is not important here
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}


//! Show the options dialog.
/*!
*/
void TNxSpooler::openOptions()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block.
   try
   {
      QDEBUG_METHOD_NAME;

      TOptions options(&m_settings, this);

      // When the restore button is pushed, the the program is configured
      // with its default values.
      bool isConnected = connect(&options, SIGNAL(pushedRestore()),
                    this, SLOT(restoreSettings()));
      if (!isConnected)
      {
            // If the connection could not be restored, throw an exception
            QString message = tr("2208095 - Internal error when connecting.");
            throw runtime_error(message.toStdString());
      }

      // The options dialog is updated with the actual options
      isConnected = connect(this, SIGNAL(settingsRestored()),
          &options, SLOT(updateOptionsRows()));
      if (!isConnected)
      {
            // If the connection could not be restored, throw an exception
            QString message = tr("2208096 - Internal error when connecting.");
            throw runtime_error(message.toStdString());
      }

      do
      {
          options.exec();  // The returned value is not important here
          prepareSharedFolder();
          prepareTimer();
      }
      while(!syst.existsProgram(m_settings.value("apps").toString()));
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}


//! Show the help window.
/*!
*/
void TNxSpooler::openHelp()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;

       Ui::helpDialog uiHelp;
       QDialog help(this);
       uiHelp.setupUi(&help);

       help.exec(); // The returned value is not important here
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}


//! Filter by extension and sort by modification time.
/*!
  Case sensitive will not be applied to extensions both to Linux and Windows.
  Older files go first.
   \param folder Folder manager object
   \return true if all went well
*/
bool TNxSpooler::filterAndSortFolder(QDir &folder) const
{
   QDEBUG_METHOD_NAME;

   QStringList filters;
   QStringList exts = m_settings.value("exts").toStringList();
   int quant_exts = exts.count();

   // This is to avoid the case where the user goes to the options window
   // of NxSpooler and deletes all the extensions listed and then
   // NxSpooler would try to open everything
   if (quant_exts == 0)
   {
      return false;
   }

   for(int i = 0; i < quant_exts; i++)
   {
      filters << QString("*.%1").arg(exts.value(i));
   }

   // An special extension will let us to open a path contained inside
   // a text file ended with the m_special_extension
   filters << "*" + m_special_extension;

   // We specify to open only files. This is to avoid cases where for example
   // the user creates a folder named "my .pdf"
   folder.setFilter(QDir::Files);

   folder.setNameFilters(filters);

   folder.setSorting(QDir::Time|QDir::Reversed);

   return true;
}


//! Initialize the settings of NxSpooler.
/*!
   Assign default values to the options without an assigned value.
   Normally when the options have no value it's because it's the first execution of the
   program in a computer and so there were no options saved in the Windows registry or
   the configuration files in the "home" folder in Linux.
*/
void TNxSpooler::initializeSettings()
{
   QDEBUG_METHOD_NAME;

   if (m_settings.value("seconds").isNull())
   {
      m_settings.setValue("seconds", m_default_interval);
   }

   if (m_settings.value("exts").isNull())
   {
      m_settings.setValue("exts", m_default_formats);
   }

   // Note: an option that refers to an application path can be an empty string. The
   // default applications are only added if the option is a null value
   if (m_settings.value("apps").isNull())
   {
      const int quant_elements = m_settings.value("exts").toStringList().count();
      QStringList apps;

      for(int i = 0; i < quant_elements; i++)
      {
         apps.append(getDefaultProgram());
      }
      m_settings.setValue("apps", apps);
   }

   if (m_settings.value("folder").isNull() || m_settings.value("folder").toString().isEmpty())
   {
      m_settings.setValue("folder", m_default_folder);
   }

   if (m_settings.value("resource").isNull() || m_settings.value("resource").toString().isEmpty())
   {
      m_settings.setValue("resource", m_default_resource);
   }
}


//! Show the window of NxSpooler.
/*!
*/
void TNxSpooler::show()
{
   // As this is a slot that can be called by Qt code (in response to a mouse
   // click, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;

       // Avoid that NxSpooler finishes when the user closes dialogs with the main window hidden
       qApp->setQuitOnLastWindowClosed(true);

       // We don't use show() because sometimes the window is shown minimized
       showNormal();

       // Avoid that NxSpooler can be restored without focus
       // Note: in Windows normally it's not allowed that a program
       // can "steal" the focus when another is using it
       qApp->setActiveWindow(this);
       qApp->activeWindow()->raise();
       m_sys_tray_icon.contextMenu()->insertAction(m_action_quit, m_action_hide);
       m_sys_tray_icon.contextMenu()->removeAction(m_action_show);
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}


//! Depending on the case, hide or show the window of NxSpooler.
/*!
  \param reason Event code generated by the system tray icon (the button pushed)
*/
void TNxSpooler::showOrHide(QSystemTrayIcon::ActivationReason reason)
{
   // As this is a slot that can be called by Qt code (in response to a mouse 
   // click in the NxSpooler icon, for example), we don't allow exceptions
   // to go out from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;
       // If the user clicks on the system tray icon, hide or show the program
       if (reason == QSystemTrayIcon::Trigger)
       {
          if (isHidden())
          {
             show();
          }
          else
          {
             hide();
          }
       }
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}


//! Hide the window of NxSpooler.
/*!
*/
void TNxSpooler::hide()
{
   // As this is a slot that can be called by Qt code (in response to pushing the 
   // "hide" button in the main window, for example), we don't allow exceptions to 
   // go out from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;

       QDialog::hide();

       // Allow that when the last window of NxSpooler is closed, the program finishes
       qApp->setQuitOnLastWindowClosed(false);

       if(QSystemTrayIcon::isSystemTrayAvailable())
       {
          m_sys_tray_icon.contextMenu()->insertAction(m_action_quit, m_action_show);
          m_sys_tray_icon.contextMenu()->removeAction(m_action_hide);
       }
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}

//! Activate an icon in the system tray, if possible. Else, show the main window in a normal way.
/*!
*/
void TNxSpooler::prepareTrayIconOrShowProgram()
{
   QDEBUG_METHOD_NAME;

   if (QSystemTrayIcon::isSystemTrayAvailable())
   {
      m_sys_tray_icon.setIcon(windowIcon());
      bool isConnected = connect(&m_sys_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                                           this, SLOT(showOrHide(QSystemTrayIcon::ActivationReason)));

      if (!isConnected)
      {
         // If the connection could not be restored, throw an exception
         QString message = tr("2805099 - The icon of the notification area could not be activated.");
         throw runtime_error(message.toStdString());
      }

      // Prepare the contextual menu of the system tray icon
      QMenu *menu = new QMenu(this);
      menu->addAction(m_action_show);
      menu->addSeparator();
      menu->addAction(m_action_options);
      menu->addAction(m_action_help);
      menu->addAction(m_action_about);
      menu->addSeparator();
      menu->addAction(m_action_quit);
      m_sys_tray_icon.setContextMenu(menu);

      m_sys_tray_icon.show();
      qApp->setQuitOnLastWindowClosed(false);
      hide();
   }
   else
   {
      // If there is no system tray available in the desktop being used, show NxSpooler
      // and do not show the button used to hide NxSpooler (otherwise the program could not be seen
      // even being executed).
      showNormal();
      m_hide->hide();
   }
}


//! Check if the shared folder exists and if not, try to create it.
/*!
*/
void TNxSpooler::prepareSharedFolder() const
{
   QDEBUG_METHOD_NAME;

   // Create the manager of the path to check
   QDir folder(m_settings.value("folder").toString());

   // Create the folder if it doesn't exist
   if (!folder.exists())
   {
      QString question = tr("The folder \"%1\" does not exist. Do you want to create it?")
                         .arg(m_settings.value("folder").toString());
      bool isAccepted = syst.confirm(question);

      if (!isAccepted)
      {
         qDebug() << TDebug::indentation << "Going to end: " << Q_FUNC_INFO <<  " AHEAD";
         return;
      }

      if (folder.mkdir(folder.path()))
      {
         QString warning = tr("The folder \"%1\" was successfully created. Now it must be shared with the name \"%2\".")
                         .arg(m_settings.value("folder").toString(), m_settings.value("resource").toString());
         syst.showWarning(warning);
      }
      else
      {
         // If he folder could not be created, thrown an exception
         QString message = tr("2805093 - The folder \"%1\" could not be created.").arg(m_settings.value("folder").toString());
         throw runtime_error(message.toStdString());
      }
   }
}


//! Activate a timer that will execute the detectFilesAndOpen() slot every X seconds.
/*!
*/
void TNxSpooler::prepareTimer()
{
   QDEBUG_METHOD_NAME;

   // Avoid that repeated callings to this method causes to try to open something
   // several times at the same moment.
   // Note: we continue even if this disconnection fails.
   disconnect(&m_timer, SIGNAL(timeout()),
                         this, SLOT(detectFilesAndOpen()));


   bool isConnected = connect(&m_timer, SIGNAL(timeout()),
                                       this, SLOT(detectFilesAndOpen()));
   if (isConnected == false)
   {
      // If the connection could not be restored, throw an exception
      QString message = tr("2805094 - The timer could not be activated.");
      throw runtime_error(message.toStdString());
   }

   m_timer.start(m_settings.value("seconds").toInt() * 1000);
}


//! Try to open a path (it can be a file, a folder, a symlink,...).
/*!
  \param path The QFileInfo of the path to open (it can be a file, a folder, a symlink,...).
  \param source The place where the path was found.
  \return Returns 0 if there was no error found.
*/
int TNxSpooler::openPath(QFileInfo &path, const QString &source)
{
   QDEBUG_METHOD_NAME;

   // Object to store the arguments to some callings
   QStringList arguments;

   // Name of the application that can be used to open the file
   QString app;

   // Stores the result of opening the path. This will be the value to return
   int op_result = 0;

   // If it's something that seems to exist (there are many cases) and it's not a folder
   if (path.exists() && !path.isDir())
   {
      // Get the index of the file extension
      int i = m_settings.value("exts").toStringList().indexOf(path.suffix());

      if (i == -1)
      {
         // This case can happen if the path to open was inside a container file
         syst.showError(tr("2208097 - NxSpooler is not configured to launch an application to open files "
                              "like \"%1\", which was found inside \"%2\".\n\n"
                              "The administrator of this computer should see if this is due to a mistake "
                              "of the program that created the file, an incorrect configuration of "
                              "NxSpooler, etc.")
                              .arg(QDir::toNativeSeparators(path.fileName()))
                              .arg(QDir::toNativeSeparators(source)));

         // We return a value distinct from 0. Let's say "1" because we found 1 error
         return 1;
      }

      app = m_settings.value("apps").toStringList().value(i);
      // There's no problem if app.isEmpty()

#ifdef Q_WS_WIN
      arguments << "/C" << "start" << "/wait" << app;
#endif
   }

   // Note: this way it worked with paths like "smb://server/resource" in Linux
   arguments << QDir::toNativeSeparators(path.filePath());

   // If it's something that seems to exist (there are many cases) and it's not a folder
   if (path.exists() && !path.isDir())
   {
      // For avoiding the problem of having a file still being formed
      // and trying to open it, we'll wait until it has a stable size

      path.setCaching(false); // To try to read the current information about the file
      qint64 file_size_in_instant_1, file_size_in_instant_2;

      do
      {
         // Get the size
         file_size_in_instant_1 = path.size();

         syst.wait(750);

         // Refresh the information that we have about the file
         path.refresh();

         file_size_in_instant_2 = path.size();
      } while (file_size_in_instant_1 != file_size_in_instant_2);

      // Try to open the file
#ifdef Q_WS_WIN
      op_result = syst.execute("cmd", arguments);
#else
      // If the user is using Linux and he has not specified the name of the program to use,
      // execute the default program
      if (app.isEmpty())
      {
         op_result = syst.execute(getDefaultProgramInLinux(), arguments);
      }
      else
      {
         op_result = syst.execute(app, arguments);
      }
#endif
   }
   else
   {
      // it's a folder, or something that doesn't seem to exist (there are many cases)

#ifdef Q_WS_WIN
      // Windows explorer has anti-standard behaviours: for example returning 1 if it could
      // open a file and also returning the same value if it couldn't.
      op_result = (syst.execute("explorer", arguments) != 1);
#else
      op_result = syst.execute("xdg-open", arguments);
#endif
   }

   return op_result;
}


//! Open the file (or folder) mentioned inside a file.
/*!
  \param containerFile The path of the container file.
  \return Returns 0 if there was no error found.
*/
int TNxSpooler::openPathWrittenInside(const QString &containerFile)
{
   QDEBUG_METHOD_NAME;

   QStringList arguments;
   QProcess process(this);

   // Read the path inside the file
   QFile container(containerFile);
   container.open(QIODevice::ReadOnly);
   QString path = container.readLine().trimmed();
   container.close();

   // Try to adapt the path to the running system
#ifdef Q_WS_WIN
   path.replace(QRegExp("^smb://"), "\\\\");
   path.replace("/", QDir::separator());
#else
   path.replace(QRegExp("^\\\\\\\\"), "smb://");
   path.replace("\\", QDir::separator());
#endif

   // Note: we'll check later the existence of what "path" refers to

   // Try to activate the NxSpooler window (set the focus to its window) so that the
   // new opened window has the focus. Note: the operating system has to allow that.
   activateWindow();

   qDebug() << TDebug::indentation << "The path that must be opened is: " << path;

   QFileInfo aux(path);

   return openPath(aux, containerFile);
}


//! Determine which program is going to be used to open files.
/*!
   In Windows, allow the system to select the default viewer application.
   In Linux, make a search but giving priority to certain applications.
  \return Name or route of the program to use in a default way.
*/
QString TNxSpooler::getDefaultProgram() const
{
   QDEBUG_METHOD_NAME;

#ifdef Q_WS_WIN
   return "";
#else
   return getDefaultProgramInLinux();
#endif
}


//! Determine in Linux which program is going to be used to view the files.
/*!
   As a last resurce, xdg-open is used, but it has the inconvenience that it doesn't wait
   the user to close the open file and so if there are more pending files, all of them
   would be opened at the same time.
   \return Name of the executable program found in Linux.
*/
QString TNxSpooler::getDefaultProgramInLinux() const
{
   QDEBUG_METHOD_NAME;

   QString command;
   QStringList commands;
   commands<<"run-mailcap"<<"sensible-browser"<<"okular"<<"kpdf"<<"evince"<<"xpdf"<<"epdfview"<<"acroread"<<"xdg-open";

   // Step through the list until one is found available
   foreach(command, commands)
   {
      if (syst.existsProgram(command))
      {
         break;
      }
   }

   // If there is no valid program found, throw an exception
   if (!syst.existsProgram(command))
   {
      QString message = tr("2805095 - A valid program to open the files could not be found.");
      throw runtime_error(message.toStdString());
   }

   return command;
}


//! Restore the default settings.
/*!
  At the end, a signal is emitted to find the options dialog (if it's open)
  so that it can adapt its visual presentation to the new configuration.
*/
void TNxSpooler::restoreSettings()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block
   try
   {
       QDEBUG_METHOD_NAME;

       m_settings.remove("exts");
       m_settings.setValue("exts", m_default_formats);

       QStringList apps;
       int quant_elements = m_default_formats.count();

       for(int i = 0; i < quant_elements; i++)
       {
          apps.append(getDefaultProgram());
       }
       m_settings.remove("apps");
       m_settings.setValue("apps", apps);

       m_settings.setValue("resource", m_default_resource);
       m_settings.setValue("folder", m_default_folder);
       m_settings.setValue("seconds", m_default_interval);
       emit settingsRestored();
   }
   catch(std::exception &excep)
   {
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}

