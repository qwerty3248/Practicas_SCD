#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include "scd.h"

using namespace std;
using namespace scd;

const int num_fumadores = 3;
Semaphore mostr_vacio(1);
Semaphore ingr_disp[num_fumadores] = { 0,0,0 };
Semaphore sanitario[num_fumadores] = {0,0,0};
Semaphore vicioso(5);
int contador_cigarros_fumadores[num_fumadores] = { 0 };
bool advertencia[num_fumadores] = { false };
int cigarros_fumados[num_fumadores] = { 0 };

int producir_ingrediente()
{
    chrono::milliseconds duracion_produ(aleatorio<10, 100>());

    cout << "Estanquero : empieza a producir ingrediente (" << duracion_produ.count() << " milisegundos)" << endl;

    this_thread::sleep_for(duracion_produ);

    const int num_ingrediente = aleatorio<0, num_fumadores - 1>();

    cout << "Estanquero : termina de producir ingrediente " << num_ingrediente << endl;

    return num_ingrediente;
}

void funcion_hebra_estanquero()
{
    int i;
    while (true) {
        i = producir_ingrediente();
        sem_wait(mostr_vacio);
        cout << "Puesto ingrediente: " << i << " en el mostrador" << endl;
        sem_signal(ingr_disp[i]);
    }
}

void funcion_hebra_sanitaria(int num_fumador)
{
    while (true) {
        sem_wait(sanitario[num_fumador]);
        if (contador_cigarros_fumadores[num_fumador] == 6) {
            cout << "Fumador " << num_fumador << " va a fumar su sexto cigarro." << endl;
            contador_cigarros_fumadores[num_fumador] = 0;
            advertencia[num_fumador] = true;
        }
        sem_signal(ingr_disp[num_fumador]);
    }
}

void fumar(int num_fumador)
{
    chrono::milliseconds duracion_fumar(aleatorio<20, 200>());

    cout << "Fumador " << num_fumador << "  :"
         << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

    this_thread::sleep_for(duracion_fumar);

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;
}

void funcion_hebra_fumador(int num_fumador)
{
    while (true) {
        sem_wait(ingr_disp[num_fumador]);
        sem_wait(vicioso);
        cout << "Retira el ingrediente " << num_fumador << " del mostrador" << endl;
        sem_signal(sanitario[num_fumador]);
        sem_signal(mostr_vacio);
        fumar(num_fumador);
        cigarros_fumados[num_fumador]++;
        if (advertencia[num_fumador] && cigarros_fumados[num_fumador] == 1) {
            cout << "Fumador " << num_fumador << " advierte: ¡Me han dado una advertencia!" << endl;
            advertencia[num_fumador] = false;
        }
    }
}

int main() {
    thread fumadores[num_fumadores];
    thread estanco(funcion_hebra_estanquero);
    thread sanitario[num_fumadores];

    for (int i = 0; i < num_fumadores; i++) {
        fumadores[i] = thread(funcion_hebra_fumador, i);
    }

    for (int i = 0; i < num_fumadores; i++) {
        sanitario[i] = thread(funcion_hebra_sanitaria, i);
    }

    for (int i = 0; i < num_fumadores; i++) {
        fumadores[i].join();
    }

    for (int i = 0; i < num_fumadores; i++) {
        sanitario[i].join();
    }

    estanco.join();

    return 0;
}

