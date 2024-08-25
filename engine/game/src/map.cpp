#include "map.h"

#include <cmath>
#include "misc.h"
#include "errors.h"
#include "viewable.h"
#include "glimage.h"
#include "glfunc.h"

// Cell

Cell::Cell(unsigned int row, unsigned int col, unsigned int terrainIndex, const PfMapTextureSet& rc_textureSet, int z) :
    PolygonGLItem(string("Case (") + itostr(row) + ";" + itostr(col) + ")",
                  MAP_LAYER + (MAP_MAX_LINES_COUNT-row)*MAP_MAX_HEIGHT*MAP_STEPS_PER_CELL + z,
                  PfRectangle((col-1)*MAP_CELL_SIZE, (row-1)*MAP_CELL_SIZE + (z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE, MAP_CELL_SIZE, MAP_CELL_SIZE),
                  rc_textureSet.terrainId(0), rectFromTextureIndex(terrainIndex)),
                  m_row(row), m_col(col), m_terrainIndex(terrainIndex), m_z(z), m_slopeOri(PfOrientation::CARDINAL_E),
                  m_slopeValues(pair<int, int>(0, 0)), mq_textureSet(&rc_textureSet)
{
	mq_nextCells_t = new const Cell*[8];
	for (int i=0;i<8;i++)
		mq_nextCells_t[i] = 0;
}

Cell::Cell(DataPackage& r_data, const PfMapTextureSet& rc_textureSet) : PolygonGLItem(""), m_row(0), m_col(0), m_terrainIndex(0), m_z(0),
m_slopeOri(PfOrientation::CARDINAL_E), m_slopeValues(pair<int, int>(0, 0)), mq_textureSet(&rc_textureSet)
{
	try
	{
		mq_nextCells_t = new const Cell*[8];
		for (int i=0;i<8;i++)
			mq_nextCells_t[i] = 0;

		int section, val, val2;

		bool cnt = true;
		while (cnt && !r_data.isOver())
		{
			section = r_data.nextEnum();
			switch (section)
			{
			    case SAVE_COORD:
                    m_row = r_data.nextUInt();
                    m_col = r_data.nextUInt();
                    break;
                case SAVE_TERRAIN:
                    m_terrainIndex = r_data.nextUInt();
                    break;
                case SAVE_Z:
                    m_z = r_data.nextInt();
                    break;
                case SAVE_SLOPE:
                    m_slopeOri = (PfOrientation::PfCardinalPoint) r_data.nextInt();
                    val = r_data.nextInt();
                    val2 = r_data.nextInt();
                    m_slopeValues = pair<int, int>(val, val2);
                    break;
                case SAVE_END:
                default:
                    cnt = false;
                    break;
			}
		}

		if (section < 0 || section > SAVE_END || r_data.isOver())
			throw ConstructorException(__LINE__, __FILE__, "Données non valides.", "Cell");

		setName(string("Case (") + itostr(m_row) + ";" + itostr(m_col) + ")");
		setLayer(MAP_LAYER + (MAP_MAX_LINES_COUNT-m_row)*MAP_MAX_HEIGHT*MAP_STEPS_PER_CELL + m_z);

		float dy = m_slopeValues.first * MAP_Z_STEP_SIZE;
		float dy2 = m_slopeValues.second * MAP_Z_STEP_SIZE;
		bool sw = (m_slopeOri == PfOrientation::CARDINAL_S || m_slopeOri == PfOrientation::CARDINAL_SW || m_slopeOri == PfOrientation::CARDINAL_W);
		bool nw = (m_slopeOri == PfOrientation::CARDINAL_W || m_slopeOri == PfOrientation::CARDINAL_NW || m_slopeOri == PfOrientation::CARDINAL_N);
		bool ne = (m_slopeOri == PfOrientation::CARDINAL_N || m_slopeOri == PfOrientation::CARDINAL_NE || m_slopeOri == PfOrientation::CARDINAL_E);
		bool se = (m_slopeOri == PfOrientation::CARDINAL_E || m_slopeOri == PfOrientation::CARDINAL_SE || m_slopeOri == PfOrientation::CARDINAL_S);

		vector<PfPoint> vertices_v;
		vertices_v.push_back(PfPoint((m_col-1)*MAP_CELL_SIZE, (m_row-1)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (sw?dy:(ne?dy2:0))));
		vertices_v.push_back(PfPoint((m_col-1)*MAP_CELL_SIZE, (m_row)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (nw?dy:(se?dy2:0))));
		if (PfOrientation(m_slopeOri).isNSEW() || nw || se)
			vertices_v.push_back(PfPoint((m_col)*MAP_CELL_SIZE, (m_row)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (ne?dy:(sw?dy2:0))));
		if (PfOrientation(m_slopeOri).isNSEW() || sw || ne)
			vertices_v.push_back(PfPoint((m_col)*MAP_CELL_SIZE, (m_row-1)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (se?dy:(nw?dy2:0))));
		setPolygon(PfPolygon(vertices_v));

		setTextureIndex(rc_textureSet.terrainId(0));
		if (PfOrientation(m_slopeOri).isNSEW())
			setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex));
		else if (nw || se)
			setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex).toTriangle(PfOrientation::CARDINAL_NW));
		else
			setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex).toTriangle(PfOrientation::CARDINAL_SW));
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de construire cet objet à partir du DataPackage.", "Cell", e);
	}
}

Cell::~Cell()
{
    delete [] mq_nextCells_t;
}

void Cell::assignNeighbour(const Cell* q_cell, PfOrientation::PfCardinalPoint ori)
{
	mq_nextCells_t[ori] = q_cell; // ori étant toujours inférieur à 8
}

void Cell::modify(unsigned int terrainIndex, int z, int spreadingLayer)
{
	m_terrainIndex = terrainIndex;
	setLayer(getLayer() + z - m_z);
	m_z = z;

	float dy = m_slopeValues.first * MAP_Z_STEP_SIZE;
	float dy2 = m_slopeValues.second * MAP_Z_STEP_SIZE;
	bool sw = (m_slopeOri == PfOrientation::CARDINAL_S || m_slopeOri == PfOrientation::CARDINAL_SW || m_slopeOri == PfOrientation::CARDINAL_W);
	bool nw = (m_slopeOri == PfOrientation::CARDINAL_W || m_slopeOri == PfOrientation::CARDINAL_NW || m_slopeOri == PfOrientation::CARDINAL_N);
	bool ne = (m_slopeOri == PfOrientation::CARDINAL_N || m_slopeOri == PfOrientation::CARDINAL_NE || m_slopeOri == PfOrientation::CARDINAL_E);
	bool se = (m_slopeOri == PfOrientation::CARDINAL_E || m_slopeOri == PfOrientation::CARDINAL_SE || m_slopeOri == PfOrientation::CARDINAL_S);

	vector<PfPoint> vertices_v;
	vertices_v.push_back(PfPoint((m_col-1)*MAP_CELL_SIZE, (m_row-1)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (sw?dy:(ne?dy2:0))));
	vertices_v.push_back(PfPoint((m_col-1)*MAP_CELL_SIZE, (m_row)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (nw?dy:(se?dy2:0))));
	if (PfOrientation(m_slopeOri).isNSEW() || nw || se)
		vertices_v.push_back(PfPoint((m_col)*MAP_CELL_SIZE, (m_row)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (ne?dy:(sw?dy2:0))));
	if (PfOrientation(m_slopeOri).isNSEW() || sw || ne)
		vertices_v.push_back(PfPoint((m_col)*MAP_CELL_SIZE, (m_row-1)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (se?dy:(nw?dy2:0))));
	setPolygon(PfPolygon(vertices_v));

	if (PfOrientation(m_slopeOri).isNSEW())
		setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex));
	else if (nw || se)
		setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex).toTriangle(PfOrientation::CARDINAL_NW));
	else
		setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex).toTriangle(PfOrientation::CARDINAL_SW));

	m_modified = true;
}

