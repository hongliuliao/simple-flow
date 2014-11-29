all: prepare 
	
	
prepare:
	mkdir -p bin/include bin/lib
	g++ -c -I dependency/simple_log/include -I src/agent src/agent/tcp_client.cpp -I dependency/simple_server/include -o bin/tcp_client.o
	g++ -c -I dependency/simple_log/include -I src/agent src/agent/file_reader.cpp -I dependency/simple_server/include -o bin/file_reader.o
	g++ -c -I dependency/simple_log/include -I src/agent src/agent/file_collector.cpp -I dependency/simple_server/include -o bin/file_collector.o
	g++ -c -I dependency/simple_log/include -I src/server src/server/flow_server.cpp -I dependency/simple_server/include -o bin/flow_server.o
	g++ -c -I dependency/simple_log/include -I src/server src/server/flow_handler.cpp -I dependency/simple_server/include -o bin/flow_handler.o
	g++ -c -I dependency/simple_log/include -I src/server -I src/agent -I src src/agent/file_agent.cpp -I dependency/simple_server/include -o bin/file_agent.o
	ar -rcs libflowserver.a bin/*.o
	
	cp src/server/*.h bin/include/
	cp src/agent/*.h bin/include/
	mv libflowserver.a bin/lib/
	rm -rf bin/*.o
	
fileagent:
	mkdir -p bin/include bin/lib
	g++  -I dependency/simple_log/include src/agent/file_reader.cpp src/agent/file_agent.cpp bin/lib/libflowserver.a dependency/simple_log/lib/libsimplelog.a dependency/simple_server/lib/libsimpleserver.a -o bin/file_agent
	
test : 	file_agent_test log_flow_server statistic_flow_server statistic_http_server redis_repl_flow_server file_collector_test

file_collector_test:
	g++ -I src/agent -I dependency/simple_log/include test/agent/file_collector_test.cpp bin/lib/libflowserver.a dependency/simple_log/lib/libsimplelog.a -o bin/file_collector_test

file_agent_test:
	g++ -I dependency/simple_log/include -I bin/include test/agent/file_agent_test.cpp bin/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a -o bin/file_agent_test

log_flow_server: 
	g++ -I dependency/simple_log/include -I bin/include test/server/log_flow_server.cpp bin/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a -o bin/log_flow_server
	
statistic_flow_server:
	g++ -I dependency/simple_log/include -I dependency/hiredis/include -I bin/include test/server/statistic_flow_server.cpp bin/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a  dependency/hiredis/lib/libhiredis.a -o bin/statistic_flow_server

statistic_http_server:
	g++ -I dependency/simple_log/include -I dependency/hiredis/include -I dependency/json-cpp/include -I dependency/simple_server/include -I bin/include test/server/statistic_http_server.cpp bin/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a dependency/json-cpp/lib/libjson_libmt.a dependency/hiredis/lib/libhiredis.a -o bin/statistic_http_server

redis_repl_flow_server:
	g++ -g -I dependency/simple_log/include -I dependency/hiredis/include -I dependency/json-cpp/include -I dependency/simple_server/include -I bin/include test/server/redis_repl_flow_server.cpp bin/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a dependency/json-cpp/lib/libjson_libmt.a dependency/hiredis/lib/libhiredis.a -o bin/redis_repl_flow_server
	
example : log_index
	
log_index:
	g++ -g -I dependency/simple_log/include -I dependency/hiredis/include -I dependency/json-cpp/include -I dependency/simple_server/include -I bin/include example/log_index.cpp example/curl_utils.cpp bin/lib/libflowserver.a dependency/simple_server/lib/libsimpleserver.a dependency/simple_log/lib/libsimplelog.a dependency/json-cpp/lib/libjson_libmt.a dependency/hiredis/lib/libhiredis.a -lcurl -o bin/log_index

clean:
	rm -rf bin/*