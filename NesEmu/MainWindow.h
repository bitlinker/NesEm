#pragma once
#include <SFML/Graphics.hpp>

class ConsoleComponents
{
	// TODO: singleton pool
};

class MainWindow
{
public:
	MainWindow();
	~MainWindow();

	bool update();

private:
	sf::RenderWindow mWindow;
};

