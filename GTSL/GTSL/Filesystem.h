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

		struct QueryResult
		{
			StaticString<Application::MAX_PATH_LENGTH> FilePath;
			
		};
		
		bool StartQuery(QueryResult& query);
		bool NextQuery(QueryResult& query) const;
		void EndQuery() const;
		
	private:
		StaticString<Application::MAX_PATH_LENGTH> query;
		handle_type handle = nullptr;
	};
	
	template<typename FT>
	void ForEach(FileQuery& fileQuery, FT&& function)
	{
		FileQuery::QueryResult query_result;
		
		if(fileQuery.StartQuery(query_result))
		{
			function(query_result);
			while(fileQuery.NextQuery(query_result)) { function(query_result); }
		}

		fileQuery.EndQuery();
	}
}
