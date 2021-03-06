/*****************************************************************************
*  This file is part of NxSpooler.
*
*  Copyright (C) 2009-2011 by Creación y Diseño Ibense S.L., Arón Galdón Ginés, Toni Asensi Esteve.
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

#include <QtCore/QSettings>
#include <QtGui/QDialog>
#include <QtGui/QFileDialog>

#include "lib_src/tsystem.h"
#include "ui_optionsDialog.h"

class TOptions : public QDialog, private Ui::optionsDialog
{
   Q_OBJECT

public:
   TOptions(QSettings *, QWidget * = 0);
   ~TOptions();

   bool checkAndSaveTheOptions();

public slots:
   void updateOptionsRows();

private slots:
   void on_m_new_ext_clicked();
   void on_m_delete_ext_clicked();
   void on_m_find_app_clicked();
   void on_m_find_path_clicked();

private:
   QSettings *m_settings; //!< Object that manages (loads and saves automatically) the configuration of NxSpooler

signals:
   void pushedRestore(); //!< This signal will be restored when the restore button is pushed
};

#endif
