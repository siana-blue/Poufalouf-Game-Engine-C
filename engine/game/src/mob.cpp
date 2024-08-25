#include "mob.h"

#include "errors.h"
#include "map.h"

Mob::Mob(const string& name, const PfRectangle& rect) : MapObject(name, rect, PfOrientation::SOUTH) {}

void Mob::update()
{
	MapObject::update();

	if (getObjStat() & OBJSTAT_STANDBY)
		setSpeed(0);
}

PfReturnCode Mob::processInstruction()
{
	if (getObjStat() & OBJSTAT_STANDBY)
		return RETURN_NOTHING;

	PfInstruction instruction = getInstruction();
	PfReturnCode rtnCode = RETURN_OK;

	try
	{
		switch (instruction)
		{
			case INSTRUCTION_LEFT:
				if (changeOrientation(PfOrientation::WEST) || (getObjStat() & OBJSTAT_JUMPING) || !(getObjStat() & OBJSTAT_STOPPED))
                    setSpeed(2);
				break;
			case INSTRUCTION_UP:
				if (changeOrientation(PfOrientation::NORTH) || (getObjStat() & OBJSTAT_JUMPING) || !(getObjStat() & OBJSTAT_STOPPED))
                    setSpeed(2);
				break;
			case INSTRUCTION_RIGHT:
				if (changeOrientation(PfOrientation::EAST) || (getObjStat() & OBJSTAT_JUMPING) || !(getObjStat() & OBJSTAT_STOPPED))
                    setSpeed(2);
				break;
			case INSTRUCTION_DOWN:
				if (changeOrientation(PfOrientation::SOUTH) || (getObjStat() & OBJSTAT_JUMPING) || !(getObjStat() & OBJSTAT_STOPPED))
                    setSpeed(2);
				break;
			case INSTRUCTION_STOP:
			    if (instructionValues().nextInt() == 1)
                    addObjStat(OBJSTAT_STOPPED);
				setSpeed(0);
				break;
			case INSTRUCTION_JUMP:
				if (!(getObjStat() & OBJSTAT_JUMPING))
					setSpeedZ(MAP_CELL_SQUARE_HEIGHT/4);
				break;
            case INSTRUCTION_CHECK:
                switch (instructionValues().nextInt())
                {
                case Map::MAP_GROUND_WATER:
                    if (getZ() == 0)
                    {
                        if (!(getObjStat() & OBJSTAT_ONWATER))
                        {
                            addObjStat(OBJSTAT_ONWATER);
                            addObjStat(OBJSTAT_STANDBY);
                        }
                    }
                    break;
                default:
                    break;
                }
                break;
			default:
				rtnCode = RETURN_NOTHING;
				break;
		}
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Impossible d'appliquer l'instruction ") + textFrom(instruction) + ".", e);
	}

	return rtnCode;
}

void Mob::updateObjStat()
{
    if ((getObjStat() & OBJSTAT_ONWATER) && !(getObjStat() & OBJSTAT_STANDBY))
        addObjStat(OBJSTAT_DEAD);
	else if (speedZ() > 0)
		addObjStat(OBJSTAT_JUMPING);
	else if (speedZ() < -MAP_CELL_SQUARE_HEIGHT/2)
		removeObjStat(OBJSTAT_JUMPING);

	MapObject::updateObjStat();
}

void Mob::updateAnim()
{
	MapObject::updateAnim();

	if (getObjStat() & OBJSTAT_ONWATER)
        changeCurrentAnimation(ANIM_DROWNING);

	PfAnimationStatus status = currentAnimationStatus();

    #define SBY_T_SIZE 3
    PfAnimationStatus sby_t[SBY_T_SIZE] = {ANIM_GETUP1, ANIM_GETUP2, ANIM_DROWNING}; // liste des animations pour lesquelles gérer le standby
    for (int i=0;i<SBY_T_SIZE;i++)
    {
        if (status == sby_t[i] && getAnimationGroup().currentConstAnim().isOver())
        {
            switch (status)
            {
            case ANIM_GETUP1:
            case ANIM_GETUP2:
                changeCurrentAnimation(ANIM_IDLE);
                break;
            default:
                break;
            }

            removeObjStat(OBJSTAT_STANDBY);
        }
    }

	if (getObjStat() & OBJSTAT_STANDBY)
		return;

	if (speedZ() < -MAP_CELL_SQUARE_HEIGHT/2)
		changeCurrentAnimation(ANIM_FALLING2);
	else if (getObjStat() & OBJSTAT_JUMPING)
		changeCurrentAnimation(ANIM_JUMPING);
	else if (getObjStat() & OBJSTAT_FALLING)
		changeCurrentAnimation(ANIM_FALLING1);
	else if (getObjStat() & OBJSTAT_LANDING)
	{
		if (status == ANIM_FALLING1)
			changeCurrentAnimation(ANIM_GETUP1);
		else if (status == ANIM_FALLING2)
		{
			changeCurrentAnimation(ANIM_GETUP2);
			addObjStat(OBJSTAT_STANDBY);
		}
		else
			changeCurrentAnimation(ANIM_IDLE);
	}
	else if (getSpeed() != 0)
		changeCurrentAnimation(ANIM_WALKING);
	else
	{
	    #define ST_T_SIZE 2
        PfAnimationStatus st_t[ST_T_SIZE] = {ANIM_GETUP1, ANIM_DROWNING}; // liste des animations à ne pas interrompre à tout bout de champ
		bool ok = true;
		for (int i=0;i<ST_T_SIZE;i++)
		{
			if (st_t[i] == status)
				ok = false;
		}
		if (ok)
			changeCurrentAnimation(ANIM_IDLE);
	}
}
