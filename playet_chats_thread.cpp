#include<iostream>
#include<thread>
#include<poll.h>
#include<unistd.h>
#include<string.h>
using namespace std;

class player {
  public:
    string player_name;
    player (string name) {
      player_name=name;;
    }
    string get_name() {
      return player_name;
    }
};


int wait_n_read(int fd, char* buf, int szBuf) {
    struct pollfd pfd = {
        .fd      = fd,
        .events  = POLLIN,
        .revents = 0
    };
    poll(&pfd, 1, -1); //wait for an event
    int ret = read(fd, buf, 255);
    if (ret == -1) {
        perror("In read()");
    }
    return ret;
}

void  strtok_find_count(char* msg, int* count, const char* max) {
    char* temp =NULL;
    temp=strtok(msg," ");
    while(temp!=NULL) {
      if(0==strcmp(max,temp))
          *count=1;
      temp=strtok(NULL," ");
    }
}

int player_thread(int fd_read, int fd_write) {
     int count =0;
     char buf[256];
     char write_msg[255];
     char read_msg[255];
     while(1) {
       memset(write_msg, 0, sizeof(write_msg));
       memset(read_msg, 0, sizeof(read_msg));
      if(wait_n_read(fd_read, read_msg,255) == -1) {
           return 0;
      } else {
           cout<<read_msg<<endl;
      }   
      //iistrcat(read_msg,to_string(count).data());
      count++;
      strcpy(write_msg, read_msg); 
      strcat(write_msg, " ");
      strcat(write_msg, to_string(count).data());
      //sprintf(buf,"Message from player B %d", count);
      write(fd_write, write_msg, sizeof(write_msg));
      if(count>=10) {
         cout<<"Break Player B"<<endl; 
         break;
      }
     } 
     return 0; 
}   
    int main(void) {
    string temp_buf;

    cout<<"Enter the player 1's Name" << endl;
    cin>> temp_buf;
    player A(temp_buf);

    cout<<"Enter the player 2's Name" << endl;
    cin>> temp_buf;
    player B(temp_buf);

    int count=0;

    int chat[4];
    enum {
       thread1_read  = 0,
       thread2_write  = 1,
       thread2_read  = 2,
       thread1_write  = 3
    };

    if(pipe(&chat[thread1_read]) == -1 || -1 == pipe(&chat[thread2_read])) {
            perror("In pipe");
    }

    thread playerThread(player_thread,chat[thread2_read],chat[thread2_write]);

    char write_msg[255];
    char read_msg[255];
    while(1) {
       memset(write_msg, 0, sizeof(write_msg));
       memset(read_msg, 0, sizeof(read_msg));
       cout<<"enter the message"<<endl;
       cin>>temp_buf;
       strcpy(write_msg, temp_buf.data());
       write(chat[thread1_write], write_msg, sizeof(write_msg));
       //cout<<"Sending Message "<<temp_buf<<endl; 
       if(wait_n_read(chat[thread1_read], read_msg,sizeof(read_msg)) == -1) {
               return 0;
       } else {
               cout<< read_msg <<endl;
       }
       temp_buf=to_string(10);
       strtok_find_count(read_msg, &count, temp_buf.data());
       if(count) {
              cout<<"Break from Player A"<<endl;
          break;
       }
    }
    playerThread.join();
    return 0;
}

