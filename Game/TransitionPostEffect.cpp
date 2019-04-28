#include "TransitionPostEffect.h"
#include "VFrame/VInterpolate.h"
#include <SFML/Graphics/RenderTexture.hpp>

TransitionPostEffect::TransitionPostEffect(const sf::String& transitionShader) : VPostEffect(transitionShader, sf::Shader::Fragment)
{
}

void TransitionPostEffect::Update(float dt)
{
	VSUPERCLASS::Update(dt);

	SetParameter("progress", 1.0f);
	SetParameter("resolution", 1.0f, 1.0f);
	SetInputTextureName("to");

	if (running)
	{
		SetParameter("from", TextureFrom);
		
		Progress += dt;
		SetParameter("progress", VInterpolate::Float(0.0f, 1.0f, Progress, 1.0f, VInterpolate::InOutQuad));

		if (Progress >= 1.0f)
		{
			running = false;
			if (onComplete != nullptr)
			{
				onComplete();
			}
		}
	}
}

void TransitionPostEffect::Start(const sf::Texture& tex, std::function<void()> OnComplete)
{
	if (running)
		return;

	TextureFrom = tex;
	running = true;
	Progress = 0.0f;

	onComplete = OnComplete;
}
