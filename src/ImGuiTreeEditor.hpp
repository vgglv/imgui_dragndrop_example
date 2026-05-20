#pragma once

class ImGuiTreeEditor {
public:
	ImGuiTreeEditor();
	~ImGuiTreeEditor();

	void draw();
private:
	struct Impl;
	Impl* d = nullptr;
};