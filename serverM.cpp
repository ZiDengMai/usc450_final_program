//
//  main.cpp
//  server
//
//  Created by 麦卡比 on 2023/11/14.
//
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
struct account{
    string id;
    string pass;
};
struct account arr[1000];
int an=0;
char login_success[]="1";
char no_user_name[]="0";
char no_password[]="-1";
char admin[]="2";
char can_find[]="1";
char cannot_find[]="2";
char not_available[]="0";
bool admin_flag=false;
//for loading file
void initial(){
    ifstream infile;
    infile.open("./member.txt");
    infile.seekg(0, infile.end);
    int len=infile.tellg();
    infile.seekg(0, infile.beg);
    char buffer[1024];
    memset(buffer,0,sizeof(buffer));
    infile.read(buffer, len);
    int ai=0;
    len=strlen(buffer);
    for(int ci=0;ci<len;ci++){
        string ids="";
        string pass="";
        while(ci<len && buffer[ci]!=','){
            ids+=buffer[ci];
            ci++;
        }
        ci+=2;
        while(ci<len && (buffer[ci]!='\n' && buffer[ci]!='\r' && buffer[ci]!='\0')){
            pass+=buffer[ci];
            ci++;
        }
        if(buffer[ci]=='\r'){
            ci++;
        }
        struct account ac={ids,pass};
        arr[ai++]=ac;
    }
    infile.close();
    an=ai;
}

