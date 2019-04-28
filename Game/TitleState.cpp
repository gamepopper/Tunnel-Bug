#include "TitleState.h"
#include <VFrame/VGlobal.h>
#include <VFrame/VInterpolate.h>
#include <VFrame/VColour.h>
#include <VFrame/VTiledSprite.h>
#include "VFrame/VPostEffect.h"

#include "TransitionPostEffect.h"
#include "BloomPostEffect.h"
#include "WavePostEffect.h"

#include "GoalState.h"
#include "PlayState.h"

int TitleState::MusicVolume = 100;

void TitleState::Initialise()
{
	VSUPERCLASS::Initialise();

	VGlobal::p()->BackgroundColor = VColour::HSVtoRGB(0, 0, 0.01f);

	if (VGlobal::p()->PostProcess == nullptr)
	{
		VPostEffect* tintPost = new VPostEffect("Assets/Tint.frag", sf::Shader::Fragment);
		tintPost->SetParameter("tint", sf::Color(145, 242, 161));

		VPostEffect* matrixPost = new VPostEffect("Assets/Matrix.frag", sf::Shader::Fragment);
		matrixPost->SetParameter("resolution", (float)VGlobal::p()->Width, (float)VGlobal::p()->Height);
		matrixPost->SetParameter("alpha", 0.8f);
		matrixPost->SetParameter("step", 4.0f);

		std::unique_ptr<VPostEffectMultipass> multipass = std::make_unique<VPostEffectMultipass>(4);
		multipass->AddPostEffect(new BloomPostEffect());
		multipass->AddPostEffect(matrixPost);
		multipass->AddPostEffect(new TransitionPostEffect("Assets/transition.frag"));
		multipass->AddPostEffect(tintPost);
		VGlobal::p()->PostProcess = std::move(multipass);
	}

	Tilemap[0] = new VTilemap();
	Tilemap[0]->Position.y = 100.0f;
	Tilemap[0]->Velocity.x = -100.0f;
	Tilemap[0]->Moves = true;
	UpdateBackgroundTilemap(0);
	Tilemap[1] = new VTilemap();
	Tilemap[1]->Position.y = 100.0f;
	Tilemap[1]->Velocity.x = -100.0f;
	Tilemap[1]->Moves = true;
	Tilemap[1]->Position.x = (float)VGlobal::p()->Width;
	UpdateBackgroundTilemap(1);
	Add(Tilemap[0]);
	Add(Tilemap[1]);

	std::unique_ptr<WavePostEffect> waveEffect = std::make_unique<WavePostEffect>();
	waveEffect->waveAmplitude.x = 10.0f;
	waveEffect->waveAmplitude.y = 25.0f;

	TitleRender = new VRenderGroup(0.0f, 0.0f, 1000, 250, 1);
	TitleRender->Sprite->SetPositionAtCentre(VGlobal::p()->Width / 2.0f, 200.0f);
	TitleRender->RenderViewTransform = false;
	TitleRender->PostEffect = std::move(waveEffect);

	Title = new VSprite();
	Title->LoadGraphic("Assets/Title.png");
	Title->SetPositionAtCentre(TitleRender->Sprite->Size / 2.0f);
	Title->Scale *= 2.0f;
	TitleRender->Add(Title);
	Add(TitleRender);

	for (int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		Options[i] = new VText(0.0f, 400.0f, (float)VGlobal::p()->Width);
		Options[i]->SetFormat("Assets/Adore64.ttf", 54, sf::Color::White, VText::ALIGNCENTRE);
		Options[i]->Origin = sf::Vector2f(0.5f, 0.5f);

		Add(Options[i]);
	}

	VText* credits = new VText(0.0f, 600.0f, (float)VGlobal::p()->Width, "GAMEPOPPER - LUDUM DARE 44\n\"YOUR LIFE IS CURRENCY\"");
	credits->SetFormat("Assets/Adore64.ttf", 36, sf::Color::White, VText::ALIGNCENTRE);
	credits->SetLineSpaceModifier(2);
	Add(credits);

	if (VGlobal::p()->Music->Status() != sf::Music::Playing)
	{
		VGlobal::p()->Music->OpenMusicFile("Assets/title.ogg");
		VGlobal::p()->Music->SetVolume((float)MusicVolume);
		VGlobal::p()->Music->Fade(true, 0.5f);
		VGlobal::p()->Music->SetLoop(true);
		VGlobal::p()->Music->Play();
	}

	VGlobal::p()->Sound->Load("Assets/blip.ogg", "blip");
	VGlobal::p()->Sound->Load("Assets/buy.ogg", "buy");
	VGlobal::p()->Sound->Load("Assets/crash.ogg", "crash");
	VGlobal::p()->Sound->Load("Assets/hit.ogg", "hit");
	VGlobal::p()->Sound->Load("Assets/orb.ogg", "orb");
	VGlobal::p()->Sound->Load("Assets/start.ogg", "start");
	VGlobal::p()->Sound->Load("Assets/stars.ogg", "stars");

	UpdateEntries();

	DefaultCamera->Fade(VColour::HSVtoRGB(0.0f, 0.0f, 0.05f), 1.0f, true);
}

void TitleState::HandleEvents(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::F)
		{
			VGlobal::p()->ToggleFullscreen();
		}
	}
}

