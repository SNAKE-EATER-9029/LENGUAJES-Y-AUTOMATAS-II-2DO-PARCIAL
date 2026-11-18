/*

 
 
 Criterios de diseño aplicados:
   1. Función pequeña (Modularización y claridad).
   2. Realiza 1 funcionalidad específica por cada bloque/función.
   3. Abstracción (Uso de tipos abstractos, estructuras y pilas).

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definición de estados del Autómata Finito Determinista (AFD)
typedef enum {
  q_tipo,     // Espera tipo de dato O inicio de expresión directa
  q_var,      // Espera el nombre de la variable
  q_asign,    // Espera el operador '='
  q0,         // Inicio de la expresión matemática tras '='
  q_num,      // Leyendo parte entera de un número
  q_dec,      // Leyendo parte decimal de un número
  q_op,       // Espera operando después de +, -, *, /
  q_pcoma,    // Espera el fin de instrucción ';' (opcional si es expresión simple)
  q_palabra,  // Espera la palabra reservada en inglés al final
  q_error     // Estado de error sintáctico
} Status;

// Tipos de datos permitidos
const char *tipos_datos[] = {
  "int", "float", "char", "double", "bool", "long", "short"
};
const int num_tipos = 7;

// Palabras reservadas en inglés
const char *palabras_reservadas[] = {
  "if", "else", "while", "for", "return", "print", "fn", "end"
};
const int num_reservadas = 8;

/*
   ¿Qué entra?    -> const char *word Cadena de texto con la palabra a evaluar
   ¿Qué hace?     -> Compara la palabra contra el arreglo estático de tipos de datos válidos en C.
   ¿Qué retorna?  -> int 1 si es un tipo de dato válido, 0 sino lo es 
*/
int es_tipo_dato(const char *word) {
  for (int j = 0; j < num_tipos; j++) {
    if (strcmp(word, tipos_datos[j]) == 0) return 1;
  }
  return 0;
}

/*
   ¿Qué entra?    -> const char *word Cadena de texto con la palabra a evaluar 
   ¿Qué hace?     -> Compara la palabra contra el catálogo de palabras reservadas permitidas.
   ¿Qué retorna?  -> int 1 si es palabra reservada, 0 si no lo es
*/
int es_palabra_reservada(const char *word) {
  for (int j = 0; j < num_reservadas; j++) {
    if (strcmp(word, palabras_reservadas[j]) == 0) return 1;
  }
  return 0;
}

/*
   ¿Qué entra?    -> char op (El operador aritmético +, -, *, /)
   ¿Qué hace?     -> Define la jerarquía o precedencia de los operadores matemáticos.
   ¿Qué retorna?  -> int (2 para alta prioridad * y /, 1 para baja prioridad + y -, 0 por defecto)
*/
int prioridad(char op) {
  if (op == '+' || op == '-') return 1;
  if (op == '*' || op == '/') return 2;
  return 0;
}

// Estructura de Pila para el manejo de cadenas de tokens
typedef struct {
  char items[100][50];
  int tope;
} PilaStr;

/*
   ¿Qué entra?    -> PilaStr *p (Puntero a la estructura de pila), const char *val (Valor a insertar)
   ¿Qué hace?     -> Incrementa el tope de la pila y copia el valor en la cima.
   ¿Qué retorna?  -> void (No retorna valor, modifica la estructura por referencia)
*/
void push(PilaStr *p, const char *val) {
  p->tope++;
  strcpy(p->items[p->tope], val);
}

/*
   ¿Qué entra?    -> PilaStr *p (Puntero a la pila), char *out (Buffer de salida para el elemento extraído)
   ¿Qué hace?     -> Extrae el elemento de la cima de la pila y decrementa el tope.
   ¿Qué retorna?  -> void (Modifica el buffer 'out' y la pila por referencia)
*/
void pop(PilaStr *p, char *out) {
  if (p->tope >= 0) {
    strcpy(out, p->items[p->tope]);
    p->tope--;
  }
}

