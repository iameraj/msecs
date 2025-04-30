
SRC = include/msecs.hpp
BUILD = build

all: ${SRC}
	cmake -B ${BUILD}
	cmake --build ${BUILD}

test: all
	${BUILD}/main-test
