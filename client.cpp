//
//  main.cpp
//  client
//
//  Created by 麦卡比 on 2023/11/14.
//

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
char login_success[]="1";
char no_user_name[]="0";
char no_password[]="-1";
char admin[]="2";
char can_find[]="1";
char cannot_find[]="2";
char not_available[]="0";
bool admin_flag=false;
void encrypted(char *str){
    int i=0;
    while(str[i]!='\0'){
        if((str[i]>='0' && str[i]<='9')){
            str[i]=((str[i]-'0')+5)%10+'0';
        }else if((str[i]>='a' && str[i]<='z')){
            str[i]=((str[i]-'a')+5)%26+'a';
        }else if((str[i]>='A' && str[i]<='Z')){
            str[i]=((str[i]-'A')+5)%26+'A';
        }
        i++;
    }
}
int main(int argc, const char * argv[]) {
    
    //main_server_socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr.sin_port=htons(45495);
    connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    char username[1024];
    char password[1024];
    char login_message[2048];
    char buffer[1024];
    char us_t[1024];
    memset(username,0,sizeof(username));
    memset(password,0,sizeof(password));
    memset(login_message,0,sizeof(login_message));
    memset(buffer,0,sizeof(buffer));
    memset(us_t,0,sizeof(us_t));
    while(true){
        printf("Please enter the username:");
        scanf("%[^\n]",username);
        getchar();
        printf("Please enter the password:");
        scanf("%[^\n]",password);
        getchar();
        strcpy(us_t,username);
        encrypted(username);
        encrypted(password);
        strcat(login_message,username);
        strcat(login_message,"\n");
        strcat(login_message,password);
        write(sock, login_message, strlen(login_message));
        printf("%s sent an authentication request to the Main Server.\n", us_t);
        read(sock, buffer, sizeof(buffer));
        if(strcmp(buffer, login_success)==0){
            memset(username,0,sizeof(username));
            memset(password,0,sizeof(password));
            memset(login_message,0,sizeof(login_message));
            memset(buffer,0,sizeof(buffer));
            printf("%s received the result of authentication from Main Server using TCP over port 45495. Authentication is successful.\n", us_t);
            break;
        }else if(strcmp(buffer, no_user_name)==0){
            printf("%s received the result of authentication from Main Server using TCP over port 45495. Authentication failed: Username not found.\n", us_t);
        }else if(strcmp(buffer, no_password)==0){
            printf("%s received the result of authentication from Main Server using TCP over port 45495. Authentication failed: Password does not match.\n",us_t);
        }else if(strcmp(buffer, admin)==0){
            printf("%s received the result of authentication from Main Server using TCP over port 45495. Authentication is successful.\n", us_t);
            admin_flag=true;
            memset(username,0,sizeof(username));
            memset(password,0,sizeof(password));
            memset(login_message,0,sizeof(login_message));
            memset(buffer,0,sizeof(buffer));
            break;
        }
        memset(username,0,sizeof(username));
        memset(password,0,sizeof(password));
        memset(login_message,0,sizeof(login_message));
        memset(buffer,0,sizeof(buffer));
    }
    char book_id[1024];
    while(true){
        memset(book_id,0,sizeof(book_id));
        memset(buffer,0,sizeof(buffer));
        printf("please enter the book code to query:\n");
        scanf("%s",book_id);
        if(admin_flag){
            printf("Request sent to the Main Server with Admin rights.\n");
        }else{
            printf("%s sent the request to the Main Server.\n",us_t);
        }
        write(sock, book_id, strlen(book_id));
        read(sock, buffer, sizeof(buffer));
        printf("Response received from the Main Server on TCP port: 45495\n");
        if(admin_flag){
            if(buffer[0]=='1'){
                string number="";
                for(int i=1;i<strlen(buffer);i++){
                    number+=buffer[i];
                }
                printf("Total number of book %s available = %s\n\n—- Start a new query —-\n",book_id,number.c_str());
            }else if(strcmp(buffer,cannot_find)==0){
                printf("Not able to find the book-code %s in the system.\n\n—- Start a new query —-\n",book_id);
            }else if(strcmp(buffer,not_available)==0){
                printf("Total number of book %s available = 0\n\n—- Start a new query —-\n",book_id);
            }
        }else{
            if(buffer[0]=='1'){
                printf("The requested book %s is available in the library.\n\n—- Start a new query —-\n",book_id);
            }else if(strcmp(buffer,cannot_find)==0){
                printf("Not able to find the book-code %s in the system.\n\n—- Start a new query —-\n",book_id);
            }else if(strcmp(buffer,not_available)==0){
                printf("The requested book %s is NOT available in the library.\n\n—- Start a new query —-\n",book_id);
            }
        }
    }
    close(sock);
    return 0;
}

