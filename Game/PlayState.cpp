#include "PlayState.h"
#include "LevelState.h"
#include "TitleState.h"
#include "TransitionPostEffect.h"

#include "VFrame/VColour.h"
#include "VFrame/VGlobal.h"
#include "VFrame/VInterpolate.h"
#include "VFrame/VTiledSprite.h"

#define PI 3.141592653589f
float CosineInterpolate(float y1, float y2,	float mu)
{
	float mu2 = (1 - cos(mu * PI)) / 2;
	return(y1 * (1 - mu2) + y2 * mu2);
}

#include <ctime>
sf::String GetTimeString(float t)
{
	time_t time = (time_t)t;
	tm* cur_time = localtime(&time);

	char s[256];
	strftime(s, 256, "%T", cur_time);
	return s;
}

GlobalPlay PlayState::Global;

class OrbSprite : public VSprite
{
	float yPos;
	float speed;
	float t;

public:
	typedef VSprite VSUPERCLASS;

	OrbSprite(float x, float y, float speed);
	virtual void Update(float dt);
};

OrbSprite::OrbSprite(float x, float y, float s)
{
	yPos = y;
	speed = s;
	t = 0.0f;

	LoadGraphic("Assets/orb.png");
	Scale *= 12.0f / 160.0f;
	Size = sf::Vector2f(12.0f, 12.0f);
	Origin = sf::Vector2f(6.0f, 6.0f);
	Offset = sf::Vector2f(6.0f, 6.0f);
	SetPositionAtCentre(x, y);
}

void OrbSprite::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	Scale = sf::Vector2f(0.2f, 0.2f) * sinf(t) * (12.0f / 160.0f);
	Scale += sf::Vector2f(1.0f, 1.0f) * (12.0f / 160.0f);
	Position.y = yPos + (sinf(t) * 30.0f);
	t += speed * dt;
}

void PlayState::GenerateMap(std::vector<sf::Vector2u>& points, int pathWidth)
{
	std::vector<char> map(((VGlobal::p()->Width) * (VGlobal::p()->Height)) / 2);
	std::fill(map.begin(), map.end(), 1);

	for (unsigned int i = 0; i < points.size() - 1; i++)
	{
		if (i > 0)
		{
			VSprite* o = new OrbSprite(points[i].x * 2.0f, VGlobal::p()->Random->GetFloat(((points[i].y * 2.0f) - 1.0f) + pathWidth, ((points[i].y * 2.0f) - 1.0f) - pathWidth), Global.OrbSpeed);
			orbs->Add(o);
		}

		for (unsigned int j = points[i + 0].x; j < points[i + 1].x; j++)
		{
			float mu = (j - points[i + 0].x) / (float)(points[i + 1].x - points[i + 0].x);
			int y = (int)CosineInterpolate((float)points[i + 0].y, (float)points[i + 1].y, mu);

			if (j == (points[i + 1].x + points[i + 0].x) / 2)
			{
				VSprite* o = new OrbSprite((float)(points[i + 1].x + points[i + 0].x), (float)VGlobal::p()->Random->GetInt(y + pathWidth, y - pathWidth), Global.OrbSpeed);
				orbs->Add(o);
			}

			for (unsigned int z = 0; z < VGlobal::p()->Height / 2; z++)
			{
				map[(z * (VGlobal::p()->Width / 2)) + j] = 2 + (((y + z) / 5) % 4);
			}
			
			if (i == 0 && j == 0)
				player->SetPositionAtCentre(0.0f, (float)(y * 2) - 1.0f);

			for (int z = y - pathWidth; z < y + pathWidth; z++)
			{
				if (z < 1 || z > (int)(VGlobal::p()->Height / 2) - 1)
					continue;

				for (int x = (int)(j - pathWidth); x < (int)(j + pathWidth); x++)
				{
					if (x >= 0 && x < (int)(VGlobal::p()->Width / 2))
						map[(z * (VGlobal::p()->Width / 2)) + x] = 0;
				}
			}
		}
	}

	tilemap = new VTilemap();
	tilemap->LoadFromArray(map, (VGlobal::p()->Width / 2), (VGlobal::p()->Height / 2), "Assets/Tilemap.png", 2, 2, false, { 1, 2, 3, 4, 5 });
	tilemap->SetTileRenderID(1, 1);
	tilemap->SetTileRenderID(2, 2);
	tilemap->SetTileRenderID(3, 3);
	tilemap->SetTileRenderID(4, 4);
	tilemap->SetTileRenderID(5, 5);
	tilemap->SetTileRenderID(6, 6);
}

