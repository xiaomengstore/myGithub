#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>
#include<stdbool.h>

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#define SCR_ROW 25             /*屏幕行数*/
#define SCR_COL 80             /*屏幕列数*/

/**
 *游戏玩家基本信息链结点结构
 */
typedef struct player_node {
    char user_name[15];           /**< 用户名*/
    char enroll_datec[10];         /**< 注册日期*/
    unsigned long enroll_date;       /**< 注册日期的数字表达*/
    char game_name[15];             /**< 游戏名称*/
    float total_time;                 /**< 游戏累计时长*/
    double  total_money;            /**< 消费总金额*/
    union achievement{
        double p;
        long t;
    }achievement;             /**< 排名指标*/
    char total_timec[15];   /*存放游戏累计时长的字符数组*/
    char total_moneyc[15];  /*存放消费总金额的字符数组*/
    char achievementc[15];  /*存放排名指标的字符数组*/
    struct player_node *next;  /**< 指向下一结点的指针*/
} PLAYER_NODE;

/**
 *游戏基本信息链结点结构
 */
typedef struct game_node {
   char class_id[4];              /**<游戏类别*/
   char game_name[15];              /**<游戏名称*/
   char game_adress[30];            /**<游戏网址*/


   struct player_node *pnext;       /**<指向玩家基本信息链*/

   struct game_node *next;          /**<指向下一结点的指针*/
   char charge_model[2];               /**<收费模式*/
    char rank_way[2];                   /**<排名方式*/
} GAME_NODE;

/**
 *游戏类别信息链结点结构
 */
typedef struct class_node {
    char class_id[4];               /**< 类别编码*/
    char class_dec[15];             /**< 类别描述*/
    struct game_node *gnext;         /**< 指向学生基本信息支链的指针*/
    struct class_node *next;         /**< 指向下一结点的指针*/
} CLASS_NODE;

/**
 *数据统计1：各款游戏信息链结点结构
 */
typedef struct game_stat_node {
    char game_name[15];         /**<游戏名称*/
    unsigned long player_num;   /**<玩家数*/
    double player_time;   /**<玩家游戏总时间（小时）*/
    double player_aver_time;    /**<玩家人均游戏时间（小时）*/
    double player_money;  /**<玩家消费总金额（元）*/
    double player_aver_money;   /**<玩家人均消费金额（元）*/
    struct game_stat_node *next;/**< 指向下一结点的指针*/
} GAME_STAT_NODE;

/**
 *数据统计2：游戏玩家排行榜信息链结点结构
 */
typedef struct game_rank_node {
    char class_id[4];               /**游戏类别*/
    char game_name[15];             /**游戏名称*/
    char rank_way[2];                  /**排名方式*/
    int rank_place;                 /**<名次*/
    char user_name[15];             /**<用户名*/
    double total_time;               /**< 游戏累计时长*/
    double  total_money;            /**< 消费总金额*/
    union achievement achievement;  /**< 排名指标*/;
    struct game_rank_node *next;    /**<指向下一个结点的指针*/
} GAME_RANK_NODE;

/**
 *数据统计3：各类游戏信息链结点结构
 */
typedef struct class_stat_node {
    char class_id[4];             /**<游戏类别*/
    char class_dec[15];             /**<类别描述*/
    unsigned long game_num;         /**<游戏款数*/
    double player_total_num;        /**<玩家数*/
    double playeer_total_time;      /**<玩家游戏总时间*/
    double player_total_money;      /**<玩家消费总金额*/
    struct class_stat_node *next;     	/**< 指向下一结点的指针*/
} CLASS_STAT_NODE;

/**
 *数据统计4：游戏玩家信息链结点结构
 */
typedef struct player_stat_node {
    char user_name[15];             /**<用户名*/
    double sum_time;                /**<游戏总时长（小时）*/
    double sum_money;               /**<消费总金额（元）*/
    struct player_stat_node *next;   	/**< 指向下一结点的指针*/
} PLAYER_STAT_NODE;

