#include "salida.h"
#include "juego.h"
#include "tablero.h"
#include "memoria.h"
#include "bits.h"
#include "aleatorio.h"


// Estos son los marcadores de la partida. Van sueltos como variables globales
// porque no puedo meterlos en una clase, y varios modulos los consultan.
long eliminacionesUsuario = 0;
long fichasEliminadas     = 0;
long combosDetectados     = 0;
long cascadasJugada       = 0;
long cascadasTotales      = 0;
long puntuacion           = 0;

// ---- Deteccion de combinaciones en dos pasadas ----
// Esto lo hago en dos vueltas a proposito. En la primera marco (sin borrar) toda
// ficha que este en una linea de 3 o mas iguales, horizontal o vertical. En la
// segunda recien borro lo marcado. Asi una ficha que pertenece a la vez a una
// linea horizontal y a una vertical se marca dos veces pero se borra una sola, y
// nunca borro algo a medio revisar (que romperia la deteccion perpendicular).
static int resolverCombinaciones()
{
    int  F = filas(), C = columnas();
    long n = posiciones();

    bool* marca = new bool[n];        // arreglo temporal para ir marcando; lo libero al final
    for (long i = 0; i < n; i++) marca[i] = false;

    int combos = 0;

    // Reviso las lineas horizontales.
    for (int f = 0; f < F; f++) {
        int c = 0;
        while (c < C) {
            int v = leerFicha(f, c);
            int run = 1;
            while (c + run < C && v != VACIO && leerFicha(f, c + run) == v) run++;
            if (v != VACIO && run >= 3) {
                combos++;
                for (int k = 0; k < run; k++) marca[(long)f * C + (c + k)] = true;
            }
            c += run;
        }
    }
    // Y ahora las verticales, de la misma forma.
    for (int c = 0; c < C; c++) {
        int f = 0;
        while (f < F) {
            int v = leerFicha(f, c);
            int run = 1;
            while (f + run < F && v != VACIO && leerFicha(f + run, c) == v) run++;
            if (v != VACIO && run >= 3) {
                combos++;
                for (int k = 0; k < run; k++) marca[(long)(f + k) * C + c] = true;
            }
            f += run;
        }
    }

    // Segunda vuelta: ahora si borro (pongo VACIO) todo lo que quedo marcado.
    int elim = 0;
    for (long i = 0; i < n; i++)
        if (marca[i]) { escribir3(buffer(), i, VACIO); elim++; }

    delete[] marca;
    combosDetectados += combos;
    return elim;
}

// ---- Gravedad y relleno ----
// Despues de borrar quedan huecos. Aqui hago "caer" las fichas hacia abajo en
// cada columna y relleno los huecos de arriba con fichas nuevas al azar.
static void gravedadYRelleno()
{
    int F = filas(), C = columnas();
    for (int c = 0; c < C; c++) {
        int destino = F - 1;
        for (int r = F - 1; r >= 0; r--) {
            int v = leerFicha(r, c);
            if (v != VACIO) { escribirFicha(destino, c, v); destino--; }
        }
        for (int r = destino; r >= 0; r--)
            escribirFicha(r, c, aleatorioEnRango(TIPOS));   // lo que sobra arriba, fichas nuevas
    }
}

// ---- El bucle de cascadas ----
// Una jugada se puede encadenar: al caer las fichas se arman combinaciones
// nuevas. Entonces repito caer-y-detectar hasta que ya no se borre nada mas.
void resolverCascadas()
{
    cascadasJugada = 0;
    while (true) {
        gravedadYRelleno();
        int elim = resolverCombinaciones();
        if (elim == 0) break;
        cascadasJugada++;
        cascadasTotales++;
        fichasEliminadas += elim;
        puntuacion += (long)elim * VALOR_BASE * cascadasJugada;  // mientras mas larga la cadena, mas puntos
    }
}

// ---- Arranque de la partida ----
void iniciarJuego(int f, int c)
{
    crearTablero(f, c);
    llenarAleatorio();
    // El tablero recien llenado puede nacer ya con combinaciones hechas. Las
    // limpio antes de empezar, pero SIN sumar puntos, porque el jugador no las
    // gano; contarlas seria regalarle puntaje.
    while (true) {
        int elim = resolverCombinaciones();
        if (elim == 0) break;
        gravedadYRelleno();
    }
    // Dejo todos los marcadores en cero para arrancar limpio.
    eliminacionesUsuario = 0;
    fichasEliminadas     = 0;
    combosDetectados     = 0;
    cascadasJugada       = 0;
    cascadasTotales      = 0;
    puntuacion           = 0;
}

// ---- La jugada del usuario ----
void eliminarFichaUsuario(int f, int c)
{
    // Primero valido que la casilla exista y que no este ya vacia.
    if (f < 0 || f >= filas() || c < 0 || c >= columnas()) {
        sout() << "  Posicion invalida.\n"; return;
    }
    if (leerFicha(f, c) == VACIO) { sout() << "  Esa posicion ya esta vacia.\n"; return; }

    // Borro la ficha elegida y dejo que las cascadas hagan el resto del trabajo.
    escribirFicha(f, c, VACIO);
    eliminacionesUsuario++;
    fichasEliminadas++;
    resolverCascadas();
}

// ---- Estado de la partida ----
void mostrarEstado()
{
    // Aqui solo imprimo el resumen de marcadores, nada de logica.
    sout() << "\n  === Estado del juego ===\n";
    sout() << "  Dimensiones            : " << filas() << " x " << columnas() << "\n";
    sout() << "  Bytes reservados       : " << bytesReservados() << "\n";
    sout() << "  Eliminaciones usuario  : " << eliminacionesUsuario << "\n";
    sout() << "  Fichas eliminadas total: " << fichasEliminadas << "\n";
    sout() << "  Combinaciones detectadas: " << combosDetectados << "\n";
    sout() << "  Cascadas (ultima jugada): " << cascadasJugada << "\n";
    sout() << "  Cascadas totales       : " << cascadasTotales << "\n";
    sout() << "  Puntuacion             : " << puntuacion << "\n";
}
