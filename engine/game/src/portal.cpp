#include "portal.h"

Portal::Portal(const string& name, const PfRectangle& rect, PfOrientation::PfOrientationValue orientation) : MapObject(name, rect, orientation) {}

pfflag32 Portal::activate(pfflag32 code)
{
    MapObject::activate();

    MapZone* p_zone = zone(BOX_TYPE_COLLISION);
    if (p_zone != 0)
    {
        p_zone->setInhibited(true);
        setFloating(true);
    }

    return EFFECT_NONE;
}

pfflag32 Portal::deactivate()
{
    MapObject::deactivate();

    MapZone* p_zone = zone(BOX_TYPE_COLLISION, true);
    if (p_zone != 0)
    {
        p_zone->setInhibited(false);
        setFloating(false);
    }

    return EFFECT_NONE;
}
