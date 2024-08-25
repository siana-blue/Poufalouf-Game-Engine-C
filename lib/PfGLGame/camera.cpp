#include "camera.h"

#include "glfunc.h"
#include "glitem.h"

PfCamera::PfCamera(float x, float y, unsigned int speed, bool borders) :
    m_x(x), m_y(y), m_speed(speed), m_keepSpeed(false), m_minStep(0.0), m_relativeToBorders(borders), mq_targetItem(0) {}

void PfCamera::update()
{
	// targetX/Y convertit le point relatif à un éventuel objet en point absolu
	float targetX = m_target.getX(), targetY = m_target.getY();
	if (mq_targetItem != 0)
	{
		targetX += mq_targetItem->rect_x() - 0.5;
		targetY += mq_targetItem->rect_y() - 0.5;
	}

	bool hasMoved = false;

	if (!DECIMAL_EQUAL(targetX, m_x))
	{
		if (m_speed == 0)
			m_x = targetX;
		else
		{
			float step = (float) m_speed/100 * (m_target.getX() - m_pin.getX());
			step = SGN(step) * MAX(ABS(m_minStep), ABS(step));
			m_prev.shift(PfOrientation::EAST, step);
			m_x = m_prev.getX();
			m_x += step;
			if (mq_targetItem != 0)
				m_x += mq_targetItem->rect_x() - 0.5;
		}
		hasMoved = true;
	}

	if (!DECIMAL_EQUAL(targetY, m_y))
	{
		if (m_speed == 0)
			m_y = targetY;
		else
		{
			float step = (float) m_speed/100 * (m_target.getY() - m_pin.getY());
			step = SGN(step) * MAX(ABS(m_minStep), ABS(step));
			m_prev.shift(PfOrientation::NORTH, step);
			m_y = m_prev.getY();
			if (mq_targetItem != 0)
				m_y += mq_targetItem->rect_y() - 0.5;
		}
		hasMoved = true;
	}

	// si on dépasse la cible, on doit arrêter le mouvement
	if ((DECIMAL_SUP(m_target.getX(), m_pin.getX()) && DECIMAL_SUP(m_x, targetX)) || (DECIMAL_INF(m_target.getX(), m_pin.getX()) && DECIMAL_INF(m_x, targetX)))
    {
		m_x = targetX;
		hasMoved = false; // peut-être un peu, mais on veut surtout passer la vitesse à 0 puisqu'on est à la fin du mouvement
    }
	if ((DECIMAL_SUP(m_target.getY(), m_pin.getY()) && DECIMAL_SUP(m_y, targetY)) || (DECIMAL_INF(m_target.getY(), m_pin.getY()) && DECIMAL_INF(m_y, targetY)))
     {
		m_y = targetY;
		hasMoved = false;
     }

	if (!hasMoved && !m_keepSpeed)
		m_speed = 0;

	float newX = m_x;
	float newY = m_y;
	if (ABS(m_limitRect.getW()) > FLOAT_MARGIN)
		newX = MAX(m_limitRect.getX(), MIN(m_limitRect.getX()+m_limitRect.getW()-1, m_x));
	if (ABS(m_limitRect.getH()) > FLOAT_MARGIN)
		newY = MAX(m_limitRect.getY(), MIN(m_limitRect.getY()+m_limitRect.getH()-1, m_y));

	translateCamera(newX*(m_relativeToBorders?(1-2*SYSTEM_BORDER_WIDTH):1), newY);
}

void PfCamera::moveAt(float x, float y)
{
	m_target.setX(x);
	m_target.setY(y);
	m_prev = PfPoint(m_x, m_y);
	if (mq_targetItem != 0)
	{
		m_prev.setX(m_prev.getX()-mq_targetItem->rect_x()+0.5);
		m_prev.setY(m_prev.getY()-mq_targetItem->rect_y()+0.5);
	}
	m_pin = m_prev;
}

void PfCamera::moveAt(float x, float y, unsigned int speed, bool keepSpeed)
{
	m_speed = speed;
	m_keepSpeed = keepSpeed;
	moveAt(x, y);
}

void PfCamera::move(float x, float y)
{
	/*
	* J'avais auparavant mis m_target.setX(m_x + x),
	* mais cela ne prend pas en compte les événements multiples en un tour de rafraîchissement.
	* En effet, pour un mouvement de souris par exemple, le mouvement est décomposé en plusieurs parties.
	* Seule la dernière sera retenue car m_x ne varie pas avant la phase de mise à jour de la caméra.
	*/

	m_target.setX(m_target.getX() + x);
	m_target.setY(m_target.getY() + y);
	m_prev = PfPoint(m_x, m_y);
	if (mq_targetItem != 0)
	{
		m_prev.setX(m_prev.getX()-mq_targetItem->rect_x()+0.5);
		m_prev.setY(m_prev.getY()-mq_targetItem->rect_y()+0.5);
	}
	m_pin = m_prev;
}

void PfCamera::move(float x, float y, unsigned int speed, bool keepSpeed)
{
	m_speed = speed;
	m_keepSpeed = keepSpeed;
	move(x, y);
}

void PfCamera::follow(const RectangleGLItem& rc_target)
{
	mq_targetItem = &rc_target;
}

void PfCamera::stopFollowing()
{
	mq_targetItem = 0;
}

void PfCamera::changeLimit(const PfRectangle& rect)
{
	m_limitRect = rect;
}

pair<float, float> PfCamera::viewport() const
{
	float x = m_x, y = m_y;

	if (ABS(m_limitRect.getW()) > FLOAT_MARGIN)
		x = MAX(m_limitRect.getX(), MIN(m_limitRect.getX()+m_limitRect.getW()-1, m_x));
	if (ABS(m_limitRect.getH()) > FLOAT_MARGIN)
		y = MAX(m_limitRect.getY(), MIN(m_limitRect.getY()+m_limitRect.getH()-1, m_y));

	return pair<float, float>(x*(m_relativeToBorders?(1-2*SYSTEM_BORDER_WIDTH):1), y);
}

const string PfCamera::followedObjectName() const
{
	if (mq_targetItem == 0)
		return "";
	return mq_targetItem->getName();
}
