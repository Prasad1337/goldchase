p_goldchase: p_goldchase.cpp libmap.a goldchase.h
	g++ p_goldchase.cpp -o p_goldchase -L. -lmap -lpanel -lncurses -lpthread -lrt

libmap.a: Screen.o Map.o
	ar -r libmap.a Screen.o Map.o

Screen.o: Screen.cpp
	g++ -c Screen.cpp

Map.o: Map.cpp
	g++ -c Map.cpp

clean:
	rm -f Screen.o Map.o libmap.a p_goldchase
