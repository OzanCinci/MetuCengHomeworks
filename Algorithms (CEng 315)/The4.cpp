#include "the4.h"


int dp_sln(char**& arr1, char**& arr2, int nrow, int ncol1, int ncol2, int**& mem){ //dynamic programming

    int del, ins, cross, min, temp;
    bool isReo=true;
    int fullElementCounter;

    mem[0][0]=0;
    for(int i=1;i<ncol2+1;i++){
        fullElementCounter=0;
        for(int w=0;w<nrow;w++){
            if (arr2[w][i-1]!='-')
                fullElementCounter++;
        }
        mem[0][i]=mem[0][i-1]+fullElementCounter;
    }
        
    for(int i=1;i<ncol1+1;i++){
        fullElementCounter=0;
        for(int w=0;w<nrow;w++){
            if (arr1[w][i-1]!='-')
                fullElementCounter++;
        }
        mem[i][0]=mem[i-1][0]+fullElementCounter;
    }
        
    for(int i=1;i<ncol1+1;i++){
        for(int j=1;j<ncol2+1;j++){
            // set everything zero
            del=0;
            ins=0;
            cross=0;
            isReo=true;
            
            // calculate insertion
            for(int l=0;l<nrow;l++)
                if (arr2[l][j-1]!='-')
                    ins++;
            
            // calculate deletion
            for(int l=0;l<nrow;l++)
                if (arr1[l][i-1]!='-')
                    del++;
        
            // copy two arrays
            char tempArray1[nrow];
            char tempArray2[nrow];
            for(int x=0;x<nrow;x++){
                tempArray1[x]=arr1[x][i-1];
                tempArray2[x]=arr2[x][j-1];
            }
            
            // sort them
            for (int i = 0; i < nrow; i++) {     
                for (int j = i+1; j < nrow; j++) {     
                   if(tempArray1[i] > tempArray1[j]) {    
                       temp = tempArray1[i];    
                       tempArray1[i] = tempArray1[j];    
                       tempArray1[j] = temp;    
                   }    
                   if(tempArray2[i] > tempArray2[j]) {    
                       temp = tempArray2[i];    
                       tempArray2[i] = tempArray2[j];    
                       tempArray2[j] = temp;    
                   }     
                }     
            }    
            // compare them to check if it is replacement or reordering
            for(int x=0;x<nrow;x++){
                if (tempArray1[x]!=tempArray2[x])
                    isReo=false;
            }
            
            //calculate cross
           if (isReo==false){
                for(int q=0;q<nrow;q++)
                    if (arr1[q][i-1]!=arr2[q][j-1]){
                        if (arr1[q][i-1]=='-' || arr2[q][j-1]=='-')
                            cross+=2;
                        else
                            cross+=1;
                    }
           } else {
                for(int q=0;q<nrow;q++)
                    if (arr1[q][i-1]!=arr2[q][j-1])
                        cross+=1;
           }
            
            // find min
            min = ins+mem[i][j-1];
            
            if (min> del+mem[i-1][j])
                min = del+mem[i-1][j];
                
            if (min > cross+mem[i-1][j-1])
                min = cross+mem[i-1][j-1];
                
            mem[i][j]=min;
        }
    }
    return mem[ncol1][ncol2];
}
