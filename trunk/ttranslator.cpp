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

//! Constructor
/*!
     \param application Application that is going to work with the TTranslator
*/
TTranslator::TTranslator(QApplication &a)
{
   qDebug() << "___" << metaObject()->className() << ":: TTranslator";

   // The language and country of this locale as a string of the form "language_country", where language is a
   // lowercase, two-letter ISO 639 language code, and country is an uppercase, two-letter ISO 3166 country code.
   QString locale = QLocale::system().name();
   QString current_language = locale.section('_', 0, 0);

   // Tries to load a file that contains translations for the source texts used in the program. The program will
   // continue even if the file is not found
   if (!load(a.applicationName().toLower() + "_" + locale ))
   {
      if (current_language != "en")
      {
          // In this case we don't use tr(), since there is no translation and we don't want to add work to the human translators
          cerr << "Warning: the file of the " << a.applicationName() << " translation for your language has not been found." << endl;
      }
   }
   a.installTranslator(this);

   // Now let's deal with the QTranslator for the case of already translated, standard items
   // like "Restore defaults" buttons, "Cancel" buttons, etc.
   QString translations_path;
   translations_path = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
   // Note: the program will continue even if the file is not found
   if (!m_translatorStandardItems.load("qt_" + locale, translations_path))
   {
      if (current_language != "en")
      {
         cerr << tr("Warning: it has not been found the file of the Qt translation for the language of your system.") << endl;
         // If we used 'cerr << TSystem::tr("Warning") << ": " << tr("it has[...]")' people who use right-to-left languages would complain :-)
      }
   }
   a.installTranslator(&m_translatorStandardItems);

   qDebug() << "END" << metaObject()->className() << ":: TTranslator";
}

//! Destructor
/*!
*/
TTranslator::~TTranslator()
{
   qDebug() << "___" << metaObject()->className() << ":: ~TTranslator()";

   qDebug() << "END" << metaObject()->className() << ":: ~TTranslator()";
}


