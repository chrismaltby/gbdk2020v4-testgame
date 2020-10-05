#include "DataManager.h"
#include "Scroll.h"

int main()
{
    // Init LCD
    LCDC_REG = 0x67;

    // Position Window Layer
    WX_REG = 7;
    WY_REG = MAXWNDPOSY + 1U;

    LoadScene(0);

    RefreshScroll();
    
    return 0;
}