/*
   ¿Qué entra?    -> char tokens[][50] (Arreglo de tokens), int num_tokens (Cantidad), char *expPOS (Buffer de salida)
   ¿Qué hace?     -> Aplica el algoritmo basado en pilas de izquierda a derecha para convertir una expresión infija a postfija.
   ¿Qué retorna?  -> void (Modifica directamente la cadena 'expPOS' con el resultado)
*/
void infija_a_postfija(char tokens[][50], int num_tokens, char *expPOS) {
  PilaStr pila;
  pila.tope = -1;
  expPOS[0] = '\0';

  // Recorrer de izquierda a derecha
  for (int k = 0; k < num_tokens; k++) {
    char *tok = tokens[k];

    if (strcmp(tok, "(") == 0) {
      push(&pila, tok);
    } 
    else if (strcmp(tok, ")") == 0) {
      char elem[50];
      while (pila.tope >= 0 && strcmp(pila.items[pila.tope], "(") != 0) {
        pop(&pila, elem);
        strcat(expPOS, elem);
        strcat(expPOS, " ");
      }
      if (pila.tope >= 0) {
        pop(&pila, elem); // Saca el '('
      }
    } 
    else if (strchr("+-*/", tok[0]) && strlen(tok) == 1) {
      char elem[50];
      while (pila.tope >= 0 && strcmp(pila.items[pila.tope], "(") != 0 &&
             prioridad(tok[0]) <= prioridad(pila.items[pila.tope][0])) {
        pop(&pila, elem);
        strcat(expPOS, elem);
        strcat(expPOS, " ");
      }
      push(&pila, tok);
    } 
    else { // Operando (número o variable)
      strcat(expPOS, tok);
      strcat(expPOS, " ");
    }
  }

  // Vaciar la pila restante al final
  while (pila.tope >= 0) {
    char elem[50];
    pop(&pila, elem);
    strcat(expPOS, elem);
    strcat(expPOS, " ");
  }
}

