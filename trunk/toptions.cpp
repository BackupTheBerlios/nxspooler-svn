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
   \brief Gestiona el diálogo de opciones.
*/

#include "toptions.h"

//! Constructor.
/*!
*/
TOptions::TOptions(QSettings *settings, QWidget *parent)
      : QDialog(parent)
      , m_settings(settings)
{
   qDebug() << "___" << metaObject()->className() << ":: TOptions";

   setupUi(this);

   // Botón para dejar los valores predeterminados en los campos de opciones
   bool isConnected = connect((QObject *)m_buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()),
                                                                        this, SIGNAL(pushedRestore()));
   if (!isConnected)
   {
         // Si no se ha podido establecer la conexión, lanzar una excepción
         QString message = tr("2208092 - Internal error when connecting");
         throw runtime_error(message.toStdString());
   }

   // Al aceptar el diálogo, asignar a los ajustes el contenido de los campos de opciones
   isConnected = connect(this, SIGNAL(accepted()),
           this, SLOT(updateSettings()));
   if (!isConnected)
   {
         // Si no se ha podido establecer la conexión, lanzar una excepción
         QString message = tr("2208093 - Internal error when connecting");
         throw runtime_error(message.toStdString());
   }

   // Poner en los campos de opciones los ajustes actuales del programa
   updateOptionsRows();

   // La última columna (la de la ruta del programa) ocupará todo el espacio horizontal sobrante
   m_exts_apps->horizontalHeader()->setStretchLastSection(true);

   // Dejar el cursor de la tabla de extensiones en su primer elemento
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


//! Pone la información de los ajustes del programa en los campos de opciones.
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


//! Pone la información de los campos de opciones en los ajustes del programa.
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

       // Recorrer las filas del control tabla para tomar las extensiones y sus aplicaciones
       const int quant_rows = m_exts_apps->rowCount();
       for (int i = 0; i < quant_rows; i++)
       {
          // Evitar agregar una extensión nula o vacía
          if(not m_exts_apps->item(i, 0)->text().isNull() && not m_exts_apps->item(i, 0)->text().isEmpty())
          {
             exts.append(m_exts_apps->item(i, 0)->text());

             // Evitar tener una ruta nula (sí se puede tener vacía)
             // For example, this way we avoid the problem when the user on an empty
             // row entered letters in the extension cell of and then pushed "Ok"
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


//! Inserta al final de la tabla una nueva línea
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


//! Borra la línea seleccionada
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


//! Abre un diálogo para que el usuario seleccione el fichero del ejecutable del programa
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

       // Si el usuario acepta el diálogo, tomar la ruta del fichero seleccionado
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


//! Abre un diálogo para que el usuario seleccione la ruta local compartida
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

       // Configurar para que sea un selector de directorios
       folder_dialog.setFileMode(QFileDialog::Directory);
       folder_dialog.setOption(QFileDialog::ShowDirsOnly);

       // Comenzar con la carpeta actual especificada
       QDir folder(m_settings->value("folder").toString());
       QDir upper_folder(folder);
       upper_folder.cdUp(); // We continue even if it fails
       folder_dialog.setDirectory(upper_folder);
       folder_dialog.selectFile(folder.dirName());

       // Si el usuario acepta el diálogo, tomar la ruta seleccionada
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
