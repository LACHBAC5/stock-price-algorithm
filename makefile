all: main.out

main.out: main.o numbers.o
	g++ -o $@ -lpython3.10 $^

%.o: %.cpp
	g++ -o $@ -c $<