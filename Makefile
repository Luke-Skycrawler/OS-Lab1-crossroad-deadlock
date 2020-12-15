default:
	# gcc -lpthread crossroad.cpp main.c philosopher.c -o crossroad
	g++ -lpthread crossroad.cpp main.cpp -o crossroad
	# rm tmp
	./crossroad 10 20
run:
	./crossroad 10 20 >output
