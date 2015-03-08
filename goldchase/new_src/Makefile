all: p_goldchase p_driver

p_goldchase: p_goldchase.cpp goldchase.h
	g++ p_goldchase.cpp -o p_goldchase -L. -lmap -lpanel -lncurses -lpthread -lrt

p_driver: p_driver.cpp
	g++ p_driver.cpp -o p_driver

Map.o: Map.cpp
	g++ -c Map.cpp

clean:
	rm -f Map.o p_goldchase p_driver
