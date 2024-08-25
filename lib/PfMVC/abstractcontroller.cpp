#include "abstractcontroller.h"

AbstractController::AbstractController(AbstractModel& r_model) : mp_model(&r_model), m_status(ALIVE) {}

void AbstractController::wakeUp()
{
	if (m_status == ASLEEP)
		m_status = ALIVE;
}
