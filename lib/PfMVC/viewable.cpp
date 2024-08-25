#include "viewable.h"

#include "errors.h"
#include "misc.h"

Viewable::Viewable(const string& name) : m_name(name), m_visible(false), m_layer(0), m_soundIndex(0), m_angle(0.0) {}

Viewable::Viewable(const string& name, const GLImage& glImage, int layer, unsigned int soundIndex) : m_name(name), m_visible(true), m_layer(layer), m_soundIndex(soundIndex), m_angle(0.0)
{
	mp_glImages_v.push_back(new GLImage(glImage));
}

Viewable::Viewable(const string& name, const PfPolygon& polygon, int layer, unsigned int soundIndex, const PfColor& color, bool line, bool coordRelativeToBorder, bool stat) :
	m_name(name), m_visible(true), m_layer(layer), m_soundIndex(soundIndex), m_angle(0.0)
{
	try
	{
		mp_glImages_v.push_back(new GLImage(polygon, color, line?GL_LINE_LOOP:GL_TRIANGLE_FAN, coordRelativeToBorder, stat));
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Erreur lors de la construction de la GLImage.", "Viewable", e);
	}
}

Viewable::Viewable(const string& name, const PfPolygon& polygon, int layer, unsigned int textureIndex, unsigned int soundIndex, const PfPolygon& coordPolygon, const PfColor& color,
				bool coordRelativeToBorder, bool stat) : m_name(name), m_visible(true), m_layer(layer), m_soundIndex(soundIndex), m_angle(0.0)
{
	try
	{
		mp_glImages_v.push_back(new GLImage(polygon, textureIndex, coordPolygon, color, coordRelativeToBorder, stat));
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Erreur lors de la construction de la GLImage.", "Viewable", e);
	}
}

Viewable::~Viewable()
{
	for (unsigned int i=0, size=mp_viewables_v.size();i<size;i++)
	{
		if (mp_viewables_v[i] != 0)
			delete mp_viewables_v[i];
	}
	for (unsigned int i=0, size=mp_glImages_v.size();i<size;i++)
	{
		if (mp_glImages_v[i] != 0)
			delete mp_glImages_v[i];
	}
}

void Viewable::addImage(GLImage* p_glImage)
{
	if (p_glImage != 0)
		mp_glImages_v.push_back(p_glImage);
}

void Viewable::addImages(vector<GLImage*>& p_glImages_v_r)
{
	for (unsigned int i=0, size=p_glImages_v_r.size();i<size;i++)
		addImage(p_glImages_v_r[i]);
}

void Viewable::addImages(const Viewable& viewable)
{
	for (unsigned int i=0, size=viewable.mp_glImages_v.size();i<size;i++)
	{
		if (viewable.mp_glImages_v[i] == 0)
			continue;
		addImage(new GLImage(*(viewable.mp_glImages_v[i])));
	}
}

void Viewable::addViewable(Viewable* p_viewable)
{
	if (p_viewable != 0)
		mp_viewables_v.push_back(p_viewable);
}

unsigned int Viewable::imagesCount() const
{
	return mp_glImages_v.size();
}

const GLImage& Viewable::imageAt(unsigned int index) const
{
	if (index >= mp_glImages_v.size())
		throw ArgumentException(__LINE__, __FILE__, string("Indice non valide, l'indice doit être compris entre 0 et ") + itostr(mp_glImages_v.size()) + ".", "index", "Viewable::imageAt");
	return *(mp_glImages_v[index]);
}

unsigned int Viewable::viewablesCount() const
{
	return mp_viewables_v.size();
}

const Viewable* Viewable::viewableAt(unsigned int index) const
{
	if (index >= mp_viewables_v.size())
		throw ArgumentException(__LINE__, __FILE__, string("Indice non valide, l'indice doit être compris entre 0 et ") + itostr(mp_viewables_v.size()) + ".", "index", "Viewable::viewableAt");
	return mp_viewables_v[index];
}

void Viewable::changeLayer(int layer)
{
    m_layer = layer;
}

void Viewable::changeAngle(double angle)
{
	for (unsigned int i=0, size=mp_glImages_v.size();i<size;i++)
	{
		if (mp_glImages_v[i] != 0)
			mp_glImages_v[i]->setAngle(angle);
	}
	for (unsigned int i=0, size=mp_viewables_v.size();i<size;i++)
	{
		if (mp_viewables_v[i] != 0)
			mp_viewables_v[i]->changeAngle(angle);
	}
}

