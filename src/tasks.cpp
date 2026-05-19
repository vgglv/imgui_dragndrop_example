#include "tasks.hpp"
#include <functional>
#include <vector>

static std::vector<std::function<void()>> myTasks;

void TaskController::addFunc(const std::function<void()>& f) {
	myTasks.emplace_back(f);
}

void TaskController::update() {
	if (myTasks.empty()) {
		return;
	}

	for (const auto& task : myTasks) {
		task();
	}
	myTasks.clear();
}