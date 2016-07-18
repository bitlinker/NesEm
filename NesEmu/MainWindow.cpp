#include "MainWindow.h"


MainWindow::MainWindow()
	: mWindow(sf::VideoMode(800, 600), "NES window")
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::update()
{
	if (!mWindow.isOpen())
	{
		return false;
	}

	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		// "close requested" event: we close the window
		if (event.type == sf::Event::Closed)
			mWindow.close();

		if (event.type == sf::Event::KeyPressed)
		{
			const sf::Event::KeyEvent& keyEvent = event.key;
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			const sf::Event::KeyEvent& keyEvent = event.key;
		}
	}

	// clear the window with black color
	mWindow.clear(sf::Color::Black);

	// TODO: draw here
	// window.draw(...);

	// end the current frame
	mWindow.display();
	return true;

}