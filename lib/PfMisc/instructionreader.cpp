#include "instructionreader.h"

InstructionReader::InstructionReader() : m_instruction(INSTRUCTION_NONE) {}

PfReturnCode InstructionReader::readInstruction(PfInstruction instruction, const DataPackage& data)
{
    m_instruction = instruction;
    m_instructionValues = data;
    return processInstruction();
}

PfReturnCode InstructionReader::readInstruction(PfInstruction instruction, int value)
{
    DataPackage data;
    data.addInt(value);
    return readInstruction(instruction, data);
}

DataPackage& InstructionReader::instructionValues()
{
    return m_instructionValues;
}
