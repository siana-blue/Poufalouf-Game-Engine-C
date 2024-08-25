#include "mapobject.h"

#include "errors.h"
#include "viewable.h"
#include "datapackage.h"
#include "mob.h"
#include "misc.h"

// MapObject

MapObject::MapObject(const string& name, const PfRectangle& rect, PfOrientation::PfOrientationValue orientation) :
AnimatedGLItem(name, 0, rect), m_z(MAP_CELL_SQUARE_HEIGHT), m_speed(0), m_speedZ(0), m_objStat(OBJSTAT_NONE), m_effects(EFFECT_NONE),
m_floating(false), m_objCode(0)
{
	changeOrientation(orientation);
}

void MapObject::move(unsigned int steps)
{
	float distance = ((steps==0)?m_speed:steps)*MAP_CELL_SIZE/MAP_STEPS_PER_CELL;

	shift(getOrientation(), distance);
	map<PfBoxType, vector<MapZone> >::iterator it;
	for (int i=0;i<ENUM_PF_BOX_TYPE_COUNT;i++)
    {
        it = m_zones_v_map.find((PfBoxType) i);
        if (it != m_zones_v_map.end())
        {
            for (unsigned int j=0, size=it->second.size();j<size;j++)
                it->second[j].shift(getOrientation(), distance);
        }
    }
	m_modified = true;
}

void MapObject::move(PfOrientation orientation, double scale)
{
	shift(orientation, scale);
	map<PfBoxType, vector<MapZone> >::iterator it;
	for (int i=0;i<ENUM_PF_BOX_TYPE_COUNT;i++)
    {
        it = m_zones_v_map.find((PfBoxType) i);
        if (it != m_zones_v_map.end())
        {
            for (unsigned int j=0, size=it->second.size();j<size;j++)
                it->second[j].shift(orientation, scale);
        }
    }
	m_modified = true;
}

PfRectangle MapObject::pathRect() const
{
	PfRectangle rtn;
	float distance = m_speed * MAP_CELL_SIZE / MAP_STEPS_PER_CELL;

    PfRectangle collisionRect(rect_x(), rect_y(), 0.0, 0.0);
    map<PfBoxType, vector<MapZone> >::const_iterator it = m_zones_v_map.find(BOX_TYPE_COLLISION);
    if (it != m_zones_v_map.end())
        collisionRect = it->second[zoneIndex(BOX_TYPE_COLLISION)].getRect();
	switch (getOrientation())
	{
		case PfOrientation::SOUTH:
			rtn = PfRectangle(collisionRect.getX(), collisionRect.getY() - distance, collisionRect.getW(), distance);
			break;
		case PfOrientation::WEST:
			rtn = PfRectangle(collisionRect.getX() - distance, collisionRect.getY(), distance, collisionRect.getH());
			break;
		case PfOrientation::NORTH:
			rtn = PfRectangle(collisionRect.getX(), collisionRect.getY() + collisionRect.getH(), collisionRect.getW(), distance);
			break;
		case PfOrientation::EAST:
			rtn = PfRectangle(collisionRect.getX() + collisionRect.getW(), collisionRect.getY(), distance, collisionRect.getH());
			break;
		default:
			break;
	}

	return rtn;
}

bool MapObject::isColliding(const MapObject& rc_object, int dz, bool borders) const
{
	PfRectangle collRect, thisCollRect;
	int collHeight = 0, thisCollHeight = 0;
	const MapZone* q_zone = rc_object.constZone(BOX_TYPE_COLLISION);
	if (q_zone != 0)
    {
        collRect = q_zone->getRect();
        collHeight = (int) q_zone->getHeight();
    }
    q_zone = constZone(BOX_TYPE_COLLISION);
    if (q_zone != 0)
    {
        thisCollRect = q_zone->getRect();
        thisCollHeight = (int) q_zone->getHeight();
    }

	collRect = PfRectangle(collRect.getX(), collRect.getY() + (m_z + dz - rc_object.getZ())*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT,
                        collRect.getW(), collRect.getH());
    thisCollRect = PfRectangle(thisCollRect.getX(), thisCollRect.getY() + dz*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT,
                        thisCollRect.getW(), thisCollRect.getH());
	if (collRect.contains(thisCollRect, borders))
	{
		if (m_z + dz <= rc_object.getZ() + collHeight && m_z + dz + (int) thisCollHeight >= rc_object.getZ())
			return true; // = pour qu'un objet tienne sur un autre
	}

	return false;
}

