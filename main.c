//autores: Bermudez Martin, Marinelli Giuliano
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emmintrin.h>

/*macro que imprime la matriz del mundo*/
#define IMPMUNDO(matriz) ({\
    for (i = 1; i < rows + 1; ++i) {\
        for (j = 1; j < cols + 1; ++j) {\
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
    for (i = 0; i < rows + 2; i++) {\
        for (j = 0; j < cols + 2; j++) {\
            printf("%i", matriz[i][j]);\
        }\
        printf("\n");\
    }\
})

int main(int argc, char *argv[]) {
    time_t secondsIni;
    time_t secondsFin;
    secondsIni = time(NULL);
    //serial(argc, argv);
    //vectorial(argc, argv);
    secondsFin = time(NULL);
    printf("\nTiempo: %ld\n", (secondsFin - secondsIni));
    return 0;
}

void serial(int argc, char *argv[]) {
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

    mundo = malloc((rows + 2) * sizeof (char *));
    mundoAux = malloc((rows + 2) * sizeof (char *));

    for (i = 0; i < rows + 2; i++) {
        mundo[i] = malloc((cols + 2) * sizeof (char));
        mundoAux[i] = malloc((cols + 2) * sizeof (char));
    }

    /*inicializar elementos con 0 o 1 en matriz llamada "mundo"*/
    s = (char *) malloc((cols + 1) * sizeof (char));
    if (s == NULL) {
        printf("Memoria llena.\n");
        return 1;
    }
    res = fgets(s, cols + 2, f);
    i = 1;
    printf("\n");
    while (res != NULL && i < rows + 1) {
        //printf("Linea %i: %s => strlen %i (incluye salto de linea)\n", i, s, strlen(s));
        for (j = 0; j < strlen(s) - 1; j++) {
            mundo[i][j + 1] = (s[j] == '.') ? 0 : 1;
        }

        /* IMPORTANTE
         para que funcione correctamente el archivo de entrada debe tener un 
         salto de linea en la ultima fila para la matriz, dejando una linea en blanco.*/

        for (j = strlen(s) - 1; j < cols; j++) {
            mundo[i][j + 1] = 0;
        }
        res = fgets(s, cols + 2, f);
        i++;
    }

    /*llenar con ceros las filas que falten*/
    for (j = i; j < rows + 1; j++) {
        memset(mundo[j], 0, cols + 2);
    }

    int iteracion = 0;
    short cantVivas = 0;
    //repetir cantidad de pasos
    do {

        /*filas y columnas de mas para hacer el mundo rendondo*/
        //primera fila (por referencia, luego las esquinas se sobreescrib       en).
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

        //printf("Iteracion %i mundo: \n", (iteracion + 1));
        //IMPMUNDO(mundo);

        //contar cantidad de celulas vecinas vivas para cada celula
        for (i = 1; i < rows + 1; i++) {
            for (j = 1; j < cols + 1; j++) {
                cantVivas = mundo[i - 1][j - 1] + mundo[i - 1][j] + mundo[i - 1][j + 1] + mundo[i][j - 1] + mundo[i][j + 1] + mundo[i + 1][j - 1] + mundo[i + 1][j] + mundo[i + 1][j + 1];
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

    printf("Estado final: \n");
    IMPMUNDO(mundo);

    fclose(f);
    free(s);
    free(mundo);
    free(mundoAux);
}

void vectorial(int argc, char *argv[]) {
    FILE *f;
    char *s, *res;
    int rows, cols, steps, n, i, j, k, p;

    /*se definen los espacios de memoria donde se almacenaran las matrices*/
    char **mundo;
    char **mundoAux;
    char **aux;

    /*se definen los vectores (registros) que se utilizaran para operar la matriz*/
    __m128i vSup, vCen, vInf, vSumIni, vSumIniSftIzq, vSumIniSftDer, vSumTot,
            vAdyEq4, vAdyEq3, vRes;
    /*contienen 16 bytes con el numeros 0, 1, 2 y 3 cargados respectivamente*/
    __m128i n0, n1, n3, n4;

    n0 = _mm_set1_epi8(0);
    n1 = _mm_set1_epi8(1);
    n3 = _mm_set1_epi8(3);
    n4 = _mm_set1_epi8(4);

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

    posix_memalign((void*) &mundo, 16, (rows + 2) * sizeof (char *));
    posix_memalign((void*) &mundoAux, 16, (rows + 2) * sizeof (char *));

    for (i = 0; i < rows + 2; i++) {
        posix_memalign((void*) &mundo[i], 16, (cols + 2 + (14 - (cols % 14))) * sizeof (char));
        posix_memalign((void*) &mundoAux[i], 16, (cols + 2 + (14 - (cols % 14))) * sizeof (char));
    }

    /*inicializar elementos con 0 o 1 en matriz llamada "mundo"*/
    s = (char *) malloc((cols + 1) * sizeof (char));
    if (s == NULL) {
        printf("Memoria llena.\n");
        return 1;
    }
    res = fgets(s, cols + 2, f);
    i = 1;
    printf("\n");
    while (res != NULL && i < rows + 1) {
        //printf("Linea %i: %s => strlen %i (incluye salto de linea)\n", i, s, strlen(s));
        for (j = 0; j < strlen(s) - 1; j++) {
            mundo[i][j + 1] = (s[j] == '.') ? 0 : 1;
        }

        /* IMPORTANTE
         para que funcione correctamente el archivo de entrada debe tener un 
         salto de linea en la ultima fila para la matriz, dejando una linea en blanco.*/

        for (j = strlen(s) - 1; j < cols; j++) {
            mundo[i][j + 1] = 0;
        }
        res = fgets(s, cols + 2, f);
        i++;
    }

    /*llenar con ceros las filas que falten*/
    for (j = i; j < rows + 1; j++) {
        memset(mundo[j], 0, cols + 2);
    }

    int iteracion = 0;
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

        //printf("Iteracion %i mundo: \n", (iteracion));
        //IMPMUNDO(mundo);

        for (i = 0; i < rows; i++) {
            for (j = 0; j <= (cols / 14); j++) {

                /*cargo los vectores para iterar 14 celulas*/
                vSup = _mm_loadu_si128((__m128i*) (mundo[i] + (j * 14)));
                vCen = _mm_loadu_si128((__m128i*) (mundo[i + 1] + (j * 14)));
                vInf = _mm_loadu_si128((__m128i*) (mundo[i + 2] + (j * 14)));

                /*sumo por cada columna*/
                vSumIni = _mm_add_epi8(vSup, vCen);
                vSumIni = _mm_add_epi8(vSumIni, vInf);

                /*obtengo dos vectores en donde muevo los operandos a la 
                 izquierda y a la derecha respectivamente.
                 Estos permitiran sumar las adyacentes de una celula incluyendo
                 a la misma*/
                vSumIniSftIzq = _mm_slli_si128(vSumIni, 1);
                vSumIniSftDer = _mm_srli_si128(vSumIni, 1);

                /*sumo los vectores SumIni*/
                vSumTot = _mm_add_epi8(vSumIni, vSumIniSftIzq);
                vSumTot = _mm_add_epi8(vSumTot, vSumIniSftDer);

                /*comparo para saber si la suma de los adyacentes es igual a 3*/
                vAdyEq3 = _mm_cmpeq_epi8(vSumTot, n3);

                /*como vAdyEq3 queda con FF en lugar de 1, pasar cada FF a un 1.*/
                vAdyEq3 = _mm_and_si128(vAdyEq3, n1);

                /*hasta ahora tenemos vAdyEq3 con las celulas que tienen que 
                 * estar vivas si tenian 3 adyacentes. (3 si estaba muerta o 2 si 
                 * estaba viva) queda por verificar que si estaba viva siga 
                 * estando viva con 4 adyacentes (3 mas ella misma).*/
                vAdyEq4 = _mm_cmpeq_epi8(vSumTot, n4);

                /*luego hacemos un and entre el vector de las celulas y vAdyEq4 
                 * para que queden vivas las que estaban vivas.*/
                vAdyEq4 = _mm_and_si128(vCen, vAdyEq4);

                /*unificamos ambos resultados vAdyEq3 y vAdyEq4.*/
                vRes = _mm_or_si128(vAdyEq4, vAdyEq3);

                /*luego correr el resultado hacia la derecha para sobreescribir 
                 * el resultado de la celda final de la iteracion anterior la 
                 * cual no habia tenido en cuenta todas las vecinas*/
                vRes = _mm_srli_si128(vRes, 1);

                /*guardo en la matrizAux el nuevo estado de las 14 celulas analisadas*/
                _mm_storeu_si128((__m128i*) (mundoAux[i + 1] + (j * 14) + 1), vRes);

            }
        }

        aux = mundo;
        mundo = mundoAux;
        mundoAux = aux;
        iteracion++;

    } while (iteracion < steps);

    printf("Estado final: \n");
    IMPMUNDO(mundo);

    fclose(f);
    free(s);
    free(mundo);
    free(mundoAux);
}