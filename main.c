#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emmintrin.h>

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

    /*se definen los vectores (registros) que se utilizaran para operar la matriz*/
    __m128i vSup, vCen, vInf, vSumIni, vSumIniSftIzq, vSumIniSftDer, vSumTot,
            vSumAdy, vCenEq0, vCenEq1, vAdyEq2, vAdyEq3, vAdyEq2OrEq3, vCenEq1AndAdyEq2OrEq3,
            vCenEq0AndAdyEq3, vRes;
    /*contienen 16 bytes con el numeros 0, 1, 2 y 3 cargados respectivamente*/
    __m128i n0, n1, n2, n3;

    n0 = _mm_set1_epi8(0);
    n1 = _mm_set1_epi8(1);
    n2 = _mm_set1_epi8(2);
    n3 = _mm_set1_epi8(3);

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

    /*aqui se asigna memoria alineada a las matrices mundo y mundoAux para luego
     *  operar con mundo y guardar el resultado en mundoAux*/
    /*el tamaño en memoria es equivalente a cada elemento de la matriz, ya que
     se guardara continua en la memoria*/
    if (posix_memalign((void*) &mundo, 16, (rows + 2)*(cols + 2)) != 0) {
        perror("posix_memalign dio error para mundo");
        exit(1);
    }
    if (posix_memalign((void*) &mundoAux, 16, (rows + 2)*(cols + 2)) != 0) {
        perror("posix_memalign dio error para mundoAux");
        exit(1);
    }

    /*asi se asignaba memoria cuando no estaba alineada*/
    /*mundo = malloc((rows + 2) * sizeof (char *));
    mundoAux = malloc((rows + 2) * sizeof (char *));

    for (i = 0; i < rows + 2; i++) {
        mundo[i] = malloc((cols + 2) * sizeof (char));
        mundoAux[i] = malloc((cols + 2) * sizeof (char));
    }*/

    /*inicializar elementos con 0 o 1 en matriz llamada "mundo"*/
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
            mundo[i][j + 1] = (s[j] == '.') ? '0' : '1';
            //printf("ENTRO JOTA %i\n",j );
        }

        /* IMPORTANTE
         para que funcione correctamente el archivo de entrada debe tener un 
         salto de linea en la ultima fila para la matriz, dejando una linea en blanco.*/

        for (j = strlen(s) - 1; j < cols; j++) {
            mundo[i][j + 1] = '0';
            //printf("22 ENTRO JOTA %i en I %i\n",j,i );
        }
        res = fgets(s, cols + 2, f);
        i++;
    };

    /*llenar con ceros las filas que falten*/
    for (j = i; j < rows + 1; j++) {
        memset(mundo[j], '0', cols + 2);
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

        printf("Iteracion %i mundo: \n", (iteracion + 1));
        IMPMUNDO(mundo);


        for (i = 0; i < rows; i++) {
            for (j = 0; j <= (cols / 14); j++) {

                /*cargo los vectores para iterar 14 celulas*/
                vSup = _mm_load_si128((__m128i*) (mundo + (i * (cols + 2))+(j * 14)));
                vCen = _mm_load_si128((__m128i*) (mundo + ((i + 1)*(cols + 2))+(j * 14)));
                vInf = _mm_load_si128((__m128i*) (mundo + ((i + 2)*(cols + 2))+(j * 14)));

                /*sumo por cada columna*/
                vSumIni = _mm_add_epi8(vSup, vCen);
                vSumIni = _mm_add_epi8(vSumIni, vInf);

                /*obtengo dos vectores en donde muevo los operandos a la 
                 * izquierda y a la derecha respectivamente.
                 Estos permitiran sumar las adyacentes de una celula incluyendo
                 a la misma*/
                vSumIniSftIzq = _mm_slli_si128(vSumIni, 1);
                vSumIniSftDer = _mm_srli_si128(vSumIni, 1);

                /*sumo los vectores SumIni*/
                vSumTot = _mm_add_epi8(vSumIni, vSumIniSftIzq);
                vSumTot = _mm_add_epi8(vSumTot, vSumIniSftDer);

                /*resto la celula seleccionada para tener solo la suma de los
                 adyacentes*/
                vSumAdy = _mm_sub_epi8(vSumTot, vCen);

                /*comparo para saber si la celula seleccionada esta viva*/
                vCenEq1 = _mm_cmpeq_epi8(vCen, n1);

                /*comparo para saber si la celula seleccionada esta muerta*/
                vCenEq0 = _mm_cmpeq_epi8(vCen, n0);

                /*comparo para saber si la suma de los adyacentes es igual a 2*/
                vAdyEq2 = _mm_cmpeq_epi8(vSumAdy, n2);
                /*comparo para saber si la suma de los adyacentes es igual a 3*/
                vAdyEq3 = _mm_cmpeq_epi8(vSumAdy, n3);

                /*comparo para saber si la suma de los adyacentes es 2 o 3*/
                vAdyEq2OrEq3 = _mm_or_si128(vAdyEq2, vAdyEq3);

                /*compara para saber si siendo que la celula seleccionada esta viva,
                 la suma de adyacentes es 2 o 3*/
                vCenEq1AndAdyEq2OrEq3 = _mm_and_si128(vCenEq1, vAdyEq2OrEq3);

                /*compara para saber si siendo que la celula seleccionada esta muerta,
                 la suma de adyacentes es 3*/
                vCenEq0AndAdyEq3 = _mm_and_si128(vCenEq0, vAdyEq3);

                /*comparo para obtener finalmente si la celula queda viva o muerta*/
                vRes = _mm_or_si128(vCenEq0AndAdyEq3, vCenEq1AndAdyEq2OrEq3);

                /*el vRes quedara con un resultado booleano es decir con
                 FF(un byte lleno de 1) en los datos true.
                 Paso los FF a numeros 1.*/
                vRes = _mm_and_si128(vRes, n1);

                /*guardo en la matrizAux el nuevo estado de las 14 celulas analisadas*/
                _mm_store_si128((__m128i*) (mundoAux + ((i + 1)*(cols + 2))+(j * 14)), vRes);

                /*esto es lo que se hacia antes*/
                /*cantVivas = 0;
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
                 */
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
