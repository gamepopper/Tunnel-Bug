#include "LevelState.h"
#include "VFrame/VGlobal.h"
#include "VFrame/VColour.h"
#include <VFrame/VTiledSprite.h>

#include "PlayState.h"
#include "TransitionPostEffect.h"

enum
{
	OPTION_ROTATION,
	OPTION_ORBSPEED,
	OPTION_PATHWIDTH,
	OPTION_MOVESPEED,
	OPTION_PATHTURNS,
	NUM_OPTIONS
};

void LevelState::Initialise()
{
	VSUPERCLASS::Initialise();

	player = new VSprite();
	player->LoadGraphic("Assets/Player.png", true, 16, 16);
	player->MaxVelocity = sf::Vector2f(PlayState::Global.MaxSpeed, PlayState::Global.MaxSpeed);
	player->Velocity.x = player->MaxVelocity.x * 0.5f;
	player->SetPositionAtCentre(-32.0f, 3.0f * VGlobal::p()->Height / 4.0f);

	playerStars = new VEmitter(0.0f, 0.0f, 32);
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

	timeText = new VText(5.0f, 10.0f, (float)VGlobal::p()->Width);
	timeText->SetFormat("Assets/Adore64.ttf", 36);
	timeText->SetOutlineThickness(5.0f);
	timeText->SetOutlineTint(sf::Color::Black);
	timeText->ScrollFactor = sf::Vector2f();
	timeText->ZoomFactor = 0.0f;

	healthText = new VText(0.0f, 10.0f, (float)VGlobal::p()->Width);
	healthText->SetFormat("Assets/Adore64.ttf", 48, sf::Color::White, VText::ALIGNCENTRE);
	healthText->SetOutlineThickness(5.0f);
	healthText->SetOutlineTint(sf::Color::Black);
	healthText->ScrollFactor = sf::Vector2f();
	healthText->ZoomFactor = 0.0f;
	healthText->Origin = sf::Vector2f(0.5f, 0.5f);

	scoreText = new VText(-5.0f, 10.0f, (float)VGlobal::p()->Width);
	scoreText->SetFormat("Assets/Adore64.ttf", 36, sf::Color::White, VText::ALIGNRIGHT);
	scoreText->SetOutlineThickness(5.0f);
	scoreText->SetOutlineTint(sf::Color::Black);
	scoreText->ScrollFactor = sf::Vector2f();
	scoreText->ZoomFactor = 0.0f;

	for (int i = 0; i < 3; i++)
	{
		option[i] = new VSprite();
		option[i]->LoadGraphic("Assets/orb.png");
		option[i]->SetPositionAtCentre((1.0f + i) * VGlobal::p()->Width / 4.0f, 3.0f * VGlobal::p()->Height / 4.0f);

		optionStars[i] = new VEmitter(option[i]->Position + (option[i]->Size / 2.0f), 32);
		optionStars[i]->LoadParticlesFromFile(32, "Assets/Player.png", false, 16, 16, sf::IntRect(0, 16, 16, 16));
		optionStars[i]->AngleVelocityRange = VRange<float>(180.0f, 180.0f);
		optionStars[i]->AngleDragRange = 36.0f;
		optionStars[i]->EmittingAngle = 270.0f;
		optionStars[i]->Circular = true;
		optionStars[i]->AlphaRange = VRangeBounds<float>(1.0f, 1.0f, 0.0f, 0.0f);
		optionStars[i]->SpeedRange = VRange<float>(300.0f);
		optionStars[i]->Frequency = 0.05f;
		optionStars[i]->ScaleRange = VRangeBounds<sf::Vector2f>(sf::Vector2f(1.0f, 1.0f) * 0.5f, sf::Vector2f(1.0f, 1.0f) * 0.8f, sf::Vector2f(1.0f, 1.0f) * 3.0f, sf::Vector2f(1.0f, 1.0f) * 4.0f);
		optionStars[i]->Lifespan = 1.0f;
		optionStars[i]->Size = option[i]->Size;
		optionStars[i]->Explosion = true;

		optionText[i] = new VText(0.0f, 0.0f, 200.0f);
		optionText[i]->SetFormat("Assets/Adore64.ttf", 18, sf::Color::White, VText::ALIGNCENTER);
		optionText[i]->SetPositionAtCentre((1.0f + i) * VGlobal::p()->Width / 4.0f, 1.0f * VGlobal::p()->Height / 4.0f);
		optionText[i]->SetOutlineThickness(3.0f);
		optionText[i]->SetOutlineTint(sf::Color::Black);
		optionText[i]->Origin = sf::Vector2f(0.5f, 0.5f);

		optionType[i] = VGlobal::p()->Random->GetInt(PlayState::Global.Tunnel <= 5 ? NUM_OPTIONS - 1 : OPTION_PATHTURNS - 1);

		switch (optionType[i])
		{
			case OPTION_ROTATION:
				optionText[i]->SetText("Turn faster.\nCosts 10 Health.");
				break;
			case OPTION_ORBSPEED:
				optionText[i]->SetText("Orbs move slower.\nCosts 10 Health.");
				break;
			case OPTION_MOVESPEED:
				optionText[i]->SetText("Move slower.\nCosts 10 Health.");
				break;
			case OPTION_PATHWIDTH:
				optionText[i]->SetText("Make the path wider.\nCosts 20 Health.");
				break;
			case OPTION_PATHTURNS:
				optionText[i]->SetText("Give paths fewer turns.\nCosts 20 Health.");
				break;
		}

		Add(optionStars[i]);
		Add(option[i]);
		Add(optionText[i]);
	}

	Add(playerStars);
	Add(player);
	Add(timeText);
	Add(healthText);
	Add(scoreText);

	curHealth = PlayState::Global.Health;
	uiTimer = new VTimer();

	if (VGlobal::p()->RenderState.texture)
	{
		TransitionPostEffect* transition = dynamic_cast<TransitionPostEffect*>(dynamic_cast<VPostEffectMultipass*>(VGlobal::p()->PostProcess.get())->GetPostEffect(2));
		transition->Start(*VGlobal::p()->RenderState.texture);
	}
}

