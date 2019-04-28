#pragma once
#include "VFrame/VPostEffect.h"
#include <functional>

class TransitionPostEffect : public VPostEffect
{
private:
	float Progress = 0.0f;
	sf::Texture TextureFrom;

	bool running = false;
	std::function<void()> onComplete;

public:
	typedef VPostEffect VSUPERCLASS;
	TransitionPostEffect(const sf::String& transitionShader);
	virtual void Update(float dt);

	void Start(const sf::Texture& tex, std::function<void()> OnComplete = nullptr);
};