void Cell::changeSlope(PfOrientation::PfCardinalPoint slopeOri, int deltaSlope, bool forceOri)
{
	bool isOpposite = (slopeOri == PfOrientation(PfOrientation(m_slopeOri).opposite()).toCardinal());
	bool isTheSame = (slopeOri == m_slopeOri);

	if (forceOri || !isOpposite)
		m_slopeOri = slopeOri;
	if (isOpposite)
	{
		if (forceOri)
		{
			int tmp = m_slopeValues.second;
			m_slopeValues.second = m_slopeValues.first;
			m_slopeValues.first = tmp + deltaSlope;
		}
		else
			m_slopeValues.second += deltaSlope;
	}
	else if (!isTheSame)
	{
		m_slopeValues.second = 0;
		m_slopeValues.first = deltaSlope;
	}
	else
		m_slopeValues.first += deltaSlope;

	if (PfOrientation(m_slopeOri).isNSEW())
		m_slopeValues.second = 0;
	if (ABS(m_slopeValues.first) > MAP_CELL_SQUARE_HEIGHT)
		m_slopeValues.first = SGN(m_slopeValues.first)*MAP_CELL_SQUARE_HEIGHT;
	if (ABS(m_slopeValues.second) > MAP_CELL_SQUARE_HEIGHT)
		m_slopeValues.second = SGN(m_slopeValues.second)*MAP_CELL_SQUARE_HEIGHT;

	float dy = m_slopeValues.first * MAP_Z_STEP_SIZE;
	float dy2 = m_slopeValues.second * MAP_Z_STEP_SIZE;
	bool sw = (m_slopeOri == PfOrientation::CARDINAL_S || m_slopeOri == PfOrientation::CARDINAL_SW || m_slopeOri == PfOrientation::CARDINAL_W);
	bool nw = (m_slopeOri == PfOrientation::CARDINAL_W || m_slopeOri == PfOrientation::CARDINAL_NW || m_slopeOri == PfOrientation::CARDINAL_N);
	bool ne = (m_slopeOri == PfOrientation::CARDINAL_N || m_slopeOri == PfOrientation::CARDINAL_NE || m_slopeOri == PfOrientation::CARDINAL_E);
	bool se = (m_slopeOri == PfOrientation::CARDINAL_E || m_slopeOri == PfOrientation::CARDINAL_SE || m_slopeOri == PfOrientation::CARDINAL_S);

	vector<PfPoint> vertices_v;
	vertices_v.push_back(PfPoint((m_col-1)*MAP_CELL_SIZE, (m_row-1)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (sw?dy:(ne?dy2:0))));
	vertices_v.push_back(PfPoint((m_col-1)*MAP_CELL_SIZE, (m_row)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (nw?dy:(se?dy2:0))));
	if (PfOrientation(m_slopeOri).isNSEW() || nw || se)
		vertices_v.push_back(PfPoint((m_col)*MAP_CELL_SIZE, (m_row)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (ne?dy:(sw?dy2:0))));
	if (PfOrientation(m_slopeOri).isNSEW() || sw || ne)
		vertices_v.push_back(PfPoint((m_col)*MAP_CELL_SIZE, (m_row-1)*MAP_CELL_SIZE + (m_z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE + (se?dy:(nw?dy2:0))));
	setPolygon(PfPolygon(vertices_v));

	if (PfOrientation(m_slopeOri).isNSEW())
		setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex));
	else if (nw || se)
		setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex).toTriangle(PfOrientation::CARDINAL_NW));
	else
		setTextCoordPolygon(rectFromTextureIndex(m_terrainIndex).toTriangle(PfOrientation::CARDINAL_SW));

	m_modified = true;
}

int Cell::cliffHeightWith(PfOrientation::PfCardinalPoint ori) const
{
	const Cell* q_nextCell = mq_nextCells_t[ori];
	if (!q_nextCell)
		return 0;

	return m_z - q_nextCell->getZ();
}

bool Cell::isSpreadingLayerHigher(const Cell& rc_cell) const
{
	if (mq_textureSet->spreadingLayer(m_terrainIndex) < mq_textureSet->spreadingLayer(rc_cell.getTerrainIndex()))
		return false;
	if (mq_textureSet->spreadingLayer(m_terrainIndex) > mq_textureSet->spreadingLayer(rc_cell.getTerrainIndex()))
		return true;
	if (m_terrainIndex > rc_cell.getTerrainIndex())
		return true;
	return false;
}

int Cell::minZ() const
{
	return m_z + MIN(0, MIN(m_slopeValues.first, m_slopeValues.second));
}

int Cell::maxZ() const
{
	return m_z + MAX(0, MAX(m_slopeValues.first, m_slopeValues.second));
}

int Cell::minZIn(const PfRectangle& rect, int z, bool roundUp) const
{
	PfRectangle r(rect);
	r.shift(PfOrientation::SOUTH, (z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE);

	// on calcule le pas (entre 0 et MAP_STEPS_PER_CELL-1) sur les deux axes x et y aux deux extrémités du rectangle.
	int minStepX = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getX()-(m_col-1)*MAP_CELL_SIZE+FLOAT_MARGIN)/MAP_STEP_SIZE));
	int maxStepX = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getX()+rect.getW()-(m_col-1)*MAP_CELL_SIZE-FLOAT_MARGIN)/MAP_STEP_SIZE));
	int minStepY = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getY()-(m_row-1)*MAP_CELL_SIZE+FLOAT_MARGIN)/MAP_STEP_SIZE));
	int maxStepY = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getY()+rect.getH()-(m_row-1)*MAP_CELL_SIZE-FLOAT_MARGIN)/MAP_STEP_SIZE));

	int rtnZ = MAX_NUMBER;
	for (int x=minStepX;x<=maxStepX;x++)
	{
		for (int y=minStepY;y<=maxStepY;y++)
			rtnZ = MIN(rtnZ, zAt(x, y, roundUp));
	}

	return (rtnZ==MAX_NUMBER)?0:rtnZ;
}

int Cell::maxZIn(const PfRectangle& rect, int z, bool roundUp) const
{
	PfRectangle r(rect);
	r.shift(PfOrientation::SOUTH, (z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE);

	// on calcule le pas (entre 0 et MAP_STEPS_PER_CELL-1) sur les deux axes x et y aux deux extrémités du rectangle.
	int minStepX = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getX()-(m_col-1)*MAP_CELL_SIZE+FLOAT_MARGIN)/MAP_STEP_SIZE));
	int maxStepX = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getX()+rect.getW()-(m_col-1)*MAP_CELL_SIZE-FLOAT_MARGIN)/MAP_STEP_SIZE));
	int minStepY = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getY()-(m_row-1)*MAP_CELL_SIZE+FLOAT_MARGIN)/MAP_STEP_SIZE));
	int maxStepY = MIN(MAP_STEPS_PER_CELL-1, (int) (MAX(0, r.getY()+rect.getH()-(m_row-1)*MAP_CELL_SIZE-FLOAT_MARGIN)/MAP_STEP_SIZE));

	int rtnZ = -MAX_NUMBER;
	for (int x=minStepX;x<=maxStepX;x++)
	{
		for (int y=minStepY;y<=maxStepY;y++)
			rtnZ = MAX(rtnZ, zAt(x, y, roundUp));
	}

	return (rtnZ==-MAX_NUMBER)?0:rtnZ;
}

int Cell::zAt(PfOrientation::PfCardinalPoint ori, bool roundUp) const
{
	if (ori == m_slopeOri)
		return m_z + m_slopeValues.first;
	if (ori == PfOrientation(PfOrientation(m_slopeOri).opposite()).toCardinal())
		return m_z + m_slopeValues.second; // pour une pente NSEW ce sera + 0.
	if (PfOrientation(ori).isNSEW()) // mais ni l'orientation de la pente ni son opposée.
	{
		if (PfOrientation(m_slopeOri).isNSEW() || PfOrientation(m_slopeOri).isNextTo(ori))
		{
			if (m_slopeValues.first != 0 && m_slopeValues.first % 2 != 0)
				return m_z + round((float) m_slopeValues.first/2 + FLOAT_MARGIN*(roundUp?1:-1));
			return m_z + m_slopeValues.first/2;
		}
		if (m_slopeValues.second != 0 && m_slopeValues.second % 2 != 0)
			return m_z + round((float) m_slopeValues.second/2 + FLOAT_MARGIN*(roundUp?1:-1));
		return m_z + m_slopeValues.second/2;
	}
	if (PfOrientation(m_slopeOri).isNextTo(ori))
		return m_z + m_slopeValues.first; // c'est le cas où ori n'est pas NSEW
	return m_z; // c'est le cas pour une ori non NSEW et non égale à m_slopeOri ou son opposée.
}

