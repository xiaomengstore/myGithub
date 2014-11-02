#include "game.h"

unsigned long ul;

int main() {
    COORD size = {SCR_COL, SCR_ROW};              /*窗口缓冲区大小*/
    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出设备句柄*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* 获取标准输入设备句柄*/

    SetConsoleTitle(gp_sys_name);                 /*设置窗口标题*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*设置窗口缓冲区大小80*25*/
    LoadData(&gp_head);                   /*数据加载*/
    InitInterface();          /*界面初始化*/
    RunSys(&gp_head);             /*系统功能模块的选择及运行*/
    CloseSys(gp_head);            /*退出系统*/

    return 0;
}
/**
 * 函数名称: LoadData
 * 函数功能: 将三类基础数据从数据文件载入到内存缓冲区和十字链表中.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, 功能函数中除了函数ExitSys的返回值可以为FALSE外,
 *           其他函数的返回值必须为TRUE.
 *
 * 调用说明: 为了能够以统一的方式调用各功能函数, 将这些功能函数的原型设为
 *           一致, 即无参数且返回值为BOOL. 返回值为FALSE时, 结束程序运行.
 */
BOOL LoadData(CLASS_NODE** gp_head) {
    int Re = 0;
    Re = CreatList(gp_head);
    gc_sys_state |= Re;
    gc_sys_state &= ~(4 + 8 + 16 - Re);
    if (gc_sys_state < ( 4 | 8 | 16)) {
        /*数据加载提示信息*/
        printf("\n系统基础数据不完整!\n");
        printf("\n按任意键继续...\n");
        getchar();
    }

    return TRUE;
}

/**
 * 函数名称: CreatList
 * 函数功能: 从数据文件读取基础数据, 并存放到所创建的十字链表中.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址, 用来返回所创建的十字链.
 * 返 回 值: int型数值, 表示链表创建的情况.
 *           0  空链, 无数据
 *           4  已加载游戏类别信息数据，无游戏基本信息和玩家基本信息数据
 *           12 已加载游戏类别信息和游戏基本信息数据，无玩家基本信息数据
 *           28 三类基础数据都已加载
 *
 * 调用说明:
 */
int CreatList(CLASS_NODE **phead) {
    CLASS_NODE *hd = NULL, *pClassNode, tmp1;
    GAME_NODE *pGameNode, tmp2;
    PLAYER_NODE *pPlayerNode, tmp3;
    FILE *pFile;
    int find;
    int re = 0;

    if ((pFile = fopen(gp_class_info_filename, "rb")) == NULL) {
        printf("游戏类别数据文件打开失败!\n");
        return re;
    }
    printf("游戏类别数据文件打开成功!\n");

    /*从数据文件中读游戏类别信息数据，存入以后进先出方式建立的主链中*/
    while (fread(&tmp1, sizeof(CLASS_NODE), 1, pFile) == 1) {
        pClassNode = (CLASS_NODE *)malloc(sizeof(CLASS_NODE));
        *pClassNode = tmp1;
        pClassNode->gnext = NULL;
        pClassNode->next = hd;
        hd = pClassNode;
    }
    fclose(pFile);
    if (hd == NULL) {
        printf("游戏基本信息数据文件加载失败!\n");
        return re;
    }
    printf("游戏基本信息数据文件加载成功!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_game_info_filename, "rb")) == NULL) { /*注意文件名的改变*/
        printf("游戏基本信息数据文件打开失败!\n");
        return re;
    }
    printf("游戏基本信息数据文件打开成功!\n");
    re += 8;

    /*从数据文件中读取游戏基本信息数据，存入主链对应结点的游戏基本信息支链中*/
    while (fread(&tmp2, sizeof(GAME_NODE), 1, pFile) == 1) {
        /*创建结点，存放从数据文件中读出的游戏基本信息*/
        pGameNode = (GAME_NODE *)malloc(sizeof(GAME_NODE));
        *pGameNode = tmp2;
        pGameNode->pnext = NULL;

        /*在主链上查找该游戏所属游戏类别对应的主链结点*/
        pClassNode = hd;
        while (pClassNode != NULL
                && strcmp(pClassNode->class_id, pGameNode->class_id) != 0) {
            pClassNode = pClassNode->next;
        }
        if (pClassNode != NULL) { /*如果找到，则将结点以后进先出方式插入游戏基本信息支链*/
            pGameNode->next = pClassNode->gnext;
            pClassNode->gnext = pGameNode;
        } else { /*如果未找到，则释放所创建结点的内存空间*/
            free(pGameNode);
        }
    }
    fclose(pFile);
    if ((pFile = fopen(gp_player_info_filename, "rb")) == NULL) {
        printf("玩家基本信息数据文件打开失败!\n");
        return re;
    }
    printf("玩家基本信息数据文件打开成功!\n");
    re += 16;

    /*从数据文件中读取玩家基本信息数据，存入游戏基本信息支链对应结点的玩家基本信息支链中*/
    while (fread(&tmp3, sizeof(PLAYER_NODE), 1, pFile) == 1) {
        /*创建结点，存放从数据文件中读出的玩家基本信息*/
        pPlayerNode = (PLAYER_NODE *)malloc(sizeof(PLAYER_NODE));
        *pPlayerNode = tmp3;

        /*查找游戏基本信息支链上对应游戏基本信息结点*/
        pClassNode = hd;
        find = 0;
        while (pClassNode != NULL && find == 0) {
            pGameNode = pClassNode->gnext;
            while (pGameNode != NULL && find == 0) {
                if (strcmp(pGameNode->game_name, pPlayerNode->game_name) == 0) {
                    find = 1;
                    break;
                }
                pGameNode = pGameNode->next;
            }
            pClassNode = pClassNode->next;
        }
        if (find) { /*如果找到，则将结点以后进先出方式插入玩家基本信息支链中*/
            pPlayerNode->next = pGameNode->pnext;
            pGameNode->pnext = pPlayerNode;
        } else { /*如果未找到，则释放所创建结点的内存空间*/
            free(pPlayerNode);
        }
    }
    fclose(pFile);
    return re;
}

/**
 * 函数名称: InitInterface
 * 函数功能: 初始化界面.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InitInterface() {
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE;  /*黄色前景和蓝色背景*/
    SetConsoleTextAttribute(gh_std_out, att);  /*设置控制台屏幕缓冲区字符属性*/

    ClearScreen();  /* 清屏*/

    /*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*屏幕字符属性*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*弹出窗口的层号为0*/
    gp_top_layer->rcArea.Left = 0;  /*弹出窗口的区域为整个屏幕窗口*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*显示菜单栏*/
    ShowState();    /*显示状态栏*/

    return;
}

/**
 * 函数名称: ClearScreen
 * 函数功能: 清除屏幕信息.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ClearScreen(void) {
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*取屏幕缓冲区信息*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*计算屏幕缓冲区字符单元数*/

    /*将屏幕缓冲区所有单元的字符属性设置为当前屏幕缓冲区字符属性*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*将屏幕缓冲区所有单元填充为空格字符*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * 函数名称: ShowMenu
 * 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ShowMenu() {
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    COORD size;
    COORD pos = {0, 0};
    int i, j;
    int PosA = 2, PosB;
    char ch;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SetConsoleCursorPosition(gh_std_out, pos);
    for (i=0; i < 5; i++) { /*在窗口第一行第一列处输出主菜单项*/
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*隐藏光标*/

    /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0} ;

    /*将窗口第一行的内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*将这一行中英文字母置为红色，其他字符单元置为白底黑字*/
    for (i=0; i<size.X; i++) {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            (gp_buff_menubar_info+i)->Attributes = FOREGROUND_RED;
        }
    }

    /*修改后的菜单条字符信息回写到窗口的第一行*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*将光标位置设置在第2行第1列*/

    /*将菜单项置为热区，热区编号为菜单项号，热区类型为0(按钮型)*/
    i = 0;
    do {
        PosB = PosA + strlen(ga_main_menu[i]);  /*定位第i+1号菜单项的起止位置*/
        for (j=PosA; j<PosB; j++) {
            gp_scr_att[j] |= (i+1) << 2; /*设置菜单项所在字符单元的属性值*/
        }
        PosA = PosB + 4;
        i++;
    } while (i<5);

    TagMainMenu(gi_sel_menu);  /*在选中主菜单项上做标记，gi_sel_menu初值为1*/

    return;
}

/**
 * 函数名称: ShowState
 * 函数功能: 显示状态条.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 状态条字符属性为白底黑字, 初始状态无状态信息.
 */
void ShowState() {
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int i;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SMALL_RECT rcMenu = {0, bInfo.dwSize.Y-1, size.X-1, bInfo.dwSize.Y-1};

    if (gp_buff_stateBar_info == NULL) {
        gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
        ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
    }

    for (i=0; i<size.X; i++) {
        (gp_buff_stateBar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                                | BACKGROUND_RED;
    }

    WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: TagMainMenu
 * 函数功能: 在指定主菜单项上置选中标志.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagMainMenu(int num) {
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) { /*num为0时，将会去除主菜单项选中标记*/
        PosA = 0;
        PosB = 0;
    } else { /*否则，定位选中主菜单项的起止位置: PosA为起始位置, PosB为截止位置*/
        for (i=1; i<num; i++) {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;

    /*去除选中菜单项前面的菜单项选中标记*/
    for (i=0; i<PosA; i++) {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*在选中菜单项上做标记，黑底白字*/
    for (i=PosA; i<PosB; i++) {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*去除选中菜单项后面的菜单项选中标记*/
    for (i=PosB; i<bInfo.dwSize.X; i++) {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*将做好标记的菜单条信息写到窗口第一行*/
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0};        /*矩形字符信息快在窗口中的输出位置*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: CloseSys
 * 函数功能: 关闭系统.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void CloseSys(CLASS_NODE *hd) {
    CLASS_NODE *pClassNode1= hd, *pClassNode2;
    GAME_NODE *pGameNode1, *pGameNode2;
    PLAYER_NODE *pPlayerNode1, *pPlayerNode2;

    while (pClassNode1!= NULL) { /*释放十字交叉链表的动态存储区*/
        pClassNode2 = pClassNode1->next;
        pGameNode1 = pClassNode1->gnext;
        while (pGameNode1 != NULL) { /*释放学生基本信息支链的动态存储区*/
            pGameNode2 = pGameNode1->next;
            pPlayerNode1 = pGameNode1->pnext;
            while (pPlayerNode1 != NULL) { /*释放缴费信息支链的动态存储区*/
                pPlayerNode2 = pPlayerNode1->next;
                free(pPlayerNode1);
                pPlayerNode1 = pPlayerNode2;
            }
            free(pGameNode1);
            pGameNode1 = pGameNode2;
        }
        free(pClassNode1);  /*释放主链结点的动态存储区*/
        pClassNode1 = pClassNode2;
    }

    ClearScreen();        /*清屏*/

    /*释放存放菜单条、状态条信息动态存储区*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*关闭标准输入和输出设备句柄*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*将窗口标题栏置为运行结束*/
    SetConsoleTitle("运行结束");

    return;
}

/**
 * 函数名称: RunSys
 * 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void RunSys(CLASS_NODE **phead) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
    char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    while (bRet) {
        /*从控制台输入缓冲区中读一条记录*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) { /*如果记录由鼠标事件产生*/
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*获取鼠标坐标位置*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*取该位置的层号*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*取该字符单元属性*/
            if (cNo == 0) { /*层号为0，表明该位置未被弹出子菜单覆盖*/
                /* cAtt > 0 表明该位置处于热区(主菜单项字符单元)
                 * cAtt != gi_sel_menu 表明该位置的主菜单项未被选中
                 * gp_top_layer->LayerNo > 0 表明当前有子菜单弹出
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0) {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    PopMenu(cAtt);       /*弹出鼠标所在主菜单项对应的子菜单*/
                }
            } else if (cAtt > 0) { /*鼠标所在位置为弹出子菜单的菜单项字符单元*/
                TagSubMenu(cAtt); /*在该子菜单项上做选中标记*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                    == FROM_LEFT_1ST_BUTTON_PRESSED) { /*如果按下鼠标左边第一键*/
                if (cNo == 0) { /*层号为0，表明该位置未被弹出子菜单覆盖*/
                    if (cAtt > 0) { /*如果该位置处于热区(主菜单项字符单元)*/
                        PopMenu(cAtt);   /*弹出鼠标所在主菜单项对应的子菜单*/
                    }
                    /*如果该位置不属于主菜单项字符单元，且有子菜单弹出*/
                    else if (gp_top_layer->LayerNo > 0) {
                        PopOff();            /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    }
                } else { /*层号不为0，表明该位置被弹出子菜单覆盖*/
                    if (cAtt > 0) { /*如果该位置处于热区(子菜单项字符单元)*/
                        PopOff(); /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/

                        /*执行对应功能函数:gi_sel_menu主菜单项号,cAtt子菜单项号*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            } else if (inRec.Event.MouseEvent.dwButtonState
                       == RIGHTMOST_BUTTON_PRESSED) { /*如果按下鼠标右键*/
                if (cNo == 0) { /*层号为0，表明该位置未被弹出子菜单覆盖*/
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
            }
        } else if (inRec.EventType == KEY_EVENT /*如果记录由按键产生*/
                   && inRec.Event.KeyEvent.bKeyDown) { /*且键被按下*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

            /*系统快捷键的处理*/
            if (vkc == 112) { /*如果按下F1键*/
                if (gp_top_layer->LayerNo != 0) { /*如果当前有子菜单弹出*/
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
                bRet = ExeFunction(5, 1);  /*运行帮助主题功能函数*/
            } else if (inRec.Event.KeyEvent.dwControlKeyState
                       & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
                /*如果按下左或右Alt键*/
                switch (vkc) { /*判断组合键Alt+字母*/
                case 88:  /*Alt+X 退出*/
                    if (gp_top_layer->LayerNo != 0) {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    bRet = ExeFunction(1,4);
                    break;
                case 70:  /*Alt+F*/
                    PopMenu(1);
                    break;
                case 77: /*Alt+M*/
                    PopMenu(2);
                    break;
                case 81: /*Alt+Q*/
                    PopMenu(3);
                    break;
                case 83: /*Alt+S*/
                    PopMenu(4);
                    break;
                case 72: /*Alt+H*/
                    PopMenu(5);
                    break;
                }
            } else if (asc == 0) { /*其他控制键的处理*/
                if (gp_top_layer->LayerNo == 0) { /*如果未弹出子菜单*/
                    switch (vkc) { /*处理方向键(左、右、下)，不响应其他控制键*/
                    case 37:
                        gi_sel_menu--;
                        if (gi_sel_menu == 0) {
                            gi_sel_menu = 5;
                        }
                        TagMainMenu(gi_sel_menu);
                        break;
                    case 39:
                        gi_sel_menu++;
                        if (gi_sel_menu == 6) {
                            gi_sel_menu = 1;
                        }
                        TagMainMenu(gi_sel_menu);
                        break;
                    case 40:
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                } else { /*已弹出子菜单时*/
                    for (loc=0,i=1; i<gi_sel_menu; i++) {
                        loc += ga_sub_menu_count[i-1];
                    }  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                    switch (vkc) { /*方向键(左、右、上、下)的处理*/
                    case 37:
                        gi_sel_menu--;
                        if (gi_sel_menu < 1) {
                            gi_sel_menu = 5;
                        }
                        TagMainMenu(gi_sel_menu);
                        PopOff();
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    case 38:
                        num = gi_sel_sub_menu - 1;
                        if (num < 1) {
                            num = ga_sub_menu_count[gi_sel_menu-1];
                        }
                        if (strlen(ga_sub_menu[loc+num-1]) == 0) {
                            num--;
                        }
                        TagSubMenu(num);
                        break;
                    case 39:
                        gi_sel_menu++;
                        if (gi_sel_menu > 5) {
                            gi_sel_menu = 1;
                        }
                        TagMainMenu(gi_sel_menu);
                        PopOff();
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    case 40:
                        num = gi_sel_sub_menu + 1;
                        if (num > ga_sub_menu_count[gi_sel_menu-1]) {
                            num = 1;
                        }
                        if (strlen(ga_sub_menu[loc+num-1]) == 0) {
                            num++;
                        }
                        TagSubMenu(num);
                        break;
                    }
                }
            } else if ((asc-vkc == 0) || (asc-vkc == 32)) { /*按下普通键*/
                if (gp_top_layer->LayerNo == 0) { /*如果未弹出子菜单*/
                    switch (vkc) {
                    case 70: /*f或F*/
                        PopMenu(1);
                        break;
                    case 77: /*m或M*/
                        PopMenu(2);
                        break;
                    case 81: /*q或Q*/
                        PopMenu(3);
                        break;
                    case 83: /*s或S*/
                        PopMenu(4);
                        break;
                    case 72: /*h或H*/
                        PopMenu(5);
                        break;
                    case 13: /*回车*/
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                } else { /*已弹出子菜单时的键盘输入处理*/
                    if (vkc == 27) { /*如果按下ESC键*/
                        PopOff();
                        gi_sel_sub_menu = 0;
                    } else if(vkc == 13) { /*如果按下回车键*/
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    } else { /*其他普通键的处理*/
                        /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++) {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*依次与当前子菜单中每一项的代表字符进行比较*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++) {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1]) { //[C] 游戏分类信息，就是数组的第二个元素
                                /*如果匹配成功*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                bRet = ExeFunction(gi_sel_menu, i-loc+1);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * 函数名称: PopMenu
 * 函数功能: 弹出指定主菜单项对应的子菜单.
 * 输入参数: num 指定的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopMenu(int num) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu) {     /*如果指定主菜单不是已选中菜单*/
        if (gp_top_layer->LayerNo != 0) { /*如果此前已有子菜单弹出*/
            PopOff();
            gi_sel_sub_menu = 0;
        }
    } else if (gp_top_layer->LayerNo != 0) { /*若已弹出该子菜单，则返回*/
        return;
    }

    gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
    TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
    LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
    for (i=1; i<gi_sel_menu; i++) {
        loc += ga_sub_menu_count[i-1];
    }
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel = ga_sub_menu + loc;   /*标签束第一个标签字符串的地址*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*标签束中标签字符串的个数*/
    COORD aLoc[labels.num];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    for (i=0; i<labels.num; i++) { /*确定标签字符串的输出位置，存放在坐标数组中*/
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
    SMALL_RECT aArea[areas.num];                    /*定义数组存放所有热区位置*/
    char aSort[areas.num];                      /*定义数组存放所有热区对应类别*/
    char aTag[areas.num];                         /*定义数组存放每个热区的编号*/
    for (i=0; i<areas.num; i++) {
        aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*热区类别都为0(按钮型)*/
        aTag[i] = i + 1;           /*热区按顺序编号*/
    }
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/
    pos.X = rcPop.Left + 2; /*+2表示热区范围*/
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++) {
        /*此循环用来在空串子菜项位置画线形成分隔，并取消此菜单项的热区属性*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) { /*串长为0，表明为空串*/
            /*首先画横线*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++) {
                /*取消该区域字符单元的热区属性*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*按位与的结果保留了低两位*/
            }
        }

    }
    /*将子菜单项的功能键设为白底红字*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++) {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0) {
            continue;  /*跳过空串*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

/**
 * 函数名称: PopUp
 * 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
 * 输入参数: pRc 弹出窗口位置数据存放的地址
 *           att 弹出窗口区域字符属性
 *           pLabel 弹出窗口中标签束信息存放的地址
             pHotArea 弹出窗口中热区信息存放的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea) {
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*弹出窗口所在位置字符单元信息入栈*/
    size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
    /*申请存放弹出窗口相关信息的动态存储区*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++) {
        /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
        for (j=pRc->Left; j<=pRc->Right; j++) {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
    /*设置弹出窗口区域字符的新属性*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++) {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*将标签束中的标签字符串在设定的位置输出*/
    for (i=0; i<pLabel->num; i++) {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0) {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }
    /*设置弹出窗口区域字符单元的新属性*/
    for (i=pRc->Top; i<=pRc->Bottom; i++) {
        /*此二重循环设置字符单元的层号*/
        for (j=pRc->Left; j<=pRc->Right; j++) {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++) {
        /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++) {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                         | (pHotArea->pTag[i] << 2);
        }
    }
    return;
}

/**
 * 函数名称: PopOff
 * 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopOff(void) {
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL)) {
        /*栈底存放的主界面屏幕信息，不用关闭*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*恢复弹出窗口区域原外观*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*恢复字符单元原属性*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++) {
        //Sleep(1);
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++) {
            //Sleep(1);
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*释放动态存储区*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * 函数名称: DrawBox
 * 函数功能: 在指定区域画边框.
 * 输入参数: pRc 存放区域位置信息的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void DrawBox(SMALL_RECT *pRc) {
    char chBox[] = {'+','-','|'};  /*画框用的字符*/
    COORD pos = {pRc->Left, pRc->Top};  /*定位在区域的左上角*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左上角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++) {
        /*此循环画上边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右上角*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++) {
        /*此循环画边框左边线和右边线*/
        //Sleep(10);
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左下角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++) {
        /*画下边框横线*/
        //Sleep(10);
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右下角*/
    return;
}

/**v
 * 函数名称: TagSubMenu
 * 函数功能: 在指定子菜单项上做选中标记.
 * 输入参数: num 选中的子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagSubMenu(int num) {
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1)) {
        /*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) { /*首先取消原选中子菜单项上的标记*/
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*白底红字*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*在制定子菜单项上做选中标记*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
    return;
}

/**
 * 函数名称: LocSubMenu
 * 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: rc 存放区域位置信息的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void LocSubMenu(int num, SMALL_RECT *rc) {
    int i, len, loc = 0;

    rc->Top = 1; /*区域的上边定在第2行，行号为1*/
    rc->Left = 1;
    for (i=1; i<num; i++) {
        /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++) {
        /*查找最长子菜单字符串，将其长度存放在rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len) {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*计算区域的右边界*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*计算区域下边的行号*/
    if (rc->Right >= SCR_COL) { /*右边界越界的处理*/
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * 函数名称: DealInput
 * 函数功能: 在弹出窗口区域设置热区, 等待并响应用户输入.
 * 输入参数: pHotArea
 *           piHot 焦点热区编号的存放地址, 即指向焦点热区编号的指针
 * 输出参数: piHot 用鼠标单击、按回车或空格时返回当前热区编号
 * 返 回 值:
 *
 * 调用说明:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
                (inRec.Event.MouseEvent.dwButtonState   /*如果记录由鼠标事件产生*/
                 == FROM_LEFT_1ST_BUTTON_PRESSED)) {
            pos = inRec.Event.MouseEvent.dwMousePosition;       /*鼠标位置*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;      /*取该位置层号*/
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0) {
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0) {
                    iRet = 13;
                    break;
                }
            }
        } else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;/*vkc:虚拟键代码, asc:字符的ASCII码值*/

            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                arrow = 0;
                switch (vkc) {
                /*方向键(左、上、右、下)的处理*/
                case 37:
                    arrow = 1;
                    break;
                case 38:
                    arrow = 2;
                    break;
                case 39:
                    arrow = 3;
                    break;
                case 40:
                    arrow = 4;
                    break;
                }
                if (arrow > 0) {
                    num = *piHot;
                    while (TRUE) {
                        if (arrow < 3) {
                            num--;
                        } else {
                            num++;
                        }
                        if ((num < 1) || (num > pHotArea->num) ||
                                ((arrow % 2) && (pHotArea->pArea[num-1].Top
                                                 == pHotArea->pArea[*piHot-1].Top)) || ((!(arrow % 2))
                                                         && (pHotArea->pArea[num-1].Top
                                                             != pHotArea->pArea[*piHot-1].Top))) {
                            break;
                        }
                    }
                    if (num > 0 && num <= pHotArea->num) {
                        *piHot = num;
                        SetHotPoint(pHotArea, *piHot);
                    }
                }
            } else if (vkc == 27) {
                /*ESC键*/
                iRet = 27;
                break;
            } else if (vkc == 13 || vkc == 32) {
                /*回车键或空格表示按下当前按钮*/
                iRet = 13;
                break;
            }
        }
    }
    return iRet;
}
/**把ihot对应的热区选中*/
void SetHotPoint(HOT_AREA *pHotArea, int iHot) {
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att,att1, att2;
    int i, width;

    att=BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;  /*白底黑字，文本框*/
    att1 =  FOREGROUND_BLUE | FOREGROUND_GREEN|FOREGROUND_RED ;  /*黑底白字，按钮被选中*/
    att2 =BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*青底黑字，弹出框的颜色*/
    for (i=0; i<pHotArea->num; i++) {
        /*将按钮类热区置为青底黑字*//*先循环设置所有都没有被选中*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0) {
            /*热区是按钮类*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
            if(pHotArea->pArea[i].Bottom!=pHotArea->pArea[i].Top) { /*如果是两行的热区*/
                /*如果是两行的热区*/
                pos.Y = pHotArea->pArea[i].Top+1;
                FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
            }
        }
    }
    /*将ihot对应的热区选中*/
    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0) {
        /*被激活热区是按钮类*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
        if(pHotArea->pArea[iHot-1].Bottom!=pHotArea->pArea[iHot-1].Top) {
            /*如果是两行的热区*/
            pos.Y = pHotArea->pArea[iHot-1].Top+1;
            FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
        }
    } else if (pHotArea->pSort[iHot-1] == 1) {
        /*被激活热区是文本框类*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * 函数名称: ExeFunction
 * 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
 * 输入参数: m 主菜单项号
 *           s 子菜单项号
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE 或 FALSE
 *
 * 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
 */
BOOL ExeFunction(int m, int s) {
    BOOL bRet = TRUE;
    /*函数指针数组，用来存放所有功能函数的入口地址*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*将功能函数入口地址存入与功能函数所在主菜单号和子菜单号对应下标的数组元素*/
    pFunction[0] = SaveData;
    pFunction[1] = BackupData;
    pFunction[2] = RestoreData;
    pFunction[3] = ExitSys;

    pFunction[4] = MaintainClassInfo;
    pFunction[5] = MaintainGameInfo;
    pFunction[6] = MaintainPlayerInfo;

    pFunction[7] = QueryClassInfo;
    pFunction[8] = QueryGameInfo;
    pFunction[9] = QueryPlayerInfo;

    pFunction[10] = StatGame;
    pFunction[11] = StatRank;
    pFunction[12] = StatClass;
    pFunction[13] = StatPlayer;
    pFunction[14] = StatCharge;

    pFunction[15] = HelpTopic;
    pFunction[16] = NULL;
    pFunction[17] = AboutGame;

    for (i=1,loc=0; i<m; i++) { /*根据主菜单号和子菜单号计算对应下标*/
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL) {
        bRet = (*pFunction[loc])();  /*用函数指针调用所指向的功能函数*/
    }

    return bRet;
}

BOOL SaveData(void) {
    char* pString[2]= {"确认保存？",
                       "确定  取消"
                      };
    if(ShowModuleYON(pString)==1) {
        PopOff();
        SaveSysData(gp_head);
        char *plabel_name[] = {"保存成功",
                               "确认"
                              };
        ShowModule(plabel_name, 2);
    } else {
        PopOff();
    }
    return TRUE;
}

BOOL BackupData(void) {
    char* pString[2]= {"确认备份？",
                       "确定  取消"
                      };
    if(ShowModuleYON(pString)==1) {
        PopOff();
        time_t rawtime;
        struct tm* timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        char string[30];
        strncpy(string,asctime(timeinfo)+4,6);
        strcpy(string+6,".dat");
        BackupSysData(gp_head,string);
        char *plabel_name[] = {"数据备份至",
                               string,
                               "确认"
                              };
        ShowModule(plabel_name, 3);
    } else {
        PopOff();
    }
    return TRUE;
}

BOOL RestoreData(void) {
    VRestoreData();
    return TRUE;
}

BOOL ExitSys(void) {
    char* pString[2]= {"确认退出？",
                       "确定  取消"
                      };
    if(ShowModuleYON(pString)==1) {
        PopOff();
        return FALSE;/*表示退出*/
    } else {
        PopOff();
        return TRUE;
    }
}

/**数据恢复弹出框*/
void VRestoreData() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"请输入备份的数据文件名","如：Aug 27.dat",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+20, rcPop.Top + 3
        },
        {
            rcPop.Left +6, rcPop.Top +5,
            rcPop.Left+9, rcPop.Top + 5
        },
        {
            rcPop.Left + 17, rcPop.Top +5,
            rcPop.Left+20, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    char filename[11];
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    if(m==0) {
                        char* plabel_name[2]= {"请输入文件名！",
                                               "确定"
                                              };
                        ShowModule(plabel_name, 2);
                        break;
                    }
                    int handle;
                    if((handle=open(filename,O_RDONLY|O_BINARY))==-1) {
                        PopOff();
                        char* plabel_name[2]= {"文件不存在~",
                                               "确定"
                                              };
                        ShowModule(plabel_name, 2);
                        break;
                    }

                    PopOff();
                    char* pString[2]= {"确认恢复？",
                                       "确定  取消"
                                      };
                    if(ShowModuleYON(pString)==1) {
                        PopOff();
                        RestoreSysData(&gp_head,filename);
                        char *plabel_name[] = {"恢复成功",
                                               "确认"
                                              };
                        ShowModule(plabel_name, 2);
                        break;
                    } else {
                        PopOff();
                        break;
                    }
                } else if(iHot==3) {
                    PopOff();
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m--;
                    filename[m]='\0';
                } else {
                    continue;
                }
            } else {
                if(m==10)
                    continue;
                WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                pos1.X++;
                SetConsoleCursorPosition(gh_std_out,pos1);
                filename[m++]=asc;
                filename[m]='\0';
                continue;
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**数据维护*/
/**维护游戏类别信息*/
BOOL MaintainClassInfo(void) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"游戏类别信息录入","游戏类别信息删除","游戏类别信息修改",
                   "取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;                    /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc;                     /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 2, rcPop.Top + 1,
            rcPop.Right-2, rcPop.Top + 1
        },
        {
            rcPop.Left + 2, rcPop.Top +2,
            rcPop.Right-2, rcPop.Top + 2
        },
        {
            rcPop.Left + 2, rcPop.Top +3,
            rcPop.Right-2, rcPop.Top + 3
        },
        {
            rcPop.Left + 8, rcPop.Top + 5,
            rcPop.Right -8, rcPop.Top + 5
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/

    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VInsertClassNode();
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VDelClassNode();
        PopOff();
    } else if(isEnter==13&&iHot==3) {
        PopOff();
        VModifClassNode();
        PopOff();
    } else {
        PopOff();
    }
    return TRUE;
}

