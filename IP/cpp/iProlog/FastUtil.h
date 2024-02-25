#pragma once

namespace iProlog {
	using namespace std;

	class FastUtil {
	public:
		static int phiMix(int x);
		static long arraySize(int expected, float f);
		static size_t nextPowerOfTwo(size_t x);
	};
}
