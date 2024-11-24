#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// Preprocess the charJump table (Bad Character Rule)
void computeCharJumpTable(const char P[], int m, int charJump[]) {
    int ch;
    for (ch = 0; ch < CHAR_MAX; ch++) {
        charJump[ch] = m; 
    }
    for (int i = 1; i <= m; i++) {
        charJump[(unsigned char)P[i-1]] = m - i; 
    }
}

// Preprocess the slide table for the matchJump (Good Suffix Rule)
void computeSlideTable(const char P[], int m, int slide[]) {
   int j = m-1, k = 0, base = 0;
   slide[0] = 0;
   slide[m] = 1;

   for (int i = m-2; i>=0; i--){
        if (P[j] == P[i]){
            j--;
            k++;
        } else {
            if(j != m){
                if(slide[m-k] == m) {slide[m-k] = m-i+1;}
                k=0;
                j=m-1;
            }
        }

        if (i == 0 && k != 0){
            base = m-k;
        }
   }

   if(base != 0){
        for(int i = 1; i<m; i++){
            if(slide[i] == m) slide[i] = base;
        }
   }
}


// Compute the matchJump table based on the slide table
void computeMatchJumpTable(int m, int slide[], int matchJump[]) {
    matchJump[0] = 0; // Not used
    for (int k = 0; k < m; k++) {
        matchJump[m - k] = k + slide[m - k];
    }
}


void boyer_moore(const char *text, const char *pattern, int *matches, int *count) {
    int n = strlen(text); 
    int m = strlen(pattern); 
    int j=m, k=m, match_count=0;
    int charJump[CHAR_MAX];
    int *slide = (int *)malloc((m + 1) * sizeof(int));
    int *matchJump = (int *)malloc((m + 1) * sizeof(int));

    for (int i = 0; i <= m; i++) {
        slide[i] = m;
        matchJump[i] = 0;
    }

    // Preprocess the tables
    computeCharJumpTable(pattern, m, charJump);
    computeSlideTable(pattern, m, slide);
    computeMatchJumpTable(m, slide, matchJump);

    //Print out the charJump and MatchJump table if needed
    /*
    for (int q = 1; q<=m; q++){
        printf("charjump[%c]: %d\n", P[q-1], charJump[(unsigned char)P[q-1]]);
    }

    for (int p = 1; p<=m; p++){
        printf("matchJump[%d]: %d\n", p, matchJump[p]);
    }
    */

    while (j <= n) {

        if (k<1){ 
            //printf("Pattern found at index %d to index %d\n", j, j+m-1); 
            matches[match_count++] = j; // Store match index
            j++;
        }

        if (text[j-1] == pattern[k-1]) {j--; k--;}
        else {
            /*
            printf("Mismatch at text[%d] = '%c'\n", j - 1, text[j - 1]);
            printf("%d, charJump: %d, MatchJump: %d\n", j, charJump[(unsigned char)text[j-1]], matchJump[k]);
            */
            j+=MAX(charJump[(unsigned char)text[j-1]], matchJump[k]);
            k=m;
        }
    }

    *count = match_count;

    free(slide);
    free(matchJump);

    //printf("Pattern not found");
}


