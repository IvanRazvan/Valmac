#pragma once
#include "Button.h"
class RunButton :
    public Button
{
protected:
    void Action() override;
public:
    using Button::Button;
};

