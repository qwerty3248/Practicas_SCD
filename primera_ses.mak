#makefile sesion 1 




all: prodcons-plantilla fumadores-plantilla prodcons-multi



prodcons-plantilla:prodcons-plantilla.cpp scd.cpp
	@g++ -o prodcons-plantilla prodcons-plantilla.cpp scd.cpp -pthread -std=c++11 -I./

	@echo "prodcons-plantilla ha sido creado"

prodcons-multi:prodcons-multi.cpp scd.cpp
	@g++ -o prodcons-multi prodcons-multi.cpp scd.cpp -pthread -std=c++11 -I./

	@echo "prodcons-multi ha sido creado"

fumadores-plantilla:fumadores-plantilla.cpp scd.cpp
	@g++ -o fumadores-plantilla fumadores-plantilla.cpp scd.cpp -pthread -std=c++11 -I./

	@echo "fumadores-plantilla ha sido creado"	

clean:
	@rm -r prodcons-plantilla  
	@rm -r prodcons-multi
	@rm -r fumadores-plantilla



