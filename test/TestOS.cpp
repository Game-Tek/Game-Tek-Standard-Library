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
	//GTEST_ASSERT_EQ(font.GlyphMap.size(), 242);
	GTEST_ASSERT_EQ(font.Metadata.UnitsPerEm, 2048);
	GTEST_ASSERT_EQ(font.Metadata.Ascender, 1880);
	GTEST_ASSERT_EQ(font.Metadata.Descender, -469);
	GTEST_ASSERT_EQ(font.Metadata.LineGap, 0);

	GTEST_ASSERT_EQ(font.GetKerning(15, 20), -18);

	{ //a
		auto& a = font.GetGlyph(U'a');
		GTEST_ASSERT_EQ(a.BoundingBox[0].X(), 27);
		GTEST_ASSERT_EQ(a.BoundingBox[0].Y(), -27);
		GTEST_ASSERT_EQ(a.BoundingBox[1].X(), 1134);
		GTEST_ASSERT_EQ(a.BoundingBox[1].Y(), 997);
		GTEST_ASSERT_EQ(a.GlyphIndex, 68);
		GTEST_ASSERT_EQ(a.AdvanceWidth, 1161);
		GTEST_ASSERT_EQ(a.LeftSideBearing, 27);
		GTEST_ASSERT_EQ(a.Contours.GetLength(), 2);
		GTEST_ASSERT_EQ(a.Contours[0].Points.GetLength(), 54);
		GTEST_ASSERT_EQ(a.Contours[1].Points.GetLength(), 11);

		GTEST_ASSERT_EQ(a.Contours[0].Points[0].Position, GTSL::ShortVector(609, 997));
		GTEST_ASSERT_EQ(a.Contours[1].Points[0].Position, GTSL::ShortVector(542, 418));
	}

	{ //A
		auto& A = font.GetGlyph(U'A');
		GTEST_ASSERT_EQ(A.BoundingBox[0].X(), 0);
		GTEST_ASSERT_EQ(A.BoundingBox[0].Y(), -21);
		GTEST_ASSERT_EQ(A.BoundingBox[1].X(), 1693);
		GTEST_ASSERT_EQ(A.BoundingBox[1].Y(), 1399);
		GTEST_ASSERT_EQ(A.GlyphIndex, 36);
		GTEST_ASSERT_EQ(A.AdvanceWidth, 1693);
		GTEST_ASSERT_EQ(A.LeftSideBearing, 0);
		GTEST_ASSERT_EQ(A.Contours.GetLength(), 2);
		GTEST_ASSERT_EQ(A.Contours[0].Points.GetLength(), 58);
		GTEST_ASSERT_EQ(A.Contours[1].Points.GetLength(), 16);

		GTEST_ASSERT_EQ(A.Contours[0].Points[0].Position, GTSL::ShortVector(1332, 691));
		GTEST_ASSERT_EQ(A.Contours[1].Points[0].Position, GTSL::ShortVector(636, 565));
	}

	{ //a tilde
		auto& a = font.GetGlyph(U'á');
		GTEST_ASSERT_EQ(a.BoundingBox[0].X(), 27);
		GTEST_ASSERT_EQ(a.BoundingBox[0].Y(), -27);
		GTEST_ASSERT_EQ(a.BoundingBox[1].X(), 1134);
		GTEST_ASSERT_EQ(a.BoundingBox[1].Y(), 1430);
		GTEST_ASSERT_EQ(a.GlyphIndex, 105);
		GTEST_ASSERT_EQ(a.AdvanceWidth, 1161);
		GTEST_ASSERT_EQ(a.LeftSideBearing, 27);
		GTEST_ASSERT_EQ(a.Contours.GetLength(), 3);
		GTEST_ASSERT_EQ(a.Contours[0].Points.GetLength(), 54);
		GTEST_ASSERT_EQ(a.Contours[1].Points.GetLength(), 11);
		GTEST_ASSERT_EQ(a.Contours[2].Points.GetLength(), 24); //tilde

		GTEST_ASSERT_EQ(a.Contours[0].Points[0].Position, GTSL::ShortVector(609, 997));
		GTEST_ASSERT_EQ(a.Contours[1].Points[0].Position, GTSL::ShortVector(542, 418));
		GTEST_ASSERT_EQ(a.Contours[2].Points[0].Position, GTSL::ShortVector(629 + 40, 1430 + 0));
	}
}