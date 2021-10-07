#pragma once

#include "Core.h"
#include "HashMap.hpp"
#include "StringCommon.h"
#include "Range.hpp"
#include "System.h"
#include "Vector.hpp"

namespace GTSL {
	//bool JSON(const StringView string) {
	//	
	//}

	class JSON {
		
	};

	template<class T>
	class Serializer;

	template<>
	class Serializer<JSON> {
		Serializer() = default;

		friend Serializer<JSON> MakeSerializer(auto&);
	public:
		using type = JSON;

		bool newLine = true;
		uint32 objectsInScope = 0;

		~Serializer() = default;
	};

	using JSONSerializer = Serializer<JSON>;

	JSONSerializer MakeSerializer(auto& buffer) {
		ToString(buffer, u8"{");
		return JSONSerializer();
	}

	void EndSerializer(auto& buffer, JSONSerializer& serializer) {
		if(serializer.objectsInScope > 1)
			DropLast(buffer, u8',');

		ToString(buffer, u8"}");
	}

	void StartJSONPair(JSONSerializer& serializer, auto& buffer, StringView name) {
		ToString(buffer, u8'"'); ToString(buffer, name); ToString(buffer, u8'"'); ToString(buffer, u8':');
	}

	void StartJSONValue(JSONSerializer& serializer, auto& buffer) {
	}

	void EndJSONObject(JSONSerializer& serializer, auto& buffer) {
		ToString(buffer, u8',');
		serializer.objectsInScope += 1;
	}

	template<class B>
	void StartObject(Serializer<JSON>& serializer, B& buffer, StringView name) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'{');
	}

	template<class B>
	void StartObject(Serializer<JSON>& serializer, B& buffer) {
		StartJSONValue(serializer, buffer); ToString(buffer, u8'{');
	}

	template<class B>
	void EndObject(Serializer<JSON>& serializer, B& buffer) {
		DropLast(buffer, u8',');
		ToString(buffer, u8"}");
		EndJSONObject(serializer, buffer);
	}

	template<class B>
	void StartArray(Serializer<JSON>& serializer, B& buffer, StringView name) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'[');
	}

	template<class B>
	void EndArray(Serializer<JSON>& serializer, B& buffer) {
		DropLast(buffer, u8',');
		ToString(buffer, u8"]");
		EndJSONObject(serializer, buffer);
	}

	template<std::integral T, class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, T& val) {
		StartJSONValue(serializer, buffer); ToString(buffer, val); EndJSONObject(serializer, buffer);
	}

	template<std::integral T, class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, T& val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val); EndJSONObject(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, float32 val) {
		StartJSONValue(serializer, buffer); ToString(buffer, val); EndJSONObject(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, float32 val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val); EndJSONObject(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, bool val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val ? u8"true" : u8"false"); EndJSONObject(serializer, buffer);
	}

	template<Enum E, class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, E, StringView enumStringType) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'"'); ToString(buffer, enumStringType); ToString(buffer, u8'"'); EndJSONObject(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView string) {
		StartJSONValue(serializer, buffer); ToString(buffer, u8'"'); ToString(buffer, string); ToString(buffer, u8'"'); EndJSONObject(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, StringView string) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'"'); ToString(buffer, string); ToString(buffer, u8'"'); EndJSONObject(serializer, buffer);
	}

	template<class B, class I>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, Range<I> range) {
		StartArray(serializer, buffer, name);

		for(auto& e : range) {
			Insert(serializer, buffer, e);
		}

		EndArray(serializer, buffer);
	}

