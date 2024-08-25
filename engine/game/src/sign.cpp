#include "sign.h"

Sign::Sign(const string& name, const PfRectangle& rect) : MapObject(name, rect)
{
    addEffects(EFFECT_PRINT);
}

pfflag32 Sign::activate(pfflag32 code)
{
    if (code & ACTIVCODE_USER && code & (ACTIVCODE_NORTH | ACTIVCODE_SOUTH))
    {
        MapObject::activate();

        return EFFECT_PRINT;
    }

    return EFFECT_NONE;
}

pfflag32 Sign::deactivate()
{
    MapObject::deactivate();

    return EFFECT_PRINT;
}
