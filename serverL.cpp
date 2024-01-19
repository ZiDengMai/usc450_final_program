//
//  main.cpp
//  udp_l_server
//
//  Created by 麦卡比 on 2023/11/20.
//
#include <iostream>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <stack>
using namespace std;
struct books{
    string id;
    int number;
};
struct books arr[1000];
int an=0;
char can_find[]="1";
char cannot_find[]="2";
char not_available[]="0";
char login_success[]="1";
char no_user_name[]="0";
char no_password[]="-1";
char admin[]="2";
char a_num[50];
bool admin_flag=false;
string number_to_string(int number){
    string ans="";
    if(number==0){
        ans="0";
        return ans;
    }
    stack<int> st;
    while(number!=0){
        st.push(number%10);
        number/=10;
    }
    while(!st.empty()){
        ans+=(char)(st.top()+'0');
        st.pop();
    }
    return ans;
}
int string_to_number(string s){
    int n=s.size();
    int times=1;
    int ans=0;
    for(int i=n-1;i>=0;i--){
        ans+=(s[i]-'0')*times;
        times*=10;
    }
    return ans;
}
void loading(){
    ifstream infile;
    infile.open("./literature.txt");
    infile.seekg(0, infile.end);
    int len=infile.tellg();
    infile.seekg(0, infile.beg);
    char buffer[1024];
    infile.read(buffer, len);
    infile.close();
    int ai=0;
    len=strlen(buffer);
    for(int ci=0;ci<len;ci++){
        string ids="";
        string count="";
        while(ci<len && buffer[ci]!=','){
            ids+=buffer[ci];
            ci++;
        }
        ci+=2;
        while(ci<len && (buffer[ci]!='\n' && buffer[ci]!='\r')){
            count+=buffer[ci];
            ci++;
        }
        if(buffer[ci]=='\r'){
            ci++;
        }
        int num=string_to_number(count);
        struct books b={ids,num};
        arr[ai++]=b;
    }
    an=ai;
}
void rewrite_file(){
    ofstream outfile;
    outfile.open("./literature.txt");
    char out[1024];
    char temp[1024];
    string t="";
    //cout<<an<<endl;
    for(int i=0;i<an;i++){
        t+=arr[i].id;
        t+=", ";
        int cou=0;
        cou=arr[i].number;
        t+=number_to_string(cou);
        t+="\r";
        t+="\n";
        //cout<<t;
    }
    //cout<<t;
    memset(out,0,sizeof(out));
    memset(temp,0,sizeof(temp));
    strcat(out, strcpy(temp,t.c_str()));
    outfile.write(out, strlen(out));
    outfile.close();
}
int check_book_code(char *code){
    for(int i=0;i<an;i++){
        if(strcmp(code,arr[i].id.c_str())==0 && arr[i].number>0){
            memset(a_num,0,sizeof(a_num));
            strcpy(a_num,number_to_string(arr[i].number).c_str());
            if(!admin_flag){
                arr[i].number-=1;
            }
            return 1;
        }else if(strcmp(code,arr[i].id.c_str())==0 && arr[i].number==0){
            return 0;
        }
    }
    return 2;
}
int main(int argc, const char * argv[]) {
    //binding server
    loading();
    int server_socket_udp_main_server = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr_udp_main_server;
    memset(&server_addr_udp_main_server,0,sizeof(sockaddr_in));
    server_addr_udp_main_server.sin_family=AF_INET;
    server_addr_udp_main_server.sin_addr.s_addr=inet_addr("127.0.0.1");
    server_addr_udp_main_server.sin_port=htons(42495);
    int len=sizeof(server_addr_udp_main_server);
    bind(server_socket_udp_main_server,(struct sockaddr*)&server_addr_udp_main_server,sizeof(server_addr_udp_main_server));
    printf("Server L is up and running using UDP on port 42495\n");
    struct sockaddr_in addr_client;
    char admin_f[20];
    memset(admin_f,0,sizeof(admin_f));
    recvfrom(server_socket_udp_main_server, admin_f, sizeof(admin_f), 0, (struct sockaddr *) &addr_client, (socklen_t *) &len);
    if(strcmp(admin_f,admin)==0){
        admin_flag=true;
    }
    char recv_buffer[1024];
    char send_buffer[1024];
    while(true){
        memset(recv_buffer, 0, sizeof(recv_buffer));
        memset(send_buffer, 0, sizeof(send_buffer));
        recvfrom(server_socket_udp_main_server, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *) &addr_client, (socklen_t *) &len);
        printf("Server L received %s code from the Main Server\n", recv_buffer);
        //loading();
        int ans=check_book_code(recv_buffer);
        if(ans==1){
            strcpy(send_buffer,can_find);
            strcat(send_buffer, a_num);
            //rewrite_file();
        }else if(ans==0){
            strcpy(send_buffer,not_available);
        }else{
            strcpy(send_buffer,cannot_find);
        }
        if(!admin_flag){
            printf("Server L finished sending the availability status of code %s to the Main Server using UDP on port 42495\n",recv_buffer);
        }else{
            printf("Server L received an inventory status request for code %s.\n",recv_buffer);
        }
        sendto(server_socket_udp_main_server, send_buffer, strlen(send_buffer), 0, (struct sockaddr *)&addr_client, len);
        if(!admin_flag){
            printf("Server L finished sending the availability status of code %s to the Main Server using UDP on port 42495\n.",recv_buffer);
        }else{
            printf("Server L finished sending the inventory status to the Main server using UDP on port 42495\n");
        }
    }
    return 0;
}
