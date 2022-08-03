OUT = ps4
CXX := clang++
TARGET := x86_64-pc-windows-gnu
SRC = src/dllmain.cpp src/SigScan.cpp minhook/src/buffer.c minhook/src/hook.c minhook/src/trampoline.c minhook/src/hde/hde32.c minhook/src/hde/hde64.c
OBJ = ${addprefix ${TARGET}/,${subst .c,.o,${SRC:.cpp=.o}}}
CXXFLAGS = -std=c++20 -Iminhook/include -Isteam -Wall -Ofast -target ${TARGET} -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=_WIN32_WINNT_WIN7
LDFLAGS := -shared -static -static-libgcc -s -pthread -lgdi32 -ldwmapi steam/steam_api64.lib

all: options ${OUT}

.PHONY: options
options:
	@echo "CXXFLAGS	= ${CXXFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CXX	= ${CXX}"

.PHONY: dirs
dirs:
	@mkdir -p ${TARGET}/src
	@mkdir -p ${TARGET}/minhook/src/hde

${TARGET}/%.o: %.cpp
	@echo BUILD $@
	@bear -- ${CXX} -c ${CXXFLAGS} $< -o $@

${TARGET}/%.o: %.c
	@echo BUILD $@
	@bear -- ${CXX} -c ${CXXFLAGS} $< -o $@

.PHONY: ${OUT}
${OUT}: dirs ${OBJ}
	@echo LINK $@
	@${CXX} ${CXXFLAGS} -o ${TARGET}/$@.dll ${OBJ} ${LDFLAGS} ${LIBS}

.PHONY: fmt
fmt:
	@cd src && clang-format -i *.h *.cpp -style=file

.PHONY: clean
clean:
	rm -rf ${TARGET}
