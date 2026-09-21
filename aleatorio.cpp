#include "aleatorio.h"

// Aqui guardo el estado del generador. Va como variable de modulo (interna a
// este archivo) porque no puedo usar clases para encapsularla.
static unsigned int estado = 2463534242u;

void sembrarAleatorio(unsigned int semilla)
{
    if (semilla == 0u) semilla = 1u;   // ojo: si arranco en cero, el xorshift se queda pegado en cero para siempre
    estado = semilla;
}

// Esta funcion da un paso del generador xorshift32. Es pura magia de bits: solo
// desplazo y hago XOR, que son justo los operadores que uso en todo el resto
// del programa. Por eso me gusto este generador para este trabajo.
static unsigned int siguiente()
{
    unsigned int x = estado;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    estado = x;
    return x;
}

int aleatorioEnRango(int n)
{
    if (n <= 0) return 0;

    unsigned int rango = (unsigned int) n;
    // Aqui viene el detalle fino. Si tomara el numero crudo y le sacara el modulo
    // n, los primeros valores saldrian un poquito mas seguido (eso es el sesgo).
    // Para evitarlo calculo un umbral (2^32 mod n). El truco (0u - rango) me da
    // 2^32 - n sin que se desborde.
    unsigned int umbral = ((unsigned int)(0u - rango)) % rango;

    unsigned int r;
    do {
        r = siguiente();
    } while (r < umbral);           // si cae en la zona que sesga, lo tiro y pido otro

    return (int)(r % rango);
}
