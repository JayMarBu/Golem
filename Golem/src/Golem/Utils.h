#pragma once
#include "golpch.h"
#define EPSILON(type) std::numeric_limits<type>::epsilon()

namespace golem
{
	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void HashCombine(std::size_t& seed, const T& v, const Rest&... rest)
	{
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(HashCombine(seed, rest), ...);
	};

	template <typename T>
	static T Max(std::initializer_list<T> a_args)
	{
		T maxVal = std::numeric_limits<T>::lowest();
		for (auto i : a_args)
			if (i > maxVal)
				maxVal = i;
		return maxVal;
	}

	template <typename T>
	static T Min(std::initializer_list<T> a_args)
	{
		T minVal = std::numeric_limits<T>::max();
		for (auto i : a_args)
			if (i < minVal)
				minVal = i;
		return minVal;
	}

	template <typename T>
	static T Abs(T val)
	{
		return (val > 0) ? val : -val;
	}

	template <typename T>
	static T Sign(T val)
	{
		return (T(0) < val) - (val < T(0));
	}

	template<typename T>
	static T Lerp(T a, T b, float t) { return {}; }

	template<>
	static float Lerp<float>(float a, float b, float t)
	{
		return(1 - t) * a + t * b;
	}

	template<>
	static double Lerp<double>(double a, double b, float t)
	{
		return(1 - (double)t) * a + t * b;
	}

	template<typename T>
	static T InvLerp(T minVal, T maxVal, T val) { return {}; }

	template<>
	static float InvLerp<float>(float minVal, float maxVal, float val)
	{
		return (val - minVal) / (maxVal - minVal);
	}

	template<>
	static double InvLerp<double>(double minVal, double maxVal, double val)
	{
		return (val - minVal) / (maxVal - minVal);
	}

	template <typename T>
	static bool AlmostEqual(T a, T b)
	{
		return  Abs<T>(a - b) <= EPSILON(T) * Max({ (T)1, Abs(a), Abs(b) });
	}

	template <typename T>
	static bool AlmostEqual(T a, T b, T epsilon)
	{
		return  Abs<T>(a - b) <= epsilon * Max({ (T)1, Abs(a), Abs(b) });
	}

	template <typename T>
	static bool VecIsZero(const T& vec)
	{
		return (glm::dot(vec,vec) > EPSILON(double));
	}
}