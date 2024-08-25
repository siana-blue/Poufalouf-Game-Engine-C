#include "glmodel.h"

#include <vector>
#include "modelitem.h"

#ifndef NDEBUG

#include <map>
#include <string>
#include "pfgui.h"
#include "misc.h"

#endif

PfCamera* GLModel::camera()
{
	return &m_camera;
}

void GLModel::showDebugInfo()
{
	#ifndef NDEBUG
	PfWidget* p_label = findItem<PfWidget>("DEBUG_LABEL");
	if (g_debug)
	{
	    unsigned int size = g_dbInt_v_map.size() + g_dbFloat_v_map.size();
	    if (size == 0)
            return;
		if (p_label == 0)
		{
            p_label = new PfWidget("DEBUG_LABEL", PfRectangle(0.0, 1.0-size*0.02, 0.4, size*0.02), MAX_LAYER, size);
            p_label->setStatic(true);
            p_label->setColor(PfColor::BLACK);
            addItem(p_label);
		}
		p_label->clearText();
		string str;
		for (map<string, vector<int> >::iterator it=g_dbInt_v_map.begin();it!=g_dbInt_v_map.end();it++)
		{
			for (unsigned int i=0, sz=it->second.size();i<sz;i++)
				str += string((i>0?" ; ":it->first + " = ")) + itostr(it->second[i]);
			if (str != "")
				p_label->addText(str);
			str.clear();
		}
		for (map<string, vector<float> >::iterator it=g_dbFloat_v_map.begin();it!=g_dbFloat_v_map.end();it++)
		{
			for (unsigned int i=0, sz=it->second.size();i<sz;i++)
				str += string((i>0?" ; ":it->first + " = ")) + ftostr(it->second[i]);
			if (str != "")
				p_label->addText(str);
			str.clear();
		}
		showItem("DEBUG_LABEL");
	}
	else if (p_label)
		hideItem("DEBUG_LABEL");
	g_dbInt_v_map.clear();
	g_dbFloat_v_map.clear();
	#endif
}

void GLModel::updateItems()
{
	vector<ModelItem*> p_mi_v = modelItems();
	for (unsigned int i=0, size=p_mi_v.size();i<size;i++)
	{
		ModelItem* p_tmp = p_mi_v[i];
		if (p_tmp == 0) // ne devrait jamais se produire
			continue;
		p_tmp->setModified(false);
		p_tmp->update();
		switch (p_tmp->getStatus())
		{
			case ModelItem::VISIBLE:
				p_tmp->setStatus(ModelItem::UNCHANGED);
				break;
			case ModelItem::DEAD:
				clearItem(p_tmp->getName());
				break;
			default:
				break;
		}
	}
}

pair<float, float> GLModel::viewportData() const
{
	return m_camera.viewport();
}

