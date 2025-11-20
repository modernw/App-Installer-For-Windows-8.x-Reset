#pragma once
#include <functional>
typedef struct raii
{
	std::function <void ()> endtask = nullptr;
	raii (std::function <void ()> pFunc = nullptr): endtask (pFunc) {}
	~raii () { if (endtask) endtask (); }
	raii (const raii &) = delete;
	raii (raii &&) = delete;
} destruct;