#ifndef XWORD_H
#define XWORD_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <string.h>
#include "CImg.h"

#define CIL cimg_library
using namespace std;

/*******************************************************************************
 This module contains functions for building a symbolic representation of a
 crossword and displaying it. The crossword is represented as a 2D array of
 characters and BLANKs. iWords are added in a random order to the array so
 that they intersect (matching letters) with words already added. Due to the
 randomness, some word orders may leave no valid intersections near the end,
 so sometimes not all words will be added. The building process is repeated
 iMaxiter times (randomly) to hopefully avoid this, and also to minimize the
 size required for the array. The crossword (solved) is displayed using CIL.*/

/*******************************************************************************
 CLASS DEFINITION: XWORD
 ******************************************************************************/

class XWORD {
  public:
  
 /* VARIABLS *******************************************************************
    ***************************************************************************/
   
    bool vdir;  // Alternating flag: adding words vertically or horizontally
    int len;    // Length (height) of crossword array (arr)
    int wid;    // Width of crossword array (arr)
    vector<string> wordsToAdd;  // List of words to add (during construction)
    vector<string> wordsAdded;  // List of words added (after construction)
    vector<vector<int>> yxDirAdded;  // List of topleft [y][x][vdir] for above
    vector<string> arr;         // Array representing constructed crossword,
                                //   where each char is a potential letter box
                                //   assigned either BLANK or a letter in a word

 /* INIT FUNCTIONS *************************************************************
    ***************************************************************************/

 /* init_xword -----------------------------------------------------------------
    Calls ::set_words(iWords); calculates an overestimate of maximum possible
    dimensions required for .arr; calls ::blank_arr with these dimensions; adds 
    the first word to .arr horizontally and changes the word's list from
    .wordsToAdd to .wordsAdded.  Returns 0 on success || 1 on fails: set_words
    fails | add_word_to_arr fails. */
    int init_xword(char ** iWords);
    
 /* set_words ------------------------------------------------------------------
    Uses std::vector::push_back to assign the input char ** list to .wordsToAdd.
    Returns 0 on success || 1 on fails: iWords is empty (NULL). */
    int set_words(char ** iWords);
    
 /* blank_arr ------------------------------------------------------------------
    Uses std::vector::push_back to assign a vector of BLANK strings to .arr such
    that it is ilen long (height) and iwid wide. Void return. */
    void blank_arr(int ilen, int iwid);

 /* ARR BIULDING FUNCTIONS *****************************************************
    ***************************************************************************/

 /* opt_gen_arr ----------------------------------------------------------------
    Generates an optimized array .arr which contains the maximum number of input
    words (iWords), and has the minimum size (.len*.wid); how: creates iMaxiter 
    xword structure attempts, and selects the optimal solution; seeds srand(); 
    calls init_xword and build_arr.  Returns 0 on success || 1 on fails:
    set_words fails | .arr is the same size as when it was initialized. */
    int opt_gen_arr(char ** iWords, int iMaxiter);
    
 /* build_arr ------------------------------------------------------------------
    Calls select_add_word repeatedly until no more words can be added to .arr 
    because it is not possible (select_add_word returns 0) or all words have 
    already been added; alternates vertical and horizontal add attempts.
    Returns 0 on success || 1 on fails: crop_arr. */
    int build_arr();
    
 /* select_add_word ------------------------------------------------------------
    Iterates randomly through all words, then randomly through all possible
    cells in .arr, then through all letters in the current word and checks for 
    an intersection (matching letter in .arr and in .wordsToAdd[ww]; if found, 
    check_add_word trys to add the word; if successful, change_list_word is
    called. Returns 0 on success (word added) || 1 on failure: it is impossible
    to add any more words to .arr (the buidl_arr loop will end, and at least one
    word will be left over). */
    int select_add_word();
    
 /* check_add_word -------------------------------------------------------------
    Try to add .wordToAdd[iw] to .arr in the direction vdir (1 = vertical,
    0 = horizontal), where the it'th letter lies is at .arr[iy][ix]; check for 
    BLANK conditions:
    + non-intersect letters: adjacent cells must be BLANK
    + start letters: adjacent cells and previous cell must be blank
    + end letters: following cell must be blank
    also check for additional chance intersections (2+ intersections for one new
    word) and loosen constraints  for these letters; if any conditions fail,
    return early, else call add_word_to_arr and append_yxDir. Return 0 on
    success || 1 on failure based on constraints above. */
    int check_add_word(int iw, int it, int iy, int ix);
    
 /* add_word_to_arr ------------------------------------------------------------
    Adds iWord to .arr in the direction vdir (1 = vertical, 0 = horizontal)
    while checking the arr bounds. Returns 0 on success || 1 on failure: aborted
    remaining letters due to bounds (this should never happen because .arr is
    initialized to an overestimate of the maximum possible required size).*/
    int add_word_to_arr(string iWord, int iy, int ix);
 
 /* change_list_word -----------------------------------------------------------
    Uses std::vector::push_back to add wordsToAdd[iw] to wordsAdded; removes
    wordToAdd[iw] from wordToAdd using std::vector::erase.  Void return. */
    void change_list_word(int iw);
    
 /* append_yxDir ---------------------------------------------------------------
    Uses std::vector::push_back to add (it order) the top / left starting point
    of each word, as it is added to wordsAdded. Void return. */
    void append_yxDir(int iy, int ix, int ivdir);
    
 /* crop_arr -------------------------------------------------------------------
    Computes non-BLANK rows and columns of .arr; uses std::vector::erase to
    remove BLANK rows of .arr and then std::string::substr to remove BLANK
    columns. Also adjusts the contents of yxDirAdded to index them appropriately
    within the new bounds of arr. Returns 0 on success || 1 on failure. */ 
    int crop_arr();
    
 /* PRINTING FUNCTIONS *********************************************************
    ***************************************************************************/
    
 /* print_wordsToAdd -----------------------------------------------------------
    This motherfucker uses cout << wordsToAdd[index] << endl. Void return. */
    void print_wordsToAdd();
    
 /* print_wordsToAdd -----------------------------------------------------------
    Why you still reading uses cout << wordsAdded[index] << endl. Void return */
    void print_wordsAdded();
    
 /* print_wordsToAdd -----------------------------------------------------------
    Holyshitwhatdyouwant uses cout << arr[y] << endl. Void return */
    void print_arr();
};

/*******************************************************************************
 CLASS DEFINITION: XWORD_UI
 ******************************************************************************/

 class XWORD_UI {
   public:
    CIL::CImg<unsigned char> grid;
    CIL::CImgDisplay gridDisp;
    
    void build_grid(XWORD xword, bool soln);
    void def_colours();
    void add_box(int iy, int ix, char clrGrid[]);
    void render_grid();
 };
 
/*******************************************************************************
 HELPER FUNCTIONS 
 ******************************************************************************/

/* random_nums -----------------------------------------------------------------
   Generates a random ordered series (vector<int>)  from 0 through imax. Returns
   the vector series. */
vector<int> random_nums(int imax);

/* str_arr_numel ---------------------------------------------------------------
   Calculates the total number of characters in a char ** array of strings. 
   Returns the total number. */
int str_arr_numel(char ** strs);


#endif