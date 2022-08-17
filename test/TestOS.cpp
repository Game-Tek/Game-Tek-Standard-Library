//123 test

#include <gtest/gtest.h>

#include "GTSL/File.hpp"
#include "GTSL/DLL.h"
#include "GTSL/MappedFile.hpp"
#include "GTSL/Filesystem.h"
#include "GTSL/Window.h"
#include "GTSL/TTF.hpp"
#include "GTSL/System.h"
#include "GTSL/Thread.hpp"

TEST(File, Construct) {
	GTSL::File file;
}

TEST(File, Open) {
	GTSL::File file;

#ifdef _WIN32
	auto res = file.Open(u8"C:/Windows/Fonts/times.ttf", GTSL::File::READ, false);
	GTEST_ASSERT_EQ(res, GTSL::File::OpenResult::OK);
#endif
}

TEST(File, Write) {
	GTSL::File file(u8"../../../test/WriteFile", GTSL::File::WRITE, true);

	file.Resize(0);

	GTSL::Buffer<GTSL::StaticAllocator<1024>> buffer;

	buffer.AllocateStructure<GTSL::uint64>(32ull);

	file.Write(buffer.GetRange());

	GTEST_ASSERT_EQ(file.GetSize(), 8);

	file.Write(buffer);

	GTEST_ASSERT_EQ(file.GetSize(), 16);
}

TEST(File, Read) {
	GTSL::File file(u8"../../../test/WriteFile", GTSL::File::READ, false);

#ifdef _WIN32
	GTEST_ASSERT_EQ(file.GetSize(), 16);
	GTEST_ASSERT_EQ(file.GetFileHash(), file.GetFileHash());
#endif

	{
		GTSL::Buffer<GTSL::StaticAllocator<1024>> buffer;

		file.Read(buffer, 8);

		GTEST_ASSERT_EQ(buffer.GetSize(), 8);

		file.Read(buffer, 8);

		GTEST_ASSERT_EQ(buffer.GetSize(), 16);

		GTSL::uint64 array[] = { 32ull, 32ull };

		GTEST_ASSERT_EQ(GTSL::Range(16, reinterpret_cast<const byte*>(array)), buffer.GetRange());
	}

	{
		file.SetPointer(0);

		GTSL::Buffer<GTSL::StaticAllocator<1024>> buffer(file);

		GTSL::uint64 array[] = { 32ull, 32ull };

		GTEST_ASSERT_EQ(GTSL::Range(16, reinterpret_cast<const byte*>(array)), buffer.GetRange());
	}
}

TEST(MappedFile, Construct) {
	GTSL::MappedFile mapped_file;
}

TEST(FileQuery, Do) {
	GTSL::FileQuery file_query(u8"*.exe");
	auto res = file_query();
	GTEST_ASSERT_EQ(res.Get(), u8"GTSL_Test.exe");

	res = file_query();
	GTEST_ASSERT_EQ(res.State(), false);
}