void TitleState::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	for (int i = 0; i < 2; i++)
	{
		Tilemap[i]->Position.x -= 100.0f * dt;
		if (Tilemap[i]->Position.x < -(float)VGlobal::p()->Width)
		{
			UpdateBackgroundTilemap(i);
			Tilemap[i]->Position.x = (float)VGlobal::p()->Width;
		}
	}

	if ((VGlobal::p()->Input->CurrentAxisValue("X") > 0.0f && VGlobal::p()->Input->LastAxisValue("X") <= 0.0f) ||
		VGlobal::p()->Input->IsButtonPressed("Left"))
	{
		t = 0.0f;
		target = -((float)(VGlobal::p()->Width * 1.5f) / NUM_TITLE_OPTIONS);
		CurrentOption = (CurrentOption - 1 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS;
		UpdateEntries();
	}

	if ((VGlobal::p()->Input->CurrentAxisValue("X") < 0.0f && VGlobal::p()->Input->LastAxisValue("X") >= 0.0f) ||
		VGlobal::p()->Input->IsButtonPressed("Right"))
	{
		t = 0.0f;
		target = ((float)(VGlobal::p()->Width * 1.5f) / NUM_TITLE_OPTIONS);
		CurrentOption = (CurrentOption + 1 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS;
		UpdateEntries();
	}

	if (VGlobal::p()->Input->IsButtonPressed("Forward"))
	{
		if (CurrentOption == TITLE_MUSICVOLUME)
		{
			MusicVolume = (MusicVolume + 10) % 110;
			VGlobal::p()->Music->SetVolume((float)MusicVolume);
			UpdateEntries();
		}

		if (CurrentOption == TITLE_SFXVOLUME)
		{
			int SoundVolume = (int)VGlobal::p()->Sound->MasterVolume() + 10;
			SoundVolume %= 110;
			VGlobal::p()->Sound->SetMasterVolume((float)SoundVolume);
			UpdateEntries();
		}

		SelectEntries();
	}

	t += dt;
	for (unsigned int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		Options[i]->Position.x = ((i - CurrentOption + 2 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS) * (float)(VGlobal::p()->Width * 1.5f) / NUM_TITLE_OPTIONS;
		Options[i]->Position.x += VInterpolate::Float(target, 0.0f, t, 1.0f, VInterpolate::OutQuad);

		//Hack to ensure the right most entry remains there during transition.
		if (target < 0 && i == (CurrentOption + 2) % NUM_TITLE_OPTIONS)
		{
			Options[i]->Position.x = Options[(i - 1 + NUM_TITLE_OPTIONS) % NUM_TITLE_OPTIONS]->Position.x - (3 * target);
		}

		Options[i]->Position.x -= (2 * (float)(VGlobal::p()->Width * 1.5f) / NUM_TITLE_OPTIONS);

		if (CurrentOption == i)
			Options[i]->Scale = VInterpolate::Vector2f(sf::Vector2f(0.4f, 0.4f), sf::Vector2f(1.0f, 1.0f), t, 1.0f, VInterpolate::InOutExpo);
		else
			Options[i]->Scale = sf::Vector2f(0.4f, 0.4f);
	}
}

void TitleState::UpdateEntries()
{
	std::string menuOptions[NUM_TITLE_OPTIONS] = 
	{
		"PLAY MODE",
		"MUSIC:" + std::to_string(MusicVolume),
		"SFX:" + std::to_string((int)VGlobal::p()->Sound->MasterVolume()),
		"EXIT GAME"
	};

	for (unsigned int i = 0; i < NUM_TITLE_OPTIONS; i++)
	{
		Options[i]->SetText(menuOptions[i]);
	}

	VGlobal::p()->Sound->Play("blip", 100.0f, VGlobal::p()->Random->GetFloat(1.5f, 0.5f));
}

void TitleState::SelectEntries()
{
	switch (CurrentOption)
	{
	case TITLE_START:
		VGlobal::p()->ChangeState(new GoalState());
		break;
	case TITLE_EXIT:
		VGlobal::p()->Exit();
		break;
	}

	VGlobal::p()->Sound->Play("start", 100.0f, VGlobal::p()->Random->GetFloat(1.5f, 0.5f));
}

void TitleState::UpdateBackgroundTilemap(unsigned int index)
{
	int indexOffset = (index % 2) * (points.size() / 2);
	int nextIndexOffset = ((index + 1) % 2) * (points.size() / 2);

	points[indexOffset] = points[nextIndexOffset + (points.size() / 2) - 1];
	points[indexOffset].x = 0;
	for (int i = 1; i < (int)(points.size() / 2); i++)
	{
		points[indexOffset + i].x = (unsigned int)((i / ((float)(points.size() / 2) - 1.0f)) * (VGlobal::p()->Width / 2));
		points[indexOffset + i].y = VGlobal::p()->Random->GetInt((VGlobal::p()->Height / 4) - 6, 6);
	}

	std::vector<char> map((VGlobal::p()->Width / 2) * (VGlobal::p()->Height / 4));
	std::fill(map.begin(), map.end(), 0);

	for (unsigned int i = 0; i < (points.size() / 2) - 1; i++)
	{
		int left = indexOffset + i + 0;
		int right = indexOffset + i + 1;
		for (unsigned int j = points[left].x; j < points[right].x; j++)
		{
			float mu = (j - points[left].x) / (float)(points[right].x - points[left].x);
			int y = (int)CosineInterpolate((float)points[left].y, (float)points[right].y, mu);
			map[((y - 4) * (VGlobal::p()->Width / 2)) + j] = 1;
			map[((y + 4) * (VGlobal::p()->Width / 2)) + j] = 1;
		}
	}

	Tilemap[index]->LoadFromArray(map, VGlobal::p()->Width / 2, VGlobal::p()->Height / 4, "Assets/Tilemap.png", 2, 2, false, { });
	Tilemap[index]->SetTileRenderID(1, 1);
}