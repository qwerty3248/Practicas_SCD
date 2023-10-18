
all: clean prodcons_mu prodcons_mu_FIFO


prodcons_mu:prodcons_mu.cpp
	@g++ -std=c++11 -pthread -o prodcons_mu scd.cpp\
	     prodcons_mu.cpp -I./
	@echo "Listo 1" 


prodcons_mu_FIFO:prodcons_mu_FIFO.cpp
	@g++ -std=c++11 -pthread -o prodcons_mu_FIFO scd.cpp\
	     prodcons_mu_FIFO.cpp -I./ 
	@echo "Listo 2"



clean:
	@rm -rf prodcons_mu_FIFO
	@rm -rf prodcons_mu
