#include "the7.h"
using namespace std;
// Implement your solution after this line

struct dijkstraResult {
    int cost;
    vector<int> path;
};

dijkstraResult calcDijkstra(vector<vector<int>>& matrix, int begin, int end, int numberOfBuildings);

void CanCatch(int numberOfBuildings, std::vector<Road> roads, int begin, int end, int printerLocation, int dorm, int timeLeft, int printPath) {
        
    vector<vector<int>> matrix(numberOfBuildings, vector<int> (numberOfBuildings, INT_MAX));

    for(auto elem: roads){
        int location1 = elem.buildings.first;
        int location2 = elem.buildings.second;
        int cost = elem.time;
        
        matrix[location1][location2] = cost;
        matrix[location2][location1] = cost;
    }
    
    // calc begin -> end
    dijkstraResult beginToEnd = calcDijkstra(matrix, begin, end, numberOfBuildings);
    if (beginToEnd.cost > timeLeft){
        cout<<"IMPOSSIBLE"<<endl;
        return;
    }
    // calc begin -> printer
    dijkstraResult beginToPrinter = calcDijkstra(matrix, begin, printerLocation, numberOfBuildings);
    // calc begin -> dorm
    dijkstraResult beginToDorm = calcDijkstra(matrix, begin, dorm, numberOfBuildings);
    // calc dorm -> end
    dijkstraResult dormToEnd = calcDijkstra(matrix, dorm, end, numberOfBuildings);
    // calc printer -> end
    dijkstraResult printerToEnd = calcDijkstra(matrix, printerLocation, end, numberOfBuildings);
    // calc dorm -> printer
    dijkstraResult dormToPrinter = calcDijkstra(matrix, dorm, printerLocation, numberOfBuildings);
    
    // calc (b d p e) and (b p d e)
    int bdpe = beginToDorm.cost + dormToPrinter.cost + printerToEnd.cost;
    int bpde = beginToPrinter.cost + dormToPrinter.cost + dormToEnd.cost;
    // (b d e) and (b p e)
    int bde = beginToDorm.cost + dormToEnd.cost;
    int bpe = beginToPrinter.cost + printerToEnd.cost;
    // to be printed
    vector<int> resultPath;
    
    if (bpde <= timeLeft && bpde <= bdpe ){
        
         // do for bpde
        cout<< "YES BOTH "<<bpde<<"MINUTES"<<endl;
        // path işlemi
        for(auto elem: beginToPrinter.path)
            resultPath.push_back(elem);
        resultPath.pop_back();
        for(int i=dormToPrinter.path.size()-1;i>=0;i--)
            resultPath.push_back(dormToPrinter.path[i]);
        resultPath.pop_back();
        for(auto elem: dormToEnd.path)
            resultPath.push_back(elem);
            
    } else if (bdpe <= timeLeft){
        
        // do for bdpe
        cout<< "YES BOTH "<<bdpe<<"MINUTES"<<endl;
        // path işlemi
        for(auto elem: beginToDorm.path)
            resultPath.push_back(elem);
        resultPath.pop_back();
        for(auto elem: dormToPrinter.path)
            resultPath.push_back(elem);
        resultPath.pop_back();
        for(auto elem: printerToEnd.path)
            resultPath.push_back(elem);
            
    }
    else if (bpe <= timeLeft && bpe <= bde){
        
        // do for bpe
        cout<< "YES PRINTER "<<bpe<<"MINUTES"<<endl;
        // path işlemi
        for(auto elem: beginToPrinter.path)
            resultPath.push_back(elem);
        resultPath.pop_back();
        for(auto elem: printerToEnd.path)
            resultPath.push_back(elem);
            
    } else if (bde<=timeLeft){
        
        // do for bde
        cout<< "YES DORM "<<bde<<"MINUTES"<<endl;
        // path işlemi
        for(auto elem: beginToDorm.path)
            resultPath.push_back(elem);
        resultPath.pop_back();
        for(auto elem: dormToEnd.path)
            resultPath.push_back(elem);
            
    }  else {
        
        // do for be 
        cout<< "YES DIRECTLY "<<beginToEnd.cost<<"MINUTES"<<endl;
        // path işlemi
        for(auto elem: beginToEnd.path)
            resultPath.push_back(elem);
        
    }
    
    if (printPath==1){
        PrintRange(resultPath.begin(),resultPath.end());
    }
    
    // return void
    return;
}


dijkstraResult calcDijkstra(vector<vector<int>>& matrix, int begin, int end, int numberOfBuildings){
    dijkstraResult result;
    vector<bool> visited(numberOfBuildings, false);  // visited list
    vector<int> costs(numberOfBuildings,INT_MAX);    // cost list
    costs[begin] = 0;
    // comparison function for priority que (min heap)
    auto cmp = [&costs](int index1, int index2){return (costs.at(index1))>(costs.at(index2));};
    priority_queue<int, vector<int>, decltype(cmp)> pq(cmp);
    vector<int> prev(numberOfBuildings,-1);          // prev list
    
    // implement Dijkstra
    pq.push(begin);
    visited[begin]=true;
    while(pq.empty()==false){
        int topElement = pq.top();
        pq.pop();
        
        for(int i=0;i<numberOfBuildings;i++){
            if (i!=topElement && matrix[topElement][i]!=INT_MAX){
                int tempCost = costs[topElement] + matrix[topElement][i];
                // if new cost is smaller than prev cost
                if (tempCost < costs[i]){
                    costs[i] = tempCost;
                    prev[i] = topElement;
                }
                if (visited[i]==false){
                    pq.push(i);
                    visited[i] = true;
                }
            }
        }
        
        // refactor pq
        vector<int> tmp;
        while(pq.empty()==false){
            tmp.push_back(pq.top());
            pq.pop();
        }
        while(tmp.empty()==false){
            pq.push(tmp.back());
            tmp.pop_back();
        }
    }
    // trace back to find the path
    int tmp = end;
    int index = end;
    result.path.insert(result.path.begin(), end);
    while(index!=-1){
        tmp = prev[index];
        if (tmp==-1)
            break;
        result.path.insert(result.path.begin(), tmp);
        index = tmp;
    }
    result.cost = costs[end];
    // return the result
    return result;
}
