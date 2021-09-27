//123 test

#include <gtest/gtest.h>

#include "GTSL/File.h"
#include "GTSL/DLL.h"
#include "GTSL/MappedFile.hpp"
#include "GTSL/Filesystem.h"
#include "GTSL/Window.h"
#include "GTSL/TTF.hpp"
#include "GTSL/System.h"

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

TEST(FileQuery, Do) {
	GTSL::FileQuery file_query;
	auto res = file_query.DoQuery(u8"*.exe");
	GTEST_ASSERT_EQ(res.Get(), u8"GTSL_Test.exe");
}

TEST(DLL, Construct) {
	GTSL::DLL dll;
}

TEST(Window, Construct) {
	GTSL::Window window;
}

TEST(Console, Print) {
	GTSL::Console::Print(u8"Test print.\n");
}

TEST(Font, Font) {
	GTSL::File file;

#ifdef _WIN32
	auto res = file.Open(u8"C:/Windows/Fonts/COOPBL.TTF", GTSL::File::READ, false);
#endif

	GTSL::Buffer<GTSL::DefaultAllocatorReference> buffer;

	GTSL::Font font{ GTSL::DefaultAllocatorReference() };

	file.Read(buffer);

	auto result = MakeFont(reinterpret_cast<const char*>(buffer.GetData()), &font);

	ASSERT_TRUE(result);

	GTEST_ASSERT_EQ(font.FullFontName, "Cooper Black Normal");
	GTEST_ASSERT_EQ(font.GlyphMap.size(), 242);
	GTEST_ASSERT_EQ(font.Metadata.UnitsPerEm, 2048);
	GTEST_ASSERT_EQ(font.Metadata.Ascender, 1880);
	GTEST_ASSERT_EQ(font.Metadata.Descender, -469);
	GTEST_ASSERT_EQ(font.Metadata.LineGap, 0);

	GTEST_ASSERT_EQ(font.GetKerning(15, 20), -18);

	{ //a
		auto& a = font.GetGlyph(u8'a');
		GTEST_ASSERT_EQ(a.BoundingBox[0].X(), 27);
		GTEST_ASSERT_EQ(a.BoundingBox[0].Y(), -27);
		GTEST_ASSERT_EQ(a.BoundingBox[1].X(), 1134);
		GTEST_ASSERT_EQ(a.BoundingBox[1].Y(), 997);
		GTEST_ASSERT_EQ(a.GlyphIndex, 68);
		GTEST_ASSERT_EQ(a.AdvanceWidth, 1161);
		GTEST_ASSERT_EQ(a.LeftSideBearing, 27);
		GTEST_ASSERT_EQ(a.NumContours, 2);
		GTEST_ASSERT_EQ(a.Paths[0].GetLength(), 32);
		GTEST_ASSERT_EQ(a.Paths[1].GetLength(), 7);
	}

	{ //A
		auto& A = font.GetGlyph(u8'A');
		GTEST_ASSERT_EQ(A.BoundingBox[0].X(), 0);
		GTEST_ASSERT_EQ(A.BoundingBox[0].Y(), -21);
		GTEST_ASSERT_EQ(A.BoundingBox[1].X(), 1693);
		GTEST_ASSERT_EQ(A.BoundingBox[1].Y(), 1399);
		GTEST_ASSERT_EQ(A.GlyphIndex, 36);
		GTEST_ASSERT_EQ(A.AdvanceWidth, 1693);
		GTEST_ASSERT_EQ(A.LeftSideBearing, 0);
		GTEST_ASSERT_EQ(A.NumContours, 2);
		GTEST_ASSERT_EQ(A.Paths[0].GetLength(), 35);
		GTEST_ASSERT_EQ(A.Paths[1].GetLength(), 9);
	}
}