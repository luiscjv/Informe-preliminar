#include "salida.h"
#include "tablero.h"
#include "memoria.h"
#include "bits.h"
#include "aleatorio.h"


// ---- De coordenada a numero de casilla ----
// Aqui convierto (fila, columna) en un solo indice lineal, que es lo que
// entiende la capa de bits de abajo.
static long indiceDe(int f, int c) { return (long)f * columnas() + c; }

int  leerFicha(int f, int c)            { return leer3(buffer(), indiceDe(f, c)); }
void escribirFicha(int f, int c, int v) { escribir3(buffer(), indiceDe(f, c), v); }

void llenarAleatorio()
{
    // Recorro todas las casillas y meto una ficha al azar en cada una.
    long n = posiciones();
    for (long i = 0; i < n; i++)
        escribir3(buffer(), i, aleatorioEnRango(TIPOS));
}

// ---- Las dos vistas del tablero ----
// Traduzco un codigo a su letra: 0->A, 1->B, ... 5->F, y 6->punto (vacia).
static char simbolo(int v)
{
    if (v >= 0 && v < 6) return (char)('A' + v);
    if (v == VACIO)      return '.';
    return '?';
}

void mostrarFichas()
{
    // Esta parte solo dibuja el tablero con letras, con la fila de numeros arriba
    // y el numero de fila a la izquierda. Nada de logica, puro pintar.
    sout() << "\n     ";
    for (int c = 0; c < columnas(); c++) sout() << (c % 10) << ' ';
    sout() << "\n";
    for (int f = 0; f < filas(); f++) {
        sout() << "  " << (f % 10) << ": ";
        for (int c = 0; c < columnas(); c++) sout() << simbolo(leerFicha(f, c)) << ' ';
        sout() << "\n";
    }
}

void mostrarBinario()
{
    // Aqui muestro lo que hay de verdad en memoria: primero todos los bytes en
    // binario, y despues el codigo de 3 bits de cada casilla. Lo uso para probar
    // con los ojos que el empaquetado quedo bien.
    sout() << "\n  Bytes reservados (" << bytesReservados() << "):\n   ";
    for (long i = 0; i < bytesReservados(); i++) {
        sout() << ' ';
        for (int b = 7; b >= 0; b--) sout() << ((buffer()[i] >> b) & 1);
    }
    sout() << "\n  Codigos de 3 bits por posicion:\n";
    for (int f = 0; f < filas(); f++) {
        sout() << "   ";
        for (int c = 0; c < columnas(); c++) {
            int v = leerFicha(f, c);
            sout() << ((v >> 2) & 1) << ((v >> 1) & 1) << (v & 1) << ' ';
        }
        sout() << "\n";
    }
}

// ---- Ayudante para reempaquetar ----
// Cuando el tablero encoge, quedan bits viejos "colgando" despues de los datos.
// Esta funcion los pone en cero para que la vista binaria no muestre basura.
static void limpiarRelleno(long nPos)
{
    long totalBits = 8L * bytesReservados();
    for (long bit = 3 * nPos; bit < totalBits; bit++) {
        long by = bit >> 3;
        int  off = (int)(bit & 7);
        buffer()[by] = (unsigned char)(buffer()[by] & ~(1 << off));
    }
}

// ---- Agregar una fila ----
void agregarFila(int pos)
{
    int F = filas(), C = columnas();
    if (pos < 0 || pos > F) { sout() << "  Posicion de fila invalida.\n"; return; }

    // Al crecer, los indices se corren hacia arriba, asi que NO puedo reempaquetar
    // sobre el mismo bloque (me pisaria datos). Por eso pido un bloque nuevo justo
    // del tamano que necesito y voy copiando.
    int  nuevoF = F + 1;
    long nuevoN = (long)nuevoF * C;
    long bytes  = bytesNecesarios(nuevoF, C);
    unsigned char* nuevo = new unsigned char[bytes];
    for (long i = 0; i < bytes; i++) nuevo[i] = 0;

    for (int r = 0; r < nuevoF; r++) {
        for (int c = 0; c < C; c++) {
            int v;
            if (r == pos)        v = aleatorioEnRango(TIPOS);      // esta es la fila nueva: va al azar
            else                 v = leerFicha(r < pos ? r : r - 1, c);   // las demas las copio corridas
            escribir3(nuevo, (long)r * C + c, v);
        }
    }
    fijarBloque(nuevo, nuevoF, C, bytes);
    (void)nuevoN;
}

