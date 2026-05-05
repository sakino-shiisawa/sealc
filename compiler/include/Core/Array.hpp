#pragma once

#include "Debug/Debug.hpp"
#include "Memory/SmartPointer.hpp"
#include <cstddef>


template <class Type>
class TArray
{
public:
	class TIterator
	{
	public:
		TIterator() = default;
		TIterator(const TIterator&) = default;
		TIterator(TIterator&&) = default;
		~TIterator() = default;

		TIterator& operator=(const TIterator&) = default;
		TIterator& operator=(TIterator&&) = default;

		TIterator& operator++()
		{
			++MyCurrent;
			ENSURE(MyCurrent < MyEnd, "Out of Range");
			return *this;
		}
		TIterator operator++(int)
		{
			TIterator Temp(*this);
			++MyCurrent;
			ENSURE(MyCurrent < MyEnd, "Out of Range");
			return Temp;
		}
		TIterator& operator--()
		{
			--MyCurrent;
			ENSURE(MyCurrent >= MyHead, "Out of Range");
			return *this;
		}
		TIterator operator--(int)
		{
			TIterator Temp(*this);
			--MyCurrent;
			ENSURE(MyCurrent >= MyHead, "Out of Range");
			return Temp;
		}

		friend TIterator operator+(const TIterator& InLhs, std::ptrdiff_t InRhs) noexcept
		{
			Type* NewCurrent = InLhs.MyCurrent + InRhs;
			ENSURE(InLhs.MyHead <= NewCurrent && NewCurrent < InLhs.MyEnd, "Out of Range");
			return TIterator(InLhs.MyHead, NewCurrent, InLhs.MyEnd);
		}
		friend TIterator operator+(std::ptrdiff_t InLhs, const TIterator& InRhs) noexcept
		{
			Type* NewCurrent = InRhs.MyCurrent + InLhs;
			ENSURE(InRhs.MyHead <= NewCurrent && NewCurrent < InRhs.MyEnd, "Out of Range");
			return TIterator(InRhs.MyHead, NewCurrent, InRhs.MyEnd);
		}
		friend TIterator operator-(const TIterator& InLhs, std::ptrdiff_t InRhs) noexcept
		{
			Type* NewCurrent = InLhs.MyCurrent - InRhs;
			ENSURE(InLhs.MyHead <= NewCurrent && NewCurrent < InLhs.MyEnd, "Out of Range");
			return TIterator(InLhs.MyHead, NewCurrent, InLhs.MyEnd);
		}
		friend TIterator operator-(std::ptrdiff_t InLhs, const TIterator& InRhs) noexcept
		{
			Type* NewCurrent = InRhs.MyCurrent - InLhs;
			ENSURE(InRhs.MyHead <= NewCurrent && NewCurrent < InRhs.MyEnd, "Out of Range");
			return TIterator(InRhs.MyHead, NewCurrent, InRhs.MyEnd);
		}

		friend std::ptrdiff_t operator-(const TIterator& InLhs, const TIterator& InRhs) noexcept
		{
			return InLhs.MyCurrent - InRhs.MyCurrent;
		}

		bool operator==(const TIterator& InRhs) const noexcept
		{
			return MyCurrent == InRhs.MyCurrent;
		}
		bool operator!=(const TIterator& InRhs) const noexcept
		{
			return !(*this == InRhs);
		}

		Type& operator*() noexcept { return *MyCurrent; }
		const Type& operator*() const noexcept { return *MyCurrent; }
		Type* operator->() noexcept { return MyCurrent; }
		const Type* operator->() const noexcept { return MyCurrent; }

	private:
		TIterator(Type* InHead, Type* InCurrent, Type* InEnd)
			: MyHead(InHead)
			, MyEnd(InEnd)
			, MyCurrent(InCurrent)
		{}

	PRIVATE:
		Type* MyHead;
		Type* MyEnd;
		Type* MyCurrent;
	private:
		friend TArray;
	};

public:
	TArray() = default;
	~TArray() = default;

