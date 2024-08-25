#include "multiphases.h"

#include "errors.h"

MultiPhases::MultiPhases() : m_newPhase(true) {}

void MultiPhases::switchPhase(const string& phaseName)
{
    if (m_phasesNames_set.find(phaseName) == m_phasesNames_set.end())
        throw ArgumentException(__LINE__, __FILE__, string("La phase ") + phaseName + " n'est pas compatible avec cet objet.", "phaseName", "MultiPhases::switchPhase");

    if (m_currentPhaseName != phaseName)
        exitCurrentPhase();

    m_currentPhaseName = phaseName;
    m_newPhase = true;
}

void MultiPhases::addPhase(const string& phaseName)
{
    m_phasesNames_set.insert(phaseName);
}

void MultiPhases::playCurrentPhase()
{
    processCurrentPhase();
    m_newPhase = false;
}
