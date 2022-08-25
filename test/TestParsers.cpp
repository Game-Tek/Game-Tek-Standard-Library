
#include <gtest/gtest.h>

#include "GTSL/File.hpp"
#include "GTSL/LUT.hpp"
#include "GTSL/JSON.hpp"
#include "GTSL/Vector.hpp"

TEST(LUT, Valid) {
#if _WIN64
	GTSL::File lutFile(u8"../../../test/Kodak Ektachrome 64.cube", GTSL::File::READ, false);
#elif __linux__
	GTSL::File lutFile(u8"../test/Kodak Ektachrome 64.cube", GTSL::File::READ, false);
#endif

	GTSL::Buffer<GTSL::DefaultAllocatorReference> buffer;
	lutFile.Read(buffer);

	GTSL::LUTData lutData;

	GTSL::Vector<GTSL::Vector3, GTSL::DefaultAllocatorReference> vectors(33 * 33 * 33);

	auto parseResult = ParseLUT(GTSL::StringView(buffer.GetSize(), buffer.GetSize(), (const char8_t*)buffer.GetData()), lutData, [&](const GTSL::Vector3 vec) { vectors.EmplaceBack(vec); });

	ASSERT_TRUE(parseResult);
	GTEST_ASSERT_EQ(lutData.Size, 33);
	GTEST_ASSERT_EQ(lutData.Min, GTSL::Vector3(0, 0, 0));
	GTEST_ASSERT_EQ(lutData.Max, GTSL::Vector3(1, 1, 1));
	ASSERT_TRUE(GTSL::Math::IsNearlyEqual(vectors[0], GTSL::Vector3(0.0170, 0.0002, 0.0111), 0.0001));
	ASSERT_TRUE(GTSL::Math::IsNearlyEqual(vectors[1], GTSL::Vector3(0.0339, 0.0009, 0.0104), 0.0001));
	ASSERT_TRUE(GTSL::Math::IsNearlyEqual(vectors[2], GTSL::Vector3(0.0519, 0.0015, 0.0097), 0.0001));
}

TEST(LUT, Invalid) {
	auto stringLut = u8" #Created by: Denver Riddle\n #Copyright: (C)Copyright Color Grading Central LLC\n DOMAIN_MIN 0.0 0.0 0.0\nDOMAIN_MAX 0 0 0\nLUT_3D_SIZE 33\n0.0170 0.0002 0.0111\n0.0339 0.0009 0.0104\n0.0519 0.0015 0.0097";

	GTSL::LUTData lut;

	GTSL::StaticVector<GTSL::Vector3, 8> vectors;

	auto parseResult = ParseLUT(stringLut, lut, [&](const GTSL::Vector3 vec) { vectors.EmplaceBack(vec); });

	ASSERT_FALSE(parseResult);
}

TEST(JSON, Serialize) {
	GTSL::uint32 bananas = 5, apples = 2;

	auto jsonString = u8R"({"bananas":5,"apples":2,"string":"test","array":[4,0,8,12],"obj":{"bool":true,"float":3.141000},"empty":[]})";

	GTSL::StaticString<256> buffer;
	GTSL::JSONSerializer serializer = GTSL::MakeSerializer(buffer);

	GTSL::Insert(serializer, buffer, u8"bananas", bananas);
	GTSL::Insert(serializer, buffer, u8"apples", apples);
	GTSL::Insert(serializer, buffer, u8"string", GTSL::StringView(u8"test"));

	GTSL::StartArray(serializer, buffer, u8"array");
		GTSL::Insert(serializer, buffer, 4u);
		GTSL::Insert(serializer, buffer, 0u);
		GTSL::Insert(serializer, buffer, 8u);
		GTSL::Insert(serializer, buffer, 12u);
	GTSL::EndArray(serializer, buffer);

	GTSL::StartObject(serializer, buffer, u8"obj");
		GTSL::Insert(serializer, buffer, u8"bool", true);
		GTSL::Insert(serializer, buffer, u8"float", 3.141f);
	GTSL::EndObject(serializer, buffer);

	GTSL::StartArray(serializer, buffer, u8"empty");
	GTSL::EndArray(serializer, buffer);

	EndSerializer(buffer, serializer);

	GTEST_ASSERT_EQ(buffer, GTSL::StringView(jsonString));
}

