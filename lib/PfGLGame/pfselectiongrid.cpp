#include "pfselectiongrid.h"

#include "errors.h"
#include "misc.h"
#include "viewable.h"

PfSelectionGrid::PfSelectionGrid(const string& name, float x, float y, float w, float h, unsigned int rows, unsigned int columns, unsigned int layer,
								PfWidget::PfWidgetStatusMap statusMap) :
	PfWidget(name, x, y, w, h, layer, statusMap), m_rowsCount(rows), m_columnsCount(columns), m_currentRow(0), m_currentCol(0), m_step(0)
{
	m_repeatingSelection = true;

	try
	{
		if (rows*columns > 256)
			throw ArgumentException(__LINE__, __FILE__, "Une grille ne peut pas avoir plus de 256 cases.", "rows/columns", "PfSelectionGrid::PfSelectionGrid");

		setColor(PfColor::BLACK);
		mp_buttons_t2 = new PfWidget**[rows];
		for (unsigned int i=0;i<rows;i++)
		{
			mp_buttons_t2[i] = new PfWidget*[columns];
			for (unsigned int j=0;j<columns;j++)
				mp_buttons_t2[i][j] = new PfWidget(name + "_gridcell_(" + itostr(i) + ";" + itostr(j) + ")",
                                                   x + j*w/columns, y + (rows-i-1)*h/rows, w/columns, h/rows, MAX_LAYER, statusMap);
		}
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de générer cette grille.", "PfSelectionGrid", e);
	}
}

PfSelectionGrid::~PfSelectionGrid()
{
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_buttons_t2[i][j] != 0)
				delete mp_buttons_t2[i][j];
		}
		delete [] mp_buttons_t2[i];
	}
	delete [] mp_buttons_t2;
}

PfReturnCode PfSelectionGrid::selectCell(unsigned int row, unsigned int col)
{
	PfReturnCode code = RETURN_NOTHING;

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_buttons_t2[i][j] != 0)
				mp_buttons_t2[i][j]->deactivate();
		}
	}

    if (row == 0 || col == 0 || row > m_rowsCount || col > m_columnsCount)
	{
		m_currentRow = m_currentCol = 0;
        code = RETURN_EMPTY;
	}
    else if (mp_buttons_t2[row-1][col-1] != 0)
    {
    	if (m_currentRow != row || m_currentCol != col)
    	{
			m_currentRow = row;
			m_currentCol = col;
			code = RETURN_OK;
    	}
        mp_buttons_t2[row-1][col-1]->activate();
    }

    m_modified = true;

    return code;
}

PfReturnCode PfSelectionGrid::selectCellAt(const PfPoint& rc_point)
{
	PfReturnCode code = RETURN_EMPTY;
	bool brk = false;

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_buttons_t2[i][j] != 0)
			{
				if (mp_buttons_t2[i][j]->getRect().contains(rc_point))
				{
					code = RETURN_NOTHING;
					if (m_currentRow != i+1 || m_currentCol != j+1)
					{
						code = selectCell(i+1, j+1);
						brk = true;
						break;
					}
				}
			}
		}
		if (brk)
			break;
	}

	return code;
}

void PfSelectionGrid::addCellImage(unsigned int textureIndex, const PfRectangle& rc_texCoordRect)
{
	m_images_v.push_back(pair<unsigned int, PfRectangle>(textureIndex, rc_texCoordRect));
}

void PfSelectionGrid::changeStep(unsigned int step)
{
	m_step = MIN(step, m_images_v.size()/m_columnsCount+1);
	m_modified = true;
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
			mp_buttons_t2[i][j]->deselect();
	}
}

void PfSelectionGrid::activate()
{
	PfWidget::activate();

	if (getPoint() != PfPoint(0.0, 0.0))
		selectCellAt(getPoint());
	resetEffects();
	if (m_currentRow == 0 || m_currentCol == 0)
		return;
	addEffects(EFFECT_SELECT | ((m_currentRow+m_step-1)*m_columnsCount+m_currentCol-1));
}

void PfSelectionGrid::addAnimation(PfAnimation* p_anim, PfAnimationStatus value)
{
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_buttons_t2[i][j] != 0)
			{
				PfAnimation* p_ = new PfAnimation(*p_anim);
				mp_buttons_t2[i][j]->addAnimation(p_, value);
			}
		}
	}
	delete p_anim;
}

void PfSelectionGrid::update()
{
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_buttons_t2[i][j] != 0)
			{
				mp_buttons_t2[i][j]->update();
				if (mp_buttons_t2[i][j]->isModified())
                {
					m_modified = true;
					mp_buttons_t2[i][j]->setModified(false);
                }
			}
		}
	}
}

Viewable* PfSelectionGrid::generateViewable() const
{
	if (m_rowsCount == 0 || m_columnsCount == 0)
		return new Viewable(getName());
	if (mp_buttons_t2[0][0] == 0)
		throw ViewableGenerationException(__LINE__, __FILE__, "Le premier bouton de cette grille est nul.", getName());

	// fond noir
	PfRectangle rect = mp_buttons_t2[m_rowsCount-1][0]->getRect();
	Viewable* p_rtn = RectangleGLItem::generateViewable();
	// images
	for (unsigned int i=0, size=MIN(m_images_v.size()-m_step*m_columnsCount, m_rowsCount*m_columnsCount);i<size;i++)
	{
		p_rtn->addImage(new GLImage(PfRectangle(rect.getX()+(i%m_columnsCount)*(rect.getW()),
													rect.getY()+(m_rowsCount-i/m_columnsCount-1)*rect.getH(),
													rect.getW(),
													rect.getH()),
										m_images_v[i+m_step*m_columnsCount].first,
										m_images_v[i+m_step*m_columnsCount].second,
										PfColor::WHITE, false));
	}
	// cases
	Viewable* p_tmp;
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_buttons_t2[i][j] != 0)
			{
				mp_buttons_t2[i][j]->setCoordRelativeToBorder(isCoordRelativeToBorder());
				mp_buttons_t2[i][j]->setStatic(isStatic());
				p_tmp = mp_buttons_t2[i][j]->generateViewable();
				p_rtn->addImages(*p_tmp);
				if (p_tmp->getSoundIndex() != 0)
					p_rtn->setSoundIndex(p_tmp->getSoundIndex());
				delete p_tmp;
			}
		}
	}

	return p_rtn;
}
