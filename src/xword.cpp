#include "xword.h"

#define BLANK   ' '
#define MAXDIM  str_arr_numel(iWords) // WARNING: ENSURE iWords EXIST LOCALLY
#define MIDLEN  len/2
#define MIDWID  wid/2
#define BOXSIZE 30

/* XWORD CLASS FUNCTIONS *******************************************************
   ****************************************************************************/

/* INITIALIZATIONS ---------------------------------------------------------- */

int XWORD::init_xword(char ** iWords) {
    // Assign words and build BLANK array
    if (set_words(iWords)) {
        return 1;
    }
    blank_arr(MAXDIM, MAXDIM);
    
    // Add the first word horizontally in the middle
    vdir = 0;
    if (add_word_to_arr(wordsToAdd[0],MIDLEN,MIDWID)) {
        return 1;
    }
    change_list_word(0);
    append_yxDir(MIDLEN, MIDWID, vdir);
    vdir = !vdir;
    return 0;
}

int XWORD::set_words(char ** iWords) {
    int w = 0;
    while(iWords[w] != NULL) {
        wordsToAdd.push_back((string)iWords[w]);
        w++;
    }
    if (w) {
        return 0; // at least one word
    } else {
        return 1; // iWords was empty (NULL)
    }
}

void XWORD::blank_arr(int ilen, int iwid) {
    len = ilen; // array height
    wid = iwid; // array width
    string temp (iwid, BLANK); // blank string
    for (int y=0; y<ilen; y++) {
        arr.push_back(temp);
    }
    return;
}

/* BUILDING ARRAY ----------------------------------------------------------- */

int XWORD::opt_gen_arr(char ** iWords, int iMaxiter) {
    if (init_xword(iWords)) {
        return 1;  // iWords is empty - can't do anything else
    }
    srand(time(0));
    
    // Build iMaxiter xword arrays ("attempt"), and overwrite the current array 
    // each time there are as many or more added words and the array is smaller
    for(int k=0; k<iMaxiter; k++) {
        XWORD attempt;                   // Delete and remake this every loop
        attempt.init_xword(iWords);      // Assign words
        if (!attempt.build_arr()) {      // Build the crossword array
            // Want build with maximum words added, then the minimum array size
            if (attempt.wordsToAdd.size() <= this->wordsToAdd.size()) {
                if (((attempt.len)*(attempt.wid)) < ((this->len)*(this->wid))) {
                    this->arr        = attempt.arr;
                    this->len        = attempt.len;
                    this->wid        = attempt.wid;
                    this->wordsToAdd = attempt.wordsToAdd;
                    this->wordsAdded = attempt.wordsAdded;
                    this->yxDirAdded = attempt.yxDirAdded;
                }
            }
        }
    }
    
    if ((len == MAXDIM) || (wid == MAXDIM)) {
        return 1; // Array is same size: optimization or cropping failure
    }    
    return 0;
}

int XWORD::build_arr() {
    int failedToAdd = 0;
    while (wordsToAdd.size() && !failedToAdd){
        failedToAdd = select_add_word(); // attempts all possible adds
        vdir = !vdir; // Alternate vertical-horizontal add attempts
    }
    if (crop_arr()) { // Crop the array to remove BLANK edges
        return 1;
    }
    return 0;
}

int XWORD::select_add_word() {
    // randomized order of words and indexes in the array
    vector<int> yxInd = random_nums(len*wid);
    vector<int> wInd = random_nums((int)wordsToAdd.size());
    unsigned w,ww,t = 0;
    int k,x,y = 0;

    for(w=0; w<wordsToAdd.size(); w++) { // For all wordsToAdd remaining
        ww = wInd[w];                    // Select a random word
        for(k=0; k<(len*wid); k++) {     // For all array indexes
            x = yxInd[k] % wid;          // Calculate x from linear index
            y = (yxInd[k]-x)/wid;        // Calculate y from linear index
            // For all letters in the word
            for(t=0; t<wordsToAdd[ww].length(); t++) {
                // If the letter matches the cell in the array
                if (arr[y][x] == wordsToAdd[ww][t]) {
                    // Try to add the word
                    if (!check_add_word(ww,t,y,x)) {
                        // If success, change from wordsToAdd to wordsAdded
                        change_list_word(ww);
                        return 0;
                    }
                }
            }
        }
    }
    return 1; // impossible to add any words
}

