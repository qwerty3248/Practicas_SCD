#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "scd.h"

using namespace std;
using namespace scd;

//**********************************************************************
// Variables globales

const unsigned 
   num_items = 60 ,   // número de items
   tam_vec   = 10 ;   // tamaño del buffer

const unsigned NP = 6,
               NC = 3;

unsigned  
   cont_prod[num_items] = {0}, // contadores de verificación: para cada dato, número de veces que se ha producido.
   cont_cons[num_items] = {0}, // contadores de verificación: para cada dato, número de veces que se ha consumido.
   siguiente_dato [NP]      = {0} ;  // siguiente dato a producir en 'producir_dato' (solo se usa ahí)

unsigned buffer[tam_vec];

unsigned consumidora = 0;
unsigned productora = 0;

Semaphore escribe(tam_vec);
Semaphore lee(0);



void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}


unsigned producir_dato(unsigned i)
{
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
   const unsigned dato_producido = i* (num_items/NP) + siguiente_dato[i] ;
   siguiente_dato[i]++ ;
   cont_prod[dato_producido] ++ ;
   cout << "Productor "<<i<<" ha producido: " << dato_producido << endl << flush ;
   return dato_producido ;
}



void consumir_dato( unsigned dato, unsigned i )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "Consumidor "<<i<<" ha consumido: " << dato << endl ;

}

void  funcion_hebra_productora(unsigned i)
{
	unsigned inicio = i* (num_items/NP);
	unsigned fin = inicio + (((num_items/NP))-1);
	
	for(unsigned k = inicio; k <= fin; k++){
		unsigned dato = producir_dato(i);
		sem_wait(escribe);
		buffer[productora % tam_vec] = dato;
		productora++;
		sem_signal(lee);
	
	}





}


void  funcion_hebra_consumidora(unsigned i)
{
	unsigned inicio = i* (num_items/NC);
	unsigned fin = inicio + (((num_items/NC))-1);
	
	for(unsigned k = inicio; k <= fin; k++){
		unsigned dato;
		sem_wait(lee);
		dato = buffer[consumidora % tam_vec];
		consumidora++;
		sem_signal(escribe);
		consumir_dato(dato,i);
	}






}

int main()
{
   cout << "-----------------------------------------------------------------" << endl
        << "Problema de los productores-consumidores (solución LIFO o FIFO ?)." << endl
        << "------------------------------------------------------------------" << endl
        << flush ;

	thread hebras_productoras[NP];
	thread hebras_consumidoras[NC];
	
	for(unsigned i = 0; i < NP ; i++){
		
		hebras_productoras[i] = thread(funcion_hebra_productora,i);
	
	}
	
	for(unsigned i = 0; i < NC ; i++){
		
		hebras_consumidoras[i] = thread(funcion_hebra_consumidora,i);
	
	}
	
	
	for(unsigned i = 0; i < NP ; i++){
		
		hebras_productoras[i].join();
	
	}
	
	for(unsigned i = 0; i < NC ; i++){
		
		hebras_consumidoras[i].join();
	
	}

   test_contadores();

}



























