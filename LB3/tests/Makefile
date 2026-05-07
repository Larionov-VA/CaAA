alglog: ../src/algo
	@g++ ../src/algo.cpp -O3 -o ../src/algo

alg: ../src/algo
	@g++ ../src/algo.cpp -O3 -DLOGGING -o ../src/algo

tests: ../src/algo
	@echo "Запуск тестов"
	@echo
	@make --no-print-directory t1
	@make --no-print-directory t2
	@make --no-print-directory t3
	@make --no-print-directory t4
	@make --no-print-directory t5
t1: alg
	@echo "Тест 1: одна замена"
	@echo "Ввод:"
	@cat test1.txt
	@echo "Вывод:"
	@../src/algo < test1.txt
	@echo
t2: alg
	@echo "Тест 2: одно удаление"
	@echo "Ввод:"
	@cat test2.txt
	@echo "Вывод:"
	@../src/algo < test2.txt
	@echo
t3: alg
	@echo "Тест 3: однa вставка"
	@echo "Ввод:"
	@cat test3.txt
	@echo "Вывод:"
	@../src/algo < test3.txt
	@echo
t4: alg
	@echo "Тест 4: одинаковые строки"
	@echo "Ввод:"
	@cat test4.txt
	@echo "Вывод:"
	@../src/algo < test4.txt
	@echo
t5: alg
	@echo "Тест 5: случайные строки разного размера"
	@echo "Ввод: длина строки 1"
	@python3 ../src/generate_two_string.py 1 > test_random.txt
	@echo "a 1 a 1" >> test_random.txt
	@cat test_random.txt
	@echo "Вывод:"
	@../src/algo < test_random.txt
	@echo "Ввод: длина строки 5"
	@python3 ../src/generate_two_string.py 5 > test_random.txt
	@echo "a 1 a 1" >> test_random.txt
	@cat test_random.txt
	@echo "Вывод:"
	@../src/algo < test_random.txt
	@echo "Ввод: длина строки 20"
	@python3 ../src/generate_two_string.py 20 > test_random.txt
	@echo "a 1 a 1" >> test_random.txt
	@cat test_random.txt
	@echo "Вывод:"
	@../src/algo < test_random.txt
	@echo "Ввод: длина строки 40"
	@python3 ../src/generate_two_string.py 40 > test_random.txt
	@echo "a 1 a 1" >> test_random.txt
	@cat test_random.txt
	@echo "Вывод:"
	@../src/algo < test_random.txt
	@echo