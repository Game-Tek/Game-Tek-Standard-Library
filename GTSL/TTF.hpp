#pragma once

#include "Core.h"

#include <map>
#include <unordered_map>
#include <vector>

#include "HashMap.hpp"

/*
* ttf-parser
*  A single header ttf parser
*  Reads the minimum information needed to render antialiased glyph geometry as fast as possible
*  Browser support using emscripten
*
*  A glyph is represented as a set of triangles (p_x, p1, p2) where p_x is the center of the glyph and
*  p1 and p2 are sequential points on the curve. Quadratic splines will have 2 tiangles associated with them,
*  (p_x, p1, p2) as before and (p1, p_c, p2) where p_c is the spline control point.
*
*  author: Kaushik Viswanathan <kaushik@ocutex.com>
*  https://github.com/kv01/ttf-parser
*/

namespace GTSL {
	struct ShortVector
	{
		int16 X, Y;
	};

	inline void get(int64* dst, const char* src, uint32& offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[offset + 7];
			reinterpret_cast<uint8_t*>(dst)[1] = src[offset + 6];
			reinterpret_cast<uint8_t*>(dst)[2] = src[offset + 5];
			reinterpret_cast<uint8_t*>(dst)[3] = src[offset + 4];
			reinterpret_cast<uint8_t*>(dst)[4] = src[offset + 3];
			reinterpret_cast<uint8_t*>(dst)[5] = src[offset + 2];
			reinterpret_cast<uint8_t*>(dst)[6] = src[offset + 1];
			reinterpret_cast<uint8_t*>(dst)[7] = src[offset + 0];
		}
		else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
			reinterpret_cast<uint8_t*>(dst)[4] = src[4];
			reinterpret_cast<uint8_t*>(dst)[5] = src[5];
			reinterpret_cast<uint8_t*>(dst)[6] = src[6];
			reinterpret_cast<uint8_t*>(dst)[7] = src[7];
		}
	
		offset += 8;
	}

	inline void get(uint64* dst, const char* src, uint32& offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[offset + 7];
			reinterpret_cast<uint8_t*>(dst)[1] = src[offset + 6];
			reinterpret_cast<uint8_t*>(dst)[2] = src[offset + 5];
			reinterpret_cast<uint8_t*>(dst)[3] = src[offset + 4];
			reinterpret_cast<uint8_t*>(dst)[4] = src[offset + 3];
			reinterpret_cast<uint8_t*>(dst)[5] = src[offset + 2];
			reinterpret_cast<uint8_t*>(dst)[6] = src[offset + 1];
			reinterpret_cast<uint8_t*>(dst)[7] = src[offset + 0];
		}
		else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
			reinterpret_cast<uint8_t*>(dst)[4] = src[4];
			reinterpret_cast<uint8_t*>(dst)[5] = src[5];
			reinterpret_cast<uint8_t*>(dst)[6] = src[6];
			reinterpret_cast<uint8_t*>(dst)[7] = src[7];
		}
	
		offset += 8;
	}

	inline void get(float32* dst, const char* src, uint32& offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[offset + 3];
			reinterpret_cast<uint8_t*>(dst)[1] = src[offset + 2];
			reinterpret_cast<uint8_t*>(dst)[2] = src[offset + 1];
			reinterpret_cast<uint8_t*>(dst)[3] = src[offset + 0];
		}
		else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
		}
	
		offset += 4;
	}

	inline void get(uint32* dst, const char* src, uint32& offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[offset + 3];
			reinterpret_cast<uint8_t*>(dst)[1] = src[offset + 2];
			reinterpret_cast<uint8_t*>(dst)[2] = src[offset + 1];
			reinterpret_cast<uint8_t*>(dst)[3] = src[offset + 0];
		}
		else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
		}
	
		offset += 4;
	}

	inline void get(int16* dst, const char* src, uint32& offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[offset + 1];
			reinterpret_cast<uint8_t*>(dst)[1] = src[offset + 0];
		}
		else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
		}
	
		offset += 2;
	}

	inline void get(uint16* dst, const char* src, uint32& offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[offset + 1];
			reinterpret_cast<uint8_t*>(dst)[1] = src[offset + 0];
		}
		else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
		}
	
		offset += 2;
	}

	inline void get(char* dst, const char* src, uint32& offset) {
		dst[0] = src[offset + 0];
		++offset;
	}

	inline void get(uint8* dst, const char* src, uint32& offset) {
		dst[0] = src[offset + 0];
		++offset;
	}

	inline void get8b(void* dst, const char* src)
	{
		if constexpr (_WIN64) {
			static_cast<uint8_t*>(dst)[0] = src[7];
			static_cast<uint8_t*>(dst)[1] = src[6];
			static_cast<uint8_t*>(dst)[2] = src[5];
			static_cast<uint8_t*>(dst)[3] = src[4];
			static_cast<uint8_t*>(dst)[4] = src[3];
			static_cast<uint8_t*>(dst)[5] = src[2];
			static_cast<uint8_t*>(dst)[6] = src[1];
			static_cast<uint8_t*>(dst)[7] = src[0];
		}
		else {
			static_cast<uint8_t*>(dst)[0] = src[0];
			static_cast<uint8_t*>(dst)[1] = src[1];
			static_cast<uint8_t*>(dst)[2] = src[2];
			static_cast<uint8_t*>(dst)[3] = src[3];
			static_cast<uint8_t*>(dst)[4] = src[4];
			static_cast<uint8_t*>(dst)[5] = src[5];
			static_cast<uint8_t*>(dst)[6] = src[6];
			static_cast<uint8_t*>(dst)[7] = src[7];
		}
	}

	inline void get4b(void* dst, const char* src) {
		if constexpr (_WIN64) {
			static_cast<uint8_t*>(dst)[0] = src[3];
			static_cast<uint8_t*>(dst)[1] = src[2];
			static_cast<uint8_t*>(dst)[2] = src[1];
			static_cast<uint8_t*>(dst)[3] = src[0];
		}
		else {
			static_cast<uint8_t*>(dst)[0] = src[0];
			static_cast<uint8_t*>(dst)[1] = src[1];
			static_cast<uint8_t*>(dst)[2] = src[2];
			static_cast<uint8_t*>(dst)[3] = src[3];
		}
	}

	inline void get2b(void* dst, const char* src)
	{
		if constexpr (_WIN64) {
			static_cast<uint8_t*>(dst)[0] = src[1];
			static_cast<uint8_t*>(dst)[1] = src[0];
		}
		else {
			static_cast<uint8_t*>(dst)[0] = src[0];
			static_cast<uint8_t*>(dst)[1] = src[1];
		}
	}

	inline void get1b(void* dst, const char* src) {
		static_cast<uint8_t*>(dst)[0] = src[0];
	}

	inline void get8b(void* dst, const char* src, uint32& offset) {
		offset += 8;
		get8b(dst, src);
	}

	inline void get4b(void* dst, const char* src, uint32& offset) {
		offset += 4;
		get4b(dst, src);
	}

	inline void get2b(void* dst, const char* src, uint32& offset) {
		offset += 2;
		get2b(dst, src);
	}

	inline void get1b(void* dst, const char* src, uint32& offset) {
		++offset;
		get1b(dst, src);
	}

	inline float32 to_2_14_float(const int16 value) {
		return static_cast<float32>(value & 0x3fff) / static_cast<float32>(1 << 14) + (-2 * ((value >> 15) & 0x1) + ((value >> 14) & 0x1));
	}
	
	struct TTFFlags {
		bool xDual : 1;
		bool yDual : 1;
		bool xShort : 1;
		bool yShort : 1;
		bool repeat : 1;
		bool isControlPoint : 1;
	};
	
	enum COMPOUND_GLYPH_FLAGS {
		ARG_1_AND_2_ARE_WORDS = 0x0001,
		ARGS_ARE_XY_VALUES = 0x0002,
		ROUND_XY_TO_GRID = 0x0004,
		WE_HAVE_A_SCALE = 0x0008,
		MORE_COMPONENTS = 0x0020,
		WE_HAVE_AN_X_AND_Y_SCALE = 0x0040,
		WE_HAVE_A_TWO_BY_TWO = 0x0080,
		WE_HAVE_INSTRUCTIONS = 0x0100,
		USE_MY_METRICS = 0x0200,
		OVERLAP_COMPOUND = 0x0400,
		SCALED_COMPONENT_OFFSET = 0x0800,
		UNSCALED_COMPONENT_OFFSET = 0x1000
	};

	struct TTFHeader {
		uint32 Version;
		uint16 NumberOfTables;
		uint16 SearchRange;
		uint16 EntrySelector;
		uint16 RangeShift;

		void Parse(const char* data, uint32& offset)
		{
			get(&Version, data, offset);
			get(&NumberOfTables, data, offset);
			/*get2b(&searchRange, data + offset); offset += sizeof(uint16);
			get2b(&entrySelector, data + offset); offset += sizeof(uint16);
			get2b(&rangeShift, data + offset); offset += sizeof(uint16);*/
			offset += sizeof(uint16) * 3;
		}
	};

	struct TableEntry
	{
		uint32 tag;
		char tagstr[5];
		uint32 checkSum;
		uint32 offsetPos;
		uint32 length;

		void Parse(const char* data, uint32& offset)
		{
			get(&tag, data, offset); offset -= 4;
			get(&tagstr[0], data, offset); get(&tagstr[1], data, offset); 	get(&tagstr[2], data, offset); get(&tagstr[3], data, offset);
			tagstr[4] = 0;
			get(&checkSum, data, offset);
			get(&offsetPos, data, offset);
			get(&length, data, offset);
		}
	};

	struct HeadTable
	{
		float32 tableVersion;
		float32 fontRevision;
		uint32 checkSumAdjustment;
		uint32 magicNumber;//0x5F0F3CF5
		uint16 flags;
		uint16 unitsPerEm;
		long long createdDate;
		long long modifiedData;
		short xMin;
		short yMin;
		short xMax;
		short yMax;
		uint16 macStyle;
		uint16 lowestRecPPEM;
		short fontDirectionHintl;
		short indexToLocFormat;
		short glyphDataFormat;

		void Parse(const char* data, uint32& offset)
		{
			get(&tableVersion, data, offset);
			get(&fontRevision, data, offset);
			get(&checkSumAdjustment, data, offset);
			get(&magicNumber, data, offset);
			get(&flags, data, offset);
			get(&unitsPerEm, data, offset);
			get(&createdDate, data, offset);
			get(&modifiedData, data, offset);
			get(&xMin, data, offset);
			get(&yMin, data, offset);
			get(&xMax, data, offset);
			get(&yMax, data, offset);
			get(&macStyle, data, offset);
			get(&lowestRecPPEM, data, offset);
			get(&fontDirectionHintl, data, offset);
			get(&indexToLocFormat, data, offset);
			get(&glyphDataFormat, data, offset);
		}
	};

	struct MaximumProfile
	{
		float32 version;
		uint16 numGlyphs;
		uint16 maxPoints;
		uint16 maxContours;
		uint16 maxCompositePoints;
		uint16 maxCompositeContours;
		uint16 maxZones;
		uint16 maxTwilightPoints;
		uint16 maxStorage;
		uint16 maxFunctionDefs;
		uint16 maxInstructionDefs;
		uint16 maxStackElements;
		uint16 maxSizeOfInstructions;
		uint16 maxComponentElements;
		uint16 maxComponentDepth;

		void Parse(const char* data, uint32& offset) {
			get(&version, data, offset);
			get(&numGlyphs, data, offset);
			get(&maxPoints, data, offset);
			get(&maxContours, data, offset);
			get(&maxCompositePoints, data, offset);
			get(&maxCompositeContours, data, offset);
			get(&maxZones, data, offset);
			get(&maxTwilightPoints, data, offset);
			get(&maxStorage, data, offset);
			get(&maxFunctionDefs, data, offset);
			get(&maxInstructionDefs, data, offset);
			get(&maxStackElements, data, offset);
			get(&maxSizeOfInstructions, data, offset);
			get(&maxComponentElements, data, offset);
			get(&maxComponentDepth, data, offset);
		}
	};

	struct NameValue
	{
		uint16 platformID;
		uint16 encodingID;
		uint16 languageID;
		uint16 nameID;
		uint16 length;
		uint16 offset_value;

		void Parse(const char* data, uint32& offset) {
			get(&platformID, data, offset);
			get(&encodingID, data, offset);
			get(&languageID, data, offset);
			get(&nameID, data, offset);
			get(&length, data, offset);
			get(&offset_value, data, offset);
		}
	};

	struct NameTable
	{
		uint16 Format;
		uint16 count;
		uint16 stringOffset;
		std::vector<NameValue> NameRecords;

		void Parse(const char* data, uint32& offset, std::string* names, uint16 maxNumberOfNames = 25)
		{
			uint32 offset_start = offset;
			get(&Format, data, offset);
			get(&count, data, offset);
			get(&stringOffset, data, offset);
			NameRecords.resize(count);
			for (uint32 i = 0; i < count; i++) {
				if (NameRecords[i].nameID > maxNumberOfNames) { continue; }

				NameRecords[i].Parse(data, offset);
				char* newNameString = new char[NameRecords[i].length];
				memcpy(newNameString, data + offset_start + stringOffset + NameRecords[i].offset_value, sizeof(char) * NameRecords[i].length);
				uint16 string_length = NameRecords[i].length;

				if (newNameString[0] == 0) {
					string_length = string_length >> 1;

					for (uint16 j = 0; j < string_length; j++) {
						newNameString[j] = newNameString[j * 2 + 1];
					}
				}

				names[NameRecords[i].nameID].assign(newNameString, string_length);

				delete[] newNameString;
			}
		}
	};

	struct HHEATable
	{
		uint16 majorVersion;
		uint16 minorVersion;
		int16 Ascender;
		int16 Descender;
		int16 LineGap;
		uint16 advanceWidthMax;
		int16 minLeftSideBearing;
		int16 minRightSideBearing;
		int16 xMaxExtent;
		int16 caretSlopeRise;
		int16 caretSlopeRun;
		int16 caretOffset;
		int16 metricDataFormat;
		uint16 numberOfHMetrics;

		void Parse(const char* data, uint32& offset)
		{
			get(&majorVersion, data, offset);
			get(&minorVersion, data, offset);
			get(&Ascender, data, offset);
			get(&Descender, data, offset);
			get(&LineGap, data, offset);
			get(&advanceWidthMax, data, offset);
			get(&minLeftSideBearing, data, offset);
			get(&minRightSideBearing, data, offset);
			get(&xMaxExtent, data, offset);
			get(&caretSlopeRise, data, offset);
			get(&caretSlopeRun, data, offset);
			get(&caretOffset, data, offset);
			offset += sizeof(int16) * 4;
			get(&metricDataFormat, data, offset);
			get(&numberOfHMetrics, data, offset);
		}
	};

	struct FontMetaData
	{
		uint16 UnitsPerEm;
		int16 Ascender;
		int16 Descender;
		int16 LineGap;
	};
	
	struct Segment {
		//0 is on curve
		//1 is control point or nan
		//2 is on curve
		GTSL::Vector2 Points[3];
	
		bool IsCurve = false;
	
		bool IsBezierCurve() const { return IsCurve; }
	};
	using Path = Vector<Segment, DefaultAllocatorReference>;
	
	struct Glyph
	{
		uint32 Character = 0;
		int16 GlyphIndex = 0;
		int16 NumContours = 0;
		Vector<Path, DefaultAllocatorReference> Paths;
		uint16 AdvanceWidth = 0;
		int16 LeftSideBearing = 0;
		Vector2 BoundingBox[2]; //min, max
		Vector2 Center;
	
		Glyph(const DefaultAllocatorReference& allocator = DefaultAllocatorReference()) : Paths(allocator) {}
	};

	struct Font
	{
		Font(const DefaultAllocatorReference& allocator) : KerningTable(8, allocator), Glyphs(128, allocator) /*,GlyphMap(128, allocator)*/ {}

		uint32 FileNameHash;
		std::string FullFontName;
		std::string NameTable[25];
		HashMap<uint32, int16, DefaultAllocatorReference> KerningTable;
		HashMap<uint32, Glyph, DefaultAllocatorReference> Glyphs;
		//HashMap<uint32, uint16, DefaultAllocatorReference> GlyphMap;
		std::unordered_map<uint32, uint16> GlyphMap;
		FontMetaData Metadata;
		uint64 LastUsed;

		[[nodiscard]] const Glyph& GetGlyph(const char8_t ch) const {
			return Glyphs.At(GlyphMap.at(ch));
		}
	};

	struct float_v4
	{
		float32 data[4];
	};

	struct FontLineInfoData
	{
		uint32 StringStartIndex;
		uint32 StringEndIndex;
		Vector2 OffsetStart;
		Vector2 OffsetEnd;
		Vector<Glyph*, DefaultAllocatorReference> GlyphIndex;
	};

	struct FontPositioningOutput
	{
		Vector<FontLineInfoData, DefaultAllocatorReference> LinePositions;
		uint32 NumTriangles;
		//PixelPositioning alignment;
		//BoundingRect bounding_rect;
		uint32 Geometry;
		uint16 FontSize;
	};

	struct FontPositioningOptions
	{
		bool IsMultiline;
		bool IsWordPreserve;
		float32 LineHeight;
		//PixelPositioning alignment;
		//BoundingRect bounding_rect;
		FontPositioningOptions()
		{
			IsMultiline = true;
			IsWordPreserve = true;
			LineHeight = 1.0f;
		}
	};

	inline int16 GetKerningOffset(Font* font_data, uint16 left_glyph, uint16 right_glyph)
	{
		auto kern_data = font_data->KerningTable.TryGet((left_glyph << 16) | right_glyph);
		return kern_data.State() ? kern_data.Get() : 0;
	}

	inline Vector2 toVector(const ShortVector sh) { return Vector2(sh.X, sh.Y); }

	inline bool MakeFont(const char* data, Font* fontData) {
		std::unordered_map<std::string, TableEntry> tables;

		uint32 ptr = 0;

		{
			TTFHeader header;
			header.Parse(data, ptr);

			for (uint16 i = 0; i < header.NumberOfTables; i++) {
				TableEntry te;
				te.Parse(data, ptr);
				tables.emplace(te.tagstr, te);
			}
		}

		auto head_table_entry = tables.find("head");
		if (head_table_entry == tables.end()) { return -2; }

		ptr = head_table_entry->second.offsetPos;

		HeadTable headTable;
		headTable.Parse(data, ptr);
		auto maxp_table_entry = tables.find("maxp");
		if (maxp_table_entry == tables.end()) { return -2; }

		ptr = maxp_table_entry->second.offsetPos;

		MaximumProfile max_profile;
		max_profile.Parse(data, ptr);
		auto name_table_entry = tables.find("name");
		if (name_table_entry == tables.end()) { return -2; }

		ptr = name_table_entry->second.offsetPos;

		NameTable name_table;
		name_table.Parse(data, ptr, fontData->NameTable);

		fontData->FullFontName = fontData->NameTable[1] + " " + fontData->NameTable[2];

		auto loca_table_entry = tables.find("loca");
		if (loca_table_entry == tables.end()) { return -2; }

		std::vector<uint32> glyph_index(max_profile.numGlyphs);

		uint32 end_of_glyf = 0;

		if (headTable.indexToLocFormat == 0)
		{
			uint32 byte_offset = loca_table_entry->second.offsetPos;

			for (uint16 i = 0; i < max_profile.numGlyphs; i++, byte_offset += sizeof(uint16))
			{
				get2b(&glyph_index[i], data + byte_offset);
				glyph_index[i] = glyph_index[i] << 1;
			}

			get2b(&end_of_glyf, data + byte_offset);
			end_of_glyf = end_of_glyf << 1;
		}
		else
		{
			uint32 byte_offset = loca_table_entry->second.offsetPos;
			for (uint16 i = 0; i < max_profile.numGlyphs; i++, byte_offset += sizeof(uint32))
			{
				get4b(&glyph_index[i], data + byte_offset);
			}
			get4b(&end_of_glyf, data + byte_offset);
		}

		auto cmap_table_entry = tables.find("cmap");
		if (cmap_table_entry == tables.end()) { return -2; }

		uint32 cmap_offset = cmap_table_entry->second.offsetPos + sizeof(uint16); //Skip version
		uint16 cmap_num_tables;
		get(&cmap_num_tables, data, cmap_offset);

		std::map<uint16, uint32> glyphReverseMap;

		bool valid_cmap_table = false;
		for (uint16 i = 0; i < cmap_num_tables; i++) {
			constexpr uint8 UNICODE_PLATFORM_INDEX = 0; constexpr uint8 WIN32_PLATFORM_INDEX = 3; constexpr uint8 WIN32_UNICODE_ENCODING = 1;

			uint16 platformID, encodingID;
			uint32 cmap_subtable_offset;
			get(&platformID, data, cmap_offset);
			get(&encodingID, data, cmap_offset);
			get(&cmap_subtable_offset, data, cmap_offset);

			if (!((platformID == UNICODE_PLATFORM_INDEX && encodingID == 3/*\(··)/*/) || (platformID == WIN32_PLATFORM_INDEX && encodingID == WIN32_UNICODE_ENCODING))) { continue; }

			cmap_subtable_offset += cmap_table_entry->second.offsetPos;
			uint16 format, length;
			get(&format, data, cmap_subtable_offset);
			get(&length, data, cmap_subtable_offset);

			if (format != 4) { continue; }

			uint16 language, segCountX2;// , searchRange, entrySelector, rangeShift;
			get(&language, data, cmap_subtable_offset);
			get(&segCountX2, data, cmap_subtable_offset);
			//get2b(&searchRange, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
			//get2b(&entrySelector, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
			//get2b(&rangeShift, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
			cmap_subtable_offset += sizeof(uint16) * 3;

			uint16 segCount = segCountX2 >> 1;
			std::vector<uint16> endCount(segCount), startCount(segCount), idRangeOffset(segCount);
			std::vector<int16> idDelta(segCount);
			for (uint16 j = 0; j < segCount; j++)
			{
				get(&endCount[j], data, cmap_subtable_offset);
			}

			cmap_subtable_offset += sizeof(uint16);

			for (uint16 j = 0; j < segCount; j++)
			{
				get2b(&startCount[j], data + cmap_subtable_offset);
				get2b(&idDelta[j], data + cmap_subtable_offset + sizeof(uint16) * segCount);
				get2b(&idRangeOffset[j], data + cmap_subtable_offset + sizeof(uint16) * segCount * 2);
				if (idRangeOffset[j] == 0)
				{
					for (uint32 k = startCount[j]; k <= endCount[j]; k++)
					{
						fontData->GlyphMap[k] = k + idDelta[j];
						glyphReverseMap[k + idDelta[j]] = k;
					}
				}
				else
				{
					uint32 glyph_address_offset = cmap_subtable_offset + sizeof(uint16) * segCount * 2; //idRangeOffset_ptr
					for (uint32 k = startCount[j]; k <= endCount[j]; k++)
					{
						uint32 glyph_address_index_offset = idRangeOffset[j] + 2 * (k - startCount[j]) + glyph_address_offset;
						uint16& glyph_map_value = fontData->GlyphMap[k];
						get2b(&glyph_map_value, data + glyph_address_index_offset);
						glyphReverseMap[glyph_map_value] = k;
						glyph_map_value += idDelta[j];
					}
				}

				cmap_subtable_offset += sizeof(uint16);
			}

			valid_cmap_table = true;
			break;
		}
		if (!valid_cmap_table) { /*BE_ASSERT(false);*/ }


		HHEATable hheaTable;
		auto hhea_table_entry = tables.find("hhea");
		if (hhea_table_entry == tables.end()) { return -2; }
		ptr = hhea_table_entry->second.offsetPos;
		hheaTable.Parse(data, ptr);

		auto glyf_table_entry = tables.find("glyf");
		{
			if (glyf_table_entry == tables.end()) { return -2; }
		}
		uint32 glyf_offset = glyf_table_entry->second.offsetPos;

		auto kern_table_entry = tables.find("kern");
		uint32 kernOffset = 0;
		if (kern_table_entry != tables.end())
		{
			kernOffset = kern_table_entry->second.offsetPos;
		}

		auto hmtx_table_entry = tables.find("hmtx");
		if (hmtx_table_entry == tables.end())
		{
			return -2;
		}
		uint32 hmtx_offset = hmtx_table_entry->second.offsetPos;
		uint16 last_glyph_advance_width = 0;

		std::vector<std::vector<uint16>> pointsPerContour((max_profile.maxContours < 4096) ? max_profile.maxContours : 4096);
		std::vector<uint16> pointsInContour((max_profile.maxContours < 4096) ? max_profile.maxContours : 4096);

		if (!max_profile.numGlyphs) { return -1; }

		Vector<bool, DefaultAllocatorReference> glyphLoaded(max_profile.numGlyphs, DefaultAllocatorReference());
		for(uint32 i = 0; i < max_profile.numGlyphs; ++i) {
			glyphLoaded.EmplaceBack(false);
		}

		auto parseGlyph = [&](uint32 i, auto&& self) -> int8
		{
			if (glyphLoaded[i] == true) { return 1; }

			Glyph& currentGlyph = fontData->Glyphs.Emplace(i);
			//currentGlyph.Paths.Initialize(3, GetPersistentAllocator());
			currentGlyph.GlyphIndex = static_cast<int16>(i);
			currentGlyph.Character = glyphReverseMap[static_cast<int16>(i)];

			if (i < hheaTable.numberOfHMetrics)
			{
				ptr = hmtx_offset + i * sizeof(uint32);

				get(&currentGlyph.AdvanceWidth, data, ptr);
				last_glyph_advance_width = currentGlyph.AdvanceWidth;
				get(&currentGlyph.LeftSideBearing, data, ptr);
			}
			else
			{
				currentGlyph.AdvanceWidth = last_glyph_advance_width;
			}

			if (i != max_profile.numGlyphs - 1 && glyph_index[i] == glyph_index[i + 1])
			{
				glyphLoaded[i] = true;
				return -1;
			}

			if (glyph_index[i] >= end_of_glyf) { return -1; }

			uint32 currentOffset = glyf_offset + glyph_index[i];

			int16 bbox[2][2];

			get(&currentGlyph.NumContours, data, currentOffset);
			get(&bbox[0][0], data, currentOffset); //xMin
			get(&bbox[0][1], data, currentOffset); //yMin
			get(&bbox[1][0], data, currentOffset); //xMax
			get(&bbox[1][1], data, currentOffset); //yMax

			currentGlyph.BoundingBox[0][0] = bbox[0][0];
			currentGlyph.BoundingBox[0][1] = bbox[0][1];
			currentGlyph.BoundingBox[1][0] = bbox[1][0];
			currentGlyph.BoundingBox[1][1] = bbox[1][1];

			//Vector2 glyphCenter = (currentGlyph.BoundingBox[0] + currentGlyph.BoundingBox[2]) / 2.0f;
			//glyphCenter.X() = 
			//glyphCenter.Y() = (currentGlyph.BoundingBox[1] + currentGlyph.BoundingBox[3]) / 2.0f;

			//currentGlyph.Center = glyphCenter;

			if (currentGlyph.NumContours > 0)
			{ //Simple glyph
				std::vector<uint16> contourEnd(currentGlyph.NumContours);

				//currentGlyph.PathList.Resize(currentGlyph.NumContours);
				//don't resize
				//code expects resize to leave valid elements which our vector doesn't
				//emplace elements as needed later to ensure valid elements

				for (uint16 j = 0; j < currentGlyph.NumContours; j++) { get2b(&contourEnd[j], data + currentOffset); currentOffset += sizeof(uint16); }

				for (uint16 contourIndex = 0; contourIndex < currentGlyph.NumContours; contourIndex++)
				{
					uint16 num_points = contourEnd[contourIndex] - (contourIndex ? contourEnd[contourIndex - 1] : -1);

					if (pointsPerContour[contourIndex].size() < num_points)
					{
						pointsPerContour[contourIndex].resize(num_points);
					}

					pointsInContour[contourIndex] = num_points;
				}

				//Skip instructions
				uint16 num_instructions;
				get2b(&num_instructions, data + currentOffset); currentOffset += sizeof(uint16);
				currentOffset += sizeof(uint8_t) * num_instructions;

				uint16 numPoints = contourEnd[static_cast<int64>(currentGlyph.NumContours) - 1] + 1;
				std::vector<uint8_t> flags(numPoints);
				std::vector<TTFFlags> flagsEnum(numPoints);
				std::vector<uint16> contour_index(numPoints);
				uint16 current_contour_index = 0;
				int16 repeat = 0;
				uint16 contour_count_first_point = 0;

				for (uint16 j = 0; j < numPoints; j++, ++contour_count_first_point)
				{
					if (repeat == 0)
					{
						get1b(&flags[j], data + currentOffset); currentOffset += sizeof(uint8_t);

						if (flags[j] & 0x8) { get1b(&repeat, data + currentOffset); currentOffset += sizeof(uint8_t); }
					}
					else
					{
						flags[j] = flags[j - 1];
						repeat--;
					}

					flagsEnum[j].isControlPoint = (!(flags[j] & 0b00000001)) != 0;
					flagsEnum[j].xShort = (flags[j] & 0b00000010) != 0;
					flagsEnum[j].yShort = (flags[j] & 0b00000100) != 0;
					flagsEnum[j].repeat = (flags[j] & 0b00001000) != 0;
					flagsEnum[j].xDual = (flags[j] & 0b00010000) != 0;
					flagsEnum[j].yDual = (flags[j] & 0b00100000) != 0;

					if (j > contourEnd[current_contour_index])
					{
						current_contour_index++;
						contour_count_first_point = 0;
					}

					contour_index[j] = current_contour_index;
					pointsPerContour[current_contour_index][contour_count_first_point] = j;
				}

				std::vector<ShortVector> glyphPoints(numPoints);

				for (uint16 j = 0; j < numPoints; ++j)
				{
					if (flagsEnum[j].xDual && !flagsEnum[j].xShort)
					{
						glyphPoints[j].X = j ? glyphPoints[j - 1].X : 0;
					}
					else
					{
						if (flagsEnum[j].xShort)
						{
							get1b(&glyphPoints[j].X, data + currentOffset); currentOffset += 1;
						}
						else
						{
							get2b(&glyphPoints[j].X, data + currentOffset); currentOffset += 2;
						}

						if (flagsEnum[j].xShort && !flagsEnum[j].xDual) { glyphPoints[j].X *= -1; }

						if (j != 0) { glyphPoints[j].X += glyphPoints[j - 1].X; }
					}
				}

				for (uint16 j = 0; j < numPoints; j++)
				{
					if (flagsEnum[j].yDual && !flagsEnum[j].yShort)
					{
						glyphPoints[j].Y = j ? glyphPoints[j - 1].Y : 0;
					}
					else
					{
						if (flagsEnum[j].yShort)
						{
							get1b(&glyphPoints[j].Y, data + currentOffset); currentOffset += 1;
						}
						else
						{
							get2b(&glyphPoints[j].Y, data + currentOffset); currentOffset += 2;
						}

						if (flagsEnum[j].yShort && !flagsEnum[j].yDual) { glyphPoints[j].Y *= -1; }

						if (j != 0) { glyphPoints[j].Y += glyphPoints[j - 1].Y; }
					}
				}

				//Generate contours
				for (uint16 contourIndex = 0; contourIndex < currentGlyph.NumContours; ++contourIndex)
				{
					currentGlyph.Paths.EmplaceBack(64, DefaultAllocatorReference());

					const uint16 numPointsInContour = pointsInContour[contourIndex];

					auto& contourPointsFlags = flagsEnum;

					uint16 pointInIndices = 0;

					//If the first point is control point
					while (contourPointsFlags[pointsPerContour[contourIndex][pointInIndices]].isControlPoint) { ++pointInIndices; }

					bool lastPointWasControlPoint = false;
					bool thisPointIsControlPoint = false;

					Segment currentCurve;
					currentCurve.Points[0] = toVector(glyphPoints[pointsPerContour[contourIndex][pointInIndices]]); //what if no more points

					++pointInIndices;

					for (uint32 p = 0; p < numPointsInContour; ++p, ++pointInIndices)
					{
						uint32 safeIndexToData = pointsPerContour[contourIndex][pointInIndices % numPointsInContour];

						thisPointIsControlPoint = contourPointsFlags[safeIndexToData].isControlPoint;

						if (thisPointIsControlPoint)
						{
							if (lastPointWasControlPoint)
							{
								auto thisPoint = toVector(glyphPoints[safeIndexToData]);
								auto newPoint = (thisPoint + currentCurve.Points[1]) * 0.5f;
								currentCurve.Points[2] = newPoint;
								currentCurve.IsCurve = true;
								currentGlyph.Paths[contourIndex].EmplaceBack(currentCurve);

								currentCurve.Points[0] = newPoint;

								currentCurve.Points[1] = thisPoint;
							}
							else
							{
								currentCurve.Points[1] = toVector(glyphPoints[safeIndexToData]);
							}
						}
						else
						{
							if (lastPointWasControlPoint)
							{
								auto thisPoint = toVector(glyphPoints[safeIndexToData]);
								currentCurve.Points[2] = thisPoint;
								currentCurve.IsCurve = true;
								currentGlyph.Paths[contourIndex].EmplaceBack(currentCurve);

								currentCurve.Points[0] = thisPoint;
							}
							else
							{
								auto thisPoint = toVector(glyphPoints[safeIndexToData]);
								currentCurve.Points[1] = GTSL::Vector2(0, 0);
								currentCurve.Points[2] = thisPoint;
								currentCurve.IsCurve = false;
								currentGlyph.Paths[contourIndex].EmplaceBack(currentCurve);

								currentCurve.Points[0] = thisPoint;
							}
						}

						lastPointWasControlPoint = thisPointIsControlPoint;
					}
				} //for contour
			}
			else //Composite glyph
			{
				for (auto compound_glyph_index = 0; compound_glyph_index < -currentGlyph.NumContours; compound_glyph_index++)
				{
					uint16 glyfFlags, glyphIndex;

					do
					{
						get2b(&glyfFlags, data + currentOffset); currentOffset += sizeof(uint16);
						get2b(&glyphIndex, data + currentOffset); currentOffset += sizeof(uint16);

						int16 glyfArgs1 = 0, glyfArgs2 = 0;
						int8_t glyfArgs1U8 = 0, glyfArgs2U8 = 0;
						bool is_word = false;
						if (glyfFlags & ARG_1_AND_2_ARE_WORDS)
						{
							get2b(&glyfArgs1, data + currentOffset); currentOffset += sizeof(int16);
							get2b(&glyfArgs2, data + currentOffset); currentOffset += sizeof(int16);
							is_word = true;
						}
						else
						{
							get1b(&glyfArgs1U8, data + currentOffset); currentOffset += sizeof(int8_t);
							get1b(&glyfArgs2U8, data + currentOffset); currentOffset += sizeof(int8_t);
						}

						float32 compositeGlyphElementTransformation[6] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

						if (glyfFlags & WE_HAVE_A_SCALE)
						{
							int16 xy_value = 0;
							get2b(&xy_value, data + currentOffset); currentOffset += sizeof(int16);
							compositeGlyphElementTransformation[0] = to_2_14_float(xy_value);
							compositeGlyphElementTransformation[3] = to_2_14_float(xy_value);
						}
						else if (glyfFlags & WE_HAVE_AN_X_AND_Y_SCALE)
						{
							int16 xy_values[2];
							get2b(&xy_values[0], data + currentOffset); currentOffset += sizeof(int16);
							get2b(&xy_values[1], data + currentOffset); currentOffset += sizeof(int16);
							compositeGlyphElementTransformation[0] = to_2_14_float(xy_values[0]);
							compositeGlyphElementTransformation[3] = to_2_14_float(xy_values[1]);
						}
						else if (glyfFlags & WE_HAVE_A_TWO_BY_TWO)
						{
							int16 xy_values[4];
							get2b(&xy_values[0], data + currentOffset); currentOffset += sizeof(int16);
							get2b(&xy_values[1], data + currentOffset); currentOffset += sizeof(int16);
							get2b(&xy_values[2], data + currentOffset); currentOffset += sizeof(int16);
							get2b(&xy_values[3], data + currentOffset); currentOffset += sizeof(int16);
							compositeGlyphElementTransformation[0] = to_2_14_float(xy_values[0]);
							compositeGlyphElementTransformation[1] = to_2_14_float(xy_values[1]);
							compositeGlyphElementTransformation[2] = to_2_14_float(xy_values[2]);
							compositeGlyphElementTransformation[3] = to_2_14_float(xy_values[3]);
						}

						bool matched_points = false;
						if (glyfFlags & ARGS_ARE_XY_VALUES)
						{
							compositeGlyphElementTransformation[4] = is_word ? glyfArgs1 : glyfArgs1U8;
							compositeGlyphElementTransformation[5] = is_word ? glyfArgs2 : glyfArgs2U8;
							if (glyfFlags & SCALED_COMPONENT_OFFSET)
							{
								compositeGlyphElementTransformation[4] *= compositeGlyphElementTransformation[0];
								compositeGlyphElementTransformation[5] *= compositeGlyphElementTransformation[3];
							}
						}
						else
						{
							matched_points = true;
						}

						//Skip instructions
						if (glyfFlags & WE_HAVE_INSTRUCTIONS)
						{
							uint16 num_instructions = 0;
							get2b(&num_instructions, data + currentOffset); currentOffset += sizeof(uint16);
							currentOffset += sizeof(uint8_t) * num_instructions;
						}

						if (glyphLoaded[glyphIndex] == false)
						{
							if (self(glyphIndex, self) < 0) {
								//BE_LOG_WARNING("ttf-parser: bad glyph index ", glyphIndex, " in composite glyph");
								continue;
							}
						}

						Glyph& compositeGlyphElement = fontData->Glyphs[glyphIndex];

						auto transformCurve = [&compositeGlyphElementTransformation](Segment& curve) -> Segment
						{
							Segment out;
							out.Points[0].X() = curve.Points[0].X() * compositeGlyphElementTransformation[0] + curve.Points[0].X() * compositeGlyphElementTransformation[1] + compositeGlyphElementTransformation[4];
							out.Points[0].Y() = curve.Points[0].Y() * compositeGlyphElementTransformation[2] + curve.Points[0].Y() * compositeGlyphElementTransformation[3] + compositeGlyphElementTransformation[5];
							out.Points[1].X() = curve.Points[1].X() * compositeGlyphElementTransformation[0] + curve.Points[1].Y() * compositeGlyphElementTransformation[1] + compositeGlyphElementTransformation[4];
							out.Points[1].Y() = curve.Points[1].X() * compositeGlyphElementTransformation[2] + curve.Points[1].Y() * compositeGlyphElementTransformation[3] + compositeGlyphElementTransformation[5];
							out.Points[2].X() = curve.Points[2].X() * compositeGlyphElementTransformation[0] + curve.Points[2].Y() * compositeGlyphElementTransformation[1] + compositeGlyphElementTransformation[4];
							out.Points[2].Y() = curve.Points[2].X() * compositeGlyphElementTransformation[2] + curve.Points[2].Y() * compositeGlyphElementTransformation[3] + compositeGlyphElementTransformation[5];
							return out;
						};

						uint32 compositeGlyphPathCount = compositeGlyphElement.Paths.GetLength();
						for (uint32 glyphPointIndex = 0; glyphPointIndex < compositeGlyphPathCount; glyphPointIndex++)
						{
							Vector<Segment, DefaultAllocatorReference>& currentCurvesList = compositeGlyphElement.Paths[glyphPointIndex];

							uint32 compositeGlyphPathCurvesCount = currentCurvesList.GetLength();

							Path newPath{ DefaultAllocatorReference() };
							if (matched_points == false) {
								for (uint32 glyphCurvesPointIndex = 0; glyphCurvesPointIndex < compositeGlyphPathCurvesCount; glyphCurvesPointIndex++)
								{
									newPath.EmplaceBack(transformCurve(currentCurvesList[glyphCurvesPointIndex]));
								}
							}
							else
							{
								//BE_LOG_WARNING("ttf-parser: unsupported matched points in ttf composite glyph");
								continue;
							}

							currentGlyph.Paths.EmplaceBack(newPath);
						}
					} while (glyfFlags & MORE_COMPONENTS);
				}
			}

			glyphLoaded[i] = true;

			return 0;
		};

		for (uint16 i = 0; i < max_profile.numGlyphs; i++)
		{
			parseGlyph(i, parseGlyph);
		}

		//Kerning table
		if (kernOffset) {
			uint32 currentOffset = kernOffset;
			uint16 kern_table_version, num_kern_subtables;
			get2b(&kern_table_version, data + currentOffset); currentOffset += sizeof(uint16);
			get2b(&num_kern_subtables, data + currentOffset); currentOffset += sizeof(uint16);
			uint16 kern_length = 0;
			uint32 kernStartOffset = currentOffset;

			for (uint16 kerningSubTableIndex = 0; kerningSubTableIndex < num_kern_subtables; kerningSubTableIndex++)
			{
				uint16 kerningVersion, kerningCoverage;
				currentOffset = kernStartOffset + kern_length;
				kernStartOffset = currentOffset;
				get2b(&kerningVersion, data + currentOffset); currentOffset += sizeof(uint16);
				get2b(&kern_length, data + currentOffset); currentOffset += sizeof(uint16);
				if (kerningVersion != 0)
				{
					currentOffset += kern_length - sizeof(uint16) * 3;
					continue;
				}
				get2b(&kerningCoverage, data + currentOffset); currentOffset += sizeof(uint16);

				uint16 num_kern_pairs;
				get2b(&num_kern_pairs, data + currentOffset); currentOffset += sizeof(uint16);
				currentOffset += sizeof(uint16) * 3;
				for (uint16 kern_index = 0; kern_index < num_kern_pairs; kern_index++)
				{
					uint16 kern_left, kern_right;
					int16 kern_value;
					get2b(&kern_left, data + currentOffset); currentOffset += sizeof(uint16);
					get2b(&kern_right, data + currentOffset); currentOffset += sizeof(uint16);
					get2b(&kern_value, data + currentOffset); currentOffset += sizeof(int16);

					fontData->KerningTable.Emplace((kern_left << 16) | kern_right, kern_value);
				}
			}
		}

		fontData->Metadata.UnitsPerEm = headTable.unitsPerEm;
		fontData->Metadata.Ascender = hheaTable.Ascender;
		fontData->Metadata.Descender = hheaTable.Descender;
		fontData->Metadata.LineGap = hheaTable.LineGap;

		return 0;
	}

	//struct TTFHeader {
	//	uint32 Version;
	//	uint16 NumberOfTables;
	//	uint16 SearchRange;
	//	uint16 EntrySelector;
	//	uint16 RangeShift;
	//
	//	uint32 Parse(const char* data, uint32 offset)
	//	{
	//		get(&Version, data, offset);
	//		get(&NumberOfTables, data, offset);
	//		/*get(&searchRange, data + offset); offset += sizeof(uint16);
	//		get(&entrySelector, data + offset); offset += sizeof(uint16);
	//		get(&rangeShift, data + offset); offset += sizeof(uint16);*/
	//		offset += sizeof(uint16) * 3;
	//		return offset;
	//	}
	//};
	//
	//struct TableEntry
	//{
	//	uint32 tag;
	//	char tagstr[5];
	//	uint32 checkSum;
	//	uint32 offsetPos;
	//	uint32 length;
	//
	//	uint32 Parse(const char* data, uint32 offset)
	//	{
	//		get(&tag, data, offset);
	//		memcpy(tagstr, data + offset, sizeof(uint32)); tagstr[4] = 0; offset += sizeof(uint32);
	//		get(&checkSum, data, offset); offset += sizeof(uint32);
	//		get(&offsetPos, data, offset); offset += sizeof(uint32);
	//		get(&length, data, offset); offset += sizeof(uint32);
	//		return offset;
	//	}
	//};
	//
	//struct HeadTable
	//{
	//	float32 tableVersion;
	//	float32 fontRevision;
	//	uint32 checkSumAdjustment;
	//	uint32 magicNumber;//0x5F0F3CF5
	//	uint16 flags;
	//	uint16 unitsPerEm;
	//	long long createdDate;
	//	long long modifiedData;
	//	short xMin;
	//	short yMin;
	//	short xMax;
	//	short yMax;
	//	uint16 macStyle;
	//	uint16 lowestRecPPEM;
	//	short fontDirectionHintl;
	//	short indexToLocFormat;
	//	short glyphDataFormat;
	//
	//	uint32 Parse(const char* data, uint32 offset)
	//	{
	//		get(&tableVersion, data, offset);
	//		get(&fontRevision, data, offset);
	//		get(&checkSumAdjustment, data, offset);
	//		get(&magicNumber, data, offset);
	//		get(&flags, data, offset);
	//		get(&unitsPerEm, data, offset);
	//		get(&createdDate, data, offset);
	//		get(&modifiedData, data, offset);
	//		get(&xMin, data, offset);
	//		get(&yMin, data, offset);
	//		get(&xMax, data, offset);
	//		get(&yMax, data, offset);
	//		get(&macStyle, data, offset);
	//		get(&lowestRecPPEM, data, offset);
	//		get(&fontDirectionHintl, data, offset);
	//		get(&indexToLocFormat, data, offset);
	//		get(&glyphDataFormat, data, offset);
	//		return offset;
	//	}
	//};
	//
	//struct MaximumProfile
	//{
	//	float32 version;
	//	uint16 numGlyphs;
	//	uint16 maxPoints;
	//	uint16 maxContours;
	//	uint16 maxCompositePoints;
	//	uint16 maxCompositeContours;
	//	uint16 maxZones;
	//	uint16 maxTwilightPoints;
	//	uint16 maxStorage;
	//	uint16 maxFunctionDefs;
	//	uint16 maxInstructionDefs;
	//	uint16 maxStackElements;
	//	uint16 maxSizeOfInstructions;
	//	uint16 maxComponentElements;
	//	uint16 maxComponentDepth;
	//
	//	uint32 Parse(const char* data, uint32 offset) {
	//		get(&version, data, offset);
	//		get(&numGlyphs, data, offset);
	//		get(&maxPoints, data, offset);
	//		get(&maxContours, data, offset);
	//		get(&maxCompositePoints, data, offset);
	//		get(&maxCompositeContours, data, offset);
	//		get(&maxZones, data, offset);
	//		get(&maxTwilightPoints, data, offset);
	//		get(&maxStorage, data, offset);
	//		get(&maxFunctionDefs, data, offset);
	//		get(&maxInstructionDefs, data, offset);
	//		get(&maxStackElements, data, offset);
	//		get(&maxSizeOfInstructions, data, offset);
	//		get(&maxComponentElements, data, offset);
	//		get(&maxComponentDepth, data, offset);
	//		return offset;
	//	}
	//};
	//
	//struct NameValue
	//{
	//	uint16 platformID;
	//	uint16 encodingID;
	//	uint16 languageID;
	//	uint16 nameID;
	//	uint16 length;
	//	uint16 offset_value;
	//
	//	uint32 Parse(const char* data, uint32 offset)
	//	{
	//		get(&platformID, data, offset);
	//		get(&encodingID, data, offset);
	//		get(&languageID, data, offset);
	//		get(&nameID, data, offset);
	//		get(&length, data, offset);
	//		get(&offset_value, data, offset);
	//		return offset;
	//	}
	//};
	//
	//struct ColorTable
	//{
	//	uint16 numBaseGlyphRecords;
	//	uint32 baseGlyphRecordsOffset;
	//	uint32 layerRecordsOffset;
	//	uint16 numLayerRecords;
	//
	//	uint32 Parse(const char* data, uint32 offset)
	//	{
	//		offset += 2; //version
	//		get(&numBaseGlyphRecords, data, offset);
	//		get(&baseGlyphRecordsOffset, data, offset);
	//		get(&layerRecordsOffset, data, offset);
	//		get(&numLayerRecords, data, offset);
	//		return offset;
	//	}
	//};
	//
	//struct NameTable
	//{
	//	uint16 Format;
	//	uint16 count;
	//	uint16 stringOffset;
	//	std::vector<NameValue> NameRecords;
	//
	//	uint32 Parse(const char* data, uint32 offset, std::string* names, uint16 maxNumberOfNames = 25)
	//	{
	//		uint32 offset_start = offset;
	//		get(&Format, data, offset);
	//		get(&count, data, offset);
	//		get(&stringOffset, data, offset);
	//		NameRecords.resize(count);
	//		for (uint32 i = 0; i < count; i++)
	//		{
	//			if (NameRecords[i].nameID > maxNumberOfNames) { continue; }
	//
	//			offset = NameRecords[i].Parse(data, offset);
	//			char* newNameString = new char[NameRecords[i].length];
	//			memcpy(newNameString, data + offset_start + stringOffset + NameRecords[i].offset_value, sizeof(char) * NameRecords[i].length);
	//			uint16 string_length = NameRecords[i].length;
	//
	//			if (newNameString[0] == 0) {
	//				string_length = string_length >> 1;
	//
	//				for (uint16 j = 0; j < string_length; j++) {
	//					newNameString[j] = newNameString[j * 2 + 1];
	//				}
	//			}
	//
	//			names[NameRecords[i].nameID].assign(newNameString, string_length);
	//
	//			delete[] newNameString;
	//		}
	//		return offset;
	//	}
	//};
	//
	//struct HHEATable
	//{
	//	uint16 majorVersion;
	//	uint16 minorVersion;
	//	int16 Ascender;
	//	int16 Descender;
	//	int16 LineGap;
	//	uint16 advanceWidthMax;
	//	int16 minLeftSideBearing;
	//	int16 minRightSideBearing;
	//	int16 xMaxExtent;
	//	int16 caretSlopeRise;
	//	int16 caretSlopeRun;
	//	int16 caretOffset;
	//	int16 metricDataFormat;
	//	uint16 numberOfHMetrics;
	//
	//	uint32 Parse(const char* data, uint32 offset)
	//	{
	//		get(&majorVersion, data, offset); offset += sizeof(uint16);
	//		get(&minorVersion, data, offset); offset += sizeof(uint16);
	//		get(&Ascender, data, offset); offset += sizeof(int16);
	//		get(&Descender, data, offset); offset += sizeof(int16);
	//		get(&LineGap, data, offset); offset += sizeof(int16);
	//		get(&advanceWidthMax, data, offset); offset += sizeof(uint16);
	//		get(&minLeftSideBearing, data, offset); offset += sizeof(int16);
	//		get(&minRightSideBearing, data, offset); offset += sizeof(int16);
	//		get(&xMaxExtent, data, offset); offset += sizeof(int16);
	//		get(&caretSlopeRise, data, offset); offset += sizeof(int16);
	//		get(&caretSlopeRun, data, offset); offset += sizeof(int16);
	//		get(&caretOffset, data, offset); offset += sizeof(int16);
	//		offset += sizeof(int16) * 4;
	//		get(&metricDataFormat, data, offset); offset += sizeof(int16);
	//		get(&numberOfHMetrics, data, offset); offset += sizeof(uint16);
	//		return offset;
	//	}
	//};
	//
	//struct float_v4
	//{
	//	float32 data[4];
	//};
	//
	//
	//
	//struct FontLineInfoData
	//{
	//	uint32 StringStartIndex;
	//	uint32 StringEndIndex;
	//	Vector2 OffsetStart;
	//	Vector2 OffsetEnd;
	//	Vector<Glyph*, DefaultAllocatorReference> GlyphIndex;
	//};
	//
	//struct FontPositioningOutput
	//{
	//	Vector<FontLineInfoData, DefaultAllocatorReference> LinePositions;
	//	uint32 NumTriangles;
	//	//PixelPositioning alignment;
	//	//BoundingRect bounding_rect;
	//	uint32 Geometry;
	//	uint16 FontSize;
	//};
	//
	//struct FontPositioningOptions
	//{
	//	bool IsMultiline;
	//	bool IsWordPreserve;
	//	float32 LineHeight;
	//	//PixelPositioning alignment;
	//	//BoundingRect bounding_rect;
	//	FontPositioningOptions()
	//	{
	//		IsMultiline = true;
	//		IsWordPreserve = true;
	//		LineHeight = 1.0f;
	//	}
	//};
	//
	//inline int16 GetKerningOffset(Font* font_data, uint16 left_glyph, uint16 right_glyph)
	//{
	//	auto kern_data = font_data->KerningTable.TryGet((left_glyph << 16) | right_glyph);
	//	return kern_data.State() ? kern_data.Get() : 0;
	//}
	//
	//inline bool MakeFont(const char* data, Font* fontData) {
	//	uint32 ptr = 0;
	//
	//	TTFHeader header;
	//	ptr = header.Parse(data, ptr);
	//
	//	std::unordered_map<std::string, TableEntry> tables;
	//	for (uint16 i = 0; i < header.NumberOfTables; i++)
	//	{
	//		TableEntry te;
	//		ptr = te.Parse(data, ptr);
	//		tables[te.tagstr] = te;
	//	}
	//
	//	auto headTableEntry = tables.find("head");
	//	if (headTableEntry == tables.end()) { return false; }
	//	HeadTable headTable;
	//	ptr = headTable.Parse(data, headTableEntry->second.offsetPos);
	//
	//	auto maxpTableEntry = tables.find("maxp");
	//	if (maxpTableEntry == tables.end()) { return false; }
	//	MaximumProfile maximumProfile;
	//	maximumProfile.Parse(data, maxpTableEntry->second.offsetPos);
	//
	//	auto nameTableEntry = tables.find("name");
	//	if (nameTableEntry == tables.end()) { return false; }
	//	NameTable nameTable;
	//	nameTable.Parse(data, nameTableEntry->second.offsetPos, fontData->NameTable);
	//
	//	fontData->FullFontName = fontData->NameTable[1] + " " + fontData->NameTable[2];
	//
	//	auto locaTableEntry = tables.find("loca");
	//	if (locaTableEntry == tables.end()) { return false; }
	//
	//	if (!maximumProfile.numGlyphs) { return false; }
	//
	//	std::vector<uint32> glyphIndices(maximumProfile.numGlyphs);
	//
	//	uint32 endOfGlyf = 0;
	//
	//	if (headTable.indexToLocFormat == 0)
	//	{
	//		uint32 byte_offset = locaTableEntry->second.offsetPos;
	//
	//		for (uint16 i = 0; i < maximumProfile.numGlyphs; i++)
	//		{
	//			get(&glyphIndices[i], data + byte_offset, byte_offset);
	//			glyphIndices[i] = glyphIndices[i] << 1;
	//		}
	//
	//		get(&endOfGlyf, data + byte_offset, byte_offset);
	//		endOfGlyf = endOfGlyf << 1;
	//	}
	//	else
	//	{
	//		uint32 byte_offset = locaTableEntry->second.offsetPos;
	//		for (uint16 i = 0; i < maximumProfile.numGlyphs; i++)
	//		{
	//			get(&glyphIndices[i], data + byte_offset, byte_offset);
	//		}
	//		get(&endOfGlyf, data + byte_offset, byte_offset);
	//	}
	//
	//	auto cmapTableEntry = tables.find("cmap");
	//	if (cmapTableEntry == tables.end()) { return false; }
	//
	//	uint32 cmap_offset = cmapTableEntry->second.offsetPos + sizeof(uint16); //Skip version
	//	uint16 cmap_num_tables;
	//	get(&cmap_num_tables, data, cmap_offset);
	//
	//	std::map<uint16, uint32> glyphReverseMap;
	//
	//	bool valid_cmap_table = false;
	//	for (uint16 i = 0; i < cmap_num_tables; i++)
	//	{
	//		constexpr uint8 UNICODE_PLATFORM_INDEX = 0; constexpr uint8 WIN32_PLATFORM_INDEX = 3; constexpr uint8 WIN32_UNICODE_ENCODING = 1;
	//
	//		uint16 platformID, encodingID;
	//		uint32 cmap_subtable_offset;
	//		get(&platformID, data + cmap_offset, cmap_offset);
	//		get(&encodingID, data + cmap_offset, cmap_offset);
	//		get(&cmap_subtable_offset, data + cmap_offset, cmap_offset);
	//
	//		if (!((platformID == UNICODE_PLATFORM_INDEX && encodingID == 3/*\(··)/*/) || (platformID == WIN32_PLATFORM_INDEX && encodingID == WIN32_UNICODE_ENCODING))) { continue; }
	//
	//		cmap_subtable_offset += cmapTableEntry->second.offsetPos;
	//		uint16 format, length;
	//		get(&format, data + cmap_subtable_offset, cmap_subtable_offset);
	//		get(&length, data + cmap_subtable_offset, cmap_subtable_offset);
	//
	//		if (format != 4) { continue; }
	//
	//		uint16 language, segCountX2;// , searchRange, entrySelector, rangeShift;
	//		get(&language, data + cmap_subtable_offset, cmap_subtable_offset);
	//		get(&segCountX2, data + cmap_subtable_offset, cmap_subtable_offset);
	//		//get(&searchRange, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
	//		//get(&entrySelector, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
	//		//get(&rangeShift, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
	//		cmap_subtable_offset += sizeof(uint16) * 3;
	//
	//		uint16 segCount = segCountX2 >> 1;
	//		std::vector<uint16> endCount(segCount), startCount(segCount), idRangeOffset(segCount);
	//		std::vector<int16> idDelta(segCount);
	//		for (uint16 j = 0; j < segCount; j++)
	//		{
	//			auto temp = 0u;
	//			get(&endCount[j], data + cmap_subtable_offset, temp);
	//		}
	//
	//		cmap_subtable_offset += sizeof(uint16);
	//
	//		for (uint16 j = 0; j < segCount; j++)
	//		{
	//			{
	//				uint32 cmapso = cmap_subtable_offset;
	//				get(&startCount[j], data, cmapso);
	//			}
	//
	//			{
	//				uint32 temp = cmap_subtable_offset + sizeof(uint16) * segCount;
	//				get(&idDelta[j], data, temp);
	//			}
	//
	//			{
	//				uint32 temp = cmap_subtable_offset + sizeof(uint16) * segCount * 2;
	//				get(&idRangeOffset[j], data, temp);
	//			}
	//
	//			if (idRangeOffset[j] == 0)
	//			{
	//				for (uint32 k = startCount[j]; k <= endCount[j]; k++)
	//				{
	//					fontData->GlyphMap.Emplace(k, k + idDelta[j]);
	//					glyphReverseMap[k + idDelta[j]] = k;
	//				}
	//			}
	//			else
	//			{
	//				uint32 glyph_address_offset = cmap_subtable_offset + sizeof(uint16) * segCount * 2; //idRangeOffset_ptr
	//				for (uint32 k = startCount[j]; k <= endCount[j]; k++)
	//				{
	//					uint32 glyph_address_index_offset = idRangeOffset[j] + 2 * (k - startCount[j]) + glyph_address_offset;
	//					uint16& glyph_map_value = fontData->GlyphMap[k];
	//					auto temp = glyph_address_index_offset;
	//					get(&glyph_map_value, data, temp);
	//					glyphReverseMap[glyph_map_value] = k;
	//					glyph_map_value += idDelta[j];
	//				}
	//			}
	//
	//			cmap_subtable_offset += sizeof(uint16);
	//		}
	//
	//		valid_cmap_table = true;
	//		break;
	//	}
	//	if (!valid_cmap_table) { return false; }
	//
	//	auto hheaTableEntry = tables.find("hhea");
	//	if (hheaTableEntry == tables.end()) { return false; }
	//	HHEATable hheaTable;
	//	uint32 hhea_offset = hheaTable.Parse(data, hheaTableEntry->second.offsetPos);
	//
	//	auto glyfTableEntry = tables.find("glyf");
	//	if (glyfTableEntry == tables.end()) { return false; }
	//	uint32 glyf_offset = glyfTableEntry->second.offsetPos;
	//
	//	auto kernTableEntry = tables.find("kern");
	//	uint32 kernOffset = 0;
	//	if (kernTableEntry != tables.end())
	//	{
	//		kernOffset = kernTableEntry->second.offsetPos;
	//	}
	//
	//	auto hmtxTableEntry = tables.find("hmtx");
	//	if (hmtxTableEntry == tables.end()) { return false; }
	//	uint32 hmtx_offset = hmtxTableEntry->second.offsetPos;
	//	uint16 last_glyph_advance_width = 0;
	//
	//	std::vector<std::vector<uint16>> pointsPerContour(GTSL::Math::Limit(maximumProfile.maxContours, (uint16)4096));
	//	std::vector<uint16> pointsInContour(GTSL::Math::Limit(maximumProfile.maxContours, (uint16)4096));
	//
	//	GTSL::Vector<bool, DefaultAllocatorReference> glyphLoaded(maximumProfile.numGlyphs);
	//	for (uint32 i = 0; i < maximumProfile.numGlyphs; ++i) {
	//		glyphLoaded.EmplaceBack(false);
	//	}
	//
	//	auto parseGlyph = [&](uint32 glyphIndex, auto&& self) -> int8 {
	//		if (glyphLoaded[glyphIndex]) { return 1; }
	//
	//		Glyph& currentGlyph = fontData->Glyphs.Emplace(glyphIndex, DefaultAllocatorReference());
	//		currentGlyph.GlyphIndex = static_cast<int16>(glyphIndex);
	//		currentGlyph.Character = glyphReverseMap[static_cast<int16>(glyphIndex)];
	//
	//		if (glyphIndex < hheaTable.numberOfHMetrics) {
	//			{
	//				uint32 temp = hmtx_offset + glyphIndex * sizeof(uint32);
	//				get(&currentGlyph.AdvanceWidth, data, temp);
	//			}
	//
	//			last_glyph_advance_width = currentGlyph.AdvanceWidth;
	//
	//			{
	//				uint32 temp = hmtx_offset + glyphIndex * sizeof(uint32) + sizeof(uint16);
	//				get(&currentGlyph.LeftSideBearing, data, temp);
	//			}
	//		}
	//		else {
	//			currentGlyph.AdvanceWidth = last_glyph_advance_width;
	//		}
	//
	//		if (glyphIndex != maximumProfile.numGlyphs - 1 && glyphIndices[glyphIndex] == glyphIndices[glyphIndex + 1]) {
	//			glyphLoaded[glyphIndex] = true;
	//			return false;
	//		}
	//
	//		if (glyphIndices[glyphIndex] >= endOfGlyf) { return false; }
	//
	//		uint32 currentOffset = glyf_offset + glyphIndices[glyphIndex];
	//
	//		get(&currentGlyph.NumContours, data, currentOffset);
	//
	//		{
	//			int16 bbox[4];
	//			get(&bbox[0], data, currentOffset); /*xMin*/ get(&bbox[1], data, currentOffset); //yMin
	//			get(&bbox[2], data, currentOffset); /*xMax*/ get(&bbox[3], data, currentOffset); //yMax
	//
	//			currentGlyph.BoundingBox[0].X() = bbox[0]; currentGlyph.BoundingBox[0].Y() = bbox[1];
	//			currentGlyph.BoundingBox[1].X() = bbox[2]; currentGlyph.BoundingBox[1].Y() = bbox[3];
	//
	//			currentGlyph.Center.X() = (currentGlyph.BoundingBox[0].X() + currentGlyph.BoundingBox[1].X()) / 2.0f;
	//			currentGlyph.Center.Y() = (currentGlyph.BoundingBox[0].Y() + currentGlyph.BoundingBox[1].Y()) / 2.0f;
	//		}
	//
	//		if (currentGlyph.NumContours > 0) //simple glyph
	//		{
	//			currentGlyph.Paths.Resize(currentGlyph.NumContours);
	//
	//			std::vector<uint16> contourEnd(currentGlyph.NumContours);
	//
	//			for (uint16 j = 0; j < currentGlyph.NumContours; j++) { get(&contourEnd[j], data + currentOffset, currentOffset); }
	//
	//			for (uint16 contourIndex = 0; contourIndex < currentGlyph.NumContours; contourIndex++) {
	//				uint16 num_points = contourEnd[contourIndex] - (contourIndex ? contourEnd[contourIndex - 1] : -1);
	//
	//				if (pointsPerContour[contourIndex].size() < num_points) {
	//					pointsPerContour[contourIndex].resize(num_points);
	//				}
	//
	//				pointsInContour[contourIndex] = num_points;
	//			}
	//
	//			//Skip instructions
	//			uint16 num_instructions;
	//			get(&num_instructions, data, currentOffset);
	//			currentOffset += sizeof(uint8) * num_instructions;
	//
	//			uint16 numPoints = contourEnd[static_cast<int64>(currentGlyph.NumContours) - 1] + 1;
	//			std::vector<uint8> flags(numPoints);
	//			std::vector<TTFFlags> flagsEnum(numPoints);
	//			std::vector<uint16> contour_index(numPoints);
	//			uint16 current_contour_index = 0;
	//			int16 repeat = 0;
	//			uint16 contour_count_first_point = 0;
	//
	//			for (uint16 j = 0; j < numPoints; j++, ++contour_count_first_point) {
	//				if (!repeat) {
	//					get(&flags[j], data + currentOffset, currentOffset);
	//
	//					if (flags[j] & 0x8) { get(&repeat, data + currentOffset, currentOffset); }
	//				}
	//				else {
	//					flags[j] = flags[j - 1];
	//					repeat--;
	//				}
	//
	//				flagsEnum[j].isControlPoint = (!(flags[j] & 0b00000001)) != 0;
	//				flagsEnum[j].xShort = (flags[j] & 0b00000010) != 0;
	//				flagsEnum[j].yShort = (flags[j] & 0b00000100) != 0;
	//				flagsEnum[j].repeat = (flags[j] & 0b00001000) != 0;
	//				flagsEnum[j].xDual = (flags[j] & 0b00010000) != 0;
	//				flagsEnum[j].yDual = (flags[j] & 0b00100000) != 0;
	//
	//				if (j > contourEnd[current_contour_index])
	//				{
	//					current_contour_index++;
	//					contour_count_first_point = 0;
	//				}
	//
	//				contour_index[j] = current_contour_index;
	//				pointsPerContour[current_contour_index][contour_count_first_point] = j;
	//			}
	//
	//			std::vector<ShortVector> glyphPoints(numPoints);
	//
	//			for (uint16 j = 0; j < numPoints; ++j) {
	//				if (flagsEnum[j].xDual && !flagsEnum[j].xShort) {
	//					glyphPoints[j].X = j ? glyphPoints[j - 1].X : 0;
	//				}
	//				else {
	//					if (flagsEnum[j].xShort) {
	//						get(&glyphPoints[j].X, data, currentOffset);
	//					}
	//					else {
	//						get(&glyphPoints[j].X, data, currentOffset);
	//					}
	//
	//					if (flagsEnum[j].xShort && !flagsEnum[j].xDual) { glyphPoints[j].X *= -1; }
	//
	//					if (j != 0) { glyphPoints[j].X += glyphPoints[j - 1].X; }
	//				}
	//			}
	//
	//			for (uint16 j = 0; j < numPoints; j++) {
	//				if (flagsEnum[j].yDual && !flagsEnum[j].yShort) {
	//					glyphPoints[j].Y = j ? glyphPoints[j - 1].Y : 0;
	//				}
	//				else {
	//					if (flagsEnum[j].yShort) {
	//						get(&glyphPoints[j].Y, data, currentOffset);
	//					}
	//					else {
	//						get(&glyphPoints[j].Y, data, currentOffset);
	//					}
	//
	//					if (flagsEnum[j].yShort && !flagsEnum[j].yDual) { glyphPoints[j].Y *= -1; }
	//
	//					if (j != 0) { glyphPoints[j].Y += glyphPoints[j - 1].Y; }
	//				}
	//			}
	//
	//			//Generate contours
	//			for (uint16 contourIndex = 0; contourIndex < currentGlyph.NumContours; ++contourIndex) {
	//				currentGlyph.Paths.EmplaceBack(pointsInContour[contourIndex]);
	//				//can be broken
	//
	//				const uint16 numPointsInContour = pointsInContour[contourIndex];
	//
	//				auto& contourPointsFlags = flagsEnum;
	//
	//				uint16 pointInIndices = 0;
	//
	//				//If the first point is control point
	//				while (contourPointsFlags[pointsPerContour[contourIndex][pointInIndices]].isControlPoint) { ++pointInIndices; }
	//
	//				bool lastPointWasControlPoint = false, thisPointIsControlPoint = false;
	//
	//				Segment currentCurve;
	//				currentCurve.Points[0] = toVector(glyphPoints[pointsPerContour[contourIndex][pointInIndices]]); //what if no more points
	//
	//				++pointInIndices;
	//
	//				for (uint32 p = 0; p < numPointsInContour; ++p, ++pointInIndices) {
	//					uint32 safeIndexToData = pointsPerContour[contourIndex][pointInIndices % numPointsInContour];
	//
	//					thisPointIsControlPoint = contourPointsFlags[safeIndexToData].isControlPoint;
	//
	//					if (thisPointIsControlPoint) {
	//						if (lastPointWasControlPoint) {
	//							auto thisPoint = toVector(glyphPoints[safeIndexToData]);
	//							auto newPoint = (thisPoint + currentCurve.Points[1]) * 0.5f;
	//							currentCurve.Points[2] = newPoint;
	//							currentCurve.IsCurve = true;
	//							currentGlyph.Paths[contourIndex].EmplaceBack(currentCurve);
	//
	//							currentCurve.Points[0] = newPoint;
	//
	//							currentCurve.Points[1] = thisPoint;
	//						}
	//						else {
	//							currentCurve.Points[1] = toVector(glyphPoints[safeIndexToData]);
	//						}
	//					}
	//					else {
	//						if (lastPointWasControlPoint) {
	//							auto thisPoint = toVector(glyphPoints[safeIndexToData]);
	//							currentCurve.Points[2] = thisPoint;
	//							currentCurve.IsCurve = true;
	//							currentGlyph.Paths[contourIndex].EmplaceBack(currentCurve);
	//
	//							currentCurve.Points[0] = thisPoint;
	//						}
	//						else {
	//							auto thisPoint = toVector(glyphPoints[safeIndexToData]);
	//							currentCurve.Points[1] = GTSL::Vector2(0, 0);
	//							currentCurve.Points[2] = thisPoint;
	//							currentCurve.IsCurve = false;
	//							currentGlyph.Paths[contourIndex].EmplaceBack(currentCurve);
	//
	//							currentCurve.Points[0] = thisPoint;
	//						}
	//					}
	//
	//					lastPointWasControlPoint = thisPointIsControlPoint;
	//				}
	//			} //for contour
	//		}
	//		else { //Composite glyph
	//			for (auto compound_glyph_index = 0; compound_glyph_index < -currentGlyph.NumContours; compound_glyph_index++) {
	//				uint16 glyfFlags, glyphIndex;
	//
	//				do {
	//					get(&glyfFlags, data, currentOffset);
	//					get(&glyphIndex, data, currentOffset);
	//
	//					int16 glyfArgs1 = 0, glyfArgs2 = 0;
	//					int8_t glyfArgs1U8 = 0, glyfArgs2U8 = 0;
	//					bool is_word = false;
	//					if (glyfFlags & ARG_1_AND_2_ARE_WORDS)
	//					{
	//						get(&glyfArgs1, data, currentOffset);
	//						get(&glyfArgs2, data, currentOffset);
	//						is_word = true;
	//					}
	//					else
	//					{
	//						get(&glyfArgs1U8, data, currentOffset);
	//						get(&glyfArgs2U8, data, currentOffset);
	//					}
	//
	//					float32 compositeGlyphElementTransformation[6] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
	//
	//					if (glyfFlags & WE_HAVE_A_SCALE)
	//					{
	//						int16 xy_value = 0;
	//						get(&xy_value, data, currentOffset);
	//						compositeGlyphElementTransformation[0] = to_2_14_float(xy_value);
	//						compositeGlyphElementTransformation[3] = to_2_14_float(xy_value);
	//					}
	//					else if (glyfFlags & WE_HAVE_AN_X_AND_Y_SCALE)
	//					{
	//						int16 xy_values[2];
	//						get(&xy_values[0], data, currentOffset);
	//						get(&xy_values[1], data, currentOffset);
	//						compositeGlyphElementTransformation[0] = to_2_14_float(xy_values[0]);
	//						compositeGlyphElementTransformation[3] = to_2_14_float(xy_values[1]);
	//					}
	//					else if (glyfFlags & WE_HAVE_A_TWO_BY_TWO)
	//					{
	//						int16 xy_values[4];
	//						get(&xy_values[0], data, currentOffset);
	//						get(&xy_values[1], data, currentOffset);
	//						get(&xy_values[2], data, currentOffset);
	//						get(&xy_values[3], data, currentOffset);
	//						compositeGlyphElementTransformation[0] = to_2_14_float(xy_values[0]);
	//						compositeGlyphElementTransformation[1] = to_2_14_float(xy_values[1]);
	//						compositeGlyphElementTransformation[2] = to_2_14_float(xy_values[2]);
	//						compositeGlyphElementTransformation[3] = to_2_14_float(xy_values[3]);
	//					}
	//
	//					bool matched_points = false;
	//					if (glyfFlags & ARGS_ARE_XY_VALUES)
	//					{
	//						compositeGlyphElementTransformation[4] = is_word ? glyfArgs1 : glyfArgs1U8;
	//						compositeGlyphElementTransformation[5] = is_word ? glyfArgs2 : glyfArgs2U8;
	//						if (glyfFlags & SCALED_COMPONENT_OFFSET)
	//						{
	//							compositeGlyphElementTransformation[4] *= compositeGlyphElementTransformation[0];
	//							compositeGlyphElementTransformation[5] *= compositeGlyphElementTransformation[3];
	//						}
	//					}
	//					else
	//					{
	//						matched_points = true;
	//					}
	//
	//					//Skip instructions
	//					if (glyfFlags & WE_HAVE_INSTRUCTIONS)
	//					{
	//						uint16 num_instructions = 0;
	//						get(&num_instructions, data, currentOffset);
	//						currentOffset += sizeof(uint8_t) * num_instructions;
	//					}
	//
	//					if (glyphLoaded[glyphIndex] == false)
	//					{
	//						if (self(glyphIndex, self) < 0) {
	//							//BE_LOG_WARNING("ttf-parser: bad glyph index ", glyphIndex, " in composite glyph");
	//							continue;
	//						}
	//					}
	//
	//					Glyph& compositeGlyphElement = fontData->Glyphs[glyphIndex];
	//
	//					auto transformCurve = [&compositeGlyphElementTransformation](Segment& curve) -> Segment
	//					{
	//						Segment out;
	//						out.Points[0].X() = curve.Points[0].X() * compositeGlyphElementTransformation[0] + curve.Points[0].X() * compositeGlyphElementTransformation[1] + compositeGlyphElementTransformation[4];
	//						out.Points[0].Y() = curve.Points[0].Y() * compositeGlyphElementTransformation[2] + curve.Points[0].Y() * compositeGlyphElementTransformation[3] + compositeGlyphElementTransformation[5];
	//						out.Points[1].X() = curve.Points[1].X() * compositeGlyphElementTransformation[0] + curve.Points[1].Y() * compositeGlyphElementTransformation[1] + compositeGlyphElementTransformation[4];
	//						out.Points[1].Y() = curve.Points[1].X() * compositeGlyphElementTransformation[2] + curve.Points[1].Y() * compositeGlyphElementTransformation[3] + compositeGlyphElementTransformation[5];
	//						out.Points[2].X() = curve.Points[2].X() * compositeGlyphElementTransformation[0] + curve.Points[2].Y() * compositeGlyphElementTransformation[1] + compositeGlyphElementTransformation[4];
	//						out.Points[2].Y() = curve.Points[2].X() * compositeGlyphElementTransformation[2] + curve.Points[2].Y() * compositeGlyphElementTransformation[3] + compositeGlyphElementTransformation[5];
	//						return out;
	//					};
	//
	//					uint32 compositeGlyphPathCount = compositeGlyphElement.Paths.GetLength();
	//					for (uint32 glyphPointIndex = 0; glyphPointIndex < compositeGlyphPathCount; glyphPointIndex++) {
	//						auto& currentCurvesList = compositeGlyphElement.Paths[glyphPointIndex];
	//
	//						uint32 compositeGlyphPathCurvesCount = currentCurvesList.GetLength();
	//
	//						Path newPath;
	//						if (!matched_points) {
	//							newPath.Resize(compositeGlyphPathCurvesCount);
	//
	//							for (uint32 glyphCurvesPointIndex = 0; glyphCurvesPointIndex < compositeGlyphPathCurvesCount; glyphCurvesPointIndex++) {
	//								newPath.EmplaceBack(transformCurve(currentCurvesList[glyphCurvesPointIndex]));
	//							}
	//						}
	//						else {
	//							//BE_LOG_WARNING("ttf-parser: unsupported matched points in ttf composite glyph");
	//							continue;
	//						}
	//
	//						currentGlyph.Paths.EmplaceBack(newPath);
	//					}
	//				} while (glyfFlags & MORE_COMPONENTS);
	//			}
	//		}
	//
	//		glyphLoaded[glyphIndex] = true;
	//
	//		return 0;
	//	};
	//
	//	for (uint16 i = 0; i < maximumProfile.numGlyphs; i++) { parseGlyph(i, parseGlyph); }
	//
	//	//Kerning table
	//	if (kernOffset)
	//	{
	//		uint32 currentOffset = kernOffset;
	//		uint16 kern_table_version, num_kern_subtables;
	//		get(&kern_table_version, data + currentOffset, currentOffset);
	//		get(&num_kern_subtables, data + currentOffset, currentOffset);
	//		uint16 kern_length = 0;
	//		uint32 kernStartOffset = currentOffset;
	//
	//		for (uint16 kerningSubTableIndex = 0; kerningSubTableIndex < num_kern_subtables; kerningSubTableIndex++)
	//		{
	//			uint16 kerningVersion, kerningCoverage;
	//			currentOffset = kernStartOffset + kern_length;
	//			kernStartOffset = currentOffset;
	//			get(&kerningVersion, data, currentOffset);
	//			get(&kern_length, data, currentOffset);
	//			if (kerningVersion != 0)
	//			{
	//				currentOffset += kern_length - sizeof(uint16) * 3;
	//				continue;
	//			}
	//			get(&kerningCoverage, data, currentOffset);
	//
	//			uint16 num_kern_pairs;
	//			get(&num_kern_pairs, data, currentOffset);
	//			currentOffset += sizeof(uint16) * 3;
	//			for (uint16 kern_index = 0; kern_index < num_kern_pairs; kern_index++)
	//			{
	//				uint16 kern_left, kern_right;
	//				int16 kern_value;
	//				get(&kern_left, data, currentOffset);
	//				get(&kern_right, data, currentOffset);
	//				get(&kern_value, data, currentOffset);
	//
	//				fontData->KerningTable.Emplace((kern_left << 16) | kern_right, kern_value);
	//			}
	//		}
	//	}
	//
	//	fontData->Metadata.UnitsPerEm = headTable.unitsPerEm;
	//	fontData->Metadata.Ascender = hheaTable.Ascender;
	//	fontData->Metadata.Descender = hheaTable.Descender;
	//	fontData->Metadata.LineGap = hheaTable.LineGap;
	//
	//	return true;
	//}


}