/**
 *数据统计5：收费模式信息链结点结构
 */
 typedef struct charge_node{
    char charge_model;              /**<收费模式*/
    unsigned long game_num;         /**<游戏款数*/
    double player_total_num;        /**<玩家数*/
    double playeer_total_time;      /**<玩家游戏总时间*/
    double player_total_money;      /**<玩家消费总金额*/
    struct charge_node *next;     	/**< 指向下一结点的指针*/
 }CHARGE_NODE;

/**
 *屏幕窗口信息链结点结点结构
 */
typedef struct layer_node {
    char LayerNo;            /**< 弹出窗口层数*/
    SMALL_RECT rcArea;       /**< 弹出窗口区域坐标*/
    CHAR_INFO *pContent;     /**< 弹出窗口区域字符单元原信息存储缓冲区*/
    char *pScrAtt;           /**< 弹出窗口区域字符单元原属性值存储缓冲区*/
    struct layer_node *next; /**< 指向下一结点的指针*/
} LAYER_NODE;

/**
 *标签束结构
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< 标签字符串数组首地址*/
    COORD *pLoc;           /**< 标签定位数组首地址*/
    int num;               /**< 标签个数*/
} LABEL_BUNDLE;

/**
 *热区结构
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< 热区定位数组首地址*/
    char *pSort;           /**< 热区类别(按键、文本框、选项框)数组首地址*/
    char *pTag;            /**< 热区序号数组首地址*/
    int num;               /**< 热区个数*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*弹出窗口信息链链头*/
CLASS_NODE *gp_head = NULL;                     /*主链头指针*/


char *gp_sys_name = "网游排行信息管理系统";    /*系统名称*/
char *gp_player_info_filename = "player.dat";    /*玩家基本信息数据文件*/
char *gp_game_info_filename = "game.dat";        /*游戏基本信息数据文件*/
char *gp_class_info_filename = "class.dat";      /*游戏类别信息数据文件*/

char *ga_main_menu[] = {"文件(F)",             /*系统主菜单名*/
    	                "数据维护(M)",
                        "数据查询(Q)",
                        "数据统计(S)",
                        "帮助(H)"
                       };

char *ga_sub_menu[] = {"[S] 数据保存",          /*系统子菜单名*/
                       "[B] 数据备份",
                       "[R] 数据恢复",
                       "[X] 退出    Alt+X",

                       "[C] 游戏分类信息",
                       "[G] 游戏基本信息",
                       "[P] 玩家基本信息",

                       "[C] 游戏分类信息",
                       "[G] 游戏基本信息",
                       "[P] 玩家基本信息",

                       "[A] 游戏信息统计",
                       "[R] 玩家排行统计",
                       "[N] 游戏类别统计",
                       "[T] 玩家信息统计",
                       "[M] 收费模式统计",

                       "[T] 帮助主题",
                       "",
                       "[A] 关于..."
                      };

int ga_sub_menu_count[] = {4, 3, 3, 5, 3};  /*各主菜单项下子菜单的个数*/
int gi_sel_menu = 1;                        /*被选中的主菜单项号,初始为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*存放菜单条屏幕区字符信息的缓冲区*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*存放状态条屏幕区字符信息的缓冲区*/

char *gp_scr_att = NULL;    /*存放屏幕上字符单元属性值的缓冲区*/
char gc_sys_state = '\0';   /*用来保存系统状态的字符*/

HANDLE gh_std_out;          /*标准输出设备句柄*/
HANDLE gh_std_in;           /*标准输入设备句柄*/

int CreatList(CLASS_NODE **pphead);              /*数据链表初始化*/
void InitInterface(void);                 /*系统界面初始化*/
void ClearScreen(void);                         /*清屏*/
void ShowMenu(void);                            /*显示菜单栏*/
void PopMenu(int num);                          /*显示下拉菜单*/
void PopPrompt(int num);                        /*显示弹出窗口*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*弹出窗口屏幕信息维护*/
void PopOff(void);                              /*关闭顶层弹出窗口*/
void DrawBox(SMALL_RECT *parea);                /*绘制边框*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*主菜单下拉菜单定位*/
void ShowState(void);                           /*显示状态栏*/
void TagMainMenu(int num);                      /*标记被选中的主菜单项*/
void TagSubMenu(int num);                       /*标记被选中的子菜单项*/
int DealInput(HOT_AREA *phot_area, int *pihot_num);  /*控制台输入处理*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*设置热区*/
void RunSys(CLASS_NODE **pphd);                  /*系统功能模块的选择和运行*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*功能模块的调用*/
void CloseSys(CLASS_NODE *phd);                  /*退出系统*/
BOOL ShowModule(char **pString, int n);

