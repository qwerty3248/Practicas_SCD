//esta variante lo que va a hacer es cuando un fuamdor acabe de fumar lo tire a 
//la papelera y haya una hebra limpiadora que limpie la papelera 

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

const int num_fumadores = 3 ;


//------------------------------------------------------------------------------
//Monitor estanquero con su constructor y las 3 funciones que les corresponden
//------------------------------------------------------------------------------

class Estanquero : public HoareMonitor
{

private: 
	CondVar fumador;
	CondVar estanquero;
	CondVar limpiador;
	CondVar basura;
	bool mostrador_lleno;
	int ingrediente_producido;
	bool basura_llena;

public:
	Estanquero();
	void obtenerIngrediente(int i);
	void ponerIngrediente(int ingrediente);
	void esperarRecogidaIngrediente();
	void limpiar();
	void Papelera(int num_fumador);
	void PuedeTirar();

};

Estanquero::Estanquero(){
	
	fumador = newCondVar();
	estanquero = newCondVar();
	basura = newCondVar();
	limpiador = newCondVar();
	mostrador_lleno = false;
	ingrediente_producido = -1;
	basura_llena = false;



}


void Estanquero::obtenerIngrediente(int i){
	
	while(!mostrador_lleno || ingrediente_producido != i){
		
		fumador.wait();
		
	}
	
	// El resto del código permanece igual.
	chrono::milliseconds duracion_produ(aleatorio<10, 100>());
	this_thread::sleep_for(duracion_produ);
	//al cogerlo el mostrador ya no está lleno
	mostrador_lleno = false;
	
	cout << "Obtengo el ingrediente " << i << ", soy el fumador " << i << endl;
	
	estanquero.signal();
}


void Estanquero::ponerIngrediente(int ingrediente){
	
	
	//tengo el ingrediente producido
	ingrediente_producido = ingrediente;
	//retardo
	chrono::milliseconds duracion_produ( aleatorio<10,100>() );
	this_thread::sleep_for( duracion_produ );
	//mensaje
	cout<<"El ingrediente "<<ingrediente<<" ha sido colocado"<<endl;
	//el mostrador esta lleno
	mostrador_lleno=true;
	//Ahora lo que hacemos que en caso de no poner esta parte del 
	//codigo se quedara pillado ya que todos habran hecho en la mayoria
	//de casos un wait, es decir están esperando en la cola de fumadores
	//porque obtener ingredientes se puede ejecutar antes que poner
	//ingrediente, lo que hacemos ahora es sacar a todos los fumadores que
	//estuvieran esperando en la cola, no pasa nada ya que en obtener 
	//ingrediente nos aseguramos de que si no es su ingrediente y el 
	//mostrador no esta lleno, seguiran esperando en este caso solo nos
	//preocupa el ingrediente, es decir, solo seguira avanzando el fumador
	//cuyo ingrediente es el correcto, el resto seguira en el while 
	//esperando hasta que llegue su ingrediente
	int aux = num_fumadores;
	while(aux != 0){
		aux--;
		fumador.signal();
	
	}
	

}


void Estanquero::esperarRecogidaIngrediente(){
	//espera mientras el mostrador este lleno, cuando un fumador
	//ponga la bandera a false, saldra del while y el estanquero se dira 
	//de salirse de la espera el mismo, para que pueda continuar la 
	//ejecuccion
	
	while(mostrador_lleno){
		
		estanquero.wait();
	
	}
	//estanquero.signal(); aqui no, la salida se la debe de dar el fumador
	//cuando coge el ingrediente, no el mismo sino se queda pillado en un
	//caso
	
	cout<<"El mostrador ya esta vacio"<<endl;
	
	


}



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

void funcion_hebra_estanquero(MRef<Estanquero> monitor)
{ 	
	int i;
	while(true){
		
		i = producir_ingrediente();
		monitor->ponerIngrediente(i);
		monitor->esperarRecogidaIngrediente();
		
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
void Estanquero::Papelera(int num_fumador){

	//si la basura esta llena debe esperar
	if (basura_llena){
		
		basura.wait();
	
	}
	
	
	cout << "Fumador " << num_fumador << "  : Tira el ingrediente a la basura." << endl;
	
	basura_llena = true;
	
	// "llama" al limpiador para que limpìe la basura
	limpiador.signal();
	
	//mensaje



}

//----------------------------------------------------------------------
/*void Tirar_a_basura(int num_fumador){

	

}*/

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( MRef<Estanquero> monitor,int num_fumador )
{
   while( true )
   {
   	
	monitor->obtenerIngrediente(num_fumador);
	fumar(num_fumador);
	monitor->Papelera(num_fumador);
	//Tirar_a_basura(num_fumador);
   }
}


//----------------------------------------------------------------------
void Estanquero::limpiar()
{
	if (!basura_llena)
	{
		limpiador.wait();
	}
	
	//mensaje de limpieza 
	
	
}

//----------------------------------------------------------------------
void Estanquero::PuedeTirar()
{
	//mensaje de limpieza
	
	basura_llena = false;
	
	//int aux = basura.get_nwt();
	//liberamos a los que estuvieran esperando porque la basura estaba llena
	//while (aux != 0){
	
	//	basura.signal();
	//	aux--;
	//}
	
	//mejor solo liberamos a uno para que ese tire la basura y llame al 
	//limpiador y luego este llame al siguiente que este esperando a 
	//tirar su cigarro
	
	
	//llama al primero de la cola que este esperando para tirar su cigarro
	basura.signal();
	
	
}

//-----------------------------------------------------------------------
void limpieza()
{
	cout<<"He limpiado la basura de los guarros que la manchan"<<endl;
}

//-----------------------------------------------------------------------
void funcion_hebra_limpiador(MRef<Estanquero> monitor)
{

	while ( true )
	{
	
		monitor->limpiar();
		limpieza();
		monitor->PuedeTirar();
		
	}


}

//----------------------------------------------------------------------

int main()
{
   
   MRef<Estanquero> monitor = Create<Estanquero>();
   		
   thread estanquero (funcion_hebra_estanquero,monitor);
   thread fumadores[num_fumadores];
   thread limpiador(funcion_hebra_limpiador,monitor);
   for(int i = 0; i < num_fumadores; i++){
   	fumadores[i] = thread (funcion_hebra_fumador,monitor, i);
   }
   for(int i = 0; i < num_fumadores; i++){
   	fumadores[i].join();
   }
   estanquero.join();
   limpiador.join();
   
   
   
}

