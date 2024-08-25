#include "geometry.h"

#include "errors.h"
#include "misc.h"

// PfPoint

PfPoint::PfPoint(float x, float y) : m_x(x), m_y(y) {}

void PfPoint::shift(const PfOrientation& orientation, double scale)
{
    m_x += orientation.getX() * scale;
    m_y += orientation.getY() * scale;
}

// PfPolygon

PfPolygon::PfPolygon(int count)
{
    if (count > MAX_VERTICES_PER_POLYGON)
        throw ConstructorException(__LINE__, __FILE__, string("Pas plus de ") + itostr(MAX_VERTICES_PER_POLYGON) + " points dans un polygone.", "PfPolygon");

    for (int i=0;i<count;i++)
        m_vertices_v.push_back(PfPoint(0.0, 0.0));
}

PfPolygon::PfPolygon(const vector<PfPoint>& vertices_v) : m_vertices_v(vertices_v)
{
    if (m_vertices_v.size() > MAX_VERTICES_PER_POLYGON)
        throw ConstructorException(__LINE__, __FILE__, string("Pas plus de ") + itostr(MAX_VERTICES_PER_POLYGON) + " points dans un polygone.", "PfPolygon");
}

PfPolygon::PfPolygon(const PfRectangle& rectangle)
{
    m_vertices_v.push_back(PfPoint(rectangle.getX(), rectangle.getY()));
    m_vertices_v.push_back(PfPoint(rectangle.getX() + rectangle.getAngleX()*rectangle.getW(), rectangle.getY() + rectangle.getH()));
    m_vertices_v.push_back(PfPoint(rectangle.getX() + rectangle.getW() * (1 + rectangle.getAngleX()), rectangle.getY() + rectangle.getH() * (1 + rectangle.getAngleY())));
    m_vertices_v.push_back(PfPoint(rectangle.getX() + rectangle.getW(), rectangle.getY() + rectangle.getAngleY() * rectangle.getH()));
}

void PfPolygon::addPoint(const PfPoint& point)
{
    if (m_vertices_v.size() == MAX_VERTICES_PER_POLYGON)
        throw ConstructorException(__LINE__, __FILE__, string("Pas plus de ") + itostr(MAX_VERTICES_PER_POLYGON) + " points dans un polygone.", "PfPolygon");

    m_vertices_v.push_back(point);
}

void PfPolygon::addPoint(float x, float y)
{
    if (m_vertices_v.size() == MAX_VERTICES_PER_POLYGON)
        throw ConstructorException(__LINE__, __FILE__, string("Pas plus de ") + itostr(MAX_VERTICES_PER_POLYGON) + " points dans un polygone.", "PfPolygon");

    m_vertices_v.push_back(PfPoint(x, y));
}

void PfPolygon::replacePointAt(unsigned int index, float x, float y)
{
    if (index >= m_vertices_v.size())
        throw ArgumentException(__LINE__, __FILE__, "Indice non valide.", "index", "PfPolygon::replacePointAt");

    m_vertices_v[index] = PfPoint(x, y);
}

PfPoint PfPolygon::pointAt(unsigned int index) const
{
    if (index >= m_vertices_v.size())
        throw ArgumentException(__LINE__, __FILE__, "Indice non valide.", "index", "PfPolygon::replacePointAt");

    return m_vertices_v[index];
}

int PfPolygon::count() const
{
    return m_vertices_v.size();
}

void PfPolygon::shift(const PfOrientation& orientation, double scale)
{
    for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
        m_vertices_v[i].shift(orientation, scale);
}

PfPoint PfPolygon::center() const
{
	float x = 0.0, y = 0.0;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
	{
		x += m_vertices_v[i].getX();
		y += m_vertices_v[i].getY();
	}

	return PfPoint(x/m_vertices_v.size(), y/m_vertices_v.size());
}

float PfPolygon::minX() const
{
	float rtn = 1000000000.0;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		rtn = MIN(rtn, m_vertices_v[i].getX());

	return rtn;
}

