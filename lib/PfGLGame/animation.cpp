#include "animation.h"

#include <string>
#include "errors.h"
#include "misc.h"

// PfAnimationFrame

PfAnimationFrame::PfAnimationFrame(unsigned int textureIndex, const PfRectangle& textCoordRectangle, unsigned int soundIndex, const PfColor& color) :
	m_textureIndex(textureIndex), m_textCoordRectangle(textCoordRectangle), m_soundIndex(soundIndex), m_color(color), m_properties(NO_FRAME_PROP) {}

PfAnimationFrame::PfAnimationFrame(DataPackage& data) : m_textureIndex(0), m_soundIndex(0), m_properties(NO_FRAME_PROP)
{
	float x, y, w, h, r, g, b;
	int section;

	bool cnt = true;
	while (cnt && !data.isOver())
	{
		section = data.nextEnum();
		switch (section)
		{
		case SAVE_COORD:
			x = data.nextFloat();
			y = data.nextFloat();
			w = data.nextFloat();
			h = data.nextFloat();
			m_textCoordRectangle = PfRectangle(x, y, w, h);
			break;
		case SAVE_COLOR:
			r = data.nextFloat();
			g = data.nextFloat();
			b = data.nextFloat();
			m_color = PfColor(r, g, b);
			break;
		case SAVE_TEXTINDEX:
			m_textureIndex = data.nextUInt();
			break;
		case SAVE_SOUNDINDEX:
			m_soundIndex = data.nextUInt();
			break;
		case SAVE_PROP:
			m_properties = data.nextChar();
			break;
		case SAVE_END:
		default:
			cnt = false;
			break;
		}
	}

	if (section < 0 || section > SAVE_END || data.isOver())
		throw ConstructorException(__LINE__, __FILE__, "Données non valides.", "PfAnimationFrame");
}

bool PfAnimationFrame::prop(PfAnimationFrameFlag flag)
{
	return (m_properties & flag);
}

void PfAnimationFrame::addProperties(PfAnimationFrameFlag flag)
{
	m_properties |= flag;
}

void PfAnimationFrame::eraseProperties()
{
	m_properties = PfAnimationFrame::NO_FRAME_PROP;
}

PfRectangle PfAnimationFrame::textCoordRectangle(PfOrientation::PfOrientationValue ori) const
{
	if (m_properties & FRAME_TURNABLE)
	{
		int colCount = 0;
		switch (ori)
		{
			case PfOrientation::WEST:
				colCount = 1;
				break;
			case PfOrientation::NORTH:
				colCount = 2;
				break;
			case PfOrientation::EAST:
				colCount = 3;
				break;
			default:
				break;
		}
		PfRectangle rect(m_textCoordRectangle.getX()+colCount*m_textCoordRectangle.getW(), m_textCoordRectangle.getY(), m_textCoordRectangle.getW(), m_textCoordRectangle.getH());
		return rect;
	}

	return m_textCoordRectangle;
}

void PfAnimationFrame::saveData(ofstream& r_ofs) const
{
	WRITE_ENUM(r_ofs, SAVE_COORD);
	WRITE_FLOAT(r_ofs, m_textCoordRectangle.getX());
	WRITE_FLOAT(r_ofs, m_textCoordRectangle.getY());
	WRITE_FLOAT(r_ofs, m_textCoordRectangle.getW());
	WRITE_FLOAT(r_ofs, m_textCoordRectangle.getH());

	WRITE_ENUM(r_ofs, SAVE_COLOR);
	WRITE_FLOAT(r_ofs, m_color.getR());
	WRITE_FLOAT(r_ofs, m_color.getG());
	WRITE_FLOAT(r_ofs, m_color.getB());

	WRITE_ENUM(r_ofs, SAVE_TEXTINDEX);
	WRITE_UINT(r_ofs, m_textureIndex);

	WRITE_ENUM(r_ofs, SAVE_SOUNDINDEX);
	WRITE_UINT(r_ofs, m_soundIndex);

	WRITE_ENUM(r_ofs, SAVE_PROP);
	WRITE_CHAR(r_ofs, m_properties);

	WRITE_ENUM(r_ofs, SAVE_END);
}

// PfAnimation

