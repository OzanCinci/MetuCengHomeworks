#include "the5.h"

/* 
    in the5.h "struct Room" is defined as below:
    
    struct Room {
        int id;
        char content;
        vector<Room*> neighbors;
    };

*/

bool contains(vector<int> v, int el);
bool helper(Room* currentRoom, vector<Room*>& stack, vector<int>& path, vector<int>& visited);

vector<int> maze_trace(vector<Room*> maze) { 

    vector<int> path;
    vector<Room*> stack;
    vector<int> visited;

    helper(maze[0], stack, path, visited);
    return path;
}

bool helper(Room* currentRoom, vector<Room*>& stack, vector<int>& path, vector<int>& visited){
    if (currentRoom->content=='*'){
        path.push_back(currentRoom->id);
        while(!stack.empty()){
            Room* temp = stack.back();
            path.push_back(temp->id);
            stack.pop_back();
        }
        return true;
    } 
    
    stack.push_back(currentRoom);
    path.push_back(currentRoom->id);
    visited.push_back(currentRoom->id);
    
    if (currentRoom->neighbors.size()==0)
        stack.pop_back();
    
    for(int i=0;i<currentRoom->neighbors.size();i++){
        
        if(contains(visited,currentRoom->neighbors[i]->id)==false){
            bool isFound = helper(currentRoom->neighbors[i], stack, path, visited);
            if (isFound == true)
                return true;
            else {
                stack.pop_back();
                path.push_back(currentRoom->id);
            }
        }
    }
    
    return false;
}

bool contains(vector<int> v, int el){
    for(int elem: v)
        if(elem==el)
            return true;
    return false;
}