//for checking name and password
int check_login(char* info){
    //split the login info
    int i=0;
    string name;
    string password;
    while(info[i]!='\n'){
        name+=(char)info[i++];
    }
    i++;
    while(info[i]!='\0'){
        password+=(char)info[i++];
    }
    for(int j=0;j<an;j++){
        if(name.compare(arr[j].id)==0 && password.compare(arr[j].pass)!=0){
            printf("Password %s does not match the username. Send a reply to the client.\n",password.c_str());
            return -1;
        }
        if(name.compare(arr[j].id)==0 && password.compare(arr[j].pass)==0){
            printf("Password %s matches the username. Send a reply to the client.\n",password.c_str());
            if(name.compare("Firns")==0){
                return 2;
            }
            return 1;
        }
    }
    printf("%s is not registered. Send a reply to the client.\n",name.c_str());
    return 0;
}
int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Main Server is up and running\n");
    initial();
    printf("Main Server loaded the member list\n");
    
    //for main_server tcp binding
    int server_socket_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in server_addr_tcp;
    memset(&server_addr_tcp,0,sizeof(server_addr_tcp));
    server_addr_tcp.sin_family=AF_INET;
    server_addr_tcp.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr_tcp.sin_port=htons(45495);
    bind(server_socket_tcp,(struct sockaddr*)&server_addr_tcp,sizeof(server_addr_tcp));
    socklen_t client_addr_size=sizeof(server_addr_tcp);
    listen(server_socket_tcp, 20);
    int client_socket=accept(server_socket_tcp, (struct sockaddr*) &server_addr_tcp, &client_addr_size);
    
    //for udp client binding
    int server_socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr_udp;
    memset(&server_addr_udp,0,sizeof(server_addr_udp));
    server_addr_udp.sin_family=AF_INET;
    server_addr_udp.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr_udp.sin_port=htons(44495);
    bind(server_socket_udp,(struct sockaddr*)&server_addr_udp,sizeof(server_addr_udp));
    
    
    //udp server address of S,L,H
    struct sockaddr_in server_addr_udp_s;
    memset(&server_addr_udp_s,0,sizeof(server_addr_udp_s));
    server_addr_udp_s.sin_family=AF_INET;
    server_addr_udp_s.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr_udp_s.sin_port=htons(41495);
    int len_udp_s=sizeof(server_addr_udp_s);
    
    struct sockaddr_in server_addr_udp_l;
    memset(&server_addr_udp_l,0,sizeof(server_addr_udp_l));
    server_addr_udp_l.sin_family=AF_INET;
    server_addr_udp_l.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr_udp_l.sin_port=htons(42495);
    int len_udp_l=sizeof(server_addr_udp_l);
    
    struct sockaddr_in server_addr_udp_h;
    memset(&server_addr_udp_h,0,sizeof(server_addr_udp_h));
    server_addr_udp_h.sin_family=AF_INET;
    server_addr_udp_h.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr_udp_h.sin_port=htons(43495);
    int len_udp_h=sizeof(server_addr_udp_h);
    
    char read_buffer[1024];
    char send_buffer[1024];
    while(true){
        //read message from client
        //read(client_socket, read_buffer, sizeof(read_buffer));
        memset(read_buffer,0,sizeof(read_buffer));
        memset(send_buffer,0,sizeof(send_buffer));
        if(read(client_socket, read_buffer, sizeof(read_buffer))==0){
            //printf("!!!!!!\n");
            client_socket=accept(server_socket_tcp, (struct sockaddr*) &server_addr_tcp, &client_addr_size);
        }else{
            printf("Main Server received the username and password from the client using TCP over port 45495\n");
            int ans=check_login(read_buffer);
            if(ans==1){
                strcpy(send_buffer, login_success);
                sendto(server_socket_udp, login_success, strlen(login_success), 0, (struct sockaddr *)&server_addr_udp_s, len_udp_s);
                sendto(server_socket_udp, login_success, strlen(login_success), 0, (struct sockaddr *)&server_addr_udp_l, len_udp_l);
                sendto(server_socket_udp, login_success, strlen(login_success), 0, (struct sockaddr *)&server_addr_udp_h, len_udp_h);
                write(client_socket, send_buffer, strlen(send_buffer));
                break;
            }else if(ans==0){
                strcpy(send_buffer, no_user_name);
                write(client_socket, send_buffer, strlen(send_buffer));
            }else if(ans==-1){
                strcpy(send_buffer, no_password);
                write(client_socket, send_buffer, strlen(send_buffer));
            }else if(ans==2){
                strcpy(send_buffer, admin);
                sendto(server_socket_udp, admin, strlen(admin), 0, (struct sockaddr *)&server_addr_udp_s, len_udp_s);
                sendto(server_socket_udp, admin, strlen(admin), 0, (struct sockaddr *)&server_addr_udp_l, len_udp_l);
                sendto(server_socket_udp, admin, strlen(admin), 0, (struct sockaddr *)&server_addr_udp_h, len_udp_h);
                write(client_socket, send_buffer, strlen(send_buffer));
                admin_flag=true;
                break;
            }
        }
    }
    while(true){
        memset(read_buffer,0,sizeof(read_buffer));
        memset(send_buffer,0,sizeof(send_buffer));
        //read the bookcode
        read(client_socket, read_buffer, sizeof(read_buffer));
        printf("Main Server received the book request from client using TCP over port 45495.\n");
        //send to s_server
        if(strlen(read_buffer)>0){
            if(read_buffer[0]=='S'){
                printf("Found %s located at Server S. Send to Server S.\n",read_buffer);
                sendto(server_socket_udp, read_buffer, strlen(read_buffer), 0, (struct sockaddr *)&server_addr_udp_s, len_udp_s);
                recvfrom(server_socket_udp, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *)&server_addr_udp_s, (socklen_t *)&len_udp_s);
                if(send_buffer[0]=='1'){
                    string number="";
                    for(int i=1;i<strlen(send_buffer);i++){
                        number+=send_buffer[i];
                    }
                    printf("Main Server received from server S the book status result using UDP over port 44495: Number of books %s available is: %s.\n",read_buffer,number.c_str());
                }else if(strcmp(send_buffer,cannot_find)==0){
                    printf("Did not find %s in the book code list.\n",read_buffer);
                }else if(strcmp(send_buffer,not_available)==0){
                    printf("Main Server received from server S the book status result using UDP over port 44495: Number of books %s available is: 0.\n",read_buffer);
                }
                write(client_socket, send_buffer, strlen(send_buffer));
            }else if(read_buffer[0]=='L'){
                printf("Found %s located at Server L. Send to Server L.\n",read_buffer);
                sendto(server_socket_udp, read_buffer, strlen(read_buffer), 0, (struct sockaddr *)&server_addr_udp_l, len_udp_l);
                recvfrom(server_socket_udp, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *)&server_addr_udp_l, (socklen_t *)&len_udp_l);
                if(send_buffer[0]=='1'){
                    string number="";
                    for(int i=1;i<strlen(send_buffer);i++){
                        number+=send_buffer[i];
                    }
                    printf("Main Server received from server L the book status result using UDP over port 44495: Number of books %s available is: %s.\n",read_buffer,number.c_str());
                }else if(strcmp(send_buffer,cannot_find)==0){
                    printf("Did not find %s in the book code list.\n",read_buffer);
                }else if(strcmp(send_buffer,not_available)==0){
                    printf("Main Server received from server L the book status result using UDP over port 44495: Number of books %s available is: 0.\n",read_buffer);
                }
                write(client_socket, send_buffer, strlen(send_buffer));
            }else if(read_buffer[0]=='H'){
                printf("Found %s located at Server H. Send to Server H.\n",read_buffer);
                sendto(server_socket_udp, read_buffer, strlen(read_buffer), 0, (struct sockaddr *)&server_addr_udp_h, len_udp_h);
                recvfrom(server_socket_udp, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *)&server_addr_udp_h, (socklen_t *)&len_udp_h);
                if(send_buffer[0]=='1'){
                    string number="";
                    for(int i=1;i<strlen(send_buffer);i++){
                        number+=send_buffer[i];
                    }
                    printf("Main Server received from server H the book status result using UDP over port 44495: Number of books %s available is: %s.\n",read_buffer,number.c_str());
                }else if(strcmp(send_buffer,cannot_find)==0){
                    printf("Did not find %s in the book code list.\n",read_buffer);
                }else if(strcmp(send_buffer,not_available)==0){
                    printf("Main Server received from server H the book status result using UDP over port 44495: Number of books %s available is: 0.\n",read_buffer);
                }
                write(client_socket, send_buffer, strlen(send_buffer));
            }else{
                printf("None of the Backend servers should receive this request\n");
                write(client_socket, cannot_find, strlen(cannot_find));
            }
            printf("Main Server sent the book status to the client.\n");
        }
        memset(read_buffer,0,sizeof(read_buffer));
        memset(send_buffer,0,sizeof(send_buffer));
    }
    close(client_socket);
    close(server_socket_tcp);
    return 0;
}
