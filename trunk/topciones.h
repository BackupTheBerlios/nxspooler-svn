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

#ifndef TOPCIONES_H
#define TOPCIONES_H

#include <QtGui/QDialog>
#include <QtCore/QSettings>
#include <QtGui/QFileDialog>
#include "ui_optionsDialog.h"
#include "tsistema.h"

class TOpciones : public QDialog, private Ui::optionsDialog
{
   Q_OBJECT

public:
   TOpciones(QSettings *ajustes, QWidget *padre = 0);
   ~TOpciones();

public slots:
   void actualizarAjustes();
   void actualizarCamposOpciones();

private slots:
   void seleccionarPrograma();
   void seleccionarRuta();

private:
   QSettings *m_ajustes; //!< Objeto gestor de la configuración del programa (carga y guarda automáticamente)

signals:
   void restaurarPulsado(); //!< Se activará esta señal cuando el botón de restaurar sea pulsado
};

#endif
