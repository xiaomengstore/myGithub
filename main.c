#include "game.h"

unsigned long ul;

int main() {
    COORD size = {SCR_COL, SCR_ROW};              /*���ڻ�������С*/
    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/

    SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/
    LoadData(&gp_head);                   /*���ݼ���*/
    InitInterface();          /*�����ʼ��*/
    RunSys(&gp_head);             /*ϵͳ����ģ���ѡ������*/
    CloseSys(gp_head);            /*�˳�ϵͳ*/

    return 0;
}
/**
 * ��������: LoadData
 * ��������: ������������ݴ������ļ����뵽�ڴ滺������ʮ��������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, ���ܺ����г��˺���ExitSys�ķ���ֵ����ΪFALSE��,
 *           ���������ķ���ֵ����ΪTRUE.
 *
 * ����˵��: Ϊ���ܹ���ͳһ�ķ�ʽ���ø����ܺ���, ����Щ���ܺ�����ԭ����Ϊ
 *           һ��, ���޲����ҷ���ֵΪBOOL. ����ֵΪFALSEʱ, ������������.
 */
BOOL LoadData(CLASS_NODE** gp_head) {
    int Re = 0;
    Re = CreatList(gp_head);
    gc_sys_state |= Re;
    gc_sys_state &= ~(4 + 8 + 16 - Re);
    if (gc_sys_state < ( 4 | 8 | 16)) {
        /*���ݼ�����ʾ��Ϣ*/
        printf("\nϵͳ�������ݲ�����!\n");
        printf("\n�����������...\n");
        getchar();
    }

    return TRUE;
}

/**
 * ��������: CreatList
 * ��������: �������ļ���ȡ��������, ����ŵ���������ʮ��������.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ, ����������������ʮ����.
 * �� �� ֵ: int����ֵ, ��ʾ�����������.
 *           0  ����, ������
 *           4  �Ѽ�����Ϸ�����Ϣ���ݣ�����Ϸ������Ϣ����һ�����Ϣ����
 *           12 �Ѽ�����Ϸ�����Ϣ����Ϸ������Ϣ���ݣ�����һ�����Ϣ����
 *           28 ����������ݶ��Ѽ���
 *
 * ����˵��:
 */