TEST(JSON, Deserialize) {
	auto jsonString = u8R"({
    "name":"ComputeShader",
    "type":"Compute", "outputSemantics":"Compute",
    "renderPass":"ColorCorrectionRenderPass",
    "input":["GlobalData"],
    "shaderVariables":[{ "type":"ImageReference", "name":"color", "defaultValue":"color" }],
    "localSize":[1,1,1],
	"shaders":["a", "b"],
    "statements":[
        { "name":"Write", "params":[ { "name": "GetScreenPosition() "}, { "type":"float4", "params" : [{ "type":"float32", "params" : [1] }] } ] }
	],
	"debug":3,
	"index":-1,
	"position":-3.0
})";

	//todo if key val pattern is not followed error

	GTSL::StaticString<64> name, type, outputSemantics, renderPass;

	GTSL::StaticVector<GTSL::StaticString<64>, 8> inputs;
	GTSL::StaticVector<GTSL::Tuple<GTSL::StaticString<64>, GTSL::StaticString<64>, GTSL::StaticString<64>>, 8> shaderVariables;
	
	auto json = GTSL::JSON(jsonString, GTSL::DefaultAllocatorReference{});

	ASSERT_TRUE(json);

	name = json[u8"name"];
	type = json[u8"type"];
	outputSemantics = json[u8"outputSemantics"].GetStringView();
	json[u8"renderPass"](renderPass);

	for (GTSL::uint32 i = 0; i < json[u8"input"]; ++i) {
		inputs.EmplaceBack();
		json[u8"input"][i](inputs.back());
	}

	for (auto sv : json[u8"shaderVariables"]) {
		shaderVariables.EmplaceBack();
		sv[u8"type"](shaderVariables.back().element);
		sv[u8"name"](shaderVariables.back().rest.element);
		sv[u8"defaultValue"](shaderVariables.back().rest.rest.element);
	}

	GTSL::uint64 x{ json[u8"localSize"][0] }, y{ json[u8"localSize"][1] }, z{ json[u8"localSize"][2] };

	GTEST_ASSERT_EQ(json[u8"shaders"].GetCount(), 2);
	GTEST_ASSERT_EQ(json[u8"shaders"][0].GetStringView(), u8"a");
	GTEST_ASSERT_EQ(json[u8"shaders"][1].GetStringView(), u8"b");

	GTEST_ASSERT_EQ(json[u8"statements"].GetCount(), 1);

	GTEST_ASSERT_EQ(name, u8"ComputeShader");
	GTEST_ASSERT_EQ(type, u8"Compute");
	GTEST_ASSERT_EQ(outputSemantics, u8"Compute");
	GTEST_ASSERT_EQ(renderPass, u8"ColorCorrectionRenderPass");
	ASSERT_TRUE(json[u8"localSize"]);

	for(auto e : json[u8"nonExistingArray"]) {
		ASSERT_FALSE(true);
	}

	GTEST_ASSERT_EQ(x, 1); GTEST_ASSERT_EQ(y, 1); GTEST_ASSERT_EQ(z, 1);

	GTEST_ASSERT_EQ(GTSL::StringView(json[u8"statements"][0][u8"name"]), u8"Write");

	GTEST_ASSERT_EQ(GTSL::StringView(json[u8"statements"][0][u8"params"][0][u8"name"]), u8"GetScreenPosition() ");

	ASSERT_FALSE(json[u8"statements"][0][u8"type"]);
	
	GTEST_ASSERT_EQ(GTSL::StringView(json[u8"statements"][0][u8"params"][1][u8"type"]), u8"float4");
	GTEST_ASSERT_EQ(GTSL::StringView(json[u8"statements"][0][u8"params"][1][u8"params"][0][u8"type"]), u8"float32");
	GTEST_ASSERT_EQ(GTSL::uint64(json[u8"statements"][0][u8"params"][1][u8"params"][0][u8"params"][0]), 1);	

	GTEST_ASSERT_EQ(GTSL::uint64(json[u8"debug"]), 3ull);
	GTEST_ASSERT_EQ(GTSL::int64(json[u8"index"]), -1ll);
	GTEST_ASSERT_EQ(GTSL::float32(json[u8"position"]), -3.0f);
}

TEST(JSON, Deserialize2) {
	auto jsonString = u8R"({
	"threadCount":1,
	"language":"en_US",

	"rayTracing":false,
	"hdr":false,
	"buffer":2,
	"resolution":[1280, 720],
	"fullScreen":false,
	"debugSync":true,
	"trace":false,

	"bitDepth":16,
	"channels":2,
	"kHz":48000,

	"debug":3
})";

	//todo if key val pattern is not followed error

	GTSL::StaticString<64> name, type, outputSemantics, renderPass;

	GTSL::StaticVector<GTSL::StaticString<64>, 8> inputs;
	GTSL::StaticVector<GTSL::Tuple<GTSL::StaticString<64>, GTSL::StaticString<64>, GTSL::StaticString<64>>, 8> shaderVariables;
	
	auto json = GTSL::JSON(jsonString, GTSL::DefaultAllocatorReference{});

	GTEST_ASSERT_EQ(GTSL::uint64(json[u8"debug"]), 3ull);
}