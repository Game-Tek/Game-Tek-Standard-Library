#pragma once

#include <type_traits>

namespace GTSL {
	template<typename T>
	constexpr bool IsIntegral() { return false; };

	template<>
	constexpr bool IsIntegral<uint32>() { return true; }

	template<>
	constexpr bool IsIntegral<uint64>() { return true; }

	template<typename T>
	concept Integral = IsIntegral<T>();
	
	template<typename E>
	inline constexpr bool IsEnum = __is_enum(E);

	template<typename U>
	inline constexpr bool IsUnion = __is_union(U);

	template<typename T>
	inline constexpr bool IsPOD = __is_pod(T);
	
	template<typename E>
	concept Enum = IsEnum<E>;

	template<typename U>
	concept Union = IsUnion<U>;

	template<typename T>
	concept POD = IsPOD<T>;

	template<Enum E>
	using UnderlyingType = __underlying_type(E);

	template<class T, class... Ts>
	constexpr bool IsInPack() noexcept {
		return (std::is_same_v<T, Ts> || ...);
	}

	template<size_t POS, typename... TYPES>
	struct TypeAt;

	template<typename HEAD, typename... TAIL >
	struct TypeAt<0, HEAD, TAIL...> {
		using type = HEAD;
	};

	template<size_t POS, typename HEAD, typename... TAIL >
	struct TypeAt<POS, HEAD, TAIL...> {
		using type = typename TypeAt<POS - 1, TAIL...>::type;
	};

	template<typename A, typename B>
	constexpr bool IsSame() { return std::is_same_v<A, B>; }

	template<typename... ARGS>
	consteval size_t PackSize() {
		return (sizeof(ARGS) + ... + 0);
	}

	template<uint32 I, typename Target, typename ListHead, typename... ListTails>
	constexpr uint32 GetTypeIndex_impl() {
		if constexpr (std::is_same_v<Target, ListHead>)
			return I;
		else
			return GetTypeIndex_impl<I + 1, Target, ListTails...>();
	}

	template<typename Target, typename... ListTails>
	constexpr uint32 GetTypeIndex() {
		static_assert(IsInPack<Target, ListTails...>(), "Type T is not in template pack.");
		return GetTypeIndex_impl<0, Target, ListTails...>();
	}

	template<uint32 AT, uint32 I, typename HEAD, typename... TAIL>
	consteval size_t PackSizeAt_impl() {
		if constexpr (AT == I)
			return 0;
		else
			return sizeof(HEAD) + PackSizeAt_impl<AT, I + 1, TAIL...>();
	}

	template<uint32 AT, typename... ARGS>
	consteval size_t PackSizeAt() {
		return PackSizeAt_impl<AT, 0, ARGS...>();
	}

	template<typename T>
	std::reference_wrapper<const T> ref(const T& a) {
		return a;
	}
}