#include <SFML/Graphics.hpp>

#include "GameEngine.h"
#include <iostream>


int main()
{
	

	GameEngine g("resources/assets.txt");
	g.run();
}