int XWORD::check_add_word(int iw, int it, int iy, int ix) {
    int k,yy,xx = 0;
    bool intersect = 0;
    int wLength = (int)wordsToAdd[iw].length();
    
    if (vdir) {
        // For all letters in the word, test the intersections and spacing
        for (k=0; k<wLength; k++) {
            yy = iy + k - it;
            intersect = (bool) (k == it);
            // Test for intersections: if letters not equal, old must be blank
            if (arr[yy][ix] != wordsToAdd[iw][k]) {
                if (arr[yy][ix] != BLANK) {
                    return 1;
                }
            } else {
                intersect = 1;
            }
            // First letter: previous cell must be blank
            if (k == 0) {
                if ( (arr[yy-1][ix] != BLANK) ||
                    ((arr[yy][ix+1] != BLANK) && (arr[yy][ix-1] == BLANK)) ) {
                    return 1;
                }
            }
            // Last letter: next cell must be blank
            if (k == wLength-1) {
                if (arr[yy+1][ix] != BLANK) {
                    return 1;
                }
            }
            // Not intersection: adjacent cells must be blank
            if (!intersect) {
                if ( (arr[yy][ix-1] != BLANK) || arr[yy][ix+1] != BLANK ) {
                    return 1;
                }
            }
        }
        add_word_to_arr(wordsToAdd[iw], (iy-it), ix);
        append_yxDir((iy-it), ix, vdir);
        return 0;
        
    } else {
        for (k=0; k<wLength; k++) {
            xx = ix + k - it;
            intersect = (bool) (k == it);
            // Test for intersections: if letters not equal, old must be blank
            if (arr[iy][xx] != wordsToAdd[iw][k]) {
                if (arr[iy][xx] != BLANK) {
                    return 1;
                }
            } else {
                intersect = 1;
            }
            // First letter: previous cell must be blank, as well as adjacent
            if (k == 0) {
                if ( (arr[iy][xx-1] != BLANK) ||
                    ((arr[iy+1][xx] != BLANK) && (arr[iy-1][xx] == BLANK)) ) {
                    return 1;
                }
            }
            // Last letter: next cell must be blank
            if (k == wLength-1) {
                if (arr[iy][xx+1] != BLANK) {
                    return 1;
                }
            }
            // Not intersection: adjacent cells must be blank
            if (!intersect) {
                if ( (arr[iy-1][xx] != BLANK) || arr[iy+1][xx] != BLANK ) {
                    return 1;
                }
            }
        }
        add_word_to_arr(wordsToAdd[iw], iy, (ix-it));
        append_yxDir(iy, (ix-it), vdir);
        return 0;
    }
}

int XWORD::add_word_to_arr(string iWord, int iy, int ix) {
    for (string::iterator letter = iWord.begin(); letter != iWord.end(); letter++) {
        arr[iy][ix] = *letter;
        if (vdir) {
            if (iy++ >= len) {
                return 1;
            }
        } else {
            if (ix++ > wid) {
                return 1;
            }
        }
    }
    return 0;
}

void XWORD::change_list_word(int iw) {
    wordsAdded.push_back(wordsToAdd[iw]);
    wordsToAdd.erase(wordsToAdd.begin()+iw);
}

void XWORD::append_yxDir(int iy, int ix, int ivdir) {
    vector<int> newRow;
    newRow.push_back(iy);
    newRow.push_back(ix);
    newRow.push_back(ivdir);
    
    yxDirAdded.push_back(newRow);
}

int XWORD::crop_arr() {
    int y,x;
    int ymax = MIDLEN;
    int ymin = MIDLEN;
    int xmax = MIDWID;
    int xmin = MIDWID;
    // Find bounds
    for (y=0; y<len; y++) {
        for (x=0; x<wid; x++) {
            if (arr[y][x] != BLANK) {
                if (y<ymin) {
                    ymin = y;
                }
                if (y>ymax) {
                    ymax = y;
                }
                if (x<xmin) {
                    xmin = x;
                }
                if (x>xmax) {
                    xmax = x;
                }
            }
        }
    }
    
    // Cropping Vertical
    len = ymax - ymin + 1;
    arr.erase(arr.begin(),arr.begin()+ymin);
    arr.erase(arr.begin()+len,arr.end());
    // Cropping Horizontal
    wid = xmax - xmin + 1;
    for (y=0; y<len; y++) {
        arr[y] = arr[y].substr(xmin,wid);
    }
    // Adjusting yxDirAdded
    for (int w=0; w<(int)yxDirAdded.size(); w++) {
        yxDirAdded[w][0] = yxDirAdded[w][0] - ymin;
        yxDirAdded[w][1] = yxDirAdded[w][1] - xmin;
    }
    return 0;
}

