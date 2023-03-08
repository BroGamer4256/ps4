project('ps4', 'c', 'cpp', 'nasm', version: '1.0.0')
warning_level = 3
cpp_std = 'c++20'
debug = true
optimization = 3
b_lto = true
b_pgo = 'use'

cpp = meson.get_compiler('cpp')

add_project_arguments(
	cpp.get_supported_arguments(
		'-D_WIN32_WINNT=_WIN32_WINNT_WIN10',
	),
	language: 'cpp',
)

add_project_link_arguments(
	cpp.get_supported_arguments(
		'-static',
		'-s'
	),
	language: 'cpp',
)

cmake = import('cmake')
minhook = cmake.subproject('minhook')
subproject('tomlplusplus')

library(
	'ps4',
	include_directories: [
		'src',
		'subprojects/tomlplusplus/'
	],
	cpp_pch : 'src/helpers.h',
	dependencies : [
		minhook.get_variable('minhook_dep'),
	],
	sources : [
		'src/mod.cpp',
		'src/SigScan.cpp',
		'src/diva.cpp',
		'src/menu_touch.asm',
		'src/menus/exit.cpp',
		'src/menus/gallery.cpp',
		'src/menus/options.cpp',
		'src/menus/pause.cpp',
		'src/menus/pvSel.cpp'
	],
	name_prefix: ''
)
