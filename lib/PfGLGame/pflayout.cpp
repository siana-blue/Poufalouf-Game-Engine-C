#include "pflayout.h"

#include "errors.h"
#include "viewable.h"
#include "misc.h"

PfLayout::PfLayout(const string& name, unsigned int rows, unsigned int columns) : PfWidget(name), m_rowsCount(rows), m_columnsCount(columns),
    m_currentRow(0), m_currentCol(0)
{
    mp_widgets_t2 = new PfWidget**[rows];
    for (unsigned int i=0;i<rows;i++)
    {
        mp_widgets_t2[i] = new PfWidget*[columns];
        for (unsigned int j=0;j<columns;j++)
            mp_widgets_t2[i][j] = 0;
    }
}

PfLayout::~PfLayout()
{
    for (unsigned int i=0;i<m_rowsCount;i++)
    {
        for (unsigned int j=0;j<m_columnsCount;j++)
        {
            if (mp_widgets_t2[i][j] != 0)
                delete mp_widgets_t2[i][j];
        }
        delete [] mp_widgets_t2[i];
    }
    delete [] mp_widgets_t2;
}

void PfLayout::addWidget(PfWidget* p_widget, unsigned int row, unsigned int col)
{
    if (row == 0 || col == 0 || row > m_rowsCount || col > m_columnsCount)
        throw ArgumentException(__LINE__, __FILE__, string("Coordonnées non valides : le tableau de widgets a pour dimensions (") + itostr(m_rowsCount) + ";" +
                                itostr(m_columnsCount) + "). Coordonnées spécifiées : (" + itostr(row) + ";" + itostr(col) + ").", "row/col", "PfLayout::addWidget");

    if (mp_widgets_t2[row-1][col-1] != 0)
        delete mp_widgets_t2[row-1][col-1];
    mp_widgets_t2[row-1][col-1] = p_widget;
}

PfReturnCode PfLayout::selectWidget(unsigned int row, unsigned int col, bool justHighlight)
{
    if (row == 0 || col == 0 || row > m_rowsCount || col > m_columnsCount)
        return RETURN_EMPTY;

	PfReturnCode code = RETURN_OK;
	PfWidget* p_widget = mp_widgets_t2[row-1][col-1];
    if (p_widget != 0)
    {
		if (!p_widget->isEnabled())
			return RETURN_EMPTY;

		if (m_currentRow == row && m_currentCol == col && !p_widget->isRepeatingSelection())
			return RETURN_NOTHING;

		if (justHighlight)
			p_widget->highlight();
		else
		{
        	m_currentRow = row;
        	m_currentCol = col;
        	p_widget->select();
		}

		for (unsigned int i=0;i<m_rowsCount;i++)
		{
			for (unsigned int j=0;j<m_columnsCount;j++)
			{
				if (mp_widgets_t2[i][j] != 0 && (row != i+1 || col != j+1)) // si le widget n'est pas le nouveau sélectionné ou surligné
				{
					if (!justHighlight || m_currentRow != i+1 || m_currentCol != j+1) // si le widget n'est pas le widget actuellement sélectionné ou qu'il y a plus que surlignage
						mp_widgets_t2[i][j]->leave(); // quitter le widget
				}
			}
		}

        m_modified = true;
    }
    else
		return RETURN_EMPTY;

    return code;
}

PfReturnCode PfLayout::selectNextWidget(PfOrientation::PfOrientationValue orientation)
{
    if (orientation == PfOrientation::NO_ORIENTATION || orientation > PfOrientation::EAST)
        throw ArgumentException(__LINE__, __FILE__, "L'orientation spécifiée n'est pas valide.", "orientation", "PfLayout::selectNextWidget");

    if (m_currentRow == 0 || m_currentCol == 0)
        throw PfException(__LINE__, __FILE__, "L'une des coordonnées actuelles est 0.");

    PfOrientation o(orientation);
    unsigned int row = m_currentRow + o.getY(), col = m_currentCol + o.getX();
    if (row == 0 || col == 0 || row > m_rowsCount || col > m_columnsCount)
        return RETURN_EMPTY;
    if (o.isEW())
    {
        unsigned int r;
        for (r=row;r>=1;r--)
        {
            if (mp_widgets_t2[r-1][col-1] != 0)
                break;
        }
        row = r;
    }
    else
    {
        unsigned int c;
        for (c=col;c>=1;c--)
        {
            if (mp_widgets_t2[row-1][c-1] != 0)
                break;
        }
        col = c;
    }
    return selectWidget(row, col);
}

