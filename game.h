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

#define SCR_ROW 25             /*��Ļ����*/
#define SCR_COL 80             /*��Ļ����*/

/**
 *��Ϸ��һ�����Ϣ�����ṹ
 */
typedef struct player_node {
    char user_name[15];           /**< �û���*/
    char enroll_datec[10];         /**< ע������*/
    unsigned long enroll_date;       /**< ע�����ڵ����ֱ��*/
    char game_name[15];             /**< ��Ϸ����*/
    float total_time;                 /**< ��Ϸ�ۼ�ʱ��*/
    double  total_money;            /**< �����ܽ��*/
    union achievement{
        double p;
        long t;
    }achievement;             /**< ����ָ��*/
    char total_timec[15];   /*�����Ϸ�ۼ�ʱ�����ַ�����*/
    char total_moneyc[15];  /*��������ܽ����ַ�����*/
    char achievementc[15];  /*�������ָ����ַ�����*/
    struct player_node *next;  /**< ָ����һ����ָ��*/
} PLAYER_NODE;

/**
 *��Ϸ������Ϣ�����ṹ
 */
typedef struct game_node {
   char class_id[4];              /**<��Ϸ���*/
   char game_name[15];              /**<��Ϸ����*/
   char game_adress[30];            /**<��Ϸ��ַ*/


   struct player_node *pnext;       /**<ָ����һ�����Ϣ��*/

   struct game_node *next;          /**<ָ����һ����ָ��*/
   char charge_model[2];               /**<�շ�ģʽ*/
    char rank_way[2];                   /**<������ʽ*/
} GAME_NODE;

/**
 *��Ϸ�����Ϣ�����ṹ
 */
typedef struct class_node {
    char class_id[4];               /**< ������*/
    char class_dec[15];             /**< �������*/
    struct game_node *gnext;         /**< ָ��ѧ��������Ϣ֧����ָ��*/
    struct class_node *next;         /**< ָ����һ����ָ��*/
} CLASS_NODE;

/**
 *����ͳ��1��������Ϸ��Ϣ�����ṹ
 */
typedef struct game_stat_node {
    char game_name[15];         /**<��Ϸ����*/
    unsigned long player_num;   /**<�����*/
    double player_time;   /**<�����Ϸ��ʱ�䣨Сʱ��*/
    double player_aver_time;    /**<����˾���Ϸʱ�䣨Сʱ��*/
    double player_money;  /**<��������ܽ�Ԫ��*/
    double player_aver_money;   /**<����˾����ѽ�Ԫ��*/
    struct game_stat_node *next;/**< ָ����һ����ָ��*/
} GAME_STAT_NODE;

/**
 *����ͳ��2����Ϸ������а���Ϣ�����ṹ
 */
typedef struct game_rank_node {
    char class_id[4];               /**��Ϸ���*/
    char game_name[15];             /**��Ϸ����*/
    char rank_way[2];                  /**������ʽ*/
    int rank_place;                 /**<����*/
    char user_name[15];             /**<�û���*/
    double total_time;               /**< ��Ϸ�ۼ�ʱ��*/
    double  total_money;            /**< �����ܽ��*/
    union achievement achievement;  /**< ����ָ��*/;
    struct game_rank_node *next;    /**<ָ����һ������ָ��*/
} GAME_RANK_NODE;

/**
 *����ͳ��3��������Ϸ��Ϣ�����ṹ
 */
typedef struct class_stat_node {
    char class_id[4];             /**<��Ϸ���*/
    char class_dec[15];             /**<�������*/
    unsigned long game_num;         /**<��Ϸ����*/
    double player_total_num;        /**<�����*/
    double playeer_total_time;      /**<�����Ϸ��ʱ��*/
    double player_total_money;      /**<��������ܽ��*/
    struct class_stat_node *next;     	/**< ָ����һ����ָ��*/
} CLASS_STAT_NODE;

/**
 *����ͳ��4����Ϸ�����Ϣ�����ṹ
 */
typedef struct player_stat_node {
    char user_name[15];             /**<�û���*/
    double sum_time;                /**<��Ϸ��ʱ����Сʱ��*/
    double sum_money;               /**<�����ܽ�Ԫ��*/
    struct player_stat_node *next;   	/**< ָ����һ����ָ��*/
} PLAYER_STAT_NODE;

/**
 *����ͳ��5���շ�ģʽ��Ϣ�����ṹ
 */
 typedef struct charge_node{
    char charge_model;              /**<�շ�ģʽ*/
    unsigned long game_num;         /**<��Ϸ����*/
    double player_total_num;        /**<�����*/
    double playeer_total_time;      /**<�����Ϸ��ʱ��*/
    double player_total_money;      /**<��������ܽ��*/
    struct charge_node *next;     	/**< ָ����һ����ָ��*/
 }CHARGE_NODE;

/**
 *��Ļ������Ϣ�������ṹ
 */