// ---- Eliminar una fila ----
void eliminarFila(int pos)
{
    int F = filas(), C = columnas();
    if (F <= 1)                 { sout() << "  No se puede eliminar la ultima fila.\n"; return; }
    if (pos < 0 || pos >= F)    { sout() << "  Posicion de fila invalida.\n"; return; }

    int  nuevoF = F - 1;
    long nuevoN = (long)nuevoF * C;

    // Aqui SI puedo trabajar sobre el mismo bloque, porque al borrar los indices
    // solo se corren hacia abajo (el nuevo nunca es mayor que el viejo). Si voy de
    // menor a mayor, nunca piso una casilla que todavia no lei.
    for (long i = 0; i < nuevoN; i++) {
        int nr = (int)(i / C), nc = (int)(i % C);
        int orow = (nr < pos) ? nr : nr + 1;
        int v = leer3(buffer(), (long)orow * C + nc);
        escribir3(buffer(), i, v);
    }
    limpiarRelleno(nuevoN);

    // La regla del 65 %: no libero memoria por gusto. Solo encojo el bloque si el
    // aprovechamiento (bits utiles / bits reservados) cae por debajo del umbral.
    double utilizacion = (double)(3 * nuevoN) / (double)(8 * bytesReservados());
    if (utilizacion < UMBRAL_ENCOGER) {
        // Vale la pena encoger: pido un bloque mas chico y copio los datos ahi.
        long bytes = bytesNecesarios(nuevoF, C);
        unsigned char* nuevo = new unsigned char[bytes];
        for (long i = 0; i < bytes; i++) nuevo[i] = 0;
        for (long i = 0; i < nuevoN; i++)
            escribir3(nuevo, i, leer3(buffer(), i));
        fijarBloque(nuevo, nuevoF, C, bytes);
    } else {
        // Todavia aprovecho bien el bloque, asi que lo dejo igual y solo cambio
        // las dimensiones.
        fijarDimensiones(nuevoF, C);
    }
}

// ---- Agregar una columna ----
void agregarColumna(int pos)
{
    int F = filas(), C = columnas();
    if (pos < 0 || pos > C) { sout() << "  Posicion de columna invalida.\n"; return; }

    // Mismo cuento que agregar fila: como crece, toca bloque nuevo y a copiar.
    int  nuevoC = C + 1;
    long bytes  = bytesNecesarios(F, nuevoC);
    unsigned char* nuevo = new unsigned char[bytes];
    for (long i = 0; i < bytes; i++) nuevo[i] = 0;

    for (int r = 0; r < F; r++) {
        for (int c = 0; c < nuevoC; c++) {
            int v;
            if (c == pos)        v = aleatorioEnRango(TIPOS);            // columna nueva: al azar
            else                 v = leerFicha(r, c < pos ? c : c - 1);  // las demas: corridas
            escribir3(nuevo, (long)r * nuevoC + c, v);
        }
    }
    fijarBloque(nuevo, F, nuevoC, bytes);
}

// ---- Eliminar una columna ----
void eliminarColumna(int pos)
{
    int F = filas(), C = columnas();
    if (C <= 1)               { sout() << "  No se puede eliminar la ultima columna.\n"; return; }
    if (pos < 0 || pos >= C)  { sout() << "  Posicion de columna invalida.\n"; return; }

    int  nuevoC = C - 1;
    long nuevoN = (long)F * nuevoC;

    // Igual que al borrar fila: reempaqueto sobre el mismo bloque, de menor a
    // mayor indice, sin pisar nada.
    for (long i = 0; i < nuevoN; i++) {
        int nr = (int)(i / nuevoC), nc = (int)(i % nuevoC);
        int ocol = (nc < pos) ? nc : nc + 1;
        int v = leer3(buffer(), (long)nr * C + ocol);
        escribir3(buffer(), i, v);
    }
    limpiarRelleno(nuevoN);

    // Y otra vez la regla del 65 % para decidir si encojo el bloque o no.
    double utilizacion = (double)(3 * nuevoN) / (double)(8 * bytesReservados());
    if (utilizacion < UMBRAL_ENCOGER) {
        long bytes = bytesNecesarios(F, nuevoC);
        unsigned char* nuevo = new unsigned char[bytes];
        for (long i = 0; i < bytes; i++) nuevo[i] = 0;
        for (long i = 0; i < nuevoN; i++)
            escribir3(nuevo, i, leer3(buffer(), i));
        fijarBloque(nuevo, F, nuevoC, bytes);
    } else {
        fijarDimensiones(F, nuevoC);
    }
}
