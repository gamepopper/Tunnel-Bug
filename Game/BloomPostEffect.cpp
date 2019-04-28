#include "BloomPostEffect.h"

BloomPostEffect::BloomPostEffect()
{
	brightness.loadFromFile("Assets/Brightness.frag", sf::Shader::Fragment);
	downSample.loadFromFile("Assets/DownSample.frag", sf::Shader::Fragment);
	gaussianBlur.loadFromFile("Assets/GaussianBlur.frag", sf::Shader::Fragment);
	addition.loadFromFile("Assets/Add.frag", sf::Shader::Fragment);

	SetBloomFactor(0.4f);
}

void BloomPostEffect::Apply(const sf::Texture& input, sf::RenderTarget& output)
{
	prepareTextures(input.getSize());

	filterBright(input, mBrightnessTexture);

	downsample(mBrightnessTexture.getTexture(), mFirstPassTextures[0]);
	mFirstPassTextures[0].display();
	blurMultipass(mFirstPassTextures);

	downsample(mFirstPassTextures[0].getTexture(), mSecondPassTextures[0]);
	mSecondPassTextures[0].display();
	blurMultipass(mSecondPassTextures);

	add(mFirstPassTextures[0].getTexture(), mSecondPassTextures[0].getTexture(), mFirstPassTextures[1]);
	mFirstPassTextures[1].display();
	add(input, mFirstPassTextures[1].getTexture(), output);
}

void BloomPostEffect::SetBloomFactor(float factor)
{
	brightness.setUniform("Factor", factor);
}

void BloomPostEffect::prepareTextures(sf::Vector2u size)
{
	if (mBrightnessTexture.getSize() != size)
	{
		mBrightnessTexture.create(size.x, size.y);
		mBrightnessTexture.setSmooth(true);

		mFirstPassTextures[0].create(size.x / 2, size.y / 2);
		mFirstPassTextures[0].setSmooth(true);
		mFirstPassTextures[1].create(size.x / 2, size.y / 2);
		mFirstPassTextures[1].setSmooth(true);

		mSecondPassTextures[0].create(size.x / 4, size.y / 4);
		mSecondPassTextures[0].setSmooth(true);
		mSecondPassTextures[1].create(size.x / 4, size.y / 4);
		mSecondPassTextures[1].setSmooth(true);
	}
}

void BloomPostEffect::filterBright(const sf::Texture& input, sf::RenderTexture& output)
{
	brightness.setUniform("texture", input);
	applyShader(brightness, output);
	output.display();
}

void BloomPostEffect::blurMultipass(RenderArray& renderTextures)
{
	sf::Vector2u textureSize = renderTextures[0].getSize();

	for (std::size_t count = 0; count < 2; ++count)
	{
		blur(renderTextures[0].getTexture(), renderTextures[1], sf::Vector2f(0.f, 1.f / textureSize.y));
		blur(renderTextures[1].getTexture(), renderTextures[0], sf::Vector2f(1.f / textureSize.x, 0.f));
	}
}

void BloomPostEffect::blur(const sf::Texture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor)
{
	gaussianBlur.setUniform("texture", input);
	gaussianBlur.setUniform("offsetFactor", offsetFactor);
	applyShader(gaussianBlur, output);
	output.display();
}

void BloomPostEffect::downsample(const sf::Texture& input, sf::RenderTexture& output)
{
	downSample.setUniform("texture", input);
	downSample.setUniform("textureSize", sf::Vector2f(input.getSize()));
	applyShader(downSample, output);
	output.display();
}

void BloomPostEffect::add(const sf::Texture& source, const sf::Texture& bloom, sf::RenderTarget& output)
{
	addition.setUniform("texture", source);
	addition.setUniform("other", bloom);
	applyShader(addition, output);
}
