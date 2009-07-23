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
   \brief Abre y borra regularmente los PDFs que aparecen en cierta ruta.

   La clase TNxSpooler se encarga de comprobar cada cierto intervalo de tiempo si
   existen ficheros PDF en una ruta indicada para abrirlos y borrarlos.
   Dicha ruta se encuentra compartida por red para que la use una conexión FreeNX.
   De este modo, basta con que funcione la compartición de ficheros por Samba
   en la sesión FreeNX para que se abra automáticamente en nuestra sesión local
   cualquier fichero PDF que se genere en la ruta.

   CASO PARTICULAR: BulmaGés
   Hay que compartir nuestra carpeta personal ".bulmages" como "bulmages$".
   El cliente de FreeNX debe ser configurado para que monte dicho recurso
   como carpeta personal "~/.bulmages" en la sesión remota.
   Cuando BulmaGés imprime algo, se genera un PDF en dicha ruta compartida.
   NxSpooler detectará ese fichero y lo abrirá localmente.
   Para evitar que se abra también en la sesión remota,
   editar el fichero "/etc/bulmages/bulmages.conf" y
   reemplazar "konqueror" por "echo" en el parámetro "CONF_PDF".
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
   m_recurso_predeterminado = t("bulmages$");
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
      if (t(excep.what()).startsWith("2805093") || t(excep.what()).startsWith("0707091"))
      {
         sist.mostrarAviso(t(excep.what()) + ".");
         show();
         abrirOpciones();
      }
      else
      {
         throw(excep);
      }
   }

   prepararTemporizador();

   qDebug() << "FIN"<<metaObject()->className() << ":: TNxSpooler";
}


//! Destruye TNxSpooler.
/*!
  De momento no lo usamos para nada (podría no estar definido).
*/
TNxSpooler::~TNxSpooler()
{
   qDebug() << "___" << metaObject()->className() << ":: ~TNxSpooler";

   qDebug() << "FIN"<<metaObject()->className() << ":: ~TNxSpooler";
}


//! Abre y borra los PDFs que se encuentra.
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
      qDebug() << "FIN" << metaObject()->className() << ":: abrir ADELANTADO";
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

#ifdef Q_WS_WIN
      argumentos << "/C" << "start" << "/wait" << m_ajustes.value("app").toString();
#endif

      argumentos << QDir::toNativeSeparators(archivo.absoluteFilePath());
      QProcess proceso(this);

#ifdef Q_WS_WIN
      resultado = proceso.execute("cmd", argumentos);
#else
      // Si estamos en Linux y el usuario ha dejado en blanco el nombre de la aplicación PDF,
      // usar la que se encuentre en este momento respetando ese ajuste vacío
      if (m_ajustes.value("app").toString().isEmpty())
      {
         resultado = proceso.execute(programaPredeterminadoLinux(), argumentos);
      }
      else
      {
         resultado = proceso.execute(m_ajustes.value("app").toString(), argumentos);
      }
#endif

      // La apertura puede fallar en el caso de que el fichero esté todavía a mitad de generarse
      if (resultado != 0)
      {
         sist.mostrarAviso(t("No se pudo abrir el archivo \"%1\"").arg(archivo.absoluteFilePath()));
      }

      // Eliminar el fichero si se ha podido abrir correctamente
      borrado = ruta.remove(archivo.fileName());
      if (!borrado)
      {
         QString mensaje = t("2805096 - No se pudo borrar el archivo \"%1\"").arg(archivo.absoluteFilePath());
         throw runtime_error(mensaje.toStdString());
      }

      // Si se pudo abrir y se pudo borrar el fichero, agregarlo al histórico
      m_listFiles->addItem(archivo.fileName());
   }

   qDebug() << "FIN" << metaObject()->className() << ":: abrir";
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

   qDebug() << "FIN" << metaObject()->className() << ":: abrirAcercaDe";
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
   while(!sist.existePrograma(m_ajustes.value("app").toString()));

   qDebug() << "FIN" << metaObject()->className() << ":: abrirOpciones";
}


//! Reduce el listado a los ficheros PDF y ordena por hora de modificación.
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
   filtros<<"*.pdf";
   ruta.setNameFilters(filtros);

   ruta.setSorting(QDir::Time|QDir::Reversed);

   qDebug() << "FIN" << metaObject()->className() << ":: filtrarOrdenarDir";
}


