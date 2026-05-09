#pragma once

#include "Debug/Debug.hpp"
#include <type_traits>


template <size_t Size, size_t Align>
struct TAlignedStorage
{
	alignas(Align) std::byte Buffer;
};

template <class, class...>
class TFunction;

template <class ReturnT, class... Args>
class TFunction <ReturnT(Args...)>
{
public:
	inline static constexpr std::size_t InlineSize = 32;
	inline static constexpr std::size_t InlineAlignSize = alignof(std::max_align_t);
	using InlineStorage = TAlignedStorage<InlineSize, InlineAlignSize>;

	struct ICallable
	{
		virtual ~ICallable() = default;

		virtual ReturnT Invoke(Args&&... InArgs) = 0;
		virtual ICallable* Clone(void* OutStorage, bool& OutInline) const = 0;
		virtual ICallable* Move(void* OutStorage, bool& OutInline) = 0;
		virtual void Destroy(bool InIsInline) noexcept = 0;
	};
	template <class FunctionT>
	struct TCallable final
		: public ICallable
	{
		FunctionT Function;

		template <class FunctionU>
		explicit TCallable(FunctionU&& InFunction)
			: Function(std::forward<FunctionU>(InFunction))
		{}

		ReturnT Invoke(Args&&... InArgs) override
		{
			if constexpr (std::is_void_v<ReturnT>)
			{
				std::invoke(Function, std::forward<Args>(InArgs)...);
			}
			else
			{
				return std::invoke(Function, std::forward<Args>(InArgs)...);
			}
		}
		ICallable* Clone(void* OutStorage, bool& OutInline) const override
		{
			if constexpr (CanInline())
			{
				OutInline = true;
				return new (OutStorage) TCallable(Function);
			}
			else
			{
				OutInline = false;
				return new TCallable(Function);
			};
		}
		ICallable* Move(void* OutStorage, bool& OutInline) override
		{
			if constexpr (CanInline())
			{
				OutInline = true;
				return new (OutStorage) TCallable(std::move(Function));
			}
			else
			{
				OutInline = false;
				return new TCallable(std::move(Function));
			}
		}
		void Destroy(bool InIsInline) noexcept override
		{
			if (InIsInline)
			{
				this->~TCallable();
			}
			else
			{
				delete this;
			}
		}
		static constexpr bool CanInline()
		{
			return sizeof(TCallable) <= InlineSize &&
				alignof(TCallable) <= InlineAlignSize &&
				std::is_nothrow_move_constructible_v<FunctionT>;
		}
	};

private:

	template <class FunctionT>
	static constexpr bool IsCallableCompatible()
	{
		if constexpr (std::is_void_v<ReturnT>)
		{
			return std::is_invocable_v<FunctionT&, Args...>;
		}
		else
		{
			return std::is_invocable_r_v<ReturnT, FunctionT&, Args...>;
		}
	}

public:
	TFunction() noexcept = default;
	TFunction(std::nullptr_t) noexcept {}
	~TFunction()
	{
		Reset();
	}

	TFunction(const TFunction& InOther)
	{
		if (InOther.MyCallable)
		{
			MyCallable = InOther.MyCallable->Clone(&MyStorage, MybInline);
		}
	}
	template <
		class Function,
		typename Decayed = std::decay_t<Function>,
		typename = std::enable_if_t<
			!std::is_same_v<Decayed, TFunction> &&
			!std::is_same_v<Decayed, std::nullptr_t> &&
			std::is_copy_constructible_v<Decayed> &&
			IsCallableCompatible<Decayed>()
		>
	>
	TFunction(Function&& InFunction)
	{
		using CallableType = TCallable<Decayed>;

		if constexpr (CallableType::CanInline())
		{
			MyCallable = new (&MyStorage) CallableType(std::forward<Function>(InFunction));
			MybInline = true;
		}
		else
		{
			MyCallable = new CallableType(std::forward<Function>(InFunction));
			MybInline = false;
		}
	}

	TFunction& operator=(const TFunction& InOther)
	{
		if (InOther.MyCallable)
		{
			MyCallable = InOther.MyCallable->Clone(&MyStorage, MybInline);
		}
		return *this;
	}
	TFunction(TFunction&& InOther) noexcept
	{
		MoveFrom(InOther);
	}
	TFunction& operator=(TFunction&& InOther) noexcept
	{
		if (this == &InOther) { return *this; }
		Reset();
		MoveFrom(InOther);
		return *this;
	}

	TFunction& operator=(std::nullptr_t) noexcept
	{
		Reset();
		return *this;
	}

	bool IsValid() const { return MyCallable != nullptr; }
	ReturnT operator()(Args&&... InArgs) const
	{
		ENSURE(MyCallable, "Bad function call");
		return MyCallable->Invoke(std::forward<Args>(InArgs)...);
	}

	void Reset() noexcept
	{
		if (MyCallable)
		{
			MyCallable->Destroy(MybInline);
			MyCallable = nullptr;
			MybInline = false;
		}
	}

private:
	void MoveFrom(TFunction& InOther) noexcept
	{
		if (!InOther.MyCallable) { return; }
		if (InOther.MybInline)
		{
			MyCallable = InOther.MyCallable->Move(&MyStorage, MybInline);
			InOther.Reset();
		}
		else
		{
			MyCallable = InOther.MyCallable;
			MybInline = false;
			InOther.MyCallable = nullptr;
			InOther.MybInline = false;
		}
	}

PRIVATE:
	InlineStorage MyStorage;
	ICallable* MyCallable { nullptr };
	bool MybInline { false };
};
