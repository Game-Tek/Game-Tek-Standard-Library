//123 test

#include <gtest/gtest.h>

#include "GTSL/File.h"
#include "GTSL/DLL.h"
#include "GTSL/MappedFile.hpp"
#include "GTSL/Filesystem.h"
#include "GTSL/Window.h"

TEST(File, Construct) {
	GTSL::File file;
}

TEST(File, Open) {
	GTSL::File file;

#ifdef _WIN32
	auto res = file.Open(u8"C:/Windows/Fonts/times.ttf", GTSL::File::READ, false);
#endif
}

TEST(MappedFile, Construct) {
	GTSL::MappedFile mapped_file;
}

TEST(FileQuery, Construct) {
	GTSL::FileQuery file_query;
}

TEST(DLL, Construct) {
	GTSL::DLL dll;
}

TEST(Window, Construct) {
	GTSL::Window window;
}