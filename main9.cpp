
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
const int E = 8;

class LectoresEscritores : public HoareMonitor
{

private: 
	int n_lec;
	bool escrib;
	CondVar lectura;
	CondVar escritura;
	CondVar revisora;


public:
	LectoresEscritores();
	void ini_lectura();
	void fin_lectura();
	void ini_escritura();
	void fin_escritura();
	void ini_revision();
	void fin_revision();



};

LectoresEscritores::LectoresEscritores(){

	n_lec = 0;
	escrib = false;
	lectura = newCondVar();
	escritura = newCondVar();
	revisora = newCondVar();


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
	
	if (n_lec == 0){
		
		escritura.signal();
	
	}



}


void LectoresEscritores::ini_escritura(){
	
	if (n_lec > 0 || escrib){
		
		escritura.wait();
	
	}
	escrib = true;
	//entra a la estructura, entonces le dice a la revisora en caso de que 
	//este esperando que se meta
	revisora.signal();


}



void LectoresEscritores::fin_escritura(){
	
	
	escrib = false;
	//como ya ha salido le dice a la revisora que ya puede salir
	revisora.signal();
	
	if (!(lectura.empty())){
		
		lectura.signal();
		
	}else{
		
		escritura.signal();
	
	}


}

void LectoresEscritores::ini_revision(){
	//solo accede si hay un escritor escribiendo, escrib = true
	if (!escrib){
		revisora.wait();
	
	}
	
	//revisa lo que quiera yo que se las tildes por ejemplo


}

void LectoresEscritores::fin_revision(){
	//si el escritor sigue dentro se queda dentro esperando aunque acabe la 
	//revision de las tildes
	if (escrib){
		revisora.wait();
	
	}
	
	//sale de la revision



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

void Revisora (MRef<LectoresEscritores> monitor){
	
	while(true){
		monitor->ini_revision();
		cout<<"Soy la revisora y he revisado las tildes =)"<<endl;
		this_thread::sleep_for( chrono::milliseconds( aleatorio<50,100>() ));
		monitor->fin_revision();
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
        thread revisora (Revisora,monitor);
        
        
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
        
        revisora.join();	




}
