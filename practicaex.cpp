/////// prodcons-alt2.cpp ////////////////////////////////////////////////
// - Se añade al problema el papel de productor de productos falsificados.
// Puede haber un número arbitrario de productores de este tipo, pero
// para el examen esta cantidad se fijará a 2. El producto falsificado
// siempre es el valor 44.
//
// - En total, los productores de productos falsificados producen la misma
// cantidad de productos falsos que los productores estándar.
//
// - Se añade al problema el papel de almacén de productos falsificados.
// Este almacén será un buffer y tendrá el mismo tamaño que el buffer del
// que ya dispones. A este almacén solo añaden productos los nuevos
// productores de productos falsificados. En el resto de los aspectos se
// comporta como el buffer que explica el guion de la práctica.
//
// - Tanto el buffer estándar como el nuevo de productos falsificados
// serán de tipo LIFO.
//
// - Ambos buffers tienen por tanto el mismo comportamiento y solo se
// diferencian en quienes son los procesos que introducen/sacan elementos
// de los mismos.
//
// - Se modifica el comportamiento de uno de los consumidores,
// concretamente del que tenga el identificador más bajo. Este consumidor
// irá consumiendo productos estándar y falsificados de forma alternada
// (alternando el tipo de producto en cada iteración).
//
// - El consumidor correspondiente informará que ha comprado el producto
// falsificado solo mediante un mensaje en la consola.
//
// - Fijar el número de productores estándar a 4, el de consumidores
// a 8 y el número total de productos creados será de 80 (80 de cada tipo)
//
//  CAMBIOS:
//      - creado una nueva asignación de índices:
//          prod_estandar - prod_falsos - consumidores - buffer
//
//      - variables:
//          - nueva etiqueta etiq_falso
//          - ajustado a los valores del enunciado
//          - num_prod_falsos
//          - max_*: maximo indice de un proceso de un tipo u otro
//          - constante: producto_falso = 44
//
//      - producir(): si el id es falso -> produce la const. producto_falso
//                  y muestra un mensaje diferente
//
//      - productor():
//          - calcula el tamaño de la producción antes del bucle.
//              - si es falso -> reparte entre num_prod_falsos
//              - si es estandar -> reparte entre num_prod_estandar
//          - añadido id_buffer para ver a qué buffer mandar:
//              - si es falso -> a id_buffer_falso
//              - si es estandar -> a id_buffer_estandar
//
//      - consumir(): no se toca
//
//      - consumidor():
//          - se ha añadido la variable 'id_buffer', que indica de qué
//          buffer tomar los datos
//
//      - buffer_estandar():
//          - gestión LIFO (solo contar el nº de celdas ocupadas)
//
//      -buffer_falso():
//          - copia del buffer anterior que solo guardará productos falsos 
//
//
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <mpi.h>  // includes de MPI

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

// ---------------------------------------------------------------------
// constantes que determinan la asignación de identificadores a roles:
// productos
const int num_items = 80;           // nº de items producidos/consumidos
const int tam_vector = 5;           // tamaño del buffer 
const int producto_falso = 44;      // producto falso

// número de procesos
const int num_prod_estandar = 4;
const int num_prod_falsos = 2;
const int num_consumidores = 8;
const int num_procesos_esperado=num_prod_estandar+num_prod_falsos+num_consumidores+2;

// etiquetas
const int etiq_estandar = 0, etiq_falso = 1, etiq_consumidor = 2;

// índices
const int id_buffer_estandar = num_procesos_esperado-2;
const int id_buffer_falso = num_procesos_esperado-1;

// índices máximos (para cada tipo de proceso)
int max_prod_estandar = num_prod_estandar;
int max_prod_falsos = max_prod_estandar+num_prod_falsos;
int max_consumidores = max_prod_falsos+num_consumidores;

// int contador[num_productores] = {0};


//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)

//----------------------------------------------------------------------

template<int min, int max> int aleatorio()
{
    static default_random_engine generador((random_device())());
    static uniform_int_distribution<int> distribucion_uniforme(min, max);
    return distribucion_uniforme(generador);
}

// ---------------------------------------------------------------------