/**录入游戏类别信息弹出框*/
void VInsertClassNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"游戏类别信息录入","类别编码","类别描述",
                   " 确定       取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 4;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top + 3,
            rcPop.Left+14, rcPop.Top + 3
        },
        {
            rcPop.Left + 10, rcPop.Top +4,
            rcPop.Left+25, rcPop.Top + 4
        },
        {
            rcPop.Left + 3, rcPop.Top +6,
            rcPop.Left+6, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+17, rcPop.Top + 6
        }
    };
    char aSort[] = {1, 1,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    pos.X=rcPop.Left+10;
    pos.Y=rcPop.Top+4;/*类别描述信息所在行*/
    SetHotPoint(&areas, iHot);
    CLASS_NODE *pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top + 4};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==3) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标bu可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pclass_node->next=NULL;
                    pclass_node->gnext=NULL;
                    if(m==0||n==0) {
                        char *q[]= {"数据不能为空!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    if(SeekClassNodeById(gp_head,pclass_node->class_id)!=NULL) {
                        char* p[]= {"类别已存在~","修改  取消"};
                        if(ShowModuleYON(p,2)) {
                            PopOff();
                            VVModifClassNode(pclass_node);
                            break;
                        } else
                            break;
                    }
                    if(InsertClassNode(&gp_head,pclass_node)) {
                        char* p[]= {"插入成功","确定"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"插入失败!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m--;
                    pclass_node->class_id[m]='\0';
                } else if(iHot==2) {
                    if(pos2.X==rcPop.Left + 10)
                        continue;
                    if((pclass_node->class_dec[n-1]&0x80)==128) {
                        pos2.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n-=2;
                        pclass_node->class_dec[n]='\0';
                    } else {
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n--;
                        pclass_node->class_dec[n]='\0';
                    }
                }
            } else if(iHot==1) {
                if(m==3)
                    continue;
                WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                pos1.X++;
                SetConsoleCursorPosition(gh_std_out,pos1);
                pclass_node->class_id[m++]=asc;
                pclass_node->class_id[m]='\0';
            } else if(iHot==2) {
                if(n==14)
                    continue;
                if((asc&0x80)==128) { /*如果是中文字符*/
                    ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                    ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                    SetConsoleCursorPosition(gh_std_out,pos2);
                    putchar(asc);
                    putchar(ascc);
                    FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                    pos2.X+=2;
                    SetConsoleCursorPosition(gh_std_out,pos2);
                    pclass_node->class_dec[n++]=asc;
                    pclass_node->class_dec[n++]=ascc;
                    pclass_node->class_dec[n]='\0';
                } else {
                    if(n==14)
                        continue;
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                    pos2.X++;
                    SetConsoleCursorPosition(gh_std_out,pos2);
                    pclass_node->class_dec[n++]=asc;
                    pclass_node->class_dec[n]='\0';
                }
            } else {
                continue;
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**删除游戏类别信息弹出框*/
void VDelClassNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"请输入游戏类别编码","类别编码",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+8;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+14, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    class_node=SeekClassNodeById(gp_head,pclass_node->class_id);
                    if(class_node!=NULL) {
                        char *plabel_name[2]= {"确认删除？","确定  取消"};
                        if(ShowModuleYON(plabel_name)) {
                            DelClassNode(&gp_head,pclass_node->class_id);
                            PopOff();
                            char *p[2]= {"删除成功","确定"};
                            ShowModule(p,2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该类别","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m--;
                    pclass_node->class_id[m]='\0';
                } else {
                    continue;
                }
            } else {
                if(m==3)
                    continue;
                WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                pos1.X++;
                SetConsoleCursorPosition(gh_std_out,pos1);
                pclass_node->class_id[m++]=asc;
                pclass_node->class_id[m]='\0';
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**修改游戏类别信息弹出框*/
void VModifClassNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"请输入游戏类别编码","类别编码",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+8;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+14, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    class_node=SeekClassNodeById(gp_head,pclass_node->class_id);
                    if(class_node!=NULL) {
                        VVModifClassNode(class_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该类别","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m--;
                    pclass_node->class_id[m]='\0';
                } else {
                    continue;
                }
            } else {
                if(m==3)
                    continue;
                WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                pos1.X++;
                SetConsoleCursorPosition(gh_std_out,pos1);
                pclass_node->class_id[m++]=asc;
                pclass_node->class_id[m]='\0';
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**修改游戏类别信息改动内容弹出框*/
void VVModifClassNode(CLASS_NODE *class_node) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"修改游戏类别信息","类别编码","类别描述",
                   " 确定       取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +4,
            rcPop.Left+25, rcPop.Top + 4
        },
        {
            rcPop.Left + 3, rcPop.Top +6,
            rcPop.Left+6, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+17, rcPop.Top + 6
        }
    };
    char aSort[] = { 1,0,0};
    char aTag[] = {1,2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);
    CLASS_NODE *pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    *pclass_node=*class_node;

    int m=strlen(class_node->class_id),n=strlen(class_node->class_dec);
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top + 4};
    WriteConsoleOutputCharacter(gh_std_out,class_node->class_id,m,pos1, &ul);
    WriteConsoleOutputCharacter(gh_std_out, class_node->class_dec,n,pos2, &ul);
    pos2.X=pos2.X+n;
    SetConsoleCursorPosition(gh_std_out,pos2);
    pos.X=rcPop.Left+10;
    pos.Y=rcPop.Top+4;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标bu可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    if(n==0) {
                        char *q[]= {"数据不能为空!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    if(ModifClassNode(&gp_head,pclass_node->class_id,pclass_node)) {
                        char* p[]= {"修改成功","确定"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"修改失败!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos2.X==rcPop.Left + 10)
                        continue;
                    if((pclass_node->class_dec[n-1]&0x80)==128) {
                        pos2.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n-=2;
                        pclass_node->class_dec[n]='\0';
                    } else {
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n--;
                        pclass_node->class_dec[n]='\0';
                    }
                }
            } else if(iHot==1) {
                if(n==14)
                    continue;
                if((asc&0x80)==128) { /*如果是中文字符*/
                    ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                    ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                    SetConsoleCursorPosition(gh_std_out,pos2);
                    putchar(asc);
                    putchar(ascc);
                    FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                    pos2.X+=2;
                    SetConsoleCursorPosition(gh_std_out,pos2);
                    pclass_node->class_dec[n++]=asc;
                    pclass_node->class_dec[n++]=ascc;
                    pclass_node->class_dec[n]='\0';
                } else {
                    if(n==14)
                        continue;
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                    pos2.X++;
                    SetConsoleCursorPosition(gh_std_out,pos2);
                    pclass_node->class_dec[n++]=asc;
                    pclass_node->class_dec[n]='\0';
                }
            } else {
                continue;
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**维护游戏基本信息*/
BOOL MaintainGameInfo(void) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"游戏基本信息录入","游戏基本信息删除","游戏基本信息修改",
                   "取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;                    /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc;                     /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 2, rcPop.Top + 1,
            rcPop.Right-2, rcPop.Top + 1
        },
        {
            rcPop.Left + 2, rcPop.Top +2,
            rcPop.Right-2, rcPop.Top + 2
        },
        {
            rcPop.Left + 2, rcPop.Top +3,
            rcPop.Right-2, rcPop.Top + 3
        },
        {
            rcPop.Left + 8, rcPop.Top + 5,
            rcPop.Right -8, rcPop.Top + 5
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/

    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VInsertGameNode();
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VDelGameNode();
        PopOff();
    } else if(isEnter==13&&iHot==3) {
        PopOff();
        VModifGameNode();
        PopOff();
    } else {
        PopOff();
    }
    return TRUE;
}

/**录入游戏基本信息弹出框*/
void VInsertGameNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"游戏基本信息录入","游戏类别","游戏名称","游戏网址","收费模式",
                   "排名方式"," 确定       取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+30;
    pos.Y = 11;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 7; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+9}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 7;       /*热区的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top + 3,
            rcPop.Left+14, rcPop.Top + 3
        },
        {
            rcPop.Left + 10, rcPop.Top +4,
            rcPop.Left+25, rcPop.Top + 4
        },
        {
            rcPop.Left + 10, rcPop.Top +5,
            rcPop.Left+40, rcPop.Top + 5
        },
        {
            rcPop.Left + 10, rcPop.Top +6,
            rcPop.Left+11, rcPop.Top + 6
        },
        {
            rcPop.Left + 10, rcPop.Top +7,
            rcPop.Left+11, rcPop.Top + 7
        },
        {
            rcPop.Left + 3, rcPop.Top +9,
            rcPop.Left+6, rcPop.Top + 9
        },
        {
            rcPop.Left + 14, rcPop.Top +9,
            rcPop.Left+17, rcPop.Top + 9
        }
    };
    char aSort[] = {1,1,1,1,1,0,0};
    char aTag[] = {1,2,3,4,5,6,7};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 8;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    GAME_NODE *pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m1=0,m2=0,m3=0,m4=0,m5=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top + 4},
          pos3= {rcPop.Left + 10, rcPop.Top +5},pos4= {rcPop.Left + 10, rcPop.Top +6},
          pos5= {rcPop.Left + 10, rcPop.Top +7};
    pos.X=pos2.X;
    pos.Y=pos2.Y;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==6) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标bu可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pgame_node->next=NULL;
                    pgame_node->pnext=NULL;
                    int icharge_model=0,irank_way=0;
                    icharge_model=pgame_node->charge_model[0]=='c'||pgame_node->charge_model[0]=='p'
                                  ||pgame_node->charge_model[0]=='t';
                    irank_way=pgame_node->rank_way[0]=='p'||pgame_node->rank_way[0]=='t';
                    if(m1==0||m2==0||m3==0||icharge_model==0||irank_way==0) {
                        char *q[]= {"数据不正确!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    if(SeekGameNode(gp_head,pgame_node->game_name)!=NULL) {
                        char* p[]= {"游戏已存在~","修改  取消"};
                        if(ShowModuleYON(p,2)) {
                            PopOff();
                            VVModifGameNode(pgame_node);
                            break;
                        } else
                            break;
                    }
                    if(InsertGameNode(gp_head,pgame_node)) {
                        char* p[]= {"插入成功","确定"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"找不到游戏类别!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==7) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos1.X==rcPop.Left + 10)
                        break;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m1--;
                    pgame_node->class_id[m1]='\0';
                    break;
                case 2:
                    if(pos2.X==rcPop.Left + 10)
                        break;
                    if((pgame_node->game_name[m2-1]&0x80)==128) {
                        pos2.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        m2-=2;
                        pgame_node->game_name[m2]='\0';
                    } else {
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        m2--;
                        pgame_node->game_name[m2]='\0';
                    }
                    break;
                case 3:
                    if(pos3.X==rcPop.Left + 10)
                        break;
                    pos3.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos3, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos3);
                    m3--;
                    pgame_node->game_adress[m3]='\0';
                    break;
                case 4:
                    if(pos4.X==rcPop.Left + 10)
                        break;
                    pos4.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos4, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos4);
                    break;
                case 5:
                    if(pos5.X==rcPop.Left + 10)
                        break;
                    pos5.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos5, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos5);
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m1==3)
                        continue;
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                    pos1.X++;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->class_id[m1++]=asc;
                    pgame_node->class_id[m1]='\0';
                    break;
                case 2:
                    if(m2==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                        pos2.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pgame_node->game_name[m2++]=asc;
                        pgame_node->game_name[m2++]=ascc;
                        pgame_node->game_name[m2]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                        pos2.X++;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pgame_node->game_name[m2++]=asc;
                        pgame_node->game_name[m2]='\0';
                    }
                    break;
                case 3:
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos3, &ul);
                    pos3.X++;
                    SetConsoleCursorPosition(gh_std_out,pos3);
                    pgame_node->game_adress[m3++]=asc;
                    pgame_node->game_adress[m3]='\0';
                    break;
                case 4:
                    if(asc=='p'||asc=='t'||asc=='c') {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos4, &ul);
                        pgame_node->charge_model[0]=asc;
                        pgame_node->charge_model[1]='\0';
                    }
                    break;
                case 5:
                    if(asc=='p'||asc=='t') {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos5, &ul);
                        pgame_node->rank_way[0]=asc;
                        pgame_node->rank_way[1]='\0';
                    }
                    break;
                default:
                    break;
                }
            }

        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**删除游戏基本信息弹出框*/
void VDelGameNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"删除游戏基本信息","游戏名称",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1=BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNode(gp_head,pgame_node->game_name);
                    if(game_node!=NULL) {
                        char *plabel_name[2]= {"确认删除？","确定  取消"};
                        if(ShowModuleYON(plabel_name)) {
                            DelGameNode(gp_head,pgame_node->game_name);
                            PopOff();
                            char *p[2]= {"删除成功","确定"};
                            ShowModule(p,2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该游戏","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pgame_node->game_name[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pgame_node->game_name[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pgame_node->game_name[m]='\0';
                    }
                } else {
                    continue;
                }
            } else {
                if(m==14)
                    continue;
                if((asc&0x80)==128) { /*如果是中文字符*/
                    ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                    ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    putchar(asc);
                    putchar(ascc);
                    FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                    pos1.X+=2;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m++]=ascc;
                    pgame_node->game_name[m]='\0';
                } else {
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                    pos1.X++;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m]='\0';
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**修改游戏基本信息弹出框*/
void VModifGameNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"修改游戏基本信息","游戏名称",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1=BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNode(gp_head,pgame_node->game_name);
                    if(game_node!=NULL) {
                        VVModifGameNode(game_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该游戏","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pgame_node->game_name[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pgame_node->game_name[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pgame_node->game_name[m]='\0';
                    }
                } else {
                    continue;
                }
            } else {
                if(m==14)
                    continue;
                if((asc&0x80)==128) { /*如果是中文字符*/
                    ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                    ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    putchar(asc);
                    putchar(ascc);
                    FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                    pos1.X+=2;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m++]=ascc;
                    pgame_node->game_name[m]='\0';
                } else {
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                    pos1.X++;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m]='\0';
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**修改游戏基本信息改动内容弹出框*/
void VVModifGameNode(GAME_NODE* game_node) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    char *pCh[] = {"游戏基本信息修改","游戏类别","游戏名称","游戏网址","收费模式",
                   "排名方式"," 确定       取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+30;
    pos.Y = 11;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 7; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+9}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 5;       /*热区的个数*/
    SMALL_RECT aArea[] = {
        {
            rcPop.Left + 10, rcPop.Top +5,
            rcPop.Left+40, rcPop.Top + 5
        },
        {
            rcPop.Left + 10, rcPop.Top +6,
            rcPop.Left+11, rcPop.Top + 6
        },
        {
            rcPop.Left + 10, rcPop.Top +7,
            rcPop.Left+11, rcPop.Top + 7
        },
        {
            rcPop.Left + 3, rcPop.Top +9,
            rcPop.Left+6, rcPop.Top + 9
        },
        {
            rcPop.Left + 14, rcPop.Top +9,
            rcPop.Left+17, rcPop.Top + 9
        }
    };
    char aSort[] = {1,1,1,0,0};
    char aTag[] = {1,2,3,4,5};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 8;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    GAME_NODE *pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    *pgame_node=*game_node;
    int m1=strlen(game_node->class_id),m2=strlen(game_node->game_name),m3=strlen(game_node->game_adress);
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top + 4},
          pos3= {rcPop.Left + 10, rcPop.Top +5},pos4= {rcPop.Left + 10, rcPop.Top +6},
          pos5= {rcPop.Left + 10, rcPop.Top +7};
    WriteConsoleOutputCharacter(gh_std_out,game_node->class_id,m1,pos1, &ul);
    WriteConsoleOutputCharacter(gh_std_out,game_node->game_name,m2,pos2, &ul);
    WriteConsoleOutputCharacter(gh_std_out,game_node->game_adress,m3,pos3, &ul);
    pos3.X=pos3.X+m3;
    WriteConsoleOutputCharacter(gh_std_out,game_node->charge_model,1,pos4, &ul);
    WriteConsoleOutputCharacter(gh_std_out,game_node->rank_way,1,pos5, &ul);
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==4) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标bu可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();

                    int icharge_model=0,irank_way=0;
                    icharge_model=pgame_node->charge_model[0]=='c'||pgame_node->charge_model[0]=='p'
                                  ||pgame_node->charge_model[0]=='t';
                    irank_way=pgame_node->rank_way[0]=='p'||pgame_node->rank_way[0]=='t';
                    if(m3==0||icharge_model==0||irank_way==0) {
                        char *q[]= {"数据不正确!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    if(ModifGameNode(gp_head,pgame_node->game_name,pgame_node)) {
                        char* p[]= {"修改成功","确定"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"修改失败!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==5) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:/*热区为编号为1，但是对应的字符串是第三个*/
                    if(pos3.X==rcPop.Left + 10)
                        continue;
                    pos3.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos3, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos3);
                    m3--;
                    pgame_node->game_adress[m3]='\0';
                    break;
                case 2:
                    if(pos4.X==rcPop.Left + 10)
                        continue;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos4, &ul);
                    break;
                case 3:
                    if(pos5.X==rcPop.Left + 10)
                        continue;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos5, &ul);
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m3==29)
                        continue;
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos3, &ul);
                    pos3.X++;
                    SetConsoleCursorPosition(gh_std_out,pos3);
                    pgame_node->game_adress[m3++]=asc;
                    pgame_node->game_adress[m3]='\0';
                    break;
                case 2:
                    if(asc=='c'||asc=='p'||asc=='t') {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos4, &ul);
                        pgame_node->charge_model[0]=asc;
                    }
                    break;
                case 3:
                    if(asc=='t'||asc=='p') {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos5, &ul);
                        pgame_node->rank_way[0]=asc;
                    }
                    break;
                default:
                    break;
                }
            }

        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**维护玩家信息*/
