all: fileagent
	mkdir -p bin/include bin/lib
	g++ -c -I dependency/simple_log/include -I src/server src/server/flow_server.cpp -o bin/flow_server.o
	g++ -c -I dependency/simple_log/include -I src/server src/server/flow_handler.cpp -o bin/flow_handler.o
	ar -rcs libflowserver.a bin/flow_server.o bin/flow_handler.o
	
	cp src/server/*.h bin/include/
	mv libflowserver.a bin/lib/
	rm -rf bin/*.o
	
fileagent:
	mkdir -p bin/include bin/lib
	g++  -I dependency/simple_log/include src/agent/file_agent.cpp dependency/simple_log/lib/libsimplelog.a -o bin/file_agent
	
test: test/server/flow_server_test.cpp
	g++ -I dependency/simple_log/include -I bin/include test/server/flow_server_test.cpp bin/lib/libflowserver.a dependency/simple_log/lib/libsimplelog.a -o bin/flow_server_test
	
test_statistic:
	g++ -I dependency/simple_log/include -I dependency/hiredis/include -I bin/include test/server/flow_test_statistic.cpp bin/lib/libflowserver.a dependency/simple_log/lib/libsimplelog.a  dependency/hiredis/lib/libhiredis.a -o bin/flow_test_statistic

statistic_server:
	g++ -I dependency/simple_log/include -I dependency/hiredis/include -I dependency/json-cpp/include -I dependency/simple_server/include -I bin/include test/server/statistic_server.cpp bin/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a dependency/json-cpp/lib/libjson_libmt.a dependency/hiredis/lib/libhiredis.a -o bin/statistic_server
	