// produce los numeros en secuencia (1,2,3,....)
int producir(int id)
{
    int producido;
    sleep_for(milliseconds(aleatorio<10,200>())); // retardo aleatorio

    /* PRODUCTORES ESTANDAR */
    if (id < num_prod_estandar) {
        static int contador[num_prod_estandar] = {0}; // contador (estático)

        // actualización del contador y produccion del valor según id
        contador[id]++;
        producido = contador[id] + id*(num_items/num_prod_estandar);

        // retorno del valor producido
        cout << "[+] productor " << id << ": ha producido valor "
             << producido << endl << flush;
    }

    /* PRODUCTORES FALSOS */
    else {
        producido = producto_falso;
        cout << "[+] PRODUCTOR FALSO " << id << ": HA FALSIFICADO EL VALOR "
             << producido << endl << flush;
    }

    return producido;
}

// ---------------------------------------------------------------------

void funcion_productor(int id, bool falso)
{
    // número de elementos a producir
    unsigned tam_produccion=num_items/(falso?num_prod_falsos:num_prod_estandar);
    unsigned id_buffer = (falso) ? id_buffer_falso : id_buffer_estandar;

    for (unsigned int i=0; i<tam_produccion; i++) {
        // produce
        int valor_prod = producir(id);

        // envía el valor y espera a que se reciba
        cout << "[+] productor " << id << ": va a enviar valor " << valor_prod
             << endl << flush;

        // envía a un buffer u otro según si es falso o no
        MPI_Send(&valor_prod, 1, MPI_INT, id_buffer, etiq_estandar, MPI_COMM_WORLD);
    }
}

// ---------------------------------------------------------------------

void consumir(int valor_cons, int id)
{
    // espera bloqueada
    sleep_for(milliseconds(aleatorio<10,200>()));
    cout << "[+] consumidor " << id << ": consumido valor "
         << valor_cons << endl << flush;
}

// ---------------------------------------------------------------------

void funcion_consumidor(int id)
{
    int peticion;
    int valor_rec = 1;
    MPI_Status estado;


    unsigned int tam_consumo = num_items/num_consumidores;
    if (id == 0) tam_consumo += num_items;

    for(unsigned int i=0; i<tam_consumo; i++) {
        unsigned int id_buffer;

        // asigna el buffer del que consumir los datos
        // si el ID es 0 y:
        //  - es impar
        //  - o se ha consumido ya todo lo estandar
        // coge del buffer falso
        if (id==0 && (i%2==0 || i > 2*num_items/num_consumidores)) {
            id_buffer = id_buffer_falso;
            cout << "CONSUMIDOR 0 VA A COMPRAR UN PRODUCTO FALSO" << endl;
        }
        else id_buffer = id_buffer_estandar;

        // envía la petición y espera a que se reciba
        MPI_Send(&peticion,  1, MPI_INT, id_buffer,
                  etiq_consumidor, MPI_COMM_WORLD);

        // espera a que se envíen los datos
        MPI_Recv (&valor_rec, 1, MPI_INT, id_buffer,
                  etiq_consumidor, MPI_COMM_WORLD, &estado);

        // consume
        cout << "[+] consumidor " << id << ": ha recibido valor "
             << valor_rec << endl << flush;
        consumir(valor_rec, id);
    }
}

// ---------------------------------------------------------------------

