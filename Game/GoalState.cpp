#include "GoalState.h"
#include "PlayState.h"

#include "VFrame/VColour.h"
#include "VFrame/VGlobal.h"
#include "VFrame/VInterpolate.h"
#include "VFrame/VTiledSprite.h"

#include "TitleState.h"

void GoalState::Initialise()
{
	VSUPERCLASS::Initialise();

	PlayState::Global.Tunnel = 0;
	PlayState::Global.Health = 100;
	PlayState::Global.Score = 0;
	PlayState::Global.PathWidth = 30;
	PlayState::Global.NumPoints = 3;
	PlayState::Global.TotalTime = 0.0f;
	PlayState::Global.RotationSpeed = 180.0f;
	PlayState::Global.OrbSpeed = 3.0f;
	PlayState::Global.MaxSpeed = 200.0f;

	player = new VSprite();
	player->LoadGraphic("Assets/Player.png", true, 16, 16);
	player->MaxVelocity = sf::Vector2f(PlayState::Global.MaxSpeed, PlayState::Global.MaxSpeed);
	player->Velocity.x = player->MaxVelocity.x;
	player->SetPositionAtCentre(-32.0f, 3.0f * VGlobal::p()->Height / 4.0f);

	playerStars = new VEmitter(0.0f, 0.0f);
	playerStars->LoadParticlesFromFile(32, "Assets/Player.png", false, 16, 16, sf::IntRect(0, 16, 16, 16));
	playerStars->AngleVelocityRange = VRange<float>(180.0f, 180.0f);
	playerStars->AngleDragRange = 36.0f;
	playerStars->AngleRange = VRange<float>(0.0f, 360.0f);
	playerStars->Circular = true;
	playerStars->AlphaRange = VRangeBounds<float>(1.0f, 1.0f, 0.0f, 0.0f);
	playerStars->SpeedRange = VRange<float>(player->MaxVelocity.x * 0.1f);
	playerStars->Frequency = 0.05f;
	playerStars->ScaleRange = VRangeBounds<sf::Vector2f>(sf::Vector2f(1.0f, 1.0f) * 0.0f, sf::Vector2f(1.0f, 1.0f) * 0.2f, sf::Vector2f(1.0f, 1.0f) * 0.8f, sf::Vector2f(1.0f, 1.0f) * 1.0f);
	playerStars->Lifespan = 0.5f;
	playerStars->Size = player->Size;
	playerStars->Constant = true;

	text = new VText(5.0f, 30.0f, (float)VGlobal::p()->Width);
	text->SetFormat("Assets/Adore64.ttf", 64, VColour::HSVtoRGB(0.0f, 0.0f, 1.0f, 0.0f), VText::ALIGNCENTER);
	text->SetOutlineThickness(5.0f);
	text->SetOutlineTint(sf::Color::Black);
	text->SetText("Fly through the tunnels, and don't touch the sides! Orbs give you a bonus!");

	DefaultCamera->Follow(player);

	Add(playerStars);
	Add(player);
	Add(text);

	VGlobal::p()->Music->Fade(false, 1.0f);
}

void GoalState::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	playerStars->EmittingAngle = player->Angle + 180.0f;
	playerStars->Position = player->Position;
	playerStars->Start();

	text->SetFillTint(VColour::HSVtoRGB(0.0f, 0.0f, 1.0f, player->Position.x < 0.0f ? 0.0f : player->Position.x / 100.0f < 1.0f ? player->Position.x / 100.0f : 1.0f));

	if ((int)player->Position.x % 20 == 0)
		VGlobal::p()->Sound->Play("stars", (1.0f - DefaultCamera->Zoom) + 0.25f * 100.0f, VGlobal::p()->Random->GetFloat(1.1f, 0.9f));

	DefaultCamera->Zoom = VInterpolate::Float(1.0f, 0.25f, (player->Position.x - (VGlobal::p()->Width / 4.0f)) / (VGlobal::p()->Width / 2.0f));
	if (player->Position.x > VGlobal::p()->Width * 1.2f ||
		(player->Position.x > 100.0f && VGlobal::p()->Input->IsButtonPressed("Forward")))
	{
		VGlobal::p()->Music->OpenMusicFile("Assets/main.ogg");
		VGlobal::p()->Music->SetVolume((float)TitleState::MusicVolume);
		VGlobal::p()->Music->Play();
		VGlobal::p()->ChangeState(new PlayState());
	}
}
