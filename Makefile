
CROSS=x86_64-w64-mingw32-
TARGET_LIB=winapi



INSTALL_PATH=/media/sf_share/

# Use of pattern rule : "%.x : %.y" generic

all: lib tests

lib: obj/time_measure.o obj/serial.o
	$(CROSS)ar rcs bin/lib$(TARGET_LIB).a $^

tests: lib
	$(CROSS)gcc -o bin/time_measure.exe -static test/test_measure.c -I inc -L bin -l$(TARGET_LIB)
	$(CROSS)gcc -o bin/test_serial.exe  -static test/test_serial.c  -I inc -L bin -l$(TARGET_LIB)

obj/%.o: src/%.c  | bin obj
	$(CROSS)gcc -c $< -Wall -Wextra -O2  -o $@ -I inc

install: all
	cp bin/time_measure.exe bin/test_serial.exe $(INSTALL_PATH)

# create folder if needed
obj:
	mkdir -p $@

bin:
	mkdir -p $@

clean:
	rm -fr obj bin