#undef NULL
#undef BOOL

	struct JSONObject {
		uint32 Position;
		enum class Type : uint8 {
			NULL, BOOL, INT, UINT, FLOAT, STRING, ARRAY, OBJECT
		} ObjectType;
	};

	bool ParseJSON(StringView string, auto&& whenBool, auto&& whenInt, auto&& whenUint, auto&& whenFloat, auto&& whenString, auto&& whenKey, auto&& whenArray, auto&& whenObject) {
		uint32 side = 0;
		uint32 count = 0;

		uint32 pair = 0;

		StringIterator tokenStart = string.begin(), tokenEnd = string.begin(), ptr = string.begin();

		while (tokenEnd < string.end()) {
			auto advance = [&] { ++tokenEnd; ++ptr; };
			auto sync = [&] { ++ptr; tokenStart = ptr; tokenEnd = ptr; };
			auto skip = [&] {
				if(tokenStart == tokenEnd) {
					sync();
				} else {
					++ptr;
				}
			};

			auto evalObject = [&] {
				auto str = StringView(tokenStart, tokenEnd);

				//side += 1;
				//
				//if (side != 1) { return false; }

				if (IsNumber(str)) {
					if(IsDecimalNumber(str)) {
						whenFloat(ToNumber<float32>(str).Get());
					} else if (*tokenStart == U'-') {
						whenInt(ToNumber<int64>(str).Get());
					} else {
						auto res = ToNumber<uint64>(str);
						if (!res) {
							__debugbreak();
						}

						whenUint(res.Get());
					}
				} else if(str == u8"true" or str == u8"false") {
						whenBool(str == u8"true");
				}

				return true;
			};

			switch (*ptr) {
			case u8'{': {
				whenObject(true);
				sync();
				side = 0;
				break;
			}
			case u8'}': {
				evalObject();
				whenObject(false);				
				count = 0;
				sync();
				break;
			}
			case u8'[': {
				whenArray(true);
				sync();
				break;
			}
			case u8']': {
				evalObject();
				sync();
				whenArray(false);				
				break;
			}
			case u8'"': {
				++pair;

				sync();

				while (ptr < string.end() and *ptr != U'"') {
					advance();
				}

				if (side) {
					whenString(StringView(tokenStart, tokenEnd));
				} else {
					whenKey(StringView(tokenStart, tokenEnd));
				}

				sync();

				break;
			}
			case u8':': {
				//evalObject();
				++side;
				sync();
				break;
			}
			case u8',': {
				evalObject();				
				++count;
				side = 0;
				sync();
				break;
			}
			case u8' ':
				skip();
				break;
			case u8'\t':
				skip();
				break;
			case u8'\n':
				skip();
				break;
			default: {
				advance();
			}
			}
		}

		return true;
	}

	template<class ALLOCATOR = DefaultAllocatorReference>
	struct JSONDeserializer {
		using Map = HashMap<Id64, JSONObject, ALLOCATOR>;

		template<typename T>
		bool operator()(StringView name, T& obj) {
			Map& start = Get<Map>(offset);

			auto search = start.TryGet(name);

			if(search) {
				obj = Get<T>(search.Get().Position);
				return true;
			}

			return false;
		}

		template<typename T>
		bool operator()(StringView name, uint32 i, T& obj) {
			Map& start = Get<Map>(offset);

			auto search = start.TryGet(name);

			if (search && search.Get().ObjectType == JSONObject::Type::ARRAY) {
				obj = Get<T>(search.Get().Position + i * sizeof(T) + 4/*array length*/);
				return true;
			}

			return false;
		}

		template<Enum T>
		bool operator()(StringView name, T& obj, auto&& lookupFunction) {
			Map& start = Get<Map>(offset);

			auto search = start.TryGet(name);

			if (search && search.Get().ObjectType == JSONObject::Type::STRING) {
				auto res = lookupFunction(StringView(Get<uint32>(search.Get().Position), Get<uint32>(search.Get().Position + 4), reinterpret_cast<const char8_t*>(buffer.GetData() + search.Get().Position + 8)), obj);
				return res;
			}

			return false;
		}

		template<class ALLOC>
		bool operator()(StringView name, String<ALLOC>& obj) {
			Map& start = Get<Map>(offset);

			auto search = start.TryGet(name);

			if (search && search.Get().ObjectType == JSONObject::Type::STRING) {
				obj = StringView(*reinterpret_cast<uint32*>(buffer.GetData() + search.Get().Position), *reinterpret_cast<uint32*>(buffer.GetData() + search.Get().Position + 4), reinterpret_cast<const char8_t*>(buffer.GetData() + search.Get().Position + 8));
				return true;
			}

			return false;
		}

		template<typename T, class ALLOC>
		bool operator()(StringView name, Vector<T, ALLOC>& obj) {
			Map& start = Get<Map>(offset);

			auto search = start.TryGet(name);

			if (search && search.Get().ObjectType == JSONObject::Type::ARRAY) {
				obj = Range<const T*>(*reinterpret_cast<uint32*>(buffer.GetData() + search.Get().Position), reinterpret_cast<const T*>(buffer.GetData() + search.Get().Position + 4));
				return true;
			}

			return false;
		}

		bool operator()(StringView name) {
			offset = Get<Map>(offset).At(name).Position;
			return true;
		}

		template<typename T>
		void WriteTo(StringView name, const T val) {
			*reinterpret_cast<T*>(buffer.GetData() + reinterpret_cast<HashMap<Id64, JSONObject, ALLOCATOR>*>(buffer.GetData())->At(name).Position) = val;
		}

		Buffer<ALLOCATOR> buffer;

		JSONObject& operator[](const StringView name) {
			return Get<Map>(offset).At(name);
		}

		void Add(const StringView name, uint32 n) {
			*reinterpret_cast<uint32*>(buffer.GetData() + reinterpret_cast<HashMap<Id64, JSONObject, ALLOCATOR>*>(buffer.GetData())->At(name).Position) += n;
		}

		template<typename T>
		T& Get(uint32 oo) {
			return *reinterpret_cast<T*>(buffer.GetData() + oo);
		}

	private:
		uint32 offset = 0;
	};

	template<class ALLOCATOR = DefaultAllocatorReference>
	bool Parse(const StringView str, JSONDeserializer<ALLOCATOR>& deserializer) {
		StaticVector<uint32, 16> objectStack;

		StringView lastKey;

		bool array = false;

		auto makeObject = [&](const JSONObject::Type t) {
			if (!array) {
				auto& ob = deserializer.Get<JSONDeserializer<ALLOCATOR>::Map>(objectStack.back()).At(lastKey);
				ob.ObjectType = t;
				ob.Position = deserializer.buffer.GetLength();
			} else {
				deserializer.Add(lastKey, 1);
			}
		};

		auto whenBool = [&](const bool value) {
			makeObject(JSONObject::Type::BOOL);
			*deserializer.buffer.AllocateStructure<bool>() = value;
		};

		auto whenInt = [&](const int64 value) {
			makeObject(JSONObject::Type::INT);
			*deserializer.buffer.AllocateStructure<int64>() = value;
		};

		auto whenUint = [&](const uint64 value) {
			makeObject(JSONObject::Type::UINT);
			*deserializer.buffer.AllocateStructure<uint64>() = value;
		};

		auto whenFloat = [&](const float32 value) {
			makeObject(JSONObject::Type::FLOAT);
			*deserializer.buffer.AllocateStructure<float32>() = value;
		};

		auto whenString = [&](const StringView value) {
			makeObject(JSONObject::Type::STRING);

			//todo: null terminator
			*deserializer.buffer.AllocateStructure<uint32>() = value.GetBytes();
			*deserializer.buffer.AllocateStructure<uint32>() = value.GetCodepoints();

			for (uint32 i = 0; i < value.GetBytes(); ++i) {
				*deserializer.buffer.AllocateStructure<char8_t>() = value.GetData()[i];
			}
		};

		auto whenKey = [&](const StringView value) {
			deserializer.Get<typename JSONDeserializer<ALLOCATOR>::Map>(objectStack.back()).Emplace(value);
			lastKey = value;
		};

		auto whenArray = [&](const bool start) {
			if (start) {
				makeObject(JSONObject::Type::ARRAY);

				deserializer.buffer.AllocateStructure<uint32>(); //length
				array = true;
			} else { //todo: might not be last key beacuase of change of scope if object are created insside array
				array = false;
			}
		};

		auto whenObject = [&](const bool start) {
			if (start) {
				if (deserializer.buffer.GetLength()) {
					makeObject(JSONObject::Type::OBJECT);
				}

				objectStack.EmplaceBack(deserializer.buffer.GetLength());

				deserializer.buffer.AllocateStructure<HashMap<Id64, JSONObject, ALLOCATOR>>();
				//*deserializer.buffer.AllocateStructure<uint32_t>() = 0;
			} else {
				objectStack.PopBack();
			}
		};

		return ParseJSON(str, whenBool, whenInt, whenUint, whenFloat, whenString, whenKey, whenArray, whenObject);
	}
}
