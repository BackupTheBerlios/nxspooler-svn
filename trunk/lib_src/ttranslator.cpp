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
   \class TTranslator
   \brief A QTranslator with some special features to save some work to the developer.
*/

#include "ttranslator.h"


//! Return the folder where the executable file of the program is. It's an auxiliary method to avoid duplicating code.
/*!
   \return The folder where the executable file of the program is.
*/
QDir TTranslator::folderWhereTheExecutableIs() const
{
   QDEBUG_METHOD_NAME

   // If we use 'return "."', in the case of executing the program using a
   // symlink, then "." would be the folder of the symlink and we don't want that.
   return m_a.applicationDirPath();
   // Because of hard links nature, if we execute the program using a hard link, then what is returned is
   // the folder of the hard link. Anyway, the worst that can happen is that the program works in English.

   /* In Qt help was written: "Warning: On Linux, applicationFilePath will try to get the path from the /proc file
   system. If that fails, it assumes that argv[0] contains the absolute file name of the executable. The function
   also assumes that the current directory has not been changed by the application. */
}


//! Receives a QTranslator and tries to make it load a file that contains translations. It searches in different folders if it doesn't find the translations files.
/*!
   \param translator The translator that has to load the translations.
   \param baseName The base name of the translation files to find.
   \param programToTranslate The program that is going to be translated.
   \param currentLanguage The language of this locale.
   \param folderWithPriority1 The path of the first folder where to search.
   \param folderWithPriority2 The path of the second folder where to search.
   \param folderWithPriority3 The path of the third folder where to search.
   \param folderWithPriority4 The path of the fourth folder where to search.
   \param folderWithPriority5 The path of the fifth folder where to search.
*/
void TTranslator::searchAndLoadTranslation(QTranslator &translator,
                                           const QString &baseName,
                                           const QString &programToTranslate,
                                           const QString &currentLanguage,
                                           const QString &folderWithPriority1,
                                           const QString &folderWithPriority2,
                                           const QString &folderWithPriority3,
                                           const QString &folderWithPriority4,
                                           const QString &folderWithPriority5)
{
   QDEBUG_METHOD_NAME

   if (!translator.load(baseName, folderWithPriority1))
      if (!translator.load(baseName, folderWithPriority2))
         if (!translator.load(baseName, folderWithPriority3))
            if (!translator.load(baseName, folderWithPriority4))
               if (folderWithPriority5 != "")
                  translator.load(baseName, folderWithPriority5);

   if (translator.isEmpty())
      if (currentLanguage != "en")
      {
          // Let's notice that in the case of missing the translation of the main program, not in
          // the case of missing the Qt translation, this message won't be translated, of course.
          cerr << tr("Warning: it has not been found a valid file of the %1 translation for the language of your system.")
                  .arg(programToTranslate) << endl;
          // If we used 'tr("Warning") << ": " << tr("it has[...]")' people who use right-to-left languages would complain :-).
      }

   // Note: the program will continue even if the translation file is not found.
}


//! Constructor
/*!
     \param program Program that is going to work with the TTranslator.
*/
TTranslator::TTranslator(QCoreApplication &program) : m_a(program)
{
   QDEBUG_METHOD_NAME

   // The language and country of this locale as a string of the form "language_country", where language is a
   // lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter ISO 3166 country code.
   QString locale = QLocale::system().name();
   QString currentLanguage = locale.section('_', 0, 0);

   QString programName = program.applicationName();
   QString programNameInSmallLetters = programName.toLower();

   // Base name of the translation file of the program.
   QString programTranslationFile = programNameInSmallLetters + "_" + locale;

   // Temporary variable, to avoid executing more code than is necessary.
   QDir operationFolder = folderWhereTheExecutableIs();
   
   // Path of the folder where the executable program is.
   QString programFolder = operationFolder.path();

   // Path of the folder that is one level up from programFolder (if it exists,
   // if not, it will be empty).
   QString upProgramFolder;
   if (operationFolder.cdUp())
      upProgramFolder = operationFolder.path();

#ifdef Q_WS_WIN
   #ifdef QT_NO_DEBUG
         searchAndLoadTranslation(*this, programTranslationFile, programName, currentLanguage, programFolder, ".", upProgramFolder, "..");
   #else // For example, to allow to execute "debug\program.exe" from the development folder (where the qm file is).
         searchAndLoadTranslation(*this, programTranslationFile, programName, currentLanguage, ".", programFolder, upProgramFolder, "..");
   #endif

#else
   // RELEASE_INSTALLATION_FOLDER is defined in the .pro file. This "ifdef-define-endif" is here only to avoid the Qt Creator visual alert.
   #ifndef RELEASE_INSTALLATION_FOLDER
      #define RELEASE_INSTALLATION_FOLDER
   #endif

   // The path where the translation files of the relase are.
   QString releaseTranslationsFolder = QString(RELEASE_INSTALLATION_FOLDER"/share/")
                                       + programNameInSmallLetters + "/translations";
   #ifdef QT_NO_DEBUG
      searchAndLoadTranslation(*this, programTranslationFile, programName, currentLanguage, releaseTranslationsFolder, programFolder, ".", upProgramFolder, "..");
   #else
      searchAndLoadTranslation(*this, programTranslationFile, programName, currentLanguage, ".", programFolder, releaseTranslationsFolder, upProgramFolder, "..");
   #endif
#endif

   program.installTranslator(this);

   // Now let's deal with the QTranslator for the case of already translated, standard
   // items like "Restore defaults" buttons, "Cancel" buttons, etc.

   // Note: in development we have Qt installed, and we normally don't modify the Qt
   // translations files so this case is simpler.
   QString qtTranslationsPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
   searchAndLoadTranslation(m_translatorStandardItems, "qt_" + locale, "Qt", currentLanguage, qtTranslationsPath, programFolder, ".", upProgramFolder, "..");

   program.installTranslator(&m_translatorStandardItems);
}


//! Destructor
/*!
*/
TTranslator::~TTranslator()
{
   QDEBUG_METHOD_NAME
}
