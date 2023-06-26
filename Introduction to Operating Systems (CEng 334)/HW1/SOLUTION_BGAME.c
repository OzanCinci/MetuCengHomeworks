#include "message.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <poll.h>
#include <stdbool.h>

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)
#define BUFFSIZE 1500

typedef struct bomber {
    int alive;
    int x;
    int y;
    bool marked_to_die;
    int arg_count;
    char* exec_path;
    char** args; // you made it pointer to a char pointer to dynamic size allocation
    int pid;
    int pipe_index;
    bool affected;
    bool win;
    int distance_to_exploded_bomb;
} bomber;

typedef struct bomb {
    bd bomb_info;
    coordinate position;
    bool exploded;
    bool planted;
    int pid;
    int fd[2];
    struct pollfd pollfd;
} bomb;


int main() 
{
    // take initial args
    int width, height, obstacle_count, bomber_count, alive_bomber_count;
    scanf("%d %d %d %d",&width, &height, &obstacle_count, &bomber_count);
    alive_bomber_count = bomber_count;

    // initiate two arrays for bombers and obstacles
    obsd obs_array[obstacle_count];
    struct bomber bomber_array[bomber_count];

    // take obstacles' info
    int x, y, durability, arg_count;
    obsd temp_obs;
    for(int i=0;i<obstacle_count;i++)
    {
        scanf("%d %d %d", &x, &y, &durability);
        temp_obs.position.x = x;
        temp_obs.position.y = y;
        temp_obs.remaining_durability = durability;
        obs_array[i] = temp_obs;
    }

    // take bombers' info
    for(int i=0;i<bomber_count;i++)
    {
        struct bomber temp_bomber;
        
        // allocation of required source and set default info
        temp_bomber.pipe_index = i;
        temp_bomber.alive = 1;
        temp_bomber.exec_path = (char *) malloc(sizeof(char)*200);

        // take bomber's info
        scanf("%d %d %d", &temp_bomber.x, &temp_bomber.y, &temp_bomber.arg_count);
        scanf("%s", temp_bomber.exec_path);
        
        int argC = temp_bomber.arg_count;
        temp_bomber.args = (char **) malloc(sizeof(char*) * (argC +1) );
        temp_bomber.args[0] = temp_bomber.exec_path;
        temp_bomber.marked_to_die = false;
        temp_bomber.affected = false;
        temp_bomber.win = false;

        for(int i=1;i<temp_bomber.arg_count;i++)
        {
            // allocation of args
            temp_bomber.args[i] = (char *) malloc(sizeof(char)*100);
            // take the arg of bomber
            scanf("%s", temp_bomber.args[i]);
        }

        // deploy it into bomber_array
        bomber_array[i] = temp_bomber;
    }

    // create a fd array for pipes
    int fd_array[bomber_count][2];

    // create pipes for every bomber
    for(int i=0;i<bomber_count;i++)
        PIPE(fd_array[i]);
    
    // then create child processes for every bomber!
    int t;
    for(int i=0;i<bomber_count;i++)
    {
        t = fork();

        // child uses fd[1]
        // parent uses fd[0]
        if (t>0)
        {
            bomber_array[i].pid = t;
            close(fd_array[i][1]);
        }
        else if (t == 0) {
            // redirection yapman lazım!
            // dup2(int oldfd, int newfd);
            dup2(fd_array[i][1], 1);
            dup2(fd_array[i][1], 0);
            close(fd_array[i][0]);
           
            execvp(bomber_array[i].exec_path, bomber_array[i].args);
        }
    }

    // set up for polling of bombers
    struct pollfd pollfds[bomber_count];
    for(int i=0;i<bomber_count;i++)
    {
        pollfds[i].fd = fd_array[i][0];
        pollfds[i].events = POLLIN;
    }

    // set up bomber_msg
    im* bomber_msg = (im*) malloc(sizeof(im));
    im* bomb_msg = (im*) malloc(sizeof(im));

    // current bomb count and bomb_array
    // you itialized it with at most 100 bombs, if it
    // is not enough then realloc() it
    int bomb_count = 0;
    bomb* bomb_array = (bomb*) malloc(sizeof(bomb)*100);


    // bomb fd array;
    struct pollfd* bomb_poll_fds;
    int* bomb_poll_int;

    int active_bomb_count = 0;
    int affected_bomber_count = 0;
    // implement while loop
    int status;
    while( alive_bomber_count > 0 )
    {
        if (active_bomb_count>0)
        {
            int bomb_ret = poll(bomb_poll_fds,bomb_count,50);
            if (bomb_ret==0 && false)
                printf("time out on polling bombs \n");
            else if (bomb_ret<0 && false)
                printf("there is an error! returned: %d \n",bomb_ret);
            else
            {
                for(int i=0;i<bomb_count;i++)
                {
                    if (bomb_array[i].exploded==true)
                        continue;
                    
                    int revent = bomb_poll_fds[i].revents;
                    int event = bomb_poll_fds[i].events;

                    if (revent && event)
                    {
                        int read_ret = read_data(bomb_poll_fds[i].fd, bomb_msg);

                        if ( bomb_msg->type == 4 ) // BOMB_EXPLODE
                        {
                            imp print_obj;
                            print_obj.pid = bomb_array[i].pid;
                            print_obj.m = bomb_msg;
                            print_output(&print_obj, NULL, NULL, NULL);
                        
                            coordinate explosion_location = bomb_array[i].position;
                            unsigned int explosion_radius = bomb_array[i].bomb_info.radius;
                        
                            // calculate radius // RADIUS UNSIGNED INTEGER -> may need recalculation !!!!!!!!!!
                            int l = (explosion_location.x <= explosion_radius )? 0: explosion_location.x - explosion_radius; 
                            int r = (explosion_location.x + explosion_radius > width-1)? (width-1) : explosion_location.x + explosion_radius;
                            int u = (explosion_location.y <= explosion_radius )? 0: explosion_location.y - explosion_radius;
                            int d = (explosion_location.y + explosion_radius > height-1) ? (height-1): explosion_location.y + explosion_radius; 

                            // calc explosion affected range
                            for(int k=0;k<obstacle_count;k++)
                            {
                                obsd temp_obs = obs_array[k];
                                int x = temp_obs.position.x;
                                int y = temp_obs.position.y;

                                if (temp_obs.remaining_durability == 0)
                                    continue;

                                if ( y==explosion_location.y )
                                {
                                    if (l<=x && x<=explosion_location.x)
                                    {   
                                        l = x;
                                    }
                                    else if (explosion_location.x <= x && x<=r)
                                    {
                                        r = x;
                                    }
                                }
                                else if ( x==explosion_location.x )
                                {
                                    if (u<=y && y<=explosion_location.y)
                                    {   
                                        u = y;
                                    }
                                    else if (explosion_location.y <= y && y<=d)
                                    {
                                        d = y;
                                    }
                                }
                            }

                            // do side affects on obstacles
                            for(int k=0;k<obstacle_count;k++)
                            {
                                obsd temp_obs = obs_array[k];
                                int x = temp_obs.position.x;
                                int y = temp_obs.position.y;

                                if (temp_obs.remaining_durability==0 || temp_obs.remaining_durability==-1)
                                    continue;

                                bool predicate = 
                                    (y==explosion_location.y && (l==x || r==x)) 
                                    || (x==explosion_location.x && (u==y || d==y));

                                if (predicate)
                                {
                                    obs_array[k].remaining_durability--;
                                    print_output(NULL,NULL,&(obs_array[k]),NULL);
                                }
                            }


                            if (affected_bomber_count<bomber_count-1)
                            {
                                // calculate affected bombers
                                int farthest_distance = 0;
                                int same_distance_count = 1;
                                bool more_than_one_affected;
                                for(int k=0;k<bomber_count;k++)
                                {
                                    bomber current_bomber = bomber_array[k];
                                    if (current_bomber.alive==0)
                                    {
                                        bomber_array[k].distance_to_exploded_bomb = -1;
                                        bomber_array[k].affected = false;
                                        continue;
                                    }

                                    int x = current_bomber.x;
                                    int y = current_bomber.y;
                                    bool predicate1 = (y == explosion_location.y) &&
                                        ((l<=x && x<=explosion_location.x)||(explosion_location.x<=x && x<=r));
                                    bool predicate2 = (x == explosion_location.x) &&
                                        ((u<=y && y<=explosion_location.y)||(explosion_location.y<=y && y<=d));
                                    
                                    // then it is affected
                                    if (predicate1)
                                    {
                                        bomber_array[k].distance_to_exploded_bomb = abs(explosion_location.x - x ); 
                                        bomber_array[k].affected = true;
                                        affected_bomber_count++;
                                    }
                                    else if (predicate2)
                                    {
                                        bomber_array[k].distance_to_exploded_bomb = abs(explosion_location.y - y );
                                        bomber_array[k].affected = true;
                                        affected_bomber_count++;
                                    }
                                    else 
                                    {
                                        bomber_array[k].affected = false;
                                        bomber_array[k].distance_to_exploded_bomb = -1;
                                    }

                                    if (bomber_array[k].distance_to_exploded_bomb>farthest_distance)
                                    {
                                        farthest_distance = bomber_array[k].distance_to_exploded_bomb;
                                        same_distance_count = 1;
                                    }
                                    else if (bomber_array[k].distance_to_exploded_bomb==farthest_distance)
                                    {
                                        same_distance_count++;
                                    }  
                                }

                                // mark them if it is gonna die 
                                more_than_one_affected = (same_distance_count>1) ? true: false;
                                bool skip_the_first_affected = false;
                                for(int k=0;k<bomber_count;k++)
                                {
                                    bomber temp_bomber = bomber_array[k];
                                    if (temp_bomber.alive==0)
                                        continue;

                                    if (temp_bomber.affected==false && affected_bomber_count+1 == bomber_count)
                                    {
                                        bomber_array[k].marked_to_die = false;
                                        bomber_array[k].win = true;
                                        continue;
                                    }

                                    if (temp_bomber.affected==true && farthest_distance>temp_bomber.distance_to_exploded_bomb)
                                    {
                                        bomber_array[k].marked_to_die = true;
                                        bomber_array[k].alive = 0;
                                    }
                                    else if (temp_bomber.affected==true && farthest_distance==temp_bomber.distance_to_exploded_bomb)
                                    {
                                        if (more_than_one_affected && affected_bomber_count == bomber_count && skip_the_first_affected==false)
                                        {
                                            skip_the_first_affected = true;
                                            bomber_array[k].affected = false;
                                            bomber_array[k].marked_to_die = false;
                                            bomber_array[k].win = true;
                                        }
                                        else
                                        {
                                            bomber_array[k].marked_to_die = true;
                                            bomber_array[k].alive = 0;
                                        }
                                    }
                                }
                            }

                            // kill the bomb process
                            bomb_array[i].exploded = true;
                            waitpid(bomb_array[i].pid, &status, 0);
                            close(bomb_poll_fds[i].fd);
                            active_bomb_count--;
                        }
                    }
                }
            }
        }


        // polling for bomber processes
        int ret = poll(pollfds,bomber_count,50);
        if (ret==0 && false)
            printf("time out on polling! \n");
        else if (ret<0 && false)
            printf("there is an error! returned: %d \n",ret);
        else
        {
            // detect which one is success
            for(int i=0;i<bomber_count;i++) 
            {
                if (bomber_array[i].alive == 0 && bomber_array[i].marked_to_die!=true)
                    continue;

                int revent = pollfds[i].revents;
                int event = pollfds[i].events;

                // read the msg from bomber
                if (event && revent)
                {
                    int read_ret = read_data(pollfds[i].fd, bomber_msg);

                    imp print_obj;
                    print_obj.pid = bomber_array[i].pid;
                    print_obj.m = bomber_msg;
                    print_output(&print_obj, NULL, NULL, NULL);
                    
                    // If a bomber is affected by the
                    // explosion, the server marks the bomber. When they send their next request, BOMBER_DIE should
                    // be sent without any other data to indicate that the bomber has died. If that bomber is second to
                    // last bomber, the last remaining bomber should be marked as well. When the last bomber makes
                    // a request, BOMBER_WIN should be sent to that bomber to let them know, they had won the game
                    if (bomber_array[i].marked_to_die==true)
                    {
                        om response;
                        response.type = 3; // BOMBER_DIE;
                        bomber_array[i].alive = 0;
                        bomber_array[i].affected = false;
                        bomber_array[i].marked_to_die = false;
                        
                        omp print_obj;
                        print_obj.pid = bomber_array[i].pid;
                        print_obj.m = &response;
                        print_output(NULL,&print_obj,NULL,NULL);
                        send_message(pollfds[i].fd, &response);
                        waitpid(bomber_array[i].pid, &status, 0);
                        close(pollfds[i].fd);
                        alive_bomber_count--;
                        continue;
                    }

                    if (bomber_array[i].win==true)
                    {
                        om response;
                        response.type = 4; // bomber win
                        omp print_obj;
                        print_obj.pid = bomber_array[i].pid;
                        print_obj.m = &response;

                        send_message(pollfds[i].fd, &response);
                        print_output(NULL,&print_obj,NULL,NULL);
                        waitpid(bomber_array[i].pid, &status, 0);
                        close(pollfds[i].fd);
                        bomber_array[i].alive = 0;
                        alive_bomber_count--;
                        continue;
                    }

                    // if msg type is BOMBER_START
                    if (bomber_msg->type == 0)
                    {
                        coordinate start_position;
                        start_position.x = bomber_array[i].x;
                        start_position.y = bomber_array[i].y;

                        omd payload;
                        payload.new_position = start_position;

                        om response;
                        response.type = 0; // 0 equals BOMBER_LOCATION
                        response.data = payload; 

                        int send_return = send_message(pollfds[i].fd, &response);

                        omp printable;
                        printable.pid = bomber_array[i].pid;
                        printable.m = &response;
                        print_output(NULL,&printable,NULL,NULL);

                    } // EĞER ÇEVRESİNDEKİ BOMBERLER ÖLÜ VEYA OBSTACLELAR KIRILMIŞ İSE İFİNİ ATMAYI UNUTMAA!!!
                    else if (bomber_msg->type == 1) // if msg type is BOMBER_SEE
                    {
                        int bomber_x = bomber_array[i].x;
                        int bomber_y = bomber_array[i].y;
                        int l,r,u,d;
                        // calculate initial borders
                        l = (bomber_x>=3) ? (bomber_x-3) : 0;
                        r = (bomber_x+3>width-1) ?  (width-1) : (bomber_x+3);
                        u = (bomber_y>=3) ? (bomber_y-3) : 0;
                        d = (bomber_y+3>height-1) ?  (height-1) : (bomber_y+3);
                        // to count total object count
                        int vision_change_count = 0;
                        bool l_changed = false; 
                        bool r_changed = false;
                        bool u_changed = false;
                        bool d_changed = false;
                        od l_od, r_od, u_od, d_od;

                        // calculate obstacles applied borders
                        for(int k=0;k<obstacle_count;k++)
                        {
                            // eğer obstacle'lar broken ise ignorela
                            if (obs_array[k].remaining_durability==0)
                                continue;

                            int x = obs_array[k].position.x;
                            int y = obs_array[k].position.y;

                            if ( x == bomber_x)
                            {
                                // calc y
                                if (u<=y && y<bomber_y)
                                {
                                    if (u_changed==false)
                                    {
                                        u_changed = true;
                                        vision_change_count++;
                                    }
                                    u = y;
                                }
                                else if (bomber_y<y && y<=d)
                                {
                                    if (d_changed==false)
                                    {
                                        d_changed = true;
                                        vision_change_count++;
                                    }
                                    d = y;
                                }
                            } else if ( y == bomber_y)
                            {
                                // calc x
                                if(l<=x && x<bomber_x)
                                {
                                    if (l_changed==false)
                                    {
                                        l_changed = true;
                                        vision_change_count++;
                                    }
                                    l = x;
                                }
                                else if(bomber_x<x && x<=r)
                                {
                                    if (r_changed==false)
                                    {
                                        r_changed = true;
                                        vision_change_count++;
                                    }
                                    r = x;
                                }
                            }
                        }
                        // iterate for other bombers
                        int indexes_of_shown[bomber_count];
                        int shown_bomber_count = 0;
                        for(int k=0;k<bomber_count;k++)
                        {
                            // eğer kendisi ise veya bomber alive değil ise ignore'la!
                            if(k==i || bomber_array[k].alive==0)
                            {
                                indexes_of_shown[k]=0;
                                continue;
                            } 
                            bomber temp_bomber = bomber_array[k];

                            if (temp_bomber.y == bomber_y )
                            {
                                // calc x
                                if (l<=temp_bomber.x && temp_bomber.x<bomber_x )
                                {
                                    indexes_of_shown[k]=1;
                                    shown_bomber_count++;
                                }
                                else if (bomber_x<temp_bomber.x && temp_bomber.x<=r)
                                {
                                    indexes_of_shown[k]=1;
                                    shown_bomber_count++;
                                }
                                else
                                    indexes_of_shown[k]=0;
                            }
                            else if (temp_bomber.x == bomber_x)
                            {
                                // calc y
                                if(u<=temp_bomber.y && temp_bomber.y<bomber_y)
                                {
                                    indexes_of_shown[k]=1;
                                    shown_bomber_count++;
                                }
                                else if (bomber_y<temp_bomber.y && temp_bomber.y<=d)
                                {
                                    indexes_of_shown[k]=1;
                                    shown_bomber_count++;
                                }
                                else
                                    indexes_of_shown[k]=0;
                            }
                            else
                                indexes_of_shown[k]=0;
                        }

                        // iterate for bombs
                        int indexes_of_shown_bombs[bomb_count];
                        int shown_bomb_count = 0;
                        for(int k=0;k<bomb_count;k++)
                        {
                            // eğer bomb patlamış ise ignorela!
                            if (bomb_array[k].exploded==true)
                                continue;

                            bomb temp_bomb = bomb_array[k];
                            int bomb_x = temp_bomb.position.x;
                            int bomb_y = temp_bomb.position.y;
                            if (bomb_y == bomber_y )
                            {
                                // calc x
                                if (l<=bomb_x && bomb_x<=bomber_x )
                                {
                                    indexes_of_shown_bombs[k]=1;
                                    shown_bomb_count++;
                                }
                                else if (bomber_x<=bomb_x && bomb_x<=r)
                                {
                                    indexes_of_shown_bombs[k]=1;
                                    shown_bomb_count++;
                                }
                                else
                                    indexes_of_shown_bombs[k]=0;
                            }
                            else if (bomb_x == bomber_x)
                            {
                                // calc y
                                if(u<=bomb_y && bomb_y<=bomber_y)
                                {
                                    indexes_of_shown_bombs[k]=1;
                                    shown_bomb_count++;
                                }
                                else if (bomber_y<=bomb_y && bomb_y<=d)
                                {
                                    indexes_of_shown_bombs[k]=1;
                                    shown_bomb_count++;
                                }
                                else
                                    indexes_of_shown_bombs[k]=0;
                            }
                            else
                                indexes_of_shown_bombs[k]=0;
                        }

                        // calc total shown object count
                        int total_shown_obj = vision_change_count + shown_bomber_count + shown_bomb_count;
                        od* od_arr = (od*)malloc(sizeof(od)*total_shown_obj);
                        int index = 0;
                        // append bombers
                        for(int k=0;k<bomber_count;k++)
                        {
                            if (indexes_of_shown[k]==1)
                            {
                                od temp_od;
                                temp_od.type = 0;
                                temp_od.position.x = bomber_array[k].x;
                                temp_od.position.y = bomber_array[k].y;
                                od_arr[index++] = temp_od;
                            }
                        }
                        // append bombs
                        for(int k=0;k<bomb_count;k++)
                        {
                            if(indexes_of_shown_bombs[k])
                            {
                                od temp_od;
                                temp_od.type = 1;
                                temp_od.position = bomb_array[k].position;
                                od_arr[index++] = temp_od;
                            }
                        }
                        // append obstacles
                        od temp_obs_od;
                        temp_obs_od.type=2;
                        if(l_changed){
                            temp_obs_od.position.x = l;
                            temp_obs_od.position.y = bomber_y;    
                            od_arr[index++] = temp_obs_od;
                        } 
                        if(r_changed){
                            temp_obs_od.position.x = r;
                            temp_obs_od.position.y = bomber_y;    
                            od_arr[index++] = temp_obs_od;
                        }
                        if(d_changed){
                            temp_obs_od.position.x = bomber_x;
                            temp_obs_od.position.y = d;    
                            od_arr[index++] = temp_obs_od;
                        }
                        if(u_changed){
                            temp_obs_od.position.x = bomber_x;
                            temp_obs_od.position.y = u;    
                            od_arr[index++] = temp_obs_od;
                        }

                        // send msg without data
                        om response;
                        response.type = 1; // 1 = BOMBER_VISION
                        omd data;
                        data.object_count = total_shown_obj;
                        response.data = data;
                        int send_msg_ret = send_message(pollfds[i].fd, &response);

                        // send msg with data
                        int send_obj_ret = send_object_data(pollfds[i].fd, total_shown_obj, od_arr);

                        // print_output
                        omp printable;
                        printable.pid = bomber_array[i].pid;
                        printable.m = &response;

                        // obstacles kısmı ?
                        print_output(NULL,&printable,NULL,od_arr);

                        // freelemeyi unutma!
                        free(od_arr);

                    }
                    else if (bomber_msg->type == 2) // if msg type is BOMBER_MOVE
                    {
                        bomber current_bomber = bomber_array[i];
                        bool move_allowed = true;
                        // CONDITION1: 1 STEP AWAY
                        // CONDITION2: NO VERTICAL
                        // CONDITION3: NO OUT OF BOUNDS
                        // CONDITION4: NO OBSTACLES OR BOMBERS IN THE WAY
                        imd data = bomber_msg->data; // where s/he wants to go as location
                        coordinate target_position = data.target_position;
                        int target_x = target_position.x;
                        int target_y = target_position.y;


                        // unsigned olabilir kontrol et!
                        // CHECK 1
                        if ( abs(current_bomber.x-target_x)>1 || abs(current_bomber.y-target_y)>1)
                            move_allowed = false;
                        
                        // CHECK 2
                        if (move_allowed && (abs(current_bomber.x-target_x) + abs(current_bomber.y-target_y) > 1) )
                            move_allowed = false;
                        
                        // CHECK 3
                        if (target_x > (width-1) || target_y > (height-1))
                            move_allowed = false;

                        // CHECK 4 (check all the alive bombers and obstacles)
                        for(int k=0;k<bomber_count;k++)
                        {
                            if (!move_allowed) break;
                            if (i==k || bomber_array[k].alive!=1) continue;

                            bomber temp_bomber = bomber_array[k];
                            if (temp_bomber.x == target_x && temp_bomber.y == target_y)
                            {
                                move_allowed = false;
                                break;
                            }
                        }
                        // CHECK 4
                        for(int k=0;k<obstacle_count;k++)
                        {
                            if (!move_allowed) break;
                            obsd temp_obs = obs_array[k];
                            if (temp_obs.remaining_durability==0) continue;

                            if ( temp_obs.position.x==target_x && temp_obs.position.y==target_y )
                            {
                                move_allowed = false;
                                break;
                            }
                        }

                        // send response in case of failure to move and success in move
                        coordinate new_position;
                        new_position.x = move_allowed ? target_x : current_bomber.x;
                        new_position.y = move_allowed ? target_y : current_bomber.y;

                        // update controller's array!!
                        bomber_array[i].x = new_position.x;
                        bomber_array[i].y = new_position.y;

                        omd payload;
                        payload.new_position = new_position;
                        
                        om response;
                        response.type = 0; // BOMBER_MOVE
                        response.data = payload;

                        omp printable;
                        printable.pid = bomber_array[i].pid;
                        printable.m = &response;
                        print_output(NULL,&printable,NULL,NULL);
                        send_message(pollfds[i].fd, &response);

                    }
                    else if (bomber_msg->type == 3) // if msg type is BOMBER_PLANT
                    {
                        // CHECK IF THERE IS ANOTHER BOMB IN THE SAME LOCATION
                        // IF SO, DO NOT PLANT THIS BOMB
                        bool is_plantable = (bomber_array[i].alive==1) ? true : false;

                        for(int k=0;k<bomb_count;k++)
                        {
                            bomb temp_bomb = bomb_array[k];
                            if (temp_bomb.exploded==true)
                                continue;

                            coordinate temp_bomb_coordinate = temp_bomb.position;
                            if (temp_bomb_coordinate.x==bomber_array[i].x && temp_bomb_coordinate.y==bomber_array[i].x)
                            {
                                is_plantable = false;
                                break;
                            }
                        }

                        if (is_plantable)
                        {
                            active_bomb_count++;

                            bomb temp_bomb;
                            temp_bomb.bomb_info = (bomber_msg->data).bomb_info;
                            
                            PIPE(temp_bomb.fd);
                            int temp_pid = fork();
                            if (temp_pid>0)
                            {
                                // do allocations in parent process
                                if (bomb_count==0)
                                {
                                    // malloc
                                    bomb_count++;
                                    bomb_array = (bomb*) malloc(sizeof(bomb));
                                    bomb_poll_fds = (struct pollfd*) malloc(sizeof(struct pollfd));
                                    bomb_poll_int = (int*) malloc(sizeof(int));
                                }
                                else 
                                {
                                    // realloc          
                                    bomb_count++;                  
                                    bomb_array = (bomb*) realloc(bomb_array, sizeof(bomb)*bomb_count);
                                    bomb_poll_fds = (struct pollfd*) realloc(bomb_poll_fds,sizeof(struct pollfd)*bomb_count);
                                    bomb_poll_int = (int *) realloc(bomb_poll_int, sizeof(int)*bomb_count);
                                }

                                temp_bomb.pid = temp_pid;
                                temp_bomb.position.x = bomber_array[i].x;
                                temp_bomb.position.y = bomber_array[i].y;
                                temp_bomb.exploded = false;
                                temp_bomb.planted = 1;

                                struct pollfd temp_pollfd;
                                temp_pollfd.fd = temp_bomb.fd[0];
                                temp_pollfd.events = POLLIN;
                                temp_bomb.pollfd = temp_pollfd;


                                bomb_array[bomb_count-1] = temp_bomb;
                                bomb_poll_fds[bomb_count-1] = temp_bomb.pollfd;
                                bomb_poll_int[bomb_count-1] = temp_bomb.pollfd.fd;
                                close(temp_bomb.fd[1]);
                            }
                            else if (temp_pid==0)
                            {
                                dup2(temp_bomb.fd[1],1);
                                dup2(temp_bomb.fd[1],0);
                                close(temp_bomb.fd[0]);

                                char stringInterval[20];
                                sprintf(stringInterval, "%ld", temp_bomb.bomb_info.interval);

                                char* args[3] = {"./bomb",stringInterval, NULL};
                                // execvp atacan!
                                execvp("./bomb", args);
                            }
                        }

                        // The response is
                        // BOMBER_PLANT_RESULT message, the data should a boolean to indicate 
                        // the success or failure of the plant.      

                        // send response
                        omd data;
                        data.planted = is_plantable ? 1 : 0;

                        om response;
                        response.type = 2; // BOMBER_PLANT_RESULT
                        response.data = data;

                        omp printable;
                        printable.pid = bomber_array[i].pid;
                        printable.m = &response;
                        print_output(NULL,&printable,NULL,NULL);

                        int send_msg_ret = send_message(pollfds[i].fd, &response);

                    }
                    else if (bomber_msg->type == 4) // if msg type is BOMB_EXPLODE
                    {
                        // not possible!
                    }
                }
            }
        }

        if (alive_bomber_count > 0)
            usleep(1000);
    }

    while(active_bomb_count>0)
    {
        int bomb_ret = poll(bomb_poll_fds,bomb_count,50);
        if (bomb_ret==0 && false)
            printf("time out on polling bombs \n");
        else if (bomb_ret<0 && false)
            printf("there is an error! returned: %d \n",bomb_ret);
        else
        {
            for(int i=0;i<bomb_count;i++)
                {
                    if (bomb_array[i].exploded==true)
                        continue;
                    
                    int revent = bomb_poll_fds[i].revents;
                    int event = bomb_poll_fds[i].events;

                    if (revent && event)
                    {
                        int read_ret = read_data(bomb_poll_fds[i].fd, bomb_msg);

                        if ( bomb_msg->type == 4 ) // BOMB_EXPLODE
                        {
                            imp print_obj;
                            print_obj.pid = bomb_array[i].pid;
                            print_obj.m = bomb_msg;
                            print_output(&print_obj, NULL, NULL, NULL);
                        
                            
                            // kill the bomb process
                            bomb_array[i].exploded = true;
                            waitpid(bomb_array[i].pid, &status, 0);
                            close(bomb_poll_fds[i].fd);
                            active_bomb_count--;
                        }
                    }
                }
        }
    }

    return 0;
}
