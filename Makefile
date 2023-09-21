CC= g++# compiler
CXXFLAGS= # compiler flags
LDFLAGS= -lcrypto -lssl # linker flags
TARGET= program# executable file name

# executable file directory
BINARY= ./bin/
# source files directory
SOURCE= ./src/
API_KIS=./src/include/api/kis/
API_KIS_DOMESTIC=./src/include/api/kis/domestic/
NET=./src/include/net/

SRC_NAMES := $(patsubst ${SOURCE}%.cpp,${BINARY}%.o,$(wildcard ${SOURCE}*.cpp))
AK_NAMES := $(patsubst ${API_KIS}%.cpp,${BINARY}%.o,$(wildcard ${API_KIS}*.cpp))
AKD_NAMES := $(patsubst ${API_KIS_DOMESTIC}%.cpp,${BINARY}%.o,$(wildcard ${API_KIS_DOMESTIC}*.cpp))
NET_NAMES := $(patsubst ${NET}%.cpp,${BINARY}%.o,$(wildcard ${NET}*.cpp))

OBJS= ${SRC_NAMES} ${AKD_NAMES} ${AK_NAMES} ${NET_NAMES}

# OBJ_NAMES= # add object files name in here( XXX.o YYY.o ... )
# OBJS= $(addprefix $(BINARY), $(OBJ_NAMES))

# make executable file from object files, excutable program file store in this directory
$(BINARY)$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# make object files from source files, object files store in BINARY directory
$(BINARY)%.o: $(SOURCE)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)%.o: $(API_KIS_DOMESTIC)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)%.o: $(API_KIS)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)%.o: $(NET)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

clean:
	rm $(BINARY)*.o
	rm $(BINARY)$(TARGET)
