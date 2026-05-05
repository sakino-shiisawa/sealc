#pragma once

#include <atomic>
#include <utility>


class FDefaultDelete
{
public:
	template <class T>
	void operator()(T* InPtr) const noexcept { delete InPtr; }
};
class FDefaultDeleteArray
{
public:
	template <class T>
	void operator()(T* InPtr) const noexcept { delete[] InPtr; }
};

class FRefCounterBase
{
public:
	FRefCounterBase() noexcept = default;
	virtual ~FRefCounterBase() = default;

	void AddSharedRef() noexcept
	{
		SharedCount.fetch_add(1, std::memory_order_relaxed);
	}
	void ReleaseSharedRef() noexcept
	{
		if (SharedCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			DestroyObject();
			ReleaseWeakRef();
		}
	}
	void AddWeakRef() noexcept
	{
		WeakCount.fetch_add(1, std::memory_order_relaxed);
	}
	void ReleaseWeakRef() noexcept
	{
		if (WeakCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			delete this;
		}
	}
	bool TryAddSharedRef() noexcept
	{
		unsigned long Count = SharedCount.load(std::memory_order_acquire);
		while (Count != 0)
		{
			if (SharedCount.compare_exchange_weak(Count, Count + 1, std::memory_order_acq_rel, std::memory_order_acquire))
			{
				return true;
			}
		}
		return false;
	}

	[[nodiscard]]
	unsigned long GetSharedCount() const noexcept
	{
		return SharedCount.load(std::memory_order_acquire);
	}

private:
	virtual void DestroyObject() noexcept = 0;

private:
	std::atomic<unsigned long> SharedCount { 1 };
	std::atomic<unsigned long> WeakCount { 1 };
};

template <class Type, class TDeleter>
class FRefCountController final : public FRefCounterBase
{
public:
	FRefCountController(Type* InPtr, TDeleter InDeleter)
		: MyPtr(InPtr)
		, Deleter(std::move(InDeleter))
	{}

private:
	void DestroyObject() noexcept override
	{
		if (MyPtr)
		{
			Deleter(MyPtr);
			MyPtr = nullptr;
		}
	}

private:
	Type* MyPtr { nullptr };
	TDeleter Deleter;
};

template <class Type>
class TSharedPtr;

template <class Type>
class TWeakPtr;

template <class Type>
class TSharedPtr
{
public:
	typedef Type ElementType;

	constexpr TSharedPtr() noexcept = default;
	constexpr TSharedPtr(std::nullptr_t) noexcept {}
	explicit TSharedPtr(Type* InPtr)
		: TSharedPtr(InPtr, FDefaultDelete {})
	{}
	template <class TDeleter>
	TSharedPtr(Type* InPtr, TDeleter InDeleter)
		: MyPtr(InPtr)
	{
		if (InPtr)
		{
			MyControl = new FRefCountController<Type, TDeleter>(InPtr, std::move(InDeleter));
		}
	}
	TSharedPtr(const TSharedPtr& InOther) noexcept
		: MyPtr(InOther.MyPtr)
		, MyControl(InOther.MyControl)
	{
		AddSharedRef();
	}
	TSharedPtr(TSharedPtr&& InOther) noexcept
		: MyPtr(InOther.MyPtr)
		, MyControl(InOther.MyControl)
	{
		MyPtr = nullptr;
		MyControl = nullptr;
	}
	~TSharedPtr() { Reset(); }

	TSharedPtr& operator=(const TSharedPtr& InOther) noexcept
	{
		if (this != &InOther)
		{
			TSharedPtr Temp(InOther);
			Swap(Temp);
		}
		return *this;
	}
	TSharedPtr& operator=(TSharedPtr&& InOther) noexcept
	{
		if (this != &InOther)
		{
			Reset();
			MyPtr = InOther.MyPtr;
			MyControl = InOther.MyControl;
			InOther.MyPtr = nullptr;
			InOther.MyControl = nullptr;
		}
		return *this;
	}

	[[nodiscard]]
	const Type* Get() const noexcept { return MyPtr; }
	[[nodiscard]]
	Type* Get() noexcept { return MyPtr; }

