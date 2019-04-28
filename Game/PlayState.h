#pragma once
#include <VFrame/VState.h>
#include <VFrame/VTilemap.h>
#include <VFrame/VSprite.h>
#include <VFrame/VText.h>
#include <VFrame/VTimer.h>
#include <VFrame/VEmitter.h>

struct GlobalPlay
{
	int Health;
	int Score;
	int Tunnel;
	char PathWidth;
	char NumPoints;
	float Time;
	float TotalTime;
	float RotationSpeed;
	float OrbSpeed;
	float MaxSpeed;
};

class PlayState : public VState
{
	VTilemap* tilemap;
	VSprite* player;
	VGroup* orbs;

	VEmitter* playerStars;

	VText* timeText;
	VText* healthText;
	VText* scoreText;

	VText* gameOverText;

	int curHealth;
	VTimer* uiTimer;

	std::function<void(VObject*, VObject*)> hurtResponse;
	std::function<void(VObject*, VObject*)> orbResponse;

public:
	typedef VState VSUPERCLASS;
	PlayState() = default;

	static GlobalPlay Global;

	virtual void Initialise() override;
	virtual void Update(float dt) override;

	void GenerateMap(std::vector<sf::Vector2u>& points, int pathWidth);
};

extern sf::String GetTimeString(float t);
extern float CosineInterpolate(float y1, float y2, float mu);