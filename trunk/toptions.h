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

#ifndef T_OPTIONS_H
#define T_OPTIONS_H

#include <QtGui/QDialog>
#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include "ui_optionsDialog.h"
#include "tnxspooler.h"

class TOptions : public QDialog, private Ui::optionsDialog
{
   Q_OBJECT

public:
   TOptions(QSettings *settings, QWidget *qwidget_parent = 0);
   ~TOptions();

public slots:
   void updateOptionsRows();
   void updateSettings();

private slots:
   void on_m_new_ext_clicked();
   void on_m_delete_ext_clicked();
   void on_m_find_app_clicked();
   void on_m_find_path_clicked();

private:
   QSettings *m_settings; //!< Objeto gestor de la configuración del programa (carga y guarda automáticamente)

signals:
   void pushedRestore(); //!< Se activará esta señal cuando el botón de restaurar sea pulsado
};

#endif