void LevelState::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	timeText->SetText(GetTimeString(PlayState::Global.Time));
	healthText->SetText(std::to_string(curHealth));
	scoreText->SetText(std::to_string(PlayState::Global.Score));

	playerStars->EmittingAngle = player->Angle + 180.0f;
	playerStars->Position = player->Position;
	playerStars->Start();

	if (uiTimer->Milliseconds() > 100)
	{
		if (curHealth > PlayState::Global.Health)
			curHealth--;

		if (player->Velocity.x > player->MaxVelocity.x * 0.5f)
			player->Velocity.x-=2.0f;

		uiTimer->Restart();

		VGlobal::p()->Sound->Play("stars", 0.25f * 100.0f, VGlobal::p()->Random->GetFloat(1.1f, 0.9f));
	}

	if (VGlobal::p()->Input->IsButtonDown("Left") || VGlobal::p()->Input->IsButtonDown("Right"))
	{
		player->Velocity.x = player->MaxVelocity.x;
	}

	for (int i = 0; i < 3; i++)
	{
		if (option[i]->alive)
		{
			option[i]->Scale = sf::Vector2f(1.0f, 1.0f) * (1.0f + (sinf(3.f + player->Position.x / player->MaxVelocity.x) * 0.1f));

			if (PlayState::Global.Health > 10 && VGlobal::p()->Overlaps(player, option[i]))
			{
				optionText[i]->Scale = sf::Vector2f(1.0f, 1.0f) * 1.5f;
				if (VGlobal::p()->Input->IsButtonPressed("Forward"))
				{
					player->Velocity.x = player->MaxVelocity.x;
					optionStars[i]->Start();
					option[i]->Kill();

					VGlobal::p()->Sound->Play("buy", 100.0f, VGlobal::p()->Random->GetFloat(1.1f, 0.9f));

					switch (optionType[i])
					{
					case OPTION_ROTATION:
						PlayState::Global.RotationSpeed += 20;
						PlayState::Global.Health -= 10;
						break;
					case OPTION_ORBSPEED:
						PlayState::Global.OrbSpeed -= 0.25f;
						PlayState::Global.Health -= 10;
						break;
					case OPTION_MOVESPEED:
						PlayState::Global.MaxSpeed -= 10.0f;
						PlayState::Global.Health -= 10;
						break;
					case OPTION_PATHWIDTH:
						PlayState::Global.PathWidth += 2;
						PlayState::Global.Health -= 20;
						break;
					case OPTION_PATHTURNS:
						PlayState::Global.NumPoints--;
						PlayState::Global.Health -= 20;
						break;
					}
				}
			}
			else
			{
				optionText[i]->Scale = sf::Vector2f(1.0f, 1.0f);
			}
		}
		else
		{
			optionText[i]->SetFillTint(VColour::HSVtoRGB(0.0f, 0.0f, 1.0f, 0.5f));
			optionText[i]->Scale = sf::Vector2f(1.0f, 1.0f);
		}
	}

	if (player->Position.x > VGlobal::p()->Width * 1.2f)
	{
		VGlobal::p()->ChangeState(new PlayState());
	}
}