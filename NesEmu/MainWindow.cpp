#include "MainWindow.h"


MainWindow::MainWindow(Console& console)
	: mWindow()
    , mPpuTexture()
    , mPpuSprite()
    , mKeyBindMap()
    , mConsole(console)
    , mIsNmiOccured(false)
{
    init();
    initKeyBindings();
    mConsole.setNmiListener(this);
}

MainWindow::~MainWindow()
{
    mConsole.setNmiListener(nullptr);
}

void MainWindow::init()
{
    sf::Image img;
    auto ppu = mConsole.getPPU();
    mWindow.create(sf::VideoMode(ppu->getWidth(), ppu->getHeight()), "NES window");
    sf::Vector2u size = mWindow.getSize();
    mPpuTexture.create(size.x, size.y);
    mPpuSprite.setTexture(mPpuTexture);
}

// TODO: configure
void MainWindow::initKeyBindings()
{
    auto jp1 = mConsole.getJoyPad(0);
    auto jp2 = mConsole.getJoyPad(1);

    // JoyPad1
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Q, KeyBinding(jp1, JoyPad::BTN_A)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::W, KeyBinding(jp1, JoyPad::BTN_B)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Down, KeyBinding(jp1, JoyPad::BTN_DOWN)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Left, KeyBinding(jp1, JoyPad::BTN_LEFT)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Right, KeyBinding(jp1, JoyPad::BTN_RIGHT)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Up, KeyBinding(jp1, JoyPad::BTN_UP)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Z, KeyBinding(jp1, JoyPad::BTN_START)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::X, KeyBinding(jp1, JoyPad::BTN_SEL)));

    // JoyPad2
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::O, KeyBinding(jp2, JoyPad::BTN_A)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::P, KeyBinding(jp2, JoyPad::BTN_B)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Numpad2, KeyBinding(jp2, JoyPad::BTN_DOWN)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Numpad4, KeyBinding(jp2, JoyPad::BTN_LEFT)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Numpad6, KeyBinding(jp2, JoyPad::BTN_RIGHT)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::Numpad8, KeyBinding(jp2, JoyPad::BTN_UP)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::N, KeyBinding(jp2, JoyPad::BTN_START)));
    mKeyBindMap.insert(std::make_pair(sf::Keyboard::M, KeyBinding(jp2, JoyPad::BTN_SEL)));
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

        // Update joypads
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
            auto bindingIt = mKeyBindMap.find(event.key.code);
            if (bindingIt != mKeyBindMap.end())
            {
                auto binding = bindingIt->second;
                binding.mJoyPad->setButton(binding.mButton, event.type == sf::Event::KeyPressed);
            }
		}
	}

    // TODO: redraw only on NMI
    // Update PPU image
    if (mIsNmiOccured)
    {
        auto ppu = mConsole.getPPU();
        auto buffer = ppu->getBuffer();
        const sf::Uint8* bufPtr = reinterpret_cast<const sf::Uint8*>(&buffer[0]);
        mPpuTexture.update(bufPtr, ppu->getWidth(), ppu->getHeight(), 0, 0);
        mWindow.draw(mPpuSprite);
        mWindow.display();
        mIsNmiOccured = false;
    }
	return true;
}

void MainWindow::onNMI()
{
    mIsNmiOccured = true;
}