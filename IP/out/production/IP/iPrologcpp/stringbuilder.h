#pragma once

//----------------------------------------------------------------------------------------
//	Copyright © 2007 - 2021 Tangible Software Solutions, Inc.
//	This class can be used by anyone provided that the copyright notice remains intact.
//
//	This class is used to replace the Java StringBuilder in C++.
//----------------------------------------------------------------------------------------
#include <string>
#include <sstream>

class StringBuilder
{
private:
	std::wstring privateString;

public:
	StringBuilder()
	{
	}

	StringBuilder(const std::wstring &initialString)
	{
		privateString = initialString;
	}

	StringBuilder(std::size_t capacity)
	{
		ensureCapacity(capacity);
	}

	wchar_t charAt(std::size_t index)
	{
		return privateString[index];
	}

	StringBuilder *append(const std::wstring &toAppend)
	{
		privateString += toAppend;
		return this;
	}

	template<typename T>
	StringBuilder *append(const T &toAppend)
	{
		privateString += toString(toAppend);
		return this;
	}

	StringBuilder *insert(std::size_t position, const std::wstring &toInsert)
	{
		privateString.insert(position, toInsert);
		return this;
	}

	template<typename T>
	StringBuilder *insert(std::size_t position, const T &toInsert)
	{
		privateString.insert(position, toString(toInsert));
		return this;
	}

	std::wstring toString()
	{
		return privateString;
	}

	std::size_t length()
	{
		return privateString.length();
	}

	void setLength(std::size_t newLength)
	{
		privateString.resize(newLength);
	}

	std::size_t capacity()
	{
		return privateString.capacity();
	}

	void ensureCapacity(std::size_t minimumCapacity)
	{
		privateString.reserve(minimumCapacity);
	}

	StringBuilder *remove(std::size_t start, std::size_t end)
	{
		privateString.erase(start, end - start);
		return this;
	}

	StringBuilder *replace(std::size_t start, std::size_t end, const std::wstring &newString)
	{
		privateString.replace(start, end - start, newString);
		return this;
	}

private:
	template<typename T>
	static std::wstring toString(const T &subject)
	{
		std::wostringstream ss;
		ss << subject;
		return ss.str();
	}
};
