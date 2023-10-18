#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "scd.h"

using namespace std;
using namespace scd;

const unsigned num_items = 60;
const unsigned tam_vec = 10;
const unsigned NP = 6; // Número de productores
const unsigned NC = 3; // Número de consumidores

unsigned cont_prod[num_items] = {0};
unsigned cont_cons[num_items] = {0};
unsigned siguiente_dato[NP] = {0};

int vec[tam_vec];
int primera_libre = 0;
int primera_ocupada = 0;

Semaphore libres(tam_vec);
Semaphore ocupadas(0);

void test_contadores() {
    bool ok = true;
    cout << "comprobando contadores ....";
    for (unsigned i = 0; i < num_items; i++) {
        if (cont_prod[i] != 1) {
            cout << "error: valor " << i << " producido " << cont_prod[i] << "veces."<<endl;
            ok = false;
        }
        if (cont_cons[i] != 1) {
            cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces"<<endl;
            ok = false;
        }
    }
    if (ok)
        cout << endl << flush << "solución (aparentemente) correcta." << endl << flush;
}

unsigned producir_dato(unsigned i/*, unsigned cont*/) {
    this_thread::sleep_for(chrono::milliseconds(aleatorio<20, 100>()));
    unsigned elementos_por_hebra = num_items / NP;
    //const unsigned dato_producido = (i * elementos_por_hebra) + cont;
    const unsigned dato_producido = (i * elementos_por_hebra) + siguiente_dato[i];
    cont_prod[dato_producido]++;
    cout << "Productor " << i << " produjo: " << dato_producido << endl << flush;
    return dato_producido;
}

void consumir_dato(unsigned i, unsigned dato) {
    assert(dato < num_items);
    cont_cons[dato]++;
    this_thread::sleep_for(chrono::milliseconds(aleatorio<20, 100>()));
    cout << "Consumidor " << i << " consumió: " << dato << endl;
}

void funcion_hebra_productora(unsigned i) {
    //unsigned contador = 0;
    unsigned elementos_por_hebra = num_items / NP;
    unsigned inicio = i * elementos_por_hebra;
    unsigned fin = inicio + elementos_por_hebra - 1; 

    for (unsigned j = inicio; j <= fin; j++) { 
        int dato = producir_dato(i/*,contador*/);
        sem_wait(libres);
        //vec[primera_libre] = dato; LIFO
        vec[primera_libre % tam_vec] = dato;
        primera_libre++;
        //contador++;
        siguiente_dato[i]++;
        sem_signal(ocupadas);
    }
}

void funcion_hebra_consumidora(unsigned i) {
    unsigned elementos_por_hebra = num_items / NC;
    unsigned inicio = i * elementos_por_hebra;
    unsigned fin = inicio + elementos_por_hebra - 1; 

    for (unsigned j = inicio; j <= fin; j++) { 
        int dato;
        sem_wait(ocupadas);
        //primera_libre--;//contador de la pila LIFO
        //dato = vec[primera_libre]; // para lifo
        dato = vec[primera_ocupada % tam_vec];
        primera_ocupada++;
        sem_signal(libres);
        consumir_dato(i, dato);
    }
}

int main() {
    cout << "-----------------------------------------------------------------" << endl
         << "Problema de los productores-consumidores (solución LIFO o FIFO ?)." << endl
         << "------------------------------------------------------------------" << endl
         << flush;

    thread hebras_productoras[NP];
    thread hebras_consumidoras[NC];

    for (unsigned i = 0; i < NP; i++) {
        hebras_productoras[i] = thread(funcion_hebra_productora, i);
    }

    for (unsigned i = 0; i < NC; i++) {
        hebras_consumidoras[i] = thread(funcion_hebra_consumidora, i);
    }

    for (unsigned i = 0; i < NP; i++) {
        hebras_productoras[i].join();
    }

    for (unsigned i = 0; i < NC; i++) {
        hebras_consumidoras[i].join();
    }

    test_contadores();

    cout << "Solucion FIFO" << endl;
    cout << "FIN" << endl;
}

