#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

class Game;

class InputDevice {
private:
	Game* game;
	//keys;

	void OnKeyDown();
	void OnMouseRemoved();

public:
	//MouseOffset;
	//MousePosition;
	//MouseWheelDown;
	;

	void AddPressedKey();
	void IsKeyDown();
	void RemovePressedKey();

};

#endif
