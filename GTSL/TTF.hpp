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
	struct ShortVector {
		int16 X, Y;

		bool operator==(const ShortVector&) const = default;
	};

	inline void read(int64* dst, const char* src, uint32* offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[*offset + 7];
			reinterpret_cast<uint8_t*>(dst)[1] = src[*offset + 6];
			reinterpret_cast<uint8_t*>(dst)[2] = src[*offset + 5];
			reinterpret_cast<uint8_t*>(dst)[3] = src[*offset + 4];
			reinterpret_cast<uint8_t*>(dst)[4] = src[*offset + 3];
			reinterpret_cast<uint8_t*>(dst)[5] = src[*offset + 2];
			reinterpret_cast<uint8_t*>(dst)[6] = src[*offset + 1];
			reinterpret_cast<uint8_t*>(dst)[7] = src[*offset + 0];
		} else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
			reinterpret_cast<uint8_t*>(dst)[4] = src[4];
			reinterpret_cast<uint8_t*>(dst)[5] = src[5];
			reinterpret_cast<uint8_t*>(dst)[6] = src[6];
			reinterpret_cast<uint8_t*>(dst)[7] = src[7];
		}
	
		*offset += 8;
	}

	inline void read(uint64* dst, const char* src, uint32* offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[*offset + 7];
			reinterpret_cast<uint8_t*>(dst)[1] = src[*offset + 6];
			reinterpret_cast<uint8_t*>(dst)[2] = src[*offset + 5];
			reinterpret_cast<uint8_t*>(dst)[3] = src[*offset + 4];
			reinterpret_cast<uint8_t*>(dst)[4] = src[*offset + 3];
			reinterpret_cast<uint8_t*>(dst)[5] = src[*offset + 2];
			reinterpret_cast<uint8_t*>(dst)[6] = src[*offset + 1];
			reinterpret_cast<uint8_t*>(dst)[7] = src[*offset + 0];
		} else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
			reinterpret_cast<uint8_t*>(dst)[4] = src[4];
			reinterpret_cast<uint8_t*>(dst)[5] = src[5];
			reinterpret_cast<uint8_t*>(dst)[6] = src[6];
			reinterpret_cast<uint8_t*>(dst)[7] = src[7];
		}
	
		*offset += 8;
	}

	inline void read(float32* dst, const char* src, uint32* offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[*offset + 3];
			reinterpret_cast<uint8_t*>(dst)[1] = src[*offset + 2];
			reinterpret_cast<uint8_t*>(dst)[2] = src[*offset + 1];
			reinterpret_cast<uint8_t*>(dst)[3] = src[*offset + 0];
		} else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
		}
	
		*offset += 4;
	}

	inline void read(uint32* dst, const char* src, uint32* offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[*offset + 3];
			reinterpret_cast<uint8_t*>(dst)[1] = src[*offset + 2];
			reinterpret_cast<uint8_t*>(dst)[2] = src[*offset + 1];
			reinterpret_cast<uint8_t*>(dst)[3] = src[*offset + 0];
		} else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
			reinterpret_cast<uint8_t*>(dst)[2] = src[2];
			reinterpret_cast<uint8_t*>(dst)[3] = src[3];
		}
	
		*offset += 4;
	}

	inline void read(int16* dst, const char* src, uint32* offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[*offset + 1];
			reinterpret_cast<uint8_t*>(dst)[1] = src[*offset + 0];
		} else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
		}
	
		*offset += 2;
	}

	inline void read(uint16* dst, const char* src, uint32* offset) {
		if constexpr (_WIN64) {
			reinterpret_cast<uint8_t*>(dst)[0] = src[*offset + 1];
			reinterpret_cast<uint8_t*>(dst)[1] = src[*offset + 0];
		} else {
			reinterpret_cast<uint8_t*>(dst)[0] = src[0];
			reinterpret_cast<uint8_t*>(dst)[1] = src[1];
		}
	
		*offset += 2;
	}

	inline void read(signed char* dst, const char* src, uint32* offset) {
		dst[0] = src[*offset + 0];
		++(*offset);
	}

	inline void read(uint8* dst, const char* src, uint32* offset) {
		dst[0] = src[*offset + 0];
		++(*offset);
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

		void Parse(const char* data, uint32& offset) {
			read(&Version, data, &offset);
			read(&NumberOfTables, data, &offset);
			/*get2b(&searchRange, data + offset); offset += sizeof(uint16);
			get2b(&entrySelector, data + offset); offset += sizeof(uint16);
			get2b(&rangeShift, data + offset); offset += sizeof(uint16);*/
			offset += sizeof(uint16) * 3;
		}
	};

	struct TableEntry {
		uint32 tag;
		char8_t tagstr[5];
		uint32 checkSum;
		uint32 offsetPos;
		uint32 length;

		void Parse(const char* data, uint32& offset) {
			read(&tag, data, &offset); offset -= 4;
			read(reinterpret_cast<uint8*>(&tagstr[0]), data, &offset); read(reinterpret_cast<uint8*>(&tagstr[1]), data, &offset); read(reinterpret_cast<uint8*>(&tagstr[2]), data, &offset); read(reinterpret_cast<uint8*>(&tagstr[3]), data, &offset);
			tagstr[4] = 0;
			read(&checkSum, data, &offset);
			read(&offsetPos, data, &offset);
			read(&length, data, &offset);
		}
	};

	struct HeadTable {
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

		void Parse(const char* data, uint32& offset) {
			read(&tableVersion, data, &offset);
			read(&fontRevision, data, &offset);
			read(&checkSumAdjustment, data, &offset);
			read(&magicNumber, data, &offset);
			read(&flags, data, &offset);
			read(&unitsPerEm, data, &offset);
			read(&createdDate, data, &offset);
			read(&modifiedData, data, &offset);
			read(&xMin, data, &offset);
			read(&yMin, data, &offset);
			read(&xMax, data, &offset);
			read(&yMax, data, &offset);
			read(&macStyle, data, &offset);
			read(&lowestRecPPEM, data, &offset);
			read(&fontDirectionHintl, data, &offset);
			read(&indexToLocFormat, data, &offset);
			read(&glyphDataFormat, data, &offset);
		}
	};

	struct MaximumProfile {
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
			read(&version, data, &offset);
			read(&numGlyphs, data, &offset);
			read(&maxPoints, data, &offset);
			read(&maxContours, data, &offset);
			read(&maxCompositePoints, data, &offset);
			read(&maxCompositeContours, data, &offset);
			read(&maxZones, data, &offset);
			read(&maxTwilightPoints, data, &offset);
			read(&maxStorage, data, &offset);
			read(&maxFunctionDefs, data, &offset);
			read(&maxInstructionDefs, data, &offset);
			read(&maxStackElements, data, &offset);
			read(&maxSizeOfInstructions, data, &offset);
			read(&maxComponentElements, data, &offset);
			read(&maxComponentDepth, data, &offset);
		}
	};

	struct NameValue {
		uint16 platformID;
		uint16 encodingID;
		uint16 languageID;
		uint16 nameID;
		uint16 length;
		uint16 offset_value;

		void Parse(const char* data, uint32& offset) {
			read(&platformID, data, &offset);
			read(&encodingID, data, &offset);
			read(&languageID, data, &offset);
			read(&nameID, data, &offset);
			read(&length, data, &offset);
			read(&offset_value, data, &offset);
		}
	};

	struct NameTable {
		uint16 Format;
		uint16 count;
		uint16 stringOffset;
		std::vector<NameValue> NameRecords;

		void Parse(const char* data, uint32& offset, std::string* names, uint16 maxNumberOfNames = 25) {
			uint32 offset_start = offset;
			read(&Format, data, &offset);
			read(&count, data, &offset);
			read(&stringOffset, data, &offset);
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

	struct HHEATable {
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

		void Parse(const char* data, uint32& offset) {
			read(&majorVersion, data, &offset);
			read(&minorVersion, data, &offset);
			read(&Ascender, data, &offset);
			read(&Descender, data, &offset);
			read(&LineGap, data, &offset);
			read(&advanceWidthMax, data, &offset);
			read(&minLeftSideBearing, data, &offset);
			read(&minRightSideBearing, data, &offset);
			read(&xMaxExtent, data, &offset);
			read(&caretSlopeRise, data, &offset);
			read(&caretSlopeRun, data, &offset);
			read(&caretOffset, data, &offset);
			offset += sizeof(int16) * 4;
			read(&metricDataFormat, data, &offset);
			read(&numberOfHMetrics, data, &offset);
		}
	};

	struct ColorTable {
		uint16 numBaseGlyphRecords;
		uint32 baseGlyphRecordsOffset;
		uint32 layerRecordsOffset;
		uint16 numLayerRecords;
	
		void Parse(const char* data, uint32* offset) {
			offset += 2; //version
			read(&numBaseGlyphRecords, data, offset);
			read(&baseGlyphRecordsOffset, data, offset);
			read(&layerRecordsOffset, data, offset);
			read(&numLayerRecords, data, offset);
		}
	};

	struct CPALTable {
		uint16 numPaletteEntries, numPalettes, numColorRecords;
		uint32 colorRecordsArrayOffset;
		std::vector<uint16> colorRecordIndices;

		struct BGRAColor {
			uint8 Blue, Green, Red, Alpha;
		};

		std::vector<BGRAColor> colors;

		void Parse(const char* data, uint32* offset) {
			uint16 version = 0;
			read(&version, data, offset);

			read(&numPaletteEntries, data, offset);
			read(&numPalettes, data, offset);
			read(&numColorRecords, data, offset);
			read(&colorRecordsArrayOffset, data, offset);

			colorRecordIndices.resize(numPalettes);

			//auto colorRecordIndex = colorRecordIndices[paletteIndex] + paletteEntryIdex;

			if(version == 1) {
				
			}
		}
	};

	struct FontMetaData {
		uint16 UnitsPerEm;
		int16 Ascender;
		int16 Descender;
		int16 LineGap;
	};

	template<uint8 N>
	struct Segment {
		//0 is on curve
		//1 is control point or nan
		//2 is on curve
		Vector2 Points[N];

		static constexpr auto NumberOfPoints = N;
	
		bool IsCurve = false;
	
		bool IsBezierCurve() const { return IsCurve; }
	};

	using Path = Vector<Segment<3>, DefaultAllocatorReference>;

	struct Point {
		TTFFlags Flags;
		ShortVector Position;
	};

	struct Contour {
		Contour(uint32 l, const DefaultAllocatorReference& allocator) : Points(l, allocator) {}

		Vector<Point, DefaultAllocatorReference> Points;
	};

	struct Glyph {
		uint32 Character = 0;
		int16 GlyphIndex = 0;
		Vector<Contour, DefaultAllocatorReference> Contours;
		uint16 AdvanceWidth = 0;
		int16 LeftSideBearing = 0;
		Vector2 BoundingBox[2]; //min, max
		Vector2 Center;
	
		Glyph(const DefaultAllocatorReference& allocator = DefaultAllocatorReference()) : Contours(allocator) {}
	};

	struct Font {
		Font(const DefaultAllocatorReference& allocator) : KerningTable(allocator) {}

		uint32 FileNameHash;
		std::string FullFontName;
		std::string NameTable[25];
		HashMap<uint32, int16, DefaultAllocatorReference> KerningTable;
		Vector<Glyph, DefaultAllocatorReference> Glyphs;
		HashMap<uint32, uint16, DefaultAllocatorReference> GlyphMap;
		FontMetaData Metadata;
		uint64 LastUsed;

		[[nodiscard]] const Glyph& GetGlyph(const char32_t ch) const {
			return Glyphs[GlyphMap[ch]];
		}

		[[nodiscard]] int16 GetKerning(const char32_t left, const char32_t right) const {
			const auto kern_data = KerningTable.TryGet((GlyphMap[left] << 16) | GlyphMap[right]);
			return kern_data ? kern_data.Get() : 0;
		}
	};

	struct FontLineInfoData {
		uint32 StringStartIndex;
		uint32 StringEndIndex;
		Vector2 OffsetStart;
		Vector2 OffsetEnd;
		Vector<Glyph*, DefaultAllocatorReference> GlyphIndex;
	};

	struct FontPositioningOutput {
		Vector<FontLineInfoData, DefaultAllocatorReference> LinePositions;
		uint32 NumTriangles;
		//PixelPositioning alignment;
		//BoundingRect bounding_rect;
		uint32 Geometry;
		uint16 FontSize;
	};

	struct FontPositioningOptions {
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

	inline Vector2 toVector(const ShortVector sh) { return Vector2(sh.X, sh.Y); }

	inline bool MakeFont(const Range<const byte*> buffer, Font* fontData) {
		const char* data = reinterpret_cast<const char*>(buffer.begin());

		HashMap<Id64, TableEntry, DefaultAllocatorReference> tables;

		uint32 ptr = 0;

		{
			TTFHeader header;
			header.Parse(data, ptr);

			for (uint16 i = 0; i < header.NumberOfTables; i++) {
				TableEntry te;
				te.Parse(data, ptr);
				tables.Emplace(te.tagstr, te);
			}
		}

		auto head_table_entry = tables.TryGet(u8"head");
		if (!head_table_entry) { return false; }

		ptr = head_table_entry.Get().offsetPos;

		HeadTable headTable;
		headTable.Parse(data, ptr);

		if (headTable.magicNumber != 0x5F0F3CF5) { return false; }

		auto maxp_table_entry = tables.TryGet(u8"maxp");
		if (!maxp_table_entry) { return false; }

		ptr = maxp_table_entry.Get().offsetPos;

		MaximumProfile max_profile;
		max_profile.Parse(data, ptr);
		auto name_table_entry = tables.TryGet(u8"name");
		if (!name_table_entry) { return false; }

		ptr = name_table_entry.Get().offsetPos;

		NameTable name_table;
		name_table.Parse(data, ptr, fontData->NameTable);

		fontData->FullFontName = fontData->NameTable[1] + " " + fontData->NameTable[2];

		auto loca_table_entry = tables.TryGet(u8"loca");
		if (!loca_table_entry) { return false; }

		std::vector<uint32> glyph_index(max_profile.numGlyphs);

		uint32 end_of_glyf = 0;

		if (headTable.indexToLocFormat == 0) {
			uint32 byte_offset = loca_table_entry.Get().offsetPos;

			for (uint16 i = 0; i < max_profile.numGlyphs; i++) {
				uint16 t = 0;
				read(&t, data, &byte_offset);
				glyph_index[i] = static_cast<uint32>(t) << 1u;
			}

			uint16 eog;
			read(&eog, data, &byte_offset);
			end_of_glyf = static_cast<uint32>(eog) << 1;
		} else {
			uint32 byte_offset = loca_table_entry.Get().offsetPos;
			for (uint16 i = 0; i < max_profile.numGlyphs; i++) {
				read(&glyph_index[i], data, &byte_offset);
			}
			read(&end_of_glyf, data, &byte_offset);
		}

		auto cmap_table_entry = tables.TryGet(u8"cmap");
		if (!cmap_table_entry) { return false; }

		uint32 cmap_offset = cmap_table_entry.Get().offsetPos + sizeof(uint16); //Skip version
		uint16 cmapNumberOfSubtables;
		read(&cmapNumberOfSubtables, data, &cmap_offset);

		//std::map<uint16, uint32> glyphReverseMap;
		HashMap<uint16, uint32, DefaultAllocatorReference> glyphReverseMap;

		bool valid_cmap_table = false;
		for (uint16 st = 0; st < cmapNumberOfSubtables; st++) {
			constexpr uint8 UNICODE1_PLATFORM_ID = 0, UNICODE2_PLATFORM_ID = 3; constexpr uint8 WIN32_PLATFORM_ID = 3; constexpr uint8 WIN32_UNICODE_ENCODING = 1;

			uint16 platformID, encodingID;
			uint32 cmap_subtable_offset;
			read(&platformID, data, &cmap_offset);
			read(&encodingID, data, &cmap_offset);
			read(&cmap_subtable_offset, data, &cmap_offset);

			if (!((platformID == UNICODE1_PLATFORM_ID && encodingID == UNICODE2_PLATFORM_ID) || (platformID == WIN32_PLATFORM_ID && encodingID == WIN32_UNICODE_ENCODING))) { continue; }

			cmap_subtable_offset += cmap_table_entry.Get().offsetPos;
			uint16 format, length;
			read(&format, data, &cmap_subtable_offset); //format number
			read(&length, data, &cmap_subtable_offset); //this is the length in bytes of the subtable

			if (format != 4) { continue; }

			uint16 language, segCountX2;// , searchRange, entrySelector, rangeShift;
			read(&language, data, &cmap_subtable_offset);
			read(&segCountX2, data, &cmap_subtable_offset);
			//get2b(&searchRange, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
			//get2b(&entrySelector, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
			//get2b(&rangeShift, data + cmap_subtable_offset); cmap_subtable_offset += sizeof(uint16);
			cmap_subtable_offset += sizeof(uint16) * 3;

			uint16 segCount = segCountX2 >> 1;
			std::vector<uint16> endCode(segCount), startCode(segCount), idRangeOffset(segCount);
			std::vector<int16> idDelta(segCount);

			for (uint16 j = 0; j < segCount; j++) { //End characterCode for each segment, last=0xFFFF.
				read(&endCode[j], data, &cmap_subtable_offset);
			}

			cmap_subtable_offset += sizeof(uint16); //reserved pad, should be zero

			for (uint16 j = 0; j < segCount; j++) { //Start character code for each segment.
				read(&startCode[j], data, &cmap_subtable_offset);
			}

			for (uint16 j = 0; j < segCount; j++) { //Delta for all character codes in segment.
				read(&idDelta[j], data, &cmap_subtable_offset);
			}

			for (uint16 j = 0; j < segCount; j++) { //Offsets into glyphIdArray or 0
				read(&idRangeOffset[j], data, &cmap_subtable_offset);
			}

			for (uint16 i = 0; i < segCount; i++) { //Glyph index array (arbitrary length)
				if (idRangeOffset[i]) { //If the idRangeOffset value for the segment is not 0, the mapping of the character codes relies on the glyphIndexArray.
					//uint32 glyph_address_offset = cmap_subtable_offset + sizeof(uint16) * segCount * 2; //idRangeOffset_ptr
					uint32 glyph_address_offset = cmap_subtable_offset - (segCount - i) * 2; //idRangeOffset_ptr
					for (uint32 c = startCode[i]; c <= endCode[i]; c++) {
						//uint32 glyph_address_index_offset = idRangeOffset[j] + 2 * (k - startCode[j]) + glyph_address_offset;
						uint32 glyph_address_index_offset = idRangeOffset[i] + 2 * (c - startCode[i]) + glyph_address_offset;
						uint16 glyphId;
						read(&glyphId, data, &glyph_address_index_offset);
						glyphId += idDelta[i];
						fontData->GlyphMap.Emplace(c, glyphId);
						if (auto res = glyphReverseMap.TryEmplace(glyphId, c); !res) {
							res.Get() = c;
						}
						//glyphReverseMap[glyph_map_value] = k;
					}
				} else {
					for (uint32 c = startCode[i]; c <= endCode[i]; c++) {
						fontData->GlyphMap.Emplace(c, c + idDelta[i]);
						if (auto res = glyphReverseMap.TryEmplace(c + idDelta[i], c); !res) {
							res.Get() = c;
						}
						//glyphReverseMap[k + idDelta[j]] = k;
					}
				}
				//cmap_subtable_offset += sizeof(uint16);
			}

			valid_cmap_table = true;
			break;
		}
		if (!valid_cmap_table) { return false; }

		HHEATable hheaTable;
		auto hhea_table_entry = tables.TryGet(u8"hhea");
		if (!hhea_table_entry) { return false; }
		ptr = hhea_table_entry.Get().offsetPos;
		hheaTable.Parse(data, ptr);

		fontData->Metadata.UnitsPerEm = headTable.unitsPerEm;
		fontData->Metadata.Ascender = hheaTable.Ascender;
		fontData->Metadata.Descender = hheaTable.Descender;
		fontData->Metadata.LineGap = hheaTable.LineGap;

		auto glyf_table_entry = tables.TryGet(u8"glyf");
		if (!glyf_table_entry) { return false; }
		
		uint32 glyf_offset = glyf_table_entry.Get().offsetPos;

		auto kern_table_entry = tables.TryGet(u8"kern");
		uint32 kernOffset = 0;
		if (kern_table_entry) {
			kernOffset = kern_table_entry.Get().offsetPos;
		}

		auto hmtx_table_entry = tables.TryGet(u8"hmtx");
		if (!hmtx_table_entry) {
			return false;
		}
		uint32 hmtx_offset = hmtx_table_entry.Get().offsetPos;
		uint16 last_glyph_advance_width = 0;

		//First dimension corresponds to each contour of the current glyph
		//Each uint16 represents and index into pointsInContour
		std::vector<std::vector<uint16>> pointsInContours((max_profile.maxContours < 4096) ? max_profile.maxContours : 4096);
		std::vector<uint16> pointsInContour((max_profile.maxContours < 4096) ? max_profile.maxContours : 4096);

		if (!max_profile.numGlyphs) { return false; }

		Vector<bool, DefaultAllocatorReference> glyphLoaded(max_profile.numGlyphs, DefaultAllocatorReference());
		glyphLoaded.EmplaceGroup(max_profile.numGlyphs, false);
		fontData->Glyphs.EmplaceGroup(max_profile.numGlyphs);

		auto parseGlyph = [&](uint32 i, auto&& self) -> bool {
			if (glyphLoaded[i] == true) { return true; }

			//if(i != fontData->Glyphs.GetLength()) {
			//	__debugbreak();
			//}

			Glyph& currentGlyph = fontData->Glyphs[i];
			//currentGlyph.Paths.Initialize(3, GetPersistentAllocator());
			currentGlyph.GlyphIndex = static_cast<int16>(i);
			currentGlyph.Character = glyphReverseMap.TryEmplace(static_cast<int16>(i), 0).Get();

			if (i < hheaTable.numberOfHMetrics) {
				ptr = hmtx_offset + i * sizeof(uint32);

				read(&currentGlyph.AdvanceWidth, data, &ptr);
				last_glyph_advance_width = currentGlyph.AdvanceWidth;
				read(&currentGlyph.LeftSideBearing, data, &ptr);
			} else {
				currentGlyph.AdvanceWidth = last_glyph_advance_width;
			}

			if (i != max_profile.numGlyphs - 1 && glyph_index[i] == glyph_index[i + 1]) {
				glyphLoaded[i] = true;
				return true;
			}

			if (glyph_index[i] >= end_of_glyf) { return false; }

			uint32 currentOffset = glyf_offset + glyph_index[i];

			int16 bbox[2][2];

			int16 numContours = 0;

			read(&numContours, data, &currentOffset);
			read(&bbox[0][0], data, &currentOffset); //xMin
			read(&bbox[0][1], data, &currentOffset); //yMin
			read(&bbox[1][0], data, &currentOffset); //xMax
			read(&bbox[1][1], data, &currentOffset); //yMax

			currentGlyph.BoundingBox[0][0] = bbox[0][0];
			currentGlyph.BoundingBox[0][1] = bbox[0][1];
			currentGlyph.BoundingBox[1][0] = bbox[1][0];
			currentGlyph.BoundingBox[1][1] = bbox[1][1];

			currentGlyph.Center = (currentGlyph.BoundingBox[0] + currentGlyph.BoundingBox[1]) / 2.0f;

			if (numContours > 0) { //Simple glyph
				std::vector<uint16> contourEnd(numContours);

				//currentGlyph.PathList.Resize(currentGlyph.NumContours);
				//don't resize
				//code expects resize to leave valid elements which our vector doesn't
				//emplace elements as needed later to ensure valid elements

				for (uint16 j = 0; j < numContours; j++) { read(&contourEnd[j], data, &currentOffset); }

				for (uint16 contourIndex = 0; contourIndex < numContours; contourIndex++) {
					uint16 num_points = contourEnd[contourIndex] - (contourIndex ? contourEnd[contourIndex - 1] : -1);

					if (pointsInContours[contourIndex].size() < num_points) {
						pointsInContours[contourIndex].resize(num_points);
					}

					pointsInContour[contourIndex] = num_points;
				}

				//Skip instructions
				uint16 num_instructions;
				read(&num_instructions, data, &currentOffset);
				currentOffset += sizeof(uint8_t) * num_instructions;

				uint16 numPoints = contourEnd[static_cast<int64>(numContours) - 1] + 1;
				//flags for every point in this glyph
				std::vector<TTFFlags> flagsEnum(numPoints);
				//??
				std::vector<uint16> contour_index(numPoints);
				uint16 current_contour_index = 0;
				uint16 contour_count_first_point = 0;
				signed char repeat = 0; //may cause bugs?

				{
					//flags for every point in this glyph
					std::vector<uint8_t> flags(numPoints);

					for (uint16 j = 0; j < numPoints; j++, ++contour_count_first_point) {

						if (repeat) {
							flags[j] = flags[j - 1];
							repeat--;
						} else {
							read(&flags[j], data, &currentOffset);

							if (flags[j] & 0x8) { read(&repeat, data, &currentOffset); } //may cause bugs
						}

						flagsEnum[j].isControlPoint = (!(flags[j] & 0b00000001)) != 0;
						flagsEnum[j].xShort = (flags[j] & 0b00000010) != 0; flagsEnum[j].yShort = (flags[j] & 0b00000100) != 0;
						flagsEnum[j].repeat = (flags[j] & 0b00001000) != 0;
						flagsEnum[j].xDual = (flags[j] & 0b00010000) != 0; flagsEnum[j].yDual = (flags[j] & 0b00100000) != 0;

						if (j > contourEnd[current_contour_index]) {
							current_contour_index++;
							contour_count_first_point = 0;
						}

						contour_index[j] = current_contour_index;
						pointsInContours[current_contour_index][contour_count_first_point] = j;
					}
				}

				//all points for current glyph, no distinction between contours
				std::vector<ShortVector> glyphPoints(numPoints);

				for (uint16 j = 0; j < numPoints; ++j) {
					if (flagsEnum[j].xDual && !flagsEnum[j].xShort) {
						glyphPoints[j].X = j ? glyphPoints[j - 1].X : 0;
					} else {
						if (flagsEnum[j].xShort) {
							read(reinterpret_cast<uint8*>(&glyphPoints[j].X), data, &currentOffset); //might cause bugs, can directly write value into 16 bit variable
						} else {
							read(&glyphPoints[j].X, data, &currentOffset);
						}

						if (flagsEnum[j].xShort && !flagsEnum[j].xDual) { glyphPoints[j].X *= -1; }

						if (j != 0) { glyphPoints[j].X += glyphPoints[j - 1].X; }
					}
				}

				for (uint16 j = 0; j < numPoints; j++) {
					if (flagsEnum[j].yDual && !flagsEnum[j].yShort) {
						glyphPoints[j].Y = j ? glyphPoints[j - 1].Y : 0;
					} else {
						if (flagsEnum[j].yShort) {
							read(reinterpret_cast<uint8*>(&glyphPoints[j].Y), data, &currentOffset); //might cause bugs
						} else {
							read(&glyphPoints[j].Y, data, &currentOffset);
						}

						if (flagsEnum[j].yShort && !flagsEnum[j].yDual) { glyphPoints[j].Y *= -1; }

						if (j != 0) { glyphPoints[j].Y += glyphPoints[j - 1].Y; }
					}
				}

				//Generate contours
				for (uint16 contourIndex = 0; contourIndex < numContours; ++contourIndex) {
					const uint16 numPointsInContour = pointsInContour[contourIndex];

					currentGlyph.Contours.EmplaceBack(numPointsInContour, DefaultAllocatorReference());

					const auto& glyphFlags = flagsEnum;

					uint16 pointInIndices = 0;

					for (uint32 p = 0; p < numPointsInContour; ++p, ++pointInIndices) {
						uint32 pointInGlyphIndex = pointsInContours[contourIndex][pointInIndices];

						auto& point = currentGlyph.Contours[contourIndex].Points.EmplaceBack();
						point.Flags = glyphFlags[pointInGlyphIndex];
						point.Position = glyphPoints[pointInGlyphIndex];
					}
				} //for contour
			} else { //Composite glyph
				for (auto compound_glyph_index = 0; compound_glyph_index < -numContours; compound_glyph_index++) {
					uint16 glyfFlags, glyphIndex;

					do
					{
						read(&glyfFlags, data, &currentOffset); //the index of the first contour in this composite glyph
						read(&glyphIndex, data, &currentOffset);

						int16 glyfArgs1 = 0, glyfArgs2 = 0;
						int8_t glyfArgs1U8 = 0, glyfArgs2U8 = 0;
						bool is_word = false;
						if (glyfFlags & ARG_1_AND_2_ARE_WORDS) {
							read(&glyfArgs1, data, &currentOffset);
							read(&glyfArgs2, data, &currentOffset);
							is_word = true;
						} else {
							read(&glyfArgs1U8, data, &currentOffset);
							read(&glyfArgs2U8, data, &currentOffset);
						}

						float32 compositeGlyphElementTransformation[6] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

						if (glyfFlags & WE_HAVE_A_SCALE) {
							int16 xy_value = 0;
							read(&xy_value, data, &currentOffset);
							compositeGlyphElementTransformation[0] = to_2_14_float(xy_value);
							compositeGlyphElementTransformation[3] = to_2_14_float(xy_value);
						} else if (glyfFlags & WE_HAVE_AN_X_AND_Y_SCALE) {
							int16 xy_values[2];
							read(&xy_values[0], data, &currentOffset);
							read(&xy_values[1], data, &currentOffset);
							compositeGlyphElementTransformation[0] = to_2_14_float(xy_values[0]);
							compositeGlyphElementTransformation[3] = to_2_14_float(xy_values[1]);
						} else if (glyfFlags & WE_HAVE_A_TWO_BY_TWO) {
							int16 xy_values[4];
							read(&xy_values[0], data, &currentOffset);
							read(&xy_values[1], data, &currentOffset);
							read(&xy_values[2], data, &currentOffset);
							read(&xy_values[3], data, &currentOffset);
							compositeGlyphElementTransformation[0] = to_2_14_float(xy_values[0]);
							compositeGlyphElementTransformation[1] = to_2_14_float(xy_values[1]);
							compositeGlyphElementTransformation[2] = to_2_14_float(xy_values[2]);
							compositeGlyphElementTransformation[3] = to_2_14_float(xy_values[3]);
						}

						bool matched_points = false;
						if (glyfFlags & ARGS_ARE_XY_VALUES) {
							compositeGlyphElementTransformation[4] = is_word ? glyfArgs1 : glyfArgs1U8;
							compositeGlyphElementTransformation[5] = is_word ? glyfArgs2 : glyfArgs2U8;
							if (glyfFlags & SCALED_COMPONENT_OFFSET) {
								compositeGlyphElementTransformation[4] *= compositeGlyphElementTransformation[0];
								compositeGlyphElementTransformation[5] *= compositeGlyphElementTransformation[3];
							}
						} else {
							matched_points = true;
						}

						//Skip instructions
						if (glyfFlags & WE_HAVE_INSTRUCTIONS) {
							uint16 num_instructions = 0;
							read(&num_instructions, data, &currentOffset);
							currentOffset += sizeof(uint8_t) * num_instructions;
						}

						if (!glyphLoaded[glyphIndex]) {
							if (!self(glyphIndex, self)) {
								return false;
							}
						}

						const Glyph& compositeGlyphElement = fontData->Glyphs[glyphIndex];

						uint32 compositeGlyphPathCount = compositeGlyphElement.Contours.GetLength();
						for (uint32 glyphPointIndex = 0; glyphPointIndex < compositeGlyphPathCount; glyphPointIndex++) {
							const auto& currentPointsList = compositeGlyphElement.Contours[glyphPointIndex].Points;

							auto& newContour = currentGlyph.Contours.EmplaceBack(currentPointsList.GetLength(), DefaultAllocatorReference());

							if (!matched_points) {
								for (uint32 glyphCurvesPointIndex = 0; glyphCurvesPointIndex < currentPointsList.GetLength(); glyphCurvesPointIndex++) {
									const auto& point = currentPointsList[glyphCurvesPointIndex];

									Point& newPoint = newContour.Points.EmplaceBack();
									newPoint.Position.X = point.Position.X * compositeGlyphElementTransformation[0] + point.Position.X * compositeGlyphElementTransformation[1] + compositeGlyphElementTransformation[4];
									newPoint.Position.Y = point.Position.Y * compositeGlyphElementTransformation[2] + point.Position.Y * compositeGlyphElementTransformation[3] + compositeGlyphElementTransformation[5];
								}
							} else {
								return false;
							}

						}
					} while (glyfFlags & MORE_COMPONENTS);
				}
			}

			glyphLoaded[i] = true;

			return true;
		};

		for (uint16 i = 0; i < max_profile.numGlyphs; ++i) {
			if (!parseGlyph(i, parseGlyph)) { return false; }
		}

		//Kerning table
		if (kernOffset) {
			uint32 currentOffset = kernOffset;
			uint16 kern_table_version, num_kern_subtables;
			read(&kern_table_version, data, &currentOffset);
			read(&num_kern_subtables, data, &currentOffset);
			uint16 kern_length = 0;
			uint32 kernStartOffset = currentOffset;

			for (uint16 kerningSubTableIndex = 0; kerningSubTableIndex < num_kern_subtables; kerningSubTableIndex++) {
				uint16 kerningVersion, kerningCoverage;
				currentOffset = kernStartOffset + kern_length;
				kernStartOffset = currentOffset;
				read(&kerningVersion, data, &currentOffset);
				read(&kern_length, data, &currentOffset);
				if (kerningVersion != 0) {
					currentOffset += kern_length - sizeof(uint16) * 3;
					continue;
				}
				read(&kerningCoverage, data, &currentOffset);

				uint16 num_kern_pairs;
				read(&num_kern_pairs, data, &currentOffset);
				currentOffset += sizeof(uint16) * 3;
				for (uint16 kern_index = 0; kern_index < num_kern_pairs; kern_index++) {
					uint16 kern_left, kern_right;
					int16 kern_value;
					read(&kern_left, data, &currentOffset);
					read(&kern_right, data, &currentOffset);
					read(&kern_value, data, &currentOffset);

					fontData->KerningTable.Emplace((kern_left << 16) | kern_right, kern_value);
				}
			}
		}

		return true;
	}

	template<uint8 N, class ALLOCATOR>
	void MakePath(const Glyph& glyph, Vector<Vector<Segment<N>, ALLOCATOR>, ALLOCATOR>* contours) {
		//Generate contours
		for (uint32 contourIndex = 0; contourIndex < glyph.Contours.GetLength(); ++contourIndex) {
			const auto& sourceContour = glyph.Contours[contourIndex];
			uint32 pointInIndices = 0;
			//If the first point is control point
			while (sourceContour.Points[pointInIndices].Flags.isControlPoint) { ++pointInIndices; }

			bool lastPointWasControlPoint = false, thisPointIsControlPoint = false;

			contours->EmplaceBack(sourceContour.Points.GetLength() / 2 + 6); //min elements

			Segment<N> currentCurve;
			currentCurve.Points[0] = toVector(sourceContour.Points[pointInIndices].Position); //what if no more points

			++pointInIndices;

			const uint8 topBoundElements = N;

			uint8 index = 1;

			for (uint32 p = 0; p < sourceContour.Points.GetLength(); ++p, ++pointInIndices) {
				const uint32 safeIndexToData = pointInIndices % sourceContour.Points.GetLength();

				index = index % topBoundElements;

				thisPointIsControlPoint = sourceContour.Points[safeIndexToData].Flags.isControlPoint;

				if (thisPointIsControlPoint) {
					if (lastPointWasControlPoint) {
						auto thisPoint = toVector(sourceContour.Points[safeIndexToData].Position);
						const auto newPoint = (thisPoint + currentCurve.Points[1]) * 0.5f;
						currentCurve.Points[2] = newPoint;
						currentCurve.IsCurve = true;
						contours->At(contourIndex).EmplaceBack(currentCurve);

						currentCurve.Points[0] = newPoint;
						currentCurve.Points[1] = thisPoint;
					} else {
						currentCurve.Points[1] = toVector(sourceContour.Points[safeIndexToData].Position);
					}
				} else {
					if (lastPointWasControlPoint) {
						const auto thisPoint = toVector(sourceContour.Points[safeIndexToData].Position);
						currentCurve.Points[2] = thisPoint;
						currentCurve.IsCurve = true;
						contours->At(contourIndex).EmplaceBack(currentCurve);

						currentCurve.Points[0] = thisPoint;
					} else {
						const auto thisPoint = toVector(sourceContour.Points[safeIndexToData].Position);
						currentCurve.Points[1] = Vector2(0, 0);
						currentCurve.Points[2] = thisPoint;
						currentCurve.IsCurve = false;
						contours->At(contourIndex).EmplaceBack(currentCurve);

						currentCurve.Points[0] = thisPoint;
					}
				}

				lastPointWasControlPoint = thisPointIsControlPoint;
				++index;
			}
		} //for contour
	}

	//O       *
	// \     / \
	//  \   /   \
	//   \ /     \
	//    *       O
}