BOOL LoadData(CLASS_NODE**);   /*数据加载*/
BOOL SaveData(void);           /*保存数据*/
BOOL BackupData(void);         /*备份数据*/
BOOL RestoreData(void);        /*恢复数据*/
BOOL ExitSys(void);            /*退出系统*/
BOOL HelpTopic(void);          /*帮助主体*/
BOOL AboutGame(void);          /*关于系统*/

BOOL MaintainClassInfo(void);    /*维护类别信息*/
BOOL MaintainGameInfo(void);     /*维护游戏基本信息*/
BOOL MaintainPlayerInfo(void);   /*维护玩家基本信息*/

BOOL QueryClassInfo(void);       /*查询类别信息*/
BOOL QueryGameInfo(void);        /*查询游戏基本信息*/
BOOL QueryPlayerInfo(void);      /*查询玩家基本信息*/

BOOL StatGame(void);         /*游戏信息统计*/
BOOL StatRank(void);        /*玩家排行统计*/
BOOL StatClass(void);        /*游戏类别统计*/
BOOL StatPlayer(void);       /*玩家信息统计*/
BOOL StatCharge(void);       /*收费模式统计*/

/**
 *以下函数书写的时候注意名称，参数，返回值
 */
BOOL InsertClassNode(CLASS_NODE **phd,CLASS_NODE *pclass_node);/*插入游戏类别信息结点*/
BOOL DelClassNode(CLASS_NODE **phd,char *class_id);/*删除游戏类别信息结点*/
BOOL ModifClassNode(CLASS_NODE **phd, char *class_id,CLASS_NODE *pclass_node);/*修改游戏类别信息结点*/

BOOL InsertGameNode(CLASS_NODE *phd, GAME_NODE *pgame_node);/*插入游戏信息结点*/
BOOL DelGameNode(CLASS_NODE *phd, char *game_name);/*删除游戏信息结点*/
BOOL ModifGameNode(CLASS_NODE *phd, char *game_name,GAME_NODE *pgame_node);/*修改游戏信息结点*/

BOOL InsertPlayerNode(CLASS_NODE *phd, PLAYER_NODE *pplayer_node);/*插入玩家信息结点*/
BOOL DelPlayerNode(CLASS_NODE *phd, char *game_name, char *user_name);/*删除玩家信息结点*/
BOOL ModifPlayerNode(CLASS_NODE *phd, char *game_name, char *user_name,PLAYER_NODE *pplayer_node);/*修改玩家信息结点*/

CLASS_NODE *SeekClassNodeById(CLASS_NODE *phd, char *class_id); /*按类别编码查找游戏分类基本结点*/
CLASS_NODE *SeekClassNodeByDec(CLASS_NODE *phd, char *class_dec);/*按类别描述查找游戏分类基本结点*/
GAME_NODE *SeekGameNode(CLASS_NODE *phd, char *game_id);  /*查找游戏基本信息结点*/
GAME_NODE *SeekGameNodeM(CLASS_NODE *phd,char**);/*模糊查询游戏基本信息结点*/
PLAYER_NODE *SeekPlayerNode(CLASS_NODE *phd, char *game_name, char *user_name);/*查找玩家基本信息结点*/
PLAYER_NODE *SeekPlayerNodeM (CLASS_NODE *phd, char* condition1,char* condition2);/*模糊查询玩家基本信息结点*/