float PfPolygon::maxX() const
{
	float rtn = -100000000.0;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		rtn = MAX(rtn, m_vertices_v[i].getX());

	return rtn;
}

float PfPolygon::minY() const
{
	float rtn = 1000000000.0;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		rtn = MIN(rtn, m_vertices_v[i].getY());

	return rtn;
}

float PfPolygon::maxY() const
{
	float rtn = -10000000000.0;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		rtn = MAX(rtn, m_vertices_v[i].getY());

	return rtn;
}

// PfRectangle

PfRectangle::PfRectangle() : m_x(0), m_y(0), m_w(0), m_h(0), m_angleX(0), m_angleY(0) {}

PfRectangle::PfRectangle(float w, float h) : m_x(0), m_y(0), m_w(w), m_h(h), m_angleX(0), m_angleY(0) {}

PfRectangle::PfRectangle(float x, float y, float w, float h, float angleX, float angleY) : m_x(x), m_y(y), m_w(w), m_h(h), m_angleX(angleX), m_angleY(angleY) {}

PfPolygon PfRectangle::toPfPolygon() const
{
	float angleX = MAX(0, MIN(1, m_angleX));
	float angleY = MAX(0, MIN(1, m_angleY));

	PfPolygon polygon;
	polygon.addPoint(m_x, m_y);
	polygon.addPoint(m_x + angleX*m_w, m_y + m_h);
	polygon.addPoint(m_x + m_w + angleX*m_w, m_y + m_h + angleY*m_h);
	polygon.addPoint(m_x + m_w, m_y + angleY*m_h);

	return polygon;
}

PfPolygon PfRectangle::toTriangle(pfflag32 edge) const
{
	PfPolygon rect = toPfPolygon();
	PfPolygon tri;

	if (edge & PfOrientation::NORTH)
	{
		if (edge & PfOrientation::EAST)
		{
			tri.addPoint(rect.pointAt(1));
			tri.addPoint(rect.pointAt(2));
			tri.addPoint(rect.pointAt(3));
		}
		else if (edge & PfOrientation::WEST)
		{
			tri.addPoint(rect.pointAt(0));
			tri.addPoint(rect.pointAt(1));
			tri.addPoint(rect.pointAt(2));
		}
		else
			throw ArgumentException(__LINE__, __FILE__, "La direction n'est pas valide, directions valides : NE, NO, SE, SO.", "edge", "PfRectangle::toTriangle");
	}
	else if (edge & PfOrientation::SOUTH)
	{
		if (edge & PfOrientation::EAST)
		{
			tri.addPoint(rect.pointAt(0));
			tri.addPoint(rect.pointAt(2));
			tri.addPoint(rect.pointAt(3));
		}
		else if (edge & PfOrientation::WEST)
		{
			tri.addPoint(rect.pointAt(0));
			tri.addPoint(rect.pointAt(1));
			tri.addPoint(rect.pointAt(3));
		}
		else
			throw ArgumentException(__LINE__, __FILE__, "La direction n'est pas valide, directions valides : NE, NO, SE, SO.", "edge", "PfRectangle::toTriangle");
	}
	else
		throw ArgumentException(__LINE__, __FILE__, "La direction n'est pas valide, directions valides : NE, NO, SE, SO.", "edge", "PfRectangle::toTriangle");

	return tri;
}

bool PfRectangle::contains(const PfPoint& rc_point, bool bordersIncluded) const
{
	float margin = ((bordersIncluded)?1:-1)*FLOAT_MARGIN;

	float x = rc_point.getX();
	float y = rc_point.getY();

	if (x < m_x + m_angleX*(y-m_y) - margin || x > m_x + m_w + m_angleX*(y-m_y) + margin)
		return false;
	if (y < m_y + m_angleY*(x-m_x)- margin || y > m_y + m_h + m_angleY*(x-m_x) + margin)
		return false;
	return true;	
}

