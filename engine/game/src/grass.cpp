#include "grass.h"

#include "animation.h"
#include "glimage.h"
#include "errors.h"
#include "misc.h"

Grass::Grass(const string& name, const PfRectangle& rect) : MapObject(name, rect)
{
    setFloating(true);
}

void Grass::generateElements(unsigned int rows)
{
    changeCurrentAnimation(ANIM_IDLE, true);
    if (currentAnimationStatus() != ANIM_IDLE)
        throw PfException(__LINE__, __FILE__, "Impossible de charger l'animation ANIM_IDLE.");
    const PfAnimationFrame* pc_frame = &(getAnimationGroup().currentFrame());
    int imgCount = (int) (1./(pc_frame->textCoordRectangle().getH()-FLOAT_MARGIN));

    m_rows_v.clear();
    for (unsigned int i=0;i<rows;i++)
        m_rows_v.push_back(RAND(1, imgCount));
}

void Grass::generateElements(const Grass& rc_grass)
{
    m_rows_v = rc_grass.m_rows_v;
}

Viewable* Grass::generateViewable() const
{
    Viewable* p_rtn = new Viewable(getName(), PfRectangle(), getLayer(), 0, 0, PfRectangle(), PfColor::WHITE, isCoordRelativeToBorder(), isStatic());

    const PfAnimationFrame* pc_frame = &(getAnimationGroup().currentFrame());
    GLImage *p_glImage, *p_glImage2;

    int code = getObjCode();

    bool mainLine, leftBorder, rightBorder;
    for (unsigned int i=0, size=m_rows_v.size();i<size;i++)
    {
        p_glImage = 0;
        p_glImage2 = 0;

        mainLine = false;
        leftBorder = rightBorder = false;

        if (code == 0 || ((code & PfOrientation::SOUTH) && i < size/8) || ((code & PfOrientation::NORTH) && i >= size*7/8))
            mainLine = true;
        else
        {
            if (code & PfOrientation::EAST)
                leftBorder = true;
            else if ((code & PfOrientation::NORTH_EAST) && i >= size*7/8)
                leftBorder = true;
            else if ((code & PfOrientation::SOUTH_EAST) && i < size/8)
                leftBorder = true;

            if (code & PfOrientation::WEST)
                rightBorder = true;
            else if ((code & PfOrientation::NORTH_WEST) && i >= size*7/8)
                rightBorder = true;
            else if ((code & PfOrientation::SOUTH_WEST) && i < size/8)
                rightBorder = true;
        }

        if (mainLine)
        {
            p_glImage = new GLImage(PfRectangle(rect_x(), rect_y()+(float) i/size*rect_h(), rect_w(), rect_h()),
                                    pc_frame->getTextureIndex(),
                                    PfRectangle(0.33, 1.0 - (m_rows_v[i]-1) * pc_frame->textCoordRectangle().getH(),
                                                pc_frame->textCoordRectangle().getW(), pc_frame->textCoordRectangle().getH()),
                                    getColor(), isCoordRelativeToBorder(), isStatic());
        }
        else
        {
            if (leftBorder)
            {
                p_glImage = new GLImage(PfRectangle(rect_x(), rect_y()+(float) i/size*rect_h(), rect_w(), rect_h()),
                                        pc_frame->getTextureIndex(),
                                        PfRectangle(0.0, 1.0 - (m_rows_v[i]-1) * pc_frame->textCoordRectangle().getH(),
                                                    pc_frame->textCoordRectangle().getW(), pc_frame->textCoordRectangle().getH()),
                                        getColor(), isCoordRelativeToBorder(), isStatic());
            }
            if (rightBorder)
            {
                if (!p_glImage)
                {
                    p_glImage = new GLImage(PfRectangle(rect_x(), rect_y()+(float) i/size*rect_h(), rect_w(), rect_h()),
                                            pc_frame->getTextureIndex(),
                                            PfRectangle(0.66, 1.0 - (m_rows_v[i]-1) * pc_frame->textCoordRectangle().getH(),
                                                        pc_frame->textCoordRectangle().getW(), pc_frame->textCoordRectangle().getH()),
                                            getColor(), isCoordRelativeToBorder(), isStatic());
                }
                else
                {
                    p_glImage2 = new GLImage(PfRectangle(rect_x(), rect_y()+(float) i/size*rect_h(), rect_w(), rect_h()),
                                             pc_frame->getTextureIndex(),
                                             PfRectangle(0.66, 1.0 - (m_rows_v[i]-1) * pc_frame->textCoordRectangle().getH(),
                                                         pc_frame->textCoordRectangle().getW(), pc_frame->textCoordRectangle().getH()),
                                             getColor(), isCoordRelativeToBorder(), isStatic());
                }
            }
        }
        if (p_glImage)
        {
            p_rtn->addViewable(new Viewable(getName() + "_row_" + itostr(i), *p_glImage,
                                            getLayer() - (int) (((float) i/size + FLOAT_MARGIN)*MAP_STEPS_PER_CELL) * MAP_MAX_HEIGHT + 1));
                                            // +1 pour le layer afin qu'à Y égal les herbes soient devant
        }
        if (p_glImage2)
        {
            p_rtn->addViewable(new Viewable(getName() + "_row_" + itostr(i) + "_2", *p_glImage2,
                               getLayer() - (int) (((float) i/size + FLOAT_MARGIN)*MAP_STEPS_PER_CELL) * MAP_MAX_HEIGHT + 1));
        }

        delete p_glImage;
        if (p_glImage2)
            delete p_glImage2;
    }

    return p_rtn;
}

void Grass::saveData(ofstream& r_ofs) const
{
    MapObject::saveData(r_ofs);

    WRITE_ENUM(r_ofs, Grass::SAVE_ROWS);
    WRITE_UINT(r_ofs, m_rows_v.size());
    for (unsigned int i=0;i<m_rows_v.size();i++)
        WRITE_UINT(r_ofs, m_rows_v[i]);

    WRITE_ENUM(r_ofs, Grass::SAVE_END);
}

void Grass::loadData(DataPackage& r_data)
{
    MapObject::loadData(r_data);

    bool cnt = true;
	int section;
	unsigned int rowsCount;
	m_rows_v.clear();
    while (cnt && !r_data.isOver())
    {
        section = r_data.nextEnum();
        switch (section)
        {
        case Grass::SAVE_ROWS:
            rowsCount = r_data.nextUInt();
            for (unsigned int i=0;i<rowsCount;i++)
                m_rows_v.push_back(r_data.nextUInt());
            break;
        case Grass::SAVE_END:
        default:
            cnt = false;
            break;
        }
    }

    if (section < 0 || section > Grass::SAVE_END || r_data.isOver())
        throw PfException(__LINE__, __FILE__, "Données non valides.");
}
