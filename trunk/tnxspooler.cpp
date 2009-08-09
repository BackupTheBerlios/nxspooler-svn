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
   \class TNxSpooler
   \brief Abre y borra regularmente los archivos de ciertas extensiones que aparecen en una ruta.

   La clase TNxSpooler se encarga de comprobar cada cierto intervalo de tiempo si
   existen archivos en una ruta indicada para abrirlos y borrarlos.
   Dicha ruta se encuentra compartida por red para que la use una conexión FreeNX.
   De este modo, basta con que funcione la compartición de ficheros por Samba
   en la sesión FreeNX para que se abra automáticamente en nuestra sesión local
   cualquier fichero con la extensión deseada que se genere en la ruta.

   CASO PARTICULAR: BulmaGés
   Hay que compartir nuestra carpeta personal ".bulmages" como "bulmages$".
   El cliente de FreeNX debe ser configurado para que monte dicho recurso
   como carpeta personal "~/.bulmages" en la sesión remota.
   Cuando BulmaGés imprime algo, se genera un archivo PDF, ODS o SXC en
   dicha ruta compartida.
   NxSpooler detectará ese fichero y lo abrirá localmente.
   Para evitar que se abra también en la sesión remota,
   editar el fichero "/etc/bulmages/bulmages.conf" y
   dejar los parámetros "CONF_PDF", "CONF_ODS" y "CONF_SXC"
   con el valor "echo".
*/

#include "tnxspooler.h"

