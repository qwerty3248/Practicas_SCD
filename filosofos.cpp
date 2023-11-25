// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: filosofos-plantilla.cpp
// Implementación del problema de los filósofos (sin camarero).
// Plantilla para completar.
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------


#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

const int num_filosofos = 5; // número de filósofos 
const int num_filo_ten = 2 * num_filosofos; // número de filósofos y tenedores
const int num_procesos = num_filo_ten + 1; // número de procesos total (por ahora solo hay filo y ten)
const int id_director = num_procesos - 1;//vamos a usar esta variable para que los 
   //filosofos puedan pedir el tenedor izquierdo primero, si no están todos
   //cogidos o falta uno por coger
const int etiq_izq = 123;

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template <int min, int max>
int aleatorio()
{
    static default_random_engine generador((random_device())());
    static uniform_int_distribution<int> distribucion_uniforme(min, max);
    return distribucion_uniforme(generador);
}

// ---------------------------------------------------------------------

void funcion_filosofos(int id)
{
    int id_ten_izq = (id + 1) % num_filo_ten; //id. tenedor izq.
    int id_ten_der = (id + num_filo_ten - 1) % num_filo_ten; //id. tenedor der.
    int recibido;

    while (true)
    {
        //preguntamos al director si puede cogerlo
        MPI_Ssend(&recibido, 1, MPI_INT, id_director, 0, MPI_COMM_WORLD);
        cout << "Filósofo " << id << " solicita ten. izq." << id_ten_izq << endl;
        // ... solicitar tenedor izquierdo (completar)
        MPI_Ssend(&recibido, 1, MPI_INT, id_ten_izq, 0, MPI_COMM_WORLD);
        cout << "Filósofo " << id << " solicita ten. der." << id_ten_der << endl;
        // ... solicitar tenedor derecho (completar)
        MPI_Ssend(&recibido, 1, MPI_INT, id_ten_der, 0, MPI_COMM_WORLD);

        cout << "Filósofo " << id << " comienza a comer" << endl;
        sleep_for(milliseconds(aleatorio<10, 100>()));

        cout << "Filósofo " << id << " suelta ten. izq. " << id_ten_izq << endl;
        // ... soltar el tenedor izquierdo (completar)
        MPI_Ssend(&recibido, 1, MPI_INT, id_ten_izq, 0, MPI_COMM_WORLD);
        //le decimos al director que ya se ha soltado el tenedor izquierdo
        MPI_Ssend(&recibido, 1, MPI_INT, id_director, etiq_izq, MPI_COMM_WORLD);

        cout << "Filósofo " << id << " suelta ten. der. " << id_ten_der << endl;
        // ... soltar el tenedor derecho (completar)
        MPI_Ssend(&recibido, 1, MPI_INT, id_ten_der, 0, MPI_COMM_WORLD);

        cout << "Filósofo " << id << " comienza a pensar" << endl;
        sleep_for(milliseconds(aleatorio<10, 100>()));
    }
}

// ---------------------------------------------------------------------

void funcion_tenedores(int id)
{
    int valor, id_filosofo; // valor recibido, identificador del filósofo
    MPI_Status estado;      // metadatos de las dos recepciones

    while (true)
    {
        // ...... recibir petición de cualquier filósofo (completar)        
        MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &estado);
        // ...... guardar en 'id_filosofo' el id. del emisor (completar)
        id_filosofo = estado.MPI_SOURCE;

        cout << "Ten. " << id << " ha sido cogido por filo. " << id_filosofo << endl;

        // Esperar a recibir el mensaje de liberación del filosofo
        MPI_Recv(&valor, 1, MPI_INT, id_filosofo, 0, MPI_COMM_WORLD, &estado);
        cout << "Ten. " << id << " ha sido liberado por filo. " << id_filosofo << endl;
    }
}

// ---------------------------------------------------------------------

void funcion_director()
{
    int valor, tenedores = 0, etiqueta_valida; //valor recibido, cuantos tenedores en mesa
    MPI_Status estado;

    while (true)
    {
        if (tenedores == (num_filosofos - 1))
            //no hay tenedores suficientes, se debe dejar tenedor
            etiqueta_valida = etiq_izq;
        else
            //si no recibimos peticion de algun filosofo
            etiqueta_valida = MPI_ANY_TAG;

        MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE, etiqueta_valida, MPI_COMM_WORLD, &estado);

        if (estado.MPI_TAG == etiq_izq)
            tenedores--;
        else
            tenedores++;
    }
}

// ---------------------------------------------------------------------

int main(int argc, char **argv)
{
    int id_propio, num_procesos_actual;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_propio);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos_actual);

    if (num_procesos == num_procesos_actual)
    {
        if (id_propio == id_director) 
            funcion_director();
        else if (id_propio % 2 == 0) // pares filosofos
            funcion_filosofos(id_propio);
        else
            funcion_tenedores(id_propio); //impares tenedores
    }
    else
    {
        if (id_propio == 0) // el primer id imprimen el error el restoa acaban
        {
            cout << "Número de procesos esperados: " << num_procesos << endl
                 << "Número de procesos en ejecución: " << num_procesos_actual << endl
                 << "(programa abortado)" << endl;
        }
    }

    MPI_Finalize();
    return 0;
}