void PlayState::Initialise()
{
	VSUPERCLASS::Initialise();

	std::vector<sf::Vector2u> points(Global.NumPoints);
	for (int i = 0; i < Global.NumPoints; i++)
	{
		points[i].x = (unsigned int)((i / (float)(Global.NumPoints - 1)) * (VGlobal::p()->Width / 2));
		points[i].y = (unsigned int)VGlobal::p()->Random->GetFloat((float)(VGlobal::p()->Height / 2) - Global.PathWidth, (float)Global.PathWidth);
	}

	player = new VSprite();
	player->LoadGraphic("Assets/Player.png", true, 16, 16);
	player->MaxVelocity = sf::Vector2f(Global.MaxSpeed, Global.MaxSpeed);
	player->Velocity.x = player->MaxVelocity.x * 0.25f;
	player->Elasticity = 0.2f;
	player->AngleMax = Global.RotationSpeed;
	player->AngleDrag = player->AngleMax * 10.0f;

	playerStars = new VEmitter(0.0f, 0.0f, 32);
	playerStars->LoadParticlesFromFile(32, "Assets/Player.png", false, 16, 16, sf::IntRect(0, 16, 16, 16));
	playerStars->AngleVelocityRange = VRange<float>(180.0f, 180.0f);
	playerStars->AngleDragRange = 36.0f;
	playerStars->AngleRange = VRange<float>(0.0f, 360.0f);
	playerStars->Circular = true;
	playerStars->AlphaRange = VRangeBounds<float>(1.0f, 1.0f, 0.0f, 0.0f);
	playerStars->SpeedRange = VRange<float>(player->MaxVelocity.x * 0.1f);
	playerStars->ElasticityRange = 0.3f;
	playerStars->Frequency = 0.05f;
	playerStars->ScaleRange = VRangeBounds<sf::Vector2f>(sf::Vector2f(1.0f, 1.0f) * 0.0f, sf::Vector2f(1.0f, 1.0f) * 0.2f, sf::Vector2f(1.0f, 1.0f) * 0.8f, sf::Vector2f(1.0f, 1.0f) * 1.0f);
	playerStars->Lifespan = 0.5f;
	playerStars->Size = player->Size;
	playerStars->Constant = true;

	orbs = new VGroup();

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

	gameOverText = new VText(0.0f, 300.0f, (float)VGlobal::p()->Width);
	gameOverText->SetFormat("Assets/Adore64.ttf", 64, sf::Color::White, VText::ALIGNCENTRE);
	gameOverText->SetOutlineThickness(5.0f);
	gameOverText->SetOutlineTint(sf::Color::Black);
	gameOverText->ScrollFactor = sf::Vector2f();
	gameOverText->ZoomFactor = 0.0f;
	gameOverText->Origin = sf::Vector2f(0.5f, 0.5f);
	gameOverText->SetFillTint(sf::Color::Transparent);

	GenerateMap(points, Global.PathWidth);
	Add(tilemap);
	Add(playerStars);
	Add(player);
	Add(orbs);
	Add(timeText);
	Add(healthText);
	Add(scoreText);
	Add(gameOverText);

	if (Global.Tunnel == 0)
	{
		VSprite* controls = new VSprite();
		controls->LoadGraphic("Assets/controls.png", true, 400, 60);
		controls->Animation.AddAnimation("default", 0, 4, 4.0f, true);
		controls->Animation.Play("default");
		controls->ScrollFactor = sf::Vector2f();
		controls->ZoomFactor = 0.0f;
		Add(controls);

		controls->SetPositionAtCentre(VGlobal::p()->Width / 2.0f, 600.0f);
	}
	
	player->Health = (float)Global.Health;
	Global.Time = 0.0f;

	curHealth = (int)player->Health;

	DefaultCamera->Follow(player);
	DefaultCamera->Zoom = 0.25f;

	hurtResponse = [this](VObject* p, VObject* t) {

		if (p->Touching)
		{
			curHealth = (int)p->Health;
			p->Hurt(10);

			if (p->Touching & p->TOUCHLEFT)
				p->Velocity.x = 20.0f;
			if (p->Touching & p->TOUCHRIGHT)
				p->Velocity.x -= 20.0f;
			if (p->Touching & p->TOUCHTOP)
				p->Velocity.y = 20.0f;
			if (p->Touching & p->TOUCHBOTTOM)
				p->Velocity.y -= 20.0f;

			p->Touching = 0;
			p->AngleVelocity += VGlobal::p()->Random->GetFloat(Global.RotationSpeed, -Global.RotationSpeed) * 5;
			player->Scale *= 1.1f;

			VGlobal::p()->Sound->Play("hit", 100.0f, VGlobal::p()->Random->GetFloat(1.5f, 0.5f));

			if (!p->alive)
			{
				playerStars->Circular = false;
				playerStars->Explosion = true;
				playerStars->Start();

				VGlobal::p()->Sound->Play("crash", 100.0f, VGlobal::p()->Random->GetFloat(1.5f, 0.5f));
				VGlobal::p()->Music->Fade(false, 1.0f);
			}
		}
	};

	orbResponse = [this](VObject* p, VObject* o) {
		if (o->alive)
			p->Health += 15;
		
		o->Kill();
		VGlobal::p()->Sound->Play("orb", 100.0f, VGlobal::p()->Random->GetFloat(1.5f, 0.5f));
	};

	uiTimer = new VTimer();

	if (VGlobal::p()->RenderState.texture)
	{
		TransitionPostEffect* transition = dynamic_cast<TransitionPostEffect*>(dynamic_cast<VPostEffectMultipass*>(VGlobal::p()->PostProcess.get())->GetPostEffect(2));
		transition->Start(*VGlobal::p()->RenderState.texture);
	}
}

