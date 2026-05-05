#pragma once

#include "Debug/Debug.hpp"
#include <array>
#include <concepts>
#include <cstddef>
#include <algorithm>
#include <new>
#include <tuple>
#include <type_traits>
#include <utility>


template <class Type, class...>
struct IndexOf;

template <class Type, class... Rest>
struct IndexOf<Type, Type, Rest...>
	: public std::integral_constant<size_t, 0>
{};
template <class Type, class UType, class... Rest>
struct IndexOf<Type, UType, Rest...>
	: std::integral_constant<size_t, 1 + IndexOf<Type, Rest...>::value>
{};


template <class Type, class... Types>
inline constexpr size_t IndexOfV = IndexOf<Type, Types...>::value;


template <class... Types>
class TVariant
{
public:
	typedef std::conditional_t<
		sizeof...(Types) < 0xFE,
		uint8_t,
		uint16_t
	> TIndex;
	inline static constexpr size_t AlignSize = std::max<size_t>({ alignof(Types)... });
	inline static constexpr size_t MaxIndex = sizeof...(Types);

	inline static constexpr size_t NPos = TIndex(~0);

public:
	template <TIndex Idx>
	using TAlt = std::tuple_element_t<Idx, std::tuple<Types...>>;
	template <class Type>
	inline static constexpr size_t IndexOfV = ::IndexOfV<Type, Types...>;

public:
	constexpr TVariant()
		requires std::default_initializable<TAlt<0>>
	{
		Construct<TAlt<0>>();
		MyIndex = NPos;
	}

	constexpr TVariant(const TVariant& InOther)
	{
		InOther.CopyTo(*this);
		MyIndex = InOther.MyIndex;
	}
	constexpr TVariant(TVariant&& InOther)
	{
		InOther.MoveTo(*this);
		InOther.MyIndex = NPos;
	}

	template <class Type>
		requires (std::is_same_v<Type, Types> || ...)
	constexpr TVariant(Type&& InValue)
	{
		Construct<std::decay_t<Type>>(std::forward<Type>(InValue));
	}
	template <class Type>
		requires (std::is_same_v<Type, Types> || ...)
	constexpr TVariant(const Type& InValue)
	{
		Construct<std::decay_t<Type>>(InValue);
	}

	constexpr ~TVariant()
	{
		Destruct();
	}

	template <class Type>
		requires (std::is_same_v<Type, Types> || ...)
	[[nodiscard]]
	bool Holds() const { return MyIndex == IndexOfV<Type>; }

	constexpr TVariant& operator=(const TVariant& InOther)
	{
		if (this == &InOther) { return *this; }
		if (!InOther.HasValue())
		{
			Reset();
			return *this;
		}
		else if (MyIndex == InOther.MyIndex)
		{
			InOther.AssignSameIndex(*this);
		}
		else
		{
			Reset();
			InOther.CopyTo(*this);
		}
		return *this;
	}
	constexpr TVariant& operator=(TVariant&& InOther)
	{
		if (this == &InOther) { return *this; }
		if (!InOther.HasValue())
		{
			Reset();
		}
		else if (MyIndex == InOther.MyIndex)
		{
			InOther.MoveAssignSameIndex(*this);
		}
		else
		{
			Reset();
			InOther.MoveTo(*this);
			MyIndex = InOther.MyIndex;
		}
		return *this;
	}

	template <class Type>
		requires (std::is_same_v<Type, Types> || ...)
	constexpr TVariant& operator=(Type&& InValue)
	{
		using UType = std::decay_t<Type>;
		if (MyIndex == IndexOfV<UType>)
		{
			MyStorage.template As<UType>() = std::forward<Type>(InValue);
		}
		else
		{
			Reset();
			Construct<UType>(std::forward<Type>(InValue));
		}
		return *this;
	}
	template <class Type>
		requires (std::is_same_v<Type, Types> || ...)
	constexpr TVariant& operator=(const Type& InValue)
	{
		using UType = std::decay_t<Type>;
		if (MyIndex == IndexOfV<UType>)
		{
			MyStorage.template As<UType>() = InValue;
		}
		else
		{
			Reset();
			Construct<UType>(InValue);
		}
		return *this;
	}

