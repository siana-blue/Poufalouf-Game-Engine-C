#include "mvcsystem.h"

#include <cassert>
#include "abstractmodel.h"
#include "abstractview.h"
#include "abstractcontroller.h"

MVCSystem::MVCSystem(AbstractModel* p_model, AbstractView* p_view, AbstractController* p_controller, bool selfDestructible) :
	mp_model(p_model), mp_view(p_view), mp_controller(p_controller), m_selfDestructible(selfDestructible)
{
	if (mp_model != 0 && mp_view != 0) // pas d'assertion ici, car le constructeur mère standard des classes dérivées est appelé sans argument.
		mp_model->addView(*mp_view);
}

MVCSystem::~MVCSystem()
{
	if (m_selfDestructible)
	{
		assert(mp_model && mp_view && mp_controller);

		delete mp_model;
		delete mp_view;
		delete mp_controller;
	}
}

bool MVCSystem::run(bool once)
{
	assert(mp_model && mp_view && mp_controller);

	mp_controller->wakeUp();
	while (mp_controller->getStatus() == AbstractController::ALIVE)
	{
		if (mp_controller->isAvailable() && !once)
			mp_controller->pollInput();
		else
			mp_controller->flushInput();
		if (mp_controller->getStatus() == AbstractController::ALIVE)
		{
			mp_controller->update();
			mp_model->notifyAll();
			mp_model->updateItems();
			mp_view->display();
		}
		if (once)
			mp_controller->setStatus(AbstractController::ASLEEP);
	}

	return (mp_controller->getStatus() != AbstractController::DEAD);
}

const DataPackage& MVCSystem::values() const
{
	assert(mp_controller);

	return mp_controller->getValues();
}
