CC= g++ # compiler
CXXFLAGS= -O2 # compiler flags
LDFLAGS= -lcrypto -lssl # linker flags
TARGET= program# executable file name

# executable file directory
BINARY= ./bin/
# source files directory
SOURCE= ./src/

NET=./src/include/net/
API_KIS=./src/include/api/kis/
API_KIS_DOMESTIC=./src/include/api/kis/domestic/
API_KIS_OVERSEAS=./src/include/api/kis/overseas/

# Each directory's object file names
SRC_NAMES := $(patsubst ${SOURCE}%.cpp,${BINARY}%.o,$(wildcard ${SOURCE}*.cpp))
NET_NAMES := $(patsubst ${NET}%.cpp,${BINARY}%.o,$(wildcard ${NET}*.cpp))
AK_NAMES := $(patsubst ${API_KIS}%.cpp,${BINARY}%.o,$(wildcard ${API_KIS}*.cpp))
AKD_NAMES := $(patsubst ${API_KIS_DOMESTIC}%.cpp,${BINARY}domestic_%.o,$(wildcard ${API_KIS_DOMESTIC}*.cpp))
AKS_NAMES := $(patsubst ${API_KIS_OVERSEAS}%.cpp,${BINARY}overseas_%.o,$(wildcard ${API_KIS_OVERSEAS}*.cpp))

# Object file names
OBJS= ${SRC_NAMES} ${NET_NAMES} ${AK_NAMES} ${AKD_NAMES} ${AKS_NAMES}

# make executable file from object files, excutable program file store in this directory
$(BINARY)$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# make object files from source files, object files store in BINARY directory
$(BINARY)%.o: $(SOURCE)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)overseas_%.o: $(API_KIS_OVERSEAS)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)domestic_%.o: $(API_KIS_DOMESTIC)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)%.o: $(API_KIS)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)%.o: $(NET)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

clean:
	rm $(BINARY)*.o
	rm $(BINARY)$(TARGET)
