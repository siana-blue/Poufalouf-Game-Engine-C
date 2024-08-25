#include "glitem.h"

#include "errors.h"
#include "viewable.h"

// GLItem

GLItem::GLItem(const string& name, int layer, const PfColor& color, bool line, double angle) :
	ModelItem(name), m_layer(layer), m_textureIndex(0), m_color(color), m_line(line), m_coordRelativeToBorder(true), m_static(false), m_angle(angle),
	m_blinkingSpeed(0), m_blinkingColor(PfColor::WHITE), m_blinkingTick(0) {}

GLItem::GLItem(const string& name, int layer, unsigned int textureIndex, const PfColor& color, double angle) :
	ModelItem(name), m_layer(layer), m_textureIndex(textureIndex), m_color(color), m_line(false), m_coordRelativeToBorder(true), m_static(false),
	m_angle(angle), m_blinkingSpeed(0), m_blinkingColor(PfColor::WHITE), m_blinkingTick(0) {}

void GLItem::blink(unsigned int speed, PfColor color)
{
    m_blinkingSpeed = speed;
    m_blinkingColor = m_color = color;
    m_blinkingTick = 0;
    setModified(true);
}

void GLItem::update()
{
    if (m_blinkingSpeed > 0)
    {
        float r, g, b;
        r = (float) ABS(m_blinkingTick) / m_blinkingSpeed * (1.0 - m_blinkingColor.getR()) + m_blinkingColor.getR();
        g = (float) ABS(m_blinkingTick) / m_blinkingSpeed * (1.0 - m_blinkingColor.getG()) + m_blinkingColor.getG();
        b = (float) ABS(m_blinkingTick) / m_blinkingSpeed * (1.0 - m_blinkingColor.getB()) + m_blinkingColor.getB();
        m_color = PfColor(r, g, b);

        if (ABS(m_blinkingTick) == (int) m_blinkingSpeed)
            m_blinkingTick *= -1;
        m_blinkingTick++;

        setModified(true);
    }
}

// PolygonGLItem

PolygonGLItem::PolygonGLItem(const string& name, int layer, const PfPolygon& polygon, const PfColor& color, bool line, double angle) :
	GLItem(name, layer, color, line, angle), m_polygon(polygon), m_textCoordPolygon(PfRectangle(1.0, 1.0)) {}

PolygonGLItem::PolygonGLItem(const string& name, int layer, const PfPolygon& polygon, unsigned int textureIndex, const PfPolygon& coordPolygon, const PfColor& color, double angle) :
	GLItem(name, layer, textureIndex, color, angle), m_polygon(polygon), m_textCoordPolygon(coordPolygon) {}

void PolygonGLItem::shift(const PfOrientation& orientation, double scale)
{
	m_polygon.shift(orientation, scale);
	m_modified = true;
}

Viewable* PolygonGLItem::generateViewable() const
{
	Viewable* p_tmp;
	try
	{
		if (getTextureIndex() != 0)
			p_tmp = new Viewable(m_name, m_polygon, getLayer(), getTextureIndex(), 0, m_textCoordPolygon, getColor(), isCoordRelativeToBorder(), isStatic());
		else
			p_tmp = new Viewable(m_name, m_polygon, getLayer(), 0, getColor(), isLine(), isCoordRelativeToBorder(), isStatic());
		p_tmp->changeAngle(getAngle());
	}
	catch (PfException& e)
	{
		throw ViewableGenerationException(__LINE__, __FILE__, "Impossible de générer le Viewable.", m_name, e);
	}

	return p_tmp;
}

// RectangleGLItem

RectangleGLItem::RectangleGLItem(const string& name, int layer, const PfRectangle& rectangle, const PfColor& color, bool line) :
	GLItem(name, layer, color, line), m_rect(rectangle), m_textCoordRect(PfRectangle(1.0, 1.0)) {}

RectangleGLItem::RectangleGLItem(const string& name, int layer, const PfRectangle& rectangle, unsigned int textureIndex, const PfRectangle& coordRectangle, const PfColor& color) :
	GLItem(name, layer, textureIndex, color), m_rect(rectangle), m_textCoordRect(coordRectangle) {}