	[[nodiscard]]
	const Type& operator->() const noexcept
	{
		ASSERT(MyPtr, "invalid address");
		return *MyPtr;
	}
	[[nodiscard]]
	Type& operator->() noexcept
	{
		ASSERT(MyPtr, "invalid address");
		return *MyPtr;
	}
	[[nodiscard]]
	const Type& operator*() const noexcept
	{
		ASSERT(MyPtr, "invalid address");
		return MyPtr;
	}
	[[nodiscard]]
	Type& operator*() noexcept
	{
		ASSERT(MyPtr, "invalid address");
		return MyPtr;
	}
	[[nodiscard]]
	explicit operator bool() const noexcept { return MyPtr != nullptr; }

	[[nodiscard]]
	unsigned long UseCount() const noexcept { return MyControl ? MyControl->GetSharedCount() : 0; }
	[[nodiscard]]
	bool IsUnique() const noexcept { return UseCount() == 1; }

	void Reset()
	{
		if (MyControl)
		{
			MyControl->ReleaseSharedRef();
			MyPtr = nullptr;
			MyControl = nullptr;
		}
	}

	void Swap(TSharedPtr& InRhs) noexcept
	{
		std::swap(MyPtr, InRhs.MyPtr);
		std::swap(MyControl, InRhs.MyControl);
	}

private:
	TSharedPtr(Type* InPtr, FRefCounterBase* InControl, bool InIsAddRef) noexcept
		: MyPtr(InPtr)
		, MyControl(InControl)
	{
		if (InIsAddRef) { AddSharedRef(); }
	}
	void AddSharedRef() noexcept { if (MyControl) { MyControl->AddSharedRef(); } }

private:
	Type* MyPtr { nullptr };
	FRefCounterBase* MyControl { nullptr };

	template <class UType>
	friend class TWeakPtr;

	template <class UType>
	friend class TSharedPtr;
};

template <class Type>
class TSharedPtr<Type[]>
{
public:
	typedef Type ElementType;

	constexpr TSharedPtr() noexcept = default;
	constexpr TSharedPtr(std::nullptr_t) noexcept {}
	explicit TSharedPtr(Type* InPtr)
		: TSharedPtr(InPtr, FDefaultDeleteArray {})
	{}
	template <class TDeleter>
	TSharedPtr(Type* InPtr, TDeleter InDeleter)
		: MyPtr(InPtr)
	{
		if (InPtr)
		{
			MyControl = new FRefCountController<Type, TDeleter>(InPtr, std::move(InDeleter));
		}
	}
	TSharedPtr(const TSharedPtr& InOther) noexcept
		: MyPtr(InOther.MyPtr)
		, MyControl(InOther.MyControl)
	{
		AddSharedRef();
	}
	TSharedPtr(TSharedPtr&& InOther) noexcept
		: MyPtr(InOther.MyPtr)
		, MyControl(InOther.MyControl)
	{
		MyPtr = nullptr;
		MyControl = nullptr;
	}
	~TSharedPtr() { Reset(); }

	TSharedPtr& operator=(const TSharedPtr& InOther) noexcept
	{
		if (this != &InOther)
		{
			TSharedPtr Temp(InOther);
			Swap(Temp);
		}
		return *this;
	}
	TSharedPtr& operator=(TSharedPtr&& InOther) noexcept
	{
		if (this != &InOther)
		{
			Reset();
			MyPtr = InOther.MyPtr;
			MyControl = InOther.MyControl;
			InOther.MyPtr = nullptr;
			InOther.MyControl = nullptr;
		}
		return *this;
	}
	[[nodiscard]]
	const Type* Get() const noexcept { return MyPtr; }
	[[nodiscard]]
	Type* Get() noexcept { return MyPtr; }
	[[nodiscard]]
	const Type& operator[](size_t InIndex) const noexcept
	{
		ASSERT(MyPtr, "invalid address");
		return MyPtr[InIndex];
	}
	[[nodiscard]]
	Type& operator[](size_t InIndex) noexcept
	{
		ASSERT(MyPtr, "invalid address");
		return MyPtr[InIndex];
	}
	[[nodiscard]]
	explicit operator bool() const noexcept { return MyPtr != nullptr; }

