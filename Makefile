CXX = g++
APP_NAME = Geometry_Dash
CPP_VERSION = c++17

BIN_DIR = bin
OBJECT_DIR = $(BIN_DIR)/objects
DATA_DIR = $(BIN_DIR)/data
INCLUDE_DIR = include
LIB_DIR = lib
SRC_DIR = src

CXX_FLAGS = -std=$(CPP_VERSION) -m64 -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/game -I$(INCLUDE_DIR)/$(APP_NAME)
SDL_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx
LD_FLAGS = -L$(LIB_DIR) -lmingw32 $(SDL_FLAGS) -lbase64 -lH2DE

SRC = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJECT_DIR)/%.o, $(SRC))





all:
	make i
	make l
	make createObjFolder
	make objects -j
	make run

objects: $(OBJ)
	$(CXX) $(CXX_FLAGS) -o $(BIN_DIR)/$(APP_NAME).exe $^ $(LD_FLAGS) $(SDL_FLAGS)

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

run:
	cd $(BIN_DIR) && $(APP_NAME)

createObjFolder:
	@if not exist "$(OBJECT_DIR)" mkdir "$(OBJECT_DIR)"
	@for /d %%d in ($(SRC_DIR)\*) do if not exist "$(OBJECT_DIR)/%%~nxd" mkdir "$(OBJECT_DIR)/%%~nxd"

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
