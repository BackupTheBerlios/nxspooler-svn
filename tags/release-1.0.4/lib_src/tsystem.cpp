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

#include "tsystem.h"

/*!
   \class TSystem
   \brief Offers services that can be used from any object of the program,
          like methods, definitions, a class to manage errors, etc.
*/

//! Show a message box when it's sure that there's a QApplication running. It's an auxiliary method to avoid duplicating code.
/*!
   \param message Warning message that is shown to the user.
   \param windowTitle The title that has the message box.
   \param icon The icon to show.
*/
void TSystem::showMsgBoxWhenHavingQApp(const QString &message, const QString &windowTitle, QMessageBox::Icon icon) const
{
   QDEBUG_METHOD_NAME

   QMessageBox msgBox;
   msgBox.setText(message);
   msgBox.setWindowTitle(windowTitle);
   msgBox.setIcon(icon);
   msgBox.exec();
}


//! Constructor.
/*!
  It will be a global object.
*/
TSystem::TSystem()
{
   QDEBUG_METHOD_NAME
}


//! Destructor.
/*!
*/
TSystem::~TSystem()
{
   QDEBUG_METHOD_NAME
}


//! Shows a question and asks the user to accept or cancel.
/*!
   \param message Question for the user.
   \return True if the user clicks in the accept button.
*/
bool TSystem::confirm(const QString &message) const
{
   QDEBUG_METHOD_NAME

   QMessageBox msgBox(QMessageBox::Question, tr("Confirm"), message, QMessageBox::Ok|QMessageBox::Cancel);

   int bt = msgBox.exec();

   return (bt == QMessageBox::Ok);
}


//! Receives a program name (or a path) as an argument and returns if it exists or not.
/*!
   If it receives a path, it checks if it exists.
   If it receives a program name, it checks it's accessible directly or using the "path" string.
   As NxSpooler allows an empty string as a program name,
   with a passed empty name it will return "true".
   \param name Path or a program name.
   \return The value "true" if the program (or path) exists or if the program is accessible
           using the "path" string.
*/
bool TSystem::existsProgram(const QString &name) const
{
   QDEBUG_METHOD_NAME

   if (name.isEmpty())
   {
      qDebug() << TDebugStartEnd::indentation << "Going to end: " << Q_FUNC_INFO <<  " AHEAD";
      return true;
   }

   // Depending on the platform, use an order or another to do the detection
   QString command;
#ifdef Q_WS_WIN
   command = QString("dir \"%1\" >nul").arg(name);
#else
   command = QString("which \"%1\" >/dev/null").arg(name);
#endif

   int system_result;
   // Depending on the platform, use the detection order with one codificaton or another
#ifdef Q_WS_WIN
   system_result = system(command.toLatin1());
#else
   system_result = system(command.toUtf8());
#endif

   return (system_result == 0);
}


//! Show a message box. It has to work even if there is no QApplication already running.
/*!
   \param message Warning message that is shown to the user.
   \param windowTitle The title that has the message box.
   \param icon The icon to show.
*/
void TSystem::showMsgBox(const QString &message, const QString &windowTitle, QMessageBox::Icon icon) const
{
   QDEBUG_METHOD_NAME

   if (qApp == NULL)
   {
        // To show a Qt dialog we need a QApplication
        int argc = 0;
        char **argv = NULL;
        QApplication temporary(argc, argv);
        showMsgBoxWhenHavingQApp(message, windowTitle, icon);
        // Let's notice that the life of the temporary QApplication finishes at the end of this block
   }
   else
   {
        showMsgBoxWhenHavingQApp(message, windowTitle, icon);
   }
}


//! Shows a warning message to the user (he can only continue).
/*!
   \param message Warning message that is shown to the user.
   \param windowTitle The title that has the message box.
*/
void TSystem::showWarning(const QString &message, const QString &windowTitle) const
{
   QDEBUG_METHOD_NAME

   cerr << message << endl;
   showMsgBox(message, windowTitle == "" ? tr("Warning") : windowTitle, QMessageBox::Warning);
}


