#pragma once

#include "BaseClasses.h"
#include "DLLExport.h"

#include <ctime>
#include <random>

namespace symphony {
	SymphonyClass RandomEngine : public BaseSingleton {
	public:
		static RandomEngine& Get();

		template<typename T> T IntInRange(T low, T high)
		{
			static_assert(std::is_integral<T>::value, "Value provided is not an intergral type!");
			std::uniform_int_distribution<T> dist(low, high);
			return dist(m_RandomEngine);
		}

	private:
		RandomEngine();
		std::mt19937 m_RandomEngine;
	};
}
