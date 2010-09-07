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

#include "toptions.h"

/*!
   \class TOptions
   \brief Manages the options dialog.
*/

//! Constructor.
/*!
*/
TOptions::TOptions(QSettings *settings, QWidget *qwidget_parent)
      : QDialog(qwidget_parent)
      , m_settings(settings)
{
   QDEBUG_METHOD_NAME;

   setupUi(this);

   // Button to set the default values in the options fields
   bool isConnected = connect((QObject *)m_buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()),
                                                                        this, SIGNAL(pushedRestore()));
   if (!isConnected)
   {
         // If the connection could not be established, throw an exception
         QString message = tr("Error 2208092: Internal problem when connecting.");
         throw runtime_error(message.toStdString());
   }

   // The option fields are updated with the actual NxSpooler options
   updateOptionsRows();

   // To avoid text duplication, we reuse the tooltip
   m_exts_apps->horizontalHeaderItem(1)->setWhatsThis(m_exts_apps->horizontalHeaderItem(1)->toolTip());

   // Adjust the with of this column
   m_exts_apps->setColumnWidth(1, 235);

   // The last column (the one with the application path) has to occupy all the horizontal space that is left
   m_exts_apps->horizontalHeader()->setStretchLastSection(true);

   // Place the cursor of the extensions table in its first element
   m_exts_apps->selectRow(0);
}


//! Destructor.
/*!
*/
TOptions::~TOptions()
{
    QDEBUG_METHOD_NAME;
}


