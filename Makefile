all: meson.build
	@ninja -C build

meson.build:
	@meson setup build --cross cross-mingw-64.txt
