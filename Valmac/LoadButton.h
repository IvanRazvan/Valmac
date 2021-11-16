#pragma once
#include "Button.h"

class LoadButton :
    public Button
{
protected:
    void Action() override;
public:
    using Button::Button;
};

