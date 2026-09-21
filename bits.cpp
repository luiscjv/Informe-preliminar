#include "bits.h"

int leer3(const unsigned char* b, long indice)
{
    long bit = 3 * indice;
    long by  = bit >> 3;          // en que byte empieza (es bit / 8, pero con un shift)
    int  off = (int)(bit & 7);    // en que posicion dentro del byte (es bit % 8)

    if (off <= 5) {
        // Caso facil: los 3 bits caben enteros en este byte, los saco y listo.
        return (b[by] >> off) & 7;
    }
    // Caso feo: la ficha se parte. La parte baja esta en b[by] y lo que sobra en
    // b[by+1]. Aqui los pego juntando los dos bytes.
    int valor = (b[by] >> off) | (b[by + 1] << (8 - off));
    return valor & 7;
}

void escribir3(unsigned char* b, long indice, int valor)
{
    valor &= 7;
    long bit = 3 * indice;
    long by  = bit >> 3;
    int  off = (int)(bit & 7);

    if (off <= 5) {
        // Facil otra vez: hago un hueco de 3 bits en su sitio y meto el valor ahi.
        b[by] = (unsigned char)((b[by] & ~(7 << off)) | (valor << off));
        return;
    }

    // Cruce de frontera. Reparto los 3 bits: 'enPrimero' se van a b[by] y el
    // resto a b[by+1]. Escribo cada byte con cuidado de no pisar los bits vecinos.
    int enPrimero = 8 - off;                 // van 1 o 2 bits al primer byte
    int restantes = 3 - enPrimero;           // y los otros 2 o 1 al siguiente

    unsigned char mascaraAlta = (unsigned char)(~(((1 << enPrimero) - 1) << off));
    b[by] = (unsigned char)((b[by] & mascaraAlta) | (valor << off));

    unsigned char mascaraBaja = (unsigned char)(~((1 << restantes) - 1));
    b[by + 1] = (unsigned char)((b[by + 1] & mascaraBaja) | (valor >> enPrimero));
}
