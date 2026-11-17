#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   ¿Qué entra?    -> char *linea (Cadena de texto con una línea del archivo)
   ¿Qué hace?     -> Divide la línea en tokens separados por espacios o saltos de línea y los imprime.
   ¿Qué retorna?  -> void (No retorna ningún valor)
*/
void separar_tokens(char *linea) {
    char *token = strtok(linea, " \t\n\r");
    while (token != NULL) {
        printf("Token: %s\n", token);
        token = strtok(NULL, " \t\n\r");
    }
}
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
        separar_tokens(buffer);
    }

    fclose(f);
    return 0;
}
