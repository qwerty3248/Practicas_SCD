
all: clean prodcons1_su_FIFO prodcons1_su


prodcons1_su_FIFO:prodcons1_su_FIFO.cpp
	@g++ -std=c++11 -pthread -o prodcons1_su_FIFO scd.cpp\
	     prodcons1_su_FIFO.cpp -I./
	@echo "Listo 1" 


prodcons1_su:prodcons1_su.cpp
	@g++ -std=c++11 -pthread -o prodcons1_su scd.cpp\
	     prodcons1_su.cpp -I./ 
	@echo "Listo 2"



clean:
	@rm -rf prodcons1_su_FIFO
	@rm -rf prodcons1_su
