PROG_NAME = main
PROJECT_PATH = Z:/Projects/PopulationEcologySimulation
SOURCE_PATH = $(PROJECT_PATH)/Source

SP_FILES = @main.
FILES =
FLAGS = -std=c++11

ALL_FILES = $(SP_FILES)

SP = $(SOURCE_PATH)
SO = $(SP)/Objects
SM = $(SP)/Managers

SFML_DEBUG_PATH = Z:/SFML-ALL/SFML-2.3-DebugI
DEBUG_PATH = $(PROJECT_PATH)/Debug
DEBUG_LIBS = -lsfml-graphics-d -lsfml-window-d -lsfml-system-d
DEBUG_FLAGS = -Wall -g

SFML_RELEASE_PATH = Z:/SFML-ALL/SFML-2.3-ReleaseI
RELEASE_PATH = $(PROJECT_PATH)/Release
RELEASE_LIBS = -lsfml-graphics -lsfml-window -lsfml-system
RELEASE_FLAGS =

OBJECT_FILES:= $(ALL_FILES)
#path substitution
empty:=
space:= @
OBJECT_FILES:= $(subst $(space),$(empty),$(OBJECT_FILES))
space:= .
OBJECT_FILES:= $(subst $(space),.o,$(OBJECT_FILES))
SOURCE_FILES:= $(subst $(space),.cpp,$(SOURCE_FILES))
	
space:= @
SP_FILES:= $(subst $(space),$(SP)/,$(SP_FILES))
SM_FILES:= $(subst $(space),$(SM)/,$(SM_FILES))
SO_FILES:= $(subst $(space),$(SO)/,$(SO_FILES))
SOURCE_FILES:= $(SP_FILES) $(SM_FILES) $(SO_FILES)
space:= .
SOURCE_FILES:= $(subst $(space),.cpp,$(SOURCE_FILES))

all:
	@echo avaiable options:
	@echo 	===DEBUG===
	@echo 	compiledebug
	@echo 	cpartdebug FILES=example.cpp ...
	@echo 	builddebug
	@echo 	rundebug
	@echo 	===RELEASE===
	@echo 	compilerelease
	@echo 	cpartrelease FILES=example.cpp ...
	@echo 	buildrelease
	@echo 	runrelease

pathhelp:
	@echo "&S" = SOURCE_PATH/
	@echo "&SO" = SOURCE_PATH/Objects/

compiledebug:
	cd ${DEBUG_PATH}/Compiled && g++ ${SOURCE_FILES} -c ${FLAGS} ${DEBUG_FLAGS} -I${SFML_DEBUG_PATH}/include
	
compilerelease:
	cd ${RELEASE_PATH}/Compiled && g++ ${SOURCE_FILES} -c ${FLAGS} ${RELEASE_FLAGS} -I${SFML_RELEASE_PATH}/include

cpartdebug:
	cd ${DEBUG_PATH}/Compiled && g++ ${FILES} -c ${FLAGS} ${DEBUG_FLAGS} -I${SFML_DEBUG_PATH}/include

cpartrelease:
	cd ${RELEASE_PATH}/Compiled && g++ ${FILES} -c ${FLAGS} ${RELEASE_FLAGS} -I${SFML_RELEASE_PATH}/include

builddebug:
	cd ${DEBUG_PATH}/Compiled && g++ ${OBJECT_FILES} -o ${DEBUG_PATH}/Executable/${PROG_NAME} ${FLAGS} ${DEBUG_FLAGS} -I${SFML_DEBUG_PATH}/include -L${SFML_DEBUG_PATH}/lib ${DEBUG_LIBS}

buildrelease:
	cd ${RELEASE_PATH}/Compiled && g++ ${OBJECT_FILES} -o ${RELEASE_PATH}/Executable/${PROG_NAME} ${FLAGS} ${RELEASE_FLAGS} -I${SFML_RELEASE_PATH}/include -L${SFML_RELEASE_PATH}/lib ${RELEASE_LIBS}
	
rundebug:
	${DEBUG_PATH}/Executable/${PROG_NAME}.exe

runrelease:
	${RELEASE_PATH}/Executable/${PROG_NAME}.exe

debugit:
	gdb ${DEBUG_PATH}/Executable/${PROG_NAME}