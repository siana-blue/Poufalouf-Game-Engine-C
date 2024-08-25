#include "mapeditormvcsystem.h"

#include "mapeditormodel.h"
#include "glview.h"
#include "mapeditorcontroller.h"

MapEditorMVCSystem::MapEditorMVCSystem(unsigned int rows, unsigned int columns, PfWad* p_wad, const string& texName)
{
	mp_model = new MapEditorModel(rows, columns, p_wad, texName);
	mp_view = new GLView;
	mp_controller = new MapEditorController(*(dynamic_cast<MapEditorModel*>(mp_model)));
	m_selfDestructible = true;

	mp_model->addView(*mp_view);
}

MapEditorMVCSystem::MapEditorMVCSystem(const string& fileName)
{
	mp_model = new MapEditorModel(fileName);
	mp_view = new GLView;
	mp_controller = new MapEditorController(*(dynamic_cast<MapEditorModel*>(mp_model)));
	m_selfDestructible = true;

	mp_model->addView(*mp_view);
}
