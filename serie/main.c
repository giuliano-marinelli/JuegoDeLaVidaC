//autores: Bermudez Martin, Marinelli Giuliano
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*macro que imprime la matriz del mundo*/
#define IMPMUNDO(matriz) ({\
    for (i = 1; i < rowsM1; ++i) {\
        for (j = 1; j < colsM1; ++j) {\
            if (matriz[i][j] == 0) {\
                printf(".");\
            } else {\
                printf("O");\
            }\
        }\
        printf("\n");\
    }\
})

/*macro que imprime la matriz completa*/
#define IMPMUNDOCOMP(matriz) ({\
    for (i = 0; i < rowsM2; i++) {\
        for (j = 0; j < colsM2; j++) {\
            printf("%i", matriz[i][j]);\
        }\
        printf("\n");\
    }\
})

int main(int argc, char *argv[]) {
    FILE *f;
    char *s, *res;
    int rows, cols, steps, n, i, j, k, p, colsM2, rowsM2, colsM1, rowsM1;

    /*se definen los espacios de memoria donde se almacenaran las matrices*/
    char **mundo;
    char **mundoAux;
    char **aux;

    /*controla que se haya ingresado un argumento en la llamada*/
    if (argc != 2) {
        perror("Error: Debe indicar el nombre del archivo de entrada\nEj: $./main.o entrada.cells\n");
        exit(1);
    }

    /*lectura del encabezado del archivo que contiene el patron de celdas*/
    f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("Error al intentar abrir el archivo.\n");
        exit(1);
    }

    n = fscanf(f, "cols %d\nrows %d\nsteps %d\n", &cols, &rows, &steps);

    if (n != 3) {
        perror("Error: formato de archivo incorrecto\n");
        exit(1);
    }

    printf("cols %d\nrows %d\nsteps %d\n", cols, rows, steps);
    rowsM2 = rows + 2;
    colsM2 = cols + 2;

    rowsM1 = rows + 1;
    colsM1 = cols + 1;

    mundo = malloc((rowsM2) * sizeof (char *));
    mundoAux = malloc((rowsM2) * sizeof (char *));

    for (i = 0; i < rowsM2; i++) {
        mundo[i] = malloc((colsM2) * sizeof (char));
        mundoAux[i] = malloc((colsM2) * sizeof (char));
    }

    /*inicializar elementos con 0 o 1 en matriz llamada "mundo"*/
    s = (char *) malloc((colsM1) * sizeof (char));
    if (s == NULL) {
        perror("Memoria llena.\n");
        exit(1);
    }
    res = fgets(s, colsM2, f);
    i = 1;

    int strlenAux = strlen(s) - 1;
    while (res != NULL && i < rowsM1) {
        //printf("Linea %i: %s => strlen %i (incluye salto de linea)\n", i, s, strlen(s));
        for (j = 0; j < strlenAux; j++) {
            mundo[i][j + 1] = (s[j] == '.') ? 0 : 1;
        }

        /* IMPORTANTE
         para que funcione correctamente el archivo de entrada debe tener un 
         salto de linea en la ultima fila para la matriz, dejando una linea en blanco.*/

        for (j = strlenAux; j < cols; j++) {
            mundo[i][j + 1] = 0;
        }
        res = fgets(s, colsM2, f);
        strlenAux = strlen(s) - 1;
        i++;
    }

    /*llenar con ceros las filas que falten*/
    for (j = i; j < rowsM1; j++) {
        memset(mundo[j], 0, colsM2);
    }
    int iAux1Men;
    int iAux1Mas;
    int jAux1Men;
    int jAux1Mas;
    int iteracion = 0;
    short cantVivas = 0;

    //repetir cantidad de pasos
    do {

        /*filas y columnas de mas para hacer el mundo rendondo*/
        //primera fila (por referencia, luego las esquinas se sobreescrib       en).
        mundo[0] = mundo[rows];
        //ultima fila (por referencia, luego las esquinas se sobreescriben).
        mundo[rowsM1] = mundo[1];
        //esquina sup-izq
        mundo[0][0] = mundo[rows][cols];
        //esquina sup-der
        mundo[0][colsM1] = mundo[rows][1];
        //esquina inf-izq
        mundo[rowsM1][0] = mundo[1][cols];
        //esquina inf-der
        mundo[rowsM1][colsM1] = mundo[1][1];
        //primer y ultima columna (sin considerar las esquinas).
        for (i = 1; i < rowsM1; i++) {
            mundo[i][0] = mundo[i][cols];
            mundo[i][colsM1] = mundo[i][1];
        }

        //printf("Iteracion %i mundo: \n", (iteracion + 1));
        //IMPMUNDO(mundo);

        //contar cantidad de celulas vecinas vivas para cada celula
        for (i = 1; i < rowsM1; i++) {
            iAux1Men = i - 1;
            iAux1Mas = i + 1;
            for (j = 1; j < colsM1; j++) {
                jAux1Men = j - 1;
                jAux1Mas = j + 1;
                cantVivas = mundo[iAux1Men][jAux1Men] + mundo[iAux1Men][j] + mundo[iAux1Men][jAux1Mas] + mundo[i][jAux1Men] + mundo[i][jAux1Mas] + mundo[iAux1Mas][jAux1Men] + mundo[iAux1Mas][j] + mundo[iAux1Mas][jAux1Mas];
                if (mundo[i][j] == 1) {
                    mundoAux[i][j] = (cantVivas == 2 || cantVivas == 3) ? 1 : 0;
                } else {
                    mundoAux[i][j] = (cantVivas == 3) ? 1 : 0;
                }
            }
        }

        aux = mundo;
        mundo = mundoAux;
        mundoAux = aux;
        iteracion++;

    } while (iteracion < steps);

    IMPMUNDO(mundo);

    fclose(f);
    free(s);
    free(mundo);
    free(mundoAux);

    return 0;
}