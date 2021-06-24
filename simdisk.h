//这是一个基于linux second extension file system体系的模拟多用户文件系统的设计
//author: Sishxo
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<time.h>
using namespace std;

#define BLOCK_SIZE 1024
#define INODE 128
#define MAX_NAME_SIZE 28//文件名最大长度

#define INODE_NUM 640
#define BLOCK_NUM 102400
#define BLOCKS_PER_GROUP 64

#define MODE_DIR 01000
#define MODE_FILE 00000
#define OWNER_R 100000000 //用户读权限
#define OWNER_W 010000000 //用户写权限
#define OWENR_X 001000000 //用户执行权限
#define GROUP_R 000100000
#define GROUP_W 000010000
#define GROUP_X 000001000
#define OTHERS_R 000000100
#define OTHERS_W 000000010
#define OTHERS_X 000000001
#define FILE_DEF_PERMISSION 0664 //可读可写&可读可写&可读 r=4 w=2 x=1
#define DIR_DEF_PERMISSION 0755  //可读可写可运行&可读可运行&可读可运行

#define FILESYSNAME "FileSystem.sys"

struct User{
    char username[8];
    char username[8];
};

struct Inode{
    unsigned short iInodeNo;//当前inode的编号
    unsigned short iMode;//rwx读写执行权限
    unsigned short iCount;//文件名指向这个Inode的链接数
    
    char iUserName[20];//文件所属的用户名
    char iGroupName[20];//文件所属的用户组名
    
    unsigned int iFileSize;//该Inode指向的文件大小，单位byte
    time_t iChangeTime;//该文件创建或者状态改变的时间,也即inode发生改变的时间
    time_t iAccessTime;//最近一次读取时间
    time_t iModifyTime;//最近一次文件内容被修改的时间

    int iDirectBlock[10];//10个直接block指向，共10*1024B=10KB
    int iIndirectBlock;//一个单间接，每条block号占用4bytes，1k可以存256条，共256k大小
    int iDoubleIndirectBlock;//一个双间接block指向，64MB大小 
    //每个文件最大允许64MB大小   
    
};

struct SuperBlock{
    unsigned short sInodeNum;//最大inode数量，也即文件和目录总共数目
    unsigned int sBlockNum;//数据块块数，共有100M，每个块1k，那就是102400个block

    unsigned short sFreeInodeNUm;//仍然空闲的inode数目
    
    unsigned int sFreeBlockNum;//仍然空闲的data block数目
    unsigned short sBlockSize;//数据块大小为1024bytes
    unsigned short sInodeSize;//inode大小128bytes
    unsigned short sSuperBlockSize;//超级块大小1024bytes
    unsigned short sBlockPerGroup;//每个block group的block数量 一个64,共1600个group

    int sFreeAddr; //用于指向空闲块的指针，索引
    int sFree[BLOCKS_PER_GROUP];//用于指向空闲块分组链接法内部block

    //superblock,inode,block,bitmap的磁盘位置
    int sSuperBlockStartAddr;
    int sInodeBitmapStartAddr;
    int sBlockBitmapStartAddr;
    int sInodeStartAddr;
    int sBlockStartAddr;
};

struct DirItem{//32个byte，一个block块1024B能存32个目录项
    char itemName[MAX_NAME_SIZE]; //目录或文件名
    int InodeAddr; //目录项对应的inode地址
};

//全局const变量定义
extern SuperBlock *superblock;
extern const int InodeStartAddr;
extern const int SuperBlockStartAddr;
extern const int InodeBitmapStartAddr;
extern const int BlockBitmapStartAddr;
extern const int FileMaxSize;
extern const int DiskSumSize;

//全局普通变量定义
extern int RootDirAddr; //根目录inode所在的地址
extern int CurrentDirAddr;
extern int TreeDirAddr;
extern int CurrentDirAddrForFindFile;
extern char CurrentDirName[310];//当前目录名称
extern char CurrentHostName[110];//当前host的名字
extern char CurrentUserName[110];//当前登陆用户的名字
extern char CurrentGroupName[110];//当前用户组的名字
extern char CurrentUserDirName[310];//当前登陆用户的目录名字

extern int nextUID;//下一个要分配的用户id
extern int nextGID;//下一个要分配的用户组id

extern bool isLogin;//是否登陆标识

extern FILE* fw;//虚拟磁盘文件，写文件指针
extern FILE* fr;//虚拟磁盘文件，读文件指针
extern bool InodeBitMap[INODE_NUM];//用于inode位示图
extern bool BlockBitMap[BLOCK_NUM];//用于data block位示图

extern char buffer[100000000];//100MB,缓存可用的最大磁盘容量100MB

/*声明必要的函数
**info 显示整个系统信息
**login 用户登录
**dir  列出目录(ls)
**create 创建文件(mkdir,touch)
**delete 删除文件(rmdir,del)
**open 打开文件(cat)
**close 关闭文件
**read 读文件
**copy 复制文件
**write 写文件
**cd 切换目录
*/

