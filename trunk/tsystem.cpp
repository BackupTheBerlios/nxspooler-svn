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
   \brief Offers services that can be used from any object of the program.
*/

#include "tsystem.h"

//! Constructor.
/*!
  It will be a global object.
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


//! Shows a question and asks the user to accept or cancel.
/*!
   \param message Question for the user
   \return True if the user clicks in the accept button
*/
bool TSystem::confirm(const QString &message) const
{
   qDebug() << "___" << metaObject()->className() << ":: confirm()";

   QMessageBox msgBox(QMessageBox::Question, tr("Confirm"), message, QMessageBox::Ok|QMessageBox::Cancel);

   int bt = msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: confirm()";
   return (bt == QMessageBox::Ok);
}


//! Receives a program name (or a path) as an argument and returns if it exists or not.
/*!
   If it receives a path, it checks if it exists.
   If it receives a program name, it checks it's accessible directly or using the "path" string.
   As NxSpooler allows an empty string as a program name,
   with a passed empty name it will return "true".
   \param name Path or a program name
   \return The value "true" if the program (or path) exists or if the program is accessible using the "path" string
*/
bool TSystem::existsProgram(const QString &name) const
{
   qDebug() << "___" << metaObject()->className() << ":: existsProgram";

   if (name.isEmpty())
   {
      qDebug() << "END" << metaObject()->className() << ":: existsProgram AHEAD";
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

   qDebug() << "END" << metaObject()->className() << ":: existsProgram";
   return (system_result == 0);
}


//! Shows a warning message to the user (he can only continue).
/*!
   \param message Warning message that is shown to the user
   \param windowTitle The title that appears in the window
*/
void TSystem::showWarning(const QString &message, const QString &windowTitle) const
{
   qDebug() << "___" << metaObject()->className() << ":: showWarning";

   QTextStream cerr(stderr);
   cerr << tr("Warning: ") << message << endl;

   QMessageBox msgBox;
   msgBox.setWindowTitle(windowTitle == "" ? tr("Warning") : windowTitle);
   msgBox.setText(message);
   msgBox.setIcon(QMessageBox::Warning);
   msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: showWarning";
}


//! Shows an error message to the user.
/*!
   \param message Error message that is shown to the user
   \param windowTitle The title that appears in the window
*/
void TSystem::showError(const QString &message, const QString &windowTitle) const
{
   qDebug() << "___" << metaObject()->className() << ":: showError()";

   QTextStream cerr(stderr);
   cerr << tr("Error: ") << message << endl;

   QMessageBox msgBox;
   msgBox.setWindowTitle(windowTitle == "" ? tr("Error"):windowTitle);
   msgBox.setText(message);
   msgBox.setIcon(QMessageBox::Critical);
   msgBox.exec();

   qDebug() << "END" << metaObject()->className() << ":: showError()";
}


//! Execute a program.
/*!
   \param program The program to execute
   \param arguments The arguments to be passed to the program
    Starts a program in a new process, waits for it to finish, and then returns the exit code of the process. Any data the new process writes to the console is forwarded to the calling process.

    The environment and working directory are inherited by the calling process.

    On Windows, arguments that contain spaces are wrapped in quotes.
*/
int
TSystem::execute(const QString &program) const
{
    qDebug() << "___" << metaObject()->className() << ":: execute()";

    QProcess process;

    // As seen in http://jira.codehaus.org/browse/IZPACK-20 and http://labs.trolltech.com/forums/topic/156, the method
    // execute() from QProcess is not used.

    // Note: more spaces and quotes were not needed
    qDebug() << "___" << "Going to execute:" << program;

    process.setReadChannelMode(QProcess::ForwardedChannels);
    process.start(program);
    process.waitForFinished(-1);

    int exitCode = process.exitCode();
    if (process.error() == QProcess::FailedToStart)
    {
       exitCode = -1;
    }

    qDebug() << "END" << metaObject()->className() << ":: execute()";

    return exitCode;
}


//! Execute a program, with arguments.
/*!
   \param program The program to execute
   \param arguments The arguments to be passed to the program
    Starts a program with arguments in a new process, waits for it to finish, and then returns the exit code of the process. Any data the new process writes to the console is forwarded to the calling process.

    The environment and working directory are inherited by the calling process.

    On Windows, arguments that contain spaces are wrapped in quotes.
*/
int
TSystem::execute(const QString &program, const QStringList &arguments) const
{
    qDebug() << "___" << metaObject()->className() << ":: execute()";

    QProcess process;

    // As seen in http://jira.codehaus.org/browse/IZPACK-20 and http://labs.trolltech.com/forums/topic/156, the method
    // execute() from QProcess is not used.

    // Note: more spaces and quotes were not needed
    qDebug() << "___" << "Going to execute:" << program << "with arguments:" << arguments;

    process.setReadChannelMode(QProcess::ForwardedChannels);
    process.start(program, arguments);
    process.waitForFinished(-1);

    int exitCode = process.exitCode();
    if (process.error() == QProcess::FailedToStart)
    {
       exitCode = -1;
    }

    qDebug() << "END" << metaObject()->className() << ":: execute()";

    return exitCode;
}


//! Quit the program due to an exception.
/*!
*/
void
TSystem::exitBecauseException(std::exception &excep)
{
    qDebug() << "___" << metaObject()->className() << ":: exitBecauseException(std::exception &excep)";

    syst.showError(QString(excep.what()) + ".", tr("Error") + " - " + qApp->applicationName());

    qDebug() << "END" << metaObject()->className() << ":: exitBecauseException(std::exception &excep)";

    exit(EXIT_FAILURE);
}


//! Quit the program due to a unidentified exception.
/*!
*/
void
TSystem::exitBecauseException()
{
    qDebug() << "___" << metaObject()->className() << ":: exitBecauseException()";

    syst.showError(tr("An unidentified problem has happened and %1 must be closed.").arg(qApp->applicationName())
                        , tr("Error") + " - " + qApp->applicationName());

    qDebug() << "END" << metaObject()->className() << ":: exitBecauseException()";

    exit(EXIT_FAILURE);
}

