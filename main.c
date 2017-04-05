#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*macro que imprime la matriz del mundo*/
#define IMPMUNDO(matriz) ({\
    for (i = 1; i < rows + 1; ++i) {\
        for (j = 1; j < cols + 1; ++j) {\
            printf("%c", matriz[i][j]);\
        }\
        printf("\n");\
    }\
})

/*macro que imprime la matriz completa*/
#define IMPMUNDOCOMP(matriz) ({\
    for (i = 0; i < rows + 2; i++) {\
        for (j = 0; j < cols + 2; j++) {\
            printf("%c", matriz[i][j]);\
        }\
        printf("\n");\
    }\
})

int main(int argc, char *argv[]) {
    FILE *f;
    char *s, *res;
    int rows, cols, steps, n, i, j, k, p;

    /*se definen los espacios de memoria donde se almacenaran las matrices*/
    char **mundo;
    char **mundoAux;
    char **aux;

    /*controla que se haya ingresado un argumento en la llamada*/

    if (argc != 2) {
        printf("Error: Debe indicar el nombre del archivo de entrada\nEj: $./main.o entrada.cells\n");
        return 1;
    }

    /*lectura del encabezado del archivo que contiene el patron de celdas*/

    f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error al intentar abrir el archivo.\n");
        return 1;
    }
    n = fscanf(f, "cols %d\nrows %d\nsteps %d\n", &cols, &rows, &steps);
    if (n != 3) {
        printf("Error: formato de archivo incorrecto\n");
        return 1;
    }
    printf("cols %d\nrows %d\nsteps %d\n", cols, rows, steps);
    /*aqui se asigna memoria a las matrices*/
    mundo = malloc((rows + 2) * sizeof (char *));
    mundoAux = malloc((rows + 2) * sizeof (char *));

    for (i = 0; i < rows + 2; i++) {
        mundo[i] = malloc((cols + 2) * sizeof (char));
        mundoAux[i] = malloc((cols + 2) * sizeof (char));
    }

    /*inicializar elementos con 0 o 1 en matriz llamada "old"*/
    s = (char *) malloc(cols * sizeof (char));
    if (s == NULL) {
        printf("Memoria llena.\n");
        return 1;
    }
    res = fgets(s, cols + 2, f);
    i = 1;
    printf("\n");

    while (res != NULL && i < rows + 1) {
        // printf("Linea %i: %s => strlen %i (incluye salto de linea)\n", i, s, strlen(s));
        for (j = 0; j < strlen(s) - 1; j++) {
            mundo[i][j + 1] = (s[j] == '.') ? '.' : 'O';
            //printf("ENTRO JOTA %i\n",j );
        }

        /* IMPORTANTE
         para que funcione correctamente el archivo de entrada debe tener un 
         salto de linea en la ultima fila para la matriz, dejando una linea en blanco.*/

        for (j = strlen(s) - 1; j < cols; j++) {
            mundo[i][j + 1] = '.';
            //printf("22 ENTRO JOTA %i en I %i\n",j,i );
        }
        res = fgets(s, cols + 2, f);
        i++;
    };

    /*llenar con ceros las filas que falten*/
    for (j = i; j < rows + 1; j++) {
        memset(mundo[j], '.', cols + 2);
    }

    int iteracion = 0;
    short cantVivas = 0;
    //repetir cantidad de pasos
    do {
        /*filas y columnas de mas para hacer el mundo rendondo*/
        //primera fila (por referencia, luego las esquinas se sobreescriben).
        mundo[0] = mundo[rows];
        //ultima fila (por referencia, luego las esquinas se sobreescriben).
        mundo[rows + 1] = mundo[1];
        //esquina sup-izq
        mundo[0][0] = mundo[rows][cols];
        //esquina sup-der
        mundo[0][cols + 1] = mundo[rows][1];
        //esquina inf-izq
        mundo[rows + 1][0] = mundo[1][cols];
        //esquina inf-der
        mundo[rows + 1][cols + 1] = mundo[1][1];
        //primer y ultima columna (sin considerar las esquinas).
        for (i = 1; i < rows + 1; i++) {
            mundo[i][0] = mundo[i][cols];
            mundo[i][cols + 1] = mundo[i][1];
        }

        //        printf("Iteracion %i mundo: \n", (iteracion + 1));
        //        IMPMUNDO(mundo);

        //contar cantidad de celulas vecinas vivas para cada celula
        for (i = 1; i < rows + 1; ++i) {
            for (j = 1; j < cols + 1; ++j) {
                cantVivas = 0;
                for (k = i - 1; k <= i + 1; k++) {
                    for (p = j - 1; p <= j + 1; p++) {
                        if (mundo[k][p] == 'O') {
                            cantVivas++;
                        }
                    }
                }
                if (mundo[i][j] == 'O') {
                    cantVivas--;
                    mundoAux[i][j] = (cantVivas == 2 || cantVivas == 3) ? 'O' : '.';
                } else {
                    mundoAux[i][j] = (cantVivas == 3) ? 'O' : '.';
                }
            }
        }

        aux = mundo;
        mundo = mundoAux;
        mundoAux = aux;
        iteracion++;
    } while (iteracion < steps);

    printf("Estado final: \n");
    //    IMPMUNDO(mundo);

    /*Salida por archivo de texto*/
    FILE *fp;
    //si no existe salida.txt lo creara
    fp = fopen("salida.txt", "w+");
    char buffer[cols];
    fputs("cols ", fp);
    /*cargo un buffer de char con el valor de cols, para despues copiar el buffer 
    en el archivo de salida con fputs*/
    sprintf(buffer, "%d", cols);
    fputs(buffer, fp);
    fputs("\n", fp);
    fputs("rows ", fp);
    sprintf(buffer, "%d", rows);
    fputs(buffer, fp);
    fputs("\n", fp);
    fputs("steps ", fp);
    sprintf(buffer, "%d", steps);
    fputs(buffer, fp);
    fputs("\n", fp);

    for (i = 1; i < rows + 1; ++i) {
        fwrite(*(mundo + i), sizeof (char), cols, fp);
        fputs("\n", fp);
    }

    fclose(fp);
    fclose(f);
    free(s);
}
