
#include <stdio.h>

extern char arrd2[10000];
extern int columns, rows;

int modulo (int left, int right) {
    int ans = left%right;
    if (ans<0) {
        ans += right;
    }
    return ans;
}
    
int cell(int i, int j){
    int col = columns;
    int row = rows;
    int liveNeighbors = 0;
    int index = col*i + j;
    
    
    int neighbor1 = col*modulo(i-1, row)    + modulo(j-1 ,col );
    int neighbor2 = col*modulo(i-1 , row)   +  j;
    int neighbor3 = col*modulo(i-1 , row)   + modulo(j+1 ,col );
    int neighbor4 = col*i                   + modulo(j-1 ,col );
    int neighbor5 = col*i                   + modulo(j+1 ,col );
    int neighbor6 = col* modulo(i+1 ,row )  + modulo(j-1 ,col );
    int neighbor7 = col*modulo(i+1 ,row )   +  j;
    int neighbor8 = col*modulo(i+1 ,row )   + modulo(j+1 ,col );
        
    if(arrd2[neighbor1] != 0)   liveNeighbors++;
    if(arrd2[neighbor2] != 0)   liveNeighbors++;
    if(arrd2[neighbor3] != 0)   liveNeighbors++;
    if(arrd2[neighbor4] != 0)   liveNeighbors++;
    if(arrd2[neighbor5] != 0)   liveNeighbors++;
    if(arrd2[neighbor6] != 0)   liveNeighbors++;
    if(arrd2[neighbor7] != 0)   liveNeighbors++;
    if(arrd2[neighbor8] != 0)   liveNeighbors++;
    
    if(arrd2[index] == 0){
        if(liveNeighbors == 3){
            return 1;
        }else{
            return 0;
        }
    }else{
        if(liveNeighbors == 2 || liveNeighbors == 3){
            if(arrd2[index] == 9)
                return 9;
            return arrd2[index]+1;
        }
        return 0;
    }
}