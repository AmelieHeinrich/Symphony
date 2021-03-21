#pragma once

#include <iostream>
#include <stdlib.h>

namespace symphony {
	// Assert class I don't use but if I ever need it I have it ;)

	class Assert
	{
	public:
		static void IsTrue(bool value, const char* message)
		{
			if (value)
			{
				std::cout << "ASSERTION FAILED! :" << message << std::endl;
				exit(-1);
			}
		}

		static void IsFalse(bool value, const char* message)
		{
			if (!value)
			{
				std::cout << "ASSERTION FAILED! :" << message << std::endl;
				exit(-1);
			}
		}

		template<typename T>
		static void IsEqual(T one, T two, const char* message)
		{
			if (one == two)
			{
				std::cout << "ASSERTION FAILED! :" << message << std::endl;
				exit(-1);
			}
		}

		template<typename T>
		static void IsNotEqual(T one, T two, const char* message)
		{
			if (one != two)
			{
				std::cout << "ASSERTION FAILED! :" << message << std::endl;
				exit(-1);
			}
		}
	};
}