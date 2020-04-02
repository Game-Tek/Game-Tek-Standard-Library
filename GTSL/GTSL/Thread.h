#pragma once

#include "Core.h"

#include <thread>

class Thread
{
	std::thread thread;

public:
	template <typename F, typename... P>
	explicit Thread(F& lambda_, P&&... params_) : thread(lambda_, params_...)
	{
	}

	static uint32 ThisTreadID() { return std::hash<std::thread::id>{}(std::this_thread::get_id()); }
	//static Thread ThisThread() { return Thread(std::this_thread()); }
	
	void Join() { thread.join(); }
	void Detach() { thread.detach(); }

	[[nodiscard]] uint32 GetId() const { return std::hash<std::thread::id>{}(thread.get_id()); }
	
	[[nodiscard]] bool CanBeJoined() const { return thread.joinable(); }
};