/*****************************************************************************
*  This file is part of NxSpooler.
*
*  Copyright (C) 2009 by Creación y Diseño Ibense S.L.
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
   \class TOpciones
   \brief Gestiona del diálogo de opciones.
*/

#include "topciones.h"

//! Constructor.
/*!
*/
TOpciones::TOpciones(QSettings *ajustes, QWidget *padre)
      : QDialog(padre)
      , m_ajustes(ajustes)
{
   qDebug() << "___" << metaObject()->className() << ":: TOpciones";

   setupUi(this);

   // Botón para la exploración de la aplicación visualizadora
   connect(m_find_app, SIGNAL(clicked()),
    (TOpciones *)this, SLOT(seleccionarPrograma()));

   // Botón para la exploración de la ruta
   connect(m_find_path, SIGNAL(clicked()),
                  this, SLOT(seleccionarRuta()));

   // Botón para dejar los valores predeterminados en los campos de opciones
   connect((QObject *)m_buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()),
                                                                        this, SIGNAL(restaurarPulsado()));

   // Al aceptar el diálogo, asignar a los ajustes el contenido de los campos de opciones
   connect(this, SIGNAL(accepted()),
           this, SLOT(actualizarAjustes()));

   // Poner en los campos de opciones los ajustes actuales del programa
   actualizarCamposOpciones();

   qDebug() << "FIN" << metaObject()->className() << ":: TOpciones";
}


//! Destructor.
/*!
*/
TOpciones::~TOpciones()
{
   qDebug() << "___" << metaObject()->className() << ":: ~TOpciones";

   qDebug() << "FIN" << metaObject()->className() << ":: ~TOpciones";
}


//! Pone la información de los ajustes del programa en los campos de opciones.
/*!
*/
void TOpciones::actualizarCamposOpciones()
{
   qDebug() << "___" << metaObject()->className() << ":: actualizarCamposOpciones";

   m_seconds->setValue(m_ajustes->value("segundos").toInt());
   m_app->setText(m_ajustes->value("app").toString());
   m_path->setText(m_ajustes->value("ruta").toString());
   m_shared->setText(m_ajustes->value("recurso").toString());

   qDebug() << "FIN" << metaObject()->className() << ":: actualizarCamposOpciones";
}


//! Pone la información de los campos de opciones en los ajustes del programa.
/*!
*/
void TOpciones::actualizarAjustes()
{
   qDebug() << "___" << metaObject()->className() << ":: actualizarAjustes";

   m_ajustes->setValue("app", m_app->text());
   m_ajustes->setValue("recurso", m_shared->text());
   m_ajustes->setValue("ruta", m_path->text());
   m_ajustes->setValue("segundos", m_seconds->value());

   if(!sist.existePrograma(m_ajustes->value("app").toString()))
   {
      sist.mostrarAviso(t("No se puede acceder a la aplicación indicada. Seleccione otra o deje el valor predeterminado."));
   }

   qDebug() << "FIN" << metaObject()->className() << ":: actualizarAjustes";
}


//! Abre un diálogo para que el usuario seleccione el fichero del ejecutable del programa
/*
*/
void TOpciones::seleccionarPrograma()
{
   qDebug() << "___" << metaObject()->className() << ":: seleccionarPrograma";

   QFileDialog dialog_fichero(this, t("Seleccione el visor de PDF"), "/");
   dialog_fichero.setFileMode(QFileDialog::ExistingFile);

   // Si el usuario acepta el diálogo, tomar la ruta del fichero seleccionado
   if (dialog_fichero.exec() == 1)
   {
      m_app->setText(QDir::toNativeSeparators((dialog_fichero.selectedFiles().first())));
   }

   qDebug() << "FIN" << metaObject()->className() << ":: seleccionarPrograma";
}


//! Abre un diálogo para que el usuario seleccione la ruta local compartida
/*
*/
void TOpciones::seleccionarRuta()
{
   qDebug() << "___" << metaObject()->className() << ":: seleccionarRuta";

   QFileDialog dialog_ruta(this, t("Seleccione la ruta local compartida"));

   // Configurar para que sea un selector de directorios
   dialog_ruta.setFileMode(QFileDialog::Directory);
   dialog_ruta.setOption(QFileDialog::ShowDirsOnly);

   // Comenzar con la ruta actual seleccionada
   QDir ruta(m_ajustes->value("ruta").toString());
   QDir ruta_sin_dir(ruta);
   ruta_sin_dir.cdUp();
   dialog_ruta.setDirectory(ruta_sin_dir);
   dialog_ruta.selectFile(ruta.dirName());

   // Si el usuario acepta el diálogo, tomar la ruta seleccionada
   if (dialog_ruta.exec() == 1)
   {
      m_path->setText(QDir::toNativeSeparators(dialog_ruta.selectedFiles().first()));
   }

   qDebug() << "FIN" << metaObject()->className() << ":: seleccionarRuta";
}
