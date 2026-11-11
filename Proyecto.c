#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   ¿Qué entra?    -> void (Ningún parámetro inicial)
   ¿Qué hace?     -> Abre el archivo codigo.txt y lee cada línea para mostrarla en pantalla.
   ¿Qué retorna?  -> int (0 al terminar con éxito, 1 si hay error de apertura)
*/
int main() {
    FILE *f = fopen("codigo.txt", "r");
    char buffer[100];

    if (!f) return 1;

    while (fgets(buffer, sizeof(buffer), f)) {
        printf("%s", buffer);
    }

    fclose(f);
    return 0;
}
