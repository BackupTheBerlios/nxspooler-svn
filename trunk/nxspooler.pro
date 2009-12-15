TEMPLATE = app
TARGET = 
DEPENDPATH += . \
    qtsingleapplication
INCLUDEPATH += . \
    qtsingleapplication

# Input
HEADERS += tsystrayicon.h \
    toptions.h \
    tsystem.h \
    qtsingleapplication/qtlocalpeer.h \
    qtsingleapplication/qtlockedfile.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/qtsinglecoreapplication.h \
    tnxspooler.h
FORMS += aboutDialog.ui \
    nxspooler.ui \
    optionsDialog.ui \
    helpDialog.ui
SOURCES += main.cpp \
    tsystrayicon.cpp \
    toptions.cpp \
    tsystem.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    qtsingleapplication/qtlockedfile.cpp \
    qtsingleapplication/qtlockedfile_win.cpp \
    qtsingleapplication/qtlockedfile_unix.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtsinglecoreapplication.cpp \
    tnxspooler.cpp
RESOURCES += nxspooler.qrc
win32 { 
    # In release mode: disable the debug messages
    CONFIG(release, debug|release):DEFINES += NDEBUG
    
    # In debug mode: enable the debug messages (necessary in Windows)
    CONFIG(debug, debug|release):CONFIG += console
}
unix:# In release mode: disable the debug messages
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# The QtSingleApplication indirectly uses the network library
QT *= network
TRANSLATIONS = nxspooler_es.ts