float MapObject::distanceBeforeCollision(const MapObject& rc_object) const
{
	float distance = m_speed * MAP_CELL_SIZE / MAP_STEPS_PER_CELL;
	float startCoord, endCoord;

	PfRectangle collRect, thisCollRect;
	int collHeight = 0, thisCollHeight = 0;
	const MapZone* q_zone = rc_object.constZone(BOX_TYPE_COLLISION);
	if (q_zone != 0)
    {
        collRect = q_zone->getRect();
        collHeight = (int) q_zone->getHeight();
    }
    q_zone = constZone(BOX_TYPE_COLLISION);
    if (q_zone != 0)
    {
        thisCollRect = q_zone->getRect();
        thisCollHeight = (int) q_zone->getHeight();
    }

	switch (getOrientation())
	{
		case PfOrientation::SOUTH:
			startCoord = thisCollRect.getY();
			endCoord = startCoord - distance;
			break;
		case PfOrientation::WEST:
			startCoord = thisCollRect.getX();
			endCoord = startCoord - distance;
			break;
		case PfOrientation::NORTH:
			startCoord = thisCollRect.getY() + thisCollRect.getH();
			endCoord = startCoord + distance;
			break;
		case PfOrientation::EAST:
			startCoord = thisCollRect.getX() + thisCollRect.getW();
			endCoord = startCoord + distance;
			break;
		default:
			startCoord = endCoord = 0.0;
			break;
	}

	if (collRect.getW() > FLOAT_MARGIN && collRect.getH() > FLOAT_MARGIN && m_z < rc_object.getZ() + (int) collHeight && m_z + (int) thisCollHeight > rc_object.getZ())
	{
		int dz = m_z - rc_object.getZ();
		collRect = PfRectangle(collRect.getX(), collRect.getY()+dz*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT, collRect.getW(), collRect.getH());

		switch (getOrientation())
		{
			case PfOrientation::SOUTH:
				if (collRect.getX() < thisCollRect.getX() + thisCollRect.getW() - FLOAT_MARGIN && collRect.getX() + collRect.getW() > thisCollRect.getX() + FLOAT_MARGIN)
					endCoord = MAX(endCoord, collRect.getY() + collRect.getH());
				break;
			case PfOrientation::WEST:
				if (collRect.getY() < thisCollRect.getY() + thisCollRect.getH() - FLOAT_MARGIN && collRect.getY() + collRect.getH() > thisCollRect.getY() + FLOAT_MARGIN)
					endCoord = MAX(endCoord, collRect.getX() + collRect.getW());
				break;
			case PfOrientation::NORTH:
				if (collRect.getX() < thisCollRect.getX() + thisCollRect.getW() - FLOAT_MARGIN && collRect.getX() + collRect.getW() > thisCollRect.getX() + FLOAT_MARGIN)
					endCoord = MIN(endCoord, collRect.getY());
				break;
			case PfOrientation::EAST:
				if (collRect.getY() < thisCollRect.getY() + thisCollRect.getH() - FLOAT_MARGIN && collRect.getY() + collRect.getH() > thisCollRect.getY() + FLOAT_MARGIN)
					endCoord = MIN(endCoord, collRect.getX());
				break;
			default:
				break;
		}
	}

	return ABS(endCoord - startCoord);
}

void MapObject::changeZ(int z)
{
	move(PfOrientation(0, -1), m_z*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT);
	m_z = z;
	map<PfBoxType, vector<MapZone> >::iterator it;
	for (int i=0;i<ENUM_PF_BOX_TYPE_COUNT;i++)
    {
        it = m_zones_v_map.find((PfBoxType) i);
        if (it != m_zones_v_map.end())
        {
            for (unsigned int j=0, size=it->second.size();j<size;j++)
                it->second[j].setZ(z);
        }
    }
	move(PfOrientation(0, 1), m_z*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT);
}

void MapObject::addSpeedZ(float vz)
{
	m_speedZ += vz;
}

int MapObject::speedZ() const
{
	return SGN(m_speedZ) * (int) (ABS(m_speedZ) + 1 - FLOAT_MARGIN);
}

