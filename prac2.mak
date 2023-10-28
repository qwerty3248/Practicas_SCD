
all: clean prodcons_mu prodcons_mu_FIFO lectores_escritores fumadores


prodcons_mu:prodcons_mu.cpp
	@g++ -std=c++11 -pthread -o prodcons_mu scd.cpp\
	     prodcons_mu.cpp -I./
	@echo "Listo 1" 


prodcons_mu_FIFO:prodcons_mu_FIFO.cpp
	@g++ -std=c++11 -pthread -o prodcons_mu_FIFO scd.cpp\
	     prodcons_mu_FIFO.cpp -I./ 
	@echo "Listo 2"

lectores_escritores:lectores_escritores.cpp
	@g++ -std=c++11 -pthread -o lectores_escritores scd.cpp\
	     lectores_escritores.cpp -I./ 
	@echo "Listo 3"

fumadores:fumadores.cpp
	@g++ -std=c++11 -pthread -o fumadores scd.cpp\
	     fumadores.cpp -I./ 
	@echo "Listo 4"

clean:
	@rm -rf prodcons_mu_FIFO
	@rm -rf prodcons_mu
	@rm -rf lectores_escritores
	@rm -rf fumadores
