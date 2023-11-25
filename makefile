build:
	rm -f application.exe
	g++ -std=c++20 -Wall -g application.cpp dist.cpp osm.cpp tinyxml2.cpp -o application.exe

run:
	./application.exe

buildtest:
	rm -f testing.exe
	g++ -std=c++20 -Wall testing.cpp -o testing.exe

runtest:
	./testing.exe

clean:
	rm -f application.exe	

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./application.exe

solution:
	./maps_solution.out

blank:
	rm -f blank.exe
	g++ blank.cpp -g -o blank.exe; ./blank.exe
	