typedef struct layer_node {
    char LayerNo;            /**< �������ڲ���*/
    SMALL_RECT rcArea;       /**< ����������������*/
    CHAR_INFO *pContent;     /**< �������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;           /**< �������������ַ���Ԫԭ����ֵ�洢������*/
    struct layer_node *next; /**< ָ����һ����ָ��*/
} LAYER_NODE;

/**
 *��ǩ���ṹ
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
    int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

/**
 *�����ṹ
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
    char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
    char *pTag;            /**< ������������׵�ַ*/
    int num;               /**< ��������*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*����������Ϣ����ͷ*/
CLASS_NODE *gp_head = NULL;                     /*����ͷָ��*/


char *gp_sys_name = "����������Ϣ����ϵͳ";    /*ϵͳ����*/
char *gp_player_info_filename = "player.dat";    /*��һ�����Ϣ�����ļ�*/
char *gp_game_info_filename = "game.dat";        /*��Ϸ������Ϣ�����ļ�*/
char *gp_class_info_filename = "class.dat";      /*��Ϸ�����Ϣ�����ļ�*/

char *ga_main_menu[] = {"�ļ�(F)",             /*ϵͳ���˵���*/
    	                "����ά��(M)",
                        "���ݲ�ѯ(Q)",
                        "����ͳ��(S)",
                        "����(H)"
                       };

char *ga_sub_menu[] = {"[S] ���ݱ���",          /*ϵͳ�Ӳ˵���*/
                       "[B] ���ݱ���",
                       "[R] ���ݻָ�",
                       "[X] �˳�    Alt+X",

                       "[C] ��Ϸ������Ϣ",
                       "[G] ��Ϸ������Ϣ",
                       "[P] ��һ�����Ϣ",

                       "[C] ��Ϸ������Ϣ",
                       "[G] ��Ϸ������Ϣ",
                       "[P] ��һ�����Ϣ",

                       "[A] ��Ϸ��Ϣͳ��",
                       "[R] �������ͳ��",
                       "[N] ��Ϸ���ͳ��",
                       "[T] �����Ϣͳ��",
                       "[M] �շ�ģʽͳ��",

                       "[T] ��������",
                       "",
                       "[A] ����..."
                      };

int ga_sub_menu_count[] = {4, 3, 3, 5, 3};  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/

char *gp_scr_att = NULL;    /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/
char gc_sys_state = '\0';   /*��������ϵͳ״̬���ַ�*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/

int CreatList(CLASS_NODE **pphead);              /*���������ʼ��*/
void InitInterface(void);                 /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopPrompt(int num);                        /*��ʾ��������*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
int DealInput(HOT_AREA *phot_area, int *pihot_num);  /*����̨���봦��*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys(CLASS_NODE **pphd);                  /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
void CloseSys(CLASS_NODE *phd);                  /*�˳�ϵͳ*/
BOOL ShowModule(char **pString, int n);

BOOL LoadData(CLASS_NODE**);   /*���ݼ���*/
BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL RestoreData(void);        /*�ָ�����*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/
BOOL HelpTopic(void);          /*��������*/
BOOL AboutGame(void);          /*����ϵͳ*/

BOOL MaintainClassInfo(void);    /*ά�������Ϣ*/
BOOL MaintainGameInfo(void);     /*ά����Ϸ������Ϣ*/
BOOL MaintainPlayerInfo(void);   /*ά����һ�����Ϣ*/

BOOL QueryClassInfo(void);       /*��ѯ�����Ϣ*/
BOOL QueryGameInfo(void);        /*��ѯ��Ϸ������Ϣ*/
BOOL QueryPlayerInfo(void);      /*��ѯ��һ�����Ϣ*/

BOOL StatGame(void);         /*��Ϸ��Ϣͳ��*/
BOOL StatRank(void);        /*�������ͳ��*/
BOOL StatClass(void);        /*��Ϸ���ͳ��*/
BOOL StatPlayer(void);       /*�����Ϣͳ��*/
BOOL StatCharge(void);       /*�շ�ģʽͳ��*/

/**
 *���º�����д��ʱ��ע�����ƣ�����������ֵ
 */
BOOL InsertClassNode(CLASS_NODE **phd,CLASS_NODE *pclass_node);/*������Ϸ�����Ϣ���*/
BOOL DelClassNode(CLASS_NODE **phd,char *class_id);/*ɾ����Ϸ�����Ϣ���*/
BOOL ModifClassNode(CLASS_NODE **phd, char *class_id,CLASS_NODE *pclass_node);/*�޸���Ϸ�����Ϣ���*/

BOOL InsertGameNode(CLASS_NODE *phd, GAME_NODE *pgame_node);/*������Ϸ��Ϣ���*/
BOOL DelGameNode(CLASS_NODE *phd, char *game_name);/*ɾ����Ϸ��Ϣ���*/
BOOL ModifGameNode(CLASS_NODE *phd, char *game_name,GAME_NODE *pgame_node);/*�޸���Ϸ��Ϣ���*/

