#pragma once
#include <VFrame/VState.h>
#include <VFrame/VSprite.h>
#include <VFrame/VText.h>
#include <VFrame/VTimer.h>
#include <VFrame/VEmitter.h>

class LevelState : public VState
{
	VSprite* player;
	VEmitter* playerStars;

	VText* timeText;
	VText* healthText;
	VText* scoreText;

	int curHealth;
	VTimer* uiTimer;

	VSprite* option[3];
	VEmitter* optionStars[3];
	VText* optionText[3];
	int optionType[3];

public:
	typedef VState VSUPERCLASS;
	LevelState() = default;

	virtual void Initialise() override;
	virtual void Update(float dt) override;
};