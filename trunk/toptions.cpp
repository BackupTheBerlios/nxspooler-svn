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
   \class TOptions
   \brief Manages the options dialog.
*/

#include "toptions.h"

//! Constructor.
/*!
*/
TOptions::TOptions(QSettings *settings, QWidget *qwidget_parent)
      : QDialog(qwidget_parent)
      , m_settings(settings)
{
   qDebug() << "___" << metaObject()->className() << ":: TOptions";

   setupUi(this);

   // Button to set the default values in the options fields
   bool isConnected = connect((QObject *)m_buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()),
                                                                        this, SIGNAL(pushedRestore()));
   if (!isConnected)
   {
         // If the connection could not be established, throw an exception
         QString message = tr("2208092 - Internal error when connecting");
         throw runtime_error(message.toStdString());
   }

   // Establish that when the options dialog is accepted, the options have to be changed
   isConnected = connect(this, SIGNAL(accepted()),
           this, SLOT(updateSettings()));
   if (!isConnected)
   {
         // If the connection could not be established, throw an exception
         QString message = tr("2208093 - Internal error when connecting");
         throw runtime_error(message.toStdString());
   }

   // The option fields are updated with the actual NxSpooler options
   updateOptionsRows();

   // The last column (the one with the application path) will occupy all the horizontal space that is left
   m_exts_apps->horizontalHeader()->setStretchLastSection(true);

   // Place the cursor of the extensions table in its first element
   m_exts_apps->selectRow(0);

   qDebug() << "END" << metaObject()->className() << ":: TOptions";
}


//! Destructor.
/*!
*/
TOptions::~TOptions()
{
   qDebug() << "___" << metaObject()->className() << ":: ~TOptions";

   qDebug() << "END" << metaObject()->className() << ":: ~TOptions";
}


//! Copy the information of the NxSpooler configuration variables to the options fields.
/*!
*/
void TOptions::updateOptionsRows()
{
   // As this is a slot that can be called by Qt code (in response to pushing the "Restore
   // defaults" button in the options window, for example), we don't allow exceptions to go
   // out from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: updateOptionsRows";

       m_seconds->setValue(m_settings->value("seconds").toInt());

       QStringList exts = m_settings->value("exts").toStringList();
       QStringList apps = m_settings->value("apps").toStringList();

       if (m_exts_apps->rowCount() > 0)
       {
          bool success = m_exts_apps->model()->removeRows(0, m_exts_apps->rowCount());
          if (not success)
          {
             QString message = tr("2108091 - A problem was found and a row could not be deleted");
             throw runtime_error(message.toStdString());
          }
      }

       int quant_elements = exts.count();
       for(int i = 0; i < quant_elements; i++)
       {
          m_exts_apps->insertRow(m_exts_apps->rowCount());
          m_exts_apps->setItem(i, 0, new QTableWidgetItem(exts.value(i)));
          qDebug() << apps.value(i);
          m_exts_apps->setItem(i, 1, new QTableWidgetItem(apps.value(i)));
       }

       m_folder->setText(m_settings->value("folder").toString());
       m_shared->setText(m_settings->value("resource").toString());

       qDebug() << "END" << metaObject()->className() << ":: updateOptionsRows";
   }
   catch(std::exception &excep)
   {
      TSystem::exitBecauseException(excep);
   }
   catch(...)
   {
      TSystem::exitBecauseException();
   }
}

//! Copy the information of the options fields to the NxSpooler configuration variables.
/*!
*/
void TOptions::updateSettings()
{
   // As this is a slot that can be called by Qt code (in response to pushing the "OK"
   // button in the options window, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: updateSettings";

       QStringList exts;
       QStringList apps;

       // Go through the rows of the table control to see the extensions and its related applications
       const int quant_rows = m_exts_apps->rowCount();
       for (int i = 0; i < quant_rows; i++)
       {
          // Avoid adding an empty or null extension
          if(not m_exts_apps->item(i, 0)->text().isNull() && not m_exts_apps->item(i, 0)->text().isEmpty())
          {
             exts.append(m_exts_apps->item(i, 0)->text());

             // Avoid adding a null application path (it can be empty).
             // For example, this way we avoid the problem of having a user entering
             // letters in the extension cell of an empty row and then pushing "Ok"
             if (m_exts_apps->item(i, 1) == NULL || m_exts_apps->item(i, 1)->text().isNull())
                apps.append("");
             else
                apps.append(m_exts_apps->item(i, 1)->text());
          }
       }

       m_settings->remove("exts");
       m_settings->remove("apps");
       m_settings->setValue("exts", exts);
       m_settings->setValue("apps", apps);

       m_settings->setValue("resource", m_shared->text());
       m_settings->setValue("folder", m_folder->text());
       m_settings->setValue("seconds", m_seconds->value());

       if(!syst.existsProgram(m_settings->value("app").toString()))
       {
          syst.showWarning(tr("The selected program can't be accessed. Please select another or set the default value."));
       }

       qDebug() << "END" << metaObject()->className() << ":: updateSettings";
   }
   catch(std::exception &excep)
   {
      TSystem::exitBecauseException(excep);
   }
   catch(...)
   {
      TSystem::exitBecauseException();
   }
}


