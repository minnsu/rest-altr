CC= g++# compiler
CXXFLAGS= -O2# compiler flags
LDFLAGS= -lcrypto -lssl -lsqlite3 # linker flags
TARGET= program# executable file name

# executable file directory
BINARY= ./bin/
# source files directory
SOURCE= ./src/

NET=./src/include/net/

API_KIS=./src/include/api/kis/
API_KIS_DOMESTIC=${API_KIS}domestic/
API_KIS_OVERSEAS=${API_KIS}overseas/

RUNTIME=./src/include/runtime/
RT_BACKTEST=${RUNTIME}backtest/
RT_ENGINE=${RUNTIME}engine/

SERIES_CALC=./src/include/series_calc/

# Each directory's object file names
SRC_NAMES := $(patsubst ${SOURCE}%.cpp,${BINARY}%.o,$(wildcard ${SOURCE}*.cpp))
NET_NAMES := $(patsubst ${NET}%.cpp,${BINARY}%.o,$(wildcard ${NET}*.cpp))

AK_NAMES := $(patsubst ${API_KIS}%.cpp,${BINARY}%.o,$(wildcard ${API_KIS}*.cpp))
AKD_NAMES := $(patsubst ${API_KIS_DOMESTIC}%.cpp,${BINARY}domestic_%.o,$(wildcard ${API_KIS_DOMESTIC}*.cpp))
AKS_NAMES := $(patsubst ${API_KIS_OVERSEAS}%.cpp,${BINARY}overseas_%.o,$(wildcard ${API_KIS_OVERSEAS}*.cpp))

RT_NAMES := $(patsubst ${RUNTIME}%.cpp,${BINARY}%.o,$(wildcard ${RUNTIME}*.cpp))
RTB_NAMES := $(patsubst ${RT_BACKTEST}%.cpp,${BINARY}runtime_backtest_%.o,$(wildcard ${RT_BACKTEST}*.cpp))
RTE_NAMES := $(patsubst ${RT_ENGINE}%.cpp,${BINARY}runtime_engine_%.o,$(wildcard ${RT_ENGINE}*.cpp))

SERIES_CALC_NAMES := $(patsubst ${SERIES_CALC}%.cpp,${BINARY}%.o,$(wildcard ${SERIES_CALC}*.cpp))

# Object file names
OBJS= ${SRC_NAMES} ${NET_NAMES} ${AK_NAMES} ${AKD_NAMES} ${AKS_NAMES} ${RT_NAMES} ${RTB_NAMES} ${RTE_NAMES} ${SERIES_CALC_NAMES}

# make executable file from object files, excutable program file store in this directory
$(BINARY)$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# make object files from source files, object files store in BINARY directory
$(BINARY)%.o: $(SOURCE)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

# Network source files compile zone.
$(BINARY)%.o: $(NET)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

# API source files compile zone.
$(BINARY)overseas_%.o: $(API_KIS_OVERSEAS)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)domestic_%.o: $(API_KIS_DOMESTIC)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)%.o: $(API_KIS)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

# Runtime source files compile zone.
$(BINARY)runtime_engine_%.o: $(RT_ENGINE)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)runtime_backtest_%.o: $(RT_BACKTEST)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(BINARY)%.o: $(RUNTIME)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)

# Series calculation files compile zone.
$(BINARY)%.o: $(SERIES_CALC)%.cpp
	$(CC) -c $< -o $@ $(CXXFLAGS)




clean:
	rm $(BINARY)*.o
	rm $(BINARY)$(TARGET)
