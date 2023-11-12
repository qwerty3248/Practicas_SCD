

all: main main2 main3 main4 main5 main6 main7


main: main.cpp ../scd.cpp ../scd.h
	@g++ -pthread -o main main.cpp ../scd.cpp -I./
	@echo "Todo creado con exito =) (main)"

main2: main2.cpp ../scd.cpp ../scd.h
	@g++ -pthread -o main2 main2.cpp ../scd.cpp -I./
	@echo "Todo creado con exito =) (main2)"

main3: main3.cpp ../scd.cpp ../scd.h
	@g++ -pthread -o main3 main3.cpp ../scd.cpp -I./
	@echo "Todo creado con exito =) (main3)"

main4: main4.cpp ../scd.cpp ../scd.h
	@g++ -pthread -o main4 main4.cpp ../scd.cpp -I./
	@echo "Todo creado con exito =) (main4)"
	
main5: main5.cpp ../scd.cpp ../scd.h
	@g++ -pthread -o main5 main5.cpp ../scd.cpp -I./
	@echo "Todo creado con exito =) (main5)"	

main6: main6.cpp ../scd.cpp ../scd.h
	@g++ -pthread -o main6 main6.cpp ../scd.cpp -I./
	@echo "Todo creado con exito =) (main6)"

main7: main7.cpp ../scd.cpp ../scd.h
	@g++ -pthread -o main7 main7.cpp ../scd.cpp -I./
	@echo "Todo creado con exito =) (main7)"
	
clean: 
	@rm -rf main
	@rm -rf main2
	@rm -rf main3 
	@rm -rf main4
	@rm -rf main5	
	@rm -rf main6
	@rm -rf main7