BOOL InsertPlayerNode(CLASS_NODE *phd, PLAYER_NODE *pplayer_node);/*���������Ϣ���*/
BOOL DelPlayerNode(CLASS_NODE *phd, char *game_name, char *user_name);/*ɾ�������Ϣ���*/
BOOL ModifPlayerNode(CLASS_NODE *phd, char *game_name, char *user_name,PLAYER_NODE *pplayer_node);/*�޸������Ϣ���*/

CLASS_NODE *SeekClassNodeById(CLASS_NODE *phd, char *class_id); /*�������������Ϸ����������*/
CLASS_NODE *SeekClassNodeByDec(CLASS_NODE *phd, char *class_dec);/*���������������Ϸ����������*/
GAME_NODE *SeekGameNode(CLASS_NODE *phd, char *game_id);  /*������Ϸ������Ϣ���*/
GAME_NODE *SeekGameNodeM(CLASS_NODE *phd,char**);/*ģ����ѯ��Ϸ������Ϣ���*/
PLAYER_NODE *SeekPlayerNode(CLASS_NODE *phd, char *game_name, char *user_name);/*������һ�����Ϣ���*/
PLAYER_NODE *SeekPlayerNodeM (CLASS_NODE *phd, char* condition1,char* condition2);/*ģ����ѯ��һ�����Ϣ���*/

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

BOOL JudgeGameNodeItem(GAME_NODE *pgame_node, char *pcond);/*�ж���Ϸ������Ϣ�������Ƿ������ѯ����*/
BOOL JudgePlayerNodeItem(CLASS_NODE *phd,PLAYER_NODE *pplayer_node, char *pcond);/*�ж���һ�����Ϣ�������Ƿ������ѯ����*/
BOOL MatchString(char *string_item, char *pcond);/*�ַ������ѯ�����Ƿ�����ָ���������ϵ*/
BOOL MatchChar(char char_item, char *pcond);/*�ַ����ѯ�����Ƿ�����ָ���������ϵ*/
BOOL MatchLong(long long_item, char *pcond);/*long���������ѯ�����Ƿ�����ָ���������ϵ*/
BOOL MatchDouble(double double_item, char *pcond);/*double���������ѯ�����Ƿ�����ָ���������ϵ*/

GAME_STAT_NODE *StatGameInfo(CLASS_NODE *phd);      /*��Ϸ��Ϣͳ��*/
void SortGameInfo(GAME_STAT_NODE *pgame_hd);        /*���������Ϸƽ��ʱ������*/
void VStatGame(GAME_STAT_NODE* game_stat_node);

GAME_RANK_NODE *StatRankList(CLASS_NODE *phd, char *game_name);/*�������ͳ��*/
void SortRankList_p(GAME_RANK_NODE *prank_hd);        /*��p������ʽ����*/
void SortRankList_t(GAME_RANK_NODE *prank_hd);        /*��t������ʽ����*/
void VStatRank(GAME_RANK_NODE* game_rank_node);

CLASS_STAT_NODE *StatClassInfo(CLASS_NODE *phd);    /*��Ϸ���ͳ��*/
void SortClassInfo(CLASS_STAT_NODE *pclass_hd);     /*�����������*/
void VStatClass(CLASS_STAT_NODE* class_stat_node);

PLAYER_STAT_NODE *StatPlayerInfo(CLASS_NODE *phd);  /*�����Ϣͳ��*/
PLAYER_STAT_NODE *Serch_User_Name(PLAYER_STAT_NODE *pplayer,char *user_name);/*�������������Ƿ��д��û���*/
void SortPlayerInfo(PLAYER_STAT_NODE *pplayer_hd);  /*����Ϸ�ۼ�ʱ������*/
void VStatPlayer(PLAYER_STAT_NODE* player_stat_node);

CHARGE_NODE *StatChargeInfo(CLASS_NODE *phd);       /*�շ�ģʽͳ��*/
void Game_Charge(GAME_NODE *game_node, CHARGE_NODE *pcharge);
 /*ͳ��ָ����Ϸ��������������ʱ�������ܽ��,���ݼӵ���Ӧ�շ�ģʽ���������*/
void SortChargeInfo(CHARGE_NODE *pcharge_hd);       /*�����������*/
void VStatCharge(CHARGE_NODE* charge_node);

BOOL SaveSysData(CLASS_NODE *phd);                       /*����ϵͳ����*/
BOOL BackupSysData(CLASS_NODE *phd, char *filename);     /*����ϵͳ����*/
BOOL RestoreSysData(CLASS_NODE **pphd, char *filename);  /*�ָ�ϵͳ����*/
void VRestoreData();

BOOL ShowModuleWithExp(char **pString, int n);
BOOL HShowModule(char **pString, int n);
void Move(HOT_AREA *pHotArea, int *piHot,char asc,char vkc);

#endif // GAME_H_INCLUDED
