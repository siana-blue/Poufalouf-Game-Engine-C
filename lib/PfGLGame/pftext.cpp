#include "pftext.h"

#include "misc.h"

PfText::PfText() : m_text_t(0), m_length(0), m_textureIndex(FONT_TEXTURE_INDEX), m_linesCount(0), m_alignProp(PfText::TEXT_ALIGN_CENTER),
    m_offset(0), m_displayedLength(0), m_typingMode(PfText::TEXT_TYPING_ALLATONCE), m_typingCounter(0) {}

PfText::PfText(const string& text, unsigned int textureIndex, float zPos_t[256], const PfRectangle& rect, unsigned int linesCount,
               TextAlignProp align) :
	m_length(text.size()), m_textureIndex(textureIndex), m_rect(rect), m_linesCount(linesCount), m_alignProp(align),
	m_offset(0), m_displayedLength(text.size()), m_typingMode(PfText::TEXT_TYPING_ALLATONCE), m_typingCounter(text.size())
{
	m_text_t = new unsigned char[m_length];
	for (unsigned int i=0;i<m_length;i++)
	{
		char c = text.at(i);
		m_text_t[i] = (c<0)?256+c:c;
	}

	for (int i=0;i<256;i++)
		m_zPos_t[i] = zPos_t[i];
}

PfText::PfText(const string& text, unsigned int textureIndex, const PfRectangle& rect, unsigned int linesCount, TextAlignProp align,
               ZPosValue zPosValue) :
	m_length(text.size()), m_textureIndex(textureIndex), m_rect(rect), m_linesCount(linesCount), m_alignProp(align),
	m_offset(0), m_displayedLength(text.size()), m_typingMode(PfText::TEXT_TYPING_ALLATONCE), m_typingCounter(text.size())
{
	m_text_t = new unsigned char[m_length];
	for (unsigned int i=0;i<m_length;i++)
	{
		char c = text.at(i);
		m_text_t[i] = (c<0)?256+c:c;
	}

	for (int i=0;i<256;i++)
	{
		switch (zPosValue)
		{
			case NO_ZPOS:
				m_zPos_t[i] = 0.0;
				break;
			case ZPOS_STANDARD:
				switch (i)
				{
					case 103:
					case 112:
					case 113:
					case 121:
					case 135:
					case 153:
						m_zPos_t[i] = -0.33;
						break;
					default:
						m_zPos_t[i] = 0.0;
						break;
				}
				break;
		}
	}
}

PfText::PfText(const PfText& text) : m_length(text.m_length), m_textureIndex(text.m_textureIndex), m_rect(text.m_rect), m_linesCount(text.m_linesCount),
    m_alignProp(text.m_alignProp), m_offset(text.m_offset), m_displayedLength(text.m_displayedLength),
    m_typingMode(text.m_typingMode), m_typingCounter(text.m_typingCounter)
{
    for (int i=0;i<256;i++)
        m_zPos_t[i] = text.m_zPos_t[i];

    m_text_t = new unsigned char[m_length];
	for (unsigned int i=0;i<m_length;i++)
		m_text_t[i] = text.m_text_t[i];
}

PfText::~PfText()
{
    if (m_text_t)
        delete [] m_text_t;
}

void PfText::changeText(const string& text)
{
    if (m_text_t)
        delete [] m_text_t;
	m_text_t = 0;
	m_length = text.size();
	m_offset = 0;
	m_displayedLength = m_length;

	if (m_length > 0)
    {
        m_text_t = new unsigned char[m_length];
        for (unsigned int i=0;i<m_length;i++)
        {
            char c = text.at(i);
            m_text_t[i] = (unsigned char) c;
        }
    }
}

void PfText::addText(const string& text, bool newLine)
{
    unsigned int m_initialLength = m_length;

    string str;
    for (unsigned int i=0;i<m_length;i++)
        str += m_text_t[i];

    if (m_text_t)
        delete [] m_text_t;
    m_text_t = 0;

    if (newLine && m_length > 0)
    {
        str += '\n';
        m_length++;
    }
    str += text;
    m_length += text.size();
    if (m_displayedLength == m_initialLength - m_offset)
        m_displayedLength += (m_length-m_initialLength);

    if (m_length > 0)
    {
        m_text_t = new unsigned char[m_length];
        for (unsigned int i=0;i<m_length;i++)
        {
            char c = str.at(i);
            m_text_t[i] = (unsigned char) c;
        }
    }
}

