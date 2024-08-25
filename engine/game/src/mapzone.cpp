#include "mapzone.h"

MapZone::MapZone(const PfRectangle& rect, int z, unsigned int height) : m_rect(rect), m_z(z), m_height(height), m_inhibited(false) {}

void MapZone::shift(const PfOrientation& orientation, double scale)
{
    m_rect.shift(orientation, scale);
}
