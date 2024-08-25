#include "geometry.h"

#include "errors.h"
#include "misc.h"

// PfOrientation

vector<pfflag> PfOrientation::cardinalPoints()
{
	vector<pfflag> x_v;
	x_v.push_back(PfOrientation::NORTH | PfOrientation::WEST);
	x_v.push_back(PfOrientation::NORTH);
	x_v.push_back(PfOrientation::NORTH | PfOrientation::EAST);
	x_v.push_back(PfOrientation::EAST);
	x_v.push_back(PfOrientation::EAST | PfOrientation::SOUTH);
	x_v.push_back(PfOrientation::SOUTH);
	x_v.push_back(PfOrientation::SOUTH | PfOrientation::WEST);
	x_v.push_back(PfOrientation::WEST);

	return x_v;
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
        case SOUTH_WEST:
            m_x = -1;
            m_y = -1;
            break;
        case NORTH_WEST:
            m_x = -1;
            m_y = 1;
            break;
        case NORTH_EAST:
            m_x = 1;
            m_y = 1;
            break;
        case SOUTH_EAST:
            m_x = 1;
            m_y = -1;
            break;
	}
}

PfOrientation::PfOrientation(pfflag orientation) : m_x(0), m_y(0)
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

PfOrientation::PfOrientation(PfCardinalPoint orientation)
{
	switch (orientation)
	{
        case CARDINAL_NW:
            m_x = -1;
            m_y = 1;
            break;
        case CARDINAL_N:
            m_x = 0;
            m_y = 1;
            break;
        case CARDINAL_NE:
            m_x = 1;
            m_y = 1;
            break;
        case CARDINAL_E:
            m_x = 1;
            m_y = 0;
            break;
        case CARDINAL_SE:
            m_x = 1;
            m_y = -1;
            break;
        case CARDINAL_S:
            m_x = 0;
            m_y = -1;
            break;
        case CARDINAL_SW:
            m_x = -1;
            m_y = -1;
            break;
        case CARDINAL_W:
            m_x = -1;
            m_y = 0;
            break;
	}
}

PfOrientation::PfOrientationValue PfOrientation::toValue() const
{
	PfOrientationValue x = NO_ORIENTATION;

	if (m_x != 0 && m_y != 0)
	{
		if (m_x > 0)
		{
			if (m_y > 0)
				x = EAST;
			else
				x = SOUTH;
		}
		else
		{
			if (m_y > 0)
				x = NORTH;
			else
				x = WEST;
		}
	}
	else if (m_x != 0 || m_y != 0)
	{
		if (m_x > 0)
			x = EAST;
		else if (m_x < 0)
			x = WEST;
		else if (m_y > 0)
			x = NORTH;
		else
			x = SOUTH;
	}

	return x;
}

PfOrientation::PfOrientationValue PfOrientation::toExactValue() const
{
    PfOrientationValue x = NO_ORIENTATION;

	if (m_x != 0 && m_y != 0)
	{
		if (m_x > 0)
		{
			if (m_y > 0)
				x = NORTH_EAST;
			else
				x = SOUTH_EAST;
		}
		else
		{
			if (m_y > 0)
				x = NORTH_WEST;
			else
				x = SOUTH_WEST;
		}
	}
	else if (m_x != 0 || m_y != 0)
	{
		if (m_x > 0)
			x = EAST;
		else if (m_x < 0)
			x = WEST;
		else if (m_y > 0)
			x = NORTH;
		else
			x = SOUTH;
	}

	return x;
}

PfOrientation::PfOrientationValue PfOrientation::oppositeValue() const
{
	PfOrientation ori(-m_x, -m_y);

	return ori.toValue();
}

pfflag PfOrientation::toFlag() const
{
	pfflag x = PfOrientation::NO_ORIENTATION;

	if (m_x > 0)
		x |= PfOrientation::EAST;
	else if (m_x < 0)
		x |= PfOrientation::WEST;

	if (m_y > 0)
		x |= PfOrientation::NORTH;
	else if (m_y < 0)
		x |= PfOrientation::SOUTH;

	return x;
}

