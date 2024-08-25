#include "mapmvcsystem.h"

#include "mapmodel.h"
#include "glview.h"
#include "mapcontroller.h"

MapMVCSystem::MapMVCSystem(const string& fileName)
{
	mp_model = new MapModel(fileName);
	mp_view = new GLView;
	mp_controller = new MapController(*(dynamic_cast<MapModel*>(mp_model)));
	m_selfDestructible = true;

	mp_model->addView(*mp_view);
}