int CreatList(CLASS_NODE **phead) {
    CLASS_NODE *hd = NULL, *pClassNode, tmp1;
    GAME_NODE *pGameNode, tmp2;
    PLAYER_NODE *pPlayerNode, tmp3;
    FILE *pFile;
    int find;
    int re = 0;

    if ((pFile = fopen(gp_class_info_filename, "rb")) == NULL) {
        printf("��Ϸ��������ļ���ʧ��!\n");
        return re;
    }
    printf("��Ϸ��������ļ��򿪳ɹ�!\n");

    /*�������ļ��ж���Ϸ�����Ϣ���ݣ������Ժ���ȳ���ʽ������������*/
    while (fread(&tmp1, sizeof(CLASS_NODE), 1, pFile) == 1) {
        pClassNode = (CLASS_NODE *)malloc(sizeof(CLASS_NODE));
        *pClassNode = tmp1;
        pClassNode->gnext = NULL;
        pClassNode->next = hd;
        hd = pClassNode;
    }
    fclose(pFile);
    if (hd == NULL) {
        printf("��Ϸ������Ϣ�����ļ�����ʧ��!\n");
        return re;
    }
    printf("��Ϸ������Ϣ�����ļ����سɹ�!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_game_info_filename, "rb")) == NULL) { /*ע���ļ����ĸı�*/
        printf("��Ϸ������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("��Ϸ������Ϣ�����ļ��򿪳ɹ�!\n");
    re += 8;

    /*�������ļ��ж�ȡ��Ϸ������Ϣ���ݣ�����������Ӧ������Ϸ������Ϣ֧����*/
    while (fread(&tmp2, sizeof(GAME_NODE), 1, pFile) == 1) {
        /*������㣬��Ŵ������ļ��ж�������Ϸ������Ϣ*/
        pGameNode = (GAME_NODE *)malloc(sizeof(GAME_NODE));
        *pGameNode = tmp2;
        pGameNode->pnext = NULL;

        /*�������ϲ��Ҹ���Ϸ������Ϸ����Ӧ���������*/
        pClassNode = hd;
        while (pClassNode != NULL
                && strcmp(pClassNode->class_id, pGameNode->class_id) != 0) {
            pClassNode = pClassNode->next;
        }
        if (pClassNode != NULL) { /*����ҵ����򽫽���Ժ���ȳ���ʽ������Ϸ������Ϣ֧��*/
            pGameNode->next = pClassNode->gnext;
            pClassNode->gnext = pGameNode;
        } else { /*���δ�ҵ������ͷ������������ڴ�ռ�*/
            free(pGameNode);
        }
    }
    fclose(pFile);
    if ((pFile = fopen(gp_player_info_filename, "rb")) == NULL) {
        printf("��һ�����Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("��һ�����Ϣ�����ļ��򿪳ɹ�!\n");
    re += 16;

    /*�������ļ��ж�ȡ��һ�����Ϣ���ݣ�������Ϸ������Ϣ֧����Ӧ������һ�����Ϣ֧����*/
    while (fread(&tmp3, sizeof(PLAYER_NODE), 1, pFile) == 1) {
        /*������㣬��Ŵ������ļ��ж�������һ�����Ϣ*/
        pPlayerNode = (PLAYER_NODE *)malloc(sizeof(PLAYER_NODE));
        *pPlayerNode = tmp3;

        /*������Ϸ������Ϣ֧���϶�Ӧ��Ϸ������Ϣ���*/
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
        if (find) { /*����ҵ����򽫽���Ժ���ȳ���ʽ������һ�����Ϣ֧����*/
            pPlayerNode->next = pGameNode->pnext;
            pGameNode->pnext = pPlayerNode;
        } else { /*���δ�ҵ������ͷ������������ڴ�ռ�*/
            free(pPlayerNode);
        }
    }
    fclose(pFile);
    return re;
}

/**
 * ��������: InitInterface
 * ��������: ��ʼ������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InitInterface() {
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE;  /*��ɫǰ������ɫ����*/
    SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

    ClearScreen();  /* ����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*��ʾ�˵���*/
    ShowState();    /*��ʾ״̬��*/

    return;
}

/**
 * ��������: ClearScreen
 * ��������: �����Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen(void) {
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*ȡ��Ļ��������Ϣ*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

    /*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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
    for (i=0; i < 5; i++) { /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0} ;

    /*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*����һ����Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
    for (i=0; i<size.X; i++) {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            (gp_buff_menubar_info+i)->Attributes = FOREGROUND_RED;
        }
    }

    /*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

    /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
    i = 0;
    do {
        PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
        for (j=PosA; j<PosB; j++) {
            gp_scr_att[j] |= (i+1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
        }
        PosA = PosB + 4;
        i++;
    } while (i<5);

    TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

    return;
}

/**
 * ��������: ShowState
 * ��������: ��ʾ״̬��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
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
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num) {
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 2, PosB;
    char ch;
    int i;

    if (num == 0) { /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
        PosA = 0;
        PosB = 0;
    } else { /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
        for (i=1; i<num; i++) {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]);
    }

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;

    /*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
    for (i=0; i<PosA; i++) {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*��ѡ�в˵���������ǣ��ڵװ���*/
    for (i=PosA; i<PosB; i++) {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED;
    }

    /*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
    for (i=PosB; i<bInfo.dwSize.X; i++) {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
    SMALL_RECT rcMenu = {0, 0, size.X-1, 0};        /*�����ַ���Ϣ���ڴ����е����λ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: CloseSys
 * ��������: �ر�ϵͳ.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void CloseSys(CLASS_NODE *hd) {
    CLASS_NODE *pClassNode1= hd, *pClassNode2;
    GAME_NODE *pGameNode1, *pGameNode2;
    PLAYER_NODE *pPlayerNode1, *pPlayerNode2;

    while (pClassNode1!= NULL) { /*�ͷ�ʮ�ֽ�������Ķ�̬�洢��*/
        pClassNode2 = pClassNode1->next;
        pGameNode1 = pClassNode1->gnext;
        while (pGameNode1 != NULL) { /*�ͷ�ѧ��������Ϣ֧���Ķ�̬�洢��*/
            pGameNode2 = pGameNode1->next;
            pPlayerNode1 = pGameNode1->pnext;
            while (pPlayerNode1 != NULL) { /*�ͷŽɷ���Ϣ֧���Ķ�̬�洢��*/
                pPlayerNode2 = pPlayerNode1->next;
                free(pPlayerNode1);
                pPlayerNode1 = pPlayerNode2;
            }
            free(pGameNode1);
            pGameNode1 = pGameNode2;
        }
        free(pClassNode1);  /*�ͷ��������Ķ�̬�洢��*/
        pClassNode1 = pClassNode2;
    }

    ClearScreen();        /*����*/

    /*�ͷŴ�Ų˵�����״̬����Ϣ��̬�洢��*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*�رձ�׼���������豸���*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*�����ڱ�������Ϊ���н���*/
    SetConsoleTitle("���н���");

    return;
}

/**
 * ��������: RunSys
 * ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void RunSys(CLASS_NODE **phead) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (bRet) {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) { /*�����¼������¼�����*/
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
            if (cNo == 0) { /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                /* cAtt > 0 ������λ�ô�������(���˵����ַ���Ԫ)
                 * cAtt != gi_sel_menu ������λ�õ����˵���δ��ѡ��
                 * gp_top_layer->LayerNo > 0 ������ǰ���Ӳ˵�����
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0) {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
                }
            } else if (cAtt > 0) { /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
                TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                    == FROM_LEFT_1ST_BUTTON_PRESSED) { /*������������ߵ�һ��*/
                if (cNo == 0) { /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                    if (cAtt > 0) { /*�����λ�ô�������(���˵����ַ���Ԫ)*/
                        PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
                    }
                    /*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
                    else if (gp_top_layer->LayerNo > 0) {
                        PopOff();            /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    }
                } else { /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
                    if (cAtt > 0) { /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
                        PopOff(); /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

                        /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����,cAtt�Ӳ˵����*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            } else if (inRec.Event.MouseEvent.dwButtonState
                       == RIGHTMOST_BUTTON_PRESSED) { /*�����������Ҽ�*/
                if (cNo == 0) { /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
            }
        } else if (inRec.EventType == KEY_EVENT /*�����¼�ɰ�������*/
                   && inRec.Event.KeyEvent.bKeyDown) { /*�Ҽ�������*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            /*ϵͳ��ݼ��Ĵ���*/
            if (vkc == 112) { /*�������F1��*/
                if (gp_top_layer->LayerNo != 0) { /*�����ǰ���Ӳ˵�����*/
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
                bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
            } else if (inRec.Event.KeyEvent.dwControlKeyState
                       & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
                /*������������Alt��*/
                switch (vkc) { /*�ж���ϼ�Alt+��ĸ*/
                case 88:  /*Alt+X �˳�*/
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
            } else if (asc == 0) { /*�������Ƽ��Ĵ���*/
                if (gp_top_layer->LayerNo == 0) { /*���δ�����Ӳ˵�*/
                    switch (vkc) { /*�������(���ҡ���)������Ӧ�������Ƽ�*/
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
                } else { /*�ѵ����Ӳ˵�ʱ*/
                    for (loc=0,i=1; i<gi_sel_menu; i++) {
                        loc += ga_sub_menu_count[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) { /*�����(���ҡ��ϡ���)�Ĵ���*/
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
            } else if ((asc-vkc == 0) || (asc-vkc == 32)) { /*������ͨ��*/
                if (gp_top_layer->LayerNo == 0) { /*���δ�����Ӳ˵�*/
                    switch (vkc) {
                    case 70: /*f��F*/
                        PopMenu(1);
                        break;
                    case 77: /*m��M*/
                        PopMenu(2);
                        break;
                    case 81: /*q��Q*/
                        PopMenu(3);
                        break;
                    case 83: /*s��S*/
                        PopMenu(4);
                        break;
                    case 72: /*h��H*/
                        PopMenu(5);
                        break;
                    case 13: /*�س�*/
                        PopMenu(gi_sel_menu);
                        TagSubMenu(1);
                        break;
                    }
                } else { /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                    if (vkc == 27) { /*�������ESC��*/
                        PopOff();
                        gi_sel_sub_menu = 0;
                    } else if(vkc == 13) { /*������»س���*/
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    } else { /*������ͨ���Ĵ���*/
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++) {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++) {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1]) { //[C] ��Ϸ������Ϣ����������ĵڶ���Ԫ��
                                /*���ƥ��ɹ�*/
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
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopMenu(int num) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu) {     /*���ָ�����˵�������ѡ�в˵�*/
        if (gp_top_layer->LayerNo != 0) { /*�����ǰ�����Ӳ˵�����*/
            PopOff();
            gi_sel_sub_menu = 0;
        }
    } else if (gp_top_layer->LayerNo != 0) { /*���ѵ������Ӳ˵����򷵻�*/
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++) {
        loc += ga_sub_menu_count[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) { /*ȷ����ǩ�ַ��������λ�ã����������������*/
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++) {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2; /*+2��ʾ������Χ*/
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++) {
        /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) { /*����Ϊ0������Ϊ�մ�*/
            /*���Ȼ�����*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++) {
                /*ȡ���������ַ���Ԫ����������*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*��λ��Ľ�������˵���λ*/
            }
        }

    }
    /*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++) {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0) {
            continue;  /*�����մ�*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea) {
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++) {
        /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++) {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++) {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++) {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0) {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }
    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++) {
        /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++) {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++) {
        /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++) {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                         | (pHotArea->pTag[i] << 2);
        }
    }
    return;
}

/**
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopOff(void) {
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL)) {
        /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*�ָ�������������ԭ���*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*�ָ��ַ���Ԫԭ����*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++) {
        //Sleep(1);
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++) {
            //Sleep(1);
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc) {
    char chBox[] = {'+','-','|'};  /*�����õ��ַ�*/
    COORD pos = {pRc->Left, pRc->Top};  /*��λ����������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++) {
        /*��ѭ�����ϱ߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++) {
        /*��ѭ�����߿�����ߺ��ұ���*/
        //Sleep(10);
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++) {
        /*���±߿����*/
        //Sleep(10);
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    return;
}

/**v
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagSubMenu(int num) {
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1)) {
        /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) { /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*���ƶ��Ӳ˵�������ѡ�б��*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc) {
    int i, len, loc = 0;

    rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
    rc->Left = 1;
    for (i=1; i<num; i++) {
        /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++) {
        /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len) {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*����������ұ߽�*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*���������±ߵ��к�*/
    if (rc->Right >= SCR_COL) { /*�ұ߽�Խ��Ĵ���*/
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * ��������: DealInput
 * ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
 * �������: pHotArea
 *           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
 * �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
 * �� �� ֵ:
 *
 * ����˵��:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
                (inRec.Event.MouseEvent.dwButtonState   /*�����¼������¼�����*/
                 == FROM_LEFT_1ST_BUTTON_PRESSED)) {
            pos = inRec.Event.MouseEvent.dwMousePosition;       /*���λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;      /*ȡ��λ�ò��*/
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
        } else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;/*vkc:���������, asc:�ַ���ASCII��ֵ*/

            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                arrow = 0;
                switch (vkc) {
                /*�����(���ϡ��ҡ���)�Ĵ���*/
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
                /*ESC��*/
                iRet = 27;
                break;
            } else if (vkc == 13 || vkc == 32) {
                /*�س�����ո��ʾ���µ�ǰ��ť*/
                iRet = 13;
                break;
            }
        }
    }
    return iRet;
}
/**��ihot��Ӧ������ѡ��*/
void SetHotPoint(HOT_AREA *pHotArea, int iHot) {
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att,att1, att2;
    int i, width;

    att=BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;  /*�׵׺��֣��ı���*/
    att1 =  FOREGROUND_BLUE | FOREGROUND_GREEN|FOREGROUND_RED ;  /*�ڵװ��֣���ť��ѡ��*/
    att2 =BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*��׺��֣����������ɫ*/
    for (i=0; i<pHotArea->num; i++) {
        /*����ť��������Ϊ��׺���*//*��ѭ���������ж�û�б�ѡ��*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0) {
            /*�����ǰ�ť��*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
            if(pHotArea->pArea[i].Bottom!=pHotArea->pArea[i].Top) { /*��������е�����*/
                /*��������е�����*/
                pos.Y = pHotArea->pArea[i].Top+1;
                FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
            }
        }
    }
    /*��ihot��Ӧ������ѡ��*/
    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0) {
        /*�����������ǰ�ť��*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
        if(pHotArea->pArea[iHot-1].Bottom!=pHotArea->pArea[iHot-1].Top) {
            /*��������е�����*/
            pos.Y = pHotArea->pArea[iHot-1].Top+1;
            FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
        }
    } else if (pHotArea->pSort[iHot-1] == 1) {
        /*�������������ı�����*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE �� FALSE
 *
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
 */
BOOL ExeFunction(int m, int s) {
    BOOL bRet = TRUE;
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
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

    for (i=1,loc=0; i<m; i++) { /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL) {
        bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
    }

    return bRet;
}

BOOL SaveData(void) {
    char* pString[2]= {"ȷ�ϱ��棿",
                       "ȷ��  ȡ��"
                      };
    if(ShowModuleYON(pString)==1) {
        PopOff();
        SaveSysData(gp_head);
        char *plabel_name[] = {"����ɹ�",
                               "ȷ��"
                              };
        ShowModule(plabel_name, 2);
    } else {
        PopOff();
    }
    return TRUE;
}

BOOL BackupData(void) {
    char* pString[2]= {"ȷ�ϱ��ݣ�",
                       "ȷ��  ȡ��"
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
        char *plabel_name[] = {"���ݱ�����",
                               string,
                               "ȷ��"
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
    char* pString[2]= {"ȷ���˳���",
                       "ȷ��  ȡ��"
                      };
    if(ShowModuleYON(pString)==1) {
        PopOff();
        return FALSE;/*��ʾ�˳�*/
    } else {
        PopOff();
        return TRUE;
    }
}

/**���ݻָ�������*/
void VRestoreData() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"�����뱸�ݵ������ļ���","�磺Aug 27.dat",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+6, rcPop.Top+2},
        {rcPop.Left+5, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    char filename[11];
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    if(m==0) {
                        char* plabel_name[2]= {"�������ļ�����",
                                               "ȷ��"
                                              };
                        ShowModule(plabel_name, 2);
                        break;
                    }
                    int handle;
                    if((handle=open(filename,O_RDONLY|O_BINARY))==-1) {
                        PopOff();
                        char* plabel_name[2]= {"�ļ�������~",
                                               "ȷ��"
                                              };
                        ShowModule(plabel_name, 2);
                        break;
                    }

                    PopOff();
                    char* pString[2]= {"ȷ�ϻָ���",
                                       "ȷ��  ȡ��"
                                      };
                    if(ShowModuleYON(pString)==1) {
                        PopOff();
                        RestoreSysData(&gp_head,filename);
                        char *plabel_name[] = {"�ָ��ɹ�",
                                               "ȷ��"
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
                /*����backspace*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**����ά��*/
/**ά����Ϸ�����Ϣ*/
BOOL MaintainClassInfo(void) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"��Ϸ�����Ϣ¼��","��Ϸ�����Ϣɾ��","��Ϸ�����Ϣ�޸�",
                   "ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;                    /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc;                     /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/

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

/**¼����Ϸ�����Ϣ������*/
void VInsertClassNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"��Ϸ�����Ϣ¼��","������","�������",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 4;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    pos.X=rcPop.Left+10;
    pos.Y=rcPop.Top+4;/*���������Ϣ������*/
    SetHotPoint(&areas, iHot);
    CLASS_NODE *pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top + 4};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==3) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù��bu�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pclass_node->next=NULL;
                    pclass_node->gnext=NULL;
                    if(m==0||n==0) {
                        char *q[]= {"���ݲ���Ϊ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    if(SeekClassNodeById(gp_head,pclass_node->class_id)!=NULL) {
                        char* p[]= {"����Ѵ���~","�޸�  ȡ��"};
                        if(ShowModuleYON(p,2)) {
                            PopOff();
                            VVModifClassNode(pclass_node);
                            break;
                        } else
                            break;
                    }
                    if(InsertClassNode(&gp_head,pclass_node)) {
                        char* p[]= {"����ɹ�","ȷ��"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"����ʧ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**ɾ����Ϸ�����Ϣ������*/
void VDelClassNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"��������Ϸ������","������",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+8;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    class_node=SeekClassNodeById(gp_head,pclass_node->class_id);
                    if(class_node!=NULL) {
                        char *plabel_name[2]= {"ȷ��ɾ����","ȷ��  ȡ��"};
                        if(ShowModuleYON(plabel_name)) {
                            DelClassNode(&gp_head,pclass_node->class_id);
                            PopOff();
                            char *p[2]= {"ɾ���ɹ�","ȷ��"};
                            ShowModule(p,2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ������","ȷ��"};
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
                /*����backspace*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**�޸���Ϸ�����Ϣ������*/
void VModifClassNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"��������Ϸ������","������",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+8;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    class_node=SeekClassNodeById(gp_head,pclass_node->class_id);
                    if(class_node!=NULL) {
                        VVModifClassNode(class_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ������","ȷ��"};
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
                /*����backspace*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**�޸���Ϸ�����Ϣ�Ķ����ݵ�����*/
void VVModifClassNode(CLASS_NODE *class_node) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"�޸���Ϸ�����Ϣ","������","�������",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵����������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
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
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù��bu�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    if(n==0) {
                        char *q[]= {"���ݲ���Ϊ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    if(ModifClassNode(&gp_head,pclass_node->class_id,pclass_node)) {
                        char* p[]= {"�޸ĳɹ�","ȷ��"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"�޸�ʧ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==3) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**ά����Ϸ������Ϣ*/
BOOL MaintainGameInfo(void) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"��Ϸ������Ϣ¼��","��Ϸ������Ϣɾ��","��Ϸ������Ϣ�޸�",
                   "ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;                    /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc;                     /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/

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

/**¼����Ϸ������Ϣ������*/
void VInsertGameNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"��Ϸ������Ϣ¼��","��Ϸ���","��Ϸ����","��Ϸ��ַ","�շ�ģʽ",
                   "������ʽ"," ȷ��       ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+30;
    pos.Y = 11;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 7; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+9}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 7;       /*�����ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 8;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    GAME_NODE *pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m1=0,m2=0,m3=0,m4=0,m5=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top + 4},
          pos3= {rcPop.Left + 10, rcPop.Top +5},pos4= {rcPop.Left + 10, rcPop.Top +6},
          pos5= {rcPop.Left + 10, rcPop.Top +7};
    pos.X=pos2.X;
    pos.Y=pos2.Y;
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==6) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù��bu�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pgame_node->next=NULL;
                    pgame_node->pnext=NULL;
                    int icharge_model=0,irank_way=0;
                    icharge_model=pgame_node->charge_model[0]=='c'||pgame_node->charge_model[0]=='p'
                                  ||pgame_node->charge_model[0]=='t';
                    irank_way=pgame_node->rank_way[0]=='p'||pgame_node->rank_way[0]=='t';
                    if(m1==0||m2==0||m3==0||icharge_model==0||irank_way==0) {
                        char *q[]= {"���ݲ���ȷ!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    if(SeekGameNode(gp_head,pgame_node->game_name)!=NULL) {
                        char* p[]= {"��Ϸ�Ѵ���~","�޸�  ȡ��"};
                        if(ShowModuleYON(p,2)) {
                            PopOff();
                            VVModifGameNode(pgame_node);
                            break;
                        } else
                            break;
                    }
                    if(InsertGameNode(gp_head,pgame_node)) {
                        char* p[]= {"����ɹ�","ȷ��"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"�Ҳ�����Ϸ���!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==7) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**ɾ����Ϸ������Ϣ������*/
void VDelGameNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"ɾ����Ϸ������Ϣ","��Ϸ����",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1=BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNode(gp_head,pgame_node->game_name);
                    if(game_node!=NULL) {
                        char *plabel_name[2]= {"ȷ��ɾ����","ȷ��  ȡ��"};
                        if(ShowModuleYON(plabel_name)) {
                            DelGameNode(gp_head,pgame_node->game_name);
                            PopOff();
                            char *p[2]= {"ɾ���ɹ�","ȷ��"};
                            ShowModule(p,2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ�����Ϸ","ȷ��"};
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
                /*����backspace*/
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
                if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**�޸���Ϸ������Ϣ������*/
void VModifGameNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"�޸���Ϸ������Ϣ","��Ϸ����",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1=BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNode(gp_head,pgame_node->game_name);
                    if(game_node!=NULL) {
                        VVModifGameNode(game_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ�����Ϸ","ȷ��"};
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
                /*����backspace*/
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
                if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**�޸���Ϸ������Ϣ�Ķ����ݵ�����*/
void VVModifGameNode(GAME_NODE* game_node) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    char *pCh[] = {"��Ϸ������Ϣ�޸�","��Ϸ���","��Ϸ����","��Ϸ��ַ","�շ�ģʽ",
                   "������ʽ"," ȷ��       ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+30;
    pos.Y = 11;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 7; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+9}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 5;       /*�����ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 8;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
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
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==4) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù��bu�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();

                    int icharge_model=0,irank_way=0;
                    icharge_model=pgame_node->charge_model[0]=='c'||pgame_node->charge_model[0]=='p'
                                  ||pgame_node->charge_model[0]=='t';
                    irank_way=pgame_node->rank_way[0]=='p'||pgame_node->rank_way[0]=='t';
                    if(m3==0||icharge_model==0||irank_way==0) {
                        char *q[]= {"���ݲ���ȷ!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    if(ModifGameNode(gp_head,pgame_node->game_name,pgame_node)) {
                        char* p[]= {"�޸ĳɹ�","ȷ��"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"�޸�ʧ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==5) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
                switch(iHot) {
                case 1:/*����Ϊ���Ϊ1�����Ƕ�Ӧ���ַ����ǵ�����*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**ά�������Ϣ*/
BOOL MaintainPlayerInfo(void) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"��һ�����Ϣ¼��","��һ�����Ϣɾ��","��һ�����Ϣ�޸�",
                   "ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;                    /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc;                     /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/

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

/**¼����һ�����Ϣ������*/
void VInsertPlayerNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0},pos_user,pos_game;
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    char *pCh[] = {"��һ�����Ϣ¼��","�û���","��Ϸ����","ע������","��Ϸ�ۼ�ʱ��",
                   "�����ܽ��","����ָ��"," ȷ��       ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+25;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 8; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+8},
        {rcPop.Left+2, rcPop.Top+10}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 8;       /*�����ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 9;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
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
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==7) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù��bu�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pplayer_node->next=NULL;
                    if(m1==0||m2==0||m3==0||m4==0||m5==0||m6==0) {
                        char *q[]= {"���ݲ���Ϊ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    if(m3!=8) {
                        char *plabel_name[2]= {"ע�����ڲ��Ϸ���","ȷ��"};
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
                        char *q[]= {"�Ҳ�������ݻ��·�!","ȷ��"};
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
                        char *q[]= {"������������!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    if(SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name)!=NULL) {
                        char* p[]= {"����Ѵ���~","�޸�  ȡ��"};
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
                        char *q[]= {"�Ҳ�������Ϸ!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    if(InsertPlayerNode(gp_head,pplayer_node)) {
                        char* p[]= {"����ɹ�","ȷ��"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"����ʧ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==8) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**ɾ����һ�����Ϣ������*/
void VDelPlayerNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"ɾ����һ�����Ϣ","�û���",
                   "��Ϸ����"," ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 4;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top +4};
    COORD pos_user= {rcPop.Left + 10, rcPop.Top +3},pos_game= {rcPop.Left + 10, rcPop.Top +4};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                    if(player_node!=NULL) {
                        char *plabel_name[2]= {"ȷ��ɾ����","ȷ��  ȡ��"};
                        if(ShowModuleYON(plabel_name)) {
                            DelPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                            PopOff();
                            char *p[2]= {"ɾ���ɹ�","ȷ��"};
                            ShowModule(p,2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ������","ȷ��"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**�޸���һ�����Ϣ������*/
void VModifPlayerNode(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"�޸���һ�����Ϣ","�û���",
                   "��Ϸ����"," ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 4;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top +4};
    COORD pos_user= {rcPop.Left + 10, rcPop.Top +3},pos_game= {rcPop.Left + 10, rcPop.Top +4};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                    if(player_node!=NULL) {
                        VVModifPlayerNode(player_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ������","ȷ��"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**�޸���һ�����Ϣ�Ķ����ݵ�����*/
void VVModifPlayerNode(PLAYER_NODE* player_node) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    char *pCh[] = {"��һ�����Ϣ�޸�","�û���","��Ϸ����","ע������","��Ϸ�ۼ�ʱ��",
                   "�����ܽ��","����ָ��"," ȷ��       ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+25;
    pos.Y = 12;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 8; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+6},
        {rcPop.Left+2, rcPop.Top+7},
        {rcPop.Left+2, rcPop.Top+8},
        {rcPop.Left+2, rcPop.Top+10}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 6;       /*�����ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 9;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
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
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==5) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù��bu�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    pplayer_node->total_money=atof(pplayer_node->total_moneyc);
                    pplayer_node->total_time=atof(pplayer_node->total_timec);
                    if(ModifPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name,pplayer_node)) {
                        char* p[]= {"�޸ĳɹ�","ȷ��"};
                        ShowModule(p,2);
                        break;
                    }

                    else {
                        char *q[]= {"�޸�ʧ��!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==6) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                    if(asc>=48&&asc<=57) { /*ֻ����������*/
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
                    if(asc>=46&&asc<=57&&asc!=47) { /*ֻ�����븡����������*/
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
                    if(asc>=46&&asc<=57&&asc!=47) { /*ֻ�����븡����������*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}
/**��ѯ*/
/**��Ϸ�����Ϣ��ѯ*/
BOOL QueryClassInfo(void) {
    BOOL bRet=TRUE;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"�����������","�������������","ȡ��"};
    int iHot=1;
    int i, j, loc = 0;
    pos.X = strlen(pCh[0])+4;
    pos.Y = 6;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;                    /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+7, rcPop.Top+4}
    }; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc;                     /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 3;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
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
/**ById��Ϸ�����Ϣ��ѯ������*/
void VQueryClassInfoById() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"��Ϸ�����Ϣ��ѯById","������",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    //WriteConsoleOutputCharacter(gh_std_out, id,3,pos1, &ul);
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    //puts(pclass_node->class_id);
                    class_node=SeekClassNodeById(gp_head,pclass_node->class_id);
                    if(class_node!=NULL) {
                        char *plabel_name[5]= {"������",class_node->class_id,"�������",class_node->class_dec,"ȷ��"};
                        ShowModuleWithExp(plabel_name, 5);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ������","ȷ��"};
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
                /*����backspace*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**ByDec��Ϸ�����Ϣ��ѯ������*/
void VQueryClassInfoByDec() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"��Ϸ�����Ϣ��ѯByDec","�������",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+8;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    CLASS_NODE* pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    CLASS_NODE* class_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
                    class_node=SeekClassNodeByDec(gp_head,pclass_node->class_dec);
                    if(class_node!=NULL) {
                        char *plabel_name[5]= {"������",class_node->class_id,"�������",class_node->class_dec,"ȷ��"};
                        ShowModuleWithExp(plabel_name, 5);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ������","ȷ��"};
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
                /*����backspace*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}


/**��Ϸ������Ϣ��ѯ*/
BOOL QueryGameInfo(void) {
    BOOL bRet = TRUE;
    char *pCh[] = { "����Ϸ���Ʋ���",
                    "����Ϸ����շ�ģʽ",
                    "��������ʽ����",
                    "ȡ��"
                  };
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot=1;
    int i, j, loc = 0;
    pos.X = strlen(pCh[1])+2;/*pch[1]����Ϊ�ڶ����ַ����*/
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;                    /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ��������ĸ�*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+8, rcPop.Top+5}
    }; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc;                     /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ�����ֻ������*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VQueryGameInfo();/*����Ϸ����Ѱ��*/
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VQueryGameInfoM();/*���Ѱ��*/
        PopOff();
    } else {
        PopOff();
    }
    return bRet;
}

/**��ȷ������Ϸ������Ϣ������*/
void VQueryGameInfo() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"��Ϸ������Ϣ��ѯ","��Ϸ����",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNode(gp_head,pgame_node->game_name);
                    if(game_node!=NULL) {
                        char *plabel_name[11]= {"��Ϸ���",game_node->class_id,"��Ϸ����",game_node->game_name,
                                                "��Ϸ��ַ",game_node->game_adress,"�շ�ģʽ",game_node->charge_model,
                                                "������ʽ",game_node->rank_way,"ȷ��"
                                               };
                        ShowModuleWithExp(plabel_name, 11);
                        //putchar(game_node->charge_model);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ�����Ϸ","ȷ��"};
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
                /*����backspace*/
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
                if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**��ϲ�����Ϸ������Ϣ������*/
void VQueryGameInfoM() {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    char *pCh[] = {"��Ϸ��Ϣ��ϲ���","��Ϸ���","�շ�ģʽ",
                   "������ʽ"," ȷ��     ȡ��"
                  };
    int iHot=1;

    pos.X = strlen(pCh[0])+4;
    pos.Y = 9;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 5; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+5},
        {rcPop.Left+2, rcPop.Top+7}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 5;       /*�����ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 6;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    GAME_NODE *pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m1=0,m2=0,m3=0,m4=0,m5=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos4= {rcPop.Left + 10, rcPop.Top +4},
          pos5= {rcPop.Left + 10, rcPop.Top +5};
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==4) {
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù��bu�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PopOff();
                    char *pcondition[3]= {pgame_node->class_id,pgame_node->charge_model,pgame_node->rank_way};
                    GAME_NODE* game_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
                    game_node=SeekGameNodeM(gp_head,pcondition);
                    if(game_node!=NULL) {
                        VVQueryGameInfoM(game_node);
                        break;
                    } else {
                        char *q[]= {"û�з�����������Ϸ","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                } else if(iHot==5) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**��ϲ�����Ϸ������Ϣ��ѯ���������*/
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
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*�������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"������������Ϸ","��Ϸ���",game_node->class_id,
                      "�շ�ģʽ��",game_node->charge_model,
                      "������ʽ��",game_node->rank_way,
                      "��Ϸ����","��Ϸ��ַ","ȷ��"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;  /*�׵׺���*/
    labels.num = 10; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[10]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    aLoc[0].X=rcPop.Left+20;
    aLoc[0].Y=rcPop.Top+1;/*������������Ϸ*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*��Ϸ���*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*class_id*/
    aLoc[3].X=rcPop.Left+19;
    aLoc[3].Y=rcPop.Top+3;/*�շ�ģʽ*/
    aLoc[4].X=rcPop.Left+29;
    aLoc[4].Y=rcPop.Top+3;/*charge_model*/
    aLoc[5].X=rcPop.Left+33;
    aLoc[5].Y=rcPop.Top+3;/*������ʽ*/
    aLoc[6].X=rcPop.Left+43;
    aLoc[6].Y=rcPop.Top+3;/*rank_way*/
    aLoc[7].X=rcPop.Left+3;
    aLoc[7].Y=rcPop.Top+5;/*��Ϸ����*/
    aLoc[8].X=rcPop.Left+20;
    aLoc[8].Y=rcPop.Top+5;/*��Ϸ��ַ*/
    aLoc[9].X=rcPop.Left+24;
    aLoc[9].Y=rcPop.Bottom-1;/*ȷ��*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[9].X, aLoc[9].Y,
            aLoc[9].X + 3, aLoc[9].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

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
    /*�����߷ָ�*/
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

/**��һ�����Ϣ��ѯ*/
BOOL QueryPlayerInfo(void) {
    BOOL bRet = TRUE;
    char *pCh[] = { "���û�������Ϸ��",
                    "�Ʋ���",
                    "��ע�����ں�����",
                    "�ܽ�����",
                    "ȡ��"
                  };
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot=1;
    int i, j, loc = 0;
    pos.X = strlen(pCh[3])+10;/*pch[3]����Ϊ���ĸ��ַ����*/
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;                    /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 5; /*��ǩ���б�ǩ�ַ����ĸ�������5��*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+8, rcPop.Top+6}
    }; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc;                     /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ�����ֻ������*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    int isEnter=DealInput(&areas, &iHot);
    if(isEnter==13&&iHot==1) {
        PopOff();
        VQueryPlayerInfo();/*���û�������Ϸ���Ʋ���*/
        PopOff();
    } else if(isEnter==13&&iHot==2) {
        PopOff();
        VQueryPlayerInfoM();/*��ϲ���*/
        PopOff();
    } else {
        PopOff();
    }
    return bRet;
}

/**��ȷ������һ�����Ϣ������*/
void VQueryPlayerInfo(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"��һ�����Ϣ��ѯ","�û���",
                   "��Ϸ����"," ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+14;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 4;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
    int m=0,n=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3},pos2= {rcPop.Left + 10, rcPop.Top +4},
          pos_user= {rcPop.Left + 10, rcPop.Top +3},pos_game= {rcPop.Left + 10, rcPop.Top +4};

    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNode(gp_head,pplayer_node->game_name,pplayer_node->user_name);
                    if(player_node!=NULL) {
                        char *plabel_name[13]= {"�û���",player_node->user_name,"��Ϸ����",player_node->game_name,
                                                "ע������",player_node->enroll_datec,"��Ϸ�ۼ�ʱ��",player_node->total_timec,
                                                "�����ܽ��",player_node->total_moneyc,"����ָ��",player_node->achievementc,
                                                "ȷ��"
                                               };
                        ShowModuleWithExp(plabel_name, 13);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ������","ȷ��"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
                    if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**��ϲ�����һ�����Ϣ������*/
void VQueryPlayerInfoM(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att;
    BOOL bRet;
    char *pCh[] = {"���ģ�����������Ϣ","ע������  1",
                   "�����ܽ��2"," ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+12;
    pos.Y = 8;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 4; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+1},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+4},
        {rcPop.Left+2, rcPop.Top+6}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 4;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 5;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    PLAYER_NODE* pplayer_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));/*��ʱ������������*/
    pplayer_node->enroll_datec[0]='1';
    pplayer_node->total_moneyc[0]='2';
    int m=1,n=1;
    COORD pos1= {rcPop.Left + 13, rcPop.Top +3},pos2= {rcPop.Left + 13, rcPop.Top +4};;
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==3) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);

                    if(m!=10) {
                        char *plabel_name[2]= {"ע�����ڲ��Ϸ���","ȷ��"};
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
                        char *q[]= {"�Ҳ�������ݻ��·�!","ȷ��"};
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
                        char *q[]= {"������������!","ȷ��"};
                        ShowModule(q,2);
                        break;
                    }
                    PLAYER_NODE* player_node=(PLAYER_NODE*)malloc(sizeof(PLAYER_NODE));
                    player_node=SeekPlayerNodeM(gp_head,pplayer_node->enroll_datec,pplayer_node->total_moneyc);
                    if(player_node!=NULL) {
                        VVQueryPlayerInfoM(player_node);
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ����ϵ����","ȷ��"};
                        ShowModule(plabel_name, 2);
                        break;
                    }
                } else if(iHot==4) {
                    break;
                } else {
                    continue;
                }
            } else if(vkc==8) {
                /*����backspace*/
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
                        char *plabel_name[2]= {"�����������","ȷ��"};
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
                        char *plabel_name[2]= {"�����������","ȷ��"};
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**��ϲ�����һ�����Ϣ��ѯ���������*/
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
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*�������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"�������������","ע������","�����ܽ��",
                      "�û���","��Ϸ����","��Ϸ�ۼ�ʱ��","ȷ��"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;  /*�׵׺���*/
    labels.num = 7; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[7]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    aLoc[0].X=rcPop.Left+25;
    aLoc[0].Y=rcPop.Top+1;/*�������������*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*ע������*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*�����ܽ��*/
    aLoc[3].X=rcPop.Left+25;
    aLoc[3].Y=rcPop.Top+3;/*�û���*/
    aLoc[4].X=rcPop.Left+40;
    aLoc[4].Y=rcPop.Top+3;/*��Ϸ����*/
    aLoc[5].X=rcPop.Left+50;
    aLoc[5].Y=rcPop.Top+3;/*��Ϸ�ۼ�ʱ��*/
    aLoc[6].X=rcPop.Left+30;
    aLoc[6].Y=rcPop.Bottom-1;/*ȷ��*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[6].X, aLoc[6].Y,
            aLoc[6].X + 3, aLoc[6].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

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
    /*�����߷ָ�*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**ͳ��*/
/**��Ϸ��Ϣͳ��  ����ͳ��1*/
BOOL StatGame(void) {
    BOOL bRet = TRUE;
    GAME_STAT_NODE* game_stat_node;
    game_stat_node=StatGameInfo(gp_head);
    if(game_stat_node!=NULL) {
        VStatGame(game_stat_node);
    } else {
        char *plabel_name[] = {"û�����ݣ�",
                               "ȷ��"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**��Ϸ��Ϣͳ�ƽ��������*/
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
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*�������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    //puts(game_node->class_id);
    char* pString[]= {"��Ϸ��Ϣͳ��","��Ϸ����","�����",
                      "�����ʱ��H","����˾�ʱ���","���������Y","����˾�����","ȷ��"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 8; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[8]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    aLoc[0].X=rcPop.Left+30;
    aLoc[0].Y=rcPop.Top+1;/*��Ϸ��Ϣͳ��*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*��Ϸ����*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*�����*/
    aLoc[3].X=rcPop.Left+20;
    aLoc[3].Y=rcPop.Top+3;/*�����Ϸ��ʱ��*/
    aLoc[4].X=rcPop.Left+35;
    aLoc[4].Y=rcPop.Top+3;/*����˾���Ϸʱ��*/
    aLoc[5].X=rcPop.Left+50;
    aLoc[5].Y=rcPop.Top+3;/*��������ܽ��*/
    aLoc[6].X=rcPop.Left+65;
    aLoc[6].Y=rcPop.Top+3;/*����˾����ѽ��*/
    aLoc[7].X=rcPop.Left+35;
    aLoc[7].Y=rcPop.Bottom-1;/*ȷ��*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[7].X, aLoc[7].Y,
            aLoc[7].X + 3, aLoc[7].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

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
    FillConsoleOutputAttribute(gh_std_out,att1,13,pos4,&ul);/*����ʽ��ɫ��ʾ*/
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
    /*�����߷ָ�*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**�������ͳ��ͳ��  ����ͳ��2*/
BOOL StatRank(void) {
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos= {0,0};
    int iRet=0,arrow,num;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc,ascc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    WORD att,att1;
    BOOL bRet;
    char *pCh[] = {"��Ϸ�������ͳ��","��Ϸ����",
                   " ȷ��       ȡ��"
                  };
    int iHot=1;
    int i, j, loc = 0;

    pos.X = strlen(pCh[0])+18;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;      /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel =pCh;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = 3; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[] = {{rcPop.Left+2, rcPop.Top+2},
        {rcPop.Left+2, rcPop.Top+3},
        {rcPop.Left+2, rcPop.Top+5}
    };  /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = 3;       /*�����ĸ��������ڱ�ǩ�ĸ���*/
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
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_GREEN|BACKGROUND_BLUE;
    att1 = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_RED;
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/

    pos.X = rcPop.Left + 1; /*�����ߵ�λ��*/
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul); /*������*/
    SetHotPoint(&areas, iHot);

    GAME_NODE* pgame_node=(GAME_NODE*)malloc(sizeof(GAME_NODE));
    int m=0;
    COORD pos1= {rcPop.Left + 10, rcPop.Top +3};
    pos.X=pos1.X;
    pos.Y=pos1.Y;
    while (TRUE) {
        /*ѭ��*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown) { /*���̲���*/

            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;
            if (asc == 0) {
                Move(&areas,&iHot,asc,vkc);
            } else if (vkc == 27) {
                /*ESC��*/
                break;
            } else if (vkc == 13) { /*�س�����ʾ���µ�ǰ��ť*/
                if(iHot==2) {
                    PopOff();
                    GetConsoleCursorInfo(gh_std_out, &lpCur);
                    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
                    SetConsoleCursorInfo(gh_std_out, &lpCur);
                    if(SeekGameNode(gp_head,pgame_node->game_name)!=NULL) {
                        GAME_RANK_NODE* game_rank_node=StatRankList(gp_head,pgame_node->game_name);
                        if(game_rank_node!=NULL) {
                            VStatRank(game_rank_node);
                        } else {
                            char *plabel_name[2]= {"����Ϸľ�����~","ȷ��"};
                            ShowModule(plabel_name, 2);
                        }
                        break;
                    } else {
                        char *plabel_name[2]= {"û���ҵ�����Ϸ","ȷ��"};
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
                /*����backspace*/
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
                if((asc&0x80)==128) { /*����������ַ�*/
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
    lpCur.bVisible = FALSE;                /*���ù�겻�ɼ�*/
    SetConsoleCursorInfo(gh_std_out, &lpCur);
    return;
}

/**�������ͳ��ͳ�Ƶ�����*/
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
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*�������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    char* pString[]= {"��Ϸ���",game_rank_node->class_id,
                      "��Ϸ���ƣ�",game_rank_node->game_name,
                      "������ʽ��",game_rank_node->rank_way,
                      "����","�û���","��Ϸ�ۼ�ʱ��","�����ܶ�","����ָ��","ȷ��"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 12; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[12]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    aLoc[0].X=rcPop.Left+3;
    aLoc[0].Y=rcPop.Top+1;
    aLoc[1].X=rcPop.Left+13;
    aLoc[1].Y=rcPop.Top+1;/*��Ϸ���*/
    aLoc[2].X=rcPop.Left+20;
    aLoc[2].Y=rcPop.Top+1;
    aLoc[3].X=rcPop.Left+30;
    aLoc[3].Y=rcPop.Top+1;/*��Ϸ����*/
    aLoc[4].X=rcPop.Left+45;
    aLoc[4].Y=rcPop.Top+1;
    aLoc[5].X=rcPop.Left+55;
    aLoc[5].Y=rcPop.Top+1;/*������ʽ*/
    aLoc[6].X=rcPop.Left+3;
    aLoc[6].Y=rcPop.Top+3;/*����*/
    aLoc[7].X=rcPop.Left+12;
    aLoc[7].Y=rcPop.Top+3;/*�û���*/
    aLoc[8].X=rcPop.Left+28;
    aLoc[8].Y=rcPop.Top+3;/*��Ϸ�ۼ�ʱ��*/
    aLoc[9].X=rcPop.Left+43;
    aLoc[9].Y=rcPop.Top+3;/*�����ܶ�*/
    aLoc[10].X=rcPop.Left+58;
    aLoc[10].Y=rcPop.Top+3;/*����ָ��*/
    aLoc[11].X=rcPop.Left+30;
    aLoc[11].Y=rcPop.Bottom-1;/*ȷ��*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[11].X, aLoc[11].Y,
            aLoc[11].X + 3, aLoc[11].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    //puts("lall");
    PopUp(&rcPop, att, &labels, &areas);
    post.X=rcPop.Left+55;
    post.Y=rcPop.Top+1;
    FillConsoleOutputAttribute(gh_std_out,att1,1,post,&ul);/*����ʽ��ɫ��ʾ*/
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
    /*�����߷ָ�*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**��Ϸ���ͳ��ͳ��  ����ͳ��3*/
BOOL StatClass(void) {
    BOOL bRet = TRUE;
    CLASS_STAT_NODE* class_stat_node;
    class_stat_node=StatClassInfo(gp_head);
    if(class_stat_node!=NULL) {
        VStatClass(class_stat_node);
    } else {
        char *plabel_name[] = {"û�����ݣ�",
                               "ȷ��"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**��Ϸ�����Ϣͳ�ƽ��������*/
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
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*�������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"�����Ϣͳ��","��Ϸ���","�������",
                      "��Ϸ����","�������","�����ʱ��","���������","ȷ��"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 8; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[8]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    aLoc[0].X=rcPop.Left+30;
    aLoc[0].Y=rcPop.Top+1;/*�����Ϣͳ��*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*��Ϸ���*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*�������*/
    aLoc[3].X=rcPop.Left+25;
    aLoc[3].Y=rcPop.Top+3;/*��Ϸ����*/
    aLoc[4].X=rcPop.Left+37;
    aLoc[4].Y=rcPop.Top+3;/*�����*/
    aLoc[5].X=rcPop.Left+50;
    aLoc[5].Y=rcPop.Top+3;/*�����ʱ��*/
    aLoc[6].X=rcPop.Left+65;
    aLoc[6].Y=rcPop.Top+3;/*���������*/
    aLoc[7].X=rcPop.Left+35;
    aLoc[7].Y=rcPop.Bottom-1;/*ȷ��*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[7].X, aLoc[7].Y,
            aLoc[7].X + 3, aLoc[7].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

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
    FillConsoleOutputAttribute(gh_std_out,att1,7,pos4,&ul);/*����ʽ��ɫ��ʾ*/
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
    /*�����߷ָ�*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**�����Ϣͳ��  ����ͳ��4*/
BOOL StatPlayer(void) {
    BOOL bRet = TRUE;
    PLAYER_STAT_NODE* player_stat_node;
    player_stat_node=StatPlayerInfo(gp_head);
    if(player_stat_node!=NULL) {
        VStatPlayer(player_stat_node);
    } else {
        char *plabel_name[] = {"û�����ݣ�",
                               "ȷ��"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**�����Ϣͳ�ƽ��������*/
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
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*�������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"�����Ϣͳ��","�û���","��Ϸ�ۼ�ʱ��H��",
                      "�����ܽ��Y","ȷ��"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    att1 = BACKGROUND_BLUE | BACKGROUND_GREEN|FOREGROUND_RED;
    labels.num = 5; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[5]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    aLoc[0].X=rcPop.Left+20;
    aLoc[0].Y=rcPop.Top+1;/*�����Ϣͳ��*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*�û���*/
    aLoc[2].X=rcPop.Left+18;
    aLoc[2].Y=rcPop.Top+3;/*��Ϸ�ۼ�ʱ��*/
    aLoc[3].X=rcPop.Left+35;
    aLoc[3].Y=rcPop.Top+3;/*�����ܽ��*/
    aLoc[4].X=rcPop.Left+25;
    aLoc[4].Y=rcPop.Bottom-1;/*ȷ��*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[4].X, aLoc[4].Y,
            aLoc[4].X + 3, aLoc[4].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;

    PopUp(&rcPop, att, &labels, &areas);
    pos1.X=rcPop.Left+3;
    pos2.X=rcPop.Left+18;
    pos2.Y=rcPop.Top+3;
    pos3.X=rcPop.Left+35;
    FillConsoleOutputAttribute(gh_std_out,att1,14,pos2,&ul);/*����ʽ��ɫ��ʾ*/
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
    /*�����߷ָ�*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 ;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    pos.Y=rcPop.Bottom-2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);
    if(DealInput(&areas, &iHot)==13);
    PopOff();
}

/**�շ�ģʽͳ��  ����ͳ��5*/
BOOL StatCharge(void) {
    BOOL bRet = TRUE;
    CHARGE_NODE* charge_node;
    charge_node=StatChargeInfo(gp_head);
    if(charge_node!=NULL) {
        VStatCharge(charge_node);
    } else {
        char *plabel_name[] = {"û�������Ϣ",
                               "ȷ��"
                              };
        ShowModule(plabel_name, 2);
    }
    return bRet;
}

/**�շ�ģʽͳ�ƽ��������*/
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
    rcPop.Left = (SCR_COL - pos.X) / 2;   /*�������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;
    char* pString[]= {"�շ�ģʽ��Ϣͳ��","�շ�ģʽ","��Ϸ����",
                      "�����","�����ʱ��","���������","ȷ��"
                     };

    att = BACKGROUND_BLUE | BACKGROUND_GREEN;
    labels.num = 7; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[7]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    aLoc[0].X=rcPop.Left+20;
    aLoc[0].Y=rcPop.Top+1;/*��Ϸ��Ϣͳ��*/
    aLoc[1].X=rcPop.Left+3;
    aLoc[1].Y=rcPop.Top+3;/*�շ�ģʽ*/
    aLoc[2].X=rcPop.Left+13;
    aLoc[2].Y=rcPop.Top+3;/*��Ϸ����*/
    aLoc[3].X=rcPop.Left+20;
    aLoc[3].Y=rcPop.Top+3;/*�����*/
    aLoc[4].X=rcPop.Left+28;
    aLoc[4].Y=rcPop.Top+3;/*�����ʱ��*/
    aLoc[5].X=rcPop.Left+40;
    aLoc[5].Y=rcPop.Top+3;/*���������*/
    aLoc[6].X=rcPop.Left+25;
    aLoc[6].Y=rcPop.Bottom-1;/*ȷ��*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[6].X, aLoc[6].Y,
            aLoc[6].X + 3, aLoc[6].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

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
    /*�����߷ָ�*/
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
    char *plabel_name[] = {"�������뷢������������",
                           "U201314970@hust.edu.cn",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 3);

    return bRet;
}

BOOL AboutGame(void) {
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���ߣ���У��",
                           "���пƼ���ѧCS1307",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 3);

    return bRet;
}

/**
 * ��������: InsertClassNode
 * ��������: ��ʮ�������в���һ����Ϸ��������Ϣ���.
 * �������: hd ����ͷָ��
 *           pclass_node ָ����Ҫ�������ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ����ɹ�, FALSE��ʾ����ʧ��
 *
 * ����˵��:
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
 * ��������: InsertGameNode
 * ��������: ��ʮ�������в���һ����Ϸ������Ϣ���.
 * �������: hd ����ͷָ��
 *           pgame_node ָ����Ҫ�������ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ����ɹ�, FALSE��ʾ����ʧ��
 *
 * ����˵��:
 */
BOOL InsertGameNode(CLASS_NODE *hd, GAME_NODE *pgame_node) {
    CLASS_NODE *pclass_node=hd;
    while(pclass_node!=NULL) {    /*��������Ѱ�Ҷ�Ӧ��Ϸ���*/
        if(strcmp(pclass_node->class_id,pgame_node->class_id)==0) { /*����ҵ�*/
            pgame_node->next=pclass_node->gnext;
            pclass_node->gnext=pgame_node;
            return TRUE;
        }
        pclass_node=pclass_node->next;
    }
    if(pclass_node==NULL) { /*δ�ҵ�*/
        return FALSE;
    }
}
/**
 * ��������: InsertPlayerNode
 * ��������: ��ʮ�������в���һ����һ�����Ϣ���.
 * �������: hd ����ͷָ��
 *           pplayer_node ָ����Ҫ�������ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ����ɹ�, FALSE��ʾ����ʧ��
 *
 * ����˵��:
 */
BOOL InsertPlayerNode(CLASS_NODE *hd, PLAYER_NODE *pplayer_node) {
    GAME_NODE *pgame_node;
    /*��ʮ�������ҵ���Ӧ��Ϸ����Ϣ���*/
    pgame_node=SeekGameNode(hd,pplayer_node->game_name);
    if(pgame_node!=NULL) {              /*����ҵ��������ý�����һ�����Ϣ֧��*/
        pplayer_node->next=pgame_node->pnext;
        pgame_node->pnext=pplayer_node;
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * ��������: DelClassNode
 * ��������: ��ʮ��������ɾ��ָ������Ϸ�����Ϣ���.
 * �������: hd ����ͷָ��
 *           class_id ������
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
 *
 * ����˵��: ɾ��ǰ�����ʾ��Ϣ����ɾ����������ϵ���Ϸ֧������Ϸ֧���ϵ����֧��
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
    if(pclass_node_current!=NULL) {     /*����ҵ�*/
        if(pclass_node_piror==NULL) {   /*�����ͷָ��*/
            *hd=pclass_node_current->next;
        } else {
            pclass_node_piror->next=pclass_node_current->next;
        }
        free(pclass_node_current);  /*�ͷŽ��洢��*/
        bRet=TRUE;
    } else {
        bRet=FALSE;
    }
    return bRet;
}

/**
* ��������: DelGameNode
* ��������: ��ʮ��������ɾ��ָ������Ϸ������Ϣ���.
* �������: hd ����ͷָ��
*           game_name ��Ϸ����
* �������: ��
* �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
*
* ����˵��: ɾ��ǰ�����ʾ��Ϣ����ɾ������Ϸ����ϵ����֧��
*/
BOOL DelGameNode(CLASS_NODE *hd, char *game_name) { /*�����ʾ��*/
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node_piror;
    GAME_NODE *pgame_node_current;
    BOOL bRet=FALSE;
    int find=0;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*����ѭ����������������ÿ��������Ϸ��Ϣ֧��*/
        pgame_node_piror=pclass_node->gnext;        /*ǰһ�����*/
        pgame_node_current=pclass_node->gnext;      /*��ǰ���*/
        while(pgame_node_current!=NULL) {
            if(strcmp(pgame_node_current->game_name,game_name)==0) { /*����ҵ�*/
                find=1;
                break;
            }
            pgame_node_piror=pgame_node_current;            /*����ǰһ���ڵ����Ϣ*/
            pgame_node_current=pgame_node_current->next;
        }
        if(find) {
            break;
        }
    }
    if(find) {
        if(pgame_node_piror==pclass_node->gnext) {
            pclass_node->gnext=pgame_node_current->next;    /*���λ�ڵ�һ����㣬���������*/
        } else {
            pgame_node_piror->next=pgame_node_current->next;
        }
        free(pgame_node_current); /*�ͷŽ��洢��*/
        bRet=TRUE;
    } else {
        bRet=FALSE;
    }
    return bRet;
}
/**
 * ��������: DelPlayerNode
 * ��������: ��ʮ��������ɾ��ָ������һ�����Ϣ���.
 * �������: hd ����ͷָ��
 *           game_name ��Ϸ����
 *           user_name �û���
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾɾ���ɹ�, FALSE��ʾɾ��ʧ��
 *
 * ����˵��: ������Ϸ���ƺ��û�����Ψһȷ����Ϣ
 */
BOOL DelPlayerNode(CLASS_NODE *hd, char *game_name, char *user_name) {

    GAME_NODE *pgame_node;
    PLAYER_NODE * pplayer_node_piror;
    PLAYER_NODE * pplayer_node_current;
    BOOL bRet=FALSE;

    pgame_node=SeekGameNode(hd,game_name);    /*��ʮ�����в��Ҹ���Ϸ������Ϣ����*/
    if(pgame_node!=NULL) {      /*����ҵ�*/
        /*����һ�����Ϣ֧���ϲ��Ҹ����û����������Ϣ*/
        pplayer_node_piror=NULL;
        pplayer_node_current=pgame_node->pnext;
        while(pplayer_node_current!=NULL
                &&strcmp(pplayer_node_current->user_name,user_name)!=0) {
            pplayer_node_piror=pplayer_node_current;
            pplayer_node_current=pplayer_node_current->next;
        }

        if(pplayer_node_current!=NULL) { /*����ҵ�*/
            bRet=TRUE;
            if(pplayer_node_piror==NULL) {      /*���λ�ڵ�һ����㣬����Ϸ����*/
                pgame_node->pnext=pplayer_node_current->next;
            } else {
                pplayer_node_piror->next=pplayer_node_current->next;
            }
            free(pplayer_node_current);         /*�ͷŽ��洢��*/
        }
    }
    return bRet;
}

/**
 * ��������: ModifClassNode
 * ��������: ��ָ������Ϸ��������Ϣ������ݽ����޸�.
 * �������: hd ����ͷָ��
 *           class_id ������
 *           pclass_node ָ�����޸����ݽ���ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 *
 * ����˵��:
 */
BOOL ModifClassNode(CLASS_NODE **hd, char *class_id, CLASS_NODE *pclass_node) {
    CLASS_NODE *pclass_node_temp;
    CLASS_NODE *pclass_node_next;
    pclass_node_temp=SeekClassNodeById(*hd,class_id);        /*������Ϸ�����*/
    if(pclass_node_temp!=NULL) { /*����ҵ�*/
        pclass_node_next=pclass_node_temp->next;        /*��ֵǰ����ָ����*/
        *pclass_node_temp=*pclass_node;
        pclass_node_temp->next=pclass_node_next;        /*�ָ�ָ���ϵ*/
        return TRUE;
    } else {
        return FALSE;
    }
}
/**
 * ��������: ModifGameNode
 * ��������: ��ָ������Ϸ������Ϣ������ݽ����޸�.
 * �������: hd ����ͷָ��
 *           game_name ��Ϸ����
 *           pgame_node ָ�����޸����ݽ���ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 *
 * ����˵��:
 */
BOOL ModifGameNode(CLASS_NODE *hd, char *game_name, GAME_NODE *pgame_node) {
    GAME_NODE *pgame_node_temp;
    GAME_NODE *pgame_node_next;
    pgame_node_temp=SeekGameNode(hd,game_name);     /*������Ϸ������Ϣ���*/
    if(pgame_node_temp!=NULL) {     /*����ҵ�*/
        pgame_node_next=pgame_node_temp->next;      /*��ֵǰ����ָ����*/
        *pgame_node_temp=*pgame_node;
        pgame_node_temp->next=pgame_node_next;      /*�ָ�ָ���ϵ*/
        return TRUE;
    } else {
        return FALSE;
    }
}
/**
 * ��������: ModifPlayerNode
 * ��������: ��ָ������һ�����Ϣ������ݽ����޸�.
 * �������: hd ����ͷָ��
 *           game_name ��Ϸ����
 *           user_name �û���
 *           pplayer_node ָ�����޸����ݽ���ָ��
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE��ʾ�޸ĳɹ�, FALSE��ʾ�޸�ʧ��
 *
 * ����˵��:
 */
BOOL ModifPlayerNode(CLASS_NODE *hd, char *game_name, char *user_name, PLAYER_NODE *pplayer_node) {
    PLAYER_NODE *pplayer_node_temp;
    PLAYER_NODE *pplayer_node_next;
    pplayer_node_temp=SeekPlayerNode(hd,game_name,user_name);       /*����ָ�������Ϣ���*/
    if(pplayer_node_temp!=NULL) {                    /*����ҵ�*/
        pplayer_node_next=pplayer_node_temp->next;  /*��ֵǰ����ָ����*/
        *pplayer_node_temp=*pplayer_node;
        pplayer_node_temp->next=pplayer_node_next;  /*�ָ�ָ���ϵ*/
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * ��������: SeekClassNodeById
 * ��������: ���������������������������Ϸ������Ϣ.
 * �������: hd ����ͷָ��
 *           class_id ������
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 *
 * ����˵��:
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
                return pclass_node;                     /*������ͷ*/
            }
        }
    }
    return NULL;
}
/**
* ��������: SeekClassNodeByDec
* ��������: �����������������������������Ϸ������Ϣ.
* �������: hd ����ͷָ��
*           class_dec �������
* �������: ��
* �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
*
* ����˵��:
*/
CLASS_NODE*SeekClassNodeByDec(CLASS_NODE *hd,char* class_dec) {
    CLASS_NODE *pclass_node;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        if(strcmp(pclass_node->class_dec,class_dec)==0) {
            return pclass_node;                 /*������ͷ*/
        }
    }
    return NULL;
}

/**
 * ��������: SeekGameNode
 * ��������: ����Ϸ���Ʋ�����Ϸ������Ϣ���.
 * �������: hd ����ͷָ��
 *           game_name ��Ϸ����
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 *
 * ����˵��:
 */
GAME_NODE *SeekGameNode(CLASS_NODE *hd, char *game_name) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    int find=0;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*����ѭ����������������ÿ��������Ϸ��Ϣ֧��*/
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
 * ��������: SeekplayerNode
 * ��������: ����Ϸ���ƺ��û������������Ϣ���.
 * �������: hd ����ͷָ��
 *           game_name ��Ϸ����
 *           user_name �û���
 * �������: ��
 * �� �� ֵ: ����ʱ���ؽ��ĵ�ַ, ���򷵻�NULL
 *
 * ����˵��:
 */
PLAYER_NODE *SeekPlayerNode(CLASS_NODE *hd, char *game_name, char *user_name) {
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    int find=0;
    pgame_node=SeekGameNode(hd,game_name);      /*���Ȳ��Ҷ�Ӧ����Ϸ������Ϣ���*/

    /*����ҵ����������Ϣ֧���ϼ�������ָ���û����������Ϣ���*/
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
 * ��������: SeekGameNodeM
 * ��������: ������������ϲ�ѯ����������������Ϸ��Ϣ���.
 * �������: hd ����ͷָ��
 *           cond_num ��������ĸ���
 *           ... ��ʾ��ѯ�������ַ���
 * �������: ��
 * �� �� ֵ: ���������������Ľ�㸴�Ƶ�����������ؽ�������ͷ����ַ
 *
 * ����˵��:
 */
GAME_NODE *SeekGameNodeM (CLASS_NODE *hd,char**condition) {
    CLASS_NODE *class_node;
    GAME_NODE *game_node;
    GAME_NODE *game_node_ret=NULL;  /*��ѯ�����ͷָ��*/
    GAME_NODE *game_node_temp;
    int i;
    BOOL bRet1,bRet2,bRet3;
    for(class_node=hd->next; class_node!=NULL; class_node=class_node->next) {
        /*����ʮ������ÿ����Ϸ��Ϣ��㣬���������������Ľ�㸴�Ƶ��������*/

        //puts(hd->class_id);
        game_node=class_node->gnext;
        while(game_node!=NULL) {
            //puts(game_node->class_id);
            //puts(condition[0]);
            bRet1=!strcmp(game_node->class_id,condition[0]); /*��Ϸ���*/
            bRet2=!strcmp(game_node->charge_model,condition[1]);/*�շ�ģʽ*/
            bRet3=!strcmp(game_node->rank_way,condition[2]);/*������ʽ*/
            //printf("%d%d%d",bRet1,bRet2,bRet3);
            if((bRet1==TRUE)&&(bRet2==TRUE)&&(bRet3==TRUE)) { /*���ʱ����������������*/
                /*���ý����Ϣ���Ƶ�һ���½�㣬�����½����뵽�������*/
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
 * ��������: SeekPlayerNodeM
 * ��������: ������������ϲ�ѯ�������������������Ϣ���.
 * �������: hd ����ͷָ��
 *           cond_num ��������ĸ���
 *           ... ��ʾ��ѯ�������ַ���
 * �������: ��
 * �� �� ֵ: ���������������Ľ�㸴�Ƶ�����������ؽ�������ͷ����ַ
 *
 * ����˵��:
 */
PLAYER_NODE *SeekPlayerNodeM (CLASS_NODE *hd,char* condition1,char* condition2) {
    //puts(condition2);
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    PLAYER_NODE *pplayer_node_ret=NULL;          /*��ѯ�����ͷָ��*/
    PLAYER_NODE *pplayer_node_temp;
    BOOL bRet1,bRet2;
    int i;
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*����ѭ������ʮ������ÿ�������Ϣ��㣬�����������Ľ�㸴�Ƶ��������*/
        pgame_node=pclass_node->gnext;
        for(; pgame_node!=NULL; pgame_node=pgame_node->next) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                bRet1=JudgePlayerNodeItem(gp_head,pplayer_node,condition1);/*����ͷָ�룬���ڴ˺���������Ϸ������ʽ*/
                bRet2=JudgePlayerNodeItem(gp_head,pplayer_node,condition2);

                if((bRet1==TRUE)&&(bRet2==TRUE)) {          /*��������������*/
                    /*���ý����Ϣ���Ƶ�һ���½�㣬�����½����뵽�������*/
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
 * ��������: JudgeGameNodeItem
 * ��������: �ж���Ϸ��Ϣ����Ƿ������������.
 * �������: pgame_node ��Ϸ��Ϣ���ָ��
 *           pcondition ������ʾ�������ַ���
 * �������: ��
 * �� �� ֵ: ��������ʱ, ����TRUE; ���򷵻�FALSE
 *
 * ����˵��:
 */
BOOL JudgeGameNodeItem(GAME_NODE *pgame_node, char *pcondition) {
    int item_num;
    BOOL bRet=FALSE;
    item_num=*pcondition;       /*�������ַ�������ȡ�Ƚ�����������*/
    switch(item_num) {          /*���������ƥ��*/
    case 1:
        bRet=MatchString(pgame_node->class_id,pcondition+1);
        break; /*��Ϸ���*/
    case 2:
        bRet=MatchString(pgame_node->charge_model,pcondition+1);
        break;/*�շ�ģʽ*/
    case 3:
        bRet=MatchString(pgame_node->rank_way,pcondition+1);
        break;/*������ʽ*/
    }
    return bRet;
}
/**
 * ��������: JudgePlayerNodeItem
 * ��������: �ж������Ϣ����Ƿ������������.
 * �������: pplayer_node �����Ϣ���ָ��
 *           pcondition ������ʾ�������ַ���
 * �������: ��
 * �� �� ֵ: ��������ʱ, ����TRUE; ���򷵻�FALSE
 *
 * ����˵��:
 */
BOOL JudgePlayerNodeItem(CLASS_NODE *hd,PLAYER_NODE *pplayer_node, char *pcondition) {
    int item_num;
    BOOL bRet=FALSE;
    item_num=*pcondition;       /*�������ַ�������ȡ�Ƚ�����������*/
    //printf("%d  ",*pcondition);
    switch(item_num) {          /*���������ƥ��*/
    case 49:
        bRet=MatchString(pplayer_node->enroll_datec,pcondition+1);
        break;/*1��asc��Ϊ49*/
    case 50:
        //printf("%f\n",pplayer_node->total_money);
        bRet=MatchDouble(pplayer_node->total_money,pcondition+1);
        //puts(pcondition+1);
        break;/*2��asc��Ϊ50*/
    }
    return bRet;
}

/**
 * ��������: MatchString
 * ��������: �Ը����ַ�������������ƥ��.
 * �������: string_item �����ַ���
 *           pcond ����ƥ����������ڵ������ַ���
 * �������: ��
 * �� �� ֵ: ƥ��ɹ�ʱ, ����TRUE; ���򷵻�FALSE
 *
 * ����˵��:
 */
BOOL MatchString(char *string_item, char *pcond) {
    char op;
    int compare_result;
    char * sub_string_pos;
    BOOL bRet=FALSE;
    compare_result=strcmp(string_item,pcond+1);     /*�ַ����Ƚϴ�С*/
    sub_string_pos=strstr(string_item,pcond+1);     /*�ַ����İ�����ϵ*/
    op=*pcond;      /*��ȡƥ�������*/
    switch(op) {
    case '=':  /*�Ƿ��������ַ������*/
        if(compare_result==0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '<':  /*�Ƿ�С�������ַ���*/
        if(compare_result<0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '>':   /*�Ƿ���������ַ���*/
        if(compare_result>0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '!':   /*�Ƿ񲻵��������ַ���*/
        if(compare_result!=0) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '@':   /*�Ƿ���������ַ���*/
        if(sub_string_pos!=NULL) {
            bRet=TRUE;
        } else {
            bRet=FALSE;
        }
        break;
    case '?':   /*�Ƿ񲻰��������ַ���*/
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
 * ��������: MatchChar
 * ��������: �Ը����ַ�����������ƥ��.
 * �������: char_item �����ַ�
 *           pcond ����ƥ����������ڵ������ַ���
 * �������: ��
 * �� �� ֵ: ƥ��ɹ�ʱ, ����TRUE; ���򷵻�FALSE
 *
 * ����˵��:
 */
BOOL MatchChar(char char_item, char *pcond) {
    char op;
    int compare_result;
    BOOL bRet=FALSE;
    op=*pcond;                              /*��ȡƥ�������*/
    compare_result=char_item-*(pcond+1);    /*�Ƚ��ַ���С*/

    switch(op) {
    case '=':       /*�Ƿ����*/
        if(compare_result==0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '<':       /*�Ƿ�С��*/
        if(compare_result<0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '>':       /*�Ƿ����*/
        if(compare_result>0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '!':       /*�Ƿ����*/
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
 * ��������: MatchLong
 * ��������: �Ը���long���ݰ���������ƥ��.
 * �������: long_item ����long������
 *           pcond ����ƥ����������ڵ������ַ���
 * �������: ��
 * �� �� ֵ: ƥ��ɹ�ʱ, ����TRUE; ���򷵻�FALSE
 *
 * ����˵��:
 */
BOOL MatchLong(long long_item, char *pcond) {
    char op;
    int compare_result;
    BOOL bRet=FALSE;
    compare_result=long_item-*(pcond+1);
    op=*pcond;

    switch(op) {
    case '=':       /*�Ƿ����*/
        if(compare_result==0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '<':       /*�Ƿ�С��*/
        if(compare_result<0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '>':       /*�Ƿ����*/
        if(compare_result>0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '!':       /*�Ƿ񲻵���*/
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
 * ��������: MatchDouble
 * ��������: �Ը���double�����ݰ���������ƥ��.
 * �������: double_item ����double����
 *           pcond ����ƥ����������ڵ������ַ���
 * �������: ��
 * �� �� ֵ: ƥ��ɹ�ʱ, ����TRUE; ���򷵻�FALSE
 *
 * ����˵��:
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
    case '=':       /*�Ƿ����*/
        if(compare_result==0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '<':       /*�Ƿ�С��*/
        if(compare_result<0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '>':       /*�Ƿ����*/
        if(compare_result>0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    case '!':       /*�Ƿ񲻵���*/
        if(compare_result!=0) {
            return TRUE;
        } else {
            return FALSE;
        }
        break;
    }
}

/**
 * ��������: StatGameInfo   ����ͳ��1
 * ��������: ͳ����Ϸ������Ϣ.
 * �������: hd ����ͷ���ָ��
 * �������: ��
 * �� �� ֵ: ����ͳ�ƽ����ͷ����ַ
 *
 * ����˵��:
 */
GAME_STAT_NODE *StatGameInfo(CLASS_NODE *phd) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    GAME_STAT_NODE *pgame_stat_node_ret=NULL;   /*ͷָ��*/
    //pgame_stat_node_ret=(GAME_STAT_NODE*)malloc(sizeof(GAME_STAT_NODE));
    //pgame_stat_node_ret->next=NULL;
    GAME_STAT_NODE *pgame_stat_node_temp;
    unsigned long player_num=0;   /*<�����*/
    double player_time=0;   /*<�����Ϸ��ʱ�䣨Сʱ��*/
    double player_aver_time=0;    /*<����˾���Ϸʱ�䣨Сʱ��*/
    double player_money=0;  /*<��������ܽ�Ԫ��*/
    double player_aver_money=0;   /*<����˾����ѽ�Ԫ��*/
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*����ѭ������ʮ�ֽ�����*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            player_num=0;
            player_time=0;
            player_money=0;
            //puts(pgame_node->game_name);
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                //puts(pplayer_node->user_name);
                player_num+=1;          /*�����ۼ�*/
                player_time+=pplayer_node->total_time;
                player_money+=pplayer_node->total_money;
                pplayer_node=pplayer_node->next;
            }
            //printf("%f%f\n",player_time,player_money);
            if(player_num==0) { /*��������Ϊ0*/
                player_aver_time=0;
                player_aver_money=0;
            } else {
                player_aver_time=player_time/player_num;    /*����ƽ��ʱ�䣬���*/
                player_aver_money=player_money/player_num;
            }

            //printf("%g%g\n",player_aver_money,player_aver_time);

            pgame_stat_node_temp=(GAME_STAT_NODE*)malloc(sizeof(GAME_STAT_NODE));
            strcpy(pgame_stat_node_temp->game_name,pgame_node->game_name);
            pgame_stat_node_temp->player_num=player_num;    /*���и�ֵ*/
            pgame_stat_node_temp->player_time=player_time;
            pgame_stat_node_temp->player_aver_time=player_aver_time;
            pgame_stat_node_temp->player_money=player_money;
            pgame_stat_node_temp->player_aver_money=player_aver_money;

            pgame_stat_node_temp->next=NULL;
            //puts(pgame_stat_node_temp->game_name);
            pgame_stat_node_temp->next=pgame_stat_node_ret; /*��������*/
            pgame_stat_node_ret=pgame_stat_node_temp;

            pgame_node=pgame_node->next;    /*ָ����һ����Ϸ���*/
        }
    }
    SortGameInfo(pgame_stat_node_ret);  /*��������*/
    return pgame_stat_node_ret;         /*����ָ��*/
}

/**
 * ��������: SortGameInfo
 * ��������: ����Ϸ��Ϣ����������.
 * �������: game_phd ��Ϸ��Ϣ��ͷ���ָ��
 * �������:
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SortGameInfo(GAME_STAT_NODE *game_phd) {
    GAME_STAT_NODE *pgame_stat_node_piror;
    GAME_STAT_NODE *pgame_stat_node_after;
    GAME_STAT_NODE *pgame_stat_node_current;
    GAME_STAT_NODE *pgame_stat_node_temp;
    pgame_stat_node_piror=game_phd;
    if(pgame_stat_node_piror==NULL) {    /*����ǿ���*/
        return;
    }
    /*����ʱ�����ڽ�����Ϣ����ʱ���*/
    pgame_stat_node_temp=(GAME_STAT_NODE*)malloc(sizeof(GAME_STAT_NODE));
    while(pgame_stat_node_piror->next!=NULL) {
        pgame_stat_node_current=pgame_stat_node_piror;
        pgame_stat_node_after=pgame_stat_node_piror->next;
        while(pgame_stat_node_after!=NULL) { /*ѡ������*/
            if(pgame_stat_node_current->player_aver_time<pgame_stat_node_after->player_aver_time) {
                pgame_stat_node_current=pgame_stat_node_after;  /*ʼ��ָ������*/
            }
            pgame_stat_node_after=pgame_stat_node_after->next;
        }
        if(pgame_stat_node_current!=pgame_stat_node_piror) {
            /*����������Ϣ��ָ���򲻽���*/
            *pgame_stat_node_temp=*pgame_stat_node_piror;
            *pgame_stat_node_piror=*pgame_stat_node_current;
            pgame_stat_node_piror->next=pgame_stat_node_temp->next;
            pgame_stat_node_temp->next=pgame_stat_node_current->next;
            *pgame_stat_node_current=*pgame_stat_node_temp;
        }
        pgame_stat_node_piror=pgame_stat_node_piror->next;
    }
    free(pgame_stat_node_temp);/*�ͷŴ洢��*/
    return;
}

/**
 * ��������: StatRankList  ����ͳ��2
 * ��������: ͳ�����������Ϣ.
 * �������: hd ����ͷ���ָ��
 * �������: ��
 * �� �� ֵ: ����ͳ�ƽ����ͷ����ַ
 *
 * ����˵��:
 */
GAME_RANK_NODE *StatRankList(CLASS_NODE *phd, char *game_name) {
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    GAME_RANK_NODE *pgame_rank_node_ret=NULL;
    GAME_RANK_NODE *pgame_rank_node_temp;
    pgame_node=SeekGameNode(phd,game_name);
    if(pgame_node!=NULL) {
        if(pgame_node->rank_way[0]=='p') { /*����ǰ����������*/
            //puts(pgame_node->rank_way);
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                /*���и�ֵ*/
                pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
                strcpy(pgame_rank_node_temp->game_name,game_name);
                strcpy(pgame_rank_node_temp->class_id,pgame_node->class_id);
                strcpy(pgame_rank_node_temp->rank_way,pgame_node->rank_way);
                strcpy(pgame_rank_node_temp->user_name,pplayer_node->user_name);
                pgame_rank_node_temp->total_time=pplayer_node->total_time;
                pgame_rank_node_temp->total_money=pplayer_node->total_money;
                pgame_rank_node_temp->achievement.p=pplayer_node->achievement.p;

                pgame_rank_node_temp->next=pgame_rank_node_ret;/*��������*/
                pgame_rank_node_ret=pgame_rank_node_temp;

                pplayer_node=pplayer_node->next;
            }
            SortRankList_p(pgame_rank_node_ret);/*����*/
        } else { /*��ʱ������*/
            //puts(pgame_node->rank_way);
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                /*��ֵ*/
                pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
                strcpy(pgame_rank_node_temp->game_name,game_name);
                strcpy(pgame_rank_node_temp->class_id,pgame_node->class_id);
                strcpy(pgame_rank_node_temp->rank_way,pgame_node->rank_way);
                strcpy(pgame_rank_node_temp->user_name,pplayer_node->user_name);
                pgame_rank_node_temp->total_time=pplayer_node->total_time;
                pgame_rank_node_temp->total_money=pplayer_node->total_money;
                pgame_rank_node_temp->achievement.t=pplayer_node->achievement.t;

                pgame_rank_node_temp->next=pgame_rank_node_ret; /*��������*/
                pgame_rank_node_ret=pgame_rank_node_temp;

                pplayer_node=pplayer_node->next;
            }
            SortRankList_t(pgame_rank_node_ret);/*����*/
        }
        return pgame_rank_node_ret;
    } else {
        return NULL;
    }
}

/**
 * ��������: SortRankList_p
 * ��������: �����������Ϣ����������.
 * �������: rank_phd ���������Ϣ��ͷ���ָ��
 * �������:
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SortRankList_p(GAME_RANK_NODE *rank_phd) {
    int rank_place=0;/*����*/
    GAME_RANK_NODE *pgame_rank_node_piror;
    GAME_RANK_NODE *pgame_rank_node_current;
    GAME_RANK_NODE *pgame_rank_node_after;
    GAME_RANK_NODE *pgame_rank_node_temp;
    pgame_rank_node_piror=rank_phd;
    if(pgame_rank_node_piror==NULL) {
        return;
    }
    if(pgame_rank_node_piror->next==NULL) {
        pgame_rank_node_piror->rank_place=1;/*�ȶԵ�һ����ֵ*/
        return;
    }

    //printf("%f",pgame_rank_node_piror->achievement.p);
    pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
    while(pgame_rank_node_piror->next!=NULL) {
        pgame_rank_node_current=pgame_rank_node_piror;
        pgame_rank_node_after=pgame_rank_node_piror->next;
        while(pgame_rank_node_after!=NULL) {

            if(pgame_rank_node_current->achievement.p<pgame_rank_node_after->achievement.p) {
                /*ʼ��ָ������*/
                pgame_rank_node_current=pgame_rank_node_after;
            }
            pgame_rank_node_after=pgame_rank_node_after->next;
        }
        if(pgame_rank_node_current!=pgame_rank_node_piror) {
            /*��������е���Ϣ*/
            *pgame_rank_node_temp=*pgame_rank_node_piror;
            *pgame_rank_node_piror=*pgame_rank_node_current;
            pgame_rank_node_piror->next=pgame_rank_node_temp->next;
            pgame_rank_node_temp->next=pgame_rank_node_current->next;
            *pgame_rank_node_current=*pgame_rank_node_temp;
        }
        rank_place+=1;/*��������*/
        pgame_rank_node_piror->rank_place=rank_place;
        pgame_rank_node_piror->next->rank_place=rank_place+1;
        pgame_rank_node_piror=pgame_rank_node_piror->next;
    }
    free(pgame_rank_node_temp);
    return;
}

/**
 * ��������: SortRankList_t
 * ��������: �����������Ϣ����������.
 * �������: rank_phd ���������Ϣ��ͷ���ָ��
 * �������:
 * �� �� ֵ: ��
 *
 * ����˵��:
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
        pgame_rank_node_piror->rank_place=1;/*�ȶԵ�һ����ֵ*/
        return;
    }
    pgame_rank_node_temp=(GAME_RANK_NODE *)malloc(sizeof(GAME_RANK_NODE));
    while(pgame_rank_node_piror->next!=NULL) {
        pgame_rank_node_current=pgame_rank_node_piror;
        pgame_rank_node_after=pgame_rank_node_piror->next;
        while(pgame_rank_node_after!=NULL) {
            if(pgame_rank_node_current->achievement.t>pgame_rank_node_after->achievement.t) {
                /*ʼ��ָ����С��*/
                pgame_rank_node_current=pgame_rank_node_after;
            }
            pgame_rank_node_after=pgame_rank_node_after->next;
        }
        if(pgame_rank_node_current!=pgame_rank_node_piror) {
            /*���������Ϣ*/
            *pgame_rank_node_temp=*pgame_rank_node_piror;
            *pgame_rank_node_piror=*pgame_rank_node_current;
            pgame_rank_node_piror->next=pgame_rank_node_temp->next;
            pgame_rank_node_temp->next=pgame_rank_node_current->next;
            *pgame_rank_node_current=*pgame_rank_node_temp;
        }
        rank_place+=1;/*��������*/
        pgame_rank_node_piror->rank_place=rank_place;
        pgame_rank_node_piror->next->rank_place=rank_place+1;
        pgame_rank_node_piror=pgame_rank_node_piror->next;
    }
    free(pgame_rank_node_temp);
    return;
}

/**
 * ��������: StatClassInfo  ����ͳ��3
 * ��������: ͳ�������Ϣ.
 * �������: hd ����ͷ���ָ��
 * �������: ��
 * �� �� ֵ: ����ͳ�ƽ����ͷ����ַ
 *
 * ����˵��:
 */
CLASS_STAT_NODE *StatClassInfo(CLASS_NODE *phd) {
    unsigned long game_num;         /*<��Ϸ����*/
    double player_total_num;        /*<�����*/
    double player_total_time;      /*<�����Ϸ��ʱ��*/
    double player_total_money;      /*<��������ܽ��*/
    CLASS_STAT_NODE *pclass_stat_node_ret=NULL;
    CLASS_STAT_NODE *pclass_stat_node_tem;
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*����ֵΪ0*/
        game_num=0;
        player_total_num=0;
        player_total_time=0;
        player_total_money=0;
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                /*ͳ��*/
                player_total_num+=1;
                player_total_money+=pplayer_node->total_money;
                player_total_time+=pplayer_node->total_time;
                pplayer_node=pplayer_node->next;
            }
            game_num+=1;
            pgame_node=pgame_node->next;
        }
        pclass_stat_node_tem=(CLASS_STAT_NODE*)malloc(sizeof(CLASS_STAT_NODE));/*��ȡ�洢��*/
        strcpy(pclass_stat_node_tem->class_id,pclass_node->class_id); /*�Խ�㸳ֵ*/
        strcpy(pclass_stat_node_tem->class_dec,pclass_node->class_dec);
        pclass_stat_node_tem->game_num=game_num;
        pclass_stat_node_tem->playeer_total_time=player_total_time;
        pclass_stat_node_tem->player_total_money=player_total_money;
        pclass_stat_node_tem->player_total_num=player_total_num;

        pclass_stat_node_tem->next=pclass_stat_node_ret;/*��������*/
        pclass_stat_node_ret=pclass_stat_node_tem;
    }
    SortClassInfo(pclass_stat_node_ret);/*����*/
    return pclass_stat_node_ret;
}

/**
 * ��������: SortClassInfo
 * ��������: �������Ϣ����������.
 * �������: class_phd �����Ϣ��ͷ���ָ��
 * �������: class_phd ������ͬʱͨ��ͷ���ָ�뷵��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SortClassInfo(CLASS_STAT_NODE *class_phd) {
    CLASS_STAT_NODE *pclass_stat_node_piror;
    CLASS_STAT_NODE *pclass_stat_node_ccurrent;
    CLASS_STAT_NODE *pclass_stat_node_after;
    CLASS_STAT_NODE *pclass_stat_node_tem;
    pclass_stat_node_piror=class_phd;
    if(pclass_stat_node_piror==NULL) {  /*����*/
        return;
    }
    pclass_stat_node_tem=(CLASS_STAT_NODE*)malloc(sizeof(CLASS_STAT_NODE));
    while(pclass_stat_node_piror->next!=NULL) {
        pclass_stat_node_ccurrent=pclass_stat_node_piror;
        pclass_stat_node_after=pclass_stat_node_piror->next;
        while(pclass_stat_node_after!=NULL) {
            if(pclass_stat_node_ccurrent->player_total_num<pclass_stat_node_after->player_total_num) {
                /*ʼ��ָ������*/
                pclass_stat_node_ccurrent=pclass_stat_node_after;
            }
            pclass_stat_node_after=pclass_stat_node_after->next;
        }
        if(pclass_stat_node_ccurrent!=pclass_stat_node_piror) {
            /*���������Ϣ��ָ���򲻱�*/
            *pclass_stat_node_tem=*pclass_stat_node_piror;
            *pclass_stat_node_piror=*pclass_stat_node_ccurrent;
            pclass_stat_node_piror->next=pclass_stat_node_tem->next;
            pclass_stat_node_tem->next=pclass_stat_node_ccurrent->next;
            *pclass_stat_node_ccurrent=*pclass_stat_node_tem;
        }
        pclass_stat_node_piror=pclass_stat_node_piror->next;
    }
    free(pclass_stat_node_tem);/*�ͷŴ洢��*/
    return;
}
/**
 * ��������: StatPlayerInfo ����ͳ��4
 * ��������: ͳ�������Ϣ.
 * �������: hd ����ͷ���ָ��
 * �������: ��
 * �� �� ֵ: ����ͳ�ƽ����ͷ����ַ
 *
 * ����˵��:�û�����������Ψһ
 */
PLAYER_STAT_NODE *StatPlayerInfo(CLASS_NODE *phd) {
    PLAYER_STAT_NODE *pplayer_stat_node_tem;
    PLAYER_STAT_NODE *pplayer_stat_node_ret=NULL;
    PLAYER_STAT_NODE *pplayer_stat_node_serch;
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*����ѭ������ʮ�ֽ�������*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            pplayer_node=pgame_node->pnext;
            while(pplayer_node!=NULL) {
                //puts(pplayer_node->user_name);
                pplayer_stat_node_serch=Serch_User_Name(pplayer_stat_node_ret,pplayer_node->user_name);
                if(pplayer_stat_node_serch!=NULL) {
                    /*������������Ѿ�����*/
                    //puts("find it");
                    pplayer_stat_node_serch->sum_money+=pplayer_node->total_money;
                    pplayer_stat_node_serch->sum_time+=pplayer_node->total_time;
                } else {
                    /*�����������û�У����½�*/
                    //puts("build it");
                    pplayer_stat_node_tem=(PLAYER_STAT_NODE*)malloc(sizeof(PLAYER_STAT_NODE));
                    strcpy(pplayer_stat_node_tem->user_name,pplayer_node->user_name);
                    pplayer_stat_node_tem->sum_time=pplayer_node->total_time;
                    pplayer_stat_node_tem->sum_money=pplayer_node->total_money;

                    pplayer_stat_node_tem->next=pplayer_stat_node_ret;/*����������*/
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

/**�������������Ƿ��д��û������Ƿ��ؽ�㣬û���򷵻�NULL*/
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
 * ��������: SortPlayerInfo
 * ��������: �������Ϣ����������.
 * �������: player_phd �����Ϣ��ͷ���ָ��
 * �������: player_phd ������ͬʱͨ��ͷ���ָ�뷵��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void SortPlayerInfo(PLAYER_STAT_NODE *player_phd) {
    PLAYER_STAT_NODE *pplayer_stat_node_piror;
    PLAYER_STAT_NODE *pplayer_stat_node_current;
    PLAYER_STAT_NODE *pplayer_stat_node_after;
    PLAYER_STAT_NODE *pplayer_stat_node_tem;
    pplayer_stat_node_piror=player_phd;
    if(pplayer_stat_node_piror==NULL) { /*����*/
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
            /*��������е���Ϣ��ָ���򲻱�*/
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
 * ��������: StatChargeInfo ����ͳ��5
 * ��������: ͳ���շ�ģʽ��Ϣ.
 * �������: hd ����ͷ���ָ��
 * �������: ��
 * �� �� ֵ: ����ͳ�ƽ����ͷ����ַ
 *
 * ����˵��:
 */
CHARGE_NODE *StatChargeInfo(CLASS_NODE *phd) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    CHARGE_NODE *pcharge_node_c,*pcharge_node_p,*pcharge_node_t,*pcharge_node;
    pcharge_node_c=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE)); /*����洢��*/
    pcharge_node_p=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE));
    pcharge_node_t=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE));
    pcharge_node_c->charge_model='c';   /*���շ�ģʽ*/
    pcharge_node_p->charge_model='p';
    pcharge_node_t->charge_model='t';
    pcharge_node_c->next=pcharge_node_p;    /*����ָ���ϵ*/
    pcharge_node_p->next=pcharge_node_t;
    pcharge_node_t->next=NULL;
    for(pcharge_node=pcharge_node_c; pcharge_node!=NULL; pcharge_node=pcharge_node->next) {
        pcharge_node->game_num=0;   /*����ֵ*/
        pcharge_node->playeer_total_time=0; /*����ֵ*/
        pcharge_node->player_total_money=0; /*����ֵ*/
        pcharge_node->player_total_num=0;   /*����ֵ*/
    }
    for(pclass_node=phd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            switch(pgame_node->charge_model[0]) {
            case 'c':
                /*��Ϸ���ݼ�����*/
                Game_Charge(pgame_node,pcharge_node_c);
                break;
            case 'p':
                /*��Ϸ���ݼ�����*/
                Game_Charge(pgame_node,pcharge_node_p);
                break;
            case 't':
                /*��Ϸ���ݼ�����*/
                Game_Charge(pgame_node,pcharge_node_t);
                break;
            }
            pgame_node=pgame_node->next;
        }
    }
    //SortChargeInfo(pcharge_node_c);
    return pcharge_node_c;/*����ͷָ��*/
}

/**ͳ��ָ����Ϸ��������������ʱ�������ܽ��,���ݼӵ���Ӧ�շ�ģʽ���������*/
void Game_Charge(GAME_NODE *game_node, CHARGE_NODE *pcharge) {
    double player_total_num=0;        /*<�����*/
    double player_total_time=0;      /*<�����Ϸ��ʱ��*/
    double player_total_money=0;      /*<��������ܽ��*/
    PLAYER_NODE *pplayer_node;
    pplayer_node=game_node->pnext;
    while(pplayer_node!=NULL) {
        player_total_num+=1;    /*����������*/
        player_total_money+=pplayer_node->total_money;
        player_total_time+=pplayer_node->total_time;
        pplayer_node=pplayer_node->next;
    }
    pcharge->playeer_total_time+=player_total_time;/*��ֵ*/
    pcharge->player_total_money+=player_total_money;/*��ֵ*/
    pcharge->player_total_num+=player_total_num;/*��ֵ*/
    pcharge->game_num+=1;/*ͳ����Ϸ����*/
}
///**
// * ��������: SortChargeInfo
// * ��������: ���շ�ģʽ��Ϣ����������.
// * �������: charge_phd �շ�ģʽ��Ϣ��ͷ���ָ��
// * �������: charge_phd ������ͬʱͨ��ͷ���ָ�뷵��
// * �� �� ֵ: ��
// *
// * ����˵��:
// */
//void SortChargeInfo(CHARGE_NODE *charge_phd)
//{
//    int i;
//    CHARGE_NODE *pcharge_node_tem;
//    CHARGE_NODE *pcharge_node_piror;
//    CHARGE_NODE *pcharge_node_current;
//    CHARGE_NODE *pcharge_node_after;
//    pcharge_node_piror=charge_phd;
//    if(pcharge_node_piror==NULL)    /*����*/
//    {
//        return;
//    }
//    pcharge_node_tem=(CHARGE_NODE*)malloc(sizeof(CHARGE_NODE));/*��ʱ���*/
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
//            /*��������е���Ϣ��ָ���򲻱�*/
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
 * ��������: SaveSysData
 * ��������: ���������������.
 * �������: hd ����ͷ���ָ��
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:
 */
BOOL SaveSysData(CLASS_NODE *hd) {
    CLASS_NODE *pclass_node;
    GAME_NODE *pgame_node;
    PLAYER_NODE *pplayer_node;
    FILE *pfout;
    int handle;
    pfout=fopen(gp_class_info_filename,"wb");
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*������Ϸ�����Ϣ*/
        fwrite(pclass_node,sizeof(CLASS_NODE),1,pfout);
    }
    fclose(pfout);

    pfout=fopen(gp_game_info_filename,"wb");
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*������Ϸ������Ϣ*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            fwrite(pgame_node,sizeof(GAME_NODE),1,pfout);
            pgame_node=pgame_node->next;
        }
    }
    fclose(pfout);

    pfout=fopen(gp_player_info_filename,"wb");
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*������һ�����Ϣ*/
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
 * ��������: BackupSysData
 * ��������: ������������ݱ��ݵ�һ�������ļ�.
 * �������: hd ����ͷ���ָ��
 *           filename �����ļ���
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE/
 *
 * ����˵��:
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
    /*����ʮ�������ֱ�ͳ�����ֻ�����Ϣ�ļ�¼����*/
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
    /*�������������Ϣ�ļ�¼����*/
    write(handle,(char*)&class_node_num,sizeof(class_node_num));
    //printf("%d",i);
    write(handle,(char*)&game_node_num,sizeof(game_node_num));
    //printf("%d",i);
    write(handle,(char*)&player_node_num,sizeof(player_node_num));
    //printf("%d",i);
    //printf("%d%d%d",class_node_num,game_node_num,player_node_num);

    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*������Ϸ�����Ϣ*/
        write(handle,(char*)pclass_node,sizeof(CLASS_NODE));
        //printf("%d",i);
    }
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*������Ϸ������Ϣ*/
        pgame_node=pclass_node->gnext;
        while(pgame_node!=NULL) {
            write(handle,(char*)pgame_node,sizeof(GAME_NODE));
            pgame_node=pgame_node->next;
        }
    }
    for(pclass_node=hd; pclass_node!=NULL; pclass_node=pclass_node->next) {
        /*������һ�����Ϣ*/
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
 * ��������: RestoreSysData
 * ��������: ��ָ�������ļ��лָ������������.
 * �������: phead ����ͷ���ָ��ĵ�ַ
 *           filename ��ű������ݵ������ļ���
 * �������:
 * �� �� ֵ: BOOL����, ����ΪTRUE
 *
 * ����˵��:���ȷ����Ϣ���Ƿ�ָ���
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
    /*��ȡ���ֻ�����Ϣ�ļ�¼��*/
    read(handle,(char*)&class_node_num,sizeof(class_node_num));
    read(handle,(char*)&game_node_num,sizeof(game_node_num));
    read(handle,(char*)&player_node_num,sizeof(player_node_num));
    //printf("%d%d%d",class_node_num,game_node_num,player_node_num);
    /*��ȡ��Ϸ�����Ϣ����������*/
    for(ulloop=1; ulloop<=class_node_num; ulloop++) {
        pclass_node=(CLASS_NODE*)malloc(sizeof(CLASS_NODE));
        read(handle,(char*)pclass_node,sizeof(CLASS_NODE));
        pclass_node->gnext=NULL;
        pclass_node->next=hd;
        hd=pclass_node;
    }
    *phead=hd;
    /*��ȡ��Ϸ������Ϣ��������Ϸ��Ϣ֧��*/
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
    /*��ȡ��һ�����Ϣ��������һ�����Ϣ֧��*/
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
    //SaveSysData(hd);    /*���ڴ������ݱ��浽�����ļ�*/
    return TRUE;
}

/**�������ڵĽ���*/
BOOL ShowModule(char **pString, int n) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;
    /*�ҳ�����ַ���*/
    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6; /*�ճ�����������Ҽ��*/
    pos.Y = n + 5;  /*�����ߺ���������*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*ȷ�ϵ������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;//| BACKGROUND_RED;  /*�׵׺���*/
    labels.num = n; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[n]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/

    for (i=0; i<n; i++) {
        aLoc[i].X = rcPop.Left + 3; /*���������鸳ֵ*/
        aLoc[i].Y = rcPop.Top + 2 + i;

    }
    str_len = strlen(pString[n-1]); /*���һ���ַ�����ȡ������λ�ã����м�*/
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[n-1].X, aLoc[n-1].Y,
            aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*�����߷ָ�*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}
/**����˵���ĵ������ڵĽ���*/
BOOL ShowModuleWithExp(char **pString, int n) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;
    /*�ҳ�����ַ���*/
    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6+10; /*�ճ�����������Ҽ��*/
    pos.Y = (n-1)/2 + 5;  /*+5��ʾ�����ߺ���������*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*ȷ�ϵ������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;//| BACKGROUND_RED;  /*�׵׺���*/
    labels.num = n; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[n]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/

    for (i=0; i<n; i+=2) {
        aLoc[i].X = rcPop.Left + 3; /*���������鸳ֵ*/
        aLoc[i].Y = rcPop.Top + 2 + i/2;
    }
    for (i=1; i<n; i+=2) {
        aLoc[i].X = rcPop.Left + 3+11; /*���������鸳ֵ,+11��ʾǰ���˵������*/
        aLoc[i].Y = rcPop.Top +2+ (i-1)/2;/*+1��ʾҪ��ǰ���˵������*/
    }
    str_len = strlen(pString[n-1]); /*���һ���ַ�����ȡ������λ�ã����м�*/
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2+3;/*+5��ʾǰ���˵��*/
    aLoc[n-1].Y = aLoc[n-1].Y + 1;

    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[n-1].X, aLoc[n-1].Y,
            aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*�����߷ָ�*/
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 +(n-1)/2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}

/**ȷ�ϻ���ȡ���������ڵĽ���*/
int ShowModuleYON(char **pString) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, str_len;
    int result=0;/*�����û���ѡ��1Ϊȷ����0Ϊȡ��*/
    pos.X = strlen(pString[0]) + 6; /*�ճ�����������Ҽ��*/
    pos.Y = 7;  /*�����ߺ���������*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*ȷ�ϵ������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN ;  /*�׵׺���*/
    labels.num = 2; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[]= {{rcPop.Left+4,rcPop.Top+2},
        {rcPop.Left+3,rcPop.Top+4},
        {rcPop.Left+9,rcPop.Top+4}
    }; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 2;  /*��������*/
    SMALL_RECT aArea[] = {{
            rcPop.Left+3,rcPop.Top+4,
            rcPop.Left+6,rcPop.Top+4
        },
        {
            rcPop.Left+9,rcPop.Top+4,
            rcPop.Left+12,rcPop.Top+4
        }
    };/*������λ*/

    char aSort[] = {0,0};/*��������*/
    char aTag[] = {1,2};/*�������*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*�����߷ָ�*/
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

/**���浯�����ڵĽ���*/
BOOL HShowModule(char **pString, int n) {
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;
    /*�ҳ�����ַ���*/
    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6; /*�ճ�����������Ҽ��*/
    pos.Y = n + 3;  /*�����ߺ���������*/
    rcPop.Left = (SCR_COL - pos.X) / 2; /*ȷ�ϵ������λ��*/
    rcPop.Right = rcPop.Left + pos.X - 1; /*-1����Ϊ��ʼ����Ϊ0*/
    rcPop.Top = (SCR_ROW - pos.Y) / 2-8;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_GREEN|BACKGROUND_RED ;
    labels.num = n; /*��ǩ���б�ǩ�ַ����ĸ���*/
    labels.ppLabel = pString; /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    COORD aLoc[n]; /*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/

    for (i=0; i<n; i++) {
        aLoc[i].X = rcPop.Left + 3; /*���������鸳ֵ*/
        aLoc[i].Y = rcPop.Top + 1 + i;

    }
    str_len = strlen(pString[n-1]); /*���һ���ַ�����ȡ������λ�ã����м�*/
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 1;

    labels.pLoc = aLoc;  /*ʹ��ǩ���ṹ����labels�ĳ�Աplocָ�������������Ԫ��*/

    areas.num = 1;  /*ֻ��һ������*/
    SMALL_RECT aArea[] = {{
            aLoc[n-1].X, aLoc[n-1].Y,
            aLoc[n-1].X + 3, aLoc[n-1].Y
        }
    };/*������λ��+3Ϊ�������ֵĳ���*/

    char aSort[] = {0};/*��������*/
    char aTag[] = {1};/*�������*/

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);
    /*�����߷ָ�*/
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
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/

    if (asc == 0) {
        arrow = 0;
        switch (vkc) {
        /*�����(���ϡ��ҡ���)�Ĵ���*/
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
