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

#ifndef TNXSPOOLER_H
#define TNXSPOOLER_H

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QMenu>
#include <QtGui/QDialog>

#include <QTranslator>

#include "ttrayicon.h"
#include "toptions.h"
#include "tsystem.h"

// Si estamos en Windows, usaremos la función system() de la librería stdlib.h
#ifdef Q_WS_WIN
#include <stdlib.h>
#endif

#include "ui_aboutDialog.h"
#include "ui_nxspooler.h"

class TNxSpooler : public QDialog, private Ui::NxSpooler
{
   Q_OBJECT

public:
   TNxSpooler(QWidget *padre = 0);
   ~TNxSpooler();

private slots:
   void abrir();
   void abrirAcercaDe();
   void abrirOpciones();
   void mostrar();
   void mostrarOcultar(QSystemTrayIcon::ActivationReason motivo);
   void ocultar();
   void prepararRutaLocal();
   void restaurarAjustes();

private:
   void filtrarOrdenarDir(QDir &ruta);
   void inicializarAjustes();
   void prepararIconoBandejaOMostrarPrograma();
   void prepararTemporizador();
   QString programaPredeterminado() const;
   QString programaPredeterminadoLinux() const;

   QSettings m_ajustes; //!< Objeto gestor de la configuración del programa (carga y guarda automáticamente)
   TIconoBandeja m_icono_bandeja; //!< Gestor del icono de bandeja
   int m_intervalo_predeterminado; //!< Número de segundos del intervalo por defecto
   QStringList m_formatos_predeterminados; //!< Lista de extensiones a incluir en la detección
   QString m_recurso_predeterminado; //!< Cadena que se mantendrá constante con el nombre de recurso por defecto
   QString m_ruta_predeterminada; //!< Cadena que se mantendrá constante con la ruta por defecto
   QTimer m_temporizador; //!< Objeto que llamará a abrir() a intervalos de tiempo regulares

signals:
   void ajustesRestaurados();
};

#endif
