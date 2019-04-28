#pragma once
#include <VFrame/VState.h>
#include <VFrame/VText.h>
#include <VFrame/VSprite.h>
#include <VFrame/VEmitter.h>

class GoalState : public VState
{
	VText* text;
	VSprite* player;
	VEmitter* playerStars;

public:
	typedef VState VSUPERCLASS;
	GoalState() = default;

	virtual void Initialise() override;
	virtual void Update(float dt) override;
};