//! Insert a new line at the end of the table.
/*
*/
void TOptions::on_m_new_ext_clicked()
{
   // As this is a slot that can be called by Qt code (in response to pushing the "New"
   // button in the options window, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block
   try
   {
       m_exts_apps->insertRow(m_exts_apps->rowCount());
       m_exts_apps->setCurrentCell(m_exts_apps->rowCount() - 1, 0);
   }
   catch(std::exception &excep)
   {
      TSystem::exitBecauseException(excep);
   }
   catch(...)
   {
      TSystem::exitBecauseException();
   }
}


//! Delete the selected row.
/*
*/
void TOptions::on_m_delete_ext_clicked()
{
   // As this is a slot that can be called by Qt code (in response to pushing the "Delete"
   // button in the options window, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block
   try
   {
       QString question = tr("Are you sure that you want to delete the selected row?");
       bool isAccepted = syst.confirm(question);

       if (isAccepted)
       {
          m_exts_apps->removeRow(m_exts_apps->currentRow());
          return;
       }
   }
   catch(std::exception &excep)
   {
      TSystem::exitBecauseException(excep);
   }
   catch(...)
   {
      TSystem::exitBecauseException();
   }
}


//! Open a new dialog so that the user can select the executable file of an application.
/*
*/
void TOptions::on_m_find_app_clicked()
{
   // As this is a slot that can be called by Qt code (in response to pushing the "Browse"
   // button in the options window, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: on_m_find_app_clicked";

       QFileDialog file_dialog(this, tr("Select the viewer program"), "/");
       file_dialog.setFileMode(QFileDialog::ExistingFile);

       // If the user accepts the dialog, get the path of the selected file
       if (file_dialog.exec() == QDialog::Accepted)
       {
          QTableWidgetItem *new_app_item = new
              QTableWidgetItem(QDir::toNativeSeparators((file_dialog.selectedFiles().first())));
          m_exts_apps->setItem(m_exts_apps->currentRow(), 1, new_app_item);
       }

       qDebug() << "END" << metaObject()->className() << ":: on_m_find_app_clicked";
   }
   catch(std::exception &excep)
   {
      TSystem::exitBecauseException(excep);
   }
   catch(...)
   {
      TSystem::exitBecauseException();
   }
}


//! Open a new dialog so that the user can select the shared local folder.
/*
*/
void TOptions::on_m_find_path_clicked()
{
   // As this is a slot that can be called by Qt code (in response to pushing the
   // m_find_path button in the options window, for example), we don't allow
   // exceptions to go out from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: on_m_find_path_clicked";

       QFileDialog folder_dialog(this, tr("Select the shared local folder"));

       // Configure it to be a folder selector
       folder_dialog.setFileMode(QFileDialog::Directory);
       folder_dialog.setOption(QFileDialog::ShowDirsOnly);

       // Start in the specified actual folder
       QDir folder(m_settings->value("folder").toString());
       QDir upper_folder(folder);
       upper_folder.cdUp(); // We continue even if it fails
       folder_dialog.setDirectory(upper_folder);
       folder_dialog.selectFile(folder.dirName());

       // If the user accepts the dialog, get the selected path
       if (folder_dialog.exec() == QDialog::Accepted)
       {
          m_folder->setText(QDir::toNativeSeparators(folder_dialog.selectedFiles().first()));
       }

       qDebug() << "END" << metaObject()->className() << ":: on_m_find_path_clicked";
   }
   catch(std::exception &excep)
   {
      TSystem::exitBecauseException(excep);
   }
   catch(...)
   {
      TSystem::exitBecauseException();
   }
}

