#include "salida.h"
#include <cstdio>   // de aqui saco stdout y stdin

// El truco del 'static' adentro de la funcion: el QTextStream se crea la primera
// vez que llamo a la funcion, y de ahi en adelante siempre es el mismo. Asi
// consigo un unico flujo compartido sin tener una variable global suelta.
QTextStream& sout()
{
    static QTextStream flujo(stdout);
    return flujo;
}

QTextStream& sentrada()
{
    static QTextStream flujo(stdin);
    return flujo;
}

void vaciarSalida()
{
    sout().flush();   // sin esto, a veces el "Opcion: " no alcanza a verse antes de leer
}
