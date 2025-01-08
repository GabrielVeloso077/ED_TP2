# Diretórios
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -I$(INC_DIR)

# Nome do executável
TARGET = $(BIN_DIR)/TP2.out

# Encontrar todos os arquivos fonte
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Regra principal
all: directories $(TARGET)

# Criar diretórios necessários
directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

# Regra para o executável
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Regra para os objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpar arquivos gerados (somente executáveis na pasta bin)
clean:
	rm -f $(BIN_DIR)/*.out $(OBJ_DIR)/*.o

.PHONY: all clean directories
