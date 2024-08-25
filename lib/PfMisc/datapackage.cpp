#include "datapackage.h"

#include "misc.h"
#include "errors.h"

DataPackage::DataPackage() : m_intIndex(0), m_uintIndex(0), m_charIndex(0), m_ucharIndex(0), m_floatIndex(0), m_doubleIndex(0), m_stringIndex(0), m_enumIndex(0), m_over(false) {}

DataPackage::DataPackage(ifstream& r_ifs, unsigned int dataCount) :
    m_intIndex(0), m_uintIndex(0), m_charIndex(0), m_ucharIndex(0), m_floatIndex(0), m_doubleIndex(0), m_stringIndex(0), m_enumIndex(0), m_over(false)
{
    try
    {
        DataType type;
        int intV, enumV;
        unsigned int uintV;
        char charV, tmp;
        unsigned char ucharV;
        float floatV;
        double doubleV;
        string strV;
        unsigned int i = 0;
        int ignoreCount = 0;
        do
        {
            for (;ignoreCount>0;ignoreCount--)
                r_ifs.read(&tmp, sizeof(char));

            r_ifs.read((char*) &type, sizeof(DataType));
            switch (type)
            {
                case DT_END:
                    break;
                case DT_INT:
                    r_ifs.read((char*) &intV, sizeof(int));
                    m_int_v.push_back(intV);
                    break;
                case DT_UINT:
                    r_ifs.read((char*) &uintV, sizeof(unsigned int));
                    m_uint_v.push_back(uintV);
                    break;
                case DT_CHAR:
                    r_ifs.read(&charV, sizeof(char));
                    m_char_v.push_back(charV);
                    break;
                case DT_UCHAR:
                    r_ifs.read((char*) &ucharV, sizeof(unsigned char));
                    m_uchar_v.push_back(ucharV);
                    break;
                case DT_FLOAT:
                    r_ifs.read((char*) &floatV, sizeof(float));
                    m_float_v.push_back(floatV);
                    break;
                case DT_DOUBLE:
                    r_ifs.read((char*) &doubleV, sizeof(double));
                    m_double_v.push_back(doubleV);
                    break;
                case DT_STRING:
                    strV.clear();
                    do
                    {
                        r_ifs.read(&tmp, sizeof(char));
                        if (tmp != '\0')
                            strV += tmp;
                    }
                    while (r_ifs.good() && tmp != '\0');
                    m_string_v.push_back(strV);
                    break;
                case DT_ENUM:
                    r_ifs.read((char*) &enumV, sizeof(int));
                    m_enum_v.push_back(enumV);
                    break;
                case DT_IGNORE:
                    r_ifs.read((char*) &ignoreCount, sizeof(int));
                    break;
                default:
                    throw PfException(__LINE__, __FILE__, "Type de données non valide.");
            }
            i++;
        } while (i < dataCount || (dataCount == 0 && type != DT_END && r_ifs.good()));

        if (r_ifs.fail())
        {
            if (dataCount > 0)
                throw PfException(__LINE__, __FILE__, string("Erreur de lecture, seulement ") + itostr(i) + " valeurs trouvées sur " + itostr(dataCount) + ".");
            else
                throw PfException(__LINE__, __FILE__, string("Erreur de lecture, valeur DT_END non trouvée."));
        }
    }
    catch (PfException& e)
    {
        throw ConstructorException(__LINE__, __FILE__, "Données non valides.", "DataPackage", e);
    }
}

int DataPackage::nextInt()
{
    int x = 0;

    if (m_intIndex < m_int_v.size())
    {
        x = m_int_v[m_intIndex];
        m_intIndex++;
    }
    else
        m_over = true;

    return x;
}

void DataPackage::addInt(int val)
{
    m_int_v.push_back(val);
}

unsigned int DataPackage::nextUInt()
{
    unsigned int x = 0;

    if (m_uintIndex < m_uint_v.size())
    {
        x =  m_uint_v[m_uintIndex];
        m_uintIndex++;
    }
    else
        m_over = true;

    return x;
}

void DataPackage::addUInt(unsigned int val)
{
    m_uint_v.push_back(val);
}

char DataPackage::nextChar()
{
    char x = 0;

    if (m_charIndex < m_char_v.size())
    {
        x = m_char_v[m_charIndex];
        m_charIndex++;
    }
    else
        m_over = true;

    return x;
}

void DataPackage::addChar(char val)
{
    m_char_v.push_back(val);
}

unsigned char DataPackage::nextUChar()
{
    unsigned char x = 0;

    if (m_ucharIndex < m_uchar_v.size())
    {
        x = m_uchar_v[m_ucharIndex];
        m_ucharIndex++;
    }
    else
        m_over = true;

    return x;
}

void DataPackage::addUChar(unsigned char val)
{
    m_uchar_v.push_back(val);
}

float DataPackage::nextFloat()
{
    float x = 0.0;

    if (m_floatIndex < m_float_v.size())
    {
        x = m_float_v[m_floatIndex];
        m_floatIndex++;
    }
    else
        m_over = true;

    return x;
}

void DataPackage::addFloat(float val)
{
    m_float_v.push_back(val);
}

double DataPackage::nextDouble()
{
    double x = 0.0;

    if (m_doubleIndex < m_double_v.size())
    {
        x = m_double_v[m_doubleIndex];
        m_doubleIndex++;
    }
    else
        m_over = true;

    return x;
}

void DataPackage::addDouble(double val)
{
    m_double_v.push_back(val);
}

string DataPackage::nextString()
{
    string x_str;

    if (m_stringIndex < m_string_v.size())
    {
        x_str = m_string_v[m_stringIndex];
        m_stringIndex++;
    }
    else
        m_over = true;

    return x_str;
}

void DataPackage::addString(const string& val)
{
    m_string_v.push_back(val);
}

int DataPackage::nextEnum()
{
    int x = 0;

    if (m_enumIndex < m_enum_v.size())
    {
        x = m_enum_v[m_enumIndex];
        m_enumIndex++;
    }
    else
        m_over = true;

    return x;
}

void DataPackage::addEnum(int val)
{
    m_enum_v.push_back(val);
}

void DataPackage::clear()
{
    m_int_v.clear();
    m_uint_v.clear();
    m_char_v.clear();
    m_uchar_v.clear();
    m_float_v.clear();
    m_double_v.clear();
    m_string_v.clear();
    m_enum_v.clear();
}
