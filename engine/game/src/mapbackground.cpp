#include "mapbackground.h"

#include "misc.h"

MapBackground::MapBackground(const string& name, unsigned int mapRowsCount, unsigned int mapColumnsCount) : AnimatedGLItem(name)
{
    m_rows = 2.0 + (int) (mapRowsCount*MAP_CELL_SIZE + 1.0); // 2.0 pour une marge de 1.0 vers le haut et vers le bas
    m_columns = 2.0 + (int) (mapColumnsCount*MAP_CELL_SIZE + 1.0);
}

Viewable* MapBackground::generateViewable() const
{
    const PfAnimationFrame* pc_frame = &(getAnimationGroup().currentFrame());

    Viewable* p_rtn = new Viewable(getName()+"(0;0)", PfRectangle(-1.0, -1.0, 1.0, 1.0), getLayer(),
                                   pc_frame->getTextureIndex(), 0, pc_frame->textCoordRectangle());

    for (unsigned int i=0;i<m_rows;i++)
    {
        for (unsigned int j=0;j<m_columns;j++)
        {
            if (i == 0 && j == 0)
                continue;
            p_rtn->addViewable(new Viewable(getName()+"("+itostr(i)+";"+itostr(j)+")", PfRectangle(-1.0+i, -1.0+j, 1.0, 1.0), getLayer(),
                                            pc_frame->getTextureIndex(), 0, pc_frame->textCoordRectangle()));
        }
    }

    return p_rtn;
}