int Cell::zAt(unsigned int xStep, unsigned int yStep, bool roundUp) const
{
	if (xStep >= MAP_STEPS_PER_CELL || yStep >= MAP_STEPS_PER_CELL)
		throw ArgumentException(__LINE__, __FILE__, "Coordonnées non valides pour le calcul de l'altitude sur une case.", "xStep/yStep", "Cell::zAt");

	if (m_slopeValues.first == 0 && m_slopeValues.second == 0)
		return m_z;
	int rtnZ = zAt(PfOrientation::CARDINAL_SW, roundUp);
	switch (m_slopeOri)
	{
        case PfOrientation::CARDINAL_E:
            return m_z + (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
        case PfOrientation::CARDINAL_W:
            return m_z + (int) ROUND_FLOAT((float) (MAP_STEPS_PER_CELL-xStep)/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
        case PfOrientation::CARDINAL_N:
            return m_z + (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
        case PfOrientation::CARDINAL_S:
            return m_z + (int) ROUND_FLOAT((float) (MAP_STEPS_PER_CELL-yStep)/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
        case PfOrientation::CARDINAL_NW: // pour comprendre à partir de là, tracer une grille MAP_STEPS_PER_CELL² et imaginer que chaque diagonale est une marche, un pas étant donc 1/MAP_STEPS_PER_CELL.
            if (yStep <= xStep) // on trace la droite y=f(x) pour séparer les deux parties.
            {
                rtnZ += (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
                rtnZ -= (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
            }
            else
            {
                rtnZ -= (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
                rtnZ += (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
            }
            return rtnZ;
        case PfOrientation::CARDINAL_NE:
            if (yStep <= MAP_STEPS_PER_CELL - xStep)
            {
                rtnZ -= (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
                rtnZ -= (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
            }
            else
            {
                rtnZ -= m_slopeValues.second;
                rtnZ += (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
                rtnZ += (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
            }
            return rtnZ;
        case PfOrientation::CARDINAL_SE:
            if (yStep >= xStep)
            {
                rtnZ -= (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
                rtnZ += (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
            }
            else
            {
                rtnZ += (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
                rtnZ -= (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
            }
            return rtnZ;
        case PfOrientation::CARDINAL_SW:
            if (yStep >= MAP_STEPS_PER_CELL - xStep)
            {
                rtnZ -= m_slopeValues.first;
                rtnZ += (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
                rtnZ += (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.second, roundUp);
            }
            else
            {
                rtnZ -= (int) ROUND_FLOAT((float) xStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
                rtnZ -= (int) ROUND_FLOAT((float) yStep/MAP_STEPS_PER_CELL*m_slopeValues.first, roundUp);
            }
            return rtnZ;
	}

	return rtnZ; // pour le compilateur
}

int Cell::minZAt(PfOrientation::PfCardinalPoint ori) const
{
	int rtnZ = MAX_NUMBER, z;
	for (unsigned int i=PfOrientation::CARDINAL_NW;i<=PfOrientation::CARDINAL_W;i++)
	{
		if (PfOrientation((PfOrientation::PfCardinalPoint) i).toFlag() & PfOrientation(ori).toFlag())
		{
			z = zAt((PfOrientation::PfCardinalPoint) i);
			if (rtnZ > z)
				rtnZ = z;
		}
	}

	return rtnZ;
}

int Cell::maxZAt(PfOrientation::PfCardinalPoint ori) const
{
	int rtnZ = -MAX_NUMBER, z;
	for (unsigned int i=PfOrientation::CARDINAL_NW;i<=PfOrientation::CARDINAL_W;i++)
	{
		if (PfOrientation((PfOrientation::PfCardinalPoint) i).toFlag() & PfOrientation(ori).toFlag())
		{
			z = zAt((PfOrientation::PfCardinalPoint) i, true);
			if (rtnZ < z)
				rtnZ = z;
		}
	}

	return rtnZ;
}

PfPolygon Cell::reconstructedPolygon() const
{
	PfPolygon poly(getPolygon());
	if (poly.count() == 3)
	{
		PfPolygon triangle(3);

		// génération du triangle complémentaire
		if (m_slopeOri == PfOrientation::CARDINAL_NW || m_slopeOri == PfOrientation::CARDINAL_SE)
		{
			triangle.replacePointAt(0, poly_minX(), poly_minY());
			triangle.replacePointAt(1, poly_maxX(), poly_minY() + MAP_CELL_SIZE);
			triangle.replacePointAt(2, poly_maxX(), poly_minY() + (m_slopeOri==PfOrientation::CARDINAL_SE?m_slopeValues.first:m_slopeValues.second)*MAP_Z_STEP_SIZE);
		}
		else
		{
			triangle.replacePointAt(0, poly_minX(), poly_maxY());
			triangle.replacePointAt(1, poly_maxX(), poly_maxY() + (m_slopeOri==PfOrientation::CARDINAL_NE?m_slopeValues.first:m_slopeValues.second)*MAP_Z_STEP_SIZE);
			triangle.replacePointAt(2, poly_maxX(), poly_maxY() - MAP_CELL_SIZE);
		}
		poly = PfPolygon(getPolygon(), triangle);
	}

	return poly;
}

bool Cell::isFlat() const
{
    return (m_slopeValues.first == 0 && m_slopeValues.second == 0);
}

Viewable* Cell::generateViewable() const
{
    if (getZ() == 0 && isFlat())
        return new Viewable(m_name);

	PfPolygon triangle(3);
	PfPolygon cellPoly = getPolygon();

	// Texture principale de cette case

	Viewable* p_rtn = PolygonGLItem::generateViewable();
	if (cellPoly.count() == 3) // c'est balo mais on ne peut pas utiliser la méthode reconstructedPolygon ici car on a besoin du triangle intermédiaire pour générer une image
	{
		// génération du triangle complémentaire
		if (m_slopeOri == PfOrientation::CARDINAL_NW || m_slopeOri == PfOrientation::CARDINAL_SE)
		{
			triangle.replacePointAt(0, poly_minX(), poly_minY());
			triangle.replacePointAt(1, poly_maxX(), poly_minY() + MAP_CELL_SIZE);
			triangle.replacePointAt(2, poly_maxX(), poly_minY() + (m_slopeOri==PfOrientation::CARDINAL_SE?m_slopeValues.first:m_slopeValues.second)*MAP_Z_STEP_SIZE);
		}
		else
		{
			triangle.replacePointAt(0, poly_minX(), poly_maxY());
			triangle.replacePointAt(1, poly_maxX(), poly_maxY() + (m_slopeOri==PfOrientation::CARDINAL_NE?m_slopeValues.first:m_slopeValues.second)*MAP_Z_STEP_SIZE);
			triangle.replacePointAt(2, poly_maxX(), poly_maxY() - MAP_CELL_SIZE);
		}
		p_rtn->addImage(new GLImage(triangle, mq_textureSet->terrainId(0),
						rectFromTextureIndex(m_terrainIndex).toTriangle((m_slopeOri==PfOrientation::CARDINAL_NW||m_slopeOri==PfOrientation::CARDINAL_SE)?PfOrientation::CARDINAL_SE:PfOrientation::CARDINAL_NE)));
		cellPoly = PfPolygon(getPolygon(), triangle);
	}

	// Falaise

	const Cell* q_nextCell = mq_nextCells_t[PfOrientation::CARDINAL_S];
	const Cell* q_leftNextCell = mq_nextCells_t[PfOrientation::CARDINAL_W];
	const Cell* q_rightNextCell = mq_nextCells_t[PfOrientation::CARDINAL_E];
	if (q_nextCell == 0 || q_nextCell->minZAt(PfOrientation::CARDINAL_N) < maxZAt(PfOrientation::CARDINAL_S))
	{
		int dz = (q_nextCell==0)?minZAt(PfOrientation::CARDINAL_S):(minZAt(PfOrientation::CARDINAL_S)-q_nextCell->minZAt(PfOrientation::CARDINAL_N));
		// le triangle de haut de falaise est géré à part, donc on prend toujours le minimum au sud
		int leftdz = (q_leftNextCell==0)?minZAt(PfOrientation::CARDINAL_S):(minZAt(PfOrientation::CARDINAL_S)-q_leftNextCell->zAt(PfOrientation::CARDINAL_SE));
		int rightdz = (q_rightNextCell==0)?minZAt(PfOrientation::CARDINAL_S):(minZAt(PfOrientation::CARDINAL_S)-q_rightNextCell->zAt(PfOrientation::CARDINAL_SW));
		int dzSlope = zAt(PfOrientation::CARDINAL_SE) - zAt(PfOrientation::CARDINAL_SW);

		// triangle de haut de falaise en cas de pente

		if (dzSlope > 0)
		{
			triangle.replacePointAt(0, cellPoly.minX(), cellPoly.minY());
			triangle.replacePointAt(1, cellPoly.minX() + cellPoly.maxW(), cellPoly.minY() + dzSlope*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT);
			triangle.replacePointAt(2, cellPoly.minX() + cellPoly.maxW(), cellPoly.minY());
		}
		else if (dzSlope < 0)
		{
			triangle.replacePointAt(0, cellPoly.minX(), cellPoly.minY());
			triangle.replacePointAt(1, cellPoly.minX(), cellPoly.minY() + ABS(dzSlope)*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT);
			triangle.replacePointAt(2, cellPoly.minX() + cellPoly.maxW(), cellPoly.minY());
		}
		if (dzSlope != 0)
		{
			p_rtn->addImage(new GLImage(triangle, mq_textureSet->cliffTexture(m_terrainIndex),
										rectFromTextureIndex(mq_textureSet->cliffIndex(m_terrainIndex), TERRAIN_CELLS_COUNT, TERRAIN_CELLS_COUNT,
														1.0, (float) ABS(dzSlope)/MAP_CELL_SQUARE_HEIGHT, 0.0, 0.0).toTriangle((dzSlope>0)?PfOrientation::CARDINAL_SE:PfOrientation::CARDINAL_SW)));
			p_rtn->addImage(new GLImage(PfRectangle(cellPoly.minX(), cellPoly.minY(), cellPoly.maxW(), ABS(dzSlope)*MAP_Z_STEP_SIZE), mq_textureSet->cliffTexture(m_terrainIndex),
										rectFromTextureIndex(mq_textureSet->cliffIndex(m_terrainIndex, (dzSlope>0)?PfOrientation::EAST:PfOrientation::WEST),
																	TERRAIN_CELLS_COUNT, TERRAIN_CELLS_COUNT, 1.0, (float) ABS(dzSlope)/MAP_CELL_SQUARE_HEIGHT)));
		}

		// falaise droite

		for (int k=0;k<=(dz-1)/MAP_CELL_SQUARE_HEIGHT;k++) // dz-1 pour éviter de faire une couche de 0 d'épaisseur si dz est un multiple de MAP_CELL_SQUARE_HEIGHT
		{
			int tempdz = MIN(MAP_CELL_SQUARE_HEIGHT, dz-k*MAP_CELL_SQUARE_HEIGHT);
			p_rtn->addImage(new GLImage(PfRectangle(cellPoly.minX(), cellPoly.minY() - MIN(dz*MAP_Z_STEP_SIZE, (k+1)*MAP_CELL_SIZE),
													cellPoly.maxW(), tempdz*MAP_Z_STEP_SIZE),
										mq_textureSet->cliffTexture(m_terrainIndex),
										rectFromTextureIndex(mq_textureSet->cliffIndex(m_terrainIndex), TERRAIN_CELLS_COUNT, TERRAIN_CELLS_COUNT,
																	1.0, (float) tempdz/MAP_CELL_SQUARE_HEIGHT, 0.0, 1.0-(float) tempdz/MAP_CELL_SQUARE_HEIGHT)));
			// bords de la falaise
			tempdz = MIN(MAP_CELL_SQUARE_HEIGHT, leftdz-k*MAP_CELL_SQUARE_HEIGHT);
			if (tempdz > 0)
			{
				p_rtn->addImage(new GLImage(PfRectangle(cellPoly.minX(), cellPoly.minY() - MIN(leftdz*MAP_Z_STEP_SIZE, (k+1)*MAP_CELL_SIZE),
														cellPoly.maxW(), MIN(tempdz*MAP_Z_STEP_SIZE, MAP_CELL_SIZE)),
											mq_textureSet->cliffTexture(m_terrainIndex),
											rectFromTextureIndex(mq_textureSet->cliffIndex(m_terrainIndex, PfOrientation::WEST), TERRAIN_CELLS_COUNT, TERRAIN_CELLS_COUNT,
																		1.0, (float) tempdz/MAP_CELL_SQUARE_HEIGHT, 0.0, 1.0-(float) tempdz/MAP_CELL_SQUARE_HEIGHT)));
			}
			tempdz = MIN(MAP_CELL_SQUARE_HEIGHT, rightdz-k*MAP_CELL_SQUARE_HEIGHT);
			if (tempdz > 0)
			{
				p_rtn->addImage(new GLImage(PfRectangle(cellPoly.minX(), cellPoly.minY() - MIN(rightdz*MAP_Z_STEP_SIZE, (k+1)*MAP_CELL_SIZE),
														cellPoly.maxW(), MIN(tempdz*MAP_Z_STEP_SIZE, MAP_CELL_SIZE)),
											mq_textureSet->cliffTexture(m_terrainIndex),
											rectFromTextureIndex(mq_textureSet->cliffIndex(m_terrainIndex, PfOrientation::EAST), TERRAIN_CELLS_COUNT, TERRAIN_CELLS_COUNT,
																		1.0, (float) tempdz/MAP_CELL_SQUARE_HEIGHT, 0.0, 1.0-(float) tempdz/MAP_CELL_SQUARE_HEIGHT)));
			}
		}
		// recouvrement en bas de falaise
		if (q_nextCell != 0 && mq_textureSet->cliffFloorTexture(q_nextCell->getTerrainIndex()) != 0)
		{
				p_rtn->addImage(new GLImage(PfRectangle(cellPoly.minX(), cellPoly.minY() - dz*MAP_Z_STEP_SIZE, cellPoly.maxW(), MAP_CELL_SIZE),
											mq_textureSet->cliffFloorTexture(q_nextCell->getTerrainIndex()),
											rectFromTextureIndex(mq_textureSet->cliffFloorIndex(q_nextCell->getTerrainIndex()))));
				if (q_leftNextCell != 0 && q_leftNextCell->getZ() <= q_nextCell->getZ())
				{
					p_rtn->addImage(new GLImage(PfRectangle(cellPoly.minX()-MAP_CELL_SIZE, cellPoly.minY() - dz*MAP_Z_STEP_SIZE, cellPoly.maxW(), MAP_CELL_SIZE),
												mq_textureSet->cliffFloorTexture(q_nextCell->getTerrainIndex()),
												rectFromTextureIndex(mq_textureSet->cliffFloorIndex(q_nextCell->getTerrainIndex(), PfOrientation::WEST))));
				}
				if (q_rightNextCell != 0 && q_rightNextCell->getZ() <= q_nextCell->getZ())
				{
					p_rtn->addImage(new GLImage(PfRectangle(cellPoly.minX()+MAP_CELL_SIZE, cellPoly.minY() - dz*MAP_Z_STEP_SIZE, cellPoly.maxW(), MAP_CELL_SIZE),
												mq_textureSet->cliffFloorTexture(q_nextCell->getTerrainIndex()),
												rectFromTextureIndex(mq_textureSet->cliffFloorIndex(q_nextCell->getTerrainIndex(), PfOrientation::EAST))));
				}
		}
	}

	// Bordures extérieures de cette case

	PfOrientation o;
	bool ok;
	if (mq_textureSet->cliffBorderTexture(m_terrainIndex) > 0 || mq_textureSet->borderTexture(m_terrainIndex) > 0)
	{
		for (int ori = PfOrientation::CARDINAL_NW;ori<=PfOrientation::CARDINAL_W;ori++)
		{
			ok = false;
			q_nextCell = mq_nextCells_t[ori];
			o = PfOrientation((PfOrientation::PfCardinalPoint) ori);
			if (q_nextCell == 0)
				continue;
			if (q_nextCell->zAt(PfOrientation(o.opposite()).toCardinal()) < zAt((PfOrientation::PfCardinalPoint) ori, true))
			{
			    bool nsew = false;
				if (o.isNS() || o.isEW())
				{
					ok = true;
					nsew = true;
				}
				else
				{
					if (ori == PfOrientation::CARDINAL_NW)
					{
						q_leftNextCell = mq_nextCells_t[PfOrientation::CARDINAL_W];
						q_rightNextCell = mq_nextCells_t[PfOrientation::CARDINAL_N];
						if (q_leftNextCell->zAt(PfOrientation::CARDINAL_NE) < zAt(PfOrientation::CARDINAL_NW) && q_rightNextCell->zAt(PfOrientation::CARDINAL_SW) < zAt(PfOrientation::CARDINAL_NW))
							ok = true;
					}
					else if (ori == PfOrientation::CARDINAL_NE)
					{
						q_leftNextCell = mq_nextCells_t[PfOrientation::CARDINAL_N];
						q_rightNextCell = mq_nextCells_t[PfOrientation::CARDINAL_E];
						if (q_leftNextCell->zAt(PfOrientation::CARDINAL_SE) < zAt(PfOrientation::CARDINAL_NE) && q_rightNextCell->zAt(PfOrientation::CARDINAL_NW) < zAt(PfOrientation::CARDINAL_NE))
							ok = true;
					}
					else if (ori == PfOrientation::CARDINAL_SE)
					{
						q_leftNextCell = mq_nextCells_t[PfOrientation::CARDINAL_S];
						q_rightNextCell = mq_nextCells_t[PfOrientation::CARDINAL_E];
						if (q_leftNextCell->zAt(PfOrientation::CARDINAL_NE) < zAt(PfOrientation::CARDINAL_SE) && q_rightNextCell->zAt(PfOrientation::CARDINAL_SW) < zAt(PfOrientation::CARDINAL_SE))
							ok = true;
					}
					else if (ori == PfOrientation::CARDINAL_SW)
					{
						q_leftNextCell = mq_nextCells_t[PfOrientation::CARDINAL_W];
						q_rightNextCell = mq_nextCells_t[PfOrientation::CARDINAL_S];
						if (q_leftNextCell->zAt(PfOrientation::CARDINAL_SE) < zAt(PfOrientation::CARDINAL_SW) && q_rightNextCell->zAt(PfOrientation::CARDINAL_NW) < zAt(PfOrientation::CARDINAL_SW))
							ok = true;
					}
				}
				if (ok)
				{
					if (mq_textureSet->cliffBorderTexture(m_terrainIndex) > 0)
					{
						PfPolygon tmpPoly(cellPoly);
						if ((o.toFlag() & PfOrientation::EAST) || (o.toFlag() & PfOrientation::WEST))
						{
							tmpPoly.mirror(PfOrientation::CARDINAL_E);
							tmpPoly.rearrangeClockwise();
						}
						tmpPoly.shift(o, MAP_CELL_SIZE);
						p_rtn->addImage(new GLImage(tmpPoly, mq_textureSet->cliffBorderTexture(m_terrainIndex),
													rectFromTextureIndex(mq_textureSet->cliffBorderIndex(m_terrainIndex, (PfOrientation::PfCardinalPoint) ori))));
					}
					if (mq_textureSet->borderTexture(m_terrainIndex) > 0)
					{
						if (nsew) // bordures intérieures de cette case
						{
							PfOrientation::PfCardinalPoint oriL, oriR;
							oriL = (PfOrientation::PfCardinalPoint) (ori - 1); // ori ne peut pas être NW.
							oriR = (PfOrientation::PfCardinalPoint) (ori==PfOrientation::CARDINAL_W?PfOrientation::CARDINAL_NW:ori+1);
							// Je dois mettre tout ce if bizarre car sinon problème de bordures internes pour les pentes triangles se rejoignant.
							// Et si je faisais le tri avec des true dans le test global, des incohérences arrivent forcément avec les bordures internes.
							// C'est pas clair ce que je dis, mais si plus tard je suis en train de relire ceci pour optimiser, crois moi, ceci est nécessaire, à moins que je ne trouve un
							// algorithme magique.
							if (zAt(oriL) != q_nextCell->zAt(PfOrientation(oriL).oppositeOnSameSide(PfOrientation(ori).toValue()))
									|| zAt(oriR) != q_nextCell->zAt(PfOrientation(oriR).oppositeOnSameSide(PfOrientation(ori).toValue())))
								p_rtn->addImage(new GLImage(cellPoly, mq_textureSet->borderTexture(m_terrainIndex),
												rectFromTextureIndex(mq_textureSet->borderIndex(m_terrainIndex, PfOrientation((PfOrientation::PfCardinalPoint) ori).toValue()))));
						}
					}
				}
			}
		}
	}

	// Débordements

	vector<pair<const Cell*, GLImage*> > p_spreadings_v;
	for (unsigned int ori = PfOrientation::CARDINAL_NW;ori<=PfOrientation::CARDINAL_W;ori++)
	{
		o = PfOrientation((PfOrientation::PfCardinalPoint) ori);
		q_nextCell = mq_nextCells_t[ori];
		if (q_nextCell != 0 && q_nextCell->getZ() == m_z && q_nextCell->getTerrainIndex() != m_terrainIndex)
		{
			if (q_nextCell->isSpreadingLayerHigher(*this))
			{
				// classer les images dans l'ordre des priorités
				ok = false;
				for (vector<pair<const Cell*, GLImage*> >::iterator it=p_spreadings_v.begin();it!=p_spreadings_v.end();++it)
				{
					if ((*it).first->isSpreadingLayerHigher(*q_nextCell))
					{
						p_spreadings_v.insert(it, pair<const Cell*, GLImage*>(q_nextCell,
																			new GLImage(getPolygon(), mq_textureSet->spreadingTexture(q_nextCell->getTerrainIndex()),
																						rectFromTextureIndex(mq_textureSet->spreadingIndex(q_nextCell->getTerrainIndex(),
																																(PfOrientation::PfCardinalPoint) ori)))));
						ok = true;
						break;
					}
				}
				if (!ok)
					p_spreadings_v.push_back(pair<const Cell*, GLImage*>(q_nextCell,
																	new GLImage(getPolygon(), mq_textureSet->spreadingTexture(q_nextCell->getTerrainIndex()),
																						rectFromTextureIndex(mq_textureSet->spreadingIndex(q_nextCell->getTerrainIndex(),
																																(PfOrientation::PfCardinalPoint) ori)))));
			}
		}
	}
	for (unsigned int i=0, size=p_spreadings_v.size();i<size;i++)
		p_rtn->addImage(p_spreadings_v[i].second);

	return p_rtn;
}

void Cell::saveData(ofstream& r_ofs) const
{
	WRITE_ENUM(r_ofs, SAVE_COORD);
	WRITE_UINT(r_ofs, m_row);
	WRITE_UINT(r_ofs, m_col);

	WRITE_ENUM(r_ofs, SAVE_TERRAIN);
	WRITE_UINT(r_ofs, m_terrainIndex);

	WRITE_ENUM(r_ofs, SAVE_Z);
	WRITE_INT(r_ofs, m_z);

	WRITE_ENUM(r_ofs, SAVE_SLOPE);
	WRITE_INT(r_ofs, m_slopeOri);
	WRITE_INT(r_ofs, m_slopeValues.first);
	WRITE_INT(r_ofs, m_slopeValues.second);

	WRITE_ENUM(r_ofs, SAVE_END);
}

// Map

int Map::layerAt(const PfRectangle& rc_rect, int z)
{
    int layer = MAP_LAYER;

    // correction de l'ordonnée du rectangle par la hauteur z
    float y = rc_rect.getY() - (z-MAP_CELL_SQUARE_HEIGHT)*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT + FLOAT_MARGIN;

    // ajout de tous les layers liés aux cases situées sur les rangées plus au Nord
    int row = (int) (y/MAP_CELL_SIZE) + 1; // rangée
    layer += (MAP_MAX_LINES_COUNT-row) * MAP_MAX_HEIGHT * MAP_STEPS_PER_CELL;

    // ajout des layers liés à l'avancée selon -Y sur la case de l'objet
    int steps = (int) ((1.0 - DECIMAL_MODULO(y, MAP_CELL_SIZE) + FLOAT_MARGIN) * MAP_STEPS_PER_CELL);
    layer += (steps-1) * MAP_MAX_HEIGHT;

    // ajout de la hauteur
    layer += z;

    // enfin, ajout d'une unité pour que l'objet soit au dessus de la case en cas de conflit
    layer++;

    return layer;
}

Map::Map(unsigned int rows, unsigned int columns, const string& texName) :
    GLItem(MAP_NAME, MAP_LAYER), m_rowsCount(rows), m_columnsCount(columns), m_seed(rand()), m_textureSet(texName), m_groundType(Map::MAP_GROUND_FLOOR)
{
	if (rows == 0 || columns == 0 || rows > MAP_MAX_LINES_COUNT || columns > MAP_MAX_LINES_COUNT)
		throw ConstructorException(__LINE__, __FILE__, string("Dimensions invalides pour la map : rows = ") + itostr(rows) + " col = " + itostr(columns) + ".", "Map");

	mp_cells_t2 = new Cell**[rows];
	for (unsigned int i=0;i<rows;i++)
	{
		mp_cells_t2[i] = new Cell*[columns];
		for (unsigned int j=0;j<columns;j++)
			mp_cells_t2[i][j] = new Cell(i+1, j+1, 0, m_textureSet);
	}
	for (unsigned int i=0;i<rows;i++)
	{
		for (unsigned int j=0;j<columns;j++)
		{
			for (int ori=PfOrientation::CARDINAL_NW;ori<=PfOrientation::CARDINAL_W;ori++)
				mp_cells_t2[i][j]->assignNeighbour(nextConstCell(i+1, j+1, PfOrientation((PfOrientation::PfCardinalPoint) ori).toFlag()), (PfOrientation::PfCardinalPoint) ori);
		}
	}
	m_objectsNames_v_t2 = new vector<string>*[rows];
	for (unsigned int i=0;i<rows;i++)
		m_objectsNames_v_t2[i] = new vector<string>[columns];
}

Map::Map(DataPackage& r_data) : GLItem(MAP_NAME, MAP_LAYER), m_rowsCount(1), m_columnsCount(1), m_seed(0), m_textureSet(""), m_groundType(Map::MAP_GROUND_FLOOR)
{
	try
	{
		int section;
		unsigned int n;

		bool cnt = true;
		while (cnt && !r_data.isOver())
		{
			section = r_data.nextEnum();
			switch (section)
			{
                case SAVE_DIM:
                    m_rowsCount = r_data.nextUInt();
                    m_columnsCount = r_data.nextUInt();
                    mp_cells_t2 = new Cell**[m_rowsCount];
                    for (unsigned int i=0;i<m_rowsCount;i++)
                        mp_cells_t2[i] = new Cell*[m_columnsCount];
                    break;
                case SAVE_SEED:
                    m_seed = r_data.nextUInt();
                    break;
                case SAVE_TEXTURE:
                    m_textureSet = PfMapTextureSet(r_data.nextString());
                    break;
                case SAVE_CELLS:
                    for (unsigned int i=0;i<m_rowsCount;i++)
                    {
                        for (unsigned int j=0;j<m_columnsCount;j++)
                            mp_cells_t2[i][j] = new Cell(r_data, m_textureSet);
                    }
                    break;
                case SAVE_GROUNDTYPE:
                    m_groundType = (Map::MapGroundType) r_data.nextEnum();
                    break;
                case SAVE_MAPLINKS:
                    n = r_data.nextUInt();
                    for (unsigned int i=0;i<n;i++)
                        m_mapLinks_v.push_back(r_data.nextString());
                    break;
                case SAVE_SCRIPT:
                    n = r_data.nextUInt();
                    for (unsigned int i=0;i<n;i++)
                        m_scriptEntries_v.push_back(r_data.nextString());
                    break;
                case SAVE_END:
                default:
                    cnt = false;
                    break;
			}
		}

		if (section < 0 || section > SAVE_END || r_data.isOver())
			throw ConstructorException(__LINE__, __FILE__, "Données non valides.", "Map");

		for (unsigned int i=0;i<m_rowsCount;i++)
		{
			for (unsigned int j=0;j<m_columnsCount;j++)
			{
				for (int ori=PfOrientation::CARDINAL_NW;ori<=PfOrientation::CARDINAL_W;ori++)
					mp_cells_t2[i][j]->assignNeighbour(nextConstCell(i+1, j+1, PfOrientation((PfOrientation::PfCardinalPoint) ori).toFlag()), (PfOrientation::PfCardinalPoint) ori);
			}
		}
		m_objectsNames_v_t2 = new vector<string>*[m_rowsCount];
		for (unsigned int i=0;i<m_rowsCount;i++)
			m_objectsNames_v_t2[i] = new vector<string>[m_columnsCount];
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de construire cet objet à partir du DataPackage.", "Map", e);
	}
}

Map::~Map()
{
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
			delete mp_cells_t2[i][j];
		delete [] mp_cells_t2[i];
	}
	delete [] mp_cells_t2;
	for (unsigned int i=0;i<m_rowsCount;i++)
		delete [] m_objectsNames_v_t2[i];
	delete [] m_objectsNames_v_t2;
}

const Cell* Map::cell(unsigned int row, unsigned int col) const
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("ligne non valide : ") + itostr(row) + " sur " + itostr(m_rowsCount) + ".", "row", "Map::cell");

	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("colonne non valide : ") + itostr(col) + " sur " + itostr(m_columnsCount) + ".", "col", "Map::cell");

	return mp_cells_t2[row-1][col-1];
}

const Cell* Map::cell(pair<unsigned int, unsigned int> coord) const
{
	return cell(coord.first, coord.second);
}

vector<pair<unsigned int, unsigned int> > Map::cellsCoord(const PfRectangle& rect, int z)
{
	vector<pair<unsigned int, unsigned int> > rtn_v;

	float x1 = MAX(0, rect.getX()) + FLOAT_MARGIN;
	float x2 = MIN(MAP_CELL_SIZE*m_columnsCount, rect.getX() + rect.getW()) - FLOAT_MARGIN;
	float y1 = MAX(0, rect.getY()-(float) (z-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE) + FLOAT_MARGIN;
	float y2 = MIN(MAP_CELL_SIZE*m_rowsCount, rect.getY() + rect.getH()-(float) (z-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE) - FLOAT_MARGIN;

	for (unsigned int i=(unsigned int) (y1/MAP_CELL_SIZE+FLOAT_MARGIN)+1, size=(unsigned int) (y2/MAP_CELL_SIZE-FLOAT_MARGIN)+1;i<=size;i++)
	{
		for (unsigned int j=(unsigned int) (x1/MAP_CELL_SIZE+FLOAT_MARGIN)+1, size2=(unsigned int) (x2/MAP_CELL_SIZE-FLOAT_MARGIN)+1;j<=size2;j++)
			rtn_v.push_back(pair<unsigned int, unsigned int>(i, j));
	}

	return rtn_v;
}

Cell* Map::nextCell(unsigned int row, unsigned int col, pfflag orientation)
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("ligne non valide : ") + itostr(row) + " sur " + itostr(m_rowsCount) + ".", "row", "Map::nextCell");

	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("colonne non valide : ") + itostr(col) + " sur " + itostr(m_columnsCount) + ".", "col", "Map::nextCell");

	int r = row, c = col;
	if (orientation & PfOrientation::NORTH)
		r += 1;
	if (orientation & PfOrientation::EAST)
		c += 1;
	if (orientation & PfOrientation::SOUTH)
		r -= 1;
	if (orientation & PfOrientation::WEST)
		c -= 1;

	if (r <= 0 || c <= 0 || (unsigned int) r > m_rowsCount || (unsigned int) c > m_columnsCount)
		return 0;
	return mp_cells_t2[r-1][c-1];
}

const Cell* Map::nextConstCell(unsigned int row, unsigned int col, pfflag orientation) const
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("ligne non valide : ") + itostr(row) + " sur " + itostr(m_rowsCount) + ".", "row", "Map::nextCell");

	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("colonne non valide : ") + itostr(col) + " sur " + itostr(m_columnsCount) + ".", "col", "Map::nextCell");

	int r = row, c = col;
	if (orientation & PfOrientation::NORTH)
		r += 1;
	if (orientation & PfOrientation::EAST)
		c += 1;
	if (orientation & PfOrientation::SOUTH)
		r -= 1;
	if (orientation & PfOrientation::WEST)
		c -= 1;

	if (r <= 0 || c <= 0 || (unsigned int) r > m_rowsCount || (unsigned int) c > m_columnsCount)
		return 0;
	return mp_cells_t2[r-1][c-1];
}

void Map::changeCell(unsigned int row, unsigned int col, int terrainIndex, int z)
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("ligne non valide : ") + itostr(row) + " sur " + itostr(m_rowsCount) + ".", "row", "Map::changeCell");
	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("colonne non valide : ") + itostr(col) + " sur " + itostr(m_columnsCount) + ".", "col", "Map::changeCell");

	mp_cells_t2[row-1][col-1]->modify(terrainIndex, (z>=0)?z:mp_cells_t2[row-1][col-1]->getZ());

	m_modified = true;
}

void Map::changeCellSlope(unsigned int row, unsigned int col, PfOrientation::PfCardinalPoint slopeOri, int deltaSlope, bool forceOri)
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("ligne non valide : ") + itostr(row) + " sur " + itostr(m_rowsCount) + ".", "row", "Map::changeCellSlope");
	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("colonne non valide : ") + itostr(col) + " sur " + itostr(m_columnsCount) + ".", "col", "Map::changeCellSlope");

	mp_cells_t2[row-1][col-1]->changeSlope(slopeOri, deltaSlope, forceOri);

	m_modified = true;
}

vector<string> Map::objectsOnCell(pair<unsigned int, unsigned int> coord_pair) const
{
	if (coord_pair.first == 0 || coord_pair.first > m_rowsCount || coord_pair.second == 0 || coord_pair.second > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées invalides : ") + itostr(coord_pair.first) + ";" + itostr(coord_pair.second)  + ".", "coord_pair", "Map::objectsOnCell");

	vector<string> rtn_v;

	for (unsigned int i=0, size=m_objectsNames_v_t2[coord_pair.first-1][coord_pair.second-1].size();i<size;i++)
		rtn_v.push_back(m_objectsNames_v_t2[coord_pair.first-1][coord_pair.second-1][i]);

	return rtn_v;
}

void Map::addObject(MapObject& r_object, unsigned int row, unsigned int col)
{
	int z = 0;
	if (row == 0 || col == 0)
		z = r_object.getZ();

	unsigned int r, c;
	if (row != 0 && col != 0)
	{
		r = row;
		c = col;
	}
	else
	{
		r = (unsigned int) ((r_object.rect_y()-(float) (z-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE+FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
		c = (unsigned int) ((r_object.rect_x()+FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
	}

	if (row != 0 && col != 0)
	{
		try
		{
			z = cell(r, c)->getZ();
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Coordonnées non valides lors de la récupération de hauteur de la case.", e);
		}
	}

	if (r == 0 || r > m_rowsCount || c == 0 || c > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées invalides : row = ") + itostr(r) + " ; col = " + itostr(c) + ".", "r_object ou row/col", "Map::addObject");

	if (row != 0 && col != 0)
	{
		r_object.move(PfOrientation(-1, 0), r_object.rect_x());
		r_object.move(PfOrientation(0, -1), r_object.rect_y()-(r_object.getZ()-MAP_CELL_SQUARE_HEIGHT)*MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT);
		r_object.move(PfOrientation(1, 0), (c-1)*MAP_CELL_SIZE-r_object.getCenter().getX()*r_object.rect_w());
		r_object.move(PfOrientation(0, 1), (r-1)*MAP_CELL_SIZE-r_object.getCenter().getY()*r_object.rect_h());
	}
	r_object.changeZ(z);
	bool hasColl = (r_object.zone(BOX_TYPE_COLLISION) != 0);
	r_object.setLayer(layerAt(hasColl?r_object.zone(BOX_TYPE_COLLISION)->getRect():r_object.getRect(), r_object.getZ()));
	m_cellsUnderObjects_v_map[r_object.getName()] = cellsCoord(r_object.getRect(), z);
	vector<pair<unsigned int, unsigned int> >* p_v = &(m_cellsUnderObjects_v_map[r_object.getName()]); // pour la facilité
	for (unsigned int i=0, size=p_v->size();i<size;i++)
		m_objectsNames_v_t2[(*p_v)[i].first-1][(*p_v)[i].second-1].push_back(r_object.getName());
	r_object.setModified(true);
}

void Map::removeObjects(const PfRectangle& rect, int z)
{
	vector<pair<unsigned int, unsigned int> > coords_v = cellsCoord(rect, z);

	for (unsigned int i=0, size=coords_v.size();i<size;i++)
		removeObjects(objectsOnCell(coords_v[i]));
}

void Map::removeObjects(const pair<unsigned int, unsigned int>& coord_pair)
{
	// suppression par la méthode des noms pour que toute autre case contenant cet objet soit également vidée.
	removeObjects(m_objectsNames_v_t2[coord_pair.first-1][coord_pair.second-1]);
}

void Map::removeObjects(const vector<string> names_v)
{
	for (vector<string>::const_iterator namesIt=names_v.begin();namesIt!=names_v.end();++namesIt)
	{
		if (m_cellsUnderObjects_v_map.find(*namesIt) == m_cellsUnderObjects_v_map.end())
			throw ArgumentException(__LINE__, __FILE__, "L'un des éléments de la liste n'est pas trouvable lors de la suppression d'un objet de la map par son nom.", "r_names_v", "Map::removeObjects");

		for (vector<pair<unsigned int, unsigned int> >::iterator cellsIt=m_cellsUnderObjects_v_map[*namesIt].begin();cellsIt!=m_cellsUnderObjects_v_map[*namesIt].end();++cellsIt)
		{
			for (vector<string>::iterator deleteIt=m_objectsNames_v_t2[cellsIt->first-1][cellsIt->second-1].begin();deleteIt!=m_objectsNames_v_t2[cellsIt->first-1][cellsIt->second-1].end();++deleteIt)
			{
				if (*namesIt == *deleteIt)
				{
					m_objectsNames_v_t2[cellsIt->first-1][cellsIt->second-1].erase(deleteIt);
					break;
				}
			}
		}
		m_cellsUnderObjects_v_map.erase(*namesIt);
	}
}

void Map::removeObject(const string& name)
{
	vector<string> v;
	v.push_back(name);
	removeObjects(v);
}

void Map::updateObjectPosition(MapObject& r_object)
{
    removeObject(r_object.getName());
    m_cellsUnderObjects_v_map[r_object.getName()] = cellsCoord(r_object.getRect(), r_object.getZ());
	vector<pair<unsigned int, unsigned int> >* p_v = &(m_cellsUnderObjects_v_map[r_object.getName()]); // pour la facilité
	for (unsigned int i=0, size=p_v->size();i<size;i++)
		m_objectsNames_v_t2[(*p_v)[i].first-1][(*p_v)[i].second-1].push_back(r_object.getName());
}

int Map::stepsBeforeCollision(const PfRectangle& pathRect, PfOrientation::PfOrientationValue orientation, int z) const
{
	float coord = 0.0;

	switch (orientation)
	{
		case PfOrientation::SOUTH:
			coord = pathRect.getY()+pathRect.getH()-((z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE);
			break;
		case PfOrientation::WEST:
			coord = pathRect.getX()+pathRect.getW();
			break;
		case PfOrientation::NORTH:
			coord = pathRect.getY()-((z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE);
			break;
		case PfOrientation::EAST:
			coord = pathRect.getX();
			break;
		default:
			break;
	}

	int sgnMargin = (orientation==PfOrientation::SOUTH || orientation==PfOrientation::WEST)?1:-1;
	// nombre de pas sur la case (entre 0 et MAP_STEPS_PER_CELL-1)
	int stepsOnCell = (int) ((coord+FLOAT_MARGIN)/MAP_CELL_SIZE * MAP_STEPS_PER_CELL) % MAP_STEPS_PER_CELL;
	// case de départ
	int firstCell = (int) ((coord+sgnMargin*FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;

	int rtn = -1, tmp = 0;
	int row1, row2, col1, col2;

	switch (orientation)
	{
		case PfOrientation::SOUTH:
			col1 = (int) ((pathRect.getX()+FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			col2 = (int) ((pathRect.getX()+pathRect.getW()-FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			for (int col=col1;col<=col2;col++)
			{
				tmp = 0;
				for (int i=firstCell-1;i>0;i--)
				{
					if (cell(i, col)->zAt(PfOrientation::CARDINAL_N, true) <= MAX(z, cell(i+1, col)->zAt(PfOrientation::CARDINAL_S, true)))
						tmp += MAP_STEPS_PER_CELL;
					else
						break;
				}
				if (rtn == -1 || tmp < rtn)
					rtn = tmp; // on prend le min des colonnes
			}
			break;
		case PfOrientation::WEST:
			row1 = (int) ((pathRect.getY()-((z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE)+FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			row2 = (int) ((pathRect.getY()+pathRect.getH()-((z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE)-FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			for (int row=row1;row<=row2;row++)
			{
				tmp = 0;
				for (int i=firstCell-1;i>0;i--)
				{
					if (cell(row, i)->zAt(PfOrientation::CARDINAL_E, true) <= MAX(z, cell(row, i+1)->zAt(PfOrientation::CARDINAL_W, true)))
						tmp += MAP_STEPS_PER_CELL;
					else
						break;
				}
				if (rtn == -1 || tmp < rtn)
					rtn = tmp;
			}
			break;
		case PfOrientation::NORTH:
			col1 = (int) ((pathRect.getX()+FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			col2 = (int) ((pathRect.getX()+pathRect.getW()-FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			for (int col=col1;col<=col2;col++)
			{
				tmp = 0;
				for (int i=firstCell+1;i<=(int)m_rowsCount;i++)
				{
					if (cell(i, col)->zAt(PfOrientation::CARDINAL_S, true) <= MAX(z, cell(i-1, col)->zAt(PfOrientation::CARDINAL_N, true)))
						tmp += MAP_STEPS_PER_CELL;
					else
						break;
				}
				if (rtn == -1 || tmp < rtn)
					rtn = tmp;
			}
			break;
		case PfOrientation::EAST:
			row1 = (int) ((pathRect.getY()-((z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE)+FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			row2 = (int) ((pathRect.getY()+pathRect.getH()-((z-MAP_CELL_SQUARE_HEIGHT)*MAP_Z_STEP_SIZE)-FLOAT_MARGIN)/MAP_CELL_SIZE) + 1;
			for (int row=row1;row<=row2;row++)
			{
				tmp = 0;
				for (int i=firstCell+1;i<=(int)m_columnsCount;i++)
				{
					if (cell(row, i)->zAt(PfOrientation::CARDINAL_W, true) <= MAX(z, cell(row, i-1)->zAt(PfOrientation::CARDINAL_E, true)))
						tmp += MAP_STEPS_PER_CELL;
					else
						break;
				}
				if (rtn == -1 || tmp < rtn)
					rtn = tmp;
			}
			break;
		default:
			break;
	}
	if (orientation == PfOrientation::NORTH || orientation == PfOrientation::EAST)
		rtn += (stepsOnCell==0)?0:(MAP_STEPS_PER_CELL-stepsOnCell);
	else
		rtn += stepsOnCell;

	if (rtn < 0)
		rtn = 0;

	return rtn;
}

int Map::zStepsForNextCell(unsigned int row, unsigned int col, pfflag32 ori) const
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "row", "Map::zStepsForNextCell");
	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "col", "Map::zStepsForNextCell");

	const Cell* pc_cell = mp_cells_t2[row-1][col-1];
	const Cell* pc_nextCell = nextConstCell(row, col, ori);

	if (pc_nextCell == 0)
		return 0;

	return pc_nextCell->getZ() - pc_cell->getZ();
}

unsigned int Map::mapLinksCount() const
{
    return m_mapLinks_v.size();
}

const string& Map::mapLink(unsigned int index) const
{
    if (index == 0 || index > m_mapLinks_v.size())
        throw ArgumentException(__LINE__, __FILE__, string("Indice non valide : l'indice doit être compris entre 1 et le nombre de lien. Indice passé en paramètre : ") +
                                                            itostr((int) index) + ". Indice maximal : " + itostr((int) m_mapLinks_v.size()) + ".",
                                "index", "Map::mapLink");

    return m_mapLinks_v[index-1];
}

unsigned int Map::addMapLink(const string& mapName)
{
    for (unsigned int i=0, size=m_mapLinks_v.size();i<size;i++)
    {
        if (m_mapLinks_v[i] == mapName)
            return i + 1;
    }
    m_mapLinks_v.push_back(mapName);

    return m_mapLinks_v.size();
}

void Map::removeMapLink(unsigned int index)
{
    if (index == 0 || index > m_mapLinks_v.size())
        throw ArgumentException(__LINE__, __FILE__, string("Indice non valide : l'indice doit être compris entre 1 et le nombre de lien. Indice passé en paramètre : ") +
                                                            itostr((int) index) + ". Indice maximal : " + itostr((int) m_mapLinks_v.size()) + ".",
                                "index", "Map::removeMapLink");
    m_mapLinks_v.erase(m_mapLinks_v.begin() + index - 1);
}

void Map::addScriptEntry(const string& text)
{
    m_scriptEntries_v.push_back(text);
}

void Map::editScriptEntry(const string& text, unsigned int index)
{
    if (index >= m_scriptEntries_v.size())
        throw ArgumentException(__LINE__, __FILE__, string("Indice de script non valide : indice = ") + itostr(index) + " ; nombre d'entrées = " +
                                itostr(m_scriptEntries_v.size()) + ".", "index", "Map::editScriptEntry");

    m_scriptEntries_v[index] = text;
}

const string& Map::scriptEntry(unsigned int index) const
{
    if (index >= m_scriptEntries_v.size())
        throw ArgumentException(__LINE__, __FILE__, string("Indice de script non valide : indice = ") + itostr(index) + " ; nombre d'entrées = " +
                                itostr(m_scriptEntries_v.size()) + ".", "index", "Map::scriptEntry");

    return m_scriptEntries_v[index];
}

unsigned int Map::scriptEntriesCount() const
{
    return m_scriptEntries_v.size();
}

void Map::update()
{
	m_modified = false;
}

Viewable* Map::generateViewable() const
{
	Viewable* p_return = new Viewable(m_name);
	p_return->setVisible(true);
	Viewable* p_tmp;

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			try
			{
				p_tmp = mp_cells_t2[i][j]->generateViewable();
				p_return->addViewable(p_tmp);
			}
			catch (PfException& e)
			{
				throw ViewableGenerationException(__LINE__, __FILE__, string("Impossible de générer la case aux coordonnées (") + itostr(i+1) + ";" + itostr(j+1) + ").",
													getName() + ",case("+itostr(i+1)+";"+itostr(j+1)+").", e);
			}
		}
	}

	return p_return;
}

void Map::saveData(ofstream& r_ofs) const
{
	WRITE_ENUM(r_ofs, SAVE_DIM);
	WRITE_UINT(r_ofs, m_rowsCount);
	WRITE_UINT(r_ofs, m_columnsCount);

	WRITE_ENUM(r_ofs, SAVE_SEED);
	WRITE_UINT(r_ofs, m_seed);

	WRITE_ENUM(r_ofs, SAVE_TEXTURE);
	WRITE_STRING(r_ofs, m_textureSet.getName());

	WRITE_ENUM(r_ofs, SAVE_CELLS);
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
			mp_cells_t2[i][j]->saveData(r_ofs);
	}

	WRITE_ENUM(r_ofs, SAVE_GROUNDTYPE);
	WRITE_ENUM(r_ofs, m_groundType);

	WRITE_ENUM(r_ofs, SAVE_MAPLINKS);
	unsigned int n = m_mapLinks_v.size();
	WRITE_UINT(r_ofs, n);
	for (unsigned int i=0;i<n;i++)
    {
        WRITE_STRING(r_ofs, m_mapLinks_v[i]);
    }

    WRITE_ENUM(r_ofs, SAVE_SCRIPT);
    n = m_scriptEntries_v.size();
    WRITE_UINT(r_ofs, n);
    for (unsigned int i=0;i<n;i++)
    {
        WRITE_STRING(r_ofs, m_scriptEntries_v[i]);
    }

	WRITE_ENUM(r_ofs, SAVE_END);
}

bool Map::isCellHidden(unsigned int row, unsigned int col) const
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "row", "Map::isCellHidden");
	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "col", "Map::isCellHidden");

	const Cell* pc_cell = mp_cells_t2[row-1][col-1];
	const Cell* pc_nextCell = nextConstCell(row, col, PfOrientation::SOUTH);

	return (pc_nextCell != 0 && pc_nextCell->getZ() >= pc_cell->getZ() + MAP_CELL_SQUARE_HEIGHT);
}

bool Map::isCliffVisible(unsigned int row, unsigned int col) const
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "row", "Map::isCliffVisible");
	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "col", "Map::isCliffVisible");

	return (heightOfVisibleCliff(row, col) > 0);
}

int Map::heightOfVisibleCliff(unsigned int row, unsigned int col) const
{
	if (row == 0 || row > m_rowsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "row", "Map::isCliffVisible");
	if (col == 0 || col > m_columnsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : (") + itostr(row) + ";" + itostr(col) + ").", "col", "Map::isCliffVisible");

	const Cell* pc_cell = mp_cells_t2[row-1][col-1];
	const Cell* pc_nextCell = nextConstCell(row, col, PfOrientation::SOUTH);

	int rtn = pc_cell->getZ();
	if (pc_nextCell != 0)
		rtn = MAX(0, rtn - pc_nextCell->getZ());

	return rtn;
}
