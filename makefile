.PHONY: run clean run-dev clean-dev

./rate: ./src/main.c
	gcc $^ -o $@

./rate-dev: ./dev/main.c
	gcc $^ -o $@

run: 
	./rate

clean:
	rm rate

run-dev:
	./rate

clean-dev:
	rm ./rate-dev

dev: ./rate

rate: ./rate