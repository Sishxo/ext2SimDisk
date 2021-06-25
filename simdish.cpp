#include "simdisk.h"
using namespace std;

void Ready(){
    //初始化变量
    nextUID = 0;
    nextGID = 0;
    isLogin = false;
    strcpy(CurrentUserName, "root");
    strcpy(CurrentGroupName, "root");

    //获取host名字 when in linux
    memset(CurrentHostName,0,sizeof(CurrentHostName));
    int hostLength=100;
    gethostname(CurrentHostName,hostLength);   
    /*when in windows
    DWORD hostLength=100;
    GetComputerName(CurrentHostName.&hostLength)
    */

    //初始化根目录地址,和根目录名字
    RootDirAddr=InodeStartAddr;
    CurrentDirAddr=RootDirAddr;
    strcpy(CurrentDirName,"/");

    char c;
    printf("Are you sure to format your file system?[y/n]");
    
}

int main(){
    char c;
    c=getchar();
    cout<<c;
    return 0;
}