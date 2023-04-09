project('GTSL', 'cpp', version: '0.0.1', license: 'MIT', default_options: ['cpp_std=c++20'])

gtest_dep = dependency('gtest', fallback: ['gtest', 'gtest_main_dep'], main: true, required: true)
x11 = dependency('x11', required: true)
xcb = dependency('xcb', required: true)
xcb_keysyms = dependency('xcb-keysyms', required: true)

test_dependencies = [
	'test/TestMain.cpp',
	'test/TestVector.cpp',
	'test/TestFixedVector.cpp',
	'test/TestMath.cpp',
	'test/TestMemory.cpp',
	'test/TestString.cpp',
	'test/TestShortString.cpp',
	'test/TestHashMap.cpp',
	'test/TestUnicode.cpp',
	'test/TestRange.cpp',
	'test/TestOS.cpp',
	'test/TestParsers.cpp',
	'test/TestTree.cpp',
	'test/TestAlgorithm.cpp',
	'test/TestStringCommon.cpp',
	'test/TestBuffer.cpp',
	'test/TestCollision.cpp',
	'test/TestIndirectionTable.cpp',
	'test/TestRatioUnit.cpp',
	'test/TestSmartPointer.cpp'
]

e = executable('GTSL_Test', test_dependencies, dependencies: [gtest_dep, x11, xcb, xcb_keysyms], cpp_args : ['-mavx'], install: true, include_directories: include_directories('GTSL'))

test('GTSL', e)