void funcion_buffer_estandar()
{
    int buffer[tam_vector];// buffer con celdas ocupadas y vacías
    int valor;                      // valor recibido o enviado
    int primera_libre = 0;             // nº de celdas ocupadas en el vector estandar
    int etiq_aceptable;             // etiqueta del emisor aceptable
    MPI_Status estado;              // estado de los mensajes recibidos


    // 'num_items' iteraciones
    for (unsigned int i=0; i<2*num_items; i++) {

        /* DETERMINAR EMISOR ACEPTABLE */

        // buffer vacío -> del productor estandar
        if (primera_libre == 0)
            etiq_aceptable = etiq_estandar;

        // buffer lleno -> del consumidor
        else if (primera_libre == tam_vector)
            etiq_aceptable = etiq_consumidor;

        // ni lleno ni vacío -> de cualquiera
        else etiq_aceptable = MPI_ANY_TAG;


        /* RECIBIR UN MENSAJE */
        MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE,
                 etiq_aceptable, MPI_COMM_WORLD, &estado);


        /* PROCESAR EL MENSAJE */
        switch(estado.MPI_TAG) {
            /* PRODUCTOR */
            case etiq_estandar:
                // introduce en el buffer y actualiza su estado
                buffer[primera_libre] = valor;
                primera_libre++;
                cout << "[!] buffer_estandar: recibido valor " << valor << endl;
                break;

            /* CONSUMIDOR */
            case etiq_consumidor:
                // lee del buffer y actualiza su estado
                primera_libre--;
                valor = buffer[primera_libre];

                // envía el valor al consumidor que ha solicitado
                cout << "[!] buffer_estandar: va a enviar valor " << valor << endl;
                MPI_Ssend(&valor, 1, MPI_INT, estado.MPI_SOURCE,
                          etiq_consumidor, MPI_COMM_WORLD);
                break;
        }
    }
}

void funcion_buffer_falso()
{
    int buffer[tam_vector];// buffer con celdas ocupadas y vacías
    int valor;                      // valor recibido o enviado
    int primera_libre = 0;             // nº de celdas ocupadas en el vector estandar
    int etiq_aceptable;             // etiqueta del emisor aceptable
    MPI_Status estado;              // estado de los mensajes recibidos


    // 'num_items' iteraciones
    for (unsigned int i=0; i<2*num_items; i++) {

        /* DETERMINAR EMISOR ACEPTABLE */

        // buffer vacío -> del productor estandar
        if (primera_libre == 0)
            etiq_aceptable = etiq_estandar;

        // buffer lleno -> del consumidor
        else if (primera_libre == tam_vector)
            etiq_aceptable = etiq_consumidor;

        // ni lleno ni vacío -> de cualquiera
        else etiq_aceptable = MPI_ANY_TAG;


        /* RECIBIR UN MENSAJE */
        MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE,
                 etiq_aceptable, MPI_COMM_WORLD, &estado);


        /* PROCESAR EL MENSAJE */
        switch(estado.MPI_TAG) {
            /* PRODUCTOR */
            case etiq_estandar:
                // introduce en el buffer y actualiza su estado
                buffer[primera_libre] = valor;
                primera_libre++;
                cout << "[!] BUFFER_FALSO: recibido valor " << valor << endl;
                break;

            /* CONSUMIDOR */
            case etiq_consumidor:
                // lee del buffer y actualiza su estado
                primera_libre--;
                valor = buffer[primera_libre];

                // envía el valor al consumidor que ha solicitado
                cout << "[!] BUFFER_FALSO: va a enviar valor " << valor << endl;
                MPI_Ssend(&valor, 1, MPI_INT, estado.MPI_SOURCE,
                          etiq_consumidor, MPI_COMM_WORLD);
                break;
        }
    }
}

// ---------------------------------------------------------------------

int main(int argc, char *argv[])
{
    /* COMPROBACIÓN */
    if (num_items%num_prod_estandar || num_items%num_prod_falsos
     || num_items%num_consumidores) {
        cerr << "[-] el nº de items debe ser múltiplo del nº de"
             << "productores y consumidores" << endl;
        exit(-1);
    }


    /* INICIALIZACIÓN */
    int id_propio, num_procesos_actual;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
    MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


    /* EJECUCIÓN */
    // (estandar) (falsos) (consumidores) (buffers)
    // 0-3 4-5 6-13 14-15

    if (num_procesos_esperado == num_procesos_actual) {
        if (id_propio < max_prod_estandar)
            funcion_productor(id_propio, false);
        else if (id_propio < max_prod_falsos)
            funcion_productor(id_propio, true);
        else if (id_propio < max_consumidores)
            funcion_consumidor(id_propio-num_prod_falsos-num_prod_estandar);
        else if (id_propio == id_buffer_estandar)
            funcion_buffer_estandar();
        else
            funcion_buffer_falso();
    }
    else if (id_propio == 0)
        cerr << "[-] error: número de procesos distinto del esperado." << endl;


    /* FINALIZACIÓN */
    MPI_Finalize();
    return 0;
}

// ---------------------------------------------------------------------
