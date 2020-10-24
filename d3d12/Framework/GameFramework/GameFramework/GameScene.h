#pragma once

#include "Scene.h"

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();

public:
	virtual void Init() override;
};

