#pragma once

#include "Core.h"
#include "HashMap.hpp"
#include "JSON.hpp"
#include "JSON.hpp"
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

		StaticVector<uint32, 16> stack;

		~Serializer() = default;
	};

	using JSONSerializer = Serializer<JSON>;

	void EndSerializer(auto& buffer, JSONSerializer& serializer) {
		if(serializer.stack.back() > 0)
			DropLast(buffer, u8',');

		ToString(buffer, u8'}');

		serializer.stack.PopBack();
	}

	void StartJSONPair(JSONSerializer& serializer, auto& buffer, StringView name) {
		ToString(buffer, u8'"'); ToString(buffer, name); ToString(buffer, u8'"'); ToString(buffer, u8':');
		++serializer.stack.back();
	}

	void StartJSONValue(JSONSerializer& serializer, auto& buffer) {
		++serializer.stack.back();
	}

	void StartJSONObject(JSONSerializer& serializer, auto& buffer) {
		serializer.stack.EmplaceBack(0);
	}

	void EndJSONValue(JSONSerializer& serializer, auto& buffer) {
		ToString(buffer, u8',');
	}

	void EndJSONObject(JSONSerializer& serializer, auto& buffer) {
		if (serializer.stack.back())
			DropLast(buffer, ',');

		serializer.stack.PopBack();
	}

	JSONSerializer MakeSerializer(auto& buffer) {
		ToString(buffer, u8"{");
		auto serializer = JSONSerializer();
		StartJSONObject(serializer, buffer);
		return serializer;
	}

	template<class B>
	void StartObject(Serializer<JSON>& serializer, B& buffer) {
		StartJSONValue(serializer, buffer);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'{');
	}

	template<class B>
	void StartObject(Serializer<JSON>& serializer, B& buffer, StringView name) {
		StartJSONPair(serializer, buffer, name);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'{');
	}

	template<class B>
	void EndObject(Serializer<JSON>& serializer, B& buffer) {
		EndJSONObject(serializer, buffer);
		ToString(buffer, u8"}");
		EndJSONValue(serializer, buffer);
	}

	template<class B>
	void StartArray(Serializer<JSON>& serializer, B& buffer) {
		StartJSONValue(serializer, buffer);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'[');
	}

	template<class B>
	void StartArray(Serializer<JSON>& serializer, B& buffer, StringView name) {
		StartJSONPair(serializer, buffer, name);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'[');
	}

	template<class B>
	void EndArray(Serializer<JSON>& serializer, B& buffer) {
		EndJSONObject(serializer, buffer);
		ToString(buffer, u8"]");
		EndJSONValue(serializer, buffer);
	}

	template<std::integral T, class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, const T& val) {
		StartJSONValue(serializer, buffer); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<std::integral T, class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, const T& val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<std::floating_point T, class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, T val) {
		StartJSONValue(serializer, buffer); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, float32 val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, bool val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val ? u8"true" : u8"false"); EndJSONValue(serializer, buffer);
	}

	template<Enum E, class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, E, StringView enumStringType) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'"'); ToString(buffer, enumStringType); ToString(buffer, u8'"'); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView string) {
		StartJSONValue(serializer, buffer); ToString(buffer, u8'"'); ToString(buffer, string); ToString(buffer, u8'"'); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(Serializer<JSON>& serializer, B& buffer, StringView name, StringView string) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'"'); ToString(buffer, string); ToString(buffer, u8'"'); EndJSONValue(serializer, buffer);
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

	enum class JSONObjectType : uint8 {
		NULL, BOOL, INT, UINT, FLOAT, STRING, ARRAY, OBJECT
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

	template<typename T>
	T& interpretBytes(byte* data, uint32_t off) {
		return *reinterpret_cast<T*>(data + off);
	}

	template<typename T>
	const T& interpretBytes(const byte* data, uint32_t off) {
		return *reinterpret_cast<const T*>(data + off);
	}

	inline uint32& getMemberLength(byte* data, uint32 offset) {
		return *reinterpret_cast<uint32*>(data + offset + 1/*type*/ + 8/*name*/);
	}

	inline uint32 getMemberLength(const byte* data, uint32 offset) {
		return *reinterpret_cast<const uint32*>(data + offset + 1/*type*/ + 8/*name*/);
	}

	inline uint32 getJumpLengthSameLevel(const byte* data, uint32_t offset) {
		uint32 of = 1/*type*/ + 8/*object name*/;

		switch (interpretBytes<JSONObjectType>(data, offset)) {
		case JSONObjectType::NULL: of += 0; break;
		case JSONObjectType::BOOL: of += 1; break;
		case JSONObjectType::INT: of += 8; break;
		case JSONObjectType::UINT: of += 8; break;
		case JSONObjectType::FLOAT: of += 4; break;
		case JSONObjectType::STRING:
			of += 4/*bytes*/ + 4/*codepoints*/ + interpretBytes<uint32>(data, offset + of)/*chars*/;
			break;
		case JSONObjectType::OBJECT:
		case JSONObjectType::ARRAY:
			auto len = getMemberLength(data, offset);
			of += 4/*len*/;

			for(auto i = 0ull; i < len; ++i) {
				of += getJumpLengthSameLevel(data, offset + of);
			}

			break;
		}

		return of;
	}

	inline uint64 getMemberName(const byte* data, uint32 offset) {
		return *reinterpret_cast<const uint64*>(data + offset + 1/*type*/);
	}

	inline JSONObjectType getMemberType(const byte* data, uint32 offset) {
		return *reinterpret_cast<const JSONObjectType*>(data + offset);
	}

	struct JSONMember {
		JSONMember() : valid(false) {}
		JSONMember(const byte* data, bool v) : buffer(data), valid(v) {}
		JSONMember(const byte* data) : buffer(data) {}
		JSONMember(const byte* data, uint32 of) : buffer(data), offset(of), type(getMemberType(data, of)) {}

		bool operator()(uint64& obj) {
			if (valid) {
				obj = *reinterpret_cast<const uint64*>(buffer + offset + 9);
				return true;
			}

			return false;
		}

		template<Enum T>
		bool operator()(T& obj, auto&& lookupFunction) {
			if (valid && type == JSONObjectType::STRING) {
				auto res = lookupFunction(StringView(*reinterpret_cast<const uint32*>(buffer + offset + 9), *reinterpret_cast<const uint32*>(buffer + offset + 4 + 9),
					reinterpret_cast<const char8_t*>(buffer + offset + 8 + 9)), obj);
				return res;
			}

			return false;
		}

		template<class ALLOC>
		bool operator()(String<ALLOC>& obj) {
			if (valid) {
				obj = StringView(*reinterpret_cast<const uint32*>(buffer + offset + 9), *reinterpret_cast<const uint32*>(buffer + offset + 4 + 9),
								reinterpret_cast<const char8_t*>(buffer + offset + 8 + 9));
				return true;
			}

			return false;
		}

		//template<typename T, class ALLOC>
		//bool operator()(Vector<T, ALLOC>& obj) {
		//	if (valid && type == JSONObjectType::ARRAY) {
		//		obj = Range<const T*>(*reinterpret_cast<const uint32*>(buffer + search.Get().Position), reinterpret_cast<const T*>(buffer.GetData() + search.Get().Position + 4));
		//		return true;
		//	}
		//
		//	return false;
		//}

		explicit operator bool() const { return valid; }

		operator StringView() const { return GetStringView(); }
		explicit operator uint64() const { return GetUint(); }

		StringView GetStringView() const {
			return { *reinterpret_cast<const uint32*>(buffer + offset + 9), *reinterpret_cast<const uint32*>(buffer + offset + 4 + 9),
			reinterpret_cast<const char8_t*>(buffer + offset + 8 + 9) };
		}

		bool GetBool() const { return interpretBytes<bool>(buffer, offset + 1 + 8); }
		int64 GetInt() const { return interpretBytes<int64>(buffer, offset + 1 + 8); }
		float32 GetFloat() const { return interpretBytes<float32>(buffer, offset + 1 + 8); }
		uint64 GetUint() const { return interpretBytes<uint64>(buffer, offset + 1 + 8); }

		JSONMember operator[](const StringView name) const {
			uint32 o = offset + 1 + 8 + 4; //skip self

			uint32 i = 0;

			while(i < GetCount() && getMemberName(buffer, o) != Hash(name)) {
				o += getJumpLengthSameLevel(buffer, o);
				++i;
			}

			if (i != GetCount()) {
				return { buffer, o };
			} else {
				return {};
			}
		}

		JSONMember operator[](const uint64 index) const {
			uint32 o = offset + 1 + 8 + 4; //skip self

			for (auto i = 0ull; i < index; ++i) {
				o += getJumpLengthSameLevel(buffer, o);
			}

			return { buffer, o };
		}

		uint32 GetCount() const {
			return getMemberLength(buffer, offset);
		}

		struct Iterator {
			Iterator(const JSONMember* p, uint64 ni) : parent(p), i(ni) {}

			Iterator operator++() {
				return { parent, ++i };
			}

			bool operator!=(const Iterator& other) {
				return i != other.i;
			}

			bool operator<(const Iterator& other) {
				return i < other.i;
			}

			JSONMember operator*() const {
				return parent->operator[](i);
			}

		private:
			const JSONMember* parent;
			uint64 i = 0;
		};

		[[nodiscard]] Iterator begin() const { return { this, 0 }; }
		[[nodiscard]] Iterator end() const { return { this, GetCount() }; }

	private:
		const byte* buffer = nullptr;
		uint32 offset = 0;
		bool valid = true;
		JSONObjectType type = JSONObjectType::NULL;
	};

	inline bool operator<(uint32 a, const JSONMember& m) {
		return a < m.GetCount();
	}

	template<class ALLOCATOR = DefaultAllocatorReference>
	JSONMember Parse(const StringView str, Buffer<ALLOCATOR>& buffer) {
		StaticVector<uint32, 16> objectOffsetsStack;

		StringView lastKey;

		auto makeObject = [&](const JSONObjectType t) {
			buffer.AllocateStructure<JSONObjectType>(t);
			buffer.AllocateStructure<uint64>(Hash(lastKey)); //name
			++getMemberLength(buffer.GetData(), objectOffsetsStack.back());
		};

		auto whenBool = [&](const bool value) {
			makeObject(JSONObjectType::BOOL);
			buffer.AllocateStructure<bool>(value);
		};

		auto whenInt = [&](const int64 value) {
			makeObject(JSONObjectType::INT);
			buffer.AllocateStructure<int64>(value);
		};

		auto whenUint = [&](const uint64 value) {
			makeObject(JSONObjectType::UINT);
			buffer.AllocateStructure<uint64>(value);
		};

		auto whenFloat = [&](const float32 value) {
			makeObject(JSONObjectType::FLOAT);
			buffer.AllocateStructure<float32>(value);
		};

		auto whenString = [&](const StringView value) {
			makeObject(JSONObjectType::STRING);

			//todo: null terminator
			buffer.AllocateStructure<uint32>(value.GetBytes());
			buffer.AllocateStructure<uint32>(value.GetCodepoints());

			for (uint32 i = 0; i < value.GetBytes(); ++i) {
				buffer.AllocateStructure<char8_t>(value.GetData()[i]);
			}
		};

		auto whenKey = [&](const StringView value) {
			lastKey = value;
		};

		auto whenArray = [&](const bool start) {
			if (start) {
				auto l = objectOffsetsStack.GetLength();

				objectOffsetsStack.EmplaceBack(buffer.GetLength());

				buffer.AllocateStructure<JSONObjectType>(JSONObjectType::ARRAY);
				buffer.AllocateStructure<uint64>(Hash(lastKey)); //name				
				buffer.AllocateStructure<uint32>(0); //length

				++getMemberLength(buffer.GetData(), objectOffsetsStack[l - 1]);
			} else {
				objectOffsetsStack.PopBack();
			}
		};

		auto whenObject = [&](const bool start) {
			if (start) {
				auto l = objectOffsetsStack.GetLength();

				objectOffsetsStack.EmplaceBack(buffer.GetLength());

				buffer.AllocateStructure<JSONObjectType>(JSONObjectType::OBJECT);

				if (l) {
					buffer.AllocateStructure<uint64>(Hash(lastKey)); //name
					++getMemberLength(buffer.GetData(), objectOffsetsStack[l - 1]);
				} else {
					buffer.AllocateStructure<uint64>(0); //name
				}

				buffer.AllocateStructure<uint32>(0); //length

			} else {
				objectOffsetsStack.PopBack();
			}
		};

		auto parseResult = ParseJSON(str, whenBool, whenInt, whenUint, whenFloat, whenString, whenKey, whenArray, whenObject);

		return { buffer.GetData(), parseResult };
	}
}
