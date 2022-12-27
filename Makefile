build:
	g++ server.cpp tcp_server.cpp -o server
	
run:
	./server
	
build-and-run:
	g++ server.cpp tcp_server.cpp -o server && ./server
