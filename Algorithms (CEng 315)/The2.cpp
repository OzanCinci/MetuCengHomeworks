#include "the2.h"
#include <cmath>

int powTen(int n){return n == 0 ? 1 : 10*powTen(n-1);}
int calcModerCount(int groupSize, int order, int maxDigitLength){
    if (order*groupSize <= maxDigitLength)
        return groupSize;
    return maxDigitLength%groupSize;
}
long countingSort(int* arr, bool ascending, int arraySize, int groupSize, int order, int maxDigitLength){
    int mCount = calcModerCount(groupSize,order,maxDigitLength);
    int mode = powTen(mCount);
    int count[mode] = {};
    int divider = powTen(order * groupSize - groupSize);
    int output[arraySize];

    for(int i = 0; i < arraySize; ++i)
        count[ ( arr[i] / divider) % mode ]++;
    
    for(int i = 1; i < mode; ++i)
        count[i] += count[i-1];

    for(int i = arraySize-1; i >= 0; --i){
        int index = (int) (arr[i]/divider) % mode ;
        output[count[index]-1] = arr[i];
        count[index]--;
    }
    
    for(int i = 0; i < arraySize; i++)
        arr[i] = output[i];
    
    return 3*arraySize + mode-1;
}
long multiDigitRadixSort(int* arr, bool ascending, int arraySize, int groupSize, int maxDigitLength){
    long numberOfIterations = 0;
    
    int iterationCount; 
    if (maxDigitLength%groupSize != 0)
        iterationCount = maxDigitLength/groupSize + 1;
    else
        iterationCount = maxDigitLength/groupSize;
    
    int iterator = 1;
    while(iterator <= iterationCount)
         numberOfIterations += countingSort(arr, ascending, arraySize, groupSize, iterator++, maxDigitLength);
    
    int temp;
    if (ascending==false)
        for (int i = 0, j = arraySize - 1; i < arraySize/2; i++, j--)  {     
            temp = arr[i];  
            arr[i] = arr[j];  
            arr[j] = temp;  
        }  
    
    return numberOfIterations;
}
