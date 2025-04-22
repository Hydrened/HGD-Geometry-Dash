CXX = g++
APP_NAME = Geometry_Dash
CPP_VERSION = c++17

BIN_DIR = bin
OBJECT_DIR = $(BIN_DIR)/objects
INCLUDE_DIR = include
LIB_DIR = lib
SRC_DIR = src

CXX_FLAGS = -std=$(CPP_VERSION) -m64 -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/game -I$(INCLUDE_DIR)/$(APP_NAME)
SDL_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_gfx
LD_FLAGS = -L$(LIB_DIR) -lmingw32 $(SDL_FLAGS) -lbase64 -linih -lH2DE

SRC = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJECT_DIR)/%.o, $(SRC))

all:
	make createObjFolder
	make objects -j
	make run

objects: $(OBJ)
	$(CXX) $(CXX_FLAGS) -o $(BIN_DIR)/$(APP_NAME).exe $^ $(LD_FLAGS) $(SDL_FLAGS) $(DLL_FLAG)

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

run:
	cd $(BIN_DIR) && $(APP_NAME)

createObjFolder:
	@if not exist "$(OBJECT_DIR)" mkdir "$(OBJECT_DIR)"
	@for /d %%d in ($(SRC_DIR)\*) do if not exist "$(OBJECT_DIR)/%%d" mkdir "$(OBJECT_DIR)/%%~nxd"

clean:
	@if exist "$(OBJECT_DIR)" rmdir /s /q "$(OBJECT_DIR)"

rebuild: clean all