	[[nodiscard]]
	explicit constexpr operator bool() const { return HasValue(); }

	[[nodiscard]]
	constexpr bool HasValue() const noexcept { return MyIndex != NPos; }

	constexpr TIndex Index() const { return MyIndex; }

	constexpr void Reset()
	{
		if (HasValue()) { Destruct(); }
	}
	
	template <class Type, class... Args>
		requires (std::is_same_v<Type, Types> || ...)
	Type& Emplace(Args&&... InArgs)
	{
		Reset();
		Construct<Type>(std::forward<Args>(InArgs)...);
		return MyStorage.template As<Type>();
	}

	template <class Type>
		requires (std::is_same_v<Type, Types> || ...)
	constexpr Type* GetIf()
	{
		return Holds<Type>() ? &MyStorage.template As<Type>() : nullptr;
	}
	template <class Type>
		requires (std::is_same_v<Type, Types> || ...)
	constexpr const Type* GetIf() const
	{
		return Holds<Type>() ? &MyStorage.template As<Type>() : nullptr;
	}

public:
	template <class Func, class FallbackFunc>
	constexpr auto Visit(Func&& InFunc, FallbackFunc&& InFallback) &
	{
		if (!HasValue()) { return std::forward<FallbackFunc>(InFallback)(); }
		return VisitImpl<Func, 0>(*this, std::forward<Func>(InFunc));
	}
	template <class Func, class FallbackFunc>
	constexpr auto Visit(Func&& InFunc, FallbackFunc&& InFallback) const &
	{
		if (!HasValue()) { return std::forward<FallbackFunc>(InFallback)(); }
		return VisitImpl<Func, 0>(*this, std::forward<Func>(InFunc));
	}

	template <class Func, class FallbackFunc>
	constexpr auto Visit(Func&& InFunc, FallbackFunc&& InFallback) &&
	{
		if (!HasValue()) { return std::forward<FallbackFunc>(InFallback)(); }
		return VisitImpl<Func, 0>(std::move(*this), std::forward<Func>(InFunc));
	}
	template <class Func, class FallbackFunc>
	constexpr auto Visit(Func&& InFunc, FallbackFunc&& InFallback) const &&
	{
		if (!HasValue()) { return std::forward<FallbackFunc>(InFallback)(); }
		return VisitImpl<Func, 0>(std::move(*this), std::forward<Func>(InFunc));
	}

public:
	template <class Type, class... Args>
		requires (std::is_same_v<Type, Types> || ...)
	constexpr void Construct(Args&&... InArgs)
	{
		new (MyStorage.MyBuffer) Type(std::forward<Args>(InArgs)...);
		MyIndex = IndexOfV<Type>;
	}

	template <class Type>
	static void DestructOne(void* InPtr)
	{
		if constexpr (!std::is_trivially_destructible_v<Type>)
		{
			std::launder(reinterpret_cast<Type*>(InPtr))->~Type();
		}
	}
	template <TIndex... Is>
	consteval auto MakeDestructorTable(std::index_sequence<Is...>)
	{
		return std::array<void(*)(void*), sizeof...(Is)> {
			&DestructOne<TAlt<Is>>...
		};
	}

	constexpr void Destruct()
	{
		if (!HasValue()) { return; }
		static constexpr auto Destructors =
			MakeDestructorTable(std::make_index_sequence<sizeof...(Types)>{});
		Destructors[MyIndex](MyStorage.MyBuffer);
		MyIndex = NPos;
	}

	constexpr void CopyTo(TVariant& OutDst) const { CopyToImpl<0>(OutDst); }
	constexpr void AssignSameIndex(TVariant& OutDst) const { AssignSameIndexImpl<0>(OutDst); }
	constexpr void MoveAssignSameIndex(TVariant& OutDst) const { MoveAssignSameIndex<0>(OutDst); }
	constexpr void MoveTo(TVariant& OutDst) const { MoveToImpl<0>(OutDst); }