PfOrientation::PfCardinalPoint PfOrientation::toCardinal() const
{
	if (m_x > 0)
	{
		if (m_y > 0)
			return PfOrientation::CARDINAL_NE;
		if (m_y == 0)
			return PfOrientation::CARDINAL_E;
		return PfOrientation::CARDINAL_SE;
	}

	if (m_x == 0)
	{
		if (m_y >= 0) // avec l'exception toujours au nord :)
			return PfOrientation::CARDINAL_N;
		return PfOrientation::CARDINAL_S;
	}

	if (m_y > 0)
		return PfOrientation::CARDINAL_NW;
	if (m_y == 0)
		return PfOrientation::CARDINAL_W;
	return PfOrientation::CARDINAL_SW;
}

PfOrientation::PfCardinalPoint PfOrientation::nextCardinal(bool clockwise) const
{
	PfOrientation::PfCardinalPoint cardO = toCardinal();

	if (!clockwise && cardO == PfOrientation::CARDINAL_NW)
		return PfOrientation::CARDINAL_W;
	if (clockwise && cardO == PfOrientation::CARDINAL_W)
		return PfOrientation::CARDINAL_NW;

	return (PfOrientation::PfCardinalPoint) (clockwise?cardO+1:cardO-1);
}

pfflag PfOrientation::opposite() const
{
	PfOrientation ori(-m_x, -m_y);

	return ori.toFlag();
}

pfflag PfOrientation::oppositeNS() const
{
	PfOrientation ori(m_x, -m_y);

	return ori.toFlag();
}

pfflag PfOrientation::oppositeEW() const
{
	PfOrientation ori(-m_x, m_y);

	return ori.toFlag();
}

PfOrientation::PfCardinalPoint PfOrientation::oppositeOnSameSide(PfOrientation::PfOrientationValue ori) const
{
	bool ns = (ori == PfOrientation::NORTH || ori == PfOrientation::SOUTH);

	switch (toCardinal())
	{
        case CARDINAL_NE:
            return (ns?CARDINAL_SE:CARDINAL_NW);
        case CARDINAL_NW:
            return (ns?CARDINAL_SW:CARDINAL_NE);
        case CARDINAL_SW:
            return (ns?CARDINAL_NW:CARDINAL_SE);
        case CARDINAL_SE:
            return (ns?CARDINAL_NE:CARDINAL_SW);
        default:
            return toCardinal();
	}

	return toCardinal(); // pour le compilateur
}

