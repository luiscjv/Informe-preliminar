#include <QCoreApplication>   // esto arranca el framework Qt
#include "salida.h"           // mi capa de entrada/salida hecha sobre Qt
#include "juego.h"
#include "tablero.h"
#include "memoria.h"
#include "aleatorio.h"

// Aqui solo pinto el menu de opciones en pantalla.
static void menu()
{
    sout() << "\n  --- Menu ---\n"
           << "  1) Mostrar tablero (fichas)\n"
           << "  2) Eliminar ficha (fila columna)\n"
           << "  3) Agregar fila (posicion)\n"
           << "  4) Eliminar fila (posicion)\n"
           << "  5) Agregar columna (posicion)\n"
           << "  6) Eliminar columna (posicion)\n"
           << "  7) Estado del juego\n"
           << "  8) Mostrar tablero (binario)\n"
           << "  0) Salir\n"
           << "  Opcion: ";
    vaciarSalida();            // fuerzo la salida para que el prompt se vea antes de leer
}

// Leo un entero del usuario. Devuelvo false si ya no queda nada por leer (fin de
// la entrada) o si vino algo que no es numero; con eso corto los bucles.
static bool leerEntero(int& destino)
{
    sentrada() >> destino;
    return sentrada().status() == QTextStream::Ok;
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);   // enciendo el nucleo de Qt
    Q_UNUSED(app);                      // no lo uso directo, pero tiene que existir

    int F, C;
    sout() << "=== Sweet Crush (representacion a 3 bits) ===\n";
    sout() << "Filas: ";    vaciarSalida();
    if (!leerEntero(F)) return 0;
    sout() << "Columnas: "; vaciarSalida();
    if (!leerEntero(C)) return 0;
    if (F < 1 || C < 1) { sout() << "Dimensiones invalidas.\n"; vaciarSalida(); return 0; }

    sembrarAleatorio(2u);               // semilla fija: la partida sale siempre igual, facil de revisar
    iniciarJuego(F, C);
    sout() << "\nTablero inicial:";
    mostrarFichas();

    // Este es el bucle principal: muestro el menu, leo la opcion y hago lo que toque.
    int op = -1;
    while (true) {
        menu();
        if (!leerEntero(op)) break;
        if (op == 0) break;

        if (op == 1) {
            mostrarFichas();
        } else if (op == 2) {
            int f, c;
            if (!leerEntero(f) || !leerEntero(c)) break;
            sout() << "\n> Eliminar ficha en (" << f << "," << c << ")\n";
            eliminarFichaUsuario(f, c);
            mostrarFichas();
            sout() << "  (cascadas en esta jugada: " << cascadasJugada
                   << ", puntuacion: " << puntuacion << ")\n";
        } else if (op == 3) {
            int p; if (!leerEntero(p)) break;
            sout() << "\n> Agregar fila en posicion " << p << "\n";
            agregarFila(p); resolverCascadas(); mostrarFichas();
        } else if (op == 4) {
            int p; if (!leerEntero(p)) break;
            sout() << "\n> Eliminar fila en posicion " << p << "\n";
            eliminarFila(p); resolverCascadas(); mostrarFichas();
        } else if (op == 5) {
            int p; if (!leerEntero(p)) break;
            sout() << "\n> Agregar columna en posicion " << p << "\n";
            agregarColumna(p); resolverCascadas(); mostrarFichas();
        } else if (op == 6) {
            int p; if (!leerEntero(p)) break;
            sout() << "\n> Eliminar columna en posicion " << p << "\n";
            eliminarColumna(p); resolverCascadas(); mostrarFichas();
        } else if (op == 7) {
            mostrarEstado();
        } else if (op == 8) {
            mostrarBinario();
        } else {
            sout() << "  Opcion no reconocida.\n";
        }
    }

    // Cierre: muestro el estado final y suelto la memoria del tablero.
    sout() << "\nFin de la partida.";
    mostrarEstado();
    vaciarSalida();
    liberarTablero();
    return 0;
}