BOOL MaintainPlayerInfo(void) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"玩家基本信息录入","玩家基本信息删除","玩家基本信息修改",
                   "取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;                    /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc;                     /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 2, rcPop.Top + 1,
            rcPop.Right-2, rcPop.Top + 1
        },
        {
            rcPop.Left + 2, rcPop.Top +2,
            rcPop.Right-2, rcPop.Top +2
        },
        {
            rcPop.Left + 2, rcPop.Top +3,
            rcPop.Right-2, rcPop.Top + 3
        },
        {
            rcPop.Left + 8, rcPop.Top + 5,
            rcPop.Right -8, rcPop.Top + 5
        }
    };
    char aSort[] = {0, 0,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/

    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VInsertPlayerNode();
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VDelPlayerNode();
        PopOff();
    } else if(isEnter==13&&iHot==3) {
        PopOff();
        VModifPlayerNode();
        PopOff();
    } else {
        PopOff();
    }
    return TRUE;
}

/**录入玩家基本信息弹出框*/
void VInsertPlayerNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0},pos_user,pos_game;
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"玩家基本信息录入","用户名","游戏名称","注册日期","游戏累计时长",
                   "消费总金额","排名指标"," 确定       取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+25;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 8; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+8},
        {rcPop.Left+2, rcPop.Top+10}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 8;       /*热区的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 14, rcPop.Top + 3,
            rcPop.Left+29, rcPop.Top + 3
        },
        {
            rcPop.Left + 14, rcPop.Top +4,
            rcPop.Left+29, rcPop.Top + 4
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+24, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+29, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +7,
            rcPop.Left+29, rcPop.Top + 7
        },
        {
            rcPop.Left + 14, rcPop.Top +8,
            rcPop.Left+29, rcPop.Top + 8
        },
        {
            rcPop.Left + 3, rcPop.Top +10,
            rcPop.Left+6, rcPop.Top + 10
        },
        {
            rcPop.Left + 14, rcPop.Top +10,
            rcPop.Left+17, rcPop.Top + 10
        }
    };
    char aSort[] = {1,1,1,1,1,1,0,0};
    char aTag[] = {1,2,3,4,5,6,7,8};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 9;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE *pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    int m1=0,m2=0,m3=0,m4=0,m5=0,m6=0;
    COORD pos1= {rcPop.Left + 14, rcPop.Top +3},pos2= {rcPop.Left + 14, rcPop.Top + 4},
          pos3= {rcPop.Left + 14, rcPop.Top +5},pos4= {rcPop.Left + 14, rcPop.Top +6},
          pos5= {rcPop.Left + 14, rcPop.Top +7},pos6= {rcPop.Left + 14, rcPop.Top +8};
    pos_user.X=pos1.X;
    pos_user.Y=pos1.Y;
    pos_game.X=pos2.X;
    pos_game.Y=pos2.Y;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==7) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标bu可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pplayer_node->next=NULL;
                    if(m1==0||m2==0||m3==0||m4==0||m5==0||m6==0) {
                        char *q[]= {"数据不能为空!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    if(m3!=8) {
                        char *plabel_name[2]= {"注册日期不合法！","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                    pplayer_node->enroll_date=atol(pplayer_node->enroll_datec);
                    //puts(pplayer_node->enroll_datec);
                    int iyear,imonth,iday;
                    char year[5],month[3],day[3];
                    strncpy(year,pplayer_node->enroll_datec,4);
                    strncpy(month,pplayer_node->enroll_datec+4,2);
                    //month[2]='\0';
                    strncpy(day,pplayer_node->enroll_datec+6,2);
                    //day[2]='\0';
                    iyear=atoi(year);
                    imonth=atoi(month);
                    iday=atoi(day);
                    //printf("%d\n%d\n%d",iyear,imonth,iday);
                    if(iyear<1970||iyear>2999||imonth<1||imonth>12)
                    {
                        char *q[]= {"找不到该年份或月份!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    int febdays;
                    int dayfault=0;
                    switch(imonth)
                    {
                        case 1:
                        case 3:
                        case 5:
                        case 7:
                        case 8:
                        case 10:
                        case 12:
                            if(iday<1||iday>31)
                                dayfault=1;
                            break;
                        case 4:
                        case 6:
                        case 9:
                        case 11:
                            if(iday<1||iday>30)
                                dayfault=1;
                            break;
                        case 2:
                            if(iyear%400==0||(iyear%4==0&&iyear%100!=0))
                                febdays=29;
                            else
                                febdays=28;
                            if(iday<1||iday>febdays)
                                dayfault=1;
                            break;
                    }
                    if(dayfault==1)
                    {
                        char *q[]= {"该月天数错误!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    if(SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name)!=NULL) {
                        char* p[]= {"玩家已存在~","修改  取消"};
                        if(ShowModuleYON(p,2)) {
                            PopOff();
                            VVModifPlayerNode(pplayer_node);
                            break;
                        } else
                            break;
                    }
                    pplayer_node->total_time=atol(pplayer_node->total_timec);
                    pplayer_node->total_money=atof(pplayer_node->total_moneyc);
                    GAME_NODE* game_node=SeekGameNode(gp_head,pplayer_node->game_name);
                    if(game_node!=NULL) {
                        if(game_node->rank_way[0]=='p') {
                            pplayer_node->achievement.p=atof(pplayer_node->achievementc);
                        } else if(game_node->rank_way[0]=='t') {
                            pplayer_node->achievement.t=atol(pplayer_node->achievementc);
                        } else
                            break;
                    } else {
                        char *q[]= {"找不到该游戏!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    if(InsertPlayerNode(gp_head,pplayer_node)) {
                        char* p[]= {"插入成功","确定"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"插入失败!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==8) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos1.X==rcPop.Left + 14)
                        continue;
                    if((pplayer_node->user_name[m1-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m1-=2;
                        pplayer_node->user_name[m1]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m1--;
                        pplayer_node->user_name[m1]='\0';
                    }
                    break;
                case 2:
                    if(pos2.X==rcPop.Left + 14)
                        continue;
                    if((pplayer_node->game_name[m2-1]&0x80)==128) {
                        pos2.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        m2-=2;
                        pplayer_node->game_name[m2]='\0';
                    } else {
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        m2--;
                        pplayer_node->game_name[m2]='\0';
                    }
                    break;
                case 3:
                    if(pos3.X==rcPop.Left + 14)
                        continue;
                    pos3.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos3, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos3);
                    m3--;
                    pplayer_node->enroll_datec[m3]='\0';
                    break;
                case 4:
                    if(pos4.X==rcPop.Left + 14)
                        continue;
                    pos4.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos4, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos4);
                    m4--;
                    pplayer_node->total_timec[m4]='\0';
                    break;
                case 5:
                    if(pos5.X==rcPop.Left + 14)
                        continue;
                    pos5.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos5, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos5);
                    m5--;
                    pplayer_node->total_moneyc[m5]='\0';
                    break;
                case 6:
                    if(pos6.X==rcPop.Left + 14)
                        continue;
                    pos6.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos6, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos6);
                    m6--;
                    pplayer_node->achievementc[m6]='\0';
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m1==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_user,&ul);
                        pos1.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m1++]=asc;
                        pplayer_node->user_name[m1++]=ascc;
                        pplayer_node->user_name[m1]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                        pos1.X++;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m1++]=asc;
                        pplayer_node->user_name[m1]='\0';
                    }
                    break;
                case 2:
                    if(m2==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_game,&ul);
                        pos2.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[m2++]=asc;
                        pplayer_node->game_name[m2++]=ascc;
                        pplayer_node->game_name[m2]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                        pos2.X++;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[m2++]=asc;
                        pplayer_node->game_name[m2]='\0';
                    }
                    break;
                case 3:
                    if(m3==8)
                        continue;
                    if(asc>=48&&asc<=57) {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos3, &ul);
                        pos3.X++;
                        SetConsoleCursorPosition(gh_std_out,pos3);
                        pplayer_node->enroll_datec[m3++]=asc;
                        pplayer_node->enroll_datec[m3]='\0';
                    }
                    break;
                case 4:
                    if(m4==14)
                        continue;
                    if(asc>=48&&asc<=57) {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos4, &ul);
                        pos4.X++;
                        SetConsoleCursorPosition(gh_std_out,pos4);
                        pplayer_node->total_timec[m4++]=asc;
                        pplayer_node->total_timec[m4]='\0';
                    }
                    break;
                case 5:
                    if(m5==14)
                        continue;
                    if(asc>=46&&asc<=57&&asc!=47) {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos5, &ul);
                        pos5.X++;
                        SetConsoleCursorPosition(gh_std_out,pos5);
                        pplayer_node->total_moneyc[m5++]=asc;
                        pplayer_node->total_moneyc[m5]='\0';
                    }
                    break;
                case 6:
                    if(m6==14)
                        continue;
                    if(asc>=46&&asc<=57&&asc!=47) {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos6, &ul);
                        pos6.X++;
                        SetConsoleCursorPosition(gh_std_out,pos6);
                        pplayer_node->achievementc[m6++]=asc;
                        pplayer_node->achievementc[m6]='\0';
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**删除玩家基本信息弹出框*/
void VDelPlayerNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"删除玩家基本信息","用户名",
                   "游戏名称"," 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 4;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 10, rcPop.Top +4,
            rcPop.Left+25, rcPop.Top + 4
        },
        {
            rcPop.Left + 3, rcPop.Top +6,
            rcPop.Left+6, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+17, rcPop.Top + 6
        }
    };
    char aSort[] = {1,1,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top +4};
    COORD pos_user= {rcPop.Left + 10, rcPop.Top +3},pos_game= {rcPop.Left + 10, rcPop.Top +4};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                    if(player_node!=NULL) {
                        char *plabel_name[2]= {"确认删除？","确定  取消"};
                        if(ShowModuleYON(plabel_name)) {
                            DelPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                            PopOff();
                            char *p[2]= {"删除成功","确定"};
                            ShowModule(p,2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该玩家","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pplayer_node->user_name[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pplayer_node->user_name[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pplayer_node->user_name[m]='\0';
                    }

                    break;
                case 2:
                    if(pos2.X==rcPop.Left + 10)
                        continue;
                    if((pplayer_node->game_name[n-1]&0x80)==128) {
                        pos2.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ",2 ,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n-=2;
                        pplayer_node->game_name[n]='\0';
                    } else {
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n--;
                        pplayer_node->game_name[n]='\0';
                    }

                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_user,&ul);
                        pos1.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m++]=asc;
                        pplayer_node->user_name[m++]=ascc;
                        pplayer_node->user_name[m]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                        pos1.X++;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m++]=asc;
                        pplayer_node->user_name[m]='\0';
                    }

                    break;
                case 2:
                    if(n==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_game,&ul);
                        pos2.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[n++]=asc;
                        pplayer_node->game_name[n++]=ascc;
                        pplayer_node->game_name[n]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                        pos2.X++;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[n++]=asc;
                        pplayer_node->game_name[n]='\0';
                    }

                    break;
                default:
                    break;
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**修改玩家基本信息弹出框*/
void VModifPlayerNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"修改玩家基本信息","用户名",
                   "游戏名称"," 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 4;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 10, rcPop.Top +4,
            rcPop.Left+25, rcPop.Top + 4
        },
        {
            rcPop.Left + 3, rcPop.Top +6,
            rcPop.Left+6, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+17, rcPop.Top + 6
        }
    };
    char aSort[] = {1,1,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top +4};
    COORD pos_user= {rcPop.Left + 10, rcPop.Top +3},pos_game= {rcPop.Left + 10, rcPop.Top +4};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                    if(player_node!=NULL) {
                        VVModifPlayerNode(player_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该玩家","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pplayer_node->user_name[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pplayer_node->user_name[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pplayer_node->user_name[m]='\0';
                    }
                    break;
                case 2:
                    if(pos2.X==rcPop.Left + 10)
                        continue;
                    if((pplayer_node->game_name[n-1]&0x80)==128) {
                        pos2.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n-=2;
                        pplayer_node->game_name[n]='\0';
                    } else {
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n--;
                        pplayer_node->game_name[n]='\0';
                    }
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_user,&ul);
                        pos1.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m++]=asc;
                        pplayer_node->user_name[m++]=ascc;
                        pplayer_node->user_name[m]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                        pos1.X++;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m++]=asc;
                        pplayer_node->user_name[m]='\0';
                    }
                    break;
                case 2:
                    if(n==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_game,&ul);
                        pos2.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[n++]=asc;
                        pplayer_node->game_name[n++]=ascc;
                        pplayer_node->game_name[n]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                        pos2.X++;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[n++]=asc;
                        pplayer_node->game_name[n]='\0';
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**修改玩家基本信息改动内容弹出框*/
void VVModifPlayerNode(PLAYER_NODE* player_node) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    char *pCh[] = {"玩家基本信息修改","用户名","游戏名称","注册日期","游戏累计时长",
                   "消费总金额","排名指标"," 确定       取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+25;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 8; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+8},
        {rcPop.Left+2, rcPop.Top+10}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 6;       /*热区的个数*/
    SMALL_RECT aArea[] = {
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+29, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+29, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +7,
            rcPop.Left+29, rcPop.Top + 7
        },
        {
            rcPop.Left + 14, rcPop.Top +8,
            rcPop.Left+29, rcPop.Top + 8
        },
        {
            rcPop.Left + 3, rcPop.Top +10,
            rcPop.Left+6, rcPop.Top + 10
        },
        {
            rcPop.Left + 14, rcPop.Top +10,
            rcPop.Left+17, rcPop.Top + 10
        }
    };
    char aSort[] = {1,1,1,1,0,0};
    char aTag[] = {1,2,3,4,5,6};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 9;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE *pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    *pplayer_node=*player_node;
    int m1=strlen(player_node->user_name),m2=strlen(player_node->game_name),
        m3=strlen(player_node->enroll_datec),m4=strlen(player_node->total_timec),
        m5=strlen(player_node->total_moneyc),m6=strlen(player_node->achievementc);
    COORD pos1= {rcPop.Left + 14, rcPop.Top +3},pos2= {rcPop.Left + 14, rcPop.Top + 4},
          pos3= {rcPop.Left + 14, rcPop.Top +5},pos4= {rcPop.Left + 14, rcPop.Top +6},
          pos5= {rcPop.Left + 14, rcPop.Top +7},pos6= {rcPop.Left + 14, rcPop.Top +8};

    WriteConsoleOutputCharacter(gh_std_out, player_node->user_name,m1,pos1, &ul);
    WriteConsoleOutputCharacter(gh_std_out, player_node->game_name,m2,pos2, &ul);
    WriteConsoleOutputCharacter(gh_std_out, player_node->enroll_datec,m3,pos3, &ul);
    pos3.X=pos3.X+m3;
    WriteConsoleOutputCharacter(gh_std_out, player_node->total_timec,m4,pos4, &ul);
    pos4.X=pos4.X+m4;
    WriteConsoleOutputCharacter(gh_std_out, player_node->total_moneyc,m5,pos5, &ul);
    pos5.X=pos5.X+m5;
    WriteConsoleOutputCharacter(gh_std_out, player_node->achievementc,m6,pos6, &ul);
    pos6.X=pos6.X+m6;

    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==5) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标bu可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pplayer_node->total_money=atof(pplayer_node->total_moneyc);
                    pplayer_node->total_time=atof(pplayer_node->total_timec);
                    if(ModifPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name,pplayer_node)) {
                        char* p[]= {"修改成功","确定"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"修改失败!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==6) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos3.X==rcPop.Left + 14)
                        continue;
                    if((pplayer_node->game_name[m3-1]&0x80)==128) {
                        pos3.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos3, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos3);
                        m3-=2;
                        pplayer_node->game_name[m3]='\0';
                    } else {
                        pos3.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos3, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos3);
                        m3--;
                        pplayer_node->game_name[m3]='\0';
                    }
                    break;
                case 2:
                    if(pos4.X==rcPop.Left + 14)
                        continue;
                    pos4.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos4, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos4);
                    m4--;
                    pplayer_node->total_timec[m4]='\0';
                    break;
                case 3:
                    if(pos5.X==rcPop.Left + 14)
                        continue;
                    pos5.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos5, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos5);
                    m5--;
                    pplayer_node->total_moneyc[m5]='\0';
                    break;
                case 4:
                    if(pos6.X==rcPop.Left + 14)
                        continue;
                    pos6.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos6, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos6);
                    m6--;
                    pplayer_node->achievementc[m6]='\0';
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m3==14)
                        continue;
                    if(asc>=46&&asc<=57&&asc!=47) {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos3, &ul);
                        pos3.X++;
                        SetConsoleCursorPosition(gh_std_out,pos3);
                        pplayer_node->game_name[m3++]=asc;
                        pplayer_node->game_name[m3]='\0';
                    }
                    break;
                case 2:
                    if(m4==14)
                        continue;
                    if(asc>=48&&asc<=57) { /*只能输入整数*/
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos4, &ul);
                        pos4.X++;
                        SetConsoleCursorPosition(gh_std_out,pos4);
                        pplayer_node->total_timec[m4++]=asc;
                        pplayer_node->total_timec[m4]='\0';
                    }
                    break;
                case 3:
                    if(m5==14)
                        continue;
                    if(asc>=46&&asc<=57&&asc!=47) { /*只能输入浮点数或整数*/
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos5, &ul);
                        pos5.X++;
                        SetConsoleCursorPosition(gh_std_out,pos5);
                        pplayer_node->total_moneyc[m5++]=asc;
                        pplayer_node->total_moneyc[m5]='\0';
                    }
                    break;
                case 4:
                    if(m6==14)
                        continue;
                    if(asc>=46&&asc<=57&&asc!=47) { /*只能输入浮点数或整数*/
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos6, &ul);
                        pos6.X++;
                        SetConsoleCursorPosition(gh_std_out,pos6);
                        pplayer_node->achievementc[m6++]=asc;
                        pplayer_node->achievementc[m6]='\0';
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}
/**查询*/
/**游戏类别信息查询*/
BOOL QueryClassInfo(void) {
    BOOL bRet=TRUE;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"按类别编码查找","按类别描述查找","取消"};
    int iHot=1;
    int i, j, loc = 0;
    pos.X = strlen(pCh[0])+4;
    pos.Y = 6;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;                    /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+7, rcPop.Top+4}
    }; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc;                     /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 2, rcPop.Top + 1,
            rcPop.Right-2, rcPop.Top + 1
        },
        {
            rcPop.Left +2, rcPop.Top + 2,
            rcPop.Right-2, rcPop.Top + 2
        },
        {
            rcPop.Left +7, rcPop.Top + 4,
            rcPop.Right -7, rcPop.Top + 4
        }
    };
    char aSort[] = {0,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 3;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VQueryClassInfoById();
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VQueryClassInfoByDec();
        PopOff();
    } else {
        PopOff();
    }
    return bRet;
}
/**ById游戏类别信息查询弹出框*/
void VQueryClassInfoById() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"游戏类别信息查询ById","类别编码",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+14, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    //WriteConsoleOutputCharacter(gh_std_out, id,3,pos1, &ul);
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    //puts(pclass_node->class_id);
                    class_node=SeekClassNodeById(gp_head,pclass_node->class_id);
                    if(class_node!=NULL) {
                        char *plabel_name[5]= {"类别编码",class_node->class_id,"类别描述",class_node->class_dec,"确定"};
                        ShowModuleWithExp(plabel_name, 5);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该类别","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m--;
                    pclass_node->class_id[m]='\0';
                } else {
                    continue;
                }
            } else {
                if(m==3)
                    continue;
                WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                pos1.X++;
                SetConsoleCursorPosition(gh_std_out,pos1);
                pclass_node->class_id[m++]=asc;
                pclass_node->class_id[m]='\0';
                continue;
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**ByDec游戏类别信息查询弹出框*/
void VQueryClassInfoByDec() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"游戏类别信息查询ByDec","类别描述",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+8;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    class_node=SeekClassNodeByDec(gp_head,pclass_node->class_dec);
                    if(class_node!=NULL) {
                        char *plabel_name[5]= {"类别编码",class_node->class_id,"类别描述",class_node->class_dec,"确定"};
                        ShowModuleWithExp(plabel_name, 5);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该类别","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pclass_node->class_dec[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pclass_node->class_dec[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pclass_node->class_dec[m]='\0';
                    }
                } else {
                    continue;
                }
            } else {
                if(m==14)
                    continue;
                if((asc&0x80)==128) {
                    ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                    ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    putchar(asc);
                    putchar(ascc);
                    FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                    pos1.X+=2;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pclass_node->class_dec[m++]=asc;
                    pclass_node->class_dec[m++]=ascc;
                    pclass_node->class_dec[m]='\0';
                } else {
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                    pos1.X++;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pclass_node->class_dec[m++]=asc;
                    pclass_node->class_dec[m]='\0';
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}


/**游戏基本信息查询*/
BOOL QueryGameInfo(void) {
    BOOL bRet = TRUE;
    char *pCh[] = { "按游戏名称查找",
                    "按游戏类别、收费模式",
                    "和排名方式查找",
                    "取消"
                  };
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot=1;
    int i, j, loc = 0;
    pos.X = strlen(pCh[1])+2;/*pch[1]是因为第二个字符串最长*/
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;                    /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数，有四个*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc;                     /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，只有三个*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 2, rcPop.Top + 1,
            rcPop.Right-2, rcPop.Top + 1
        },
        {
            rcPop.Left +2, rcPop.Top + 2,
            rcPop.Right-2, rcPop.Top + 3
        },
        {
            rcPop.Left + 8, rcPop.Top + 5,
            rcPop.Right -10,rcPop.Top + 5
        }
    };
    char aSort[] = {0,0,0};
    char aTag[] = {1,2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VQueryGameInfo();/*按游戏名称寻找*/
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VQueryGameInfoM();/*组合寻找*/
        PopOff();
    } else {
        PopOff();
    }
    return bRet;
}