/*
   ¿Qué entra?    -> const char *texto (Línea de texto leída desde el archivo)
   ¿Qué hace?     -> Ejecuta el autómata de estados para validar la sintaxis de la línea y coordinar la conversión a postfija.
   ¿Qué retorna?  -> void (Imprime directamente los resultados y validaciones en la consola)
*/
void evaluar_linea(const char *texto) {
  int i = 0;
  Status estado = q_tipo;
  int parentesis = 0;
  int tiene_asignacion = 0;

  char tipo_detectado[20] = "";
  char variable_detectada[30] = "";
  char palabra_res[30] = "";

  int idx_temp = 0;
  char temp[50] = "";

  char tokens_exp[50][50];
  int num_tokens = 0;
  char num_buffer[50] = "";
  int idx_num = 0;

  while (texto[i] != '\0') {
    char c = texto[i];

    if (c == '\n' || c == '\r') break;

    if (c == ' ' && estado != q_tipo && estado != q_var && estado != q_palabra) {
      i++;
      continue;
    }

    switch (estado) {
      case q_tipo:
        if ((c >= '0' && c <= '9') || c == '(' || c == '-') {
          estado = q0;
          continue; 
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
          temp[idx_temp++] = c;
        } else if (c == ' ') {
          temp[idx_temp] = '\0';
          if (es_tipo_dato(temp)) {
            strcpy(tipo_detectado, temp);
            estado = q_var;
            idx_temp = 0;
            tiene_asignacion = 1;
          } else {
            estado = q_error;
          }
        } else {
          estado = q_error;
        }
        break;

      case q_var:
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
          temp[idx_temp++] = c;
        } else if (c == ' ' || c == '=') {
          temp[idx_temp] = '\0';
          if (strlen(temp) > 0) {
            strcpy(variable_detectada, temp);
            idx_temp = 0;
            if (c == '=') estado = q0;
            else estado = q_asign;
          } else if (c == '=') {
            estado = q0;
          }
        } else {
          estado = q_error;
        }
        break;

      case q_asign:
        if (c == '=') {
          estado = q0;
        } else if (c != ' ') {
          estado = q_error;
        }
        break;

      case q0:
        if (c >= '0' && c <= '9') {
          estado = q_num;
          num_buffer[idx_num++] = c;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
          strcpy(tokens_exp[num_tokens++], "(");
        } else if (c == '-') {
          estado = q_op;
          strcpy(tokens_exp[num_tokens++], "-");
        } else {
          estado = q_error;
        }
        break;

      case q_num:
        if (c >= '0' && c <= '9') {
          estado = q_num;
          num_buffer[idx_num++] = c;
        } else if (c == '.') {
          estado = q_dec;
          num_buffer[idx_num++] = c;
        } else {
          if (idx_num > 0) {
            num_buffer[idx_num] = '\0';
            strcpy(tokens_exp[num_tokens++], num_buffer);
            idx_num = 0;
          }

          if (c == '+' || c == '-' || c == '*' || c == '/') {
            estado = q_op;
            char op_str[2] = {c, '\0'};
            strcpy(tokens_exp[num_tokens++], op_str);
          } else if (c == ')') {
            parentesis--;
            strcpy(tokens_exp[num_tokens++], ")");
            if (parentesis < 0) estado = q_error;
          } else if (c == ';') {
            if (parentesis == 0) estado = q_pcoma;
            else estado = q_error;
          } else {
            estado = q_error;
          }
        }
        break;

      case q_dec:
        if (c >= '0' && c <= '9') {
          estado = q_dec;
          num_buffer[idx_num++] = c;
        } else {
          if (idx_num > 0) {
            num_buffer[idx_num] = '\0';
            strcpy(tokens_exp[num_tokens++], num_buffer);
            idx_num = 0;
          }

          if (c == '+' || c == '-' || c == '*' || c == '/') {
            estado = q_op;
            char op_str[2] = {c, '\0'};
            strcpy(tokens_exp[num_tokens++], op_str);
          } else if (c == ')') {
            parentesis--;
            strcpy(tokens_exp[num_tokens++], ")");
            if (parentesis < 0) estado = q_error;
          } else if (c == ';') {
            if (parentesis == 0) estado = q_pcoma;
            else estado = q_error;
          } else {
            estado = q_error;
          }
        }
        break;

      case q_op:
        if (c >= '0' && c <= '9') {
          estado = q_num;
          num_buffer[idx_num++] = c;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
          strcpy(tokens_exp[num_tokens++], "(");
        } else {
          estado = q_error;
        }
        break;

      case q_pcoma:
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
          estado = q_palabra;
          idx_temp = 0;
          temp[idx_temp++] = c;
        } else if (c != ' ') {
          estado = q_error;
        }
        break;

      case q_palabra:
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
          temp[idx_temp++] = c;
        } else if (c == ' ') {
          temp[idx_temp] = '\0';
          break;
        } else {
          estado = q_error;
        }
        break;

      case q_error:
        break;
    }

    i++;
  }

  if (idx_num > 0) {
    num_buffer[idx_num] = '\0';
    strcpy(tokens_exp[num_tokens++], num_buffer);
  }

  temp[idx_temp] = '\0';
  if (estado == q_palabra) {
    strcpy(palabra_res, temp);
  }

  if (((estado == q_pcoma || estado == q_palabra) || (!tiene_asignacion && (estado == q_num || estado == q_dec))) && parentesis == 0) {
    printf("-> Expresion SINTACTICAMENTE CORRECTA\n");

    if (tiene_asignacion) {
      printf("   - Tipo de dato:       %s\n", tipo_detectado);
      printf("   - Variable:           %s\n", variable_detectada);
      printf("   - Asignacion ('='):   SI\n");
      printf("   - Fin de linea (';'): SI\n");
    } else {
      printf("   - Modo:               Expresion Aritmetica Simple\n");
    }

    if (estado == q_palabra && es_palabra_reservada(palabra_res)) {
      printf("   - Palabra reservada:  '%s'\n", palabra_res);
    } else if (estado == q_palabra) {
      printf("   - Identificador:      '%s' (No es palabra reservada)\n", palabra_res);
    }

    // CONVERSIÓN A NOTACIÓN POSTFIJA
    char expPOS[200] = "";
    infija_a_postfija(tokens_exp, num_tokens, expPOS);

    printf("   - Notacion Infija:   ");
    for (int k = 0; k < num_tokens; k++) printf("%s ", tokens_exp[k]);
    printf("\n");
    printf("   - Notacion Postfija: %s\n", expPOS);

  } else {
    printf("-> Expresion INCORRECTA (Error sintactico o parentesis desbalanceados)\n");
  }
}

/*
   ¿Qué entra?    -> void (Ningún parámetro por defecto en línea de comandos)
   ¿Qué hace?     -> Abre el archivo 'expresiones.txt', lee línea por línea su contenido y coordina el análisis global.
   ¿Qué retorna?  -> int (0 si finaliza con éxito, 1 si ocurre un error al abrir el archivo)
*/
int main() {
  FILE *archivo;
  char buffer[256];

  archivo = fopen("expresiones.txt", "r");

  if (archivo == NULL) {
    printf("Error al abrir el archivo.\n");
    return 1;
  }

  printf("=== ANALIZADOR Y CONVERTIDOR INFIJA -> POSTFIJA ===\n\n");

  while (fgets(buffer, sizeof(buffer), archivo) != NULL) {
    buffer[strcspn(buffer, "\n")] = 0;

    printf("Linea: \"%s\"\n", buffer);
    evaluar_linea(buffer);
    printf("--------------------------------------------------\n");
  }

  fclose(archivo);
  return 0;
}