//! Checks the options that the user has chosen in the form and if it finds no errors, it saves the options.
/*!
   \return A boolean that tells if the options could be saved.
*/
bool TOptions::checkAndSaveTheOptions()
{
      QDEBUG_METHOD_NAME;

      QStringList exts;
      QStringList apps;
      // List of "booleans" that indicate if the extension must be opened only if found inside a container file, a ".nxspooler-open" file
      QVariantList onlyInsideContainer;

      enum TableColumns { extensionColumn = 0, containedColumn = 1, programColumn = 2 };

      // Go through the rows of the table control to see the extensions and its related applications
      const int quant_rows = m_exts_apps->rowCount();
      for (int i = 0; i < quant_rows; i++)
      {
          // Note: in this case if we use ".isEmpty()" we don't need to use ".isNull()". A previous "trimmed()" didn't afect this rule
          bool extensionIsEmpty = m_exts_apps->item(i, extensionColumn) == NULL // This check is made "just in case"
                                 || m_exts_apps->item(i, extensionColumn)->text().trimmed().isEmpty();

          if (extensionIsEmpty)
          {
             // If the extension is empty but a program has been specified, then we consider that there is a mistake
             if (m_exts_apps->item(i, programColumn) != NULL
                && !m_exts_apps->item(i, programColumn)->text().trimmed().isEmpty())
                {
                   syst.showError(tr("Error 0209101: A program has been associated with an empty extension."));
                   return false;
                }
          }
          else // If the extension is not empty, we add it to the list
          {
             const QString nameOfTheExtension = m_exts_apps->item(i, extensionColumn)->text();
             exts.append(nameOfTheExtension);
             onlyInsideContainer.append(m_exts_apps->item(i, containedColumn)->checkState() == Qt::Checked);

             // Avoid adding a null application path (in this program it can be empty). For
             // example, this way we avoid the problem of having a user entering letters
             // in the extension cell of an empty row and then pushing "Ok"
             if (m_exts_apps->item(i, programColumn) == NULL || m_exts_apps->item(i, programColumn)->text().isNull())
                apps.append("");
             else // if the application path is not null
             {
                apps.append(m_exts_apps->item(i, programColumn)->text());

                if (!syst.existsProgram(m_exts_apps->item(i, programColumn)->text().trimmed()))
                {
                   // Warn the user that the specified program wasn't found (maybe it's because it's still not installed).
                   // Note: if cell of the program is empty, this warning won't be shown
                   syst.showWarning(tr("A program \"%1\" could not be accessed.").arg(m_exts_apps->item(i, programColumn)->text()));
                }
             }

             // Search for rows that correspond to the same extension.
             // Note: this search is case-insensitive
             QList <QTableWidgetItem *> extensionAppearances = m_exts_apps->findItems(nameOfTheExtension, Qt::MatchFixedString);
             // If more than one instance is found
             if (extensionAppearances.size() > 1)
             {
                syst.showError(tr("Error 0309101: The \"%1\" extension appears in several rows.").arg(nameOfTheExtension));
                return false;
             }
          }
      }

      m_settings->remove("exts");
      m_settings->remove("onlyInsideContainer");
      m_settings->remove("apps");

      m_settings->setValue("exts", exts);
      m_settings->setValue("onlyInsideContainer", onlyInsideContainer);
      m_settings->setValue("apps", apps);

      m_settings->setValue("resource", m_shared->text());
      m_settings->setValue("folder", m_folder->text());
      m_settings->setValue("seconds", m_seconds->value());

      // The options have been saved
      return true;
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
       QDEBUG_METHOD_NAME;

       m_seconds->setValue(m_settings->value("seconds").toInt());

       QStringList exts = m_settings->value("exts").toStringList();
       QStringList apps = m_settings->value("apps").toStringList();
       QVariantList onlyInsideContainer = m_settings->value("onlyInsideContainer").toList();

       // Delete rows if they are found
       if (m_exts_apps->rowCount() > 0)
       {
          bool success = m_exts_apps->model()->removeRows(0, m_exts_apps->rowCount());
          if (not success)
          {
             QString message = tr("Error 2108091: A problem was found and a row could not be deleted.");
             throw runtime_error(message.toStdString());
          }
       }

       // Insert rows
       int quant_elements = exts.count();
       for(int i = 0; i < quant_elements; i++)
       {
          m_exts_apps->insertRow(m_exts_apps->rowCount());

          m_exts_apps->setItem(i, 0, new QTableWidgetItem(exts.value(i)));
          qDebug() << TDebug::indentation << exts.value(i);

          qDebug() << TDebug::indentation << onlyInsideContainer.value(i).toBool();
          QTableWidgetItem *checkboxOnlyInsideContainer = new QTableWidgetItem();
          checkboxOnlyInsideContainer->setCheckState(onlyInsideContainer.value(i).toBool()?Qt::Checked:Qt::Unchecked);
          // To avoid that the user can, for example, write text at the right to the checkbox
          checkboxOnlyInsideContainer->setFlags(Qt::ItemFlags(checkboxOnlyInsideContainer->flags() - Qt::ItemIsEditable));
          // Finally, we "put" the checkbox inside
          m_exts_apps->setItem(i, 1, checkboxOnlyInsideContainer);

          qDebug() << TDebug::indentation << apps.value(i);
          m_exts_apps->setItem(i, 2, new QTableWidgetItem(apps.value(i)));
       }

       m_folder->setText(m_settings->value("folder").toString());
       m_shared->setText(m_settings->value("resource").toString());
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


//! Insert a new line at the end of the table.
/*
*/
void TOptions::on_m_new_ext_clicked()
{
   // As this is a slot that can be called by Qt code (in response to pushing the "New"
   // button in the options window, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;

       m_exts_apps->insertRow(m_exts_apps->rowCount());
       m_exts_apps->setCurrentCell(m_exts_apps->rowCount() - 1, 0);

       // By default, set the value "false", so for the extension there's no restriction
       // of being opened only inside a container file
       QTableWidgetItem *checkboxOnlyInsideContainer = new QTableWidgetItem();
       checkboxOnlyInsideContainer->setCheckState(Qt::Unchecked);
       // To avoid that the user can, for example, write text at the right to the checkbox
       checkboxOnlyInsideContainer->setFlags(Qt::ItemFlags(checkboxOnlyInsideContainer->flags() - Qt::ItemIsEditable));
       // Finally, we "put" the checkbox inside
       m_exts_apps->setItem(m_exts_apps->rowCount() - 1, 1, checkboxOnlyInsideContainer);
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
       QDEBUG_METHOD_NAME;

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
      syst.exitBecauseException(excep);
   }
   catch(...)
   {
      syst.exitBecauseException();
   }
}


//! Open a new dialog so that the user can select the executable file of an application.
/*
*/
void TOptions::on_m_find_app_clicked()
{
   // As this is a slot that can be called by Qt code (in response to pushing the "Browse"
   // button in the options window, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;

       QFileDialog file_dialog(this, tr("Select the viewer program"), "/");
       file_dialog.setFileMode(QFileDialog::ExistingFile);

       // If the user accepts the dialog, get the path of the selected file
       if (file_dialog.exec() == QDialog::Accepted)
       {
          QTableWidgetItem *new_app_item = new
          QTableWidgetItem(QDir::toNativeSeparators((file_dialog.selectedFiles().first())));
          m_exts_apps->setItem(m_exts_apps->currentRow(), 2, new_app_item);
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


//! Open a new dialog so that the user can select the shared local folder.
/*
*/
void TOptions::on_m_find_path_clicked()
{
   // As this is a slot that can be called by Qt code (in response to pushing the
   // m_find_path button in the options window, for example), we don't allow
   // exceptions to go out from here. So we use a "try" block.
   try
   {
       QDEBUG_METHOD_NAME;

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

