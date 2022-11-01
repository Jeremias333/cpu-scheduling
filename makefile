.PHONY: run clean run-dev clean-dev

./rate: ./src/main.c
	gcc $^ -o $@

./rate-dev: ./dev/main.c
	gcc $^ -o $@

./test_struct: ./dev/test_struct.c
	gcc $^ -o $@

./test_get_infos: ./dev/test_get_infos.c
	gcc $^ -o $@

./test_calc: ./dev/test_calc.c
	gcc $^ -o $@

run: 
	./rate

clean:
	rm rate

run-dev:
	./rate

clean-dev:
	rm ./rate-dev

dev: ./rate-dev

rate: ./rate

test: ./test_struct ./test_get_infos ./test_calc