void PlayState::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	if (player->alive)
		Global.Time += dt;
	else
	{
		curHealth = 0;
		gameOverText->SetText("Game Over! You lasted this long:\n" + GetTimeString(Global.TotalTime + Global.Time));
		gameOverText->SetFillTint(VColour::HSVtoRGB(0.0f, 0.0f, 1.0f, uiTimer->Seconds() > 1.0f ? 1.0f : uiTimer->Seconds()));
		if (uiTimer->Seconds() > 2.0f && VGlobal::p()->Input->IsButtonDown("Forward"))
		{
			VGlobal::p()->ChangeState(new TitleState());
		}

		return;
	}

	player->Acceleration = sf::Vector2f();
	player->AngleAcceleration = (VGlobal::p()->Input->CurrentAxisValue("X") / 100.0f) * (-player->AngleMax * 4.0f);
	if (VGlobal::p()->Input->IsButtonDown("Left"))
		player->AngleAcceleration = -player->AngleMax * 4.0f;
	if (VGlobal::p()->Input->IsButtonDown("Right"))
		player->AngleAcceleration = player->AngleMax * 4.0f;

	if (VGlobal::p()->Input->IsButtonDown("Forward"))
	{
		player->Acceleration.x = (float)cosf(player->Angle * PI / 180.0f) * player->MaxVelocity.x;
		player->Acceleration.y = (float)sinf(player->Angle * PI / 180.0f) * player->MaxVelocity.x;
		playerStars->EmittingAngle = player->Angle + 180.0f;
		playerStars->Position = player->Position;
		playerStars->Start();
	}
	else
	{
		playerStars->Stop();
	}

	player->Scale.x = VInterpolate::Float(player->Scale.x, 1.0f, powf(0.01f, 1 - dt));
	player->Scale.y = VInterpolate::Float(player->Scale.y, 1.0f, powf(0.012f, 1 - dt));

	VGlobal::p()->Collides(playerStars, tilemap);
	VGlobal::p()->Collides(player, tilemap, hurtResponse);
	VGlobal::p()->Overlaps(player, orbs, orbResponse);

	if (uiTimer->Milliseconds() > 100)
	{
		if (VGlobal::p()->Input->IsButtonDown("Forward"))
			VGlobal::p()->Sound->Play("stars", 100.0f, VGlobal::p()->Random->GetFloat(1.5f, 0.5f));

		if (player->Health > 0.0f)
			player->Health -= 0.05f;
		else
		{
			player->Health = 0.0f;
			player->Kill();

			if (!player->alive)
			{
				playerStars->Circular = false;
				playerStars->Explosion = true;
				playerStars->Start();

				VGlobal::p()->Sound->Play("crash", 100.0f, VGlobal::p()->Random->GetFloat(1.5f, 0.5f));
				VGlobal::p()->Music->Fade(false, 1.0f);
			}
		}

		if (curHealth < (int)player->Health)
			curHealth++;
		if (curHealth > (int)player->Health)
			curHealth--;

		uiTimer->Restart();
	}

	healthText->Scale = sf::Vector2f(1.0f, 1.0f) * (curHealth != player->Health ? 1.25f : 1.0f);

	if (Global.Tunnel == 0)
	{
		timeText->SetText(GetTimeString(Global.Time) + "\nTIME");
		healthText->SetText(std::to_string((int)curHealth) + "\nHEALTH");
		scoreText->SetText(std::to_string(Global.Score) + "\nSCORE");
	}
	else
	{
		timeText->SetText(GetTimeString(Global.Time));
		healthText->SetText(std::to_string((int)curHealth));
		scoreText->SetText(std::to_string(Global.Score));
	}

	if (player->Position.x > VGlobal::p()->Width)
	{
		Global.Health = (int)player->Health;
		Global.Tunnel++;
		Global.PathWidth--;

		Global.Score += Global.Health;
		Global.Score += (((Global.NumPoints - 2) * 15) - Global.Time) > 0 ? (int)(((Global.NumPoints - 2) * 15) - Global.Time) : 0;
		Global.NumPoints++;
		
		Global.MaxSpeed += 10.0f;
		Global.TotalTime += Global.Time;

		VGlobal::p()->ChangeState(new LevelState());
	}
}