/**精确查找游戏基本信息弹出框*/
void VQueryGameInfo() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"游戏基本信息查询","游戏名称",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNode(gp_head,pgame_node->game_name);
                    if(game_node!=NULL) {
                        char *plabel_name[11]= {"游戏类别",game_node->class_id,"游戏名称",game_node->game_name,
                                                "游戏地址",game_node->game_adress,"收费模式",game_node->charge_model,
                                                "排名方式",game_node->rank_way,"确定"
                                               };
                        ShowModuleWithExp(plabel_name, 11);
                        //putchar(game_node->charge_model);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该游戏","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pgame_node->game_name[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pgame_node->game_name[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pgame_node->game_name[m]='\0';
                    }
                } else {
                    continue;
                }
            } else {
                if(m==14)
                    continue;
                if((asc&0x80)==128) { /*如果是中文字符*/
                    ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                    ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    putchar(asc);
                    putchar(ascc);
                    FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                    pos1.X+=2;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m++]=ascc;
                    pgame_node->game_name[m]='\0';
                } else {
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                    pos1.X++;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m]='\0';
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**组合查找游戏基本信息弹出框*/
void VQueryGameInfoM() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    char *pCh[] = {"游戏信息组合查找","游戏类别","收费模式",
                   "排名方式"," 确定     取消"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 9;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 5; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+7}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 5;       /*热区的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top + 3,
            rcPop.Left+14, rcPop.Top + 3
        },
        {
            rcPop.Left + 10, rcPop.Top +4,
            rcPop.Left+11, rcPop.Top + 4
        },
        {
            rcPop.Left + 10, rcPop.Top +5,
            rcPop.Left+11, rcPop.Top + 5
        },
        {
            rcPop.Left + 3, rcPop.Top +7,
            rcPop.Left+6, rcPop.Top + 7
        },
        {
            rcPop.Left + 12, rcPop.Top +7,
            rcPop.Left+15, rcPop.Top + 7
        }
    };
    char aSort[] = {1,1,1,0,0};
    char aTag[] = {1,2,3,4,5};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 6;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    GAME_NODE *pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m1=0,m2=0,m3=0,m4=0,m5=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos4= {rcPop.Left + 10, rcPop.Top +4},
          pos5= {rcPop.Left + 10, rcPop.Top +5};
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==4) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标bu可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    char *pcondition[3]= {pgame_node->class_id,pgame_node->charge_model,pgame_node->rank_way};
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNodeM(gp_head,pcondition);
                    if(game_node!=NULL) {
                        VVQueryGameInfoM(game_node);
                        break;
                    } else {
                        char *q[]= {"没有符合条件的游戏","确定"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==5) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos1.X==rcPop.Left + 10)
                        break;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m1--;
                    pgame_node->class_id[m1]='\0';
                    break;
                case 2:
                    if(pos4.X==rcPop.Left + 10)
                        break;
                    pos4.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos4, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos4);
                    break;
                case 3:
                    if(pos5.X==rcPop.Left + 10)
                        break;
                    pos5.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos5, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos5);
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m1==3)
                        continue;
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                    pos1.X++;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->class_id[m1++]=asc;
                    pgame_node->class_id[m1]='\0';
                    break;
                case 2:
                    if(asc=='c'||asc=='p'||asc=='t') {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos4, &ul);
                        pgame_node->charge_model[0]=asc;
                        pgame_node->charge_model[1]='\0';
                    }
                    break;
                case 3:
                    if(asc=='p'||asc=='t') {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos5, &ul);
                        pgame_node->rank_way[0]=asc;
                        pgame_node->rank_way[1]='\0';
                    }
                    break;
                default:
                    break;
                }
            }

        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**组合查找游戏基本信息查询结果弹出框*/
void VVQueryGameInfoM(GAME_NODE* game_node) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos,pos1,pos2;
    WORD att;
    int iHot = 1;
    int i, str_len;

    pos.X = 50;
    pos.Y = 15;
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"符合条件的游戏","游戏类别：",game_node->class_id,
                      "收费模式：",game_node->charge_model,
                      "排名方式：",game_node->rank_way,
                      "游戏名称","游戏网址","确定"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;  /*白底黑字*/
    labels.num = 10; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[10]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    aLoc[0].X=rcPop.Left+20;
    aLoc[0].Y=rcPop.Top+1;/*符合条件的游戏*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*游戏类别*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*class_id*/
    aLoc[3].X=rcPop.Left+19;
    aLoc[3].Y=rcPop.Top+3;/*收费模式*/
    aLoc[4].X=rcPop.Left+29;
    aLoc[4].Y=rcPop.Top+3;/*charge_model*/
    aLoc[5].X=rcPop.Left+33;
    aLoc[5].Y=rcPop.Top+3;/*排名方式*/
    aLoc[6].X=rcPop.Left+43;
    aLoc[6].Y=rcPop.Top+3;/*rank_way*/
    aLoc[7].X=rcPop.Left+3;
    aLoc[7].Y=rcPop.Top+5;/*游戏名称*/
    aLoc[8].X=rcPop.Left+20;
    aLoc[8].Y=rcPop.Top+5;/*游戏网址*/
    aLoc[9].X=rcPop.Left+24;
    aLoc[9].Y=rcPop.Bottom-1;/*确定*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[9].X, aLoc[9].Y,
            aLoc[9].X + 3, aLoc[9].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;

    PopUp(&rcPop, att, &labels, &areas);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+20;
    for (i=rcPop.Top+6; game_node!=NULL; game_node=game_node->next,i++) {
        pos1.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, game_node->game_name, strlen(game_node->game_name),
                                    pos1, &ul);
        pos2.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, game_node->game_adress, strlen(game_node->game_adress),
                                    pos2, &ul);
    }
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Top+4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**玩家基本信息查询*/
BOOL QueryPlayerInfo(void) {
    BOOL bRet = TRUE;
    char *pCh[] = { "按用户名和游戏名",
                    "称查找",
                    "按注册日期和消费",
                    "总金额查找",
                    "取消"
                  };
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot=1;
    int i, j, loc = 0;
    pos.X = strlen(pCh[3])+10;/*pch[3]是因为第四个字符串最长*/
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;                    /*标签束第一个标签字符串的地址*/
    labels.num = 5; /*标签束中标签字符串的个数，有5个*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+8, rcPop.Top+6}
    }; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc;                     /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，只有三个*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 2, rcPop.Top + 1,
            rcPop.Right-2, rcPop.Top + 2
        },
        {
            rcPop.Left +2, rcPop.Top + 3,
            rcPop.Right-2, rcPop.Top + 4
        },
        {
            rcPop.Left +8, rcPop.Top +6,
            rcPop.Right -8,rcPop.Top + 6
        }
    };
    char aSort[] = {0,0,0};
    char aTag[] = {1,2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VQueryPlayerInfo();/*按用户名和游戏名称查找*/
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VQueryPlayerInfoM();/*组合查找*/
        PopOff();
    } else {
        PopOff();
    }
    return bRet;
}

