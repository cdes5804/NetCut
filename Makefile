INCLUDE_PATH = src/
CPP_FLAGS = -Wall -Wextra -O2 -std=c++17
PTHREAD = -pthread

M_OBJS = bin/models/scanner.o bin/models/arp.o bin/models/host.o bin/models/interface.o bin/models/controller.o bin/utils/string_utils.o bin/utils/thread_utils.o bin/utils/socket_utils.o bin/utils/mac_utils.o

all: bin/main

clean:
	rm -rf bin/

bin/main: src/main.cpp $(M_OBJS) 
	g++ $^ -o $@ $(CPP_FLAGS) $(PTHREAD) -I $(INCLUDE_PATH)

bin/models/scanner.o: src/models/scanner.cpp
	mkdir -p bin/models
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/models/arp.o: src/models/arp.cpp
	mkdir -p bin/models
	g++ $^ -c -o $@ $(CPP_FLAGS) $(PTHREAD) -I $(INCLUDE_PATH)

bin/models/host.o: src/models/host.cpp
	mkdir -p bin/models
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/models/interface.o: src/models/interface.cpp
	mkdir -p bin/models
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/models/controller.o: src/models/controller.cpp
	mkdir -p bin/models
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/string_utils.o: src/utils/string_utils.cpp
	mkdir -p bin/utils/
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/thread_utils.o: src/utils/thread_utils.cpp
	mkdir -p bin/utils/
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/socket_utils.o: src/utils/socket_utils.cpp
	mkdir -p bin/utils/
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/mac_utils.o: src/utils/mac_utils.cpp
	mkdir -p bin/utils/
	g++ $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)