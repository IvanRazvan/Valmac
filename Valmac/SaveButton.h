#pragma once
#include "Button.h"
class SaveButton :
    public Button
{
protected:
    void Action() override;
public:
    using Button::Button;
};

