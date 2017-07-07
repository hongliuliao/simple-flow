.PHONY: all test clean deps tags 

CXX=g++
CXXFLAGS += -g -Wall
LDFLAGS += -pthread

DEPS_INCLUDE_PATH=-I deps/json-cpp/include/ -I deps/http-parser/
DEPS_LIB_PATH=deps/json-cpp/output/lib/libjson_libmt.a deps/http-parser/libhttp_parser.a
SRC_INCLUDE_PATH=-I src
OUTPUT_INCLUDE_PATH=-I output/include
OUTPUT_LIB_PATH=output/lib/libsimpleserver.a

objects := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))

all: prepare deps libsimpleserver.a
	cp src/*.h output/include/
	mv libsimpleserver.a output/lib/

prepare: 
	mkdir -p output/include output/lib output/bin

tags:
	ctags -R /usr/include src deps

deps:
	make -C deps/http-parser package
	make -C deps/json-cpp

libsimpleserver.a: $(objects)
	ar -rcs libsimpleserver.a src/*.o

test: http_server_test http_parser_test

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(DEPS_INCLUDE_PATH) $(SRC_INCLUDE_PATH) $< -o $@

http_server_test: test/http_server_test.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEPS_INCLUDE_PATH) $(OUTPUT_INCLUDE_PATH) $< $(OUTPUT_LIB_PATH) $(DEPS_LIB_PATH) -o output/bin/$@

http_parser_test: test/http_parser_test.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEPS_INCLUDE_PATH) $(OUTPUT_INCLUDE_PATH) $< $(OUTPUT_LIB_PATH) $(DEPS_LIB_PATH) -o output/bin/$@

clean:
	rm -rf src/*.o
	rm -rf output/*
