#pragma once

#include "Application.h"
#include "StaticString.hpp"

namespace GTSL
{
	class FileQuery
	{
	public:
		using handle_type = void*;
		
		FileQuery(const StaticString<Application::MAX_PATH_LENGTH>& query) : query(query)
		{
		}
		
		bool DoQuery();

		operator bool() { return DoQuery(); }

		auto GetFileNameWithExtension() const { return fileNameWithExtension; }
	
	private:
		StaticString<Application::MAX_PATH_LENGTH> query;
		StaticString<Application::MAX_PATH_LENGTH> fileNameWithExtension;
		handle_type handle = nullptr;
	};
}