PfReturnCode PfLayout::selectWidgetAt(const PfPoint& rc_point, bool justHighlight)
{
	/*
	 * Dans un premier temps, mettre à jour le widget actuel par une resélection en simple emphase
	 * (pour les spinbox par exemple qui ont besoin de se mettre à jour au niveau des surlignages de boutons).
	 * C'est important pour un cas particulier : si la souris se déplace en un seul tour de rafraîchissement d'une boîte numérique vers un bouton par exemple.
	 * Si cette mise à jour ne se faisait que dans le cas où la souris est dans le vide, alors la spinbox ne serait pas mise à jour et le bouton flèche
	 * pourrait rester surligné.
	 * Ici, on réinitialise tout d'abord le widget actuel, puis on voit si on doit en sélectionner un autre ou mettre à jour le point de l'actuel.
	 */

	if (m_currentRow != 0 && m_currentCol != 0)
	{
		selectWidget(m_currentRow, m_currentCol, true);
		mp_widgets_t2[m_currentRow-1][m_currentCol-1]->setPoint(PfPoint(0.0, 0.0));
	}

	// Puis on regarde s'il y a des changements à faire

	PfReturnCode code = RETURN_EMPTY;
	bool done = false;

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_widgets_t2[i][j] != 0)
			{
				if (mp_widgets_t2[i][j]->getRect().contains(rc_point))
				{
					mp_widgets_t2[i][j]->setPoint(rc_point);
					code = RETURN_NOTHING;
					if (m_currentRow != i+1 || m_currentCol != j+1 || mp_widgets_t2[i][j]->isRepeatingSelection())
					{
						code = selectWidget(i+1, j+1, justHighlight);
						done = true;
						break;
					}
				}
			}
		}
		if (done)
			break;
	}

	return code;
}

pair<unsigned int, unsigned int> PfLayout::coordAt(const PfPoint& rc_point)
{
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_widgets_t2[i][j] != 0)
			{
				if (mp_widgets_t2[i][j]->getRect().contains(rc_point))
					return pair<unsigned int, unsigned int>(i+1, j+1);
			}
		}
	}

	return pair<unsigned int, unsigned int>(0, 0);
}

PfWidget* PfLayout::widget(unsigned int row, unsigned int col)
{
	if (row == 0 || col == 0 || row > m_rowsCount || col > m_columnsCount)
		return 0;

	return mp_widgets_t2[row-1][col-1];
}

PfWidget* PfLayout::widget(const string& name)
{
	PfWidget* p_rtn = 0;

	bool done = false;
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_widgets_t2[i][j] != 0 && mp_widgets_t2[i][j]->getName() == name)
			{
				p_rtn = mp_widgets_t2[i][j];
				done = true;
				break;
			}
		}
		if (done)
            break;
	}

	return p_rtn;
}

const PfWidget* PfLayout::constWidget(unsigned int row, unsigned col) const
{
	if (row == 0 || col == 0 || row > m_rowsCount || col > m_columnsCount)
		return 0;

	return mp_widgets_t2[row-1][col-1];
}

void PfLayout::leaveAllButCurrent()
{
	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_widgets_t2[i][j] != 0)
			{
				mp_widgets_t2[i][j]->leave();
				if (m_currentRow == i+1 && m_currentCol == j+1)
					mp_widgets_t2[i][j]->select();
			}
		}
	}
}

void PfLayout::enable()
{
	PfWidget::enable();

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_widgets_t2[i][j] != 0)
				mp_widgets_t2[i][j]->enable();
		}
	}
	selectWidget(m_currentRow, m_currentCol);
}

void PfLayout::disable()
{
	PfWidget::disable();

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_widgets_t2[i][j] != 0)
				mp_widgets_t2[i][j]->disable();
		}
	}
}

