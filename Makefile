.PHONY: all test clean example deps

CXX=g++
CXXFLAGS += -g
LDFLAGS += -pthread

HIREDIS_INC_PATH=-I dependency/hiredis
HIREDIS_LIB_PATH=dependency/hiredis/libhiredis.a

HTTP_PARSER_INC_PATH=-I dependency/ehttp/deps/http-parser/
HTTP_PARSER_LIB_PATH=dependency/ehttp/deps/http-parser/libhttp_parser.a

JSONCPP_INC_PATH=-I dependency/ehttp/deps/json-cpp/include
JSONCPP_LIB_PATH=dependency/ehttp/deps/json-cpp/output/lib/libjson_libmt.a

EHTTP_INC_PATH=-I dependency/ehttp/output/include
EHTTP_LIB_PATH=dependency/ehttp/output/lib/libsimpleserver.a

DEPS_INCLUDE_PATH=$(HIREDIS_INC_PATH) $(HTTP_PARSER_INC_PATH) $(JSONCPP_INC_PATH) $(EHTTP_INC_PATH)
DEPS_LIB_PATH=$(EHTTP_LIB_PATH) $(HIREDIS_LIB_PATH) $(HTTP_PARSER_LIB_PATH) $(JSONCPP_LIB_PATH) 

SRC_INCLUDE_PATH=-I src/agent -I src/server
OUTPUT_INCLUDE_PATH=-I bin/include
OUTPUT_LIB_PATH=bin/lib/libflowserver.a

agent_objects := $(patsubst %.cpp,%.o,$(wildcard src/agent/*.cpp))
server_objects := $(patsubst %.cpp,%.o,$(wildcard src/server/*.cpp))

all: deps prepare libflowserver.a 

deps:
	make -C dependency/ehttp
	make -C dependency/hiredis
	
	
prepare:
	mkdir -p bin/include bin/lib
	g++ -c $(DEPS_INCLUDE_PATH) -I src/agent src/agent/tcp_client.cpp -o bin/tcp_client.o
	g++ -c $(DEPS_INCLUDE_PATH) -I src/agent src/agent/file_reader.cpp -o bin/file_reader.o
	g++ -c $(DEPS_INCLUDE_PATH) -I src/server src/server/flow_server.cpp -o bin/flow_server.o
	g++ -c $(DEPS_INCLUDE_PATH) -I src/server src/server/flow_handler.cpp -o bin/flow_handler.o
	g++ -c $(DEPS_INCLUDE_PATH) -I src/server -I src/agent -I src src/agent/file_agent.cpp -o bin/file_agent.o
	g++ -c $(DEPS_INCLUDE_PATH) -I src/server -I src/agent -I src src/agent/reg_utils.cpp -I dependency/ehttp/output/include -o bin/reg_utils.o
	
	cp src/server/*.h bin/include/
	cp src/agent/*.h bin/include/

libflowserver.a : $(agent_objects) $(server_objects)
	ar -rcs libflowserver.a src/agent/*.o src/server/*.o
	mv libflowserver.a bin/lib/
	rm -rf bin/*.o
 
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(DEPS_INCLUDE_PATH) $(SRC_INCLUDE_PATH) $< -o $@

	
fileagent:
	mkdir -p bin/include bin/lib
	g++ $(LDFLAGS) $(DEPS_INCLUDE_PATH) $(SRC_INCLUDE_PATH) src/agent/file_reader.cpp src/agent/file_agent.cpp bin/lib/libflowserver.a $(DEPS_LIB_PATH) -o bin/file_agent
	
test : 	file_agent_test log_flow_server statistic_flow_server statistic_http_server redis_repl_flow_server 
	g++ $(LDFLAGS) $(DEPS_INCLUDE_PATH) $(SRC_INCLUDE_PATH) -I bin/include test/agent/reg_utils_test.cpp bin/lib/libflowserver.a $(DEPS_LIB_PATH) -o bin/reg_utils_test
	
file_agent_test:
	g++ $(DEPS_INCLUDE_PATH) $(SRC_INCLUDE_PATH) -I bin/include test/agent/file_agent_test.cpp bin/lib/libflowserver.a $(DEPS_LIB_PATH) -o bin/file_agent_test

flow_utils_test: test/agent/flow_utils_test.cpp
	$(CXX) $(CXXFLAGS) $(DEPS_INCLUDE_PATH) $(OUTPUT_INCLUDE_PATH) $< $(OUTPUT_LIB_PATH) $(DEPS_LIB_PATH) -lcurl -lz -lssl -lidn -o bin/$@

log_flow_server: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEPS_INCLUDE_PATH) $(SRC_INCLUDE_PATH) -I bin/include test/server/log_flow_server.cpp bin/lib/libflowserver.a $(DEPS_LIB_PATH) -o bin/log_flow_server
	
statistic_flow_server:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEPS_INCLUDE_PATH) -I bin/include test/server/statistic_flow_server.cpp $(OUTPUT_LIB_PATH) $(DEPS_LIB_PATH) -o bin/statistic_flow_server

statistic_http_server:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEPS_INCLUDE_PATH) -I bin/include test/server/statistic_http_server.cpp $(OUTPUT_LIB_PATH) $(DEPS_LIB_PATH) -lcurl -o bin/statistic_http_server

redis_repl_flow_server:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(DEPS_INCLUDE_PATH) -I bin/include test/server/redis_repl_flow_server.cpp $(OUTPUT_LIB_PATH) $(DEPS_LIB_PATH) -lcurl  -o bin/redis_repl_flow_server
	
example : 
	$(CXX) $(CXXFLAGS) $(DEPS_INCLUDE_PATH) -I bin/include example/curl_utils.cpp example/log_index.cpp $(OUTPUT_LIB_PATH) $(DEPS_LIB_PATH) -lcurl -o bin/log_index

clean:
	rm -rf bin/*
