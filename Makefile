INCLUDE_PATH = src/
CPP_FLAGS = -Wall -Wextra -O3 -std=c++17
PTHREAD = -pthread
CC = g++
PISTACHE_CFLAGS = $(shell pkg-config --cflags libpistache)
PISTACHE_LDFLAGS = $(shell pkg-config --libs libpistache)

M_OBJS = bin/models/scanner.o bin/models/arp.o bin/models/host.o bin/models/interface.o \
		bin/models/controller.o bin/utils/string_utils.o bin/utils/thread_utils.o \
		bin/utils/socket_utils.o bin/utils/mac_utils.o bin/routes/api.o

all: bin/netcut

clean:
	rm -rf bin/

bin/netcut: src/main.cpp $(M_OBJS)
	$(CC) $^ -o $@ $(CPP_FLAGS) $(PTHREAD) -I $(INCLUDE_PATH) $(PISTACHE_LDFLAGS)

bin/models/scanner.o: src/models/scanner.cpp
	mkdir -p bin/models
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/models/arp.o: src/models/arp.cpp
	mkdir -p bin/models
	$(CC) $^ -c -o $@ $(CPP_FLAGS) $(PTHREAD) -I $(INCLUDE_PATH)

bin/models/host.o: src/models/host.cpp
	mkdir -p bin/models
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/models/interface.o: src/models/interface.cpp
	mkdir -p bin/models
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/models/controller.o: src/models/controller.cpp
	mkdir -p bin/models
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/string_utils.o: src/utils/string_utils.cpp
	mkdir -p bin/utils/
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/thread_utils.o: src/utils/thread_utils.cpp
	mkdir -p bin/utils/
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/socket_utils.o: src/utils/socket_utils.cpp
	mkdir -p bin/utils/
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/utils/mac_utils.o: src/utils/mac_utils.cpp
	mkdir -p bin/utils/
	$(CC) $^ -c -o $@ $(CPP_FLAGS) -I $(INCLUDE_PATH)

bin/routes/api.o: src/routes/api.cpp
	mkdir -p bin/routes/
	$(CC) $^ -c -o $@ $(CPP_FLAGS) $(PISTACHE_CFLAGS) -I $(INCLUDE_PATH)