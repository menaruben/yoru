CC = clang
CFLAGS = -Wall -Wextra -I../asserts
TARGET = yoru_math_test
SRC = yoru_math_test.c
DEPS = yoru_math.h ../asserts/yoru_asserts.h

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	-./$(TARGET); status=$$?; rm -f $(TARGET); exit $$status

clean:
	rm -f $(TARGET)