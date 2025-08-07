APP_NAME = Geometry_Dash
CPP_VERSION = 20

CXX = g++
BIN_DIR = bin
OBJECT_DIR = $(BIN_DIR)/objects
DATA_DIR = $(BIN_DIR)/data
INCLUDE_DIR = include
LIB_DIR = lib
SRC_DIR = src

CXX_FLAGS = -std=c++$(CPP_VERSION) -m64 -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/game -I$(INCLUDE_DIR)/$(APP_NAME)
SDL_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx
LD_FLAGS = -L$(LIB_DIR) -lmingw32 $(SDL_FLAGS) -lbase64 -lH2DE

SRC = \
    $(wildcard $(SRC_DIR)/*.cpp) \
    $(wildcard $(SRC_DIR)/**/*.cpp) \
    $(wildcard $(SRC_DIR)/**/**/*.cpp)

OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJECT_DIR)/%.o, $(SRC))





all:
	make i
	make l
	make $(BIN_DIR)/$(APP_NAME).exe -j16
	make run

$(BIN_DIR)/$(APP_NAME).exe: $(OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LD_FLAGS)

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXX_FLAGS) -c $< -o $@

createObjFolder:
	@if not exist "$(OBJECT_DIR)" mkdir "$(OBJECT_DIR)"

run:
	cd $(BIN_DIR) && $(APP_NAME)

clean:
	@if exist "$(OBJECT_DIR)" rmdir /s /q "$(OBJECT_DIR)"

rebuild: clean all





data:
	@if exist "$(OBJECT_DIR)/data.o" del "$(OBJECT_DIR)\data.o"
	make

i: 
	make jsonToGdd PATH_ARG=$(DATA_DIR)/items/blocks
	make jsonToGdd PATH_ARG=$(DATA_DIR)/items/triggers

ir: i run

l:
	make jsonToGdd PATH_ARG=$(DATA_DIR)/levels/0
	make jsonToGdd PATH_ARG=$(DATA_DIR)/levels/1

lr: l run

blr: b l run

jsonToGdd:
	$(BIN_DIR)/jsonToGdd/jsonToGdd $(PATH_ARG)