/* XWORD PRINTING ----------------------------------------------------------- */

void XWORD::print_wordsToAdd() {
    for (unsigned w=0; w<wordsToAdd.size(); w++) {
        cout << wordsToAdd[w] << endl;
    }
}

void XWORD::print_wordsAdded() {
    for (unsigned w=0; w<wordsAdded.size(); w++) {
        cout << wordsAdded[w] << endl;
    }
}

void XWORD::print_arr() {
    for (unsigned y=0; y<arr.size(); y++) {
        cout << arr[y] << endl;
    }
}

/* XWORD_UI CLASS FUNCTIONS ****************************************************
   ****************************************************************************/
  
void XWORD_UI::build_grid(XWORD xword, bool soln) {
    int clrBkgd = 255;
    char clrGrid[3] = {000,000,000};
    const int clrText[3] = {255,000,255};
    
    char* ch = new char[3];
    int xx,yy;
    
    grid.assign((xword.wid*BOXSIZE)+1, (xword.len*BOXSIZE)+1, 1, 3, clrBkgd);

    // Drawing boxes
    for (int y=0; y<xword.len; y++) {
        for (int x=0; x<xword.wid; x++) {
            if( xword.arr[y][x] != BLANK) {
                add_box(y,x,clrGrid);
                if (soln) {
                    ch = strncpy(ch, &xword.arr[y][x], 1);
                    CIL::CImg<unsigned char> textbox;
                    textbox.draw_text(0, 0, ch, clrText, clrBkgd, 1, 0.6*BOXSIZE);
                    xx = (x+0.5)*BOXSIZE - textbox.width()/2;
                    yy = (y+0.5)*BOXSIZE - textbox.height()/2;
                    grid.draw_text(xx, yy, ch, clrText, clrBkgd, 1, 0.6*BOXSIZE);
                }
            }
        }
    }

    for (int w=0; w<(int)xword.yxDirAdded.size(); w++) {
        sprintf(ch, "%01d", w+1);
        xx = (xword.yxDirAdded[w][1]+0.1)*BOXSIZE;
        yy = (xword.yxDirAdded[w][0]+0.1)*BOXSIZE;
        grid.draw_text(xx, yy, ch, clrGrid, clrBkgd, 1, 0.4*BOXSIZE);
    }
    
    delete ch;    
    gridDisp.assign(grid,"CROSSWORD",3,0,1);
}

void XWORD_UI::add_box(int iy, int ix, char clrGrid[]) {
    int ymin = (iy)  *BOXSIZE;
    int ymax = (iy+1)*BOXSIZE;
    int xmin = (ix)  *BOXSIZE;
    int xmax = (ix+1)*BOXSIZE;
    
    for (int y=ymin; y<=ymax; y++) {
        for (int c=0; c<3; c++) {
            grid(xmin,y,c) = clrGrid[c];
            grid(xmax,y,c) = clrGrid[c];
        }
    }
    for (int x=xmin; x<=xmax; x++) {
        for (int c=0; c<3; c++) {
            grid(x,ymin,c) = clrGrid[c];
            grid(x,ymax,c) = clrGrid[c];
        }
    }
}
   
void XWORD_UI::render_grid() {
    gridDisp.show();
    while(!gridDisp.is_closed()) {
        gridDisp.wait();
    }
}
   
/* HELPER FUNCTIONS ************************************************************
   ****************************************************************************/
   
vector<int> random_nums(int imax) {
    
    vector<int> nums;
    for (int i=0; i<imax; i++) {
        nums.push_back(i);
    }
    random_shuffle(nums.begin(),nums.end());
    return nums;
}

int str_arr_numel(char ** strs) {
    int numel = 0;
    int s = 0;
    while (strs[s] != NULL) {
        numel += strlen(strs[s]);
        s++;
    }
    return numel;
}

/* EOF *************************************************************************
   ****************************************************************************/

