#pragma once
#include <VFrame/VState.h>
#include <VFrame/VText.h>
#include <VFrame/VSprite.h>
#include <VFrame/VTimer.h>
#include <VFrame/VTilemap.h>
#include <VFrame/VRenderGroup.h>

class TitleState : public VState
{
public:
	typedef VState VSUPERCLASS;

	TitleState() = default;

	static int MusicVolume;
	float t = 1.0f;
	float target = 0.0f;

	virtual void Initialise() override;
	virtual void HandleEvents(const sf::Event& event);
	virtual void Update(float dt) override;

	void UpdateEntries();
	void SelectEntries();
	
	enum
	{
		TITLE_START,
		TITLE_MUSICVOLUME,
		TITLE_SFXVOLUME,
		TITLE_EXIT,
		NUM_TITLE_OPTIONS
	};

	VText* Options[NUM_TITLE_OPTIONS];
	int CurrentOption = 0;

	VRenderGroup* TitleRender;
	VSprite* Title;
	VTilemap* Tilemap[2];
	std::vector<sf::Vector2u> points = std::vector<sf::Vector2u>(16);

	void UpdateBackgroundTilemap(unsigned index);
};