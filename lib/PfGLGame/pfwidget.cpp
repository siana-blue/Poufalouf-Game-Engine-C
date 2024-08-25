#include "pfwidget.h"

#include "viewable.h"

PfWidget::PfWidget(const string& name, unsigned int layer, PfWidgetStatusMap statusMap) :
	AnimatedGLItem(name, layer), m_repeatingSelection(false), m_effectFlags(EFFECT_NONE), m_point(PfPoint(0.0, 0.0)),
	m_widgetStatus(PfWidget::WIDGET_ENABLED), m_justChangedState(false)
{
	defineAnimationMap(statusMap);
}

PfWidget::PfWidget(const string& name, float x, float y, float w, float h, unsigned int layer, PfWidgetStatusMap statusMap) :
	AnimatedGLItem(name, layer, PfRectangle(x, y, w, h)), m_repeatingSelection(false), m_effectFlags(EFFECT_NONE), m_point(PfPoint(0.0, 0.0)),
	m_widgetStatus(PfWidget::WIDGET_ENABLED), m_justChangedState(false)
{
	defineAnimationMap(statusMap);
}

PfWidget::PfWidget(const string& name, PfRectangle rect, unsigned int layer, unsigned int linesCount, float hMargin, float vMargin, PfWidgetStatusMap statusMap) :
    AnimatedGLItem(name, layer, rect), m_repeatingSelection(false), m_effectFlags(EFFECT_NONE), m_point(PfPoint(0.0, 0.0)),
	m_widgetStatus(PfWidget::WIDGET_ENABLED), m_justChangedState(false),
	m_text("", FONT_TEXTURE_INDEX, PfRectangle(rect.getX()+hMargin*rect.getW(), rect.getY()+vMargin*rect.getH(),
                                               (1-2*hMargin)*rect.getW(), (1-2*vMargin)*rect.getH()), linesCount)
{
    defineAnimationMap(statusMap);
}

void PfWidget::addEffects(pfflag32 effects)
{
	m_effectFlags |= effects;
}

void PfWidget::resetEffects()
{
	m_effectFlags = EFFECT_NONE;
}

void PfWidget::changeStatusAnimation(PfWidgetStatus status, PfAnimationStatus anim)
{
	m_animation_map[status] = anim;
}

void PfWidget::addText(const string& text)
{
    m_text.addText(text, true);
	m_modified = true;
}

void PfWidget::clearText()
{
	m_text.changeText("");
	m_modified = true;
}

void PfWidget::changeTextAlignProp(PfText::TextAlignProp align)
{
    m_text.setAlignProp(align);
    m_modified = true;
}

void PfWidget::changeText(const string& text)
{
	clearText();
	addText(text);

    m_modified = true;
}

void PfWidget::enable()
{
	changeStatus(PfWidget::WIDGET_ENABLED);
}

void PfWidget::disable()
{
	changeStatus(PfWidget::WIDGET_DISABLED, false);
}

void PfWidget::highlight()
{
	if (m_widgetStatus == PfWidget::WIDGET_DISABLED)
		return;
	changeStatus(PfWidget::WIDGET_HIGHLIGHTED);
}

void PfWidget::leave()
{
	if (m_widgetStatus == PfWidget::WIDGET_DISABLED)
		return;

	deactivate(); // apparement inutile mais attention aux redéfinitions
	deselect(); // apparement inutile mais attention aux redéfinitions

	changeStatus(PfWidget::WIDGET_ENABLED, false);
}

void PfWidget::select()
{
	if (m_widgetStatus == PfWidget::WIDGET_DISABLED)
		return;
	changeStatus(PfWidget::WIDGET_SELECTED);
}

void PfWidget::deselect()
{
	if (m_widgetStatus == PfWidget::WIDGET_DISABLED)
		return;

	deactivate(); // apparement inutile mais attention aux redéfinitions

	changeStatus(PfWidget::WIDGET_ENABLED, false);
}

void PfWidget::activate()
{
	if (m_widgetStatus == PfWidget::WIDGET_DISABLED)
		return;
	changeStatus(PfWidget::WIDGET_ACTIVATED);
}

