#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include "scd.h"

using namespace std;
using namespace scd;

const int num_fumadores = 3;
Semaphore mostr_vacio(1);
Semaphore suministros(1);
Semaphore ingr_disp[3] = {0, 0, 0};
Semaphore capacidad(1); 
int buffer[3];
int primera_ocupada = 0, primera_libre = 0, ocupacion = 0;

int producir_ingrediente(int i)
{
    chrono::milliseconds duracion_produ(aleatorio<10, 100>());
    cout << "Suministradora["<<i<<"]: empieza a producir ingrediente (" << duracion_produ.count() << " milisegundos)" << endl;
    this_thread::sleep_for(duracion_produ);
    const int num_ingrediente = aleatorio<0, num_fumadores - 1>();
    cout << "Suministradora["<<i<<"]: termina de producir ingrediente " << num_ingrediente << endl;
    return num_ingrediente;
}

void funcion_hebra_suministradora(int num_suministradora)
{
    int i;
    while (true)
    {
        i = producir_ingrediente(num_suministradora);
        sem_wait(capacidad);
        sem_wait(suministros);
        buffer[primera_libre % 3] = i;
        primera_libre++;
        ocupacion++;
        sem_signal(suministros);
        sem_signal(capacidad);
    }
}

void funcion_hebra_estanquero()
{
    int i;
    while (true)
    {
        sem_wait(capacidad);
        sem_wait(mostr_vacio);
        if (ocupacion > 0)
        {
            i = buffer[primera_ocupada % 3];
            primera_ocupada++;
            ocupacion--;
            cout << "Puesto ingrediente: " << i << " en el mostrador" << endl;
            sem_signal(ingr_disp[i]);
        }
        sem_signal(mostr_vacio);
        sem_signal(capacidad);
    }
}

void fumar(int num_fumador)
{
    chrono::milliseconds duracion_fumar(aleatorio<20, 200>());
    cout << "Fumador " << num_fumador << " : empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;
    this_thread::sleep_for(duracion_fumar);
    cout << "Fumador " << num_fumador << " : termina de fumar, comienza espera de ingrediente." << endl;
}

void funcion_hebra_fumador(int num_fumador)
{
    while (true)
    {
        sem_wait(ingr_disp[num_fumador]);
        cout << "Retira el ingrediente " << num_fumador << " del mostrador" << endl;
        sem_signal(mostr_vacio);
        fumar(num_fumador);
    }
}

int main()
{
    thread estanquero(funcion_hebra_estanquero);
    thread fumadores[num_fumadores];
    thread suministradoras[2];
    for (int i = 0; i < num_fumadores; i++)
    {
        fumadores[i] = thread(funcion_hebra_fumador, i);
    }
    for (int i = 0; i < 2; i++)
    {
        suministradoras[i] = thread(funcion_hebra_suministradora, i);
    }
    for (int i = 0; i < num_fumadores; i++)
    {
        fumadores[i].join();
    }
    for (int i = 0; i < 2; i++)
    {
        suministradoras[i].join();
    }
    estanquero.join();

    return 0;
}

