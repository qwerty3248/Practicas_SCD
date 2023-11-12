#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "scd.h"

using namespace std ;
using namespace scd ;

// numero de fumadores 
const int num_ingredientes_producir = 20;
int num_ingredientes_producidos = 0;
const int num_fumadores = 3 ;
Semaphore mostr_vacio (1);
Semaphore ingr_disp [3] = {0,0,0};
bool cartel_cerrado = false;


//-------------------------------------------------------------------------
// Función que simula la acción de producir un ingrediente, como un retardo
// aleatorio de la hebra (devuelve número de ingrediente producido)

int producir_ingrediente()
{
   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_produ( aleatorio<10,100>() );

   // informa de que comienza a producir
   cout << "Estanquero : empieza a producir ingrediente (" << duracion_produ.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_produ' milisegundos
   this_thread::sleep_for( duracion_produ );

   const int num_ingrediente = aleatorio<0,num_fumadores-1>() ;

   // informa de que ha terminado de producir
   cout << "Estanquero : termina de producir ingrediente " << num_ingrediente << endl;
   //ya ha producido un ingrediente más
   num_ingredientes_producidos++;
   
   return num_ingrediente ;
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero()
{ 	
	int i;
	while(!cartel_cerrado){
		
		i = producir_ingrediente();
		sem_wait(mostr_vacio);
		cout<<"Puesto ingrediente: "<<i<<" en el mostrador"<<endl;
		sem_signal(ingr_disp[i]);
		
		if (num_ingredientes_producidos >= num_ingredientes_producir){
			cartel_cerrado = true;
			if (cartel_cerrado){
				cout<<"Colocando el cartel de cerrado"<<endl;
				int aux = num_fumadores;
				int contador = 0;
				while (aux != 0){
					sem_signal(ingr_disp[contador]);
					contador++;
					aux--;
				}
			}
		}
	}
	
	
	
	
}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( !cartel_cerrado )
   {
   	sem_wait(ingr_disp[num_fumador]);
   	cout<<"Retira el ingrediente "<<num_fumador<<" del mostrador"<<endl;
   	sem_signal(mostr_vacio);
	fumar(num_fumador);
   }
   
   cout<<"Soy el fumador "<<num_fumador<<" y el estanquero ha cerrado =("<<endl;
   
}

//----------------------------------------------------------------------

int main()
{
   thread estanquero (funcion_hebra_estanquero);
   thread fumadores[num_fumadores];
   for(int i = 0; i < num_fumadores; i++){
   	fumadores[i] = thread (funcion_hebra_fumador, i);
   }
   for(int i = 0; i < num_fumadores; i++){
   	fumadores[i].join();
   }
   estanquero.join();
   
   
   
   
   
}

