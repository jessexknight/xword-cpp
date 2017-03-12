#include <iostream>
#include "CImg.h"
using namespace cimg_library;

int main(void){
    CImg<unsigned char> grid(500,500,1,1);
    grid.fill(200);
    for (int i=0;i<500;i++){
        grid(i,250) = 0;
    }
    
    CImgDisplay grid_disp(grid,"CROSSWORD");
    while (!grid_disp.is_closed()) {
        grid_disp.wait();
    }
    
    return 0;
}