MapZone* MapObject::zone(PfBoxType type, bool returnInhibited)
{
    map<PfBoxType, vector<MapZone> >::iterator it = m_zones_v_map.find(type);
    if (it == m_zones_v_map.end())
        return 0;
    if (it->second[zoneIndex(type)].isInhibited() && !returnInhibited)
        return 0;
    return &(it->second[zoneIndex(type)]);
}

const MapZone* MapObject::constZone(PfBoxType type, bool returnInhibited) const
{
    map<PfBoxType, vector<MapZone> >::const_iterator it = m_zones_v_map.find(type);
    if (it == m_zones_v_map.end())
        return 0;
    if (it->second[zoneIndex(type)].isInhibited() && !returnInhibited)
        return 0;
    return &(it->second[zoneIndex(type)]);
}

void MapObject::addZone(PfBoxType type, const MapZone& zone)
{
    MapZone z(zone);
    z.shift(PfOrientation::EAST, rect_x());
    z.shift(PfOrientation::NORTH, rect_y());
    m_zones_v_map[type].push_back(z);
    map<PfBoxType, unsigned int>::iterator it = m_zonesIndex_map.find(type);
    if (it == m_zonesIndex_map.end())
        m_zonesIndex_map[type] = 0;
}

unsigned int MapObject::zoneIndex(PfBoxType type) const
{
    map<PfBoxType, unsigned int>::const_iterator it = m_zonesIndex_map.find(type);
    if (it == m_zonesIndex_map.end())
        throw ArgumentException(__LINE__, __FILE__, string("Impossible de trouver de zone pour le type ") + textFrom(type) + ".", "type", "MapObject::zoneIndex");
    return it->second;
}

void MapObject::changeZoneIndex(PfBoxType type, unsigned int index)
{
    map<PfBoxType, unsigned int>::iterator it = m_zonesIndex_map.find(type);
    if (it == m_zonesIndex_map.end())
        return;

    if (index >= m_zones_v_map[type].size())
        throw ArgumentException(__LINE__, __FILE__,
                                string("Index non valide pour la zone ") + textFrom(type) + ". Indice demandé : " + itostr(index) +
                                ". Nombre de zones de ce type : " + itostr(m_zones_v_map[type].size()) + ".", "index", "MapObject::changeZoneIndex");
    m_zonesIndex_map[type] = index;
}

pfflag32 MapObject::activate(pfflag32 code)
{
    addObjStat(OBJSTAT_ACTIVATED);

    return EFFECT_NONE;
}

pfflag32 MapObject::deactivate()
{
    removeObjStat(OBJSTAT_ACTIVATED);

    return EFFECT_NONE;
}

bool MapObject::changeCurrentAnimation(PfAnimationStatus value, bool reset)
{
    if (AnimatedGLItem::changeCurrentAnimation(value, reset))
    {
        map<PfAnimationStatus, vector<pair<PfBoxType, unsigned int> > >::const_iterator it = m_boxAnimLinks_v_map.find(value);
        if (it != m_boxAnimLinks_v_map.end() && it->second.size() > 0)
        {
            for (unsigned int i=0, size=it->second.size();i<size;i++)
                changeZoneIndex(it->second[i].first, it->second[i].second);
        }

        return true;
    }

    return false;
}

void MapObject::update()
{
	AnimatedGLItem::update();

	updateObjStat();
	updateAnim();

	#ifndef NDEBUG
	if (g_debug)
		m_modified = true;
	#endif
}

Viewable* MapObject::generateViewable() const
{
	Viewable* p_vw = AnimatedGLItem::generateViewable();
	#ifndef NDEBUG
	if (g_debug)
    {
        PfBoxType boxType[4] = {BOX_TYPE_COLLISION, BOX_TYPE_TRIGGER, BOX_TYPE_DOOR, BOX_TYPE_ACTION};
        PfColor boxColor[4] = {PfColor::RED, PfColor::GREEN, PfColor::BROWN, PfColor::YELLOW};
        PfRectangle boxRect;
        for (int i=0;i<4;i++)
        {
            map<PfBoxType, vector<MapZone> >::const_iterator it = m_zones_v_map.find(boxType[i]);
            if (it == m_zones_v_map.end())
                continue;
            boxRect = it->second[zoneIndex(boxType[i])].getRect();
            p_vw->addViewable(new Viewable(p_vw->getName() + "_DEBUG_" + textFrom(boxType[i]), boxRect, MAX_LAYER, 0, boxColor[i], true,
                                     isCoordRelativeToBorder(), isStatic()));
        }
    }
	#endif

	return p_vw;
}

