#include <VFrame/VGame.h>
#include <memory>

#include "TitleState.h"
#include "VFrame/VGlobal.h"

/**
Theme: Your Life Is Currency

Navigate a ship through a winding RNG path, touching the walls loses health.
Health can be regained by collecting orbs.
Health can be spent to make the future paths easier.

TODO:
Generate tilemap out of a spline with infinite amount of nodes. DONE
Ship that can only navigate through rotation and thrust. DONE
Get the following options implemented between levels:
- Increase Path Width
- Increase Rotation Speed
- Decrease Path Windingness
- Decrease Orb Speed
- Decrease Move Speed

Use physics engine for collision handling?
*/

int main()
{
	std::unique_ptr<VGame> game = std::make_unique<VGame>();

	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 0;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 4;
	settings.minorVersion = 5;

	VGlobal::p()->Input->SetAxisInput("X", sf::Keyboard::A, sf::Keyboard::D, VInputHandler::PovX);
	VGlobal::p()->Input->SetButtonInput("Left", sf::Keyboard::Left, GAMEPAD_BUTTON::BUTTON_LEFT_SHOULDER);
	VGlobal::p()->Input->SetButtonInput("Right", sf::Keyboard::Right, GAMEPAD_BUTTON::BUTTON_RIGHT_SHOULDER);
	VGlobal::p()->Input->SetButtonInput("Forward", sf::Keyboard::Space, GAMEPAD_BUTTON::BUTTON_A);
	
	return game->Run("Tunnel Bug", new TitleState(), 640, 360, 1280, 720, 60.0f, 7, settings);
}
