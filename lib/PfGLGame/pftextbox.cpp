#include "pftextbox.h"

#include "errors.h"
#include "viewable.h"

#include "pftext.h"

PfTextBox::PfTextBox(const string& name, const PfRectangle& rect, unsigned int layer, float hMargin, float vMargin, unsigned int textureIndex,
                     PfWidget::PfWidgetStatusMap statusMap) :
	PfWidget(name, rect.getX(), rect.getY(), rect.getW(), rect.getH(), layer, statusMap), m_cursorShown(false), m_fileNameChar(false),
	m_multiLine(false), m_linesCount(1), m_hMargin(hMargin), m_vMargin(vMargin), m_fontTextureIndex(textureIndex) {}

void PfTextBox::addChar(char c)
{
	if (!m_fileNameChar || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == ' ')
	{
		m_text += c;
		m_modified = true;
	}
}

void PfTextBox::backspace()
{
	if (m_text.size() > 0)
	{
		m_text.erase(m_text.size()-1);
		m_modified = true;
	}
}

void PfTextBox::reset()
{
	m_text = "";
	m_modified = true;
}

void PfTextBox::changeText(const string& text)
{
	bool b = false;

	if (m_fileNameChar)
	{
		for (unsigned int i=0, size=text.size();i<size;i++)
		{
			char c = text[i];
			b = ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == ' ');
		}
	}
	else
		b = true;

	if (b)
	{
	    if (m_text != text)
        {
            m_text = text;
            //resetTyping();
            m_modified = true;
        }
	}
}

void PfTextBox::leave()
{
	PfWidget::leave();

	m_cursorShown = false;
	m_modified = true;
}

void PfTextBox::select()
{
	PfWidget::select();

	m_cursorShown = true;
	m_modified = true;
}

void PfTextBox::deselect()
{
    PfWidget::deselect();

	m_cursorShown = false;
	m_modified = true;
}

void PfTextBox::activate()
{
	PfWidget::activate();

	m_cursorShown = true;
	m_modified = true;
}

void PfTextBox::update()
{
    PfWidget::update();

//    if (m_printedCharCount < m_text.size())
//    {
//        switch (m_typingMode)
//        {
//        case TEXT_TYPING_SLOW:
//            m_printedCharCount++;
//            m_modified = true;
//            break;
//        case TEXT_TYPING_FAST:
//            m_printedCharCount += 2;
//            m_printedCharCount = MIN(m_printedCharCount, m_text.size());
//            m_modified = true;
//            break;
//        default:
//            break;
//        }
//    }
}

Viewable* PfTextBox::generateViewable() const
{
	Viewable* p_ = PfWidget::generateViewable();

	PfText text(m_text, m_fontTextureIndex, PfRectangle(rect_x()+m_hMargin*rect_w(), rect_y()+m_vMargin*rect_h(),
                                                        (1-2*m_hMargin)*rect_w(), (1-2*m_vMargin)*rect_h()),
                m_linesCount, PfText::TEXT_ALIGN_JUSTIFIED);
    try
    {
        if (text.getLength() > 0)
        {
            unsigned int offset, length;
            offset = 0; // temporaire, en attente gestion dynamique
            length = m_text.size();
            text.changeDisplay(offset, length);
        }
    }
    catch (PfException& e)
    {
        throw ViewableGenerationException(__LINE__, __FILE__, "Impossible d'afficher le texte.", p_->getName(), e);
    }
	vector<GLImage*>* pn_images_v_p_n = text.toGLImages(isCoordRelativeToBorder(), isStatic(), m_cursorShown);
	for (unsigned int i=0, size=pn_images_v_p_n->size();i<size;i++)
		p_->addImage(pn_images_v_p_n->at(i));
	delete pn_images_v_p_n;

	return p_;
}

PfReturnCode PfTextBox::processInstruction()
{
	PfInstruction instruction = getInstruction();
	int value = instructionValues().nextInt();

	if (value < 0 || value >= 256)
		throw ArgumentException(__LINE__, __FILE__, "La valeur associée à l'instruction doit être comprise entre 0 et 255.", "value", "PfTextBox::processInstruction");

	unsigned char c = (unsigned char) value;
	switch (instruction)
	{
		case INSTRUCTION_ACTIVATE:
			m_cursorShown = true;
			m_modified = true;
			return RETURN_OK;
		case INSTRUCTION_DELETE:
			if (m_cursorShown)
			{
				backspace();
				return RETURN_OK;
			}
			break;
		case INSTRUCTION_MODIFY:
			if (m_cursorShown && (c >= 32 || c == '\n'))
			{
				addChar(c);
				return RETURN_OK;
			}
			break;
		default:
			break;
	}

	return RETURN_NOTHING;
}