/**精确查找玩家基本信息弹出框*/
void VQueryPlayerInfo(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"玩家基本信息查询","用户名",
                   "游戏名称"," 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+14;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 4;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 10, rcPop.Top +4,
            rcPop.Left+25, rcPop.Top + 4
        },
        {
            rcPop.Left + 3, rcPop.Top +6,
            rcPop.Left+6, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+17, rcPop.Top + 6
        }
    };
    char aSort[] = {1,1,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top +4},
          pos_user= {rcPop.Left + 10, rcPop.Top +3},pos_game= {rcPop.Left + 10, rcPop.Top +4};

    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                    if(player_node!=NULL) {
                        char *plabel_name[13]= {"用户名",player_node->user_name,"游戏名称",player_node->game_name,
                                                "注册日期",player_node->enroll_datec,"游戏累计时长",player_node->total_timec,
                                                "消费总金额",player_node->total_moneyc,"排名指标",player_node->achievementc,
                                                "确定"
                                               };
                        ShowModuleWithExp(plabel_name, 13);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该玩家","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pplayer_node->user_name[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pplayer_node->user_name[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pplayer_node->user_name[m]='\0';
                    }
                    break;
                case 2:
                    if(pos2.X==rcPop.Left + 10)
                        continue;
                    if((pplayer_node->game_name[n-1]&0x80)==128) {
                        pos2.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ", 2,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n-=2;
                        pplayer_node->game_name[n]='\0';
                    } else {
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n--;
                        pplayer_node->game_name[n]='\0';
                    }
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_user,&ul);
                        pos1.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m++]=asc;
                        pplayer_node->user_name[m++]=ascc;
                        pplayer_node->user_name[m]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                        pos1.X++;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->user_name[m++]=asc;
                        pplayer_node->user_name[m]='\0';
                    }
                    break;
                case 2:
                    if(n==14)
                        continue;
                    if((asc&0x80)==128) { /*如果是中文字符*/
                        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                        ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        putchar(asc);
                        putchar(ascc);
                        FillConsoleOutputAttribute(gh_std_out,att1,15,pos_game,&ul);
                        pos2.X+=2;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[n++]=asc;
                        pplayer_node->game_name[n++]=ascc;
                        pplayer_node->game_name[n]='\0';
                    } else {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                        pos2.X++;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->game_name[n++]=asc;
                        pplayer_node->game_name[n]='\0';
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**组合查找玩家基本信息弹出框*/
void VQueryPlayerInfoM(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"组合模糊查找玩家信息","注册日期  1",
                   "消费总金额2"," 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 4; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 4;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 13, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 13, rcPop.Top +4,
            rcPop.Left+25, rcPop.Top + 4
        },
        {
            rcPop.Left + 3, rcPop.Top +6,
            rcPop.Left+6, rcPop.Top + 6
        },
        {
            rcPop.Left + 14, rcPop.Top +6,
            rcPop.Left+17, rcPop.Top + 6
        }
    };
    char aSort[] = {1,1,0,0};
    char aTag[] = {1, 2,3,4};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));/*暂时存放输入的条件*/
    pplayer_node->enroll_datec[0]='1';
    pplayer_node->total_moneyc[0]='2';
    int m=1,n=1;
    COORD pos1= {rcPop.Left + 13, rcPop.Top +3},pos2= {rcPop.Left + 13, rcPop.Top +4};;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);

                    if(m!=10) {
                        char *plabel_name[2]= {"注册日期不合法！","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                    int iyear,imonth,iday;
                    char year[5],month[3],day[3];
                    strncpy(year,pplayer_node->enroll_datec+2,4);
                    strncpy(month,pplayer_node->enroll_datec+6,2);
                    //month[2]='\0';
                    strncpy(day,pplayer_node->enroll_datec+8,2);
                    //day[2]='\0';
                    iyear=atoi(year);
                    imonth=atoi(month);
                    iday=atoi(day);
                    //printf("%d\n%d\n%d",iyear,imonth,iday);
                    if(iyear<1970||iyear>2999||imonth<1||imonth>12)
                    {
                        char *q[]= {"找不到该年份或月份!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    int febdays;
                    int dayfault=0;
                    switch(imonth)
                    {
                        case 1:
                        case 3:
                        case 5:
                        case 7:
                        case 8:
                        case 10:
                        case 12:
                            if(iday<1||iday>31)
                                dayfault=1;
                            break;
                        case 4:
                        case 6:
                        case 9:
                        case 11:
                            if(iday<1||iday>30)
                                dayfault=1;
                            break;
                        case 2:
                            if(iyear%400==0||(iyear%4==0&&iyear%100!=0))
                                febdays=29;
                            else
                                febdays=28;
                            if(iday<1||iday>febdays)
                                dayfault=1;
                            break;
                    }
                    if(dayfault==1)
                    {
                        char *q[]= {"该月天数错误!","确定"};
                        ShowModule(q,2);
                        break;
                    }
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNodeM(gp_head,pplayer_node->enroll_datec,pplayer_node->total_moneyc);
                    if(player_node!=NULL) {
                        VVQueryPlayerInfoM(player_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到符合的玩家","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*按下backspace*/
                switch(iHot) {
                case 1:
                    if(pos1.X==rcPop.Left + 13)
                        continue;
                    pos1.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    m--;
                    pplayer_node->enroll_datec[m]='\0';
                    break;
                case 2:
                    if(pos2.X==rcPop.Left + 13)
                        continue;
                    pos2.X--;
                    WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                    SetConsoleCursorPosition(gh_std_out,pos2);
                    n--;
                    pplayer_node->total_moneyc[n]='\0';
                    break;
                default:
                    break;
                }
            } else {
                switch(iHot) {
                case 1:
                    if(m==10)
                        continue;
                    if(asc>=48&&asc<=57||asc>=60&&asc<=62) {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                        pos1.X++;
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        pplayer_node->enroll_datec[m++]=asc;
                        pplayer_node->enroll_datec[m]='\0';
                    }
                    if(pplayer_node->enroll_datec[1]>=48&&pplayer_node->enroll_datec[1]<=57) {
                        char *plabel_name[2]= {"请先输入符号","确定"};
                        HShowModule(plabel_name, 2);
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pplayer_node->enroll_datec[m]='\0';
                    }
                    break;
                case 2:
                    if(n==16)
                        continue;
                    if(asc>=48&&asc<=57||asc>=60&&asc<=62||asc==46) {
                        WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos2, &ul);
                        pos2.X++;
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        pplayer_node->total_moneyc[n++]=asc;
                        pplayer_node->total_moneyc[n]='\0';
                    }
                    if(pplayer_node->total_moneyc[1]>=48&&pplayer_node->total_moneyc[1]<=57) {
                        char *plabel_name[2]= {"请先输入符号","确定"};
                        HShowModule(plabel_name, 2);
                        pos2.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos2, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos2);
                        n--;
                        pplayer_node->total_moneyc[n]='\0';
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**组合查找玩家基本信息查询结果弹出框*/
void VVQueryPlayerInfoM(PLAYER_NODE* player_node) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos,pos1,pos2,pos3,pos4,pos5;
    WORD att;
    int iHot = 1;
    int i, str_len;

    pos.X = 62;
    pos.Y = 15;
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"符合条件的玩家","注册日期","消费总金额",
                      "用户名","游戏名称","游戏累计时长","确定"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;  /*白底黑字*/
    labels.num = 7; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[7]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    aLoc[0].X=rcPop.Left+25;
    aLoc[0].Y=rcPop.Top+1;/*符合条件的玩家*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*注册日期*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*消费总金额*/
    aLoc[3].X=rcPop.Left+25;
    aLoc[3].Y=rcPop.Top+3;/*用户名*/
    aLoc[4].X=rcPop.Left+40;
    aLoc[4].Y=rcPop.Top+3;/*游戏名称*/
    aLoc[5].X=rcPop.Left+50;
    aLoc[5].Y=rcPop.Top+3;/*游戏累计时长*/
    aLoc[6].X=rcPop.Left+30;
    aLoc[6].Y=rcPop.Bottom-1;/*确定*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[6].X, aLoc[6].Y,
            aLoc[6].X + 3, aLoc[6].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    //puts("lall");
    PopUp(&rcPop, att, &labels, &areas);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+13;
    pos3.X=rcPop.Left+25;
    pos4.X=rcPop.Left+40;
    pos5.X=rcPop.Left+50;
    for (i=rcPop.Top+4; player_node!=NULL; player_node=player_node->next,i++) {
        pos1.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, player_node->enroll_datec, strlen(player_node->enroll_datec),
                                    pos1, &ul);
        pos2.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, player_node->total_moneyc, strlen(player_node->total_moneyc),
                                    pos2, &ul);
        pos3.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, player_node->user_name, strlen(player_node->user_name),
                                    pos3, &ul);
        pos4.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, player_node->game_name, strlen(player_node->game_name),
                                    pos4, &ul);
        pos5.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, player_node->total_timec, strlen(player_node->total_timec),
                                    pos5, &ul);
    }
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**统计*/
/**游戏信息统计  数据统计1*/
BOOL StatGame(void) {
    BOOL bRet = TRUE;
    GAME_STAT_NODE* game_stat_node;
    game_stat_node=StatGameInfo(gp_head);
    if(game_stat_node!=NULL) {
        VStatGame(game_stat_node);
    } else {
        char *plabel_name[] = {"没有数据！",
                               "确认"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**游戏信息统计结果弹出框*/
void VStatGame(GAME_STAT_NODE* game_stat_node) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos,pos1,pos2,pos3,pos4,pos5,pos6;
    WORD att,att1;
    int iHot = 1;
    int i, str_len;

    pos.X = 78;
    pos.Y = 15;
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    //puts(game_node->class_id);
    char* pString[]= {"游戏信息统计","游戏名称","玩家数",
                      "玩家总时间H","玩家人均时间↓","玩家总消费Y","玩家人均消费","确定"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 8; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[8]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    aLoc[0].X=rcPop.Left+30;
    aLoc[0].Y=rcPop.Top+1;/*游戏信息统计*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*游戏名称*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*玩家数*/
    aLoc[3].X=rcPop.Left+20;
    aLoc[3].Y=rcPop.Top+3;/*玩家游戏总时间*/
    aLoc[4].X=rcPop.Left+35;
    aLoc[4].Y=rcPop.Top+3;/*玩家人均游戏时间*/
    aLoc[5].X=rcPop.Left+50;
    aLoc[5].Y=rcPop.Top+3;/*玩家消费总金额*/
    aLoc[6].X=rcPop.Left+65;
    aLoc[6].Y=rcPop.Top+3;/*玩家人均消费金额*/
    aLoc[7].X=rcPop.Left+35;
    aLoc[7].Y=rcPop.Bottom-1;/*确定*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[7].X, aLoc[7].Y,
            aLoc[7].X + 3, aLoc[7].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    //puts("lall");
    PopUp(&rcPop, att, &labels, &areas);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+13;
    pos3.X=rcPop.Left+20;
    pos4.X=rcPop.Left+35;
    pos4.Y=rcPop.Top+3;
    pos5.X=rcPop.Left+50;
    pos6.X=rcPop.Left+65;
    FillConsoleOutputAttribute(gh_std_out,att1,13,pos4,&ul);/*排序方式红色显示*/
    for (i=rcPop.Top+4; game_stat_node!=NULL; game_stat_node=game_stat_node->next,i++) {
        pos1.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, game_stat_node->game_name, strlen(game_stat_node->game_name),
                                    pos1, &ul);
        pos2.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos2);
        printf("%d",game_stat_node->player_num);
        pos3.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos3);
        printf("%g",game_stat_node->player_time);
        pos4.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos4);
        printf("%g",game_stat_node->player_aver_time);
        pos5.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos5);
        printf("%g",game_stat_node->player_money);
        pos6.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos6);
        printf("%g",game_stat_node->player_aver_money);
        pos.X=2;
        pos.Y=i;
        FillConsoleOutputAttribute(gh_std_out,att,76,pos,&ul);
    }
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**玩家排行统计统计  数据统计2*/
BOOL StatRank(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc,ascc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"游戏玩家排行统计","游戏名称",
                   " 确定       取消"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel =pCh;   /*标签束第一个标签字符串的地址*/
    labels.num = 3; /*标签束中标签字符串的个数*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = 3;       /*热区的个数，等于标签的个数*/
    SMALL_RECT aArea[] = {{
            rcPop.Left + 10, rcPop.Top +3,
            rcPop.Left+25, rcPop.Top + 3
        },
        {
            rcPop.Left + 3, rcPop.Top +5,
            rcPop.Left+6, rcPop.Top + 5
        },
        {
            rcPop.Left + 14, rcPop.Top +5,
            rcPop.Left+17, rcPop.Top + 5
        }
    };
    char aSort[] = {1,0,0};
    char aTag[] = {1, 2,3};
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/

    pos.X = rcPop.Left + 1; /*画横线的位置*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*画横线*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*键盘产生*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC键*/
                break;
            } else if (vkc == 13) { /*回车键表示按下当前按钮*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*设置光标不可见*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    if(SeekGameNode(gp_head,pgame_node->game_name)!=NULL) {
                        GAME_RANK_NODE* game_rank_node=StatRankList(gp_head,pgame_node->game_name);
                        if(game_rank_node!=NULL) {
                            VStatRank(game_rank_node);
                        } else {
                            char *plabel_name[2]= {"该游戏木有玩家~","确定"};
                            ShowModule(plabel_name, 2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"没有找到该游戏","确定"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==3) {
                    PopOff();
                    break;
                } else {
                    continue;
                }
            }

            else if(vkc==8) {
                /*按下backspace*/
                if(iHot==1) {
                    if(pos1.X==rcPop.Left + 10)
                        continue;
                    if((pgame_node->game_name[m-1]&0x80)==128) {
                        pos1.X-=2;
                        WriteConsoleOutputCharacter(gh_std_out, "  ",2,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m-=2;
                        pgame_node->game_name[m]='\0';
                    } else {
                        pos1.X--;
                        WriteConsoleOutputCharacter(gh_std_out, " ", 1,pos1, &ul);
                        SetConsoleCursorPosition(gh_std_out,pos1);
                        m--;
                        pgame_node->game_name[m]='\0';
                    }
                } else {
                    continue;
                }
            } else {
                if(m==14)
                    continue;
                if((asc&0x80)==128) { /*如果是中文字符*/
                    ReadConsoleInput(gh_std_in, &inRec, 1, &res);
                    ascc = inRec.Event.KeyEvent.uChar.AsciiChar;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    putchar(asc);
                    putchar(ascc);
                    FillConsoleOutputAttribute(gh_std_out,att1,15,pos,&ul);
                    pos1.X+=2;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m++]=ascc;
                    pgame_node->game_name[m]='\0';
                } else {
                    WriteConsoleOutputCharacter(gh_std_out, &asc, 1,pos1, &ul);
                    pos1.X++;
                    SetConsoleCursorPosition(gh_std_out,pos1);
                    pgame_node->game_name[m++]=asc;
                    pgame_node->game_name[m]='\0';
                }
            }
        }
    }
    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;                /*设置光标不可见*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**玩家排行统计统计弹出框*/
void VStatRank(GAME_RANK_NODE* game_rank_node) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos,pos1,pos2,pos3,pos4,pos5,post,posc;
    WORD att,att1;
    int iHot = 1;
    int i, str_len;

    pos.X = 70;
    pos.Y = 15;
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    char* pString[]= {"游戏类别：",game_rank_node->class_id,
                      "游戏名称：",game_rank_node->game_name,
                      "排名方式：",game_rank_node->rank_way,
                      "名次","用户名","游戏累计时长","消费总额","排名指标","确定"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 12; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[12]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    aLoc[0].X=rcPop.Left+3;
    aLoc[0].Y=rcPop.Top+1;
    aLoc[1].X=rcPop.Left+13;
    aLoc[1].Y=rcPop.Top+1;/*游戏类别*/
    aLoc[2].X=rcPop.Left+20;
    aLoc[2].Y=rcPop.Top+1;
    aLoc[3].X=rcPop.Left+30;
    aLoc[3].Y=rcPop.Top+1;/*游戏名称*/
    aLoc[4].X=rcPop.Left+45;
    aLoc[4].Y=rcPop.Top+1;
    aLoc[5].X=rcPop.Left+55;
    aLoc[5].Y=rcPop.Top+1;/*排名方式*/
    aLoc[6].X=rcPop.Left+3;
    aLoc[6].Y=rcPop.Top+3;/*名次*/
    aLoc[7].X=rcPop.Left+12;
    aLoc[7].Y=rcPop.Top+3;/*用户名*/
    aLoc[8].X=rcPop.Left+28;
    aLoc[8].Y=rcPop.Top+3;/*游戏累计时长*/
    aLoc[9].X=rcPop.Left+43;
    aLoc[9].Y=rcPop.Top+3;/*消费总额*/
    aLoc[10].X=rcPop.Left+58;
    aLoc[10].Y=rcPop.Top+3;/*排名指标*/
    aLoc[11].X=rcPop.Left+30;
    aLoc[11].Y=rcPop.Bottom-1;/*确定*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[11].X, aLoc[11].Y,
            aLoc[11].X + 3, aLoc[11].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    //puts("lall");
    PopUp(&rcPop, att, &labels, &areas);
    post.X=rcPop.Left+55;
    post.Y=rcPop.Top+1;
    FillConsoleOutputAttribute(gh_std_out,att1,1,post,&ul);/*排序方式红色显示*/
    posc.X=rcPop.Left+58;
    posc.Y=rcPop.Top+3;
    FillConsoleOutputAttribute(gh_std_out,att1,8,posc,&ul);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+12;
    pos3.X=rcPop.Left+28;
    pos4.X=rcPop.Left+43;
    pos5.X=rcPop.Left+58;
    for (i=rcPop.Top+4; game_rank_node!=NULL; game_rank_node=game_rank_node->next,i++) {
        pos1.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos1);
        printf("%d",game_rank_node->rank_place);
        pos2.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, game_rank_node->user_name, strlen(game_rank_node->user_name),
                                    pos2, &ul);
        pos3.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos3);
        printf("%g",game_rank_node->total_time);
        pos4.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos4);
        printf("%g",game_rank_node->total_money);
        pos5.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos5);
        if(game_rank_node->rank_way[0]=='p') {
            printf("%g",game_rank_node->achievement);
        } else {
            printf("%d",game_rank_node->achievement);
        }
        pos.X=5;
        pos.Y=i;
        FillConsoleOutputAttribute(gh_std_out,att,70,pos,&ul);
    }
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**游戏类别统计统计  数据统计3*/
BOOL StatClass(void) {
    BOOL bRet = TRUE;
    CLASS_STAT_NODE* class_stat_node;
    class_stat_node=StatClassInfo(gp_head);
    if(class_stat_node!=NULL) {
        VStatClass(class_stat_node);
    } else {
        char *plabel_name[] = {"没有数据！",
                               "确认"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**游戏类别信息统计结果弹出框*/
void VStatClass(CLASS_STAT_NODE* class_stat_node) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos,pos1,pos2,pos3,pos4,pos5,pos6;
    WORD att,att1;
    int iHot = 1;
    int i;

    pos.X = 78;
    pos.Y = 15;
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"类别信息统计","游戏类别","类别描述",
                      "游戏款数","玩家数↓","玩家总时间","玩家总消费","确定"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 8; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[8]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    aLoc[0].X=rcPop.Left+30;
    aLoc[0].Y=rcPop.Top+1;/*类别信息统计*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*游戏类别*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*类别描述*/
    aLoc[3].X=rcPop.Left+25;
    aLoc[3].Y=rcPop.Top+3;/*游戏款数*/
    aLoc[4].X=rcPop.Left+37;
    aLoc[4].Y=rcPop.Top+3;/*玩家数*/
    aLoc[5].X=rcPop.Left+50;
    aLoc[5].Y=rcPop.Top+3;/*玩家总时间*/
    aLoc[6].X=rcPop.Left+65;
    aLoc[6].Y=rcPop.Top+3;/*玩家总消费*/
    aLoc[7].X=rcPop.Left+35;
    aLoc[7].Y=rcPop.Bottom-1;/*确定*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[7].X, aLoc[7].Y,
            aLoc[7].X + 3, aLoc[7].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;

    PopUp(&rcPop, att, &labels, &areas);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+13;
    pos3.X=rcPop.Left+25;
    pos4.X=rcPop.Left+37;
    pos4.Y=rcPop.Top+3;
    pos5.X=rcPop.Left+50;
    pos6.X=rcPop.Left+65;
    FillConsoleOutputAttribute(gh_std_out,att1,7,pos4,&ul);/*排序方式红色显示*/
    for (i=rcPop.Top+4; class_stat_node!=NULL; class_stat_node=class_stat_node->next,i++) {
        pos1.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, class_stat_node->class_id, strlen(class_stat_node->class_id),
                                    pos1, &ul);
        pos2.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, class_stat_node->class_dec, strlen(class_stat_node->class_dec),
                                    pos2, &ul);
        pos3.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos3);
        printf("%d",class_stat_node->game_num);
        pos4.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos4);
        printf("%g",class_stat_node->player_total_num);
        pos5.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos5);
        printf("%g",class_stat_node->playeer_total_time);
        pos6.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos6);
        printf("%g",class_stat_node->player_total_money);
        pos.X=2;
        pos.Y=i;
        FillConsoleOutputAttribute(gh_std_out,att,76,pos,&ul);
    }
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**玩家信息统计  数据统计4*/
BOOL StatPlayer(void) {
    BOOL bRet = TRUE;
    PLAYER_STAT_NODE* player_stat_node;
    player_stat_node=StatPlayerInfo(gp_head);
    if(player_stat_node!=NULL) {
        VStatPlayer(player_stat_node);
    } else {
        char *plabel_name[] = {"没有数据！",
                               "确认"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**玩家信息统计结果弹出框*/
void VStatPlayer(PLAYER_STAT_NODE* player_stat_node) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos,pos1,pos2,pos3,pos4,pos5,pos6;
    WORD att,att1;
    int iHot = 1;
    int i;

    pos.X = 50;
    pos.Y = 15;
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"玩家信息统计","用户名","游戏累计时长H↓",
                      "消费总金额Y","确定"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 5; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[5]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    aLoc[0].X=rcPop.Left+20;
    aLoc[0].Y=rcPop.Top+1;/*玩家信息统计*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*用户名*/
    aLoc[2].X=rcPop.Left+18;
    aLoc[2].Y=rcPop.Top+3;/*游戏累计时长*/
    aLoc[3].X=rcPop.Left+35;
    aLoc[3].Y=rcPop.Top+3;/*消费总金额*/
    aLoc[4].X=rcPop.Left+25;
    aLoc[4].Y=rcPop.Bottom-1;/*确定*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[4].X, aLoc[4].Y,
            aLoc[4].X + 3, aLoc[4].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;

    PopUp(&rcPop, att, &labels, &areas);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+18;
    pos2.Y=rcPop.Top+3;
    pos3.X=rcPop.Left+35;
    FillConsoleOutputAttribute(gh_std_out,att1,14,pos2,&ul);/*排序方式红色显示*/
    for (i=rcPop.Top+4; player_stat_node!=NULL; player_stat_node=player_stat_node->next,i++) {
        pos1.Y=i;
        WriteConsoleOutputCharacter(gh_std_out, player_stat_node->user_name, strlen(player_stat_node->user_name),
                                    pos1, &ul);
        pos2.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos2);
        printf("%g",player_stat_node->sum_time);
        pos3.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos3);
        printf("%g",player_stat_node->sum_money);
        pos.X=15;
        pos.Y=i;
        FillConsoleOutputAttribute(gh_std_out,att,50,pos,&ul);
    }
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**收费模式统计  数据统计5*/
BOOL StatCharge(void) {
    BOOL bRet = TRUE;
    CHARGE_NODE* charge_node;
    charge_node=StatChargeInfo(gp_head);
    if(charge_node!=NULL) {
        VStatCharge(charge_node);
    } else {
        char *plabel_name[] = {"没有类别信息",
                               "确认"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**收费模式统计结果弹出框*/
void VStatCharge(CHARGE_NODE* charge_node) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos,pos1,pos2,pos3,pos4,pos5;
    WORD att;
    int iHot = 1;
    int i;

    pos.X = 52;
    pos.Y = 10;
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"收费模式信息统计","收费模式","游戏款数",
                      "玩家数","玩家总时间","玩家总消费","确定"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    labels.num = 7; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[7]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    aLoc[0].X=rcPop.Left+20;
    aLoc[0].Y=rcPop.Top+1;/*游戏信息统计*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*收费模式*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*游戏款数*/
    aLoc[3].X=rcPop.Left+20;
    aLoc[3].Y=rcPop.Top+3;/*玩家数*/
    aLoc[4].X=rcPop.Left+28;
    aLoc[4].Y=rcPop.Top+3;/*玩家总时间*/
    aLoc[5].X=rcPop.Left+40;
    aLoc[5].Y=rcPop.Top+3;/*玩家总消费*/
    aLoc[6].X=rcPop.Left+25;
    aLoc[6].Y=rcPop.Bottom-1;/*确定*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[6].X, aLoc[6].Y,
            aLoc[6].X + 3, aLoc[6].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+13;
    pos3.X=rcPop.Left+20;
    pos4.X=rcPop.Left+28;
    pos5.X=rcPop.Left+40;

    for (i=rcPop.Top+4; charge_node!=NULL; charge_node=charge_node->next,i++) {
        pos1.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos1);
        putchar(charge_node->charge_model);
        pos2.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos2);
        printf("%d",charge_node->game_num);
        pos3.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos3);
        printf("%g",charge_node->player_total_num);
        pos4.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos4);
        printf("%g",charge_node->playeer_total_time);
        pos5.Y=i;
        SetConsoleCursorPosition(gh_std_out,pos5);
        printf("%g",charge_node->player_total_money);
        pos.X=15;
        pos.Y=i;
        FillConsoleOutputAttribute(gh_std_out,att,51,pos,&ul);
    }
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

BOOL HelpTopic(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"有问题请发送至作者邮箱",
                           "U201314970@hust.edu.cn",
                           "确认"
                          };

    ShowModule(plabel_name, 3);

    return bRet;
}

BOOL AboutGame(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"作者：程校猛",
                           "华中科技大学CS1307",
                           "确认"
                          };

    ShowModule(plabel_name, 3);

    return bRet;
}

/**
 * 函数名称: InsertClassNode
 * 函数功能: 在十字链表中插入一个游戏类别基本信息结点.
 * 输入参数: hd 主链头指针
 *           pclass_node 指向所要插入结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示插入成功, FALSE表示插入失败
 *
 * 调用说明:
 */
BOOL InsertClassNode(CLASS_NODE **hd,CLASS_NODE *pclass_node) {
    if(*hd==NULL) {
        *hd=pclass_node;
        (*hd)->next=NULL;
        (*hd)->gnext=NULL;
    } else {
        pclass_node->next=(*hd)->next;
        (*hd)->next=pclass_node;
    }
    return TRUE;
}

/**
 * 函数名称: InsertGameNode
 * 函数功能: 在十字链表中插入一个游戏基本信息结点.
 * 输入参数: hd 主链头指针
 *           pgame_node 指向所要插入结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示插入成功, FALSE表示插入失败
 *
 * 调用说明:
 */
BOOL InsertGameNode(CLASS_NODE *hd, GAME_NODE *pgame_node) {
    CLASS_NODE *pclass_node=hd;
    while(pclass_node!=NULL) {    /*在链表中寻找对应游戏类别*/
        if(strcmp(pclass_node->class_id,pgame_node->class_id)==0) { /*如果找到*/
            pgame_node->next=pclass_node->gnext;
            pclass_node->gnext=pgame_node;
            return TRUE;
        }
        pclass_node=pclass_node->next;
    }
    if(pclass_node==NULL) { /*未找到*/
        return FALSE;
    }
}
/**
 * 函数名称: InsertPlayerNode
 * 函数功能: 在十字链表中插入一个玩家基本信息结点.
 * 输入参数: hd 主链头指针
 *           pplayer_node 指向所要插入结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示插入成功, FALSE表示插入失败
 *
 * 调用说明:
 */
BOOL InsertPlayerNode(CLASS_NODE *hd, PLAYER_NODE *pplayer_node) {
    GAME_NODE *pgame_node;
    /*在十字链中找到对应游戏的信息结点*/
    pgame_node=SeekGameNode(hd,pplayer_node->game_name);
    if(pgame_node!=NULL) {              /*如果找到，则插入该结点的玩家基本信息支链*/
        pplayer_node->next=pgame_node->pnext;
        pgame_node->pnext=pplayer_node;
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * 函数名称: DelClassNode
 * 函数功能: 从十字链表中删除指定的游戏类别信息结点.
 * 输入参数: hd 主链头指针
 *           class_id 类别编码
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
 *
 * 调用说明: 删除前输出提示信息，会删除此类别结点上的游戏支链及游戏支链上的玩家支链
 */
BOOL DelClassNode(CLASS_NODE **hd, char *class_id) {
    CLASS_NODE *pclass_node_piror=NULL;
    CLASS_NODE *pclass_node_current=*hd;
    BOOL bRet=FALSE;
    while(pclass_node_current!=NULL) {
        if(strcmp(pclass_node_current->class_id,class_id)==0) {
            break;
        }
        pclass_node_piror=pclass_node_current;
        pclass_node_current=pclass_node_current->next;
    }
    if(pclass_node_current!=NULL) {     /*如果找到*/
        if(pclass_node_piror==NULL) {   /*如果是头指针*/
            *hd=pclass_node_current->next;
        } else {
            pclass_node_piror->next=pclass_node_current->next;
        }
        free(pclass_node_current);  /*释放结点存储区*/
        bRet=TRUE;
    } else {
        bRet=FALSE;
    }
    return bRet;
}

/**
* 函数名称: DelGameNode
* 函数功能: 从十字链表中删除指定的游戏基本信息结点.
* 输入参数: hd 主链头指针
*           game_name 游戏名称
* 输出参数: 无
* 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
*
* 调用说明: 删除前输出提示信息，会删除此游戏结点上的玩家支链
*/
BOOL DelGameNode(CLASS_NODE *hd, char *game_name) { /*输出提示！*/
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node_piror;
    GAME_NODE *pgame_node_current;
    BOOL bRet=FALSE;
    int find=0;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*二重循环依次搜索主链的每个结点的游戏信息支链*/
        pgame_node_piror=pclass_node->gnext;        /*前一个结点*/
        pgame_node_current=pclass_node->gnext;      /*当前结点*/
        while(pgame_node_current!=NULL) {
            if(strcmp(pgame_node_current->game_name,game_name)==0) { /*如果找到*/
                find=1;
                break;
            }
            pgame_node_piror=pgame_node_current;            /*保存前一个节点的信息*/
            pgame_node_current=pgame_node_current->next;
        }
        if(find) {
            break;
        }
    }
    if(find) {
        if(pgame_node_piror==pclass_node->gnext) {
            pclass_node->gnext=pgame_node_current->next;    /*如果位于第一个结点，即类别链上*/
        } else {
            pgame_node_piror->next=pgame_node_current->next;
        }
        free(pgame_node_current); /*释放结点存储区*/
        bRet=TRUE;
    } else {
        bRet=FALSE;
    }
    return bRet;
}
/**
 * 函数名称: DelPlayerNode
 * 函数功能: 从十字链表中删除指定的玩家基本信息结点.
 * 输入参数: hd 主链头指针
 *           game_name 游戏名称
 *           user_name 用户名
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示删除成功, FALSE表示删除失败
 *
 * 调用说明: 根据游戏名称和用户名可唯一确定信息
 */
BOOL DelPlayerNode(CLASS_NODE *hd, char *game_name, char *user_name) {

    GAME_NODE *pgame_node;
    PLAYER_NODE * pplayer_node_piror;
    PLAYER_NODE * pplayer_node_current;
    BOOL bRet=FALSE;

    pgame_node=SeekGameNode(hd,game_name);    /*在十字链中查找该游戏基本信息链点*/
    if(pgame_node!=NULL) {      /*如果找到*/
        /*在玩家基本信息支链上查找给定用户名的玩家信息*/
        pplayer_node_piror=NULL;
        pplayer_node_current=pgame_node->pnext;
        while(pplayer_node_current!=NULL
                &&strcmp(pplayer_node_current->user_name,user_name)!=0) {
            pplayer_node_piror=pplayer_node_current;
            pplayer_node_current=pplayer_node_current->next;
        }

        if(pplayer_node_current!=NULL) { /*如果找到*/
            bRet=TRUE;
            if(pplayer_node_piror==NULL) {      /*如果位于第一个结点，即游戏链上*/
                pgame_node->pnext=pplayer_node_current->next;
            } else {
                pplayer_node_piror->next=pplayer_node_current->next;
            }
            free(pplayer_node_current);         /*释放结点存储区*/
        }
    }
    return bRet;
}

/**
 * 函数名称: ModifClassNode
 * 函数功能: 对指定的游戏类别基本信息结点内容进行修改.
 * 输入参数: hd 主链头指针
 *           class_id 类别编码
 *           pclass_node 指向存放修改内容结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 *
 * 调用说明:
 */
BOOL ModifClassNode(CLASS_NODE **hd, char *class_id, CLASS_NODE *pclass_node) {
    CLASS_NODE *pclass_node_temp;
    CLASS_NODE *pclass_node_next;
    pclass_node_temp=SeekClassNodeById(*hd,class_id);        /*查找游戏类别结点*/
    if(pclass_node_temp!=NULL) { /*如果找到*/
        pclass_node_next=pclass_node_temp->next;        /*赋值前保存指针域*/
        *pclass_node_temp=*pclass_node;
        pclass_node_temp->next=pclass_node_next;        /*恢复指向关系*/
        return TRUE;
    } else {
        return FALSE;
    }
}
/**
 * 函数名称: ModifGameNode
 * 函数功能: 对指定的游戏基本信息结点内容进行修改.
 * 输入参数: hd 主链头指针
 *           game_name 游戏名称
 *           pgame_node 指向存放修改内容结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 *
 * 调用说明:
 */
BOOL ModifGameNode(CLASS_NODE *hd, char *game_name, GAME_NODE *pgame_node) {
    GAME_NODE *pgame_node_temp;
    GAME_NODE *pgame_node_next;
    pgame_node_temp=SeekGameNode(hd,game_name);     /*查找游戏基本信息结点*/
    if(pgame_node_temp!=NULL) {     /*如果找到*/
        pgame_node_next=pgame_node_temp->next;      /*赋值前保存指针域*/
        *pgame_node_temp=*pgame_node;
        pgame_node_temp->next=pgame_node_next;      /*恢复指向关系*/
        return TRUE;
    } else {
        return FALSE;
    }
}
/**
 * 函数名称: ModifPlayerNode
 * 函数功能: 对指定的玩家基本信息结点内容进行修改.
 * 输入参数: hd 主链头指针
 *           game_name 游戏名称
 *           user_name 用户名
 *           pplayer_node 指向存放修改内容结点的指针
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE表示修改成功, FALSE表示修改失败
 *
 * 调用说明:
 */
BOOL ModifPlayerNode(CLASS_NODE *hd, char *game_name, char *user_name, PLAYER_NODE *pplayer_node) {
    PLAYER_NODE *pplayer_node_temp;
    PLAYER_NODE *pplayer_node_next;
    pplayer_node_temp=SeekPlayerNode(hd,game_name,user_name);       /*查找指定玩家信息结点*/
    if(pplayer_node_temp!=NULL) {                    /*如果找到*/
        pplayer_node_next=pplayer_node_temp->next;  /*赋值前保存指针域*/
        *pplayer_node_temp=*pplayer_node;
        pplayer_node_temp->next=pplayer_node_next;  /*恢复指向关系*/
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * 函数名称: SeekClassNodeById
 * 函数功能: 按类别编码查找满足条件的网络游戏分类信息.
 * 输入参数: hd 主链头指针
 *           class_id 类别编码
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 *
 * 调用说明:
 */
CLASS_NODE* SeekClassNodeById(CLASS_NODE *hd,char* class_id) {
    CLASS_NODE *pclass_node;
    if(hd==NULL) {
        return NULL;
    } else if(strcmp(hd->class_id,class_id)==0) {
        return hd;
    } else {
        for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
            //puts("bbb");
            //puts(pclass_node->class_id);
            //puts(class_id);
            if(strcmp(pclass_node->class_id,class_id)==0) {
                return pclass_node;                     /*返回链头*/
            }
        }
    }
    return NULL;
}
/**
* 函数名称: SeekClassNodeByDec
* 函数功能: 按类别描述查找满足条件的网络游戏分类信息.
* 输入参数: hd 主链头指针
*           class_dec 类别描述
* 输出参数: 无
* 返 回 值: 查中时返回结点的地址, 否则返回NULL
*
* 调用说明:
*/
CLASS_NODE*SeekClassNodeByDec(CLASS_NODE *hd,char* class_dec) {
    CLASS_NODE *pclass_node;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        if(strcmp(pclass_node->class_dec,class_dec)==0) {
            return pclass_node;                 /*返回链头*/
        }
    }
    return NULL;
}

/**
 * 函数名称: SeekGameNode
 * 函数功能: 按游戏名称查找游戏基本信息结点.
 * 输入参数: hd 主链头指针
 *           game_name 游戏名称
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 *
 * 调用说明:
 */
GAME_NODE *SeekGameNode(CLASS_NODE *hd, char *game_name) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    int find=0;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*二重循环依次搜索主链的每个结点的游戏信息支链*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            if(strcmp(pgame_node->game_name,game_name)==0) {
                find=1;
                break;
            }
            pgame_node=pgame_node->next;
        }
        if(find) {
            break;
        }
    }
    if(find) {
        return pgame_node;
    } else {
        return NULL;
    }
}

/**
 * 函数名称: SeekplayerNode
 * 函数功能: 按游戏名称和用户名查找玩家信息结点.
 * 输入参数: hd 主链头指针
 *           game_name 游戏名称
 *           user_name 用户名
 * 输出参数: 无
 * 返 回 值: 查中时返回结点的地址, 否则返回NULL
 *
 * 调用说明:
 */
PLAYER_NODE *SeekPlayerNode(CLASS_NODE *hd, char *game_name, char *user_name) {
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    int find=0;
    pgame_node=SeekGameNode(hd,game_name);      /*首先查找对应的游戏基本信息结点*/

    /*如果找到，在玩家信息支链上继续查找指定用户名的玩家信息结点*/
    if(pgame_node!=NULL) {
        pplayer_node=pgame_node->pnext;
        while(pplayer_node!=NULL) {
            if(strcmp(pplayer_node->user_name,user_name)==0) {
                find=1;
                break;
            }
            pplayer_node=pplayer_node->next;
        }
    }
    if(find) {
        return pplayer_node;
    } else {
        return NULL;
    }
}

/**
 * 函数名称: SeekGameNodeM
 * 函数功能: 按多种条件组合查询满足条件的所有游戏信息结点.
 * 输入参数: hd 主链头指针
 *           cond_num 组合条件的个数
 *           ... 表示查询条件的字符串
 * 输出参数: 无
 * 返 回 值: 将所有满足条件的结点复制到结果链表，返回结果链表的头结点地址
 *
 * 调用说明:
 */
GAME_NODE *SeekGameNodeM (CLASS_NODE *hd,char**condition) {
    CLASS_NODE *class_node;
    GAME_NODE *game_node;
    GAME_NODE *game_node_ret=NULL;  /*查询结果的头指针*/
    GAME_NODE *game_node_temp;
    int i;
    BOOL bRet1,bRet2,bRet3;
    for(class_node=hd->next; class_node!=NULL; class_node=class_node->next) {
        /*搜索十字链中每个游戏信息结点，将满足所有条件的结点复制到结果链表*/

        //puts(hd->class_id);
        game_node=class_node->gnext;
        while(game_node!=NULL) {
            //puts(game_node->class_id);
            //puts(condition[0]);
            bRet1=!strcmp(game_node->class_id,condition[0]); /*游戏类别*/
            bRet2=!strcmp(game_node->charge_model,condition[1]);/*收费模式*/
            bRet3=!strcmp(game_node->rank_way,condition[2]);/*排名方式*/
            //printf("%d%d%d",bRet1,bRet2,bRet3);
            if((bRet1==TRUE)&&(bRet2==TRUE)&&(bRet3==TRUE)) { /*相等时，所有条件都满足*/
                /*将该结点信息复制到一个新结点，并将新结点加入到结果链表*/
                game_node_temp=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                *game_node_temp=*game_node;
                game_node_temp->next=game_node_ret;
                game_node_ret=game_node_temp;
            }
            game_node=game_node->next;
        }
    }
    return game_node_ret;
}

/**
 * 函数名称: SeekPlayerNodeM
 * 函数功能: 按多种条件组合查询满足条件的所有玩家信息结点.
 * 输入参数: hd 主链头指针
 *           cond_num 组合条件的个数
 *           ... 表示查询条件的字符串
 * 输出参数: 无
 * 返 回 值: 将所有满足条件的结点复制到结果链表，返回结果链表的头结点地址
 *
 * 调用说明:
 */
PLAYER_NODE *SeekPlayerNodeM (CLASS_NODE *hd,char* condition1,char* condition2) {
    //puts(condition2);
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    PLAYER_NODE *pplayer_node_ret=NULL;          /*查询结果的头指针*/
    PLAYER_NODE *pplayer_node_temp;
    BOOL bRet1,bRet2;
    int i;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*二重循环搜索十字链中每个玩家信息结点，将满足条件的结点复制到结果链表*/
        pgame_node=pclass_node->gnext;
        for(; pgame_node!=NULL; pgame_node=pgame_node->next) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                bRet1=JudgePlayerNodeItem(gp_head,pplayer_node,condition1);/*传入头指针，便于此函数搜索游戏排名方式*/
                bRet2=JudgePlayerNodeItem(gp_head,pplayer_node,condition2);

                if((bRet1==TRUE)&&(bRet2==TRUE)) {          /*所有条件都满足*/
                    /*将该结点信息复制到一个新结点，并将新结点加入到结果链表*/
                    pplayer_node_temp=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    *pplayer_node_temp=*pplayer_node;
                    pplayer_node_temp->next=pplayer_node_ret;
                    pplayer_node_ret=pplayer_node_temp;
                }
                pplayer_node=pplayer_node->next;
            }
        }
    }
    return pplayer_node_ret;
}

