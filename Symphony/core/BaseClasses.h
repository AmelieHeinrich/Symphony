#pragma once

namespace symphony {

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