#include "abstractview.h"

#include <cassert>
#include <vector>
#include <SDL.h>
#include "viewable.h"
#include "modelitem.h"
#include "misc.h"

AbstractView::AbstractView() : m_timeCounter(SDL_GetTicks()) {}

AbstractView::~AbstractView()
{
	for (map<string, Viewable*>::iterator it=mpn_viewables_map.begin();it!=mpn_viewables_map.end();++it)
	{
		assert(it->second);
		delete it->second;
	}
}

void AbstractView::display()
{
	int nextLayer = 0;
	vector<const Viewable*> q_vws_v, q_vws2_v;
	vector<const Viewable*>* q_vws_v_p = &q_vws_v;
	vector<const Viewable*>* q_vws2_v_p = &q_vws2_v;
	vector<const Viewable*>* q_temp_v_p;
	const Viewable* q_vw;

	uint32_t time = SDL_GetTicks() - m_timeCounter;
	if (time < FPS_RATE)
		SDL_Delay(FPS_RATE - time);
	m_timeCounter = SDL_GetTicks();

	initializeDisplay();

	for (map<string, Viewable*>::const_iterator it=mpn_viewables_map.begin();it!=mpn_viewables_map.end();++it)
	{
		assert(it->second);
		if (it->second->isVisible())
		{
			if (viewportContains(*(it->second)))
			{
		  		if (it->second->getLayer() == 0) // affiche les viewables de premier plan
			   		displayViewable(*(it->second));
				else // stocke les viewables des autres plans dans une liste
				{
					q_vws_v.push_back(it->second);
					if (it->second->getLayer() < nextLayer || nextLayer == 0) // détermine le prochain plan à afficher
				   		nextLayer = it->second->getLayer();
				}
			}
			// dans la première boucle, les Viewable liés sont traités comme des Viewables normaux et sont triés
			for (unsigned int i=0, size=it->second->viewablesCount();i<size;i++)
			{
				q_vw = it->second->viewableAt(i);
				if (viewportContains(*q_vw))
				{
					if (q_vw->getLayer() == 0)
						displayViewable(*q_vw);
					else
					{
						q_vws_v.push_back(q_vw);
						if (q_vw->getLayer() < nextLayer || nextLayer == 0)
							nextLayer = q_vw->getLayer();
					}
				}
			}
		}
	}

	while (!q_vws_v_p->empty()) // tant qu'il reste des plans dont les viewables n'ont pas encore été affichés
	{
		int currentLayer = nextLayer;
		for (unsigned int i=0, size=q_vws_v_p->size();i<size;i++) // boucle pour chaque viewable restant
		{
			q_vw = (*q_vws_v_p)[i];
			if (q_vw->getLayer() == currentLayer) // ce viewable appartient au plan en cours d'affichage ?
				displayViewable(*q_vw);
			else
			{
				q_vws2_v_p->push_back(q_vw); // liste des viewables non encore affichés
				if (q_vw->getLayer() < nextLayer || nextLayer == currentLayer) // détermine le prochain plan à afficher
					nextLayer = q_vw->getLayer();
			}
		}

		q_vws_v_p->clear(); // la liste utilisée lors de cette itération est vidée
		q_temp_v_p = q_vws2_v_p; // la liste des viewables non encore affichés est transférée vers un pointeur temporaire
		q_vws2_v_p = q_vws_v_p; // le pointeur 2 est interverti avec le pointeur 1 pour redevenir vide et jouer son rôle de stockage de viewables non encore affichés...
		q_vws_v_p = q_temp_v_p; // ... à la prochaine itération, tandis que q_vws_v_p regagne son rôle de liste à parcourir.
	}

	finalizeDisplay();
}

void AbstractView::update(const map<string, ModelItem*>& p_modelItems_map)
{
	for (map<string, ModelItem*>::const_iterator it=p_modelItems_map.begin();it!=p_modelItems_map.end();++it)
	{
		assert(it->second);
		// ModelItem dont la clé est encore inconnue
		if (mpn_viewables_map.find(it->first) == mpn_viewables_map.end())
		{
			Viewable* p_vw = it->second->generateViewable();
			assert(p_vw);
			mpn_viewables_map.insert(pair<string, Viewable*>(it->first, p_vw));
		}
		else if (it->second->isModified())
		{
			removeObjectFromMap(mpn_viewables_map, it->first);
			Viewable* p_vw = it->second->generateViewable();
			mpn_viewables_map.insert(pair<string, Viewable*>(it->first, p_vw));
		}
		switch (it->second->getStatus())
		{
			case ModelItem::VISIBLE:
				mpn_viewables_map[it->first]->setVisible(true);
				break;
			case ModelItem::INVISIBLE:
				mpn_viewables_map[it->first]->setVisible(false);
				break;
			case ModelItem::DEAD:
				removeObjectFromMap(mpn_viewables_map, it->first);
				break;
			default:
				break;
		}
	}
}

bool AbstractView::viewportContains(const Viewable&) const
{
	return true;
}