vector<GLImage*>* PfText::toGLImages(bool relativeToBorders, bool stat, bool cursor) const
{
    vector<GLImage*>* pn_glImages_v_p_n = new vector<GLImage*>;

    float x = 0, y = 0, w = m_rect.getH()/m_linesCount;
	if (m_length == 0)
    {
        if (cursor)
        {
            y = m_rect.getY() + (m_linesCount-1)*m_rect.getH()/m_linesCount;
            switch (m_alignProp)
            {
            case TEXT_ALIGN_LEFT:
            case TEXT_ALIGN_JUSTIFIED:
                x = m_rect.getX();
                break;
            case TEXT_ALIGN_CENTER:
                x = m_rect.getX() + m_rect.getW()/2 - w/2;
                break;
            case TEXT_ALIGN_RIGHT:
                x = m_rect.getX() + m_rect.getW() - 0.1*w;
                break;
            }
            pn_glImages_v_p_n->push_back(new GLImage(PfRectangle(x, y, 0.1*w, w), PfColor::WHITE, GL_TRIANGLE_FAN, relativeToBorders, stat));
        }
		return pn_glImages_v_p_n;
    }

    unsigned int maxCharPerLine = charCountPerLine();
    unsigned int lastI = m_offset, charCount, displayCharCount, lastWord, maxI = m_offset + m_displayedLength;
    if (m_typingMode != TEXT_TYPING_ALLATONCE)
        maxI = MIN(maxI, m_offset + m_typingCounter);

    for (unsigned int line=0;line<m_linesCount;line++)
    {
        // détermination du contenu de la ligne à afficher
        charCount = displayCharCount = 0;
        lastWord = 0;
        for (unsigned int i=lastI;i<maxI;i++)
        {
            charCount++;
            displayCharCount++;
            if (m_text_t[i] == '\n')
            {
                displayCharCount--;
                break;
            }
            if (displayCharCount == maxCharPerLine)
            {
                // on ne veut pas tronquer de mot, on recule donc si on n'est pas en fin de mot, sauf s'il y a un seul mot sur la ligne
                if (m_text_t[i] == ' ')
                    displayCharCount--; // suppression de l'espace en fin de ligne
                else if (lastWord > 0 && i < maxI - 1 && m_text_t[i+1] != ' ')
                {
                    charCount -= (i - lastWord);
                    displayCharCount -= (i - lastWord);
                }
                break;
            }
            if (m_text_t[i] == ' ')
            {
                if (charCount == 1) // supprimer un espace en début de ligne
                {
                    charCount--;
                    displayCharCount--;
                    lastI++;
                }
                else // il s'agit d'un nouveau mot
                    lastWord = i - 1; // on se place à la fin du dernier mot
            }
        }

        // affichage de la ligne
        y = m_rect.getY() + (m_linesCount-line-1)*m_rect.getH()/m_linesCount;
        switch (m_alignProp)
        {
        case TEXT_ALIGN_LEFT:
        case TEXT_ALIGN_JUSTIFIED:
            x = m_rect.getX();
            break;
        case TEXT_ALIGN_CENTER:
            x = m_rect.getX() + m_rect.getW()/2 - (displayCharCount*w)/2;
            break;
        case TEXT_ALIGN_RIGHT:
            x = m_rect.getX() + m_rect.getW() - displayCharCount*w;
            break;
        }
        for (unsigned int i=lastI;i<lastI+charCount;i++)
        {
            pn_glImages_v_p_n->push_back(new GLImage(PfRectangle(x, y + m_zPos_t[(int) m_text_t[i]]*w, w, w), m_textureIndex,
                                                     PfRectangle(m_text_t[i]%16*1./16, (16-m_text_t[i]/16-1)*1./16, 1./16, 1./16),
                                                     PfColor::WHITE, relativeToBorders, stat));
            x += w;
            // si on n'est pas à la dernière ligne, traiter l'éventuelle justification
            if (m_alignProp == TEXT_ALIGN_JUSTIFIED && lastI+charCount < maxI && m_text_t[lastI+charCount-1] != '\n')
                x += (m_rect.getW() - displayCharCount*w)/(displayCharCount-1);
        }
        y -= w;
        lastI += charCount;

        if (lastI == maxI)
            break;
    }
    if (cursor)
    {
        if (m_text_t[lastI-1] != '\n')
            y += w;
        else
        {
            switch (m_alignProp)
            {
            case TEXT_ALIGN_LEFT:
            case TEXT_ALIGN_JUSTIFIED:
                x = m_rect.getX();
                break;
            case TEXT_ALIGN_CENTER:
                x = m_rect.getX() + m_rect.getW()/2 - w/2;
                break;
            case TEXT_ALIGN_RIGHT:
                x = m_rect.getX() + m_rect.getW() - 0.1*w;
                break;
            }
        }
        pn_glImages_v_p_n->push_back(new GLImage(PfRectangle(x, y, 0.1*w, w), PfColor::WHITE, GL_TRIANGLE_FAN, relativeToBorders, stat));
    }

    return pn_glImages_v_p_n;
}

unsigned int PfText::charCountPerLine() const
{
    return (unsigned int) (m_rect.getW() / (m_rect.getH()/m_linesCount));
}

unsigned int PfText::displayedCharCount() const
{
    return charCountPerLine() * m_linesCount;
}

void PfText::changeDisplay(unsigned int offset, unsigned int length)
{
    if (offset > m_length || offset + length > m_length)
        throw ArgumentException(__LINE__, __FILE__, string("Arguments inadaptés, longueur du texte : ") + itostr(m_length) + ".\noffset=" + itostr(offset) + " ; length=" + itostr(length),
                                "offset/length", "PfText::changeDisplay");

    m_offset = offset;
    m_length = length;
}

void PfText::updateTyping()
{
    switch (m_typingMode)
    {
    case TEXT_TYPING_ALLATONCE:
        m_typingCounter = m_length;
        break;
    case TEXT_TYPING_SLOW:
        if (m_typingCounter < m_length)
            m_typingCounter++;
        break;
    case TEXT_TYPING_FAST:
        if (m_typingCounter < m_length)
            m_typingCounter += 2;
        break;
    }
}

PfText& PfText::operator=(const PfText& text)
{
    if (&text != this)
    {
        m_length = text.m_length;
        m_textureIndex = text.m_textureIndex;
        m_rect = text.m_rect;
        m_linesCount = text.m_linesCount;
        m_alignProp = text.m_alignProp;
        m_offset = text.m_offset;
        m_displayedLength = text.m_displayedLength;
        m_typingMode = text.m_typingMode;
        m_typingCounter = text.m_typingCounter;

        for (int i=0;i<256;i++)
            m_zPos_t[i] = text.m_zPos_t[i];

        if (m_text_t != 0)
            delete [] m_text_t;
        m_text_t = new unsigned char[m_length];
        for (unsigned int i=0;i<m_length;i++)
            m_text_t[i] = text.m_text_t[i];
    }

    return *this;
}