bool PfRectangle::contains(const PfRectangle& rc_rect, bool bordersIncluded) const
{
	float margin = ((bordersIncluded)?1:-1)*FLOAT_MARGIN;

	float x = rc_rect.getX();
	float y = rc_rect.getY();
	float xw = x + rc_rect.getW();
	float yh = y + rc_rect.getH();

	if (xw < m_x + m_angleX*(y-m_y) - margin || x > m_x + m_w + m_angleX*(y-m_y) + margin)
		return false;
	if (yh < m_y + m_angleY*(x-m_x) - margin || y > m_y + m_h + m_angleY*(x-m_x) + margin)
		return false;
	return true;
}

void PfRectangle::shift(const PfOrientation& orientation, double scale)
{
	m_x += orientation.getX() * scale;
	m_y += orientation.getY() * scale;
}

void PfRectangle::resize(double scale)
{
	m_x = m_x + m_w/2 * (1.0-scale);
	m_y  = m_y + m_h/2 * (1.0-scale);
	m_w *= scale;
	m_h *= scale;
}

// PfColor

PfColor::PfColor() : m_r(1.0), m_g(1.0), m_b(1.0) {}

PfColor::PfColor(float r, float g, float b) : m_r(MAX(0.0, MIN(1.0, r))), m_g(MAX(0.0, MIN(1.0, g))), m_b(MAX(0.0, MIN(1.0, b))) {}

PfColor::PfColor(PfColorValue color)
{
    switch (color)
    {
        case BLACK:
            m_r = 0.0;
            m_g = 0.0;
            m_b = 0.0;
            break;
        case WHITE:
            m_r = 1.0;
            m_g = 1.0;
            m_b = 1.0;
            break;
        case RED:
            m_r = 1.0;
            m_g = 0.0;
            m_b = 0.0;
            break;
        case GREEN:
            m_r = 0.0;
            m_g = 1.0;
            m_b = 0.0;
            break;
        case BLUE:
            m_r = 0.0;
            m_g = 0.0;
            m_b = 1.0;
            break;
    }
}

bool PfColor::operator==(const PfColor& color)
{
	if (m_r != color.getR())
		return false;
	if (m_g != color.getG())
		return false;
	if (m_b != color.getB())
		return false;

	return true;
}

bool PfColor::operator!=(const PfColor& color)
{
	return !(*this == color);
}

// PfOrientation

vector<pfflag32> PfOrientation::cardinalPoints()
{
	vector<pfflag32> ori;
	ori.push_back(PfOrientation::NORTH | PfOrientation::WEST);
	ori.push_back(PfOrientation::NORTH);
	ori.push_back(PfOrientation::NORTH | PfOrientation::EAST);
	ori.push_back(PfOrientation::EAST);
	ori.push_back(PfOrientation::EAST | PfOrientation::SOUTH);
	ori.push_back(PfOrientation::SOUTH);
	ori.push_back(PfOrientation::SOUTH | PfOrientation::WEST);
	ori.push_back(PfOrientation::WEST);

	return ori;
}

PfOrientation::PfOrientation() : m_x(0), m_y(0) {}

PfOrientation::PfOrientation(int x, int y) : m_x(x), m_y(y) {}

PfOrientation::PfOrientation(PfOrientationValue orientation)
{
	switch (orientation)
	{
		case NO_ORIENTATION:
			m_x = 0;
			m_y = 0;
			break;
		case NORTH:
			m_x = 0;
			m_y = 1;
			break;
		case EAST:
			m_x = 1;
			m_y = 0;
			break;
		case SOUTH:
			m_x = 0;
			m_y = -1;
			break;
		case WEST:
			m_x = -1;
			m_y = 0;
			break;
	}
}

