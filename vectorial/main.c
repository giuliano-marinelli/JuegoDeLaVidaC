//autores: Bermudez Martin, Marinelli Giuliano
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emmintrin.h>

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
    int iAux1Mas;
    int jPor14;
    int colsDiv14 = cols / 14;
    int iteracion = 0;
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
        for (i = 0; i < rows; i++) {
            iAux1Mas = i + 1;
            for (j = 0; j <= (colsDiv14); j++) {
                jPor14 = j * 14;
                /*cargo los vectores para iterar 14 celulas*/
                vSup = _mm_loadu_si128((__m128i*) (mundo[i] + (jPor14)));
                vCen = _mm_loadu_si128((__m128i*) (mundo[iAux1Mas] + (jPor14)));
                vInf = _mm_loadu_si128((__m128i*) (mundo[i + 2] + (jPor14)));

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
                _mm_storeu_si128((__m128i*) (mundoAux[iAux1Mas] + (jPor14) + 1), vRes);

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
