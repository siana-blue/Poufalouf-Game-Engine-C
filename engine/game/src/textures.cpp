#include "textures.h"

#include "errors.h"
#include "misc.h"
#include "glfunc.h"
#include "enum.h"

unsigned int PfMapTextureSet::s_newTerrainIndex = TERRAIN_TEXTURE_INDEX;
map<string, unsigned int> g_texIndexes_map;

PfRectangle rectFromTextureIndex(int index, unsigned int rowsCount, unsigned int columnsCount, float xFraction, float yFraction, float xOffset, float yOffset)
{
	if (index < 0)
		return PfRectangle(0.0, 0.0, 0.0, 0.0);

	return PfRectangle(TERRAIN_PIXEL_SIZE/2 + (float) (index%columnsCount+xOffset)/columnsCount,
					   1.0 + TERRAIN_PIXEL_SIZE/2 - (float) (index/columnsCount+1-yOffset)/rowsCount,
					   1./columnsCount*xFraction - TERRAIN_PIXEL_SIZE,
					   1./rowsCount*yFraction - TERRAIN_PIXEL_SIZE);
}

PfMapTextureSet::PfMapTextureSet(const string& texName) : m_name(texName)
{
	if (texName == "")
		return;

	try
	{
		string str = string(TEXTURES_DIR) + texName + "." + TEXTURES_EXT;
		ifstream ifs(str.c_str(), ios::binary);
		if (!ifs.is_open())
			throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", str);

		int val, val2, val3;
		ifs.read((char*) &val, sizeof(int));
		if (val != PFGAME_VERSION)
			throw FileException(__LINE__, __FILE__,
								string("Le fichier de textures est d'une version dépassée.\nVersion du jeu : ") + itostr(PFGAME_VERSION) + "\nVersion du fichier : " + itostr(val), str);

		if (g_texIndexes_map.find(texName) != g_texIndexes_map.end())
			m_terrainIDs_v.push_back(g_texIndexes_map[texName]);
		else
		{
			m_terrainIDs_v.push_back(s_newTerrainIndex);
			g_texIndexes_map[texName] = s_newTerrainIndex;
		}

		int counter;
		ifs.read((char*) &counter, sizeof(int));
		for (int i=0;i<counter;i++)
		{
			if (i > 0)
				m_terrainIDs_v.push_back(m_terrainIDs_v[0]+1);
			addTexture(ifs, m_terrainIDs_v[i]);
			if (m_terrainIDs_v[i] >= s_newTerrainIndex)
				s_newTerrainIndex++;
		}

		// Remplissages par défaut

		for (int i=0;i<TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT;i++)
		{
			m_bordersLinks_map[i] = pair<unsigned int, int>(0, 0);
			m_cliffsLinks_map[i] = pair<unsigned int, int>(0, 0);
			m_cliffBordersLinks_map[i] = pair<unsigned int, int>(0, 0);
		}

		// Remplissages d'après fichier

		map<unsigned int, pair<unsigned int, int> >* links_map_p;
		ifs.read((char*) &val, sizeof(int));
		while (ifs.good() && val != END_OF_LIST_INT)
		{
			if (val < 0 || val > ENUM_PF_TEXTURE_SCRIPT_SECTION_COUNT)
				throw FileException(__LINE__, __FILE__, "Valeur de section de script non valide.", str);
			switch ((PfTextureScriptSection) val)
			{
				case TEXSCT_SPR:
					links_map_p = &m_spreadingLinks_map;
					break;
				case TEXSCT_BRD:
					links_map_p = &m_bordersLinks_map;
					break;
				case TEXSCT_CLF:
					links_map_p = &m_cliffsLinks_map;
					break;
				case TEXSCT_CLB:
					links_map_p = &m_cliffBordersLinks_map;
					break;
				case TEXSCT_FLC:
					links_map_p = &m_cliffFloorLinks_map;
					break;
			}

			ifs.read((char*) &counter, sizeof(int));
			for (int i=0;i<counter;i++)
			{
				ifs.read((char*) &val, sizeof(int));
				ifs.read((char*) &val2, sizeof(int));
				ifs.read((char*) &val3, sizeof(int));
				(*links_map_p)[val2] = pair<unsigned int, int>(m_terrainIDs_v[val], val3);
				if (links_map_p == &m_spreadingLinks_map)
				{
					ifs.read((char*) &val, sizeof(int));
					m_spreadingLayers_map[val2] = val;
				}
			}

			ifs.read((char*) &val, sizeof(int));
		}

		if (m_terrainIDs_v.size() == 0)
			throw FileException(__LINE__, __FILE__, "Le fichier est incomplet : il faut au moins un nom de fichier texture valide.", str);

		if (ifs.fail())
			throw FileException(__LINE__, __FILE__, "Le fichier de textures est incomplet.", str);

		ifs.close();
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de construire ce jeu de texture.", "PfMapTextureSet", e);
	}
}
unsigned int PfMapTextureSet::terrainId(unsigned int index) const
{
	if (index >= m_terrainIDs_v.size())
		throw ArgumentException(__LINE__, __FILE__, string("Indice non valide, nombre de terrains : ") + itostr(m_terrainIDs_v.size()) + ".", "index", "PfMapTextureSet::terrainId");

	return m_terrainIDs_v[index];
}

int PfMapTextureSet::spreadingLayer(unsigned int index) const
{
	map<unsigned int, int>::const_iterator it = m_spreadingLayers_map.find(index);
	if (it == m_spreadingLayers_map.end())
		return 0;
	return it->second;
}

