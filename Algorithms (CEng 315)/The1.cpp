#include "the1.h"
#include <climits>

// HELPER HEADERS //
void insertionSort(int* arr, long &comparison, long & swap, int size) {
    int index = 1;
    int traverser, prev, temp;
    
    while ( index < size ){
        traverser = index++;
        while (traverser > 0){
            prev = arr[traverser-1];
            
            if(++comparison && arr[traverser]<prev){
                temp = arr[traverser];
                arr[traverser] = prev;
                arr[--traverser] = temp;
                swap++;
            } else
                break;
        }
    }
}
void heapify(int* arr, int arraySize, int i,long& comparison, long& swap, int* partitionIndexes){
    int smallest = i;
    int left = 2 * i + 1; // left = 2*i + 1
    int right = 2 * i + 2; // right = 2*i + 2
    
    if (left < arraySize){
        if (arr[left]<arr[i])
            smallest = left;
        if(arr[left]!=INT_MAX)
            comparison++;
    }
    if (right<arraySize){
        if(arr[right]<arr[smallest])
            smallest = right;
        if(arr[right]!=INT_MAX)
            comparison++;
    }

    if (smallest!=i){
        swap++;
        int temp = arr[smallest];
        arr[smallest] = arr[i];
        arr[i] = temp;
        
        int temp_index = partitionIndexes[smallest];
        partitionIndexes[smallest] = partitionIndexes[i];
        partitionIndexes[i] = temp_index;
    
        heapify(arr,arraySize,smallest,comparison,swap,partitionIndexes);
    }
}
void buildMinHeap(int* arr, int arraySize, long& comparison, long& swap, int* partitionIndexes){
    int startIndex = (arraySize / 2) - 1;
 
    for (int i = startIndex; i >= 0; i--) {
        heapify(arr, arraySize, i, comparison, swap, partitionIndexes);
    }
}
int kWayMergeSortWithHeap(int* arr, int K, int size, long& comparison, long& swap){
    int number_of_calls = 1;
    
    if (size<K)
        insertionSort(arr,comparison,swap,size);
    else {
        
        int partitionSizes = size/K;
        for(int i=0;i<K;i++)
            number_of_calls += kWayMergeSortWithHeap(arr+i*partitionSizes,K,partitionSizes,comparison,swap);
            
        int partitionIndexes[K];
        for(int i=0;i<K;i++)
            partitionIndexes[i] = i*partitionSizes;
  
        int heap[K];
        for(int i=0;i<K;i++)
            heap[i] = arr[partitionIndexes[i]];
 
        buildMinHeap(heap, K, comparison, swap, partitionIndexes);

        int result[size];
        int result_index = 0;
        int smallest;
        while(result_index<=size){
            if(heap[0]==INT_MAX) break;
            smallest = heap[0];
            result[result_index++] = smallest;
            
            ++partitionIndexes[0];
            if (partitionIndexes[0]>=size || partitionIndexes[0] % partitionSizes==0){
                heap[0]= INT_MAX;
            } else {
                heap[0] = arr[partitionIndexes[0]];
            }
            heapify(heap,K,0,comparison,swap,partitionIndexes);
        }
 
        for(int i=0;i<size;i++)
            arr[i] = result[i];
    }
        
    return number_of_calls;
}
