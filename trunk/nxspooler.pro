TEMPLATE = app
TARGET = 
DEPENDPATH += . \
    qtsingleapplication \
    lib_src
INCLUDEPATH += . \
    qtsingleapplication \
    lib_src

# Input
HEADERS += toptions.h \
    tnxspooler.h \
    lib_src/tsystem.h \
    lib_src/tdebug.h \
    lib_src/tdebugindentation.h \
    lib_src/tsystrayicon.h \
    lib_src/ttranslator.h \
    qtsingleapplication/qtlocalpeer.h \
    qtsingleapplication/qtlockedfile.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/qtsinglecoreapplication.h
FORMS += aboutDialog.ui \
    nxspooler.ui \
    optionsDialog.ui \
    helpDialog.ui
SOURCES += main.cpp \
    toptions.cpp \
    tnxspooler.cpp \
    lib_src/tsystem.cpp \
    lib_src/tdebug.cpp \
    lib_src/tdebugindentation.cpp \
    lib_src/tsystrayicon.cpp \
    lib_src/ttranslator.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    qtsingleapplication/qtlockedfile.cpp \
    qtsingleapplication/qtlockedfile_win.cpp \
    qtsingleapplication/qtlockedfile_unix.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtsinglecoreapplication.cpp
RESOURCES += nxspooler.qrc

win32 { 
    # In debug mode: enable the debug messages (this step was necessary in Windows)
    CONFIG(debug, debug|release):CONFIG += console
}

unix { 
    # In release mode: disable the debug messages
    CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

    isEmpty(RELEASE_INSTALLATION_FOLDER) { # Remember, don't put the "{" in the next line.
        # Note: you can execute, for example, 'qmake "RELEASE_INSTALLATION_FOLDER=/usr"' but
        # if you don't, RELEASE_INSTALLATION_FOLDER must have a default value.
        # Give a value to the variable RELEASE_INSTALLATION_FOLDER if it's empty.
        RELEASE_INSTALLATION_FOLDER = /usr/local
    }
   
    # Using this variable, make a "#define"
    DEFINES += RELEASE_INSTALLATION_FOLDER=\\\"$${RELEASE_INSTALLATION_FOLDER}\\\"
}

# The QtSingleApplication indirectly uses the network library
QT *= network

TRANSLATIONS = nxspooler_es.ts

RC_FILE = nxspooler.rc

# We follow the norms of http://www.physics.ohio-state.edu/~ntg/780/gsl_examples/recommended_C++_options
# except the options of -Wconversion (there were too many warnings), -g and -Werror. We also use -Wno-long-long.
QMAKE_CXXFLAGS += -pedantic -ansi -W -Wall -Wshadow -fno-common -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fshort-enums -Wno-long-long