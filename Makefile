OUT = ps4
CC = clang
CXX = clang++
TARGET = x86_64-pc-windows-gnu
SRC = src/mod.cpp src/SigScan.cpp src/helpers.c src/menus.cpp src/diva.cpp tomlc99/toml.c minhook/src/buffer.c minhook/src/hook.c minhook/src/trampoline.c minhook/src/hde/hde32.c minhook/src/hde/hde64.c
OBJ = $(addprefix $(TARGET)/,$(subst .c,.o,$(SRC:.cpp=.o)))
CFLAGS = -g -std=c99 -Iminhook/include -Itomlc99 -Wall -Ofast -target $(TARGET) -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=_WIN32_WINNT_WIN7
CXXFLAGS = -g -std=c++20 -Iminhook/include -Itomlc99 -Wall -Ofast -target $(TARGET) -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=_WIN32_WINNT_WIN7
LDFLAGS = -shared -static -static-libgcc -s -pthread -lgdi32 -ldwmapi

all: options $(OUT)

.PHONY: options
options:
	@echo "CXXFLAGS	= $(CXXFLAGS)"
	@echo "LDFLAGS	= $(LDFLAGS)"
	@echo "CXX	= $(CXX)"

.PHONY: dirs
dirs:
	@mkdir -p $(TARGET)/src
	@mkdir -p $(TARGET)/minhook/src/hde
	@mkdir -p $(TARGET)/tomlc99

$(TARGET)/%.o: %.cpp
	@echo BUILD $<
	@bear -- $(CXX) -c $(CXXFLAGS) $< -o $@

$(TARGET)/%.o: %.c
	@echo BUILD $<
	@bear -- $(CC) -c $(CFLAGS) $< -o $@

.PHONY: $(OUT)
$(OUT): dirs $(OBJ)
	@echo LINK $@
	@$(CXX) $(CXXFLAGS) -o $(TARGET)/$@.dll $(OBJ) $(LDFLAGS)

.PHONY: fmt
fmt:
	@cd src && clang-format -i *.h *.cpp -style=file

.PHONY: clean
clean:
	rm -rf $(TARGET)
