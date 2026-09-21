#include "memoria.h"

// Aqui viven las cuatro variables que describen el tablero. Son 'static' para
// que nadie de afuera las toque directo; solo se llega a ellas por las funciones.
static unsigned char* g_buffer  = 0;
static int            g_filas   = 0;
static int            g_columnas = 0;
static long           g_bytes   = 0;

long bytesNecesarios(int f, int c)
{
    long bits = 3L * f * c;
    return (bits + 7) / 8;              // esto es el techo de la division (redondeo hacia arriba)
}

void crearTablero(int f, int c)
{
    g_filas    = f;
    g_columnas = c;
    g_bytes    = bytesNecesarios(f, c);
    g_buffer   = new unsigned char[g_bytes];
    for (long i = 0; i < g_bytes; i++) g_buffer[i] = 0;   // arranco todo en cero
}

void liberarTablero()
{
    delete[] g_buffer;
    g_buffer = 0;
    g_filas = g_columnas = 0;
    g_bytes = 0;
}

int  filas()          { return g_filas; }
int  columnas()       { return g_columnas; }
long posiciones()     { return (long)g_filas * g_columnas; }
long bytesReservados(){ return g_bytes; }
unsigned char* buffer(){ return g_buffer; }

void fijarBloque(unsigned char* nuevo, int f, int c, long bytes)
{
    delete[] g_buffer;    // suelto el bloque viejo antes de quedarme con el nuevo
    g_buffer   = nuevo;
    g_filas    = f;
    g_columnas = c;
    g_bytes    = bytes;
}

void fijarDimensiones(int f, int c)
{
    g_filas    = f;
    g_columnas = c;
}
