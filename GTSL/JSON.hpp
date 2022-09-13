#pragma once

#include "Core.h"
#include "HashMap.hpp"
#include "StringCommon.h"
#include "Range.hpp"
#include "Vector.hpp"
#include "String.hpp"

namespace GTSL {
	class JSONSerialization {
		
	};

	template<class T>
	class Serializer;

	template<>
	class Serializer<JSONSerialization> {
		Serializer() = default;

		friend Serializer<JSONSerialization> MakeSerializer(auto&);
	public:
		using type = JSONSerialization;

		bool newLine = true;

		StaticVector<uint32, 16> stack;
		//String<> storage;

		~Serializer() = default;
	};
	
	using JSONSerializer = Serializer<JSONSerialization>;

	template<typename T>
	struct JSONObject {
		JSONObject(const StringView name, const T& r) : Name(name), Ref(r) {}
		StringView Name; const T& Ref;
	};

	template<typename T>
	JSONObject(StringView, const T&) -> JSONObject<T>;

	template<typename T>
	inline JSONSerializer& operator<<(JSONSerializer& serializer, const JSONObject<T> n) {
		return serializer;
	}

	template<class S = StaticString<1024>>
	void EndSerializer(auto& buffer, JSONSerializer& serializer) {
		if(serializer.stack.back() > 0)
			RTrimLast(buffer, u8',');

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
			RTrimLast(buffer, ',');

		serializer.stack.PopBack();
	}

	JSONSerializer MakeSerializer(auto& buffer) {
		ToString(buffer, u8"{");
		auto serializer = JSONSerializer();
		StartJSONObject(serializer, buffer);
		return serializer;
	}

