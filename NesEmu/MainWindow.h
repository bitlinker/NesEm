#pragma once
#include <SFML/Graphics.hpp>
#include "Console.h"
#include "INmiListener.h"

class KeyBinding
{
public:
    KeyBinding(const std::shared_ptr<JoyPad>& joypad, JoyPad::Buttons button)
        : mJoyPad(joypad)
        , mButton(button)
    {
    }

public:
    // TODO: encapsulate?
    std::shared_ptr<JoyPad> mJoyPad;
    JoyPad::Buttons mButton;
};

class MainWindow : public INmiListener
{
public:
    MainWindow(Console& console);
    ~MainWindow();

    bool update();

    // INmiListener:
    virtual void onNMI();

private:
    void init();
    void initKeyBindings();

private:
    sf::RenderWindow mWindow;
    sf::Texture mPpuTexture;
    sf::Sprite mPpuSprite;

    std::map<sf::Keyboard::Key, KeyBinding> mKeyBindMap;

    Console& mConsole;

    bool mIsNmiOccured;
};