//! Shows an error message to the user.
/*!
   \param message Error message that is shown to the user.
   \param windowTitle The title that that has the message box.
*/
void TSystem::showError(const QString &message, const QString &windowTitle) const
{
   QDEBUG_METHOD_NAME

   cerr << message << endl;
   showMsgBox(message, windowTitle == "" ? tr("Error") : windowTitle, QMessageBox::Critical);
}


//! Execute a program.
/*!
   \param program The program to execute.
   \return The exit code of the executed program.
    Starts a program in a new process, waits for it to finish, and then returns the exit code
    of the process. Any data the new process writes to the console is forwarded to the calling process.

    The environment and working directory are inherited by the calling process.

    On Windows, arguments that contain spaces are wrapped in quotes.
*/
int TSystem::execute(const QString &program) const
{
    QDEBUG_METHOD_NAME

    QProcess process;

    // As seen in http://jira.codehaus.org/browse/IZPACK-20 and http://labs.trolltech.com/forums/topic/156, the method
    // execute() from QProcess is not used.

    qDebug() << TDebugStartEnd::indentation << " Going to execute: " << program;

    process.setReadChannelMode(QProcess::ForwardedChannels);
    process.start(program);
    process.waitForFinished(-1);

    int exitCode = process.exitCode();
    if (process.error() == QProcess::FailedToStart)
    {
       exitCode = -1;
    }

    return exitCode;
}


//! Execute a program, with arguments.
/*!
   \param program The program to execute.
   \param arguments The arguments to be passed to the program.
   \return The exit code of the executed program.
    Starts a program with arguments in a new process, waits for it to finish, and then returns
    the exit code of the process. Any data the new process writes to the console is forwarded to
    the calling process.

    The environment and working directory are inherited by the calling process.

    On Windows, arguments that contain spaces are wrapped in quotes.
*/
int TSystem::execute(const QString &program, const QStringList &arguments) const
{
    QDEBUG_METHOD_NAME

    QProcess process;

    // As seen in http://jira.codehaus.org/browse/IZPACK-20 and http://labs.trolltech.com/forums/topic/156, the method
    // execute() from QProcess is not used.

    qDebug() << TDebugStartEnd::indentation << "Going to execute: " << program << " with arguments: " << arguments;

    process.setReadChannelMode(QProcess::ForwardedChannels);
    process.start(program, arguments);
    process.waitForFinished(-1);

    int exitCode = process.exitCode();
    if (process.error() == QProcess::FailedToStart)
    {
       exitCode = -1;
    }

    return exitCode;
}


//! Wait during several miliseconds.
/*!
   \param miliseconds The quantity of miliseconds to wait.
*/
void TSystem::wait(int miliseconds) const
{
   QDEBUG_METHOD_NAME

   // Note: this code was extracted from the "qtlocalpeer.cpp" file
    // that came with the source code of NxSpooler.
    #ifdef Q_WS_WIN
       Sleep(DWORD(miliseconds));
    #else
       struct timespec ts = { miliseconds / 1000, (miliseconds % 1000) * 1000 * 1000 };
       nanosleep(&ts, NULL);
    #endif
}


//! Tell the TSystem the name of the program.
/*!
   \param application The name of the program.

    The TSystem needs to know the name of the program, for example when finding
    an error and having no QCoreApplication running.

    The name of the method follows the Qt Style.
*/
QString TSystem::setApplicationName(const QString &application)
{
    QDEBUG_METHOD_NAME

    return m_applicationName = application;
}


//! Get the name of the program.
/*!
*/
QString TSystem::applicationName() const
{
    QDEBUG_METHOD_NAME

    return m_applicationName;
}


//! Quit the program due to an exception.
/*!
*/
void
TSystem::exitBecauseException(std::exception &excep)
{
    QDEBUG_METHOD_NAME

    syst.showError(excep.what(), tr("Error") + " - " + m_applicationName);

    exit(EXIT_FAILURE);
}


//! Quit the program due to a unidentified exception.
/*!
*/
void
TSystem::exitBecauseException()
{
    QDEBUG_METHOD_NAME

    syst.showError(tr("Error 0309102: An unidentified problem has happened and %1 must be closed.").arg(m_applicationName)
                        , tr("Error") + " - " + m_applicationName);

    exit(EXIT_FAILURE);
}