/**
 * 函数名称: JudgeGameNodeItem
 * 函数功能: 判断游戏信息结点是否满足给定条件.
 * 输入参数: pgame_node 游戏信息结点指针
 *           pcondition 用来表示条件的字符串
 * 输出参数: 无
 * 返 回 值: 满足条件时, 返回TRUE; 否则返回FALSE
 *
 * 调用说明:
 */
BOOL JudgeGameNodeItem(GAME_NODE *pgame_node, char *pcondition) {
    int item_num;
    BOOL bRet=FALSE;
    item_num=*pcondition;       /*从条件字符串中提取比较数据项的项号*/
    switch(item_num) {          /*分情况进行匹配*/
    case 1:
        bRet=MatchString(pgame_node->class_id,pcondition+1);
        break; /*游戏类别*/
    case 2:
        bRet=MatchString(pgame_node->charge_model,pcondition+1);
        break;/*收费模式*/
    case 3:
        bRet=MatchString(pgame_node->rank_way,pcondition+1);
        break;/*排名方式*/
    }
    return bRet;
}
/**
 * 函数名称: JudgePlayerNodeItem
 * 函数功能: 判断玩家信息结点是否满足给定条件.
 * 输入参数: pplayer_node 玩家信息结点指针
 *           pcondition 用来表示条件的字符串
 * 输出参数: 无
 * 返 回 值: 满足条件时, 返回TRUE; 否则返回FALSE
 *
 * 调用说明:
 */
BOOL JudgePlayerNodeItem(CLASS_NODE *hd,PLAYER_NODE *pplayer_node, char *pcondition) {
    int item_num;
    BOOL bRet=FALSE;
    item_num=*pcondition;       /*从条件字符串中提取比较数据项的项号*/
    //printf("%d  ",*pcondition);
    switch(item_num) {          /*分情况进行匹配*/
    case 49:
        bRet=MatchString(pplayer_node->enroll_datec,pcondition+1);
        break;/*1的asc码为49*/
    case 50:
        //printf("%f\n",pplayer_node->total_money);
        bRet=MatchDouble(pplayer_node->total_money,pcondition+1);
        //puts(pcondition+1);
        break;/*2的asc码为50*/
    }
    return bRet;
}

