#include "abstractmodel.h"

#include <cassert>
#include "modelitem.h"
#include "abstractview.h"
#include "misc.h"
#include "errors.h"

AbstractModel::~AbstractModel()
{
	for (map<string, ModelItem*>::iterator it=mpn_modelItems_map.begin();it!=mpn_modelItems_map.end();++it)
	{
		assert(it->second);
		delete it->second;
	}
}

void AbstractModel::addView(AbstractView& r_view)
{
	for (unsigned int i=0;i<mp_views_v.size();i++)
	{
		if (mp_views_v[i] == &r_view) // ne pas ajouter deux fois la même vue.
			return;
	}
	mp_views_v.push_back(&r_view);
}

void AbstractModel::notifyAll()
{
	pair<float, float> viewport_pair = viewportData();
	for (unsigned int i=0;i<mp_views_v.size();i++)
	{
		assert(mp_views_v[i]);
		mp_views_v[i]->updateViewport(viewport_pair.first, viewport_pair.second);
		mp_views_v[i]->update(mpn_modelItems_map);
	}
}

void AbstractModel::addItem(ModelItem* p_item)
{
	if (mpn_modelItems_map.find(p_item->getName()) != mpn_modelItems_map.end()) // remplacer un objet existant
		removeObjectFromMap(mpn_modelItems_map, p_item->getName());

	mpn_modelItems_map.insert(pair<string, ModelItem*>(p_item->getName(), p_item));
}

void AbstractModel::removeItem(const string& keyName)
{
	if (mpn_modelItems_map.find(keyName) == mpn_modelItems_map.end())
		throw ArgumentException(__LINE__, __FILE__, "Aucun objet nommé " + keyName + " dans ce modèle.", "keyName", "AbstractModel::removeItem");
	assert(mpn_modelItems_map[keyName]);
	mpn_modelItems_map[keyName]->setStatus(ModelItem::DEAD);
}

void AbstractModel::showItem(const string& keyName)
{
	if (mpn_modelItems_map.find(keyName) == mpn_modelItems_map.end())
		throw ArgumentException(__LINE__, __FILE__, "Aucun objet nommé " + keyName + " dans ce modèle.", "keyName", "AbstractModel::showItem");
	assert(mpn_modelItems_map[keyName]);
	mpn_modelItems_map[keyName]->setStatus(ModelItem::VISIBLE);
}

void AbstractModel::hideItem(const string& keyName)
{
	if (mpn_modelItems_map.find(keyName) == mpn_modelItems_map.end())
		throw ArgumentException(__LINE__, __FILE__, "Aucun objet nommé " + keyName + " dans ce modèle.", "keyName", "AbstractModel::hideItem");
	assert(mpn_modelItems_map[keyName]);
	mpn_modelItems_map[keyName]->setStatus(ModelItem::INVISIBLE);
}

bool AbstractModel::contains(const string& keyName) const
{
	if (mpn_modelItems_map.find(keyName) == mpn_modelItems_map.end())
		return false;
	return true;
}

void AbstractModel::clearItem(const string& keyName)
{
	if (mpn_modelItems_map.find(keyName) == mpn_modelItems_map.end())
		throw ArgumentException(__LINE__, __FILE__, "Aucun objet nommé " + keyName + " dans ce modèle.", "keyName", "AbstractModel::clearItem");

	#ifndef NDEBUG
	#ifdef DBG_MODELITEM
	LOG("Deleting: " << keyName << "\n"); // à placer avant la suppression car la clé et le nom sont des références vers la même chaîne.
	#endif
	#endif

	removeObjectFromMap(mpn_modelItems_map, keyName);

	#ifndef NDEBUG
	#ifdef DBG_MODELITEM
	LOG("Deleted OK !\n");
	#endif
	#endif
}

vector<ModelItem*> AbstractModel::modelItems()
{
	vector<ModelItem*> p_items_v;
	for(map<string, ModelItem*>::iterator it=mpn_modelItems_map.begin();it!=mpn_modelItems_map.end();++it)
		p_items_v.push_back(it->second);

	return p_items_v;
}