PfAnimation::PfAnimation() : m_loop(false), m_over(false), m_currentFrame(0) {}

PfAnimation::PfAnimation(unsigned int textureIndex, const PfRectangle& textCoordRectangle, bool loop, unsigned int soundIndex, const PfColor& color) :
	m_loop(loop), m_over(false), m_currentFrame(1)
{
    addFrame(new PfAnimationFrame(textureIndex, textCoordRectangle, soundIndex, color));
}

PfAnimation::PfAnimation(DataPackage& data) : m_loop(false), m_over(false), m_currentFrame(0)
{
	int section;
	unsigned int framesCount = 0;

	bool cnt = true;
	while (cnt && !data.isOver())
	{
		section = data.nextEnum();
		switch (section)
		{
		case SAVE_LOOP:
			m_loop = (data.nextChar() == 1);
			break;
		case SAVE_CURRENTFRAME:
			m_currentFrame = data.nextUInt();
			break;
		case SAVE_FRAMESCOUNT:
			framesCount = data.nextUInt();
			break;
		case SAVE_FRAMES:
			for (unsigned int i=0;i<framesCount;i++)
				mp_frames_v.push_back(new PfAnimationFrame(data));
			break;
		case SAVE_END:
		default:
			cnt = false;
			break;
		}
	}

	if (section < 0 || section > SAVE_END || data.isOver())
		throw ConstructorException(__LINE__, __FILE__, "Données non valides.", "PfAnimation");
}

PfAnimation::PfAnimation(const PfAnimation& anim) : m_loop(anim.m_loop), m_over(anim.m_over), m_currentFrame(anim.m_currentFrame)
{
	for (unsigned int i=0, size=anim.mp_frames_v.size();i<size;i++)
		addFrame(new PfAnimationFrame(*(anim.mp_frames_v[i])));
}

PfAnimation::~PfAnimation()
{
    for (unsigned int i=0, size=mp_frames_v.size();i<size;i++)
    {
        if (mp_frames_v[i] != 0)
            delete mp_frames_v[i];
    }
}

bool PfAnimation::increaseFrame()
{
    if (m_currentFrame < mp_frames_v.size())
    {
        if (m_currentFrame == 0)
            throw PfException(__LINE__, __FILE__, "La frame actuelle est 0.");

        m_currentFrame++;
        return true;
    }
	else if (m_loop && m_currentFrame > 0)
		m_currentFrame = 1;
	else
		m_over = true;

    return false;
}

void PfAnimation::reset()
{
	if (mp_frames_v.size() == 0)
		throw PfException(__LINE__, __FILE__, "Cette animation n'a pas de frame.");

	m_currentFrame = 1;
	m_over = false;
}

bool PfAnimation::addFrame(PfAnimationFrame* p_frame, unsigned int index)
{
    if (p_frame == 0)
        return false;

    if (index >= mp_frames_v.size())
        mp_frames_v.push_back(p_frame);
    else
    {
        mp_frames_v.push_back(mp_frames_v[mp_frames_v.size()-1]);
        for (unsigned int i=mp_frames_v.size()-2;i>index;i--)
            mp_frames_v[i] = mp_frames_v[i-1];
        mp_frames_v[index] = p_frame;
    }

    return true;
}

const PfAnimationFrame& PfAnimation::currentFrame() const
{
    if (m_currentFrame == 0)
        throw PfException(__LINE__, __FILE__, "La frame actuelle est 0.");
    if (m_currentFrame > mp_frames_v.size())
        throw PfException(__LINE__, __FILE__,
                          string("La frame actuelle est ") + itostr(m_currentFrame) + ". Le nombre de frames " + "total est " + itostr(mp_frames_v.size()) + ".");

    return *(mp_frames_v[m_currentFrame-1]);
}

PfAnimation& PfAnimation::operator=(const PfAnimation& anim)
{
	m_loop = anim.m_loop;
	m_over = anim.m_over;
	for (unsigned int i=0, size=anim.mp_frames_v.size();i<size;i++)
	{
		if (mp_frames_v[i] != 0)
			delete mp_frames_v[i];
		mp_frames_v[i]  = new PfAnimationFrame(*(anim.mp_frames_v[i]));
	}
	m_currentFrame = anim.m_currentFrame;

	return *this;
}