/**
 * 函数名称: MatchString
 * 函数功能: 对给定字符串按条件进行匹配.
 * 输入参数: string_item 给定字符串
 *           pcond 包含匹配运算符在内的条件字符串
 * 输出参数: 无
 * 返 回 值: 匹配成功时, 返回TRUE; 否则返回FALSE
 *
 * 调用说明:
 */
BOOL MatchString(char *string_item, char *pcond) {
    char op;
    int compare_result;
    char * sub_string_pos;
    BOOL bRet=FALSE;
    compare_result=strcmp(string_item,pcond+1);     /*字符串比较大小*/
    sub_string_pos=strstr(string_item,pcond+1);     /*字符串的包含关系*/
    op=*pcond;      /*提取匹配运算符*/
    switch(op) {
    case '=':  /*是否与条件字符串相等*/
        if(compare_result==0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '<':  /*是否小于条件字符串*/
        if(compare_result<0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '>':   /*是否大于条件字符串*/
        if(compare_result>0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '!':   /*是否不等于条件字符串*/
        if(compare_result!=0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '@':   /*是否包含条件字符串*/
        if(sub_string_pos!=NULL) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '?':   /*是否不包含条件字符串*/
        if(sub_string_pos==NULL) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    }
    return bRet;
}

/**
 * 函数名称: MatchChar
 * 函数功能: 对给定字符按条件进行匹配.
 * 输入参数: char_item 给定字符
 *           pcond 包含匹配运算符在内的条件字符串
 * 输出参数: 无
 * 返 回 值: 匹配成功时, 返回TRUE; 否则返回FALSE
 *
 * 调用说明:
 */
BOOL MatchChar(char char_item, char *pcond) {
    char op;
    int compare_result;
    BOOL bRet=FALSE;
    op=*pcond;                              /*获取匹配运算符*/
    compare_result=char_item-*(pcond+1);    /*比较字符大小*/

    switch(op) {
    case '=':       /*是否相等*/
        if(compare_result==0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '<':       /*是否小于*/
        if(compare_result<0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '>':       /*是否大于*/
        if(compare_result>0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '!':       /*是否不相等*/
        if(compare_result!=0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    }
    return bRet;
}

/**
 * 函数名称: MatchLong
 * 函数功能: 对给定long数据按条件进行匹配.
 * 输入参数: long_item 给定long型数据
 *           pcond 包含匹配运算符在内的条件字符串
 * 输出参数: 无
 * 返 回 值: 匹配成功时, 返回TRUE; 否则返回FALSE
 *
 * 调用说明:
 */
BOOL MatchLong(long long_item, char *pcond) {
    char op;
    int compare_result;
    BOOL bRet=FALSE;
    compare_result=long_item-*(pcond+1);
    op=*pcond;

    switch(op) {
    case '=':       /*是否等于*/
        if(compare_result==0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '<':       /*是否小于*/
        if(compare_result<0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '>':       /*是否大于*/
        if(compare_result>0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '!':       /*是否不等于*/
        if(compare_result!=0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    }
    return bRet;
}

/**
 * 函数名称: MatchDouble
 * 函数功能: 对给定double型数据按条件进行匹配.
 * 输入参数: double_item 给定double数据
 *           pcond 包含匹配运算符在内的条件字符串
 * 输出参数: 无
 * 返 回 值: 匹配成功时, 返回TRUE; 否则返回FALSE
 *
 * 调用说明:
 */
BOOL MatchDouble(double double_item, char *pcond) {
    char op;
    double compare_result;
    char condition[20];
    strncpy(condition, pcond+1, sizeof(pcond+1)+2);
    condition[sizeof(condition)] = '\0';
    compare_result=double_item-atof(condition);
    //puts(condition);
    //printf("%f\n",atof(condition));
    //printf("%f  ",double_item);
    //printf("%f\n",compare_result);
    op=*pcond;

    switch(op) {
    case '=':       /*是否等于*/
        if(compare_result==0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '<':       /*是否小于*/
        if(compare_result<0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '>':       /*是否大于*/
        if(compare_result>0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '!':       /*是否不等于*/
        if(compare_result!=0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    }
}

/**
 * 函数名称: StatGameInfo   数据统计1
 * 函数功能: 统计游戏基本信息.
 * 输入参数: hd 主链头结点指针
 * 输出参数: 无
 * 返 回 值: 返回统计结果链头结点地址
 *
 * 调用说明:
 */
GAME_STAT_NODE *StatGameInfo(CLASS_NODE *phd) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    GAME_STAT_NODE *pgame_stat_node_ret=NULL;   /*头指针*/
    //pgame_stat_node_ret=(GAME_STAT_NODE*)malloc(sizeof(GAME_STAT_NODE));
    //pgame_stat_node_ret->next=NULL;
    GAME_STAT_NODE *pgame_stat_node_temp;
    unsigned long player_num=0;   /*<玩家数*/
    double player_time=0;   /*<玩家游戏总时间（小时）*/
    double player_aver_time=0;    /*<玩家人均游戏时间（小时）*/
    double player_money=0;  /*<玩家消费总金额（元）*/
    double player_aver_money=0;   /*<玩家人均消费金额（元）*/
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*二重循环遍历十字交叉连*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            player_num=0;
            player_time=0;
            player_money=0;
            //puts(pgame_node->game_name);
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                //puts(pplayer_node->user_name);
                player_num+=1;          /*进行累加*/
                player_time+=pplayer_node->total_time;
                player_money+=pplayer_node->total_money;
                pplayer_node=pplayer_node->next;
            }
            //printf("%f%f\n",player_time,player_money);
            if(player_num==0) { /*如果玩家数为0*/
                player_aver_time=0;
                player_aver_money=0;
            } else {
                player_aver_time=player_time/player_num;    /*计算平均时间，金额*/
                player_aver_money=player_money/player_num;
            }

            //printf("%g%g\n",player_aver_money,player_aver_time);

            pgame_stat_node_temp=(GAME_STAT_NODE*)malloc(sizeof(GAME_STAT_NODE));
            strcpy(pgame_stat_node_temp->game_name,pgame_node->game_name);
            pgame_stat_node_temp->player_num=player_num;    /*进行赋值*/
            pgame_stat_node_temp->player_time=player_time;
            pgame_stat_node_temp->player_aver_time=player_aver_time;
            pgame_stat_node_temp->player_money=player_money;
            pgame_stat_node_temp->player_aver_money=player_aver_money;

            pgame_stat_node_temp->next=NULL;
            //puts(pgame_stat_node_temp->game_name);
            pgame_stat_node_temp->next=pgame_stat_node_ret; /*加入链表*/
            pgame_stat_node_ret=pgame_stat_node_temp;

            pgame_node=pgame_node->next;    /*指向下一个游戏结点*/
        }
    }
    SortGameInfo(pgame_stat_node_ret);  /*进行排序*/
    return pgame_stat_node_ret;         /*返回指针*/
}

/**
 * 函数名称: SortGameInfo
 * 函数功能: 对游戏信息链进行排序.
 * 输入参数: game_phd 游戏信息链头结点指针
 * 输出参数:
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SortGameInfo(GAME_STAT_NODE *game_phd) {
    GAME_STAT_NODE *pgame_stat_node_piror;
    GAME_STAT_NODE *pgame_stat_node_after;
    GAME_STAT_NODE *pgame_stat_node_current;
    GAME_STAT_NODE *pgame_stat_node_temp;
    pgame_stat_node_piror=game_phd;
    if(pgame_stat_node_piror==NULL) {    /*如果是空链*/
        return;
    }
    /*排序时，用于交换信息的临时结点*/
    pgame_stat_node_temp=(GAME_STAT_NODE*)malloc(sizeof(GAME_STAT_NODE));
    while(pgame_stat_node_piror->next!=NULL) {
        pgame_stat_node_current=pgame_stat_node_piror;
        pgame_stat_node_after=pgame_stat_node_piror->next;
        while(pgame_stat_node_after!=NULL) { /*选择法排序*/
            if(pgame_stat_node_current->player_aver_time<pgame_stat_node_after->player_aver_time) {
                pgame_stat_node_current=pgame_stat_node_after;  /*始终指向最大的*/
            }
            pgame_stat_node_after=pgame_stat_node_after->next;
        }
        if(pgame_stat_node_current!=pgame_stat_node_piror) {
            /*交换结点的信息，指针域不交换*/
            *pgame_stat_node_temp=*pgame_stat_node_piror;
            *pgame_stat_node_piror=*pgame_stat_node_current;
            pgame_stat_node_piror->next=pgame_stat_node_temp->next;
            pgame_stat_node_temp->next=pgame_stat_node_current->next;
            *pgame_stat_node_current=*pgame_stat_node_temp;
        }
        pgame_stat_node_piror=pgame_stat_node_piror->next;
    }
    free(pgame_stat_node_temp);/*释放存储区*/
    return;
}

/**
 * 函数名称: StatRankList  数据统计2
 * 函数功能: 统计玩家排行信息.
 * 输入参数: hd 主链头结点指针
 * 输出参数: 无
 * 返 回 值: 返回统计结果链头结点地址
 *
 * 调用说明:
 */
GAME_RANK_NODE *StatRankList(CLASS_NODE *phd, char *game_name) {
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    GAME_RANK_NODE *pgame_rank_node_ret=NULL;
    GAME_RANK_NODE *pgame_rank_node_temp;
    pgame_node=SeekGameNode(phd,game_name);
    if(pgame_node!=NULL) {
        if(pgame_node->rank_way[0]=='p') { /*如果是按积分排序的*/
            //puts(pgame_node->rank_way);
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                /*进行赋值*/
                pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
                strcpy(pgame_rank_node_temp->game_name,game_name);
                strcpy(pgame_rank_node_temp->class_id,pgame_node->class_id);
                strcpy(pgame_rank_node_temp->rank_way,pgame_node->rank_way);
                strcpy(pgame_rank_node_temp->user_name,pplayer_node->user_name);
                pgame_rank_node_temp->total_time=pplayer_node->total_time;
                pgame_rank_node_temp->total_money=pplayer_node->total_money;
                pgame_rank_node_temp->achievement.p=pplayer_node->achievement.p;

                pgame_rank_node_temp->next=pgame_rank_node_ret;/*加入结果链*/
                pgame_rank_node_ret=pgame_rank_node_temp;

                pplayer_node=pplayer_node->next;
            }
            SortRankList_p(pgame_rank_node_ret);/*排序*/
        } else { /*按时间排名*/
            //puts(pgame_node->rank_way);
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                /*赋值*/
                pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
                strcpy(pgame_rank_node_temp->game_name,game_name);
                strcpy(pgame_rank_node_temp->class_id,pgame_node->class_id);
                strcpy(pgame_rank_node_temp->rank_way,pgame_node->rank_way);
                strcpy(pgame_rank_node_temp->user_name,pplayer_node->user_name);
                pgame_rank_node_temp->total_time=pplayer_node->total_time;
                pgame_rank_node_temp->total_money=pplayer_node->total_money;
                pgame_rank_node_temp->achievement.t=pplayer_node->achievement.t;

                pgame_rank_node_temp->next=pgame_rank_node_ret; /*加入结果链*/
                pgame_rank_node_ret=pgame_rank_node_temp;

                pplayer_node=pplayer_node->next;
            }
            SortRankList_t(pgame_rank_node_ret);/*排序*/
        }
        return pgame_rank_node_ret;
    } else {
        return NULL;
    }
}

/**
 * 函数名称: SortRankList_p
 * 函数功能: 对玩家排行信息链进行排序.
 * 输入参数: rank_phd 玩家排行信息链头结点指针
 * 输出参数:
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SortRankList_p(GAME_RANK_NODE *rank_phd) {
    int rank_place=0;/*名次*/
    GAME_RANK_NODE *pgame_rank_node_piror;
    GAME_RANK_NODE *pgame_rank_node_current;
    GAME_RANK_NODE *pgame_rank_node_after;
    GAME_RANK_NODE *pgame_rank_node_temp;
    pgame_rank_node_piror=rank_phd;
    if(pgame_rank_node_piror==NULL) {
        return;
    }
    if(pgame_rank_node_piror->next==NULL) {
        pgame_rank_node_piror->rank_place=1;/*先对第一个赋值*/
        return;
    }

    //printf("%f",pgame_rank_node_piror->achievement.p);
    pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
    while(pgame_rank_node_piror->next!=NULL) {
        pgame_rank_node_current=pgame_rank_node_piror;
        pgame_rank_node_after=pgame_rank_node_piror->next;
        while(pgame_rank_node_after!=NULL) {

            if(pgame_rank_node_current->achievement.p<pgame_rank_node_after->achievement.p) {
                /*始终指向最大的*/
                pgame_rank_node_current=pgame_rank_node_after;
            }
            pgame_rank_node_after=pgame_rank_node_after->next;
        }
        if(pgame_rank_node_current!=pgame_rank_node_piror) {
            /*交换结点中的信息*/
            *pgame_rank_node_temp=*pgame_rank_node_piror;
            *pgame_rank_node_piror=*pgame_rank_node_current;
            pgame_rank_node_piror->next=pgame_rank_node_temp->next;
            pgame_rank_node_temp->next=pgame_rank_node_current->next;
            *pgame_rank_node_current=*pgame_rank_node_temp;
        }
        rank_place+=1;/*计算名次*/
        pgame_rank_node_piror->rank_place=rank_place;
        pgame_rank_node_piror->next->rank_place=rank_place+1;
        pgame_rank_node_piror=pgame_rank_node_piror->next;
    }
    free(pgame_rank_node_temp);
    return;
}

/**
 * 函数名称: SortRankList_t
 * 函数功能: 对玩家排行信息链进行排序.
 * 输入参数: rank_phd 玩家排行信息链头结点指针
 * 输出参数:
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SortRankList_t(GAME_RANK_NODE *rank_phd) {
    int rank_place=0;
    GAME_RANK_NODE *pgame_rank_node_piror;
    GAME_RANK_NODE *pgame_rank_node_current;
    GAME_RANK_NODE *pgame_rank_node_after;
    GAME_RANK_NODE *pgame_rank_node_temp;
    pgame_rank_node_piror=rank_phd;
    if(pgame_rank_node_piror==NULL) {
        return;
    }
    if(pgame_rank_node_piror->next==NULL) {
        pgame_rank_node_piror->rank_place=1;/*先对第一个赋值*/
        return;
    }
    pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
    while(pgame_rank_node_piror->next!=NULL) {
        pgame_rank_node_current=pgame_rank_node_piror;
        pgame_rank_node_after=pgame_rank_node_piror->next;
        while(pgame_rank_node_after!=NULL) {
            if(pgame_rank_node_current->achievement.t>pgame_rank_node_after->achievement.t) {
                /*始终指向最小的*/
                pgame_rank_node_current=pgame_rank_node_after;
            }
            pgame_rank_node_after=pgame_rank_node_after->next;
        }
        if(pgame_rank_node_current!=pgame_rank_node_piror) {
            /*交换结点信息*/
            *pgame_rank_node_temp=*pgame_rank_node_piror;
            *pgame_rank_node_piror=*pgame_rank_node_current;
            pgame_rank_node_piror->next=pgame_rank_node_temp->next;
            pgame_rank_node_temp->next=pgame_rank_node_current->next;
            *pgame_rank_node_current=*pgame_rank_node_temp;
        }
        rank_place+=1;/*计算名次*/
        pgame_rank_node_piror->rank_place=rank_place;
        pgame_rank_node_piror->next->rank_place=rank_place+1;
        pgame_rank_node_piror=pgame_rank_node_piror->next;
    }
    free(pgame_rank_node_temp);
    return;
}

/**
 * 函数名称: StatClassInfo  数据统计3
 * 函数功能: 统计类别信息.
 * 输入参数: hd 主链头结点指针
 * 输出参数: 无
 * 返 回 值: 返回统计结果链头结点地址
 *
 * 调用说明:
 */
CLASS_STAT_NODE *StatClassInfo(CLASS_NODE *phd) {
    unsigned long game_num;         /*<游戏款数*/
    double player_total_num;        /*<玩家数*/
    double player_total_time;      /*<玩家游戏总时间*/
    double player_total_money;      /*<玩家消费总金额*/
    CLASS_STAT_NODE *pclass_stat_node_ret=NULL;
    CLASS_STAT_NODE *pclass_stat_node_tem;
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*赋初值为0*/
        game_num=0;
        player_total_num=0;
        player_total_time=0;
        player_total_money=0;
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                /*统计*/
                player_total_num+=1;
                player_total_money+=pplayer_node->total_money;
                player_total_time+=pplayer_node->total_time;
                pplayer_node=pplayer_node->next;
            }
            game_num+=1;
            pgame_node=pgame_node->next;
        }
        pclass_stat_node_tem=(CLASS_STAT_NODE*)malloc(sizeof(CLASS_STAT_NODE));/*获取存储区*/
        strcpy(pclass_stat_node_tem->class_id,pclass_node->class_id); /*对结点赋值*/
        strcpy(pclass_stat_node_tem->class_dec,pclass_node->class_dec);
        pclass_stat_node_tem->game_num=game_num;
        pclass_stat_node_tem->playeer_total_time=player_total_time;
        pclass_stat_node_tem->player_total_money=player_total_money;
        pclass_stat_node_tem->player_total_num=player_total_num;

        pclass_stat_node_tem->next=pclass_stat_node_ret;/*加入结果链*/
        pclass_stat_node_ret=pclass_stat_node_tem;
    }
    SortClassInfo(pclass_stat_node_ret);/*排序*/
    return pclass_stat_node_ret;
}

/**
 * 函数名称: SortClassInfo
 * 函数功能: 对类别信息链进行排序.
 * 输入参数: class_phd 类别信息链头结点指针
 * 输出参数: class_phd 排序结果同时通过头结点指针返回
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SortClassInfo(CLASS_STAT_NODE *class_phd) {
    CLASS_STAT_NODE *pclass_stat_node_piror;
    CLASS_STAT_NODE *pclass_stat_node_ccurrent;
    CLASS_STAT_NODE *pclass_stat_node_after;
    CLASS_STAT_NODE *pclass_stat_node_tem;
    pclass_stat_node_piror=class_phd;
    if(pclass_stat_node_piror==NULL) {  /*空链*/
        return;
    }
    pclass_stat_node_tem=(CLASS_STAT_NODE*)malloc(sizeof(CLASS_STAT_NODE));
    while(pclass_stat_node_piror->next!=NULL) {
        pclass_stat_node_ccurrent=pclass_stat_node_piror;
        pclass_stat_node_after=pclass_stat_node_piror->next;
        while(pclass_stat_node_after!=NULL) {
            if(pclass_stat_node_ccurrent->player_total_num<pclass_stat_node_after->player_total_num) {
                /*始终指向最大的*/
                pclass_stat_node_ccurrent=pclass_stat_node_after;
            }
            pclass_stat_node_after=pclass_stat_node_after->next;
        }
        if(pclass_stat_node_ccurrent!=pclass_stat_node_piror) {
            /*交换结点信息，指针域不变*/
            *pclass_stat_node_tem=*pclass_stat_node_piror;
            *pclass_stat_node_piror=*pclass_stat_node_ccurrent;
            pclass_stat_node_piror->next=pclass_stat_node_tem->next;
            pclass_stat_node_tem->next=pclass_stat_node_ccurrent->next;
            *pclass_stat_node_ccurrent=*pclass_stat_node_tem;
        }
        pclass_stat_node_piror=pclass_stat_node_piror->next;
    }
    free(pclass_stat_node_tem);/*释放存储区*/
    return;
}
/**
 * 函数名称: StatPlayerInfo 数据统计4
 * 函数功能: 统计玩家信息.
 * 输入参数: hd 主链头结点指针
 * 输出参数: 无
 * 返 回 值: 返回统计结果链头结点地址
 *
 * 调用说明:用户名在链表中唯一
 */