	template<class B>
	void StartObject(JSONSerializer& serializer, B& buffer) {
		StartJSONValue(serializer, buffer);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'{');
	}

	template<class B>
	void StartObject(JSONSerializer& serializer, B& buffer, StringView name) {
		StartJSONPair(serializer, buffer, name);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'{');
	}

	template<class B>
	void EndObject(JSONSerializer& serializer, B& buffer) {
		EndJSONObject(serializer, buffer);
		ToString(buffer, u8"}");
		EndJSONValue(serializer, buffer);
	}

	template<class B>
	void StartArray(JSONSerializer& serializer, B& buffer) {
		StartJSONValue(serializer, buffer);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'[');
	}

	template<class B>
	void StartArray(JSONSerializer& serializer, B& buffer, StringView name) {
		StartJSONPair(serializer, buffer, name);
		StartJSONObject(serializer, buffer);
		ToString(buffer, u8'[');
	}

	template<class B>
	void EndArray(JSONSerializer& serializer, B& buffer) {
		EndJSONObject(serializer, buffer);
		ToString(buffer, u8"]");
		EndJSONValue(serializer, buffer);
	}

	template<std::integral T, class B>
	void Insert(JSONSerializer& serializer, B& buffer, const T& val) {
		StartJSONValue(serializer, buffer); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<std::integral T, class B>
	void Insert(JSONSerializer& serializer, B& buffer, StringView name, const T& val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<std::floating_point T, class B>
	void Insert(JSONSerializer& serializer, B& buffer, T val) {
		StartJSONValue(serializer, buffer); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(JSONSerializer& serializer, B& buffer, StringView name, float32 val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(JSONSerializer& serializer, B& buffer, StringView name, bool val) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, val ? u8"true" : u8"false"); EndJSONValue(serializer, buffer);
	}

	template<Enum E, class B>
	void Insert(JSONSerializer& serializer, B& buffer, StringView name, E, StringView enumStringType) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'"'); ToString(buffer, enumStringType); ToString(buffer, u8'"'); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(JSONSerializer& serializer, B& buffer, StringView string) {
		StartJSONValue(serializer, buffer); ToString(buffer, u8'"'); ToString(buffer, string); ToString(buffer, u8'"'); EndJSONValue(serializer, buffer);
	}

	template<class B>
	void Insert(JSONSerializer& serializer, B& buffer, StringView name, StringView string) {
		StartJSONPair(serializer, buffer, name); ToString(buffer, u8'"'); ToString(buffer, string); ToString(buffer, u8'"'); EndJSONValue(serializer, buffer);
	}

	template<class B, class I>
	void Insert(JSONSerializer& serializer, B& buffer, StringView name, Range<I> range) {
		StartArray(serializer, buffer, name);

		for(auto& e : range) {
			Insert(serializer, buffer, e);
		}

		EndArray(serializer, buffer);
	}

	enum class JSONObjectType : uint8 {
		NONE, BOOL, INT, UINT, FLOAT, STRING, ARRAY, OBJECT
	};

	bool ParseJSON(StringView string, auto&& whenBool, auto&& whenInt, auto&& whenUint, auto&& whenFloat, auto&& whenString, auto&& whenKey, auto&& whenArray, auto&& whenObject) {
		StringIterator tokenStart = string.begin(), tokenEnd = string.begin(), ptr = string.begin();

		GTSL::StaticVector<bool, 32> useKeys;
		GTSL::StaticVector<uint32, 32> commas;

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

				if (IsNumber(str)) {
					if(IsDecimalNumber(str)) {
						whenFloat(ToNumber<float32>(str).Get());
					} else if (*tokenStart == U'-') {
						whenInt(ToNumber<int64>(str).Get());
					} else {
						auto res = ToNumber<uint64>(str);
						if (!res) {
#if _WIN64
							__debugbreak();
#endif
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
				useKeys.EmplaceBack(true);
				commas.EmplaceBack(0);
				break;
			}
			case u8'}': {
				evalObject();
				whenObject(false);
				sync();
				useKeys.PopBack();
				commas.PopBack();
				break;
			}
			case u8'[': {
				whenArray(true);
				sync();
				useKeys.EmplaceBack(false);
				commas.EmplaceBack(0);
				break;
			}
			case u8']': {
				evalObject();
				sync();
				whenArray(false);
				useKeys.PopBack();
				commas.PopBack();
				break;
			}
			case u8'"': {
				sync();

				while (ptr < string.end() and *ptr != U'"') {
					advance();
				}

				if (useKeys.back()) {
					if (commas.back()) {
						whenString(StringView(tokenStart, tokenEnd));
					} else {
						whenKey(StringView(tokenStart, tokenEnd));						
					}
				} else {
					whenString(StringView(tokenStart, tokenEnd));
				}

				sync();

				break;
			}
			case u8':': {
				//evalObject();
				sync();
				commas.back() += 1;
				break;
			}
			case u8',': {
				evalObject();
				sync();
				commas.back() -= 1;
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
		case JSONObjectType::NONE: of += 0; break;
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

	template<class ALLOCATOR>
	struct JSON {
		template<typename T>
		static T& as(byte* data, uint64* offset) {
			auto& ref=*reinterpret_cast<T*>(data + *offset);
			*offset += sizeof(T);
			return ref;
		}

		JSON(const GTSL::StringView string_view, const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {
			Allocate(this->allocator, string_view.GetBytes() * 2, &buffer, &capacity);

			StaticVector<uint32, 16> objectOffsetsStack;

			StringView lastKey;

			uint64 length = 0;

			auto makeObject = [&](const JSONObjectType t) {
				as<JSONObjectType>(buffer, &length) = t;
				as<uint64>(buffer, &length) = Hash(lastKey);
				++getMemberLength(buffer, objectOffsetsStack.back());
			};

			auto whenBool = [&](const bool value) {
				makeObject(JSONObjectType::BOOL);
				as<bool>(buffer, &length) = value;
			};

			auto whenInt = [&](const int64 value) {
				makeObject(JSONObjectType::INT);
				as<int64>(buffer, &length) = value;
			};

			auto whenUint = [&](const uint64 value) {
				makeObject(JSONObjectType::UINT);
				as<uint64>(buffer, &length) = value;
			};

			auto whenFloat = [&](const float32 value) {
				makeObject(JSONObjectType::FLOAT);
				as<float32>(buffer, &length) = value;
			};

			auto whenString = [&](const StringView value) {
				makeObject(JSONObjectType::STRING);

				//todo: null terminator
				as<uint32>(buffer, &length) = value.GetBytes();
				as<uint32>(buffer, &length) = value.GetCodepoints();

				for (uint32 i = 0; i < value.GetBytes(); ++i) {
					as<char8_t>(buffer, &length) = value.GetData()[i];
				}
			};

			auto whenKey = [&](const StringView value) {
				lastKey = value;
			};

			auto whenArray = [&](const bool start) {
				if (start) {
					auto l = objectOffsetsStack.GetLength();

					objectOffsetsStack.EmplaceBack(length);

					as<JSONObjectType>(buffer, &length) = JSONObjectType::ARRAY;
					as<uint64>(buffer, &length) = Hash(lastKey);
					as<uint32>(buffer, &length) = 0u;

					++getMemberLength(buffer, objectOffsetsStack[l - 1]);
				} else {
					objectOffsetsStack.PopBack();
				}
			};

			auto whenObject = [&](const bool start) {
				if (start) {
					auto l = objectOffsetsStack.GetLength();

					objectOffsetsStack.EmplaceBack(length);

					as<JSONObjectType>(buffer, &length) = JSONObjectType::OBJECT;

					if (l) {
						as<uint64>(buffer, &length) = Hash(lastKey); //name
						++getMemberLength(buffer, objectOffsetsStack[l - 1]);
					} else {
						as<uint64>(buffer, &length) = 0ull; //name
					}

					as<uint32>(buffer, &length) = 0u; //length

				} else {
					objectOffsetsStack.PopBack();
				}
			};

			valid = ParseJSON(string_view, whenBool, whenInt, whenUint, whenFloat, whenString, whenKey, whenArray, whenObject);

			if(length >= capacity) {
				exit(-1);
			}
		}

		JSON(const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator), valid(false) {}

		JSON(const byte* data, uint32 of, const ALLOCATOR& allocator = ALLOCATOR()) : allocator(allocator) {
			auto size = getJumpLengthSameLevel(data, of);
			Allocate(this->allocator, size, &buffer, &capacity);
			MemCopy(size, data + of, buffer);
		}

		JSON(JSON&& other) noexcept : allocator(GTSL::MoveRef(other.allocator)), buffer(other.buffer), capacity(other.capacity), valid(other.valid) {
			other.buffer = nullptr;
			other.valid = false;
			other.capacity = 0;
		}

		JSON& operator=(JSON&& other) noexcept {
			if(buffer) {
				Deallocate(allocator, capacity, buffer);
			}

			allocator = GTSL::MoveRef(other.allocator);
			buffer = other.buffer;
			capacity = other.capacity;
			valid = other.valid;

			other.buffer = nullptr;
			other.valid = false;
			other.capacity = 0;

			return *this;
		}

		~JSON() {
			if(buffer) {
				Deallocate(allocator, capacity, buffer);
			}
		}

		bool operator()(uint64& obj) {
			if (valid) {
				obj = *reinterpret_cast<const uint64*>(buffer + 0 + 9);
				return true;
			}

			return false;
		}

		template<Enum T>
		bool operator()(T& obj, auto&& lookupFunction) {
			if (valid && interpretBytes<JSONObjectType>(buffer, 0) == JSONObjectType::STRING) {
				auto res = lookupFunction(StringView(*reinterpret_cast<const uint32*>(buffer + 0 + 9), *reinterpret_cast<const uint32*>(buffer + 0 + 4 + 9),
					reinterpret_cast<const char8_t*>(buffer + 0 + 8 + 9)), obj);
				return res;
			}

			return false;
		}

		template<class ALLOC>
		bool operator()(String<ALLOC>& obj) {
			if (valid) {
				obj = StringView(*reinterpret_cast<const uint32*>(buffer + 0 + 9), *reinterpret_cast<const uint32*>(buffer + 0 + 4 + 9),
								reinterpret_cast<const char8_t*>(buffer + 0 + 8 + 9));
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
		explicit operator int64() const { return GetInt(); }
		explicit operator float32() const { return GetFloat(); }

		StringView GetStringView() const {
			return { *reinterpret_cast<const uint32*>(buffer + 0 + 9), *reinterpret_cast<const uint32*>(buffer + 0 + 4 + 9),
			reinterpret_cast<const char8_t*>(buffer + 0 + 8 + 9) };
		}

		bool GetBool() const { return interpretBytes<bool>(buffer, 0 + 1 + 8); }
		int64 GetInt() const { return interpretBytes<int64>(buffer, 0 + 1 + 8); }
		float32 GetFloat() const { return interpretBytes<float32>(buffer, 0 + 1 + 8); }
		uint64 GetUint() const { return interpretBytes<uint64>(buffer, 0 + 1 + 8); }

		JSON operator[](const StringView name) const {
			uint32 o = 0 + 1 + 8 + 4; //skip self

			uint32 i = 0;

			while(i < GetCount() && getMemberName(buffer, o) != Hash(name)) {
				o += getJumpLengthSameLevel(buffer, o);
				++i;
			}

			if (i != GetCount()) {
				return { buffer, o, allocator.GetAllocator() };
			} else {
				return { allocator.GetAllocator() };
			}
		}

		JSON operator[](const uint64 index) const {
			uint32 o = 0 + 1 + 8 + 4; //skip self

			for (auto i = 0ull; i < index; ++i) {
				o += getJumpLengthSameLevel(buffer, o);
			}

			return { buffer, o, allocator.GetAllocator() };
		}

		uint32 GetCount() const {
			if(valid) {
				return getMemberLength(buffer, 0);				
			}

			return 0;
		}

		struct Iterator {
			Iterator(const JSON* p, uint64 ni) : parent(p), i(ni) {}

			Iterator operator++() {
				return { parent, ++i };
			}

			bool operator!=(const Iterator& other) {
				return i != other.i;
			}

			bool operator<(const Iterator& other) {
				return i < other.i;
			}

			JSON operator*() const {
				return parent->operator[](i);
			}

		private:
			const JSON* parent;
			uint64 i = 0;
		};

		[[nodiscard]] Iterator begin() const { return { this, 0 }; }
		[[nodiscard]] Iterator end() const { return { this, GetCount() }; }

		inline friend bool operator<(uint32 a, const JSON& m) {
			return a < m.GetCount();
		}

	private:
		[[no_unique_address]] GTSL::DoubleAllocator<64, ALLOCATOR> allocator;
		byte* buffer = nullptr;
		uint32 capacity = 0;
		
		bool valid = true;
	};
}