void PfWidget::deactivate()
{
	if (m_widgetStatus == PfWidget::WIDGET_DISABLED)
		return;
	changeStatus(PfWidget::WIDGET_ENABLED, false);
}

bool PfWidget::isEnabled() const
{
	return (m_widgetStatus >= PfWidget::WIDGET_ENABLED);
}

bool PfWidget::isHighlighted() const
{
	return (m_widgetStatus >= PfWidget::WIDGET_HIGHLIGHTED);
}

bool PfWidget::isSelected() const
{
	return (m_widgetStatus >= PfWidget::WIDGET_SELECTED);
}

bool PfWidget::isActivated() const
{
	return (m_widgetStatus >= PfWidget::WIDGET_ACTIVATED);
}

void PfWidget::update()
{
	AnimatedGLItem::update();

	map<PfWidgetStatus, PfAnimationStatus>::iterator it = m_animation_map.find(m_widgetStatus);

	if (it == m_animation_map.end())
		changeCurrentAnimation(ANIM_IDLE, m_justChangedState);
	else
		changeCurrentAnimation(it->second, m_justChangedState);

	m_justChangedState = false;
}

Viewable* PfWidget::generateViewable() const
{
	Viewable* p_ = AnimatedGLItem::generateViewable();

	vector<GLImage*>* pn_images_v_p_n;
    pn_images_v_p_n = m_text.toGLImages(isCoordRelativeToBorder(), isStatic());
    for (unsigned int j=0, size2=pn_images_v_p_n->size();j<size2;j++)
        p_->addImage(pn_images_v_p_n->at(j));
    delete pn_images_v_p_n;

	return p_;
}

PfReturnCode PfWidget::processInstruction()
{
	switch (getInstruction())
	{
	case INSTRUCTION_ACTIVATE:
		activate();
		return RETURN_OK;
	case INSTRUCTION_SELECT:
		select();
		return RETURN_OK;
	default:
		break;
	}

	return RETURN_NOTHING;
}

void PfWidget::changeStatus(PfWidgetStatus status, bool onlyHigherRank)
{
	if ((!onlyHigherRank || status > m_widgetStatus) && status != m_widgetStatus)
	{
		m_widgetStatus = status;
		m_justChangedState = true;
	}
}

void PfWidget::defineAnimationMap(PfWidgetStatusMap statusMap)
{
    switch (statusMap)
	{
	case WIDGET_STANDARD_MAP:
		m_animation_map[WIDGET_ENABLED] = ANIM_IDLE;
		m_animation_map[WIDGET_DISABLED] = ANIM_DISABLED;
		m_animation_map[WIDGET_HIGHLIGHTED] = ANIM_HIGHLIGHTED;
		m_animation_map[WIDGET_SELECTED] = ANIM_SELECTED;
		m_animation_map[WIDGET_ACTIVATED] = ANIM_ACTIVATED;
		break;
	case WIDGET_HIGHLIGHT_MAP:
		m_animation_map[WIDGET_ENABLED] = ANIM_IDLE;
		m_animation_map[WIDGET_DISABLED] = ANIM_DISABLED;
		m_animation_map[WIDGET_HIGHLIGHTED] = ANIM_HIGHLIGHTED;
		m_animation_map[WIDGET_SELECTED] = ANIM_HIGHLIGHTED;
		m_animation_map[WIDGET_ACTIVATED] = ANIM_HIGHLIGHTED;
		break;
	case WIDGET_SELECT_MAP:
		m_animation_map[WIDGET_ENABLED] = ANIM_IDLE;
		m_animation_map[WIDGET_DISABLED] = ANIM_DISABLED;
		m_animation_map[WIDGET_HIGHLIGHTED] = ANIM_HIGHLIGHTED;
		m_animation_map[WIDGET_SELECTED] = ANIM_SELECTED;
		m_animation_map[WIDGET_ACTIVATED] = ANIM_SELECTED;
		break;
	default:
		break;
	}
}