int PfOrientation::toInt() const
{
	int x_rtn = -1;

	int x, y;
	x = (m_x>0)?1:((m_x<0)?-1:0);
	y = (m_y>0)?1:((m_y<0)?-1:0);

	if (x == -1 && y == 1) // NORD-OUEST
		x_rtn = 0;
	else if (x == 0 && y == 1)
		x_rtn = 1;
	else if (x == 1 && y == 1)
		x_rtn = 2;
	else if (x == 1 && y == 0)
		x_rtn = 3;
	else if (x == 1 && y == -1)
		x_rtn = 4;
	else if (x == 0 && y == -1)
		x_rtn = 5;
	else if (x == -1 && y == -1)
		x_rtn = 6;
	else if (x == -1 && y == 0) // OUEST
		x_rtn = 7;

	return x_rtn;
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

bool PfOrientation::isNSEW() const
{
	return (m_x == 0 || m_y == 0);
}

bool PfOrientation::isNextTo(PfOrientation::PfCardinalPoint ori) const
{
	PfOrientation::PfCardinalPoint card = toCardinal();

	if (ori == CARDINAL_NW && card == CARDINAL_W)
		return true;
	if (card == CARDINAL_NW && ori == CARDINAL_W)
		return true;
	return (ABS(card-ori) == 1);
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

// PfPoint

PfPoint::PfPoint(float x, float y) : m_x(x), m_y(y) {}

void PfPoint::shift(const PfOrientation& orientation, double scale)
{
    m_x += orientation.getX() * scale;
    m_y += orientation.getY() * scale;
}

bool operator==(const PfPoint& a, const PfPoint& b)
{
    return DECIMAL_EQUAL(a.getX() - b.getX(), 0.0) && DECIMAL_EQUAL(a.getY() - b.getY(), 0.0);
}

bool operator!=(const PfPoint& a, const PfPoint& b)
{
	return !(a == b);
}

// PfPolygon

PfPolygon::PfPolygon(int count)
{
    if (count > MAX_VERTICES_PER_POLYGON)
        throw ConstructorException(__LINE__, __FILE__,
                                   string("Pas plus de ") + itostr(MAX_VERTICES_PER_POLYGON) + " points dans un polygone. Nombre de points demandés : " + itostr(count),
                                   "PfPolygon");

    for (int i=0;i<count;i++)
        m_vertices_v.push_back(PfPoint(0.0, 0.0));
}

PfPolygon::PfPolygon(const vector<PfPoint>& rc_vertices_v) : m_vertices_v(rc_vertices_v)
{
    if (m_vertices_v.size() > MAX_VERTICES_PER_POLYGON)
        throw ConstructorException(__LINE__, __FILE__,
                                   string("Pas plus de ") + itostr(MAX_VERTICES_PER_POLYGON) + " points dans un polygone. Nombre de points demandés : " + itostr(rc_vertices_v.size()),
                                   "PfPolygon");
}

PfPolygon::PfPolygon(const PfRectangle& rectangle)
{
    m_vertices_v.push_back(PfPoint(rectangle.getX(), rectangle.getY()));
    m_vertices_v.push_back(PfPoint(rectangle.getX() + rectangle.getAngleX()*rectangle.getW(), rectangle.getY() + rectangle.getH()));
    m_vertices_v.push_back(PfPoint(rectangle.getX() + rectangle.getW() * (1 + rectangle.getAngleX()), rectangle.getY() + rectangle.getH() * (1 + rectangle.getAngleY())));
    m_vertices_v.push_back(PfPoint(rectangle.getX() + rectangle.getW(), rectangle.getY() + rectangle.getAngleY() * rectangle.getH()));
}

PfPolygon::PfPolygon(const PfPolygon& triangle1, const PfPolygon& triangle2)
{
	if (triangle1.count() != 3 || triangle2.count() != 3)
		throw ConstructorException(__LINE__, __FILE__, "Les triangles passés en paramètre n'en sont pas.", "PfPolygon");

	// Refaire les schémas pour comprendre l'algo.

	m_vertices_v.push_back(triangle1.pointAt(0));

	if (triangle1.pointAt(1).getX() < triangle2.pointAt(1).getX())
		m_vertices_v.push_back(triangle1.pointAt(1));
	else
		m_vertices_v.push_back(triangle2.pointAt(1));

	if (triangle1.pointAt(2).getY() > triangle2.pointAt(2).getY() + FLOAT_MARGIN)
		m_vertices_v.push_back(triangle1.pointAt(2));
	else if (ABS(triangle1.pointAt(2).getY() - triangle2.pointAt(2).getY()) < FLOAT_MARGIN)
		m_vertices_v.push_back(triangle2.pointAt(1));
	else
		m_vertices_v.push_back(triangle2.pointAt(2));

	if (triangle1.pointAt(2).getY() < triangle2.pointAt(2).getY() - FLOAT_MARGIN)
		m_vertices_v.push_back(triangle1.pointAt(2));
	else
		m_vertices_v.push_back(triangle2.pointAt(2));
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
        throw ArgumentException(__LINE__, __FILE__, "Indice non valide : " + itostr(index) + ". Nombre de points : " + itostr(m_vertices_v.size()),
                                "index", "PfPolygon::replacePointAt");

    m_vertices_v[index] = PfPoint(x, y);
}

PfPoint PfPolygon::pointAt(unsigned int index) const
{
    if (index >= m_vertices_v.size())
        throw ArgumentException(__LINE__, __FILE__, "Indice non valide : " + itostr(index) + ". Nombre de points : " + itostr(m_vertices_v.size()),
                                "index", "PfPolygon::pointAt");

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

void PfPolygon::mirror(PfOrientation::PfCardinalPoint ori, double scale)
{
	PfPoint c = center();
	bool isV = (PfOrientation(ori).isNS() || !PfOrientation(ori).isNSEW());
	bool isH = (PfOrientation(ori).isEW() || !PfOrientation(ori).isNSEW());

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		replacePointAt(i, (isH?((scale+1)*c.getX() - pointAt(i).getX()):pointAt(i).getX()),
                       (isV?((scale+1)*c.getY() - pointAt(i).getY()):pointAt(i).getY()));
}

void PfPolygon::rearrangeClockwise()
{
	vector<PfPoint> vertices_v;
	vector<int> tmp_v, tmp2_v;
	float minX = MAX_NUMBER, minY = MAX_NUMBER;
	int tmpI = -1;

	// Prendre comme premier point le point le plus bas à gauche (tous les points de Y mini sont sélectionnés et le plus à gauche est retenu).

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
	{
	    if (DECIMAL_INFEQUAL(m_vertices_v[i].getX(), minX))
		{
			minX = m_vertices_v[i].getX();
			tmp_v.push_back(i);
			// purge de tous les points mis dans le vecteur mais de X supérieurs
			for (unsigned int j=0, size2=tmp_v.size()-1;j<size2;j++)
			{
			    if (DECIMAL_SUP(m_vertices_v[tmp_v[j]].getX(), minX))
				{
					tmp_v.erase(tmp_v.begin()+j);
					j--;
				}
			}
		}
	}
	for (unsigned int i=0, size=tmp_v.size();i<size;i++)
	{
	    if (DECIMAL_INF(m_vertices_v[tmp_v[i]].getY(), minY))
		{
			minY = m_vertices_v[tmp_v[i]].getY();
			tmpI = tmp_v[i];
		}
	}

	if (tmpI < 0) // aucun point dans le polygone ?
		return;

	vertices_v.push_back(m_vertices_v[tmpI]);
	m_vertices_v.erase(m_vertices_v.begin()+tmpI);

	// Tous les points de Y strictement supérieurs au premier sont sélectionnés, et celui de X mini est retenu. S'il y a égalité, le point de Y le plus faible est pris.
	// Ceci est réalisé pour chaque point par rapport au précédent, jusqu'à ce qu'il n'y ait plus de points de Y supérieur au précédent.

	do
	{
		minX = minY = MAX_NUMBER;
		tmp_v.clear();
		tmp2_v.clear();
		tmpI = -1;
		for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		{
		    if (DECIMAL_SUP(m_vertices_v[i].getY(), vertices_v[vertices_v.size()-1].getY()))
				tmp_v.push_back(i);
		}
		for (unsigned int i=0, size=tmp_v.size();i<size;i++)
		{
		    if (DECIMAL_INFEQUAL(m_vertices_v[tmp_v[i]].getX(), minX))
			{
				minX = m_vertices_v[tmp_v[i]].getX();
				tmp2_v.push_back(tmp_v[i]);
				// purge de tous les points mis dans le vecteur mais de X supérieurs
				for (unsigned int j=0, size2=tmp2_v.size()-1;j<size2;j++)
				{
				    if (DECIMAL_SUP(m_vertices_v[tmp2_v[j]].getX(), minX))
					{
						tmp2_v.erase(tmp2_v.begin()+j);
						j--;
					}
				}
			}
		}
		for (unsigned int i=0, size=tmp2_v.size();i<size;i++)
		{
		    if (DECIMAL_INF(m_vertices_v[tmp2_v[i]].getY(), minY))
			{
				minY = m_vertices_v[tmp2_v[i]].getY();
				tmpI = tmp2_v[i];
			}
		}

		if (tmpI < 0) // plus de point ?
			break;

		vertices_v.push_back(m_vertices_v[tmpI]);
		m_vertices_v.erase(m_vertices_v.begin()+tmpI);

	} while (tmp_v.size() > 0);

	// Tous les points restants sont sélectionnés, et celui de Y le plus haut est retenu. S'il y a égalité, le point de X le plus faible est pris.
	// Ceci est fait jusqu'à avoir parcouru tous les points de ce polygone.

	do
	{
		minX = MAX_NUMBER;
		minY = -MAX_NUMBER; // haha! Ici minY devient max mais je ne veux pas créer de nouvelle variable. Haha :) (en relisant ce commentaire bien plus tard je constate que je suis d'humeur badine souvent)
		tmp_v.clear();
		tmpI = -1;
		for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		{
		    if (DECIMAL_SUPEQUAL(m_vertices_v[i].getY(), minY))
			{
				minY = m_vertices_v[i].getY();
				tmp_v.push_back(i);
				// purge de tous les points mis dans le vecteur mais de Y inférieurs
				for (unsigned int j=0, size2=tmp_v.size()-1;j<size2;j++)
				{
				    if (DECIMAL_INF(m_vertices_v[tmp_v[j]].getY(), minY))
					{
						tmp_v.erase(tmp_v.begin()+j);
						j--;
					}
				}
			}
		}
		for (unsigned int i=0, size=tmp_v.size();i<size;i++)
		{
		    if (DECIMAL_INF(m_vertices_v[tmp_v[i]].getX(), minX))
			{
				minX = m_vertices_v[tmp_v[i]].getX();
				tmpI = tmp_v[i];
			}
		}

		if (tmpI < 0) // plus de point ?
			break;

		vertices_v.push_back(m_vertices_v[tmpI]);
		m_vertices_v.erase(m_vertices_v.begin()+tmpI);

	} while (m_vertices_v.size() > 0);

	m_vertices_v = vertices_v;
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
	float x = MAX_NUMBER;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		x = MIN(x, m_vertices_v[i].getX());

	return x;
}

float PfPolygon::maxX() const
{
	float x = -MAX_NUMBER;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		x = MAX(x, m_vertices_v[i].getX());

	return x;
}

float PfPolygon::minY() const
{
	float x = MAX_NUMBER;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		x = MIN(x, m_vertices_v[i].getY());

	return x;
}

float PfPolygon::maxY() const
{
	float x = -MAX_NUMBER;

	for (unsigned int i=0, size=m_vertices_v.size();i<size;i++)
		x = MAX(x, m_vertices_v[i].getY());

	return x;
}

float PfPolygon::maxW() const
{
	return maxX() - minX();
}

float PfPolygon::maxH() const
{
	return maxY() - minY();
}

// PfRectangle

PfRectangle::PfRectangle() : m_x(0.0), m_y(0.0), m_w(0.0), m_h(0.0), m_angleX(0.0), m_angleY(0.0) {}

PfRectangle::PfRectangle(float w, float h) : m_x(0.0), m_y(0.0), m_w(w), m_h(h), m_angleX(0.0), m_angleY(0.0) {}

PfRectangle::PfRectangle(float x, float y, float w, float h, float angleX, float angleY) : m_x(x), m_y(y), m_w(w), m_h(h), m_angleX(angleX), m_angleY(angleY) {}

PfPolygon PfRectangle::toPfPolygon() const
{
	PfPolygon x_polygon;
	x_polygon.addPoint(m_x, m_y);
	x_polygon.addPoint(m_x + m_angleX*m_w, m_y + m_h);
	x_polygon.addPoint(m_x + m_w + m_angleX*m_w, m_y + m_h + m_angleY*m_h);
	x_polygon.addPoint(m_x + m_w, m_y + m_angleY*m_h);

	return x_polygon;
}

PfPoint PfRectangle::center() const
{
	return toPfPolygon().center();
}

PfPolygon PfRectangle::toTriangle(PfOrientation::PfCardinalPoint edge) const
{
	PfPolygon rect = toPfPolygon();
	PfPolygon x_tri;

	pfflag32 ori = PfOrientation(edge).toFlag();

	if (ori & PfOrientation::NORTH)
	{
		if (ori & PfOrientation::EAST)
		{
			x_tri.addPoint(rect.pointAt(1));
			x_tri.addPoint(rect.pointAt(2));
			x_tri.addPoint(rect.pointAt(3));
		}
		else if (ori & PfOrientation::WEST)
		{
			x_tri.addPoint(rect.pointAt(0));
			x_tri.addPoint(rect.pointAt(1));
			x_tri.addPoint(rect.pointAt(2));
		}
		else
			throw ArgumentException(__LINE__, __FILE__, "La direction n'est pas valide, directions valides : NE, NO, SE, SO.", "edge", "PfRectangle::toTriangle");
	}
	else if (ori & PfOrientation::SOUTH)
	{
		if (ori & PfOrientation::EAST)
		{
			x_tri.addPoint(rect.pointAt(0));
			x_tri.addPoint(rect.pointAt(2));
			x_tri.addPoint(rect.pointAt(3));
		}
		else if (ori & PfOrientation::WEST)
		{
			x_tri.addPoint(rect.pointAt(0));
			x_tri.addPoint(rect.pointAt(1));
			x_tri.addPoint(rect.pointAt(3));
		}
		else
			throw ArgumentException(__LINE__, __FILE__, "La direction n'est pas valide, directions valides : NE, NO, SE, SO.", "edge", "PfRectangle::toTriangle");
	}
	else
		throw ArgumentException(__LINE__, __FILE__, "La direction n'est pas valide, directions valides : NE, NO, SE, SO.", "edge", "PfRectangle::toTriangle");

	return x_tri;
}

bool PfRectangle::contains(const PfPoint& rc_point, bool bordersIncluded) const
{
	float margin = ((bordersIncluded)?1:-1)*FLOAT_MARGIN;

	float x = rc_point.getX();
	float y = rc_point.getY();

	if (x < m_x + m_angleX*(y-m_y) - margin || x > m_x + m_w + m_angleX*(y-m_y) + margin)
		return false;
	if (y < m_y + m_angleY*(x-m_x) - margin || y > m_y + m_h + m_angleY*(x-m_x) + margin)
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
        case YELLOW:
            m_r = 1.0;
            m_g = 1.0;
            m_b = 0.0;
            break;
        case CYAN:
            m_r = 0.0;
            m_g = 1.0;
            m_b = 1.0;
            break;
        case MAGENTA:
            m_r = 1.0;
            m_g = 0.0;
            m_b = 1.0;
            break;
        case GRAY:
            m_r = 0.5;
            m_g = 0.5;
            m_b = 0.5;
            break;
        case BROWN:
            m_r = 0.5;
            m_g = 0.0;
            m_b = 0.0;
            break;
    }
}

bool PfColor::operator==(const PfColor& color)
{
    if (!DECIMAL_EQUAL(m_r, color.getR()))
		return false;
    if (!DECIMAL_EQUAL(m_g, color.getG()))
		return false;
    if (!DECIMAL_EQUAL(m_b, color.getB()))
		return false;

	return true;
}

bool PfColor::operator!=(const PfColor& color)
{
	return !(*this == color);
}