void RectangleGLItem::shift(const PfOrientation& orientation, double scale)
{
	m_rect.shift(orientation, scale);
	m_modified = true;
}

Viewable* RectangleGLItem::generateViewable() const
{
	Viewable* p_tmp;
	try
	{
		if (getTextureIndex() != 0)
			p_tmp = new Viewable(m_name, m_rect, getLayer(), getTextureIndex(), 0, m_textCoordRect, getColor(), isCoordRelativeToBorder(), isStatic());
		else
			p_tmp = new Viewable(m_name, m_rect, getLayer(), 0, getColor(), isLine(), isCoordRelativeToBorder(), isStatic());
		p_tmp->changeAngle(getAngle());
	}
	catch (PfException& e)
	{
		throw ViewableGenerationException(__LINE__, __FILE__, "Impossible de générer le Viewable.", m_name, e);
	}

	return p_tmp;
}

// AnimatedGLItem

AnimatedGLItem::AnimatedGLItem(const string& name, int layer, const PfRectangle& rectangle, const PfColor& color) :
	RectangleGLItem(name, layer, rectangle), m_orientation(PfOrientation::NO_ORIENTATION)
{
	m_animationGroup.addAnimation(new PfAnimation(0, PfRectangle(), false, 0, color), ANIM_IDLE);
	changeCurrentAnimation(ANIM_IDLE);
}

AnimatedGLItem::AnimatedGLItem(const string& name, int layer, const PfRectangle& rectangle, unsigned int textureIndex, unsigned int soundIndex, const PfRectangle& coordRectangle,
							const PfColor& color) : RectangleGLItem(name, layer, rectangle), m_orientation(PfOrientation::NO_ORIENTATION)
{
	m_animationGroup.addAnimation(new PfAnimation(textureIndex, coordRectangle, false, soundIndex, color), ANIM_IDLE);
	changeCurrentAnimation(ANIM_IDLE);
}

AnimatedGLItem::AnimatedGLItem(const string& name, int layer, const PfRectangle& rectangle, PfAnimation* p_anim) :
	RectangleGLItem(name, layer, rectangle), m_orientation(PfOrientation::NO_ORIENTATION)
{
	m_animationGroup.addAnimation(p_anim, ANIM_IDLE);
	changeCurrentAnimation(ANIM_IDLE);
}

void AnimatedGLItem::addAnimation(PfAnimation* p_anim, PfAnimationStatus status)
{
	try
	{
		m_animationGroup.addAnimation(p_anim, status);
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Impossible d'ajouter l'animation.", e);
	}
}

bool AnimatedGLItem::changeCurrentAnimation(PfAnimationStatus status, bool reset)
{
	bool b = m_animationGroup.changeAnimation(status);
	if (b || reset)
	{
		m_animationGroup.currentAnimation()->reset();
		setModified(true);
	}

	return b;
}

PfAnimationStatus AnimatedGLItem::currentAnimationStatus() const
{
	return m_animationGroup.getCurrentAnimation();
}

bool AnimatedGLItem::changeOrientation(PfOrientation::PfOrientationValue orientation)
{
    if (m_orientation != orientation)
    {
        m_orientation = orientation;
        m_modified = true;

        return true;
    }

    return false;
}

void AnimatedGLItem::update()
{
    GLItem::update();

	if (m_animationGroup.currentAnimation()->increaseFrame())
		m_modified = true;
	else if (m_animationGroup.currentAnimation()->isLoop())
	{
		m_animationGroup.currentAnimation()->reset();
		m_modified = true;
	}
}

Viewable* AnimatedGLItem::generateViewable() const
{
	PfColor color(m_animationGroup.currentFrame().getColor());

	Viewable* p_tmp;
	try
	{
		p_tmp = new Viewable(m_name, getRect(), getLayer(), m_animationGroup.currentFrame().getTextureIndex(), m_animationGroup.currentFrame().getSoundIndex(),
							m_animationGroup.currentFrame().textCoordRectangle(m_orientation), ((color==PfColor::WHITE)?getColor():color), isCoordRelativeToBorder(), isStatic());
	}
	catch (PfException& e)
	{
		throw ViewableGenerationException(__LINE__, __FILE__, "impossible de générer le Viewable.", m_name, e);
	}

	return p_tmp;
}

