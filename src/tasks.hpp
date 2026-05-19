#pragma once
#include <functional>

class TaskController {
public:
	static void addFunc(const std::function<void()>& f);
	static void update();
};