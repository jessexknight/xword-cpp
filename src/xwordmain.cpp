#include "xword.h"
#define MAXITER 100
/*
  
*/

int main(int argc, char** argv) {
    XWORD x;
    XWORD_UI u;

    x.opt_gen_arr(&argv[1], MAXITER);

    cout << "--- TO  ADD ---\n";
    x.print_wordsToAdd();
    cout << "---------------\n";
    cout << "---- ADDED ----\n";
    x.print_wordsAdded();
    cout << "---------------\n";
    cout << "----- ARR -----\n";
    x.print_arr();
    cout << "---------------\n";
    
    u.build_grid(x,1);
    u.render_grid();
    
    return 0;
}