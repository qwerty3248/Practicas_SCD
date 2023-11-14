#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <thread>
#include "scd.h"
#include <mutex>

using namespace std ;
using namespace scd ;

const int L = 1;
const int E = 8;
const int ACCESOS_PERMITIDOS=8;

class LectoresEscritores : public HoareMonitor
{

private: 
	int n_lec;
	bool escrib;
	bool hacer_limpieza;
	int accesos;
	CondVar lectura;
	CondVar escritura;
	CondVar limpiadora;
	CondVar aux;


public:
	LectoresEscritores();
	void ini_lectura();
	void fin_lectura();
	void ini_escritura();
	void fin_escritura();
	void limpiar();



};

LectoresEscritores::LectoresEscritores(){

	n_lec = 0;
	escrib = false;
	accesos = 0;
	hacer_limpieza=false;
	aux = newCondVar();
	limpiadora = newCondVar();
	lectura = newCondVar();
	escritura = newCondVar();


}


void LectoresEscritores::ini_lectura(){

	if (escrib){
		
		lectura.wait();
		
	}
	n_lec ++;
	
	lectura.signal();


}


void LectoresEscritores::fin_lectura(){
	n_lec--;
	accesos++;
	
	if (n_lec == 0){
		
		escritura.signal();
	
	}
	



}


void LectoresEscritores::ini_escritura(){
	
	if (n_lec > 0 || escrib){
		
		escritura.wait();
	
	}
	escrib = true;


}



void LectoresEscritores::fin_escritura(){
	
	accesos++;
	escrib = false;
	
	if (accesos >= ACCESOS_PERMITIDOS){
		hacer_limpieza = true;
		limpiadora.signal();
		
	
	}
	if (hacer_limpieza){
		aux.wait();//lo meto en la cola de la limpiadora
	
	}
	
	if (!(escritura.empty())){
		
		escritura.signal();
	
	}else{
		
		lectura.signal();
	
	}


}

void LectoresEscritores::limpiar(){


	if (!hacer_limpieza){
		limpiadora.wait();	
	}
	
	cout<<"Limpiador: Se han completado "<<accesos<<endl;
	//cout<<"Identificador: "<<endl;
	
	accesos=0;
	hacer_limpieza = false;
	aux.signal();
	
	/*limpiadora.wait();//siempre espera hasta que la despierten
	accesos = 0;
	this_thread::sleep_for( chrono::milliseconds( aleatorio<50,100>() ));
	cout<<"Soy el limpiador y he acabado de limpiar"<<endl;
	limpiadora.signal();//libero al escritor anterior
	*/
	
	


}

void Escritor (MRef<LectoresEscritores> monitor,int escritor){
	
	while(true){
		monitor->ini_escritura();
		cout<<"Soy el escritor "<<escritor<<" y he escrito."<<endl;
		this_thread::sleep_for( chrono::milliseconds( aleatorio<50,100>() ));
		monitor->fin_escritura();
	}



}

void Lector (MRef<LectoresEscritores> monitor,int lector){
	
	while(true){
		monitor->ini_lectura();
		cout<<"Soy el lector "<<lector<<" y estoy leyendo."<<endl;
		this_thread::sleep_for( chrono::milliseconds( aleatorio<50,100>() ));
		monitor->fin_lectura();
	}


}

void Limpiador (MRef<LectoresEscritores> monitor) {

	while (true){
		
		monitor->limpiar();
		
	}

}


int main (){

cout    << "--------------------------------------------------------------------" << endl
        << "Problema de los lectores y los escritores con monitores. " << endl
        << "--------------------------------------------------------------------" << endl
        << flush ;
        
        MRef<LectoresEscritores> monitor = Create<LectoresEscritores>();
        
        thread lectores[L];
        thread escritores[E];
        thread limpiador (Limpiador,monitor);
        
        
        for (int i = 0; i < L ; i++){
        
        	lectores[i] = thread (Lector,monitor,i);
        	
        }
        
        for (int i = 0; i < E ; i++){
        	
        	escritores[i] = thread (Escritor,monitor,i);
        
        }
        
        for (int i = 0; i < L ; i++){
        	
        	escritores[i].join();
        
        }
        
        for (int i = 0; i < E ; i++){
        	
        	lectores[i].join();
        
        }	
        limpiador.join();




}


