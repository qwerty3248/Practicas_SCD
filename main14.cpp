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

const int num_fumadores = 4 ;
Semaphore mostr_vacio (1);
Semaphore contar_cigarros(1);
Semaphore impar(0);
Semaphore ingr_disp [4] = {0,0,0,0};
int num_cigarrillos_fumados = 0;


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

   return num_ingrediente ;
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero()
{ 	
	int i;
	bool primera = true;
	while(true){
		
		
		i = producir_ingrediente();
		if (!primera&&(num_cigarrillos_fumados % 3 == 0)){
			sem_wait(impar);
		}
		sem_wait(mostr_vacio);
		cout<<"Puesto ingrediente: "<<i<<" en el mostrador"<<endl;
		sem_signal(ingr_disp[i]);
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
    if (num_cigarrillos_fumados %3 == 0){
    	sem_signal(impar);
    }

}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {
   	sem_wait(ingr_disp[num_fumador]);
   	cout<<"Retira el ingrediente "<<num_fumador<<" del mostrador"<<endl;
   	sem_signal(mostr_vacio);
   	sem_wait(contar_cigarros);
   	if (num_cigarrillos_fumados % 2 == 0){
   	
   		cout<<"Soy el fumador "<<num_fumador<<" llevamos: "<<num_cigarrillos_fumados<<" fumados y aviso al estanquero de que puede poner más ingredientes"<<endl;
   	}else{
   		cout<<"Soy el fumador "<<num_fumador<<" llevamos: "<<num_cigarrillos_fumados<<" fumados y aviso al estanquero de que voy a fumar"<<endl;
   	
   	}
	fumar(num_fumador);
	num_cigarrillos_fumados++;
	sem_signal(contar_cigarros);
	
   }
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

