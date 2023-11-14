SRC = .
BIN = .
CPP_FILES = $(wildcard $(SRC)/*.cpp)
BINARIES  = $(patsubst $(SRC)/%.cpp, $(BIN)/%, $(CPP_FILES))
SCD 	  = libscd
COMP	  = g++
FLAGS 	  = -std=c++11 -pthread -Wfatal-errors

all: $(BINARIES)
	@echo "[+] todo compilado!"

%: %.cpp $(SCD)/scd.o $(SCD)/scd.h
	@echo "[+] compilando $@..."
	@$(COMP) $(FLAGS) -o $@ $^ -I$(SCD)

$(SCD)/scd.o: $(SCD)/scd.cpp
	@echo "[+] compilando librería de SCD..."
	@$(COMP) $(FLAGS) -c $^ -o $@ -I$(SCD)

clean:
	@echo "[-] limpiando..."
	@rm -rf $(BINARIES) $(SCD)/*.o