TEST(DirectoryQuery, Construct) {
	GTSL::DirectoryQuery directoryQuery(u8".", true, GTSL::DirectoryQuery::CHANGE_FILE_NAME);
	auto result = directoryQuery([](GTSL::StringView file_path, GTSL::DirectoryQuery::FileAction){});
	GTEST_ASSERT_EQ(result, false);
	result = directoryQuery([](GTSL::StringView file_path, GTSL::DirectoryQuery::FileAction){});
	GTEST_ASSERT_EQ(result, false);
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

TEST(Thread, Construct) {
	GTSL::Thread thread;
}

TEST(Font, FontCOOPBL) {
	GTSL::File file;

#ifdef _WIN32
	auto res = file.Open(u8"C:/Windows/Fonts/COOPBL.TTF", GTSL::File::READ, false);
	if (res == GTSL::File::OpenResult::ERROR) { GTEST_SKIP_("Could not open test file."); }
#endif

	GTSL::Buffer<GTSL::DefaultAllocatorReference> buffer;

	GTSL::Font font{ GTSL::DefaultAllocatorReference() };

	file.Read(buffer);

	auto result = MakeFont(buffer.GetRange(), &font);

	ASSERT_TRUE(result);

	GTEST_ASSERT_EQ(font.FullFontName, "Cooper Black Normal");
	//GTEST_ASSERT_EQ(font.GlyphMap.size(), 242);
	GTEST_ASSERT_EQ(font.Metadata.UnitsPerEm, 2048);
	GTEST_ASSERT_EQ(font.Metadata.Ascender, 1880);
	GTEST_ASSERT_EQ(font.Metadata.Descender, -469);
	GTEST_ASSERT_EQ(font.Metadata.LineGap, 0);

	GTEST_ASSERT_EQ(font.GetKerning(U',', U'1'), -18);
	GTEST_ASSERT_EQ(font.GetKerning(U'r', U'q'), -11); //85, 84 : -11
	GTEST_ASSERT_EQ(font.GetKerning(U'Ù', U'A'), -105); //212, 36 : -105

	{ //a
		auto& a = font.GetGlyph(U'a');
		GTEST_ASSERT_EQ(a.Min.X(), 27);
		GTEST_ASSERT_EQ(a.Min.Y(), -27);
		GTEST_ASSERT_EQ(a.Max.X(), 1134);
		GTEST_ASSERT_EQ(a.Max.Y(), 997);
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
		GTEST_ASSERT_EQ(A.Min.X(), 0);
		GTEST_ASSERT_EQ(A.Min.Y(), -21);
		GTEST_ASSERT_EQ(A.Max.X(), 1693);
		GTEST_ASSERT_EQ(A.Max.Y(), 1399);
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
		auto& aTilde = font.GetGlyph(U'á');
		GTEST_ASSERT_EQ(aTilde.Min.X(), 27);
		GTEST_ASSERT_EQ(aTilde.Min.Y(), -27);
		GTEST_ASSERT_EQ(aTilde.Max.X(), 1134);
		GTEST_ASSERT_EQ(aTilde.Max.Y(), 1430);
		GTEST_ASSERT_EQ(aTilde.GlyphIndex, 105);
		GTEST_ASSERT_EQ(aTilde.AdvanceWidth, 1161);
		GTEST_ASSERT_EQ(aTilde.LeftSideBearing, 27);
		GTEST_ASSERT_EQ(aTilde.Contours.GetLength(), 3);
		GTEST_ASSERT_EQ(aTilde.Contours[0].Points.GetLength(), 54);
		GTEST_ASSERT_EQ(aTilde.Contours[1].Points.GetLength(), 11);
		GTEST_ASSERT_EQ(aTilde.Contours[2].Points.GetLength(), 24); //tilde

		GTEST_ASSERT_EQ(aTilde.Contours[0].Points[0].Position, GTSL::ShortVector(609, 997));
		GTEST_ASSERT_EQ(aTilde.Contours[1].Points[0].Position, GTSL::ShortVector(542, 418));
		GTEST_ASSERT_EQ(aTilde.Contours[2].Points[0].Position, GTSL::ShortVector(629 + 40, 1430 + 0));
	}

	{
		auto& a = font.GetGlyph(U'a');

		GTSL::Vector<GTSL::Vector<GTSL::Segment<3>, GTSL::DefaultAllocatorReference>, GTSL::DefaultAllocatorReference> contours;

		GTSL::MakePath(a, &contours);

		const auto& contour = contours[1];

		GTEST_ASSERT_EQ(contour[0].Points[0], GTSL::Vector2(542, 418));
		GTEST_ASSERT_EQ(contour[0].Points[1], GTSL::Vector2(498, 418));
		GTEST_ASSERT_EQ(contour[0].Points[2], GTSL::Vector2(465.5, 382));
		ASSERT_TRUE(contour[0].IsBezierCurve());

		GTEST_ASSERT_EQ(contour[1].Points[0], GTSL::Vector2(465.5, 382));
		GTEST_ASSERT_EQ(contour[1].Points[1], GTSL::Vector2(433, 346));
		GTEST_ASSERT_EQ(contour[1].Points[2], GTSL::Vector2(433, 296));
		ASSERT_TRUE(contour[1].IsBezierCurve());

		GTEST_ASSERT_EQ(contour[2].Points[0], GTSL::Vector2(433, 296));
		GTEST_ASSERT_EQ(contour[2].Points[1], GTSL::Vector2(433, 248));
		GTEST_ASSERT_EQ(contour[2].Points[2], GTSL::Vector2(456, 218.5));
		ASSERT_TRUE(contour[2].IsBezierCurve());

		GTEST_ASSERT_EQ(contour[3].Points[0], GTSL::Vector2(456, 218.5));
		GTEST_ASSERT_EQ(contour[3].Points[1], GTSL::Vector2(479, 189));
		GTEST_ASSERT_EQ(contour[3].Points[2], GTSL::Vector2(517, 189));
		ASSERT_TRUE(contour[3].IsBezierCurve());

		GTEST_ASSERT_EQ(contour[4].Points[0], GTSL::Vector2(517, 189));
		GTEST_ASSERT_EQ(contour[4].Points[1], GTSL::Vector2(597, 189));
		GTEST_ASSERT_EQ(contour[4].Points[2], GTSL::Vector2(597, 324));
		ASSERT_TRUE(contour[4].IsBezierCurve());

		GTEST_ASSERT_EQ(contour[5].Points[0], GTSL::Vector2(597, 324));
		GTEST_ASSERT_EQ(contour[5].Points[1], GTSL::Vector2(597, 379));
		GTEST_ASSERT_EQ(contour[5].Points[2], GTSL::Vector2(585.5, 398));
		ASSERT_TRUE(contour[5].IsBezierCurve());

		GTEST_ASSERT_EQ(contour[6].Points[0], GTSL::Vector2(585.5, 398));
		GTEST_ASSERT_EQ(contour[6].Points[1], GTSL::Vector2(574, 417));
		GTEST_ASSERT_EQ(contour[6].Points[2], GTSL::Vector2(542, 418));
		ASSERT_TRUE(contour[6].IsBezierCurve());
	}
}

TEST(Font, FontARIAL) {
	GTSL::File file;

#ifdef _WIN32
	auto res = file.Open(u8"C:/Windows/Fonts/arial.TTF", GTSL::File::READ, false);
	if (res == GTSL::File::OpenResult::ERROR) { GTEST_SKIP_("Could not open test file."); }
#endif

	GTSL::Buffer<GTSL::DefaultAllocatorReference> buffer;

	GTSL::Font font{ GTSL::DefaultAllocatorReference() };

	file.Read(buffer);

	auto result = MakeFont(buffer.GetRange(), &font);

	{
		auto& a = font.GetGlyph(U'Ë');

		GTSL::Vector<GTSL::Vector<GTSL::Segment<3>, GTSL::DefaultAllocatorReference>, GTSL::DefaultAllocatorReference> contours;

		GTSL::MakePath(a, &contours);

		GTEST_ASSERT_EQ(a.Min.X(), 162);
		GTEST_ASSERT_EQ(a.Min.Y(), 0);
		GTEST_ASSERT_EQ(a.Max.X(), 1256);
		GTEST_ASSERT_EQ(a.Max.Y(), 1761);
		GTEST_ASSERT_EQ(a.GlyphIndex, 201);
		GTEST_ASSERT_EQ(a.AdvanceWidth, 1366);
		GTEST_ASSERT_EQ(a.LeftSideBearing, 162);
		GTEST_ASSERT_EQ(a.Contours.GetLength(), 3);

		//glyph 142 at dx = 364, dy = 286

		const auto& contour = contours[1];

		GTEST_ASSERT_EQ(contour[0].Points[0], GTSL::Vector2(61 + 364, 1270 + 286));
		GTEST_ASSERT_EQ(contour[0].Points[2], GTSL::Vector2(61 + 364, 1475 + 286));
		ASSERT_TRUE(!contour[0].IsBezierCurve());
	}

	ASSERT_TRUE(result);
}