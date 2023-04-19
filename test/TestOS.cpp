//123 test

#include <gtest/gtest.h>

#include "GTSL/File.hpp"
#include "GTSL/DLL.h"
#include "GTSL/MappedFile.hpp"
#include "GTSL/Filesystem.hpp"
#include "GTSL/Window.hpp"
#include "GTSL/TTF.hpp"
#include "GTSL/System.hpp"
#include "GTSL/Thread.hpp"
#include "GTSL/Delegate.hpp"

#include "GTSL/Mutex.h"
#include "GTSL/ConditionVariable.h"
#include "GTSL/Atomic.hpp"
#include "GTSL/Semaphore.h"

TEST(File, Construct) {
	GTSL::File file;
}

TEST(File, Open) {
	GTSL::File file;

	auto res = file.Open(u8"./test/COOPBL.TTF", GTSL::File::READ, false);

	ASSERT_TRUE(file);
	GTEST_ASSERT_EQ(res, GTSL::File::OpenResult::OK);
}

TEST(File, Write) {
	GTSL::File file(u8"./test/WriteFile", GTSL::File::WRITE, true);
#if _WIN64
#elif __linux__
	//GTSL::File file(u8"../test/WriteFile", GTSL::File::WRITE, true);
#endif

	file.Resize(0);

	GTSL::Buffer<GTSL::StaticAllocator<1024>> buffer;

	buffer.AllocateStructure<GTSL::uint64>(32ull);

	file.Write(buffer.GetRange());

	GTEST_ASSERT_EQ(file.GetSize(), 8);

	file.Write(buffer);

	GTEST_ASSERT_EQ(file.GetSize(), 16);
}

TEST(File, Read) {

	GTSL::File file(u8"./test/WriteFile", GTSL::File::READ, false);
#ifdef _WIN32
#elif __linux__
	//GTSL::File file(u8"../test/WriteFile", GTSL::File::READ, false);
#endif

	GTEST_ASSERT_EQ(file.GetSize(), 16);
	GTEST_ASSERT_EQ(file.GetFileHash(), file.GetFileHash());

	{
		GTSL::Buffer<GTSL::StaticAllocator<1024>> buffer;

		file.Read(buffer, 8);

		GTEST_ASSERT_EQ(buffer.GetSize(), 8);

		file.Read(buffer, 8);

		GTEST_ASSERT_EQ(buffer.GetSize(), 16);

		GTSL::uint64 array[] = { 32ull, 32ull };

		GTEST_ASSERT_EQ(GTSL::Range(16, reinterpret_cast<const GTSL::byte*>(array)), buffer.GetRange());
	}

	{
		file.SetPointer(0);

		GTSL::Buffer<GTSL::StaticAllocator<1024>> buffer(file);

		GTSL::uint64 array[] = { 32ull, 32ull };

		GTEST_ASSERT_EQ(GTSL::Range(16, reinterpret_cast<const GTSL::byte*>(array)), buffer.GetRange());
	}
}

TEST(File, ReadRaw) {
	GTSL::File file(u8"./test/WriteFile", GTSL::File::READ, false);

	GTSL::byte buffer[16];

	auto bytesRead = file.Read(16, buffer);

	GTEST_ASSERT_EQ(bytesRead, 16);

	GTSL::uint64 array[] = { 32ull, 32ull };
	GTEST_ASSERT_EQ(GTSL::Range<const GTSL::byte*>(16, reinterpret_cast<const GTSL::byte*>(array)), GTSL::Range<const GTSL::byte*>(16, buffer));
}

TEST(MappedFile, Construct) {
	GTSL::MappedFile mapped_file;
}

TEST(FileQuery, Do) {
	GTSL::FileQuery file_query(u8"./test/*.cube");

	auto res = file_query();
	GTEST_ASSERT_EQ(res.Get(), u8"Kodak Ektachrome 64.cube");

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
	dll.LoadLibrary(u8"");
	void(*function)() = nullptr;
	dll.LoadDynamicFunction(u8"none", &function);

	GTEST_ASSERT_EQ(function, nullptr);
}

TEST(Window, Construct) {
	GTSL::Window window;

	auto onEvent = [](GTSL::Window*, void*, GTSL::Window::WindowEvents, void*) {
		ASSERT_FALSE(true);
	};

	window.Initialize(u8"GTSLTestWindow", u8"Hello", GTSL::Window::API::XCB, { 1280, 720 }, nullptr, GTSL::Window::WindowDelegate::Create(onEvent), nullptr);

	if (window.GetXCBConnection() == nullptr) {
		GTEST_SKIP() << "XCB not supported.";
	}

	window.SetWindowVisibility(true);

	auto extent = window.GetFramebufferExtent();

	GTEST_ASSERT_EQ(extent, GTSL::Extent2D(1280, 720));
}