unsigned int PfMapTextureSet::spreadingTexture(unsigned int index) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_spreadingLinks_map.find(index);
	if (it == m_spreadingLinks_map.end())
		return 0;
	return it->second.first;
}

int PfMapTextureSet::spreadingIndex(unsigned int index, PfOrientation::PfCardinalPoint ori) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_spreadingLinks_map.find(index);
	if (it == m_spreadingLinks_map.end())
		return -1;

	PfOrientation o(ori);
	int rtn = it->second.second;

	if (o.toFlag() & PfOrientation::SOUTH)
	{
		if (rtn < TERRAIN_CELLS_COUNT)
			return -1;
		else
			rtn -= TERRAIN_CELLS_COUNT;
	}
	if (o.toFlag() & PfOrientation::WEST)
	{
		if (rtn % TERRAIN_CELLS_COUNT == TERRAIN_CELLS_COUNT - 1)
			return -1;
		else
			rtn += 1;
	}
	if (o.toFlag() & PfOrientation::NORTH)
	{
		if (rtn >= TERRAIN_CELLS_COUNT*(TERRAIN_CELLS_COUNT-1))
			return -1;
		else
			rtn += TERRAIN_CELLS_COUNT;
	}
	if (o.toFlag() & PfOrientation::EAST)
	{
		if (rtn % TERRAIN_CELLS_COUNT == 0)
			return -1;
		else
			rtn -= 1;
	}

	return rtn;
}

unsigned int PfMapTextureSet::borderTexture(unsigned int index) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_bordersLinks_map.find(index);
	if (it == m_bordersLinks_map.end())
		return 0;
	return it->second.first;
}

int PfMapTextureSet::borderIndex(unsigned int index, PfOrientation::PfOrientationValue ori) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_bordersLinks_map.find(index);
	if (it == m_bordersLinks_map.end())
		return -1;

	int rtn = it->second.second;
	switch (ori)
	{
	case PfOrientation::EAST:
		if (rtn >= TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT-1)
			return -1;
		rtn += 1;
		break;
	case PfOrientation::SOUTH:
		if (rtn >= TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT-2)
			return -1;
		rtn += 2;
		break;
	case PfOrientation::WEST:
		if (rtn >= TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT-3)
			return -1;
		rtn += 3;
		break;
	default:
		break;
	}

	return rtn;
}

unsigned int PfMapTextureSet::cliffTexture(unsigned int index) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_cliffsLinks_map.find(index);
	if (it == m_cliffsLinks_map.end())
		return 0;
	return it->second.first;
}

int PfMapTextureSet::cliffIndex(unsigned int index, PfOrientation::PfOrientationValue ori) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_cliffsLinks_map.find(index);
	if (it == m_cliffsLinks_map.end())
		return -1;

	int rtn = it->second.second;
	switch (ori)
	{
	case PfOrientation::WEST:
		if (rtn >= TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT-1)
			return -1;
		rtn += 1;
		break;
	case PfOrientation::EAST:
		if (rtn >= TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT-2)
			return -1;
		rtn += 2;
		break;
	default:
		break;
	}

	return rtn;
}

unsigned int PfMapTextureSet::cliffBorderTexture(unsigned int index) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_cliffBordersLinks_map.find(index);
	if (it == m_cliffBordersLinks_map.end())
		return 0;
	return it->second.first;
}

int PfMapTextureSet::cliffBorderIndex(unsigned int index, PfOrientation::PfCardinalPoint ori) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_cliffBordersLinks_map.find(index);
	if (it == m_cliffBordersLinks_map.end())
		return -1;

	PfOrientation o(ori);
	int rtn = it->second.second;

	if (o.toFlag() & PfOrientation::NORTH)
	{
		if (rtn < TERRAIN_CELLS_COUNT)
			return -1;
		else
			rtn -= TERRAIN_CELLS_COUNT;
	}
	if (o.toFlag() & PfOrientation::EAST)
	{
		if (rtn % TERRAIN_CELLS_COUNT == TERRAIN_CELLS_COUNT - 1)
			return -1;
		else
			rtn += 1;
	}
	if (o.toFlag() & PfOrientation::SOUTH)
	{
		if (rtn >= TERRAIN_CELLS_COUNT*(TERRAIN_CELLS_COUNT-1))
			return -1;
		else
			rtn += TERRAIN_CELLS_COUNT;
	}
	if (o.toFlag() & PfOrientation::WEST)
	{
		if (rtn % TERRAIN_CELLS_COUNT == 0)
			return -1;
		else
			rtn -= 1;
	}

	return rtn;
}

unsigned int PfMapTextureSet::cliffFloorTexture(unsigned int index) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_cliffFloorLinks_map.find(index);
	if (it == m_cliffFloorLinks_map.end())
		return 0;
	return it->second.first;
}

int PfMapTextureSet::cliffFloorIndex(unsigned int index, PfOrientation::PfOrientationValue ori) const
{
	map<unsigned int, pair<unsigned int, int> >::const_iterator it = m_cliffFloorLinks_map.find(index);
	if (it == m_cliffFloorLinks_map.end())
		return -1;

	int rtn = it->second.second;
	switch (ori)
	{
	case PfOrientation::WEST:
		break;
	case PfOrientation::EAST:
		if (rtn >= TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT-2)
			return -1;
		rtn += 2;
		break;
	default:
		if (rtn >= TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT-1)
			return -1;
		rtn += 1;
		break;
	}

	return rtn;
}
