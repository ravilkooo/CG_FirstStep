#ifndef DISPLAY_WIN_H
#define DISPLAY_WIN_H

#include <windows.h>
#include <iostream>

class DisplayWindow
{
public:
	int screenWidth = 800;
	int screenHeight = 800;
	HWND hWnd;

	DisplayWindow();
	DisplayWindow(LPCWSTR applicationName, HINSTANCE hInstance, int screenWidth, int screenHeight);

	void Initialize(LPCWSTR applicationName, HINSTANCE hInstance, int screenWidth, int screenHeight);

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
};

#endif // DISPLAY_WIN_H