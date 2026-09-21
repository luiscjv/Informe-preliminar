#-------------------------------------------------
# Sweet Crush - proyecto de consola sobre Qt 5
# Construir con:  qmake && make
#-------------------------------------------------
QT       += core
QT       -= gui

CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE  = app
TARGET    = sweetcrush

SOURCES  += main.cpp \
            salida.cpp \
            aleatorio.cpp \
            bits.cpp \
            memoria.cpp \
            tablero.cpp \
            juego.cpp

HEADERS  += salida.h \
            aleatorio.h \
            bits.h \
            memoria.h \
            tablero.h \
            juego.h
