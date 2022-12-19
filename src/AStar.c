#include "entity.h"
#include "math.h"

//the obstacles position in world space
//[0][0]-[0][1] are the 2d x coordinates and [1][0]-[1][1] are the y coordinates of the box
//201 total space, 200 for limit of defenses, and 1 extra for the center box

//world coordinates
//x: -40 y:40
//x:40 y:40
//x:40 y:-40
//x:-40 y-40

int obstacleWS[201][2][2] = {0};
int k = 0;
int world[80][80] = {0};

int pMoves[4];
void obstacleWorldSpace(Box obstacle){
    int x1 = obstacle.x;
    int y1 = obstacle.y;
    int x2 = obstacle.w + x1;
    int y2 = obstacle.d + y1;

    obstacleWS[k][0][0] = (int) x1;
    obstacleWS[k][0][1] = (int) x2;
    obstacleWS[k][1][0] = (int) y1;
    obstacleWS[k][1][1] = (int) y2;
    k++;
}
//takes 3d vector but gets distance on the 2d plane
float distance2D(Vector3D start, Vector3D end){
    return sqrtf(powf(end.x, start.x) + powf(end.y, start.y));
}

void buildWorld(){
    for(int i=0; i<201; i++){
        int xLength = abs(obstacleWS[i][0][0] - obstacleWS[i][0][1]);
        int yLength = abs(obstacleWS[i][1][0] - obstacleWS[i][1][1]);
        for(int j=0; j<xLength; j++){
            world[obstacleWS[i][0][0]+j][obstacleWS[i][1][0]] = 1;
        }
        for(int j=0; j<yLength; j++){
            world[obstacleWS[i][0][0]][obstacleWS[i][1][0]+j] = 1;
        }
    }
}
int distanceWS(Vector2D vector);

void possibleMoves(Vector2D currentPos, int world[100][100]){
    pMoves[0] = world[(int)currentPos.x + 1][(int)currentPos.y]; //possible move in +x direction
    pMoves[1] = world[(int)currentPos.x - 1][(int)currentPos.y]; //possible move in -x direction
    pMoves[2] = world[(int)currentPos.x][(int)currentPos.y + 1]; //possible move in +y direction
    pMoves[3] = world[(int)currentPos.x][(int)currentPos.y - 1]; //possible move in -y direction

    for(int i=0; i<201; i++){
        //not a set obstacle
        if(obstacleWS[i][0][0] == 0 && obstacleWS[i][0][1] == 0){
            continue;
        }
        else{//it is a set obstacle
            for(int j=0; j<4; j++){
                if(pMoves[j] == 1)
                    return;
            }
        }
    }
}

void ASearch(int world[100][100], Entity *searchingEnt, Entity *player){
    //the AI will see the world as a grid of x and y positions
    //right now the world is 100x100 as a template until I measure it
    //the grid will be each flat value of x and y, no decimal points
    
    //convert each box object into 
    for(int i=0; i<200; i++){
        //check if its an empty, if each x,y,z position is 0 then the box isn't a real defense
        if(!(searchingEnt->defenseBounds[i].x == 0 && searchingEnt->defenseBounds[i].y == 0 && searchingEnt->defenseBounds[i].z == 0)){
            obstacleWorldSpace(searchingEnt->defenseBounds[i]);
        }
    }

    //also add the center box to 
    Box centerBox = gfc_box(0,0,-25, 10,10,10);
    obstacleWorldSpace(centerBox);

    //the ultimate goal of the AI to reach
    Vector3D playerPos = player->position;
    //AI's position
    Vector3D entPos = searchingEnt->position;

    //used for the heuristic function of A*
    float hf = distance2D(entPos, playerPos);


}