void MapObject::saveData(ofstream& r_ofs) const
{
	WRITE_ENUM(r_ofs, MapObject::SAVE_ORIENTATION);
	WRITE_INT(r_ofs, (int) getOrientation());

	WRITE_ENUM(r_ofs, MapObject::SAVE_COORD);
	WRITE_FLOAT(r_ofs, rect_x());
	WRITE_FLOAT(r_ofs, rect_y()-(m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT); // on corrige de Z car lors du chargement la méthode changeZ est appelée

	WRITE_ENUM(r_ofs, MapObject::SAVE_Z);
	WRITE_INT(r_ofs, m_z);

	WRITE_ENUM(r_ofs, MapObject::SAVE_SPEED);
	WRITE_INT(r_ofs, m_speed);
	WRITE_FLOAT(r_ofs, m_speedZ);

	WRITE_ENUM(r_ofs, MapObject::SAVE_CODEOBJ);
	WRITE_INT(r_ofs, m_objCode);

	WRITE_ENUM(r_ofs, MapObject::SAVE_END);
}

void MapObject::loadData(DataPackage& r_data)
{
	bool cnt = true;
	int section;
    while (cnt && !r_data.isOver())
    {
        section = r_data.nextEnum();
        switch (section)
        {
        case MapObject::SAVE_ORIENTATION:
            changeOrientation((PfOrientation::PfOrientationValue) r_data.nextInt());
            break;
        case MapObject::SAVE_COORD:
            move(PfOrientation(1, 0), r_data.nextFloat());
            move(PfOrientation(0, 1), r_data.nextFloat());
            break;
        case MapObject::SAVE_Z:
            changeZ(r_data.nextInt());
            break;
        case MapObject::SAVE_SPEED:
            setSpeed(r_data.nextInt());
            setSpeedZ(r_data.nextFloat());
            break;
        case MapObject::SAVE_CODEOBJ:
            m_objCode = r_data.nextInt();
            break;
        case MapObject::SAVE_END:
        default:
            cnt = false;
            break;
        }
    }

    if (section < 0 || section > MapObject::SAVE_END || r_data.isOver())
        throw PfException(__LINE__, __FILE__, "Données non valides.");

    setModified(true);
}

void MapObject::updateObjStat()
{
	// phase de nettoyage

	if (m_objStat & OBJSTAT_LANDING)
		removeObjStat(OBJSTAT_LANDING);

	// phase de mise à jour

	if (getObjCode() != 0)
        addObjStat(OBJSTAT_STANDBY);
	else if (speedZ() < 0)
		addObjStat(OBJSTAT_FALLING);
	else if (speedZ() == 0)
	{
		if (m_objStat & OBJSTAT_FALLING)
		{
			removeObjStat(OBJSTAT_FALLING);
			removeObjStat(OBJSTAT_JUMPING);
			addObjStat(OBJSTAT_LANDING);
		}
	}
	if (m_speed != 0)
        removeObjStat(OBJSTAT_STOPPED);

	// phase d'entretien

	pfflag32 stat = OBJSTAT_JUMPING | OBJSTAT_FALLING;
	if (!(m_objStat & stat))
		addObjStat(OBJSTAT_ONTHEFLOOR);
	else
		removeObjStat(OBJSTAT_ONTHEFLOOR);
}

void MapObject::updateAnim()
{
    if (getObjStat() & OBJSTAT_ACTIVATED)
        changeCurrentAnimation(ANIM_ACTIVATED);
    else if (currentAnimationStatus() == ANIM_ACTIVATED)
        changeCurrentAnimation(ANIM_DEACTIVATED);
    if (currentAnimationStatus() == ANIM_DEACTIVATED && getAnimationGroup().currentConstAnim().isOver())
        changeCurrentAnimation(ANIM_IDLE);
}

void MapObject::addObjStat(PfObjectStatus objStat)
{
	m_objStat |= objStat;
}

void MapObject::removeObjStat(PfObjectStatus objStat)
{
	m_objStat &= ~objStat;
}

void MapObject::resetObjStat()
{
	m_objStat = OBJSTAT_NONE;
}

void MapObject::addEffects(pfflag32 effects)
{
    m_effects |= effects;
}

void MapObject::resetEffects()
{
    m_effects = EFFECT_NONE;
}