PfOrientation::PfOrientation(pfflag32 orientation) : m_x(0), m_y(0)
{
	if (orientation & PfOrientation::WEST)
		m_x = -1;
	if (orientation & PfOrientation::EAST)
		m_x = 1;
	if (orientation & PfOrientation::NORTH)
		m_y = 1;
	if (orientation & PfOrientation::SOUTH)
		m_y = -1;
}

PfOrientation::PfOrientationValue PfOrientation::toValue() const
{
	PfOrientationValue rtn = NO_ORIENTATION;

	if (m_x != 0 && m_y != 0)
	{
		if (m_x > 0)
		{
			if (m_y > 0)
				rtn = EAST;
			else
				rtn = SOUTH;
		}
		else
		{
			if (m_y > 0)
				rtn = NORTH;
			else
				rtn = WEST;
		}
	}
	else if (m_x != 0 || m_y != 0)
	{
		if (m_x > 0)
			rtn = EAST;
		else if (m_x < 0)
			rtn = WEST;
		else if (m_y > 0)
			rtn = NORTH;
		else
			rtn = SOUTH;
	}

	return rtn;
}

PfOrientation::PfOrientationValue PfOrientation::oppositeValue() const
{
	PfOrientation ori(-m_x, -m_y);

	return ori.toValue();
}

pfflag32 PfOrientation::toFlag() const
{
	pfflag32 rtn = PfOrientation::NO_ORIENTATION;

	if (m_x > 0)
		rtn |= PfOrientation::EAST;
	else if (m_x < 0)
		rtn |= PfOrientation::WEST;

	if (m_y > 0)
		rtn |= PfOrientation::NORTH;
	else if (m_y < 0)
		rtn |= PfOrientation::SOUTH;

	return rtn;
}

pfflag32 PfOrientation::opposite() const
{
	PfOrientation ori(-m_x, -m_y);

	return ori.toFlag();
}

pfflag32 PfOrientation::oppositeNS() const
{
	PfOrientation ori(m_x, -m_y);

	return ori.toFlag();
}

pfflag32 PfOrientation::oppositeEW() const
{
	PfOrientation ori(-m_x, m_y);

	return ori.toFlag();
}

int PfOrientation::toInt() const
{
	int rtn = -1;

	int x, y;
	x = (m_x>0)?1:((m_x<0)?-1:0);
	y = (m_y>0)?1:((m_y<0)?-1:0);

	if (x == -1 && y == 1) // NORD-OUEST
		rtn = 0;
	else if (x == 0 && y == 1)
		rtn = 1;
	else if (x == 1 && y == 1)
		rtn = 2;
	else if (x == 1 && y == 0)
		rtn = 3;
	else if (x == 1 && y == -1)
		rtn = 4;
	else if (x == 0 && y == -1)
		rtn = 5;
	else if (x == -1 && y == -1)
		rtn = 6;
	else if (x == -1 && y == 0) // OUEST
		rtn = 7;

	return rtn;
}

bool PfOrientation::isPerpendicularTo(const PfOrientation& ori) const
{
	int scalar = m_x*ori.m_x + m_y*ori.m_y;

	return (scalar == 0);
}

bool PfOrientation::isParallelTo(const PfOrientation& ori) const
{
	int vectoZ = m_x*ori.m_y - m_y*ori.m_x;

	return (vectoZ == 0);
}

bool PfOrientation::isNS() const
{
	return (m_x == 0);
}

bool PfOrientation::isEW() const
{
	return (m_y == 0);
}

bool operator==(const PfOrientation& a, const PfOrientation& b)
{
	int ax, ay, bx, by;
	ax = (a.getX()>0)?1:((a.getX()<0)?-1:0);
	ay = (a.getY()>0)?1:((a.getY()<0)?-1:0);
	bx = (b.getX()>0)?1:((b.getX()<0)?-1:0);
	by = (b.getY()>0)?1:((b.getY()<0)?-1:0);

	return (ax == bx && ay == by);
}

bool operator!=(const PfOrientation& a, const PfOrientation& b)
{
	return !(a == b);
}
