#include "menumvcsystem.h"

#include "menumodel.h"
#include "glview.h"
#include "menucontroller.h"
#include "wad.h"

MenuMVCSystem::MenuMVCSystem(const string& name, PfWad& r_wad)
{
	mp_model = new MenuModel(name, r_wad);
	mp_view = new GLView;
	mp_controller = new MenuController(*(dynamic_cast<MenuModel*>(mp_model)));
	m_selfDestructible = true;

	mp_model->addView(*mp_view);
}

void MenuMVCSystem::addText(const string& text)
{
	MenuModel* p_model = dynamic_cast<MenuModel*>(mp_model);

	p_model->addText(text);
}

void MenuMVCSystem::clearTexts(bool update)
{
	MenuModel* p_model = dynamic_cast<MenuModel*>(mp_model);

	p_model->clearTexts(update);
}

void MenuMVCSystem::addValue(int value)
{
	MenuModel* p_model = dynamic_cast<MenuModel*>(mp_model);

	p_model->addValue(value);
}

void MenuMVCSystem::clearValues(bool update)
{
	MenuModel* p_model = dynamic_cast<MenuModel*>(mp_model);

	p_model->clearValues(update);
}

