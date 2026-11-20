#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   ¿Qué entra?    -> char *token (Palabra extraída de la línea de código)
   ¿Qué hace?     -> Compara el token contra un arreglo de palabras reservadas del lenguaje C.
   ¿Qué retorna?  -> int (1 si es palabra reservada, 0 si no lo es)
*/
int es_reservada(char *token) {
    const char *reservadas[] = {"int", "float", "char", "void", "if", "else", "while", "for", "return"};
    int total = 9;
    
    for (int i = 0; i < total; i++) {
        if (strcmp(token, reservadas[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
   ¿Qué entra?    -> char *token (Cadena extraída de la línea de código)
   ¿Qué hace?     -> Verifica si todos los caracteres del token son dígitos numéricos (o un punto decimal).
   ¿Qué retorna?  -> int (1 si es un número válido, 0 si no lo es)
*/
int es_numero(char *token) {
    int puntos = 0;
    for (int i = 0; token[i] != '\0'; i++) {
        if (token[i] == '.') {
            puntos++;
            if (puntos > 1) return 0;
        } else if (token[i] < '0' || token[i] > '9') {
            return 0;
        }
    }
    return 1;
}

/*
   ¿Qué entra?    -> char *linea (Cadena de texto con una línea del archivo)
   ¿Qué hace?     -> Divide la línea en tokens y los clasifica en palabras reservadas, números o tokens generales.
   ¿Qué retorna?  -> void (No retorna ningún valor)
*/
void separar_tokens(char *linea) {
    char *token = strtok(linea, " \t\n\r");
    while (token != NULL) {
        if (es_reservada(token)) {
            printf("Palabra reservada: %s\n", token);
        } else if (es_numero(token)) {
            printf("Numero: %s\n", token);
        } else {
            printf("Token: %s\n", token);
        }
        token = strtok(NULL, " \t\n\r");
    }
}

/*
   ¿Qué entra?    -> void (Ningún parámetro inicial)
   ¿Qué hace?     -> Abre el archivo codigo.txt y manda cada línea a la función separar_tokens.
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
