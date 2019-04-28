#include "WavePostEffect.h"

WavePostEffect::WavePostEffect()
{
	horizontalWave.loadFromFile("Assets/HorizontalWave.frag", sf::Shader::Fragment);
	verticalWave.loadFromFile("Assets/VerticalWave.frag", sf::Shader::Fragment);

	waveSpeed.x = 1.0f;
	waveSpeed.y = 1.0f;

	waveMultiply.x = 1.0f;
	waveMultiply.y = 1.0f;

	waveAmplitude.x = 0.0f;
	waveAmplitude.y = 0.0f;
}

void WavePostEffect::Apply(const sf::Texture& input, sf::RenderTarget& output)
{
	prepareTextures(input.getSize());
	horizontalPass(input, mPassTexture);
	verticalPass(mPassTexture.getTexture(), output);
}

void WavePostEffect::Update(float dt)
{
	timerX += dt * waveSpeed.x;
	timerY += dt * waveSpeed.y;

	horizontalWave.setUniform("time", timerX);
	verticalWave.setUniform("time", timerY);
}

void WavePostEffect::prepareTextures(sf::Vector2u size)
{
	if (mPassTexture.getSize() != size)
	{
		mPassTexture.create(size.x, size.y);
		mPassTexture.setSmooth(true);
	}
}

void WavePostEffect::horizontalPass(const sf::Texture& input, sf::RenderTarget& output)
{
	horizontalWave.setUniform("texture", input);
	horizontalWave.setUniform("textureSize", sf::Vector2f(input.getSize()));
	horizontalWave.setUniform("waveMultiply", waveMultiply.x);
	horizontalWave.setUniform("amplitude", waveAmplitude.x);

	applyShader(horizontalWave, output);
}

void WavePostEffect::verticalPass(const sf::Texture& input, sf::RenderTarget& output)
{
	verticalWave.setUniform("texture", input);
	verticalWave.setUniform("textureSize", sf::Vector2f(input.getSize()));
	verticalWave.setUniform("waveMultiply", waveMultiply.y);
	verticalWave.setUniform("amplitude", waveAmplitude.y);

	applyShader(verticalWave, output);
}