//! Construye un objeto TNxSpooler asignándole un padre.
/*!
   Si no ocurre alguna excepción que lo impida, se inician los diferentes
   aspectos de un objeto TNxSpooler.
   \param padre Si no se especifica, el objeto TNxSpooler será una ventana independiente
*/
TNxSpooler::TNxSpooler(QWidget *padre)
   : QDialog(padre)
{
   qDebug() << "___" << metaObject()->className() << ":: TNxSpooler";

   // Adoptar a los objetos miembro que lo requieran
   m_ajustes.setParent(this);
   m_icono_bandeja.setParent(this);
   m_temporizador.setParent(this);

   // Definir los valores predeterminados "constantes"
   m_intervalo_predeterminado = 3;
   m_formatos_predeterminados.append("pdf");
   m_formatos_predeterminados.append("ods");
   m_formatos_predeterminados.append("sxc");
   m_recurso_predeterminado = "bulmages$"; // Nota: esta cadena no se traducirá
   m_ruta_predeterminada = QDir::toNativeSeparators(QDir::homePath().append(QDir::separator()).append(".bulmages"));

   setupUi(this);
   prepararIconoBandejaOMostrarPrograma();

   try
   {
      inicializarAjustes();
      prepararRutaLocal();
   }
   catch (std::exception &excep)
   {
      // Si no existía la ruta y no se ha podido crear o si no existe el programa,
      // llevar al usuario al diálogo de opciones
      if (QString(excep.what()).startsWith("2805093") || QString(excep.what()).startsWith("0707091"))
      {
         sist.mostrarAviso(QString(excep.what()) + ".");
         show();
         abrirOpciones();
      }
      else
      {
         throw(excep);
      }
   }

   prepararTemporizador();

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
void TNxSpooler::abrir()
{
   qDebug() << "___" << metaObject()->className() << ":: abrir";

   QDir ruta(m_ajustes.value("ruta").toString());
   filtrarOrdenarDir(ruta);

   // No continuar en este método si no existen ficheros a tratar
   if (ruta.count() == 0)
   {
      qDebug() << "END" << metaObject()->className() << ":: abrir AHEAD";
      return;
   }

   QStringList argumentos;
   bool borrado = false;
   int resultado = 0;
   QFileInfoList archivos = ruta.entryInfoList();

   // Abrir uno a uno cada fichero y si ha funcionado bien, borrarlo
   foreach(QFileInfo archivo, archivos)
   {
      argumentos.clear();

      int i = m_ajustes.value("exts").toStringList().indexOf(archivo.completeSuffix());
      QString app = m_ajustes.value("apps").toStringList().value(i);

#ifdef Q_WS_WIN
      argumentos << "/C" << "start" << "/wait" << app;
#endif

      argumentos << QDir::toNativeSeparators(archivo.absoluteFilePath());
      QProcess proceso(this);

#ifdef Q_WS_WIN
      resultado = proceso.execute("cmd", argumentos);
#else
      // Si estamos en Linux y el usuario ha dejado en blanco el nombre de la aplicación de apertura,
      // usar la que se encuentre en este momento respetando ese ajuste vacío
      if (app.isEmpty())
      {
         resultado = proceso.execute(programaPredeterminadoLinux(), argumentos);
      }
      else
      {
         resultado = proceso.execute(app, argumentos);
      }
#endif

      // La apertura puede fallar en el caso de que el fichero esté todavía a mitad de generarse
      if (resultado != 0)
      {
         sist.mostrarAviso(tr("The file \"%1\" could not be opened").arg(archivo.absoluteFilePath()));
      }

      // Eliminar el fichero si se ha podido abrir correctamente
      borrado = ruta.remove(archivo.fileName());
      if (!borrado)
      {
         QString mensaje = tr("2805096 - The file \"%1\" could not be deleted").arg(archivo.absoluteFilePath()); 
         throw runtime_error(mensaje.toStdString());
      }

      // Si se pudo abrir y se pudo borrar el fichero, agregarlo al histórico
      m_listFiles->addItem(archivo.fileName());
   }

   qDebug() << "END" << metaObject()->className() << ":: abrir";
}


//! Muestra información sobre el programa, los autores y la licencia.
/*!
*/
void TNxSpooler::abrirAcercaDe()
{
   qDebug() << "___" << metaObject()->className() << ":: abrirAcercaDe";

   Ui::aboutDialog ui_acerca_de;
   QDialog acerca_de(this);
   ui_acerca_de.setupUi(&acerca_de);

   acerca_de.exec();

   qDebug() << "END" << metaObject()->className() << ":: abrirAcercaDe";
}


//! Muestra el diálogo de opciones.
/*!
*/
void TNxSpooler::abrirOpciones()
{
   qDebug() << "___" << metaObject()->className() << ":: abrirOpciones";

   TOpciones opciones(&m_ajustes, this);

   // Cuando se pulsa el botón de restaurar, los ajustes del programa vuelven a sus valores predeterminados
   connect(&opciones, SIGNAL(restaurarPulsado()),
                this, SLOT(restaurarAjustes()));

   // Tras ser restaurados los ajustes del programa, el cambio debe reflejarse en el diálogo
   connect(this, SIGNAL(ajustesRestaurados()),
      &opciones, SLOT(actualizarCamposOpciones()));

   do
   {
      opciones.exec();
      prepararRutaLocal();
      prepararTemporizador();
   }
   while(!sist.existePrograma(m_ajustes.value("apps").toString()));

   qDebug() << "END" << metaObject()->className() << ":: abrirOpciones";
}


//! Reduce el listado a los ficheros con las extensiones deseadas y ordena por hora de modificación.
/*!
   Con esta manera de ordenar nos aseguramos de que el documento que lleva más tiempo esperando es el primero
   en ser abierto.
   \param ruta Gestor del directorio
*/
void TNxSpooler::filtrarOrdenarDir(QDir &ruta)
{
   qDebug() << "___" << metaObject()->className() << ":: filtrarOrdenarDir";

   // Tratar sólo ciertos tipos de ficheros mediante un filtro.
   // Tanto en Windows como en Linux no diferenciará mayúsculas de minúsculas.
   QStringList filtros;
   QStringList exts = m_ajustes.value("exts").toStringList();
   for(int i = 0; i < exts.count(); i++)
   {
      filtros << QString("*.%1").arg(exts.value(i));
   }
   ruta.setNameFilters(filtros);

   ruta.setSorting(QDir::Time|QDir::Reversed);

   qDebug() << "END" << metaObject()->className() << ":: filtrarOrdenarDir";
}


//! Inicializa los ajustes de NxSpooler.
/*!
   Asigna su valor por defecto a los ajustes que no hayan obtenido su contenido almacenado.
   Normalmente, cuando los ajustes no tienen valor es por que estamos en la primera ejecución
   del programa en el ordenador y no había ajustes guardados en el registro de Windows o los
   ficheros de configuración en nuestro "home" de Linux.
*/
void TNxSpooler::inicializarAjustes()
{
   qDebug() << "___" << metaObject()->className() << ":: inicializarAjustes";

   if (m_ajustes.value("segundos").isNull())
   {
      m_ajustes.setValue("segundos", m_intervalo_predeterminado);
   }

   if (m_ajustes.value("exts").isNull())
   {
      m_ajustes.setValue("exts", m_formatos_predeterminados);
   }

   // Nota: se permite dejar una ruta de aplicación como una cadena vacía,
   // sólo ponemos las predeterminadas si el ajuste está nulo (todavía no es ni una lista de cadenas)
   if (m_ajustes.value("apps").isNull())
   {
      const int elementos = m_ajustes.value("exts").toStringList().count();
      QStringList apps;

      for(int i = 0; i < elementos; i++)
      {
         apps.append(programaPredeterminado());
      }
      m_ajustes.setValue("apps", apps);
   }

   if (m_ajustes.value("ruta").isNull() || m_ajustes.value("ruta").toString().isEmpty())
   {
      m_ajustes.setValue("ruta", m_ruta_predeterminada);
   }

   if (m_ajustes.value("recurso").isNull() || m_ajustes.value("recurso").toString().isEmpty())
   {
      m_ajustes.setValue("recurso", m_recurso_predeterminado);
   }

   qDebug() << "END" << metaObject()->className() << ":: inicializarAjustes";
}


//! Muestra la ventana de NxSpooler.
/*!
*/
void TNxSpooler::mostrar()
{
   qDebug() << "___" << metaObject()->className() << ":: mostrar";

   // Evitar que NxSpooler termine al cerrar diálogos con la ventana principal oculta
   qApp->setQuitOnLastWindowClosed(true);

   // No usamos show() porque a veces la ventana se muestra minimizada
   showNormal();

   // Evitar que en ocasiones NxSpooler se restaure sin foco
   //NOTE: De todos modos, en Windows no está permitido que un programa
   // robe el foco a otra que lo está usando
   qApp->setActiveWindow(this);
   qApp->activeWindow()->raise();
   m_icono_bandeja.contextMenu()->insertAction(m_action_show, m_action_hide);
   m_icono_bandeja.contextMenu()->removeAction(m_action_show);

   qDebug() << "END" << metaObject()->className() << ":: mostrar";
}


//! Oculta o muestra alternadamente la ventana de NxSpooler.
/*!
  \param motivo Código de evento que genera el icono de bandeja (el botón pulsado)
*/
void TNxSpooler::mostrarOcultar(QSystemTrayIcon::ActivationReason motivo)
{
   qDebug() << "___" << metaObject()->className() << ":: mostrarOcultar";

   // Si el usuario hace click sobre el icono de la bandeja, mostrar u ocultar el programa
   if (motivo == QSystemTrayIcon::Trigger)
   {
      if (isHidden())
      {
         mostrar();
      }
      else
      {
         ocultar();
      }
   }

   qDebug() << "END" << metaObject()->className() << ":: mostrarOcultar";
}


//! Oculta la ventana de NxSpooler.
/*!
*/
void TNxSpooler::ocultar()
{
   qDebug() << "___" << metaObject()->className() << ":: ocultar";

   hide();

   // Permitir que cuando la última ventana de NxSpooler se cierre, éste termine
   qApp->setQuitOnLastWindowClosed(false);

   if(QSystemTrayIcon::isSystemTrayAvailable())
   {
      m_icono_bandeja.contextMenu()->insertAction(m_action_hide, m_action_show);
      m_icono_bandeja.contextMenu()->removeAction(m_action_hide);
   }

   qDebug() << "END" << metaObject()->className() << ":: ocultar";
}


//! Activa un icono en la bandeja de sistema si es posible. En caso contrario, muestra la ventana principal de forma normal.
/*!
*/
void TNxSpooler::prepararIconoBandejaOMostrarPrograma()
{
   qDebug() << "___" << metaObject()->className() << ":: prepararIconoBandejaOMostrarPrograma";

   if (QSystemTrayIcon::isSystemTrayAvailable())
   {
      m_icono_bandeja.setIcon(windowIcon());
      bool conectado = connect(&m_icono_bandeja, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                                           this, SLOT(mostrarOcultar(QSystemTrayIcon::ActivationReason)));

      if (!conectado)
      {
         // Si no se ha podido establecer la conexión, lanzar una excepción
         QString mensaje = tr("2805099 - The icon of the notification area could not be activated");
         throw runtime_error(mensaje.toStdString());
      }

      // Preparar el menú contextual del icono de bandeja
      QMenu *menu = new QMenu(this);
      menu->addAction(m_action_show);
      menu->addSeparator();
      menu->addAction(m_action_options);
      menu->addAction(m_action_about);
      menu->addSeparator();
      menu->addAction(m_action_exit);
      m_icono_bandeja.setContextMenu(menu);

      m_icono_bandeja.show();
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

   qDebug() << "END" << metaObject()->className() << ":: prepararIconoBandejaOMostrarPrograma";
}


//! Comprobar si existe la ruta e intentar crearla en caso de que no.
/*!
*/
void TNxSpooler::prepararRutaLocal()
{
   qDebug() << "___" << metaObject()->className() << ":: prepararRutaLocal";

   // Crear el gestor de la ruta a comprobar
   QDir ruta(m_ajustes.value("ruta").toString());

   // Crear el directorio si no existe
   if (!ruta.exists())
   {
      QString pregunta = tr("The folder \"%1\" does not exist. Do you want to create it?").arg(m_ajustes.value("ruta").toString());
      bool aceptado = sist.confirmar(pregunta);

      if(!aceptado)
      {
         qDebug() << "END" << metaObject()->className() << ":: prepararRutaLocal() AHEAD";
         return;
      }

      if(ruta.mkdir(ruta.path()))
      {
         QString aviso = tr("The folder \"%1\" was successfully created. Now it must be shared with the name \"%2\".") 
                         .arg(m_ajustes.value("ruta").toString(), m_ajustes.value("recurso").toString());
         sist.mostrarAviso(aviso);
      }
      else
      {
         // Si no se ha podido crear el directorio inexistente, lanzar una excepción
         QString mensaje = tr("2805093 - The folder \"%1\" could not be created").arg(m_ajustes.value("ruta").toString());
         throw runtime_error(mensaje.toStdString());
      }
   }

   qDebug() << "END" << metaObject()->className() << ":: prepararRutaLocal";
}


//! Activa un temporizador que se ejecutará el slot abrir según el intervalo especificado.
/*!
*/
void TNxSpooler::prepararTemporizador()
{
   qDebug() << "___" << metaObject()->className() << ":: prepararTemporizador";

   // Evitar que sucesivas llamadas a este método intenten abrir varias veces al mismo tiempo
   disconnect(&m_temporizador, SIGNAL(timeout()),
                         this, SLOT(abrir()));

   bool conectado = connect(&m_temporizador, SIGNAL(timeout()),
                                       this, SLOT(abrir()));

   if (conectado == false)
   {
      // Si no se ha podido establecer la conexión, lanzar una excepción
      QString mensaje = tr("2805094 - The timer could not be activated");
      throw runtime_error(mensaje.toStdString());
   }

   m_temporizador.start(m_ajustes.value("segundos").toInt() * 1000);

   qDebug() << "END" << metaObject()->className() << ":: prepararTemporizador";
}


//! Decide qué programa usar para abrir archivos.
/*!
   En Windows, dejamos en manos del sistema la selección de la aplicación visora por defecto.
   En Linux, se realiza una búsqueda priorizando ciertas aplicaciones.
  \return Nombre o ruta del programa a utilizar de forma predeterminada
*/
QString TNxSpooler::programaPredeterminado() const
{
   qDebug() << "___" << metaObject()->className() << ":: programaPredeterminado";

   qDebug() << "END" << metaObject()->className() << ":: programaPredeterminado";

#ifdef Q_WS_WIN
   return "";
#else
   return programaPredeterminadoLinux();
#endif
}


//! Decide qué programa usar en Linux para ver los archivos.
/*!
   Como último recurso se usa xdg-open, pero tiene el inconveniente de que no espera a que
   el usuario cierre el fichero abierto y por tanto se ejecuta toda la cola de ficheros al mismo tiempo.
   \return Nombre del ejecutable de la aplicación que se encuentre en Linux
*/
QString TNxSpooler::programaPredeterminadoLinux() const
{
   qDebug() << "___" << metaObject()->className() << ":: programaPredeterminadoLinux";

   QString comando;
   QStringList comandos;
   comandos<<"run-mailcap"<<"sensible-browser"<<"okular"<<"kpdf"<<"evince"<<"xpdf"<<"epdfview"<<"acroread"<<"xdg-open";

   // Recorrer los elementos de la lista hasta encontrar uno accesible por ruta o por PATH
   foreach(comando, comandos)
   {
      if (sist.existePrograma(comando))
      {
         break;
      }
   }

   // Si no se ha podido encontrar un programa adecuado, lanzar una excepción
   if (!sist.existePrograma(comando))
   {
      QString mensaje = tr("2805095 - A valid program to open the files could not be found");
      throw runtime_error(mensaje.toStdString());
   }

   qDebug() << "END" << metaObject()->className() << ":: programaPredeterminadoLinux";
   return comando;
}


//! Deja los ajustes del programa en sus valores predeterminados.
/*!
  Al final se emite una señal que capta el diálogo de opciones si está abierto
  para que adapte sus campos de opciones a los nuevos ajustes.
*/
void TNxSpooler::restaurarAjustes()
{
   qDebug() << "___" << metaObject()->className() << ":: restaurarAjustes";

   m_ajustes.remove("exts");
   m_ajustes.setValue("exts", m_formatos_predeterminados);

   QStringList apps;
   int elementos = m_formatos_predeterminados.count();

   for(int i = 0; i < elementos; i++)
   {
      apps.append(programaPredeterminado());
   }
   m_ajustes.remove("apps");
   m_ajustes.setValue("apps", apps);

   m_ajustes.setValue("recurso", m_recurso_predeterminado);
   m_ajustes.setValue("ruta", m_ruta_predeterminada);
   m_ajustes.setValue("segundos", m_intervalo_predeterminado);
   emit ajustesRestaurados();

   qDebug() << "END" << metaObject()->className() << ":: restaurarAjustes";
}
