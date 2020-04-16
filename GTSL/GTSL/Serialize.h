#pragma once

#include "Stream.h"

namespace GTSL
{
	template <typename T>
	void operator<<(OutStream& outStream, const Vector<T>& vector)
	{
		outStream.Write(vector.GetLength());

		for (auto& e : vector) { outStream << e; }
	}

	template <typename T>
	void operator>>(InStream& inStream, Vector<T>& vector)
	{
		typename Vector<T>::length_type length = 0;

		inStream.Read(&length);

		vector.Resize(length);

		for (auto& e : vector) { inStream >> e; }
	}
}