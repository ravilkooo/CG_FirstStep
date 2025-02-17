#ifndef GAME_COMP_H
#define GAME_COMP_H

class Game;

class GameComponent {
public:
	Game* game;
	
	GameComponent(Game* game);

	void Initialize();

	void Draw();

	void Reload();

	void Update();

	void DestroyResources();
};

#endif