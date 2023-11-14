
#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <thread>
#include "scd.h"
#include <mutex>

using namespace std ;
using namespace scd ;

const int L = 4;
const int E = 2;

class LectoresEscritores : public HoareMonitor
{

private: 
	int n_lec;
	bool escrib;
	int contador;
	CondVar lectura;
	CondVar escritura;


public:
	LectoresEscritores();
	void ini_lectura();
	void fin_lectura();
	void ini_escritura();
	void fin_escritura();



};

LectoresEscritores::LectoresEscritores(){

	n_lec = 0;
	escrib = false;
	lectura = newCondVar();
	escritura = newCondVar();
	contador=0;


}


void LectoresEscritores::ini_lectura(){
	
	
	if (escrib){
		
		lectura.wait();
		
	}
	
	if (contador < 2){
		lectura.wait();
	
	}
	n_lec ++;
	
	lectura.signal();


}


void LectoresEscritores::fin_lectura(){
	n_lec--;
	
	contador = 0;
	
	if (n_lec == 0){
			
		escritura.signal();
	
	}
	
	



}


void LectoresEscritores::ini_escritura(){
	
	if ((n_lec > 0 || escrib) && contador == 2){
		
		escritura.wait();
	
	}
	
	escrib = true;

}



void LectoresEscritores::fin_escritura(){
	
	
	escrib = false;
	contador++;
	if (!(lectura.empty()) && contador == 2){
		
		lectura.signal();
	
	}else{
		
		escritura.signal();
	
	}


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


int main (){

cout    << "--------------------------------------------------------------------" << endl
        << "Problema de los lectores y los escritores con monitores. " << endl
        << "--------------------------------------------------------------------" << endl
        << flush ;
        
        MRef<LectoresEscritores> monitor = Create<LectoresEscritores>();
        
        thread lectores[L];
        thread escritores[E];
        
        
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




}
