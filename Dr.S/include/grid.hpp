#ifndef __GRID__H__
#define __GRID__H_

enum class GridItem { pill, badguy, special };


class GridItemObject {
public:
    GridItemObject() = default;
    GridItemObject(const GridItem &i) : item(i) {
        
    }
    void setItem(const GridItem &i) {
        item = i;
    }
    
    GridItem &Item() {
        return item;
    }
protected:
    GridItem item;
};















#endif

