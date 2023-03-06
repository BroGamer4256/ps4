project('ps4', 'c', 'cpp', 'nasm', version: '1.0.0')
warning_level = 3
cpp_std = 'c++20'
debug = true

add_project_arguments(
	'-DWIN32_LEAN_AND_MEAN',
	'-D_WIN32_WINNT=_WIN32_WINNT_WIN10',
	'-Ofast', # Speed
	language: 'cpp',
)

add_project_link_arguments(
	'-static',
	language: 'cpp',
)

cmake = import('cmake')
minhook = cmake.subproject('minhook')
subproject('tomlplusplus')

shared_module(
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
		'src/helpers.cpp',
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
