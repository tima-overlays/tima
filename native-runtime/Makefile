
TEST_BIN=testing.x

CC=g++
CCFLAGS=-Ishared

SRC=shared/automata.cc \
		shared/executor.cc \
		shared/mailbox.cc \
		shared/tima_nature.cc \
		unix/main.cc \
		unix/unix_tima_nature.cc \
		tests/ex1.cc \
		tests/ex1_semantic.cc


OBJ=$(SRC:.cc=.o)

all: ${TEST_BIN}

${TEST_BIN}: ${OBJ}
	${CC} -o ${TEST_BIN} ${OBJ}

%.o: %.cc
	${CC} -std=c++11 ${CCFLAGS} -c -o $@ $<

clean:
	rm -f ${OBJ}
	rm -f ${TEST_BIN}
