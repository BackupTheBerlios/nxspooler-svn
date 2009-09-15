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
   \class TNxSpooler
   \brief Abre y borra regularmente los archivos de ciertas extensiones que aparecen en una ruta.

   La clase TNxSpooler se encarga de comprobar cada cierto intervalo de tiempo si
   existen archivos en una ruta indicada para abrirlos y borrarlos.
   Dicha ruta se encuentra compartida por red para que la use una conexión FreeNX.
   De este modo, basta con que funcione la compartición de ficheros por Samba
   en la sesión FreeNX para que se abra automáticamente en nuestra sesión local
   cualquier fichero con la extensión deseada que se genere en la ruta.
*/

#include "tnxspooler.h"

//! Construye un objeto TNxSpooler asignándole un padre.
/*!
   Si no ocurre alguna excepción que lo impida, se inician los diferentes
   aspectos de un objeto TNxSpooler.
   \param parent Si no se especifica, el objeto TNxSpooler será una ventana independiente
*/
TNxSpooler::TNxSpooler(QWidget *parent)
   : QDialog(parent)
{
   qDebug() << "___" << metaObject()->className() << ":: TNxSpooler";

   // Adoptar a los objetos miembro que lo requieran
   m_settings.setParent(this);
   m_sys_tray_icon.setParent(this);
   m_timer.setParent(this);

   // Definir los valores predeterminados "constantes"
   m_default_interval = 3;
   m_default_formats.append("pdf");
   m_default_formats.append("ods");
   m_default_formats.append("sxc");
   m_default_resource = "nxspooler$";
   m_default_folder = QDir::toNativeSeparators(QDir::homePath().append(QDir::separator()).append(".nxspooler"));

   setupUi(this);
   prepareTrayIconOrShowProgram();

   try
   {
      initializeSettings();
      prepareSharedFolder();
   }
   catch (std::exception &excep)
   {
      // Si no existía la ruta y no se ha podido crear o si no existe el programa,
      // llevar al usuario al diálogo de opciones
      if (QString(excep.what()).startsWith("2805093") || QString(excep.what()).startsWith("0707091"))
      {
         syst.showWarning(QString(excep.what()) + ".");
         show();
         openOptions();
      }
      else
      {
         throw (excep);
      }
   }

   prepareTimer();

   qDebug() << "END" << metaObject()->className() << ":: TNxSpooler";
}


//! Destruye TNxSpooler.
/*!
  De momento no lo usamos para nada (podría no estar definido).
*/
TNxSpooler::~TNxSpooler()
{
   qDebug() << "___" << metaObject()->className() << ":: ~TNxSpooler";

   qDebug() << "END" << metaObject()->className() << ":: ~TNxSpooler";
}


//! Abre y borra los archivos con extensión deseada que se encuentra.
/*!
*/
void TNxSpooler::open()
{
   // As this is a slot that can be called by Qt code (in response to a call
   // made by m_timer, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: open";

       QDir folder(m_settings.value("folder").toString());
       // Note: previously we have made sure that existed m_settings.value("folder")

       if (!filterAndSortFolder(folder))
           return;

       // No continuar en este método si no existen ficheros a tratar
       if (folder.count() == 0)
       {
          qDebug() << "END" << metaObject()->className() << ":: open AHEAD";
          return;
       }

       QStringList arguments;
       bool hasBeenDeleted = false;
       int result = 0;
       QFileInfoList files = folder.entryInfoList();

       // Abrir uno a uno cada fichero y si ha funcionado bien, borrarlo
       foreach(QFileInfo file, files)
       {
          arguments.clear();

          // We are using suffix because completeSuffix fails with names like for example "first.second.ext"
          int i = m_settings.value("exts").toStringList().indexOf(file.suffix());
          // This shouldn't happen, but just in case...
          if (i == -1)
          {
             QString message = tr("2208097 - Extension not found");
             throw runtime_error(message.toStdString());
          }

          QString app = m_settings.value("apps").toStringList().value(i);
          // There's no problem if app.isEmpty()

    #ifdef Q_WS_WIN
          arguments << "/C" << "start" << "/wait" << app;
    #endif

          arguments << QDir::toNativeSeparators(file.absoluteFilePath());
          QProcess process(this);

    #ifdef Q_WS_WIN
          result = process.execute("cmd", arguments);
    #else
          // Si estamos en Linux y el usuario ha dejado en blanco el nombre de la aplicación de apertura,
          // usar la que se encuentre en este momento respetando ese ajuste vacío
          if (app.isEmpty())
          {
             result = process.execute(getDefaultProgramInLinux(), arguments);
          }
          else
          {
             result = process.execute(app, arguments);
          }
    #endif

          // La apertura puede fallar en el caso de que el fichero esté todavía a mitad de generarse
          // It also happens if the user didn't specify a valid application to open a file with that extension
          if (result != 0)
          {
             syst.showWarning(tr("The file \"%1\" could not be opened").arg(file.absoluteFilePath()));
          }

          // Si la extensión está marcada como borrable, eliminar el fichero si se ha podido abrir correctamente
          if(m_settings.value("exts_delete").toList().value(i).toBool() == true)
          {
             hasBeenDeleted = folder.remove(file.fileName());
             if (!hasBeenDeleted)
             {
               QString message = tr("2805096 - The file \"%1\" could not be deleted").arg(file.absoluteFilePath());
               throw runtime_error(message.toStdString());
             }
             // Si se pudo abrir y se pudo borrar el fichero, agregarlo al histórico
             m_listDeletedFiles->addItem(file.fileName());
          }
          else
          {
          }
       }

       qDebug() << "END" << metaObject()->className() << ":: open";
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


//! Muestra información sobre el programa, los autores y la licencia.
/*!
*/
void TNxSpooler::openAboutNxSpooler()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: openAboutNxSpooler";

       Ui::aboutDialog uiAbout;
       QDialog about(this);
       uiAbout.setupUi(&about);
       uiAbout.m_app_name_and_version->setText(qApp->applicationName() + " " + qApp->applicationVersion());

       about.exec(); // The returned value is not important here

       qDebug() << "END" << metaObject()->className() << ":: openAboutNxSpooler";
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


//! Muestra el diálogo de opciones.
/*!
*/
void TNxSpooler::openOptions()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block
   try
   {
      qDebug() << "___" << metaObject()->className() << ":: openOptions";

      TOptions options(&m_settings, this);

      // Cuando se pulsa el botón de restaurar, los ajustes del programa vuelven a sus valores predeterminados
      bool isConnected = connect(&options, SIGNAL(pushedRestore()),
                    this, SLOT(restoreSettings()));
      if (!isConnected)
      {
            // Si no se ha podido establecer la conexión, lanzar una excepción
            QString message = tr("2208095 - Internal error when connecting");
            throw runtime_error(message.toStdString());
      }

      // Tras ser restaurados los ajustes del programa, el cambio debe reflejarse en el diálogo
      isConnected = connect(this, SIGNAL(settingsRestored()),
          &options, SLOT(updateOptionsRows()));
      if (!isConnected)
      {
            // Si no se ha podido establecer la conexión, lanzar una excepción
            QString message = tr("2208096 - Internal error when connecting");
            throw runtime_error(message.toStdString());
      }

      do
      {
          options.exec();  // The returned value is not important here
          prepareSharedFolder();
          prepareTimer();
      }
      while(!syst.existsProgram(m_settings.value("apps").toString()));

      qDebug() << "END" << metaObject()->className() << ":: openOptions";
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


//! Muestra la ventana de ayuda.
/*!
*/
void TNxSpooler::openHelp()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: openHelp";

       Ui::helpDialog uiHelp;
       QDialog help(this);
       uiHelp.setupUi(&help);

       help.exec(); // The returned value is not important here

       qDebug() << "END" << metaObject()->className() << ":: openHelp";
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


//! Reduce el listado a los ficheros con las extensiones deseadas y ordena por hora de modificación.
/*!
   Con esta manera de ordenar nos aseguramos de que el documento que lleva más tiempo esperando es el primero
   en ser abierto.
   \param folder Gestor del directorio
   \return Verdadero si el filtrado ha sido realizado correctamente
*/
bool TNxSpooler::filterAndSortFolder(QDir &folder) const
{
   qDebug() << "___" << metaObject()->className() << ":: filterAndSortFolder";

   // Tratar sólo ciertos tipos de ficheros mediante un filtro.
   // Tanto en Windows como en Linux no diferenciará mayúsculas de minúsculas.
   QStringList filters;
   QStringList exts = m_settings.value("exts").toStringList();
   int quant_exts = exts.count();

   // This is to avoid the case where the user goes to the options window
   // of NxSpooler and deletes all the extensions listed and then
   // NxSpooler would try to open everything
   if (quant_exts == 0)
      return false;

   for(int i = 0; i < quant_exts; i++)
   {
      filters << QString("*.%1").arg(exts.value(i));
   }

   // Before (thinking only in print documents), we were specifying to open only
   // files to avoid cases where for example the user creates a folder named "my .pdf",
   // but having also directories we can upload files to the remote program using a file browser
   folder.setFilter(QDir::AllEntries);

   folder.setNameFilters(filters);

   folder.setSorting(QDir::Time|QDir::Reversed);

   qDebug() << "END" << metaObject()->className() << ":: filterAndSortFolder";

   return true;
}


//! Inicializa los ajustes de NxSpooler.
/*!
   Asigna su valor por defecto a los ajustes que no hayan obtenido su contenido almacenado.
   Normalmente, cuando los ajustes no tienen valor es por que estamos en la primera ejecución
   del programa en el ordenador y no había ajustes guardados en el registro de Windows o los
   ficheros de configuración en nuestro "home" de Linux.
*/
void TNxSpooler::initializeSettings()
{
   qDebug() << "___" << metaObject()->className() << ":: initializeSettings";

   if (m_settings.value("seconds").isNull())
   {
      m_settings.setValue("seconds", m_default_interval);
   }

   if (m_settings.value("exts").isNull())
   {
      m_settings.setValue("exts", m_default_formats);
   }

   // By default, new extensions must be deleted
   if (m_settings.value("exts_delete").isNull())
   {
      QVariantList lista;
      lista.append(true);
      lista.append(true);
      lista.append(true);
      m_settings.setValue("exts_delete", lista);
   }

   // Nota: se permite dejar una ruta de aplicación como una cadena vacía,
   // sólo ponemos las predeterminadas si el ajuste está nulo (todavía no es ni una lista de cadenas)
   if (m_settings.value("apps").isNull())
   {
      const int quant_elements = m_settings.value("exts").toStringList().count();
      QStringList apps;

      for(int i = 0; i < quant_elements; i++)
      {
         apps.append(getDefaultProgram());
      }
      m_settings.setValue("apps", apps);
   }

   if (m_settings.value("folder").isNull() || m_settings.value("folder").toString().isEmpty())
   {
      m_settings.setValue("folder", m_default_folder);
   }

   if (m_settings.value("resource").isNull() || m_settings.value("resource").toString().isEmpty())
   {
      m_settings.setValue("resource", m_default_resource);
   }

   qDebug() << "END" << metaObject()->className() << ":: initializeSettings";
}


//! Muestra la ventana de NxSpooler.
/*!
*/
void TNxSpooler::show()
{
   // As this is a slot that can be called by Qt code (in response to a mouse
   // click, for example), we don't allow exceptions to go out
   // from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: show";

       // Evitar que NxSpooler termine al cerrar diálogos con la ventana principal oculta
       qApp->setQuitOnLastWindowClosed(true);

       // No usamos show() porque a veces la ventana se muestra minimizada
       showNormal();

       // Evitar que en ocasiones NxSpooler se restaure sin foco
       //NOTE: De todos modos, en Windows no está permitido que un programa
       // robe el foco a otra que lo está usando
       qApp->setActiveWindow(this);
       qApp->activeWindow()->raise();
       m_sys_tray_icon.contextMenu()->insertAction(m_action_quit, m_action_hide);
       m_sys_tray_icon.contextMenu()->removeAction(m_action_show);

       qDebug() << "END" << metaObject()->className() << ":: show";
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


//! Oculta o muestra alternadamente la ventana de NxSpooler.
/*!
  \param reason Código de evento que genera el icono de bandeja (el botón pulsado)
*/
void TNxSpooler::showOrHide(QSystemTrayIcon::ActivationReason reason)
{
   // As this is a slot that can be called by Qt code (in response to a mouse 
   // click in the NxSpooler icon, for example), we don't allow exceptions
   // to go out from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: showOrHide";
       // Si el usuario hace click sobre el icono de la bandeja, mostrar u ocultar el programa
       if (reason == QSystemTrayIcon::Trigger)
       {
          if (isHidden())
          {
             show();
          }
          else
          {
             hide();
          }
       }

       qDebug() << "END" << metaObject()->className() << ":: showOrHide";
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


//! Oculta la ventana de NxSpooler.
/*!
*/
void TNxSpooler::hide()
{
   // As this is a slot that can be called by Qt code (in response to pushing the 
   // "hide" button in the main window, for example), we don't allow exceptions to 
   // go out from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: hide";

       QDialog::hide();

       // Permitir que cuando la última ventana de NxSpooler se cierre, éste termine
       qApp->setQuitOnLastWindowClosed(false);

       if(QSystemTrayIcon::isSystemTrayAvailable())
       {
          m_sys_tray_icon.contextMenu()->insertAction(m_action_quit, m_action_show);
          m_sys_tray_icon.contextMenu()->removeAction(m_action_hide);
       }

       qDebug() << "END" << metaObject()->className() << ":: hide";
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

//! Activa un icono en la bandeja de sistema si es posible. En caso contrario, muestra la ventana principal de forma normal.
/*!
*/
void TNxSpooler::prepareTrayIconOrShowProgram()
{
   qDebug() << "___" << metaObject()->className() << ":: prepareTrayIconOrShowProgram";

   if (QSystemTrayIcon::isSystemTrayAvailable())
   {
      m_sys_tray_icon.setIcon(windowIcon());
      bool isConnected = connect(&m_sys_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                                           this, SLOT(showOrHide(QSystemTrayIcon::ActivationReason)));

      if (!isConnected)
      {
         // Si no se ha podido establecer la conexión, lanzar una excepción
         QString message = tr("2805099 - The icon of the notification area could not be activated");
         throw runtime_error(message.toStdString());
      }

      // Preparar el menú contextual del icono de bandeja
      QMenu *menu = new QMenu(this);
      menu->addAction(m_action_show);
      menu->addSeparator();
      menu->addAction(m_action_options);
      menu->addAction(m_action_help);
      menu->addAction(m_action_about);
      menu->addSeparator();
      menu->addAction(m_action_quit);
      m_sys_tray_icon.setContextMenu(menu);

      m_sys_tray_icon.show();
      qApp->setQuitOnLastWindowClosed(false);
      hide();
   }
   else
   {
      // Si no hay bandeja del sistema disponible en el escritorio utilizado, mostrar NxSpooler
      // y esconder el botón de ocultar (luego no se podría volver a ver NxSpooler aun estando en marcha)
      showNormal();
      m_hide->hide();
   }

   qDebug() << "END" << metaObject()->className() << ":: prepareTrayIconOrShowProgram";
}


//! Comprobar si existe la ruta e intentar crearla en caso de que no.
/*!
*/
void TNxSpooler::prepareSharedFolder() const
{
   qDebug() << "___" << metaObject()->className() << ":: prepareSharedFolder";

   // Crear el gestor de la ruta a comprobar
   QDir folder(m_settings.value("folder").toString());

   // Crear el directorio si no existe
   if (!folder.exists())
   {
      QString question = tr("The folder \"%1\" does not exist. Do you want to create it?")
                         .arg(m_settings.value("folder").toString());
      bool isAccepted = syst.confirm(question);

      if (!isAccepted)
      {
         qDebug() << "END" << metaObject()->className() << ":: prepareSharedFolder() AHEAD";
         return;
      }

      if (folder.mkdir(folder.path()))
      {
         QString warning = tr("The folder \"%1\" was successfully created. Now it must be shared with the name \"%2\".")
                         .arg(m_settings.value("folder").toString(), m_settings.value("resource").toString());
         syst.showWarning(warning);
      }
      else
      {
         // Si no se ha podido crear el directorio inexistente, lanzar una excepción
         QString message = tr("2805093 - The folder \"%1\" could not be created").arg(m_settings.value("folder").toString());
         throw runtime_error(message.toStdString());
      }
   }

   qDebug() << "END" << metaObject()->className() << ":: prepareSharedFolder";
}


//! Activa un temporizador que se ejecutará el slot abrir según el intervalo especificado.
/*!
*/
void TNxSpooler::prepareTimer()
{
   qDebug() << "___" << metaObject()->className() << ":: prepareTimer";

   // Evitar que sucesivas llamadas a este método intenten abrir varias veces al mismo tiempo
   // Note: we continue even if this disconnection fails
   disconnect(&m_timer, SIGNAL(timeout()),
                         this, SLOT(open()));


   bool isConnected = connect(&m_timer, SIGNAL(timeout()),
                                       this, SLOT(open()));
   if (isConnected == false)
   {
      // Si no se ha podido establecer la conexión, lanzar una excepción
      QString message = tr("2805094 - The timer could not be activated");
      throw runtime_error(message.toStdString());
   }

   m_timer.start(m_settings.value("seconds").toInt() * 1000);

   qDebug() << "END" << metaObject()->className() << ":: prepareTimer";
}


//! Decide qué programa usar para abrir archivos.
/*!
   En Windows, dejamos en manos del sistema la selección de la aplicación visora por defecto.
   En Linux, se realiza una búsqueda priorizando ciertas aplicaciones.
  \return Nombre o ruta del programa a utilizar de forma predeterminada
*/
QString TNxSpooler::getDefaultProgram() const
{
   qDebug() << "___" << metaObject()->className() << ":: getDefaultProgram";

   qDebug() << "END" << metaObject()->className() << ":: getDefaultProgram";

#ifdef Q_WS_WIN
   return "";
#else
   return getDefaultProgramInLinux();
#endif
}


//! Decide qué programa usar en Linux para ver los archivos.
/*!
   Como último recurso se usa xdg-open, pero tiene el inconveniente de que no espera a que
   el usuario cierre el fichero abierto y por tanto se ejecuta toda la cola de ficheros al mismo tiempo.
   \return Nombre del ejecutable de la aplicación que se encuentre en Linux
*/
QString TNxSpooler::getDefaultProgramInLinux() const
{
   qDebug() << "___" << metaObject()->className() << ":: getDefaultProgramInLinux";

   QString command;
   QStringList commands;
   commands<<"run-mailcap"<<"sensible-browser"<<"okular"<<"kpdf"<<"evince"<<"xpdf"<<"epdfview"<<"acroread"<<"xdg-open";

   // Recorrer los elementos de la lista hasta encontrar uno accesible por ruta o por PATH
   foreach(command, commands)
   {
      if (syst.existsProgram(command))
      {
         break;
      }
   }

   // Si no se ha podido encontrar un programa adecuado, lanzar una excepción
   if (!syst.existsProgram(command))
   {
      QString message = tr("2805095 - A valid program to open the files could not be found");
      throw runtime_error(message.toStdString());
   }

   qDebug() << "END" << metaObject()->className() << ":: getDefaultProgramInLinux";
   return command;
}


//! Deja los ajustes del programa en sus valores predeterminados.
/*!
  Al final se emite una señal que capta el diálogo de opciones si está abierto
  para que adapte sus campos de opciones a los nuevos ajustes.
*/
void TNxSpooler::restoreSettings()
{
   // As this is a slot that can be called by Qt code (in response to a pushed button, for example), we
   // don't allow exceptions to go out from here. So we use a "try" block
   try
   {
       qDebug() << "___" << metaObject()->className() << ":: restoreSettings";

       m_settings.remove("exts");
       m_settings.setValue("exts", m_default_formats);

       QStringList apps;
       QVariantList exts_delete;
       int quant_elements = m_default_formats.count();

       for(int i = 0; i < quant_elements; i++)
       {
          apps.append(getDefaultProgram());
          exts_delete.append(true);
       }

       m_settings.remove("apps");
       m_settings.setValue("apps", apps);
       m_settings.remove("exts_delete");
       m_settings.setValue("exts_delete", exts_delete);

       m_settings.setValue("resource", m_default_resource);
       m_settings.setValue("folder", m_default_folder);
       m_settings.setValue("seconds", m_default_interval);
       emit settingsRestored();

       qDebug() << "END" << metaObject()->className() << ":: restoreSettings";
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

