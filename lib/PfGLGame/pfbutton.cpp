#include "pfbutton.h"

#include "viewable.h"

PfButton::PfButton(const string& name, float x, float y, float w, float h, unsigned int layer, unsigned int fontTextureIndex,
                   const string& text, float hMargin, float vMargin, PfWidget::PfWidgetStatusMap statusMap) :
    PfWidget(name, x, y, w, h, layer, statusMap),
    m_text(text, fontTextureIndex, PfRectangle(x+hMargin*w, y+vMargin*h, (1-2*hMargin)*w, (1-2*vMargin)*h)) {}

void PfButton::changeText(const string& text)
{
	m_text.changeText(text);
	m_modified = true;
}

Viewable* PfButton::generateViewable() const
{
    Viewable* p_ = PfWidget::generateViewable();

    vector<GLImage*>* pn_images_v_p_n = m_text.toGLImages(isCoordRelativeToBorder(), isStatic());
    for (unsigned int i=0, size=pn_images_v_p_n->size();i<size;i++)
        p_->addImage(pn_images_v_p_n->at(i));

    delete pn_images_v_p_n;

    return p_;
}
