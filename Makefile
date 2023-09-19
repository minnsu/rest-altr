CC= g++# compiler
CXXFLAGS= # compiler flags
LDFLAGS= -lcrypto -lssl # linker flags
TARGET= program# executable file name

BINARY= ./bin/# executable file directory
SOURCE= ./src/# source files directory

# get cpp file list from src directory using wildcard and change its pattern by patsubst
OBJ_NAMES := $(patsubst ${SOURCE}%.cpp,${BINARY}%.o,$(wildcard ${SOURCE}*.cpp))
OBJS= ${OBJ_NAMES}

# OBJ_NAMES= # add object files name in here( XXX.o YYY.o ... )
# OBJS= $(addprefix $(BINARY), $(OBJ_NAMES))

# make executable file from object files, excutable program file store in this directory
$(BINARY)$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# make object files from source files, object files store in BINARY directory
$(BINARY)%.o: $(SOURCE)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

clean:
	rm $(BINARY)*.o
	rm $(BINARY)$(TARGET)
