#include "fence.h"

Fence::Fence(const string& name, const PfRectangle& rect) : MapObject(name, rect) {}

void Fence::update()
{
	MapObject::update();

	switch (getObjCode())
	{
    case 0:
        setRect(PfRectangle(rect_x(), rect_y(), MAP_CELL_SIZE, MAP_CELL_SIZE));
        break;
    case 1:
        setRect(PfRectangle(rect_x(), rect_y(), 2*MAP_CELL_SIZE, MAP_CELL_SIZE));
        break;
    case 2:
        setRect(PfRectangle(rect_x(), rect_y(), MAP_CELL_SIZE, 2*MAP_CELL_SIZE));
        break;
	}
}

void Fence::updateAnim()
{
    switch (getObjCode())
    {
    case 0:
        changeCurrentAnimation(ANIM_IDLE);
        break;
    case 1:
        changeCurrentAnimation(ANIM_MISC1);
        break;
    case 2:
        changeCurrentAnimation(ANIM_MISC2);
        break;
    }
}
