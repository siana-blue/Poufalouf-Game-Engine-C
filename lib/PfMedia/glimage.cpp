#include "glimage.h"

#include <string>
#include "errors.h"
#include "misc.h"

GLImage::GLImage() : m_mode(GL_POINTS), m_verticesCount(0), m_textureIndex(0), m_data_t(0), m_valid(false), m_static(false),
    m_angle(0.0), m_centerX(0.0), m_centerY(0.0) {}

GLImage::GLImage(const GLImage& glImage) :
	m_mode(glImage.m_mode), m_verticesCount(glImage.m_verticesCount), m_textureIndex(glImage.m_textureIndex), m_valid(glImage.m_valid),
	m_static(glImage.m_static), m_angle(glImage.m_angle), m_centerX(glImage.m_centerX), m_centerY(glImage.m_centerY)
{
	if (glImage.m_data_t == 0)
		m_data_t = 0;
	else
	{
		int dataCount = (m_textureIndex!=0)?8*m_verticesCount:6*m_verticesCount;
		m_data_t = new GLfloat[dataCount];
		for (int i=0;i<dataCount;i++)
			m_data_t[i] = glImage.m_data_t[i];
	}
}

GLImage::GLImage(const GLfloat* data_t_c, int verticesCount, unsigned int textureIndex, GLenum mode, float centerX, float centerY) :
	m_mode(mode), m_verticesCount(verticesCount), m_textureIndex(textureIndex), m_valid(verticesCount > 0), m_static(false), m_angle(0.0), m_centerX(centerX), m_centerY(centerY)
{
	if (m_verticesCount > MAX_VERTICES_PER_POLYGON)
		throw ConstructorException(__LINE__, __FILE__, string("Pas plus de ") + itostr(MAX_VERTICES_PER_POLYGON) + " points par polygone.", "GLImage");

	if (verticesCount == 0)
		m_data_t = 0;
	else
	{
		int dataCount = (m_textureIndex!=0)?8*m_verticesCount:6*m_verticesCount;
		m_data_t = new GLfloat[dataCount];
		for (int i=0;i<dataCount;i++)
			m_data_t[i] = data_t_c[i];
	}
}

GLImage::GLImage(const PfPolygon& polygon, const PfColor& color, GLenum mode, bool coordRelativeToBorder, bool stat) :
	m_mode(mode), m_verticesCount(polygon.count()), m_textureIndex(0), m_valid(polygon.count() > 0), m_static(!coordRelativeToBorder || stat), m_angle(0.0), m_centerX(0.0), m_centerY(0.0)
{
	m_data_t = new GLfloat[6*m_verticesCount];

	try
	{
		int n = 0;
		float minX = MAX_NUMBER, minY = MAX_NUMBER, maxX = -MAX_NUMBER, maxY = -MAX_NUMBER;
		for (int i=0;i<m_verticesCount;i++)
		{
			m_data_t[n++] = color.getR();
			m_data_t[n++] = color.getG();
			m_data_t[n++] = color.getB();

			m_data_t[n++] = coordRelativeToBorder?polygon.pointAt(i).getX()*(1-2*SYSTEM_BORDER_WIDTH) + SYSTEM_BORDER_WIDTH:polygon.pointAt(i).getX();
			if (m_data_t[n-1] > maxX)
				maxX = m_data_t[n-1];
			if (m_data_t[n-1] < minX)
				minX = m_data_t[n-1];
			m_data_t[n++] = polygon.pointAt(i).getY() * (float) g_windowHeight / g_windowWidth;
			if (m_data_t[n-1] > maxY)
				maxY = m_data_t[n-1];
			if (m_data_t[n-1] < minY)
				minY = m_data_t[n-1];
			m_data_t[n++] = 0.0;
		}

		// Calcul du centre par centre de la bounding box.
		m_centerX = (minX + maxX) / 2;
		m_centerY = (minY + maxY) / 2;
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Erreur lors de la lecture des points du polygone lors de la création de la GLImage.", "GLImage", e);
	}
}

GLImage::GLImage(const PfPolygon& polygon, unsigned int textureIndex, const PfPolygon& coordPolygon, const PfColor& color, bool coordRelativeToBorder, bool stat) :
	m_mode(GL_TRIANGLE_FAN), m_verticesCount(polygon.count()), m_textureIndex(textureIndex), m_valid(polygon.count() > 0), m_static(!coordRelativeToBorder || stat), m_angle(0.0),
	m_centerX(0.0), m_centerY(0.0)
{
	m_data_t = new GLfloat[(textureIndex == 0)?6*m_verticesCount:8*m_verticesCount];

	try
	{
		int n = 0;
		float minX = MAX_NUMBER, minY = MAX_NUMBER, maxX = -MAX_NUMBER, maxY = -MAX_NUMBER;
		for (int i=0;i<m_verticesCount;i++)
		{
			if (textureIndex != 0)
			{
				m_data_t[n++] = coordPolygon.pointAt(i).getX();
				m_data_t[n++] = coordPolygon.pointAt(i).getY();
			}
			m_data_t[n++] = color.getR();
			m_data_t[n++] = color.getG();
			m_data_t[n++] = color.getB();

			m_data_t[n++] = coordRelativeToBorder?polygon.pointAt(i).getX()*(1-2*SYSTEM_BORDER_WIDTH) + SYSTEM_BORDER_WIDTH:polygon.pointAt(i).getX();
			if (m_data_t[n-1] > maxX)
				maxX = m_data_t[n-1];
			if (m_data_t[n-1] < minX)
				minX = m_data_t[n-1];
			m_data_t[n++] = polygon.pointAt(i).getY() * (float) g_windowHeight / g_windowWidth;
			if (m_data_t[n-1] > maxY)
				maxY = m_data_t[n-1];
			if (m_data_t[n-1] < minY)
				minY = m_data_t[n-1];
			m_data_t[n++] = 0.0;
		}

		// Calcul du centre par centre de la bounding box.
		m_centerX = (minX + maxX) / 2;
		m_centerY = (minY + maxY) / 2;
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Erreur lors de la lecture des points des polygones lors de la création de la GLImage.", "GLImage", e);
	}
}

GLImage::~GLImage()
{
	if (m_data_t != 0)
		delete [] m_data_t;
}

bool GLImage::isTextured() const
{
	return m_textureIndex != 0;
}

vector<PfPoint> GLImage::points() const
{
	vector<PfPoint> x_v;

	for (int i=0;i<m_verticesCount;i++)
	{
		if (m_textureIndex != 0)
			x_v.push_back(PfPoint(m_data_t[i*8+5], m_data_t[i*8+6]));
		else
			x_v.push_back(PfPoint(m_data_t[i*6+3], m_data_t[i*6+4]));
	}

	return x_v;
}

GLImage& GLImage::operator=(const GLImage& glImage)
{
    if (&glImage != this)
    {
        m_mode = glImage.m_mode;
        m_verticesCount = glImage.m_verticesCount;
        m_textureIndex = glImage.m_textureIndex;
        m_valid = glImage.m_valid;
        m_static = glImage.m_static;
        m_angle = glImage.m_angle;
        m_centerX = glImage.m_centerX;
        m_centerY = glImage.m_centerY;

        if (m_data_t != 0)
            delete [] m_data_t;
        if (glImage.m_data_t == 0)
            m_data_t = 0;
        else
        {
            int dataCount = (m_textureIndex!=0)?8*m_verticesCount:6*m_verticesCount;
            m_data_t = new GLfloat[dataCount];
            for (int i=0;i<dataCount;i++)
                m_data_t[i] = glImage.m_data_t[i];
        }
    }

	return *this;
}