TEST(Console, Print) {
	GTSL::Console::Print(u8"Test print. 😁\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::RED);
	GTSL::Console::Print(u8"Red\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::GREEN);
	GTSL::Console::Print(u8"Green\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::BLUE);
	GTSL::Console::Print(u8"Blue\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::YELLOW);
	GTSL::Console::Print(u8"Yellow\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::MAGENTA);
	GTSL::Console::Print(u8"Magenta\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::CYAN);
	GTSL::Console::Print(u8"Cyan\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::WHITE);
	GTSL::Console::Print(u8"White\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::BLACK);
	GTSL::Console::Print(u8"Black\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::ORANGE);
	GTSL::Console::Print(u8"Orange\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::GRAY);
	GTSL::Console::Print(u8"Gray\n");

	GTSL::Console::SetTextColor(GTSL::Console::TextColor::RESET);
	GTSL::Console::Print(u8"Reset\n");

	GTSL::Console::SetTextEffect(GTSL::Console::TextEffect::BOLD);
	GTSL::Console::Print(u8"Bold\n");
	GTSL::Console::SetTextEffect(GTSL::Console::TextEffect::RESET);	

	GTSL::Console::SetTextEffect(GTSL::Console::TextEffect::UNDERLINE);
	GTSL::Console::Print(u8"Underline\n");

	GTSL::Console::SetTextEffect(GTSL::Console::TextEffect::BLINK);
	GTSL::Console::Print(u8"Blink\n");

	GTSL::Console::SetTextEffect(GTSL::Console::TextEffect::CROSS);
	GTSL::Console::Print(u8"Cross\n");

	GTSL::Console::SetTextEffect(GTSL::Console::TextEffect::INVERT);
	GTSL::Console::Print(u8"Invert\n");

	GTSL::Console::SetTextEffect(GTSL::Console::TextEffect::RESET);
	GTSL::Console::Print(u8"Reset\n");
}

TEST(Thread, Construct) {
	GTSL::uint64 value = 0;

	auto increment = [](GTSL::uint64* value) {
		(*value) += 4;
	};

	GTSL::Thread thread(GTSL::DefaultAllocatorReference{}, 0, GTSL::Delegate<void(GTSL::uint64*)>::Create(increment), &value);

	ASSERT_TRUE(thread);

	thread.Join(GTSL::DefaultAllocatorReference{});

	GTEST_ASSERT_EQ(value, 4);
}

TEST(Font, FontCOOPBL) {
	GTSL::File file(u8"./test/COOPBL.TTF", GTSL::File::READ, false);

	if (!file) { 
		GTEST_SKIP_("Could not open test file.");
	}

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
	//GTEST_ASSERT_EQ(font.GetKerning(U'�', U'A'), -105); //212, 36 : -105

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

TEST(Mutex, Create) {
	GTSL::Mutex mutex;
	mutex.Lock();
	mutex.Unlock();

	auto tryResult = mutex.TryLock();

	ASSERT_TRUE(tryResult);

	mutex.Unlock();
}

TEST(ConditionVariable, Create) {
	GTSL::ConditionVariable conditionVariable;

	conditionVariable.NotifyAll();
	conditionVariable.NotifyOne();
}

TEST(Atomic, Create) {
	GTSL::Atomic<GTSL::uint32> atomic;

	++atomic;

	GTEST_ASSERT_EQ(atomic, 1u);
}

TEST(RWMutex, Create) {
	GTSL::ReadWriteMutex rwmutex;

	rwmutex.ReadLock();
	rwmutex.ReadUnlock();

	rwmutex.WriteLock();
	rwmutex.WriteUnlock();
}

TEST(Semaphore, Semaphore) {
	GTSL::Semaphore semaphore;
	semaphore.Wait();
}

TEST(OS, Path) {
	auto path = GTSL::Application::GetPathToExecutable();

	ASSERT_TRUE(GTSL::IsIn(path, u8"GTSL_Test"));
}

TEST(OS, ThreadCount) {
	auto tCount = GTSL::Application::ThreadCount();

	GTEST_ASSERT_NE(tCount, 0);
}

TEST(OS, SystemInfo) {
	auto systemInfo = GTSL::System::GetSystemInfo();
}