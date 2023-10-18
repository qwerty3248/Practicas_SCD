#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "scd.h"

using namespace std;
using namespace scd;

const int NP = 10;
const int NC = 2;
int n_coches[2] = {0};
Semaphore llena[2] = {0,0};
Semaphore pasa[2] = {1,1};
unsigned impar[10] = {0};
unsigned par[10] = {0};


unsigned aleatorio(unsigned min, unsigned max) {
    static thread_local mt19937 generator(hash<thread::id>{}(this_thread::get_id()));
    uniform_int_distribution<unsigned> distribution(min, max);
    return distribution(generator);
}

void funcion_coche(int i)
{
	int cabina, iter = 0;	
	while(true){
		if(iter % 2 != 0){
			if(i < 5){
				cabina = 0;
			
			}else{
				cabina = 1;
			
			}
		 	
		
		}else{
			
			cabina = aleatorio(0,1);
		
		}
		
		n_coches[cabina]++;
		cout <<n_coches[cabina];
		cout << "Coche " << i << " en cabina " << cabina << endl;
		sem_wait(pasa[cabina]);
		this_thread::sleep_for(chrono::milliseconds(aleatorio(20, 100)));
		cout << "Pago del peaje de " << i << " en cabina " << cabina << endl;
		if(cabina % 2 == 0){
			
			par[i]++;	
				
		
		}else{
			impar[i]++;
		
		}
		
		sem_signal(llena[cabina]);
		iter++;
		
	
	}



}

void funcion_cabina(int i){
	
	while(true){
		
		sem_wait(llena[i]);
		this_thread::sleep_for(chrono::milliseconds(aleatorio<20, 100>()));
		cout<<"Entrando coche en cabina "<<i<<endl;
		sem_signal(pasa[i]);
	
	
	
	}
	

}



int main() {

	    thread coches[NP];
	    thread cabinas[NC];

	    for (int i = 0; i < NP; i++) {
		coches[i] = thread(funcion_coche, i);
	    }

	    for (int i = 0; i < NC; i++) {
		cabinas[i] = thread(funcion_cabina, i);
	    }

	    for (int i = 0; i < NP; i++) {
		coches[i].join();
	    }

	    for (int i = 0; i < NC; i++) {
		cabinas[i].join();
	    }


		
	    return 0;

    
}