	[[nodiscard]]
	unsigned long UseCount() const noexcept { return MyControl ? MyControl->GetSharedCount() : 0; }
	[[nodiscard]]
	bool IsUnique() const noexcept { return UseCount() == 1; }

	void Reset()
	{
		if (MyControl)
		{
			MyControl->ReleaseSharedRef();
			MyPtr = nullptr;
			MyControl = nullptr;
		}
	}
	void Swap(TSharedPtr& InRhs) noexcept
	{
		std::swap(MyPtr, InRhs.MyPtr);
		std::swap(MyControl, InRhs.MyControl);
	}

private:
	TSharedPtr(Type* InPtr, FRefCounterBase* InControl, bool InIsAddRef) noexcept
		: MyPtr(InPtr)
		, MyControl(InControl)
	{
		if (InIsAddRef) { AddSharedRef(); }
	}
	void AddSharedRef() noexcept { if (MyControl) { MyControl->AddSharedRef(); } }

private:
	Type* MyPtr { nullptr };
	FRefCounterBase* MyControl { nullptr };

	template <class UType>
	friend class TWeakPtr;
};

template <class Type>
class TWeakPtr
{
public:
	constexpr TWeakPtr() noexcept = default;

	TWeakPtr(const TSharedPtr<Type>& InPtr) noexcept
		: MyPtr(InPtr.MyPtr)
		, MyControl(InPtr.MyControl)
	{
		AddWeakRef();
	}
	TWeakPtr(const TWeakPtr& InOther) noexcept
		: MyPtr(InOther.MyPtr)
		, MyControl(InOther.MyControl)
	{
		AddWeakRef();
	}
	TWeakPtr(TWeakPtr&& InOther) noexcept
		: MyPtr(InOther.MyPtr)
		, MyControl(InOther.MyControl)
	{
		InOther.MyPtr = nullptr;
		InOther.MyControl = nullptr;
	}
	~TWeakPtr() { Reset(); }

	TWeakPtr& operator=(const TWeakPtr& InOther) noexcept
	{
		if (this != &InOther)
		{
			TWeakPtr Temp(InOther);
			Swap(Temp);
		}
		return *this;
	}
	TWeakPtr& operator=(TWeakPtr&& InOther) noexcept
	{
		if (this != &InOther)
		{
			Reset();
			MyPtr = InOther.MyPtr;
			MyControl = InOther.MyControl;
			InOther.MyPtr = nullptr;
			InOther.MyControl = nullptr;
		}
		return *this;
	}

	[[nodiscard]]
	TSharedPtr<Type> Pin() const noexcept
	{
		if (MyControl && MyControl->TryAddSharedRef())
		{
			return TSharedPtr<Type>(MyPtr, MyControl, false);
		}
		return TSharedPtr<Type>();
	}

	[[nodiscard]]
	bool IsExpired() const noexcept { return !MyControl || MyControl->GetSharedCount() == 0; }
	void Reset() noexcept
	{
		if (MyControl)
		{
			MyControl->ReleaseWeakRef();
			MyPtr = nullptr;
			MyControl = nullptr;
		}
	}
	void Swap(TWeakPtr& InOther) noexcept
	{
		std::swap(MyPtr, InOther.MyPtr);
		std::swap(MyControl, InOther.MyControl);
	}

private:
	void AddWeakRef() noexcept
	{
		if (MyControl) { MyControl->AddWeakRef(); }
	}

private:
	Type* MyPtr { nullptr };
	FRefCounterBase* MyControl { nullptr };
};

template <class Type, class... Args>
[[nodiscard]]
inline TSharedPtr<Type> MakeShared(Args&&... InArgs)
{
	return TSharedPtr<Type>(new Type(std::forward<Args...>(InArgs)...));
}
template <class Type>
[[nodiscard]]
inline TSharedPtr<Type[]> MakeSharedArray(size_t InCount)
{
	return TSharedPtr<Type[]>(new Type[InCount]{});
}
