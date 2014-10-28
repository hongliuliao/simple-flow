all: fileagent
	mkdir -p bin/include bin/lib
	g++ -c -I dependency/simple_log/include -I src/server src/server/flow_server.cpp -o bin/flow_server.o
	ar -rcs libflowserver.a bin/flow_server.o
	
	cp src/server/*.h bin/include/
	mv libflowserver.a bin/lib/
	rm -rf bin/*.o
	
fileagent:
	mkdir -p bin/include bin/lib
	g++  -I dependency/simple_log/include src/agent/file_agent.cpp dependency/simple_log/lib/libsimplelog.a -o bin/file_agent
	
test: test/server/flow_server_test.cpp
	g++ -I dependency/simple_log/include -I bin/include test/server/flow_server_test.cpp bin/lib/libflowserver.a dependency/simple_log/lib/libsimplelog.a -o bin/flow_server_test