/////// prodcons-alt1.cpp ///////////////////////////////////////////
// Construir un programa MPI, que se deberá llamar prod_cons_ex.cpp,
// operaciones de paso de mensaje síncronas siguiendo el esquema del
// productor-consumidor, en el que tenemos cuatro procesos consumidores
// (identificadores de proceso del 0 al 3) y dos procesos productores
// (identificadores de proceso del 4 al 5) que se comunican mediante un
// proceso intermedio (identificador 6), que gestiona un vector de
// tamaño 5. Los productores generarán 30 elementos enteros cada uno.
// Otras consideraciones:
//     - La gestión del vector se debe hacer en modo FIFO.
//     - Cuando el proceso intermedio recibe un elemento concreto de cada
//     productor, no puede atender peticiones de ningún productor hasta
//     vaciar el vector. Es decir, cuando el proceso intermedio reciba el
//     décimo elemento producido por el productor con identificador 5
//     (sean cuales sean los valores), debe indicar que se ha dado esa
//     situación con un mensaje por pantalla y no puede recibir elementos
//     de ningún productor hasta que no haya enviado todos los elementos
//     que haya en ese momento en el vector a los procesos consumidores.

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
const int num_items = 60;           // nº de items producidos/consumidos
const int tam_vector = 5;           // tamaño del buffer 

// solución con múltiples productores/consumidores
const int num_productores = 2;
const int num_consumidores = 4;
const int num_procesos_esperado = num_productores+num_consumidores+1;
const int etiq_productor = 0, etiq_consumidor = 1;
const int id_buffer = num_productores+num_consumidores;
int contador[num_productores] = {0};


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
    int producido;                                // valor producido
    // static int contador[num_productores] = {0};   // contador (estático)
    sleep_for(milliseconds(aleatorio<10,200>())); // retardo aleatorio

    // actualización del contador y produccion del valor según id
    contador[id]++;
    producido = contador[id] + id*(num_items/num_productores);

    // retorno del valor producido
    cout << "[+] productor " << id << ": ha producido valor "
         << producido << endl << flush;
    return producido;
}

// ---------------------------------------------------------------------

void funcion_productor(int id)
{
    for (unsigned int i=0; i<num_items/num_productores; i++) {
        // produce
        int valor_prod = producir(id);

        // envía el valor y espera a que se reciba
        cout << "[+] productor " << id << ": va a enviar valor " << valor_prod
             << endl << flush;
        MPI_Send(&valor_prod, 1, MPI_INT, id_buffer,
                  etiq_productor, MPI_COMM_WORLD);
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
    int valor_rec;
    MPI_Status estado;

    // 'num_items' repeticiones
    for(unsigned int i=0; i<num_items/num_consumidores; i++) {
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

void funcion_buffer()
{
    int buffer[tam_vector];      // buffer con celdas ocupadas y vacías
    int valor;                   // valor recibido o enviado
    int primera_libre = 0;       // índice de primera celda libre
    int etiq_aceptable;          // etiqueta del emisor aceptable
    MPI_Status estado;           // estado de los mensajes recibidos
    int contador[num_productores] = {0}; // producido por cada uno

    bool limpiando = false;      // se activa cuando se envían los valores concretos


    // 'num_items' iteraciones
    for (unsigned int i=0; i<num_items*2; i++) {

        /* DETERMINAR EMISOR ACEPTABLE */

        // buffer vacío -> del productor
        if (primera_libre == 0){
            etiq_aceptable = etiq_productor;

            // deja de limpiar
            if (limpiando) {
                cout << "[!] buffer: FIN DE LA LIMPIEZA" << endl;
                limpiando = false;
            }
        }

        // buffer lleno o limpiandose -> del consumidor
        else if (primera_libre == tam_vector || limpiando)
            etiq_aceptable = etiq_consumidor;

        // ni lleno ni vacío ni limpiando -> de cualquiera
        else etiq_aceptable = MPI_ANY_TAG;


        /* RECIBIR UN MENSAJE */
        MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE,
                 etiq_aceptable, MPI_COMM_WORLD, &estado);


        /* PROCESAR EL MENSAJE */
        switch(estado.MPI_TAG) {
            /* PRODUCTOR */
            case etiq_productor:
                // introduce en el buffer y actualiza su estado
                buffer[primera_libre] = valor;
                primera_libre++;
                cout << "[!] buffer: recibido valor " << valor << endl;

                // aumenta el contador
                contador[estado.MPI_SOURCE-num_consumidores]++;

                // cuando se envía el décimo elemento, empieza a limpiar
                if (contador[estado.MPI_SOURCE-num_consumidores] == 10) {
                    limpiando = true;
                    cout << "[!] buffer: LIMPIANDO... " << valor << endl;
                }

                break;

            /* CONSUMIDOR */
            case etiq_consumidor:
                // lee del buffer y actualiza su estado
                primera_libre--;
                valor = buffer[primera_libre];

                // envía el valor al consumidor que ha solicitado
                cout << "[!] buffer: va a enviar valor " << valor << endl;
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
    if (num_items%num_productores || num_items%num_consumidores) {
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
    if (num_procesos_esperado == num_procesos_actual) {
        if (id_propio < num_consumidores) funcion_consumidor(id_propio);
        else if (id_propio == id_buffer) funcion_buffer();
        else funcion_productor(id_propio-num_consumidores);
    }
    else if (id_propio == 0)
        cerr << "[-] error: número de procesos distinto del esperado." << endl;


    /* FINALIZACIÓN */
    MPI_Finalize();
    return 0;
}

// ---------------------------------------------------------------------