void PfAnimation::saveData(ofstream& r_ofs) const
{
	WRITE_ENUM(r_ofs, SAVE_LOOP);
	WRITE_CHAR(r_ofs, (m_loop?1:0));

	WRITE_ENUM(r_ofs, SAVE_CURRENTFRAME);
	WRITE_UINT(r_ofs, m_currentFrame);

	WRITE_ENUM(r_ofs, SAVE_FRAMESCOUNT);
	WRITE_UINT(r_ofs, mp_frames_v.size());

	WRITE_ENUM(r_ofs, SAVE_FRAMES);
	for (unsigned int i=0, size=mp_frames_v.size();i<size;i++)
	{
		if (mp_frames_v[i] != 0)
			mp_frames_v[i]->saveData(r_ofs);
		else
			WRITE_ENUM(r_ofs, SAVE_END);
	}

	WRITE_ENUM(r_ofs, SAVE_END);
}

// PfAnimationGroup

PfAnimationGroup::PfAnimationGroup() : m_currentAnimation(ANIM_NONE) {}

PfAnimationGroup::~PfAnimationGroup()
{
    for (map<PfAnimationStatus, PfAnimation*>::iterator it = mp_animations_map.begin();it!=mp_animations_map.end();++it)
    {
        if (it->second != 0)
            delete it->second;
    }
}

void PfAnimationGroup::addAnimation(PfAnimation* p_anim, PfAnimationStatus value)
{
    if (p_anim == 0)
        throw ArgumentException(__LINE__, __FILE__, "L'animation à ajouter est nulle.", "p_anim", "PfAnimationGroup::addAnimation");
    if (value == ANIM_NONE)
        throw ArgumentException(__LINE__, __FILE__, "La valeur de l'animation à ajouter est ANIM_NONE.", "value", "PfAnimationGroup::addAnimation");

    removeObjectFromMap(mp_animations_map, value);

    mp_animations_map.insert(pair<PfAnimationStatus, PfAnimation*>(value, p_anim));
}

bool PfAnimationGroup::changeAnimation(PfAnimationStatus value)
{
    if (value == m_currentAnimation)
        return false;

    map<PfAnimationStatus, PfAnimation*>::iterator it = mp_animations_map.find(value);
    if (it == mp_animations_map.end() || it->second == 0)
        return false;

	it->second->reset();
    m_currentAnimation = it->first;

    return true;
}

PfAnimation* PfAnimationGroup::currentAnimation()
{
    if (m_currentAnimation == ANIM_NONE)
        throw PfException(__LINE__, __FILE__, "L'animation actuelle est ANIM_NONE.");
    map<PfAnimationStatus, PfAnimation*>::const_iterator it = mp_animations_map.find(m_currentAnimation);
    if (it == mp_animations_map.end() || it->second == 0)
        throw PfException(__LINE__, __FILE__, "L'animation en cours est introuvable.");

    return it->second;
}

const PfAnimation& PfAnimationGroup::currentConstAnim() const
{
    if (m_currentAnimation == ANIM_NONE)
        throw PfException(__LINE__, __FILE__, "L'animation actuelle est ANIM_NONE.");
    map<PfAnimationStatus, PfAnimation*>::const_iterator it = mp_animations_map.find(m_currentAnimation);
    if (it == mp_animations_map.end() || it->second == 0)
        throw PfException(__LINE__, __FILE__, "L'animation en cours est introuvable.");

    return *(it->second);
}

const PfAnimationFrame& PfAnimationGroup::currentFrame() const
{
    const PfAnimationFrame* p_frame;
    try
    {
        if (m_currentAnimation == ANIM_NONE)
            throw PfException(__LINE__, __FILE__, "L'animation actuelle est ANIM_NONE.");
        map<PfAnimationStatus, PfAnimation*>::const_iterator it = mp_animations_map.find(m_currentAnimation);
        if (it == mp_animations_map.end() || it->second == 0)
            throw PfException(__LINE__, __FILE__, "L'animation en cours est introuvable.");
        p_frame = &(it->second->currentFrame());
    }
    catch (PfException& e)
    {
        throw PfException(__LINE__, __FILE__, "Impossible de récupérer la frame en cours", e);
    }

    return *p_frame;
}