void VInsertClassNode(void);
void VInsertGameNode(void);
void VInsertPlayerNode(void);
void VDelClassNode(void);
void VDelGameNode(void);
void VDelPlayerNode(void);
void VModifClassNode(void);
void VVModifClassNode(CLASS_NODE *class_node);
void VModifGameNode(void);
void VVModifGameNode(GAME_NODE* game_node);
void VModifPlayerNode(void);
void VVModifPlayerNode(PLAYER_NODE* player_node);

void VQueryClassInfoById(void);
void VQueryClassInfoByDec(void);
void VQueryGameInfo(void);
void VQueryGameInfoM(void);
void VVQueryGameInfoM(GAME_NODE* game_node);
void VQueryPlayerInfo(void);
void VQueryPlayerInfoM(void);
void VVQueryPlayerInfoM(PLAYER_NODE* player_node);

BOOL JudgeGameNodeItem(GAME_NODE *pgame_node, char *pcond);/*判断游戏基本信息数据项是否满足查询条件*/
BOOL JudgePlayerNodeItem(CLASS_NODE *phd,PLAYER_NODE *pplayer_node, char *pcond);/*判断玩家基本信息数据项是否满足查询条件*/
BOOL MatchString(char *string_item, char *pcond);/*字符串与查询条件是否满足指定的运算关系*/
BOOL MatchChar(char char_item, char *pcond);/*字符与查询条件是否满足指定的运算关系*/
BOOL MatchLong(long long_item, char *pcond);/*long型数据与查询条件是否满足指定的运算关系*/
BOOL MatchDouble(double double_item, char *pcond);/*double型数据与查询条件是否满足指定的运算关系*/

GAME_STAT_NODE *StatGameInfo(CLASS_NODE *phd);      /*游戏信息统计*/
void SortGameInfo(GAME_STAT_NODE *pgame_hd);        /*按玩家玩游戏平均时间排序*/
void VStatGame(GAME_STAT_NODE* game_stat_node);

GAME_RANK_NODE *StatRankList(CLASS_NODE *phd, char *game_name);/*玩家排行统计*/
void SortRankList_p(GAME_RANK_NODE *prank_hd);        /*按p排名方式排序*/
void SortRankList_t(GAME_RANK_NODE *prank_hd);        /*按t排名方式排序*/
void VStatRank(GAME_RANK_NODE* game_rank_node);

CLASS_STAT_NODE *StatClassInfo(CLASS_NODE *phd);    /*游戏类别统计*/
void SortClassInfo(CLASS_STAT_NODE *pclass_hd);     /*按玩家数排序*/
void VStatClass(CLASS_STAT_NODE* class_stat_node);

PLAYER_STAT_NODE *StatPlayerInfo(CLASS_NODE *phd);  /*玩家信息统计*/
PLAYER_STAT_NODE *Serch_User_Name(PLAYER_STAT_NODE *pplayer,char *user_name);/*搜索生成链上是否含有此用户名*/
void SortPlayerInfo(PLAYER_STAT_NODE *pplayer_hd);  /*按游戏累计时长排序*/
void VStatPlayer(PLAYER_STAT_NODE* player_stat_node);

CHARGE_NODE *StatChargeInfo(CLASS_NODE *phd);       /*收费模式统计*/
void Game_Charge(GAME_NODE *game_node, CHARGE_NODE *pcharge);
 /*统计指定游戏的玩家数，玩家总时间和玩家总金额,数据加到对应收费模式的主链结点*/
void SortChargeInfo(CHARGE_NODE *pcharge_hd);       /*按玩家数排序*/
void VStatCharge(CHARGE_NODE* charge_node);

BOOL SaveSysData(CLASS_NODE *phd);                       /*保存系统数据*/
BOOL BackupSysData(CLASS_NODE *phd, char *filename);     /*备份系统数据*/
BOOL RestoreSysData(CLASS_NODE **pphd, char *filename);  /*恢复系统数据*/
void VRestoreData();

BOOL ShowModuleWithExp(char **pString, int n);
BOOL HShowModule(char **pString, int n);
void Move(HOT_AREA *pHotArea, int *piHot,char asc,char vkc);

#endif // GAME_H_INCLUDED
