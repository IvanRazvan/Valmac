#pragma once
#include "Button.h"
class InitButton :
    public Button
{
protected:
    void Action() override;
public:
    using Button::Button;
};