	template <TIndex Idx>
		requires (Idx == sizeof...(Types))
	constexpr void CopyToImpl(TVariant&) const {}
	template <TIndex Idx>
		requires (Idx < sizeof...(Types))
	constexpr void CopyToImpl(TVariant& OutDst) const
	{
		if (MyIndex == Idx)
		{
			OutDst.Construct<TAlt<Idx>>(MyStorage.template As<TAlt<Idx>>());
			return;
		}
		CopyToImpl<Idx + 1>(OutDst);
	}
	template <TIndex Idx>
		requires (Idx == sizeof...(Types))
	constexpr void AssignSameIndexImpl(TVariant&) const {}
	template <TIndex Idx>
		requires (Idx < sizeof...(Types))
	constexpr void AssignSameIndexImpl(TVariant& OutDst) const
	{
		if (MyIndex == Idx)
		{
			OutDst.MyStorage.template As<TAlt<Idx>>() = MyStorage.template As<TAlt<Idx>>();
			return;
		}
		AssignSameIndexImpl<Idx + 1>(OutDst);
	}
	template <TIndex Idx>
		requires (Idx == sizeof...(Types))
	constexpr void MoveAssignSameIndex(TVariant&) const {}
	template <TIndex Idx>
		requires (Idx < sizeof...(Types))
	constexpr void MoveAssignSameIndex(TVariant& OutDst) const
	{
		if (MyIndex == Idx)
		{
			OutDst.MyStorage.template As<TAlt<Idx>>() = std::move(MyStorage).template As<TAlt<Idx>>();
		}
		MoveAssignSameIndex<Idx + 1>(OutDst);
	}
	template <TIndex Idx>
		requires (Idx == sizeof...(Types))
	constexpr void MoveToImpl(TVariant&) const {}
	template <TIndex Idx>
		requires (Idx < sizeof...(Types))
	constexpr void MoveToImpl(TVariant& OutDst) const
	{
		if (MyIndex == Idx)
		{
			OutDst.Construct<TAlt<Idx>>(std::move(MyStorage).template As<TAlt<Idx>>());
		}
		MoveToImpl<Idx + 1>(OutDst);
	}

	template <class Func, TIndex Idx, class Variant>
		requires (Idx == sizeof...(Types))
	static constexpr decltype(auto) VisitImpl(Variant, Func&&)
	{
		return typename std::invoke_result_t<Func, TAlt<0>>{};
	}
	template <class Func, TIndex Idx, class Variant>
		requires (Idx < sizeof...(Types))
	static constexpr decltype(auto) VisitImpl(Variant InSelf, Func&& InFunc)
	{
		if (InSelf.MyIndex == Idx)
		{
			if constexpr (std::is_rvalue_reference_v<Variant&&>)
			{
				return std::forward<Func>(InFunc)(
					std::move(InSelf.MyStorage).template As<TAlt<Idx>>()
				);
			}
			else
			{
				return std::forward<Func>(InFunc)(
					InSelf.MyStorage.template As<TAlt<Idx>>()
				);
			}
		}
		return VisitImpl<Func, Idx + 1>(std::forward<Variant>(InSelf), std::forward<Func>(InFunc));
	}

public:
	struct TStorage
	{
		alignas(AlignSize)
		std::byte MyBuffer[std::max<size_t>({ sizeof(Types)... })];

		template <class Type>
		[[nodiscard]]
		const Type& As() const
		{
			return *std::launder(reinterpret_cast<const Type*>(MyBuffer));
		}
		template <class Type>
		[[nodiscard]]
		Type& As()
		{
			return *std::launder(reinterpret_cast<Type*>(MyBuffer));
		}
	};

PRIVATE:
	TStorage MyStorage;
	TIndex MyIndex;
};
