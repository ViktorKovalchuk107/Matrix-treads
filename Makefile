all:
	@g++ -std=c++11 main.cpp -o main
	@./main
clean:
	@rm ./main