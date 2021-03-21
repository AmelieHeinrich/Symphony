#pragma once

namespace symphony {

	// Base classes so it's easier to make singletons or other specific classes

	class BaseNonMovable
	{
	public:
		BaseNonMovable(BaseNonMovable&&) = delete;
		BaseNonMovable& operator=(BaseNonMovable&&) = delete;

	protected:
		BaseNonMovable() = default;
	};

	class BaseNonCopyable
	{
	public:
		BaseNonCopyable() = default;
		BaseNonCopyable(const BaseNonCopyable&) = delete;
		BaseNonCopyable& operator=(const BaseNonCopyable&) = delete;
	};

	class BaseSingleton : public BaseNonCopyable, public BaseNonMovable
	{

	};
}