//! Inicializa los ajustes de la aplicación PDF.
/*!
   Asigna su valor por defecto a los ajustes que no hayan obtenido su contenido almacenado.
   Normalmente, cuando los ajustes no tienen valor es por que estamos en la primera ejecución
   del programa en el ordenador y no había ajustes guardados en el registro de Windows o los
   ficheros de configuración de nuestro home de Linux.
*/
void TNxSpooler::inicializarAjustes()
{
   qDebug() << "___" << metaObject()->className() << ":: inicializarAjustes";

   if (m_ajustes.value("segundos").isNull())
   {
      m_ajustes.setValue("segundos", m_intervalo_predeterminado);
   }

   // Se permite dejar la ruta de aplicación PDF como una cadena vacía,
   // sólo ponemos la predeterminada si el parámetro está nulo (todavía no es ni una cadena)
   if (m_ajustes.value("app").isNull())
   {
      m_ajustes.setValue("app", programaPredeterminado());
   }
   else
   {
      if (!sist.existePrograma(m_ajustes.value("app").toString()))
      {
         QString mensaje = t("0707091 - No se encuentra el programa especificado para abrir ficheros PDF");
         throw runtime_error(mensaje.toStdString());
      }
   }

   if (m_ajustes.value("ruta").isNull() || m_ajustes.value("ruta").toString().isEmpty())
   {
      m_ajustes.setValue("ruta", m_ruta_predeterminada);
   }

   if (m_ajustes.value("recurso").isNull() || m_ajustes.value("recurso").toString().isEmpty())
   {
      m_ajustes.setValue("recurso", m_recurso_predeterminado);
   }

   qDebug() << "FIN" << metaObject()->className() << ":: inicializarAjustes";
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

   qDebug() << "FIN" << metaObject()->className() << ":: mostrar";
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

   qDebug() << "FIN" << metaObject()->className() << ":: mostrarOcultar";
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

   qDebug() << "FIN" << metaObject()->className() << ":: ocultar";
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
         QString mensaje = t("2805099 - No se pudo activar el icono de la bandeja del sistema");
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

   qDebug() << "FIN" << metaObject()->className() << ":: prepararIconoBandejaOMostrarPrograma";
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
      QString pregunta = t("El directorio \"%1\" no existe. ¿Quiere crearlo?").arg(m_ajustes.value("ruta").toString());
      bool aceptado = sist.confirmar(pregunta);

      if(!aceptado)
      {
         qDebug() << "FIN"<<metaObject()->className() << ":: prepararRutaLocal() ADELANTADO";
         return;
      }

      if(ruta.mkdir(ruta.path()))
      {
         QString aviso = t("Directorio \"%1\" creado con éxito. Ahora debe compartirlo con el nombre \"%2\".").arg(m_ajustes.value("ruta").toString(), m_ajustes.value("recurso").toString());
         sist.mostrarAviso(aviso);
      }
      else
      {
         // Si no se ha podido crear el directorio inexistente, lanzar una excepción
         QString mensaje = t("2805093 - No se pudo crear el directorio \"%1\"").arg(m_ajustes.value("ruta").toString());
         throw runtime_error(mensaje.toStdString());
      }
   }

   qDebug() << "FIN"<<metaObject()->className() << ":: prepararRutaLocal";
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
      QString mensaje = t("2805094 - No se pudo activar el temporizador");
      throw runtime_error(mensaje.toStdString());
   }

   m_temporizador.start(m_ajustes.value("segundos").toInt() * 1000);

   qDebug() << "FIN"<<metaObject()->className() << ":: prepararTemporizador";
}


//! Decide qué programa usar para ver los ficheros PDF.
/*!
   En Windows, dejamos en manos del sistema la selección de la aplicación PDF por defecto.
   En Linux, se realiza una búsqueda priorizando ciertas aplicaciones.
  \return Nombre o ruta del programa a utilizar de forma predeterminada
*/
QString TNxSpooler::programaPredeterminado() const
{
   qDebug() << "___" << metaObject()->className() << ":: programaPredeterminado";

   qDebug() << "FIN" << metaObject()->className() << ":: programaPredeterminado";

#ifdef Q_WS_WIN
   return "";
#else
   return programaPredeterminadoLinux();
#endif
}


//! Decide qué programa usar en Linux para ver los ficheros PDF.
/*!
   Como último recurso se usa xdg-open, pero tiene el inconveniente de que no espera a que
   el usuario cierre el fichero abierto y por tanto se ejecuta toda la cola de ficheros al mismo tiempo.
   \return Nombre del ejecutable de la aplicación PDF que se encuentre en Linux
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
      QString mensaje = t("2805095 - No se encuentra un programa adecuado para abrir los ficheros");
      throw runtime_error(mensaje.toStdString());
   }

   qDebug() << "FIN" << metaObject()->className() << ":: programaPredeterminadoLinux";
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

   m_ajustes.setValue("app", programaPredeterminado());
   m_ajustes.setValue("recurso", m_recurso_predeterminado);
   m_ajustes.setValue("ruta", m_ruta_predeterminada);
   m_ajustes.setValue("segundos", m_intervalo_predeterminado);
   emit ajustesRestaurados();

   qDebug() << "FIN" << metaObject()->className() << ":: restaurarAjustes";
}