PLAYER_STAT_NODE *StatPlayerInfo(CLASS_NODE *phd) {
    PLAYER_STAT_NODE *pplayer_stat_node_tem;
    PLAYER_STAT_NODE *pplayer_stat_node_ret=NULL;
    PLAYER_STAT_NODE *pplayer_stat_node_serch;
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*三重循环遍历十字交叉链表*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                //puts(pplayer_node->user_name);
                pplayer_stat_node_serch=Serch_User_Name(pplayer_stat_node_ret,pplayer_node->user_name);
                if(pplayer_stat_node_serch!=NULL) {
                    /*如果生成链中已经存在*/
                    //puts("find it");
                    pplayer_stat_node_serch->sum_money+=pplayer_node->total_money;
                    pplayer_stat_node_serch->sum_time+=pplayer_node->total_time;
                } else {
                    /*如果生成链中没有，则新建*/
                    //puts("build it");
                    pplayer_stat_node_tem=(PLAYER_STAT_NODE*)malloc(sizeof(PLAYER_STAT_NODE));
                    strcpy(pplayer_stat_node_tem->user_name,pplayer_node->user_name);
                    pplayer_stat_node_tem->sum_time=pplayer_node->total_time;
                    pplayer_stat_node_tem->sum_money=pplayer_node->total_money;

                    pplayer_stat_node_tem->next=pplayer_stat_node_ret;/*加入生成链*/
                    pplayer_stat_node_ret=pplayer_stat_node_tem;
                }
                pplayer_node=pplayer_node->next;
            }
            pgame_node=pgame_node->next;
        }
    }
    SortPlayerInfo(pplayer_stat_node_ret);
    return pplayer_stat_node_ret;
}

/**搜索生成链上是否含有此用户名，是返回结点，没有则返回NULL*/
PLAYER_STAT_NODE* Serch_User_Name(PLAYER_STAT_NODE *pplayer_stat_node_ret,char *user_name) {
    PLAYER_STAT_NODE *pserch=pplayer_stat_node_ret;
    if(pserch==NULL) {
        return NULL;
    }
    while(pserch!=NULL) {
        if(strcmp(pserch->user_name,user_name)==0) {
            return pserch;
        }
        pserch=pserch->next;
    }
    return NULL;
}

/**
 * 函数名称: SortPlayerInfo
 * 函数功能: 对玩家信息链进行排序.
 * 输入参数: player_phd 玩家信息链头结点指针
 * 输出参数: player_phd 排序结果同时通过头结点指针返回
 * 返 回 值: 无
 *
 * 调用说明:
 */
void SortPlayerInfo(PLAYER_STAT_NODE *player_phd) {
    PLAYER_STAT_NODE *pplayer_stat_node_piror;
    PLAYER_STAT_NODE *pplayer_stat_node_current;
    PLAYER_STAT_NODE *pplayer_stat_node_after;
    PLAYER_STAT_NODE *pplayer_stat_node_tem;
    pplayer_stat_node_piror=player_phd;
    if(pplayer_stat_node_piror==NULL) { /*空链*/
        return;
    }
    pplayer_stat_node_tem=(PLAYER_STAT_NODE*)malloc(sizeof(PLAYER_STAT_NODE));
    while(pplayer_stat_node_piror->next!=NULL) {
        pplayer_stat_node_current=pplayer_stat_node_piror;
        pplayer_stat_node_after=pplayer_stat_node_piror->next;
        while(pplayer_stat_node_after!=NULL) {
            if(pplayer_stat_node_current->sum_time<pplayer_stat_node_after->sum_time) {
                pplayer_stat_node_current=pplayer_stat_node_after;
            }
            pplayer_stat_node_after=pplayer_stat_node_after->next;
        }
        if(pplayer_stat_node_current!=pplayer_stat_node_piror) {
            /*交换结点中的信息，指针域不变*/
            *pplayer_stat_node_tem=*pplayer_stat_node_piror;
            *pplayer_stat_node_piror=*pplayer_stat_node_current;
            pplayer_stat_node_piror->next=pplayer_stat_node_tem->next;
            pplayer_stat_node_tem->next=pplayer_stat_node_current->next;
            *pplayer_stat_node_current=*pplayer_stat_node_tem;
        }
        pplayer_stat_node_piror=pplayer_stat_node_piror->next;
    }
    free(pplayer_stat_node_tem);
    return;
}
/**
 * 函数名称: StatChargeInfo 数据统计5
 * 函数功能: 统计收费模式信息.
 * 输入参数: hd 主链头结点指针
 * 输出参数: 无
 * 返 回 值: 返回统计结果链头结点地址
 *
 * 调用说明:
 */
CHARGE_NODE *StatChargeInfo(CLASS_NODE *phd) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    CHARGE_NODE *pcharge_node_c,*pcharge_node_p,*pcharge_node_t,*pcharge_node;
    pcharge_node_c=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE)); /*申请存储区*/
    pcharge_node_p=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE));
    pcharge_node_t=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE));
    pcharge_node_c->charge_model='c';   /*赋收费模式*/
    pcharge_node_p->charge_model='p';
    pcharge_node_t->charge_model='t';
    pcharge_node_c->next=pcharge_node_p;    /*设置指向关系*/
    pcharge_node_p->next=pcharge_node_t;
    pcharge_node_t->next=NULL;
    for(pcharge_node=pcharge_node_c; pcharge_node!=NULL; pcharge_node=pcharge_node->next) {
        pcharge_node->game_num=0;   /*赋初值*/
        pcharge_node->playeer_total_time=0; /*赋初值*/
        pcharge_node->player_total_money=0; /*赋初值*/
        pcharge_node->player_total_num=0;   /*赋初值*/
    }
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            switch(pgame_node->charge_model[0]) {
            case 'c':
                /*游戏数据加入结点*/
                Game_Charge(pgame_node,pcharge_node_c);
                break;
            case 'p':
                /*游戏数据加入结点*/
                Game_Charge(pgame_node,pcharge_node_p);
                break;
            case 't':
                /*游戏数据加入结点*/
                Game_Charge(pgame_node,pcharge_node_t);
                break;
            }
            pgame_node=pgame_node->next;
        }
    }
    //SortChargeInfo(pcharge_node_c);
    return pcharge_node_c;/*返回头指针*/
}

/**统计指定游戏的玩家数，玩家总时间和玩家总金额,数据加到对应收费模式的主链结点*/
void Game_Charge(GAME_NODE *game_node, CHARGE_NODE *pcharge) {
    double player_total_num=0;        /*<玩家数*/
    double player_total_time=0;      /*<玩家游戏总时间*/
    double player_total_money=0;      /*<玩家消费总金额*/
    PLAYER_NODE *pplayer_node;
    pplayer_node=game_node->pnext;
    while(pplayer_node!=NULL) {
        player_total_num+=1;    /*玩家数据相加*/
        player_total_money+=pplayer_node->total_money;
        player_total_time+=pplayer_node->total_time;
        pplayer_node=pplayer_node->next;
    }
    pcharge->playeer_total_time+=player_total_time;/*赋值*/
    pcharge->player_total_money+=player_total_money;/*赋值*/
    pcharge->player_total_num+=player_total_num;/*赋值*/
    pcharge->game_num+=1;/*统计游戏款数*/
}
///**
// * 函数名称: SortChargeInfo
// * 函数功能: 对收费模式信息链进行排序.
// * 输入参数: charge_phd 收费模式信息链头结点指针
// * 输出参数: charge_phd 排序结果同时通过头结点指针返回
// * 返 回 值: 无
// *
// * 调用说明:
// */
//void SortChargeInfo(CHARGE_NODE *charge_phd)
//{
//    int i;
//    CHARGE_NODE *pcharge_node_tem;
//    CHARGE_NODE *pcharge_node_piror;
//    CHARGE_NODE *pcharge_node_current;
//    CHARGE_NODE *pcharge_node_after;
//    pcharge_node_piror=charge_phd;
//    if(pcharge_node_piror==NULL)    /*空链*/
//    {
//        return;
//    }
//    pcharge_node_tem=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE));/*临时结点*/
//    while(pcharge_node_piror->next!=NULL)
//    {
//        pcharge_node_current=pcharge_node_piror;
//        pcharge_node_after=pcharge_node_piror->next;
//        while(pcharge_node_after!=NULL)
//        {
//            if(pcharge_node_current->player_total_num<pcharge_node_after->player_total_num)
//            {
//                pcharge_node_current=pcharge_node_after;
//            }
//            pcharge_node_after=pcharge_node_after->next;
//        }
//
//        if(pcharge_node_current!=pcharge_node_piror)
//        {
//            /*交换结点中的信息，指针域不变*/
//            *pcharge_node_tem=*pcharge_node_piror;
//            *pcharge_node_piror=*pcharge_node_current;
//            pcharge_node_piror->next=pcharge_node_tem->next;
//            pcharge_node_tem->next=pcharge_node_current->next;
//            *pcharge_node_current=*pcharge_node_tem;
//        }
//        pcharge_node_piror=pcharge_node_piror->next;
//    }
//    free(pcharge_node_tem);
//    return;
//}

/**
 * 函数名称: SaveSysData
 * 函数功能: 保存三类基础数据.
 * 输入参数: hd 主链头结点指针
 * 输出参数:
 * 返 回 值: BOOL类型, 总是为TRUE
 *
 * 调用说明:
 */
BOOL SaveSysData(CLASS_NODE *hd) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    FILE *pfout;
    int handle;
    pfout=fopen(gp_class_info_filename,"wb");
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*保存游戏类别信息*/
        fwrite(pclass_node,sizeof(CLASS_NODE),1,pfout);
    }
    fclose(pfout);

    pfout=fopen(gp_game_info_filename,"wb");
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*保存游戏基本信息*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            fwrite(pgame_node,sizeof(GAME_NODE),1,pfout);
            pgame_node=pgame_node->next;
        }
    }
    fclose(pfout);

    pfout=fopen(gp_player_info_filename,"wb");
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*保存玩家基本信息*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                fwrite(pplayer_node,sizeof(PLAYER_NODE),1,pfout);
                pplayer_node=pplayer_node->next;
            }
            pgame_node=pgame_node->next;
        }
    }
    fclose(pfout);
    return TRUE;
}

/**
 * 函数名称: BackupSysData
 * 函数功能: 将三类基础数据备份到一个数据文件.
 * 输入参数: hd 主链头结点指针
 *           filename 数据文件名
 * 输出参数:
 * 返 回 值: BOOL类型, 总是为TRUE/
 *
 * 调用说明:
 */
BOOL BackupSysData(CLASS_NODE *hd, char *filename) {
    int i;
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    unsigned long class_node_num=0;
    unsigned long game_node_num=0;
    unsigned long player_node_num=0;
    int handle;
    /*遍历十字链，分别统计三种基础信息的记录总数*/
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        class_node_num++;
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            game_node_num++;
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                player_node_num++;
                pplayer_node=pplayer_node->next;
            }
            pgame_node=pgame_node->next;
        }
    }

    if((handle=open(filename,O_WRONLY|O_BINARY))==-1) {
        handle=open(filename,O_CREAT|O_BINARY,S_IWRITE);
    }
    /*保存三类基础信息的记录总数*/
    write(handle,(char*)&class_node_num,sizeof(class_node_num));
    //printf("%d",i);
    write(handle,(char*)&game_node_num,sizeof(game_node_num));
    //printf("%d",i);
    write(handle,(char*)&player_node_num,sizeof(player_node_num));
    //printf("%d",i);
    //printf("%d%d%d",class_node_num,game_node_num,player_node_num);

    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*保存游戏类别信息*/
        write(handle,(char*)pclass_node,sizeof(CLASS_NODE));
        //printf("%d",i);
    }
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*保存游戏基本信息*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            write(handle,(char*)pgame_node,sizeof(GAME_NODE));
            pgame_node=pgame_node->next;
        }
    }
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*保存玩家基本信息*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                //puts(pplayer_node->user_name);
                write(handle,(char*)pplayer_node,sizeof(PLAYER_NODE));
                pplayer_node=pplayer_node->next;
            }
            pgame_node=pgame_node->next;
        }
    }
    close(handle);
    return TRUE;
}

/**
 * 函数名称: RestoreSysData
 * 函数功能: 从指定数据文件中恢复三类基础数据.
 * 输入参数: phead 主链头结点指针的地址
 *           filename 存放备份数据的数据文件名
 * 输出参数:
 * 返 回 值: BOOL类型, 总是为TRUE
 *
 * 调用说明:输出确认信息，是否恢复？
 */
BOOL RestoreSysData(CLASS_NODE **phead, char *filename) {
    CLASS_NODE *hd=NULL;
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    unsigned long class_node_num=0;
    unsigned long game_node_num=0;
    unsigned long player_node_num=0;
    unsigned long ulloop;
    int handle;
    int find;

    if((handle=open(filename,O_RDONLY|O_BINARY))==-1) {
        handle=open(filename,O_CREAT|O_BINARY,S_IREAD);
    }
    /*读取三种基础信息的记录数*/
    read(handle,(char*)&class_node_num,sizeof(class_node_num));
    read(handle,(char*)&game_node_num,sizeof(game_node_num));
    read(handle,(char*)&player_node_num,sizeof(player_node_num));
    //printf("%d%d%d",class_node_num,game_node_num,player_node_num);
    /*读取游戏类别信息，建立主链*/
    for(ulloop=1; ulloop<=class_node_num; ulloop++) {
        pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
        read(handle,(char*)pclass_node,sizeof(CLASS_NODE));
        pclass_node->gnext=NULL;
        pclass_node->next=hd;
        hd=pclass_node;
    }
    *phead=hd;
    /*读取游戏基本信息，建立游戏信息支链*/
    for(ulloop=1; ulloop<=game_node_num; ulloop++) {
        pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
        read(handle,(char*)pgame_node,sizeof(GAME_NODE));
        //puts(pgame_node->game_name);
        pgame_node->pnext=NULL;
        pclass_node=hd;
        while(pclass_node!=NULL
                &&strcmp(pclass_node->class_id,pgame_node->class_id)!=0) {
            pclass_node=pclass_node->next;
        }

        if(pclass_node!=NULL) {
            pgame_node->next=pclass_node->gnext;
            pclass_node->gnext=pgame_node;
        } else {
            free(pgame_node);
        }
    }
    /*读取玩家基本信息，建立玩家基本信息支链*/
    for(ulloop=1; ulloop<=player_node_num; ulloop++) {
        pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
        read(handle,(char*)pplayer_node,sizeof(PLAYER_NODE));
        pclass_node=hd;
        //puts(pplayer_node->user_name);
        find=0;

        while((pclass_node!=NULL)&&(find==0)) {
            pgame_node=pclass_node->gnext;
            while(pgame_node!=NULL) {
                if(strcmp(pgame_node->game_name,pplayer_node->game_name)==0) {
                    find=1;
                    break;
                }
                pgame_node=pgame_node->next;
            }
            pclass_node=pclass_node->next;
            if(find==1) {
                //puts("lalala");
                break;
            }
        }
        if(find) {
            pplayer_node->next=pgame_node->pnext;
            pgame_node->pnext=pplayer_node;
        } else {
            free(pplayer_node);
        }
    }
    close(handle);
    //SaveSysData(hd);    /*将内存中数据保存到数据文件*/
    return TRUE;
}

/**弹出窗口的建立*/
BOOL ShowModule(char **pString, int n) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;
    /*找出最长的字符串*/
    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6; /*空出弹出框的左右间隔*/
    pos.Y = n + 5;  /*画横线和上下留白*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*确认弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;//| BACKGROUND_RED;  /*白底黑字*/
    labels.num = n; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[n]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/

    for (i=0; i<n; i++) {
        aLoc[i].X = rcPop.Left + 3; /*给坐标数组赋值*/
        aLoc[i].Y = rcPop.Top + 2 + i;

    }
    str_len = strlen(pString[n-1]); /*最后一个字符串（取消）的位置，在中间*/
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[n-1].X, aLoc[n-1].Y,
            aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}
/**包含说明的弹出窗口的建立*/
BOOL ShowModuleWithExp(char **pString, int n) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;
    /*找出最长的字符串*/
    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6+10; /*空出弹出框的左右间隔*/
    pos.Y = (n-1)/2 + 5;  /*+5表示画横线和上下留白*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*确认弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;//| BACKGROUND_RED;  /*白底黑字*/
    labels.num = n; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[n]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/

    for (i=0; i<n; i+=2) {
        aLoc[i].X = rcPop.Left + 3; /*给坐标数组赋值*/
        aLoc[i].Y = rcPop.Top + 2 + i/2;
    }
    for (i=1; i<n; i+=2) {
        aLoc[i].X = rcPop.Left + 3+11; /*给坐标数组赋值,+11表示前面的说明部分*/
        aLoc[i].Y = rcPop.Top +2+ (i-1)/2;/*+1表示要和前面的说明对齐*/
    }
    str_len = strlen(pString[n-1]); /*最后一个字符串（取消）的位置，在中间*/
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2+3;/*+5表示前面的说明*/
    aLoc[n-1].Y = aLoc[n-1].Y + 1;

    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[n-1].X, aLoc[n-1].Y,
            aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 +(n-1)/2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}

/**确认或者取消弹出窗口的建立*/
int ShowModuleYON(char **pString) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, str_len;
    int result=0;/*返回用户的选择，1为确定，0为取消*/
    pos.X = strlen(pString[0]) + 6; /*空出弹出框的左右间隔*/
    pos.Y = 7;  /*画横线和上下留白*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*确认弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*白底黑字*/
    labels.num = 2; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[]= {{rcPop.Left+4,rcPop.Top+2},
        {rcPop.Left+3,rcPop.Top+4},
        {rcPop.Left+9,rcPop.Top+4}
    }; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 2;  /*两个热区*/
    SMALL_RECT aArea[] = {{
            rcPop.Left+3,rcPop.Top+4,
            rcPop.Left+6,rcPop.Top+4
        },
        {
            rcPop.Left+9,rcPop.Top+4,
            rcPop.Left+12,rcPop.Top+4
        }
    };/*热区定位*/

    char aSort[] = {0,0};/*热区类型*/
    char aTag[] = {1,2};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 3;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        result=1;
    } else {
        result=0;
    }
    return result;

}

/**警告弹出窗口的建立*/
BOOL HShowModule(char **pString, int n) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;
    /*找出最长的字符串*/
    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6; /*空出弹出框的左右间隔*/
    pos.Y = n + 3;  /*画横线和上下留白*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*确认弹出框的位置*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1是因为开始坐标为0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2-8;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_GREEN|BACKGROUND_RED ;
    labels.num = n; /*标签束中标签字符串的个数*/
    labels.ppLabel = pString; /*标签束第一个标签字符串的地址*/
    COORD aLoc[n]; /*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/

    for (i=0; i<n; i++) {
        aLoc[i].X = rcPop.Left + 3; /*给坐标数组赋值*/
        aLoc[i].Y = rcPop.Top + 1 + i;

    }
    str_len = strlen(pString[n-1]); /*最后一个字符串（取消）的位置，在中间*/
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 1;

    labels.pLoc = aLoc;  /*使标签束结构变量labels的成员ploc指向坐标数组的首元素*/

    areas.num = 1;  /*只有一个热区*/
    SMALL_RECT aArea[] = {{
            aLoc[n-1].X, aLoc[n-1].Y,
            aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };/*热区定位，+3为两个汉字的长度*/

    char aSort[] = {0};/*热区类型*/
    char aTag[] = {1};/*热区编号*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*画横线分隔*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}

void Move(HOT_AREA *pHotArea, int *piHot,char asc,char vkc) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/

    if (asc == 0) {
        arrow = 0;
        switch (vkc) {
        /*方向键(左、上、右、下)的处理*/
        case 37:
            arrow = 1;
            break;
        case 38:
            arrow = 2;
            break;
        case 39:
            arrow = 3;
            break;
        case 40:
            arrow = 4;
            break;
        }
        if (arrow > 0) {
            num = *piHot;
            while (TRUE) {
                if (arrow < 3) {
                    num--;
                } else {
                    num++;
                }
                if ((num < 1) || (num > pHotArea->num) ||
                        ((arrow % 2) && (pHotArea->pArea[num-1].Top
                                         == pHotArea->pArea[*piHot-1].Top)) || ((!(arrow % 2))
                                                 && (pHotArea->pArea[num-1].Top
                                                     != pHotArea->pArea[*piHot-1].Top))) {
                    break;
                }
            }
            if (num > 0 && num <= pHotArea->num) {
                *piHot = num;
                SetHotPoint(pHotArea, *piHot);
            }
        }
    }
    return;
}
