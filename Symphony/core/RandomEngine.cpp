#include "RandomEngine.h"

namespace symphony {
	RandomEngine& RandomEngine::Get()
	{
		static RandomEngine r;
		return r;
	}

	RandomEngine::RandomEngine()
	{
		m_RandomEngine.seed(static_cast<unsigned>(std::time(nullptr)));
	}
}