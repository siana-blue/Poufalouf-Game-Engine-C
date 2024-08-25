#include "pngtoglloader.h"

#include <stdio.h>
#include "errors.h"

void png_read_data(png_structp p_png, png_bytep data_t, png_size_t length)
{
	PNGDataBuffer* p_memData = (PNGDataBuffer*) png_get_io_ptr(p_png);
	memcpy(data_t, p_memData->data(length), length);
	p_memData->offset += length;
}

PNGToGLLoader::PNGToGLLoader(const string& fileName) : m_name(0)
{
	try
	{
		FILE* p_file = fopen(fileName.c_str(), "rb");
		if (!p_file)
			throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", fileName);

		PNGDataBuffer dummy;
		loadPNG(dummy, p_file);

		fclose(p_file);
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, string("Impossible de créer une image valide à partir de ce fichier : ") + fileName + ".", "PNGToGLLoader", e);
	}
}

PNGToGLLoader::PNGToGLLoader(PNGDataBuffer& r_data) : m_name(0)
{
	loadPNG(r_data);
	if (!r_data.isValid())
		throw ConstructorException(__LINE__, __FILE__, "Erreur dans les données de l'image PNG.", "PNGToGLLoader");
}

PNGToGLLoader::~PNGToGLLoader()
{
	if (m_texels_t != 0)
		delete [] m_texels_t;
}

void PNGToGLLoader::addTextureToGL()
{
	GLuint texName;
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, m_texels_t);

	m_name = texName;
}

void PNGToGLLoader::bindTextureToGL()
{
	if (m_name == 0)
		throw PfException(__LINE__, __FILE__, "Tentative de liaison d'une texture non initialisée au contexte OpenGL.");

	glBindTexture(GL_TEXTURE_2D, m_name);
}

bool PNGToGLLoader::isAddedToGL()
{
	return (m_name != 0);
}

void PNGToGLLoader::loadPNG(PNGDataBuffer& r_data, FILE* p_file)
{
	if (!r_data.isValid() && !p_file)
		throw ArgumentException(__LINE__, __FILE__, "Aucun argument valide n'a été transmis à la cette méthode.", "r_data/p_file", "PNGToGLLoader::loadPNG");

	png_bytep* p_rows_t = 0;

	if (r_data.isValid())
	{
		if (!png_check_sig((png_bytep) r_data.data(8), 8*sizeof(char)))
			throw PfException(__LINE__, __FILE__, "Fichier PNG non valide.");
		if (!r_data.isValid())
			throw PfException(__LINE__, __FILE__, "Lecture non valide des données..");
	}
	else
	{
		png_byte sig[8];
		fread(sig, 1, sizeof(sig), p_file);
		if (!png_check_sig(sig, sizeof(sig)))
			throw PfException(__LINE__, __FILE__, "Fichier PNG non valide.");
	}

	png_structp p_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!p_png)
		throw PfException(__LINE__, __FILE__, "Erreur lors de la création de la structure PNG.");
	png_infop p_info = png_create_info_struct(p_png);
	if (!p_info)
	{
		png_destroy_read_struct(&p_png, 0, 0);
		throw PfException(__LINE__, __FILE__, "Erreur lors de la création de l'info PNG.");
	}

	if (setjmp(png_jmpbuf(p_png)))
	{
		png_destroy_read_struct(&p_png, &p_info, 0);

		throw PfException(__LINE__, __FILE__, "Erreur inconnue (PNG).");
	}

	if (!p_file)
		png_set_read_fn(p_png, (png_voidp*) &r_data, png_read_data);
	else
	{
		png_init_io(p_png, p_file);
		png_set_sig_bytes(p_png, 8*sizeof(char));
	}

	png_read_info(p_png, p_info);

	// transformations à effectuer lors de la lecture du fichier

	int bitDepth, colorType;
	bitDepth = png_get_bit_depth(p_png, p_info);
	colorType = png_get_color_type(p_png, p_info);
	if (colorType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(p_png);
	else if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(p_png);
	if (png_get_valid(p_png, p_info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(p_png);
	if (bitDepth == 16)
		png_set_strip_16(p_png);
	else if (bitDepth < 8)
		png_set_packing(p_png);

	png_read_update_info(p_png, p_info);
	if (!r_data.isValid() && !p_file)
		throw PfException(__LINE__, __FILE__, "Lecture invalide (PNG).");

	// chargement de l'image

	png_uint_32 w, h;
	png_get_IHDR(p_png, p_info, &w, &h, &bitDepth, &colorType, 0, 0, 0);
	m_width = w;
	m_height = h;
	switch (colorType)
	{
		case PNG_COLOR_TYPE_GRAY:
			m_format = GL_LUMINANCE;
			m_internalFormat = 1;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			m_format = GL_LUMINANCE_ALPHA;
			m_internalFormat = 2;
			break;
		case PNG_COLOR_TYPE_RGB:
			m_format = GL_RGB;
			m_internalFormat = 3;
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			m_format = GL_RGBA;
			m_internalFormat = 4;
			break;
		default:
			break;
	}
	m_texels_t = new GLubyte[m_width*m_height*m_internalFormat];
	p_rows_t = new png_bytep[m_height];
	for (unsigned int i=0;i<m_height;i++)
	{
		p_rows_t[i] = m_texels_t + (m_height - (i + 1)) * m_width * m_internalFormat;
	}
	png_read_image(p_png, p_rows_t);
	delete [] p_rows_t;

	png_read_end(p_png, 0);
	png_destroy_read_struct(&p_png, &p_info, 0);
}