	TArray(std::initializer_list<Type> InList)
	{
		MyData = MakeSharedArray<Type>(InList.size());
		size_t Idx = 0;
		for (auto& Item : InList)
		{
			MyData[Idx++] = Item;
		}
		MyCount = InList.size();
		MyCapacity = InList.size();
	}

	TArray(const TArray&) = default;
	TArray(TArray&&) = default;

	TArray& operator=(const TArray&) = default;
	TArray& operator=(TArray&&) = default;

	size_t Size() const noexcept { return MyCount; }
	size_t Capacity() const noexcept { return MyCapacity; }
	bool IsEmpty() const noexcept { return Size() == 0; }

	void ShrinkToFit() noexcept
	{
		if (MyCount == MyCapacity) { return; }
		TSharedPtr<Type[]> Temp = MakeSharedArray<Type>(MyCount);
		for (size_t Idx = 0; Idx < MyCount; ++Idx)
		{
			Temp[Idx] = std::move(MyData[Idx]);
		}
		MyData = std::move(Temp);
		MyCapacity = MyCount;
	}

	void Reserve(size_t InNewCapacity) noexcept
	{
		if (MyCapacity >= InNewCapacity)
		{
			MyCapacity = InNewCapacity;
			return;
		}
		TSharedPtr<Type[]> Temp = MakeSharedArray<Type>(InNewCapacity);
		for (size_t Idx = 0; Idx < MyCount; ++Idx)
		{
			Temp[Idx] = std::move(MyData[Idx]);
		}
		MyData = std::move(Temp);
		MyCapacity = InNewCapacity;
	}
	void Resize(size_t InNewSize) noexcept
	{
		if (MyCapacity < InNewSize)
		{
			Reserve(InNewSize);
		}
		else if (MyCount > InNewSize)
		{
			for (size_t Idx = InNewSize; Idx < MyCount; ++Idx)
			{
				MyData[Idx].~Type();
			}
		}
		MyCount = InNewSize;
	}

	void PushBack(const Type& InValue) noexcept
	{
		if (!MyCount || MyCount == MyCapacity) { Reserve(MyCapacity ? MyCapacity * 2 : 1); }
		MyData[MyCount++] = InValue;
	}
	void PushBack(Type&& InValue) noexcept
	{
		if (!MyCount || MyCount == MyCapacity) { Reserve(MyCapacity ? MyCapacity * 2 : 1); }
		MyData[MyCount++] = std::move(InValue);
	}
	template <class... Args>
	void EmplaceBack(Args&&... InArgs) noexcept
	{
		if (!MyCount || MyCount == MyCapacity) { Reserve(MyCapacity ? MyCapacity * 2 : 1); }
		new (&MyData[MyCount++]) Type(std::forward<Args...>(InArgs)...);
	}
	void PopBack() noexcept
	{
		MyData[--MyCount].~Type();
	}

	void Clear() noexcept
	{
		for (size_t Idx = 0; Idx < MyCapacity; ++Idx)
		{
			MyData[Idx].~Type();
		}
		MyCount = 0;
	}

	Type* Data() noexcept { return MyData.Get(); }
	const Type* Data() const noexcept { return MyData.Get(); }

	const Type& operator[](size_t InIndex) const noexcept
	{
		ASSERT(MyCount > InIndex, "Out of Memory");
		return MyData[InIndex];
	}
	Type& operator[](size_t InIndex) noexcept
	{
		ASSERT(MyCount > InIndex, "Out of Memory");
		return MyData[InIndex];
	}

	TIterator begin() noexcept { return TIterator(MyData.Get(), MyData.Get(), MyData.Get() + MyCount); }
	TIterator end() noexcept { return TIterator(MyData.Get(), MyData.Get() + MyCount, MyData.Get() + MyCount); }
	TIterator begin() const noexcept { return TIterator(MyData.Get(), MyData.Get(), MyData.Get() + MyCount); }
	TIterator end() const noexcept { return TIterator(MyData.Get(), MyData.Get() + MyCount, MyData.Get() + MyCount); }

PRIVATE:
	TSharedPtr<Type[]> MyData;
	size_t MyCount { 0 };
	size_t MyCapacity { 0 };
};
