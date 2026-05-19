#pragma once

class MyWindow {
public:
	MyWindow();
	~MyWindow();

	void draw();
private:
	struct Impl;
	Impl* d = nullptr;
};