#include "jungle.h"

#include <cstdlib>
#include "geometry.h"
#include "errors.h"
#include "misc.h"
#include "map.h"

Jungle::Jungle(const string& name, const PfRectangle& rect) : MapObject(name, rect)
{
    setFloating(true);
}

void Jungle::generateJungleElements(int minEleCount, int maxEleCount, float margin)
{
    changeCurrentAnimation(ANIM_IDLE, true);
    if (currentAnimationStatus() != ANIM_IDLE)
        throw PfException(__LINE__, __FILE__, "Impossible de charger l'animation ANIM_IDLE.");
    const PfAnimationFrame* pc_frame = &(getAnimationGroup().currentFrame());

    int imgCount = (int) (1./(pc_frame->textCoordRectangle().getW()-FLOAT_MARGIN)) * (int) (1./(pc_frame->textCoordRectangle().getH()-FLOAT_MARGIN));
    int elementsCount = RAND(minEleCount, maxEleCount);
    float x, y, w, h;
    int trial = 0;
    bool ok;

    m_elements_v.clear();

    for (int i=0;i<elementsCount;i++)
    {
        ok = true;

        w = RAND(60, 100)/100.0;
        h = RAND(60, 100)/100.0;
        x = RAND(0, 100)/100.0 * (1.0-margin*w+w*(1-margin)/2) - w*(1-margin)/2;
        y = RAND(0, 100)/100.0 * (1.0-margin*h+h*(1-margin)/2) - h*(1-margin)/2;

        // test des marges de positionnement
        PfRectangle r;
        for (unsigned int j=0, size=m_elements_v.size();j<size;j++)
        {
            r = m_elements_v[j].rect;
            if (x+w/2 - r.getX()+r.getW()/2 < margin)
            {
                trial++;
                i--;
                ok = false;
                break;
            }
            if (y+h/2 - r.getY()+r.getH()/2 < margin)
            {
                trial++;
                i--;
                ok = false;
                break;
            }
        }
        if (!ok)
        {
            if (trial < 100)
                continue;
            else
            {
                #ifndef NDEBUG
                throw PfException(__LINE__, __FILE__, "Impossible d'ajouter un élément lors de la création de la jungle après 10 tentatives.");
                #else
                i++;
                continue;
                #endif
            }
        }

        // ajout de l'élément
        m_elements_v.push_back(JungleElement(PfRectangle(x, y, w, h), RAND(1, imgCount)));
    }
}

Viewable* Jungle::generateViewable() const
{
    Viewable* p_rtn = new Viewable(getName(), PfRectangle(), getLayer(), 0, 0, PfRectangle(), PfColor::WHITE, isCoordRelativeToBorder(), isStatic());

    float x, y, w, h;
    const PfAnimationFrame* pc_frame = &(getAnimationGroup().currentFrame());
    int columns = (int) (1./(pc_frame->textCoordRectangle().getW()-FLOAT_MARGIN));

    GLImage* p_gl;
    for (unsigned int i=0, size=m_elements_v.size();i<size;i++)
    {
        x = rect_x() + m_elements_v[i].rect.getX()*rect_w();
        y = rect_y() + m_elements_v[i].rect.getY()*rect_h();
        w = m_elements_v[i].rect.getW()*rect_w();
        h = m_elements_v[i].rect.getH()*rect_h();
        PfRectangle r(x, y, w, h);
        p_gl = new GLImage(r, pc_frame->getTextureIndex(),
                           PfRectangle(((m_elements_v[i].textureIndex-1)%columns) * pc_frame->textCoordRectangle().getW(),
                                        1.0 - ((m_elements_v[i].textureIndex-1)/columns+1) * pc_frame->textCoordRectangle().getH(),
                                        pc_frame->textCoordRectangle().getW(), pc_frame->textCoordRectangle().getH()),
                           getColor(), isCoordRelativeToBorder(), isStatic());
        p_rtn->addViewable(new Viewable(getName()+"_ele"+itostr(i+1), *p_gl, Map::layerAt(r, getZ())));
    }

    return p_rtn;
}

void Jungle::saveData(ofstream& r_ofs) const
{
    MapObject::saveData(r_ofs);

    WRITE_ENUM(r_ofs, Jungle::SAVE_COUNT);
    WRITE_UINT(r_ofs, m_elements_v.size());

    WRITE_ENUM(r_ofs, Jungle::SAVE_RECTS);
    for (unsigned int i=0, size=m_elements_v.size();i<size;i++)
    {
        WRITE_FLOAT(r_ofs, m_elements_v[i].rect.getX());
        WRITE_FLOAT(r_ofs, m_elements_v[i].rect.getY());
        WRITE_FLOAT(r_ofs, m_elements_v[i].rect.getW());
        WRITE_FLOAT(r_ofs, m_elements_v[i].rect.getH());
    }

    WRITE_ENUM(r_ofs, Jungle::SAVE_INDEX);
    for (unsigned int i=0, size=m_elements_v.size();i<size;i++)
        WRITE_UINT(r_ofs, m_elements_v[i].textureIndex);

    WRITE_ENUM(r_ofs, Jungle::SAVE_END);
}

void Jungle::loadData(DataPackage& r_data)
{
    MapObject::loadData(r_data);

    bool cnt = true, errFlag = true, vectorCreated = false;
    int section;
    unsigned int eleCount = 0;
    float x, y, w, h;
    m_elements_v.clear();
    while (cnt && !r_data.isOver())
    {
        section = r_data.nextEnum();
        switch (section)
        {
        case Jungle::SAVE_COUNT:
            eleCount = r_data.nextUInt();
            errFlag = false;
            break;
        case Jungle::SAVE_RECTS:
            if (errFlag)
                throw PfException(__LINE__, __FILE__, "Chargement d'une Jungle : erreur car rectangles chargés avant le nombre d'éléments.");
            for (unsigned int i=0;i<eleCount;i++)
            {
                x = r_data.nextFloat();
                y = r_data.nextFloat();
                w = r_data.nextFloat();
                h = r_data.nextFloat();
                if (vectorCreated)
                    m_elements_v[i].rect = PfRectangle(x, y, w, h);
                else
                    m_elements_v.push_back(JungleElement(PfRectangle(x, y, w, h), 0));
            }
            vectorCreated = true;
            break;
        case Jungle::SAVE_INDEX:
            if (errFlag)
                throw PfException(__LINE__, __FILE__, "Chargement d'une Jungle : erreur car textures chargées avant le nombre d'éléments.");
            for (unsigned int i=0;i<eleCount;i++)
            {
                if (vectorCreated)
                    m_elements_v[i].textureIndex = r_data.nextUInt();
                else
                    m_elements_v.push_back(JungleElement(PfRectangle(), r_data.nextUInt()));
            }
            vectorCreated = true;
            break;
        case Jungle::SAVE_END:
        default:
            cnt = false;
            break;
        }
    }

    if (section < 0 || section > Jungle::SAVE_END || r_data.isOver())
        throw PfException(__LINE__, __FILE__, "Données non valides.");
}