void PfLayout::select()
{
	PfWidget::select();

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (m_currentRow == i+1 && m_currentCol == j+1 && mp_widgets_t2[i][j] != 0)
				mp_widgets_t2[i][j]->select();
		}
	}
}

void PfLayout::deselect()
{
	PfWidget::deselect();

	for (unsigned int i=0;i<m_rowsCount;i++)
	{
		for (unsigned int j=0;j<m_columnsCount;j++)
		{
			if (mp_widgets_t2[i][j] != 0)
				mp_widgets_t2[i][j]->deselect();
		}
	}
}

bool PfLayout::slide()
{
    if (m_currentRow == 0 || m_currentCol == 0)
        return false;

	if (!mp_widgets_t2[m_currentRow-1][m_currentCol-1]->isEnabled())
		return false;
	if (mp_widgets_t2[m_currentRow-1][m_currentCol-1]->slide())
	{
		m_modified = true;
		resetEffects();
		addEffects(mp_widgets_t2[m_currentRow-1][m_currentCol-1]->getEffectFlags());
		return true;
	}

	return false;
}

void PfLayout::update()
{
    for (unsigned int i=0;i<m_rowsCount;i++)
    {
        for (unsigned int j=0;j<m_columnsCount;j++)
        {
            if (mp_widgets_t2[i][j] != 0)
			{
				mp_widgets_t2[i][j]->update();
				if (mp_widgets_t2[i][j]->isModified())
					m_modified = true;
				mp_widgets_t2[i][j]->setModified(false);
			}
		}
	}
}

Viewable* PfLayout::generateViewable() const
{
    Viewable* p_return = PfWidget::generateViewable();
    Viewable* p_tmp;
    for (unsigned int i=0;i<m_rowsCount;i++)
    {
        for (unsigned int j=0;j<m_columnsCount;j++)
        {
            if (mp_widgets_t2[i][j] != 0)
	    	{
				p_tmp = mp_widgets_t2[i][j]->generateViewable();
				p_return->addViewable(p_tmp);
				if (p_tmp->getSoundIndex() != 0)
					p_return->setSoundIndex(p_tmp->getSoundIndex());
	    	}
        }
    }

    return p_return;
}

PfReturnCode PfLayout::processInstruction()
{
	PfInstruction instruction = getInstruction();
	int value = instructionValues().nextInt();
	PfReturnCode rtnCode = RETURN_NOTHING;

	resetEffects();
	if (m_currentRow != 0 && m_currentCol != 0 && mp_widgets_t2[m_currentRow-1][m_currentCol-1] != 0)
	{
		rtnCode = mp_widgets_t2[m_currentRow-1][m_currentCol-1]->readInstruction(instruction, value);
		if (mp_widgets_t2[m_currentRow-1][m_currentCol-1]->isActivated())
		{
			addEffects(mp_widgets_t2[m_currentRow-1][m_currentCol-1]->getEffectFlags());
			for (unsigned int i=0;i<m_rowsCount;i++)
			{
				for (unsigned int j=0;j<m_columnsCount;j++)
				{
					if (i != m_currentRow - 1 || j != m_currentCol - 1)
					{
						if (mp_widgets_t2[i][j] != 0)
							mp_widgets_t2[i][j]->deactivate();
					}
				}
			}
		}
	}

	if (rtnCode != RETURN_OK) // si les widgets n'ont pas répondu
	{
		switch (instruction)
		{
			case INSTRUCTION_LEFT:
				selectNextWidget(PfOrientation::WEST);
				rtnCode = RETURN_OK;
				break;
			case INSTRUCTION_UP:
				selectNextWidget(PfOrientation::NORTH);
				rtnCode = RETURN_OK;
				break;
			case INSTRUCTION_RIGHT:
				selectNextWidget(PfOrientation::EAST);
				rtnCode = RETURN_OK;
				break;
			case INSTRUCTION_DOWN:
				selectNextWidget(PfOrientation::SOUTH);
				rtnCode = RETURN_OK;
				break;
			case INSTRUCTION_STOP:
				disable();
				rtnCode = RETURN_OK;
				break;
			default:
				break;
		}
	}

	if (rtnCode == RETURN_OK)
		m_modified = true;

	return rtnCode;
}
