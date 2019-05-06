/**
  ******************************************************************************
  * @file:      at.c
  * @author:    Cat�����ƽ��
  * @version:   V1.0
  * @date:      2018-5-9
  * @brief:     AT�����������
  * @attention:
  ******************************************************************************
  */


#include "./at.h"


static AT_S* head_at = 0;


#define AT_OK(p_at)     at_send_pack(p_at, "ok\r\n", sizeof("ok\r\n"))
#define AT_ERR(p_at)    at_send_pack(p_at, "err\r\n", sizeof("err\r\n"))
#define AT_BUSY(p_at)   at_send_pack(p_at, "busy\r\n", sizeof("busy\r\n"))


/**
  * @brief  ����һ���ֽڣ����ڻظ�AT���ͷ�
  * @param  p_at �������
  * @param  c Ҫ���͵�����
  * @retval null
  */
static void at_send_char(AT_S* p_at, uint8_t c)
{
    p_at->send_char(c);
}


/**
  * @brief  ����һ���������ڻظ�AT���ͷ�
  * @param  p_at �������
  * @param  data Ҫ���͵�����
  * @param  num Ҫ���͵ĸ���
  * @retval null
  */
static void at_send_pack(AT_S* p_at, uint8_t* data, uint16_t num)
{
    uint16_t i;

    for(i = 0; i < num; i++) {
        at_send_char(p_at, data[i]);
    }
}


/**
  * @brief  ��ȡһ���ֽڣ�������Դ�ĵط����յ��������ñ�������
  * @param  p_at �������
  * @param  c �յ���һ���ֽڣ���Ҫ��������
  * @retval null
  */
void at_get_char(AT_S* p_at, uint8_t c)
{
    if(p_at->sta != AT_STAT_PROCESS) {
        /* ����� */
        QUEUE_IN(p_at->rx_buff, c);
        /* ������ڽ��� */
        p_at->receive_flag = TRUE;
    } else {
        AT_BUSY(p_at);  /* ���ڽ������ظ�æ */
    }
}


/**
  * @brief  ��ȡ�յ�������ȣ�������"AT"
  * @param  p_cmd ����ָ��
  * @retval ��������ȣ�����-1��ʾ����
  */
static int8_t at_get_cmd_len(uint8_t* p_cmd)
{
    uint8_t n, i;

    n = 0;
    i = CMD_MAX_LEN;

    while(i--) {
        if((*p_cmd == '\r')
                || (*p_cmd == '=')
                || (*p_cmd == '?')
                || ((*p_cmd >= '0') && (*p_cmd <= '9'))) {
            /* ��������� */
            return n;
        } else {
            p_cmd++;
            n++;
        }
    }

    return -1;
}


/**
  * @brief  Ѱ��ƥ��������ִ�ж�Ӧ�ĺ���
  * @param  p_at �������
  * @retval null
  */
static void at_cmd_match(AT_S* p_at)
{
    uint8_t error = FALSE;
    int8_t len;
    uint8_t i;
    int16_t id = -1;

    /*
        ��һ���������û����������ĳ���
    */
    len = at_get_cmd_len(p_at->p_cmd);
    PRINTF("%s, len = %d\r\n", p_at->p_cmd, len);

    if(len == -1) return;

    /*
        �ڶ�����Ѱ��ƥ�������
    */
    for(i = 0; i < p_at->cmd_max; i++) {
        if(p_at->cmd_list[i].cmd_len == len) {
            if(0 == memcmp(p_at->p_cmd, p_at->cmd_list[i].cmd_name, len)) {
                id = i;
                break;  /* �ɹ��ҵ�ƥ����������������Ѱ���� */
            }
        }
    }

    if(id == -1) {      /* �������û���ҵ� */
        AT_ERR(p_at);
        return;
    }

    /*
        ��������ִ�������Ӧ�ĺ���
    */
    p_at->p_cmd += len;

    if((p_at->p_cmd[0] >= '0') && (p_at->p_cmd[0] <= '9') || (p_at->p_cmd[0] == '=')) {
        if(p_at->cmd_list[id].setup_func) {    /* ����֧��������� */
            p_at->cmd_list[id].setup_func((char*)p_at->p_cmd);
        } else {                                /* ���֧��������� */
            error = TRUE;
        }
    } else if(p_at->p_cmd[0] == '\r') {
        if(p_at->cmd_list[id].exe_func) {
            p_at->cmd_list[id].exe_func();
        } else {
            error = TRUE;
        }
    } else if((p_at->p_cmd[0] == '?') && (p_at->p_cmd[1] == '\r')) {
        if(p_at->cmd_list[id].query_func) {
            p_at->cmd_list[id].query_func();
        } else {
            error = TRUE;
        }
    } else if((p_at->p_cmd[0] == '=') && (p_at->p_cmd[1] == '?')  && (p_at->p_cmd[2] == '\r')) {
        if(p_at->cmd_list[id].test_func) {
            p_at->cmd_list[id].test_func();
        } else {
            error = TRUE;
        }
    } else {
        error = TRUE;
    }

    if(error == TRUE) {
        AT_ERR(p_at);
    } else {
        AT_OK(p_at);
    }
}


/**
  * @brief  �Խ��ջ������ݴ���Ѱ��AT��atͷ��
  * @param  p_at �������
  * @retval null
  */
static void at_process(AT_S* p_at)
{
    uint8_t temp;
    uint8_t atHead[2];
    uint8_t cmd_line[CMD_MAX_LEN];

    if(p_at->receive_flag == TRUE) {    /* ������ڽ��գ����Ȳ��ż����� */
        p_at->receive_flag = FALSE;
        return;
    }

    /*
        ��һ����Ѱ��"AT"��"at"
    */
    if(p_at->sta == AT_STAT_IDLE) {
        while(QUEUE_OUT(p_at->rx_buff, temp)) {
            atHead[0] = atHead[1];
            atHead[1] = temp;

            if((memcmp(atHead, "AT", 2) == 0) || (memcmp(atHead, "at", 2) == 0)) {
                p_at->p_cmd = cmd_line;
                p_at->sta = AT_STAT_RECEIVE;
                break;  /* �ɹ��ҵ�AT������whileѭ�� */
            } else if(temp == '\n') {
                /* ֻ�յ��س� */
                AT_ERR(p_at);
            }
        }
    }

    /*
        �ڶ���������ɹ��ҵ�"AT"��"at"����ʼ��������������ڽ����������յ��κ�
        �����������Ӧ
    */
    if(p_at->sta == AT_STAT_RECEIVE) {
        while(QUEUE_OUT(p_at->rx_buff, temp)) {
            *p_at->p_cmd = temp;

            if(temp == '\n') {
                p_at->p_cmd++;
                *p_at->p_cmd = '\0';
                p_at->p_cmd = cmd_line;    /* ����ָ���׵�ַ */
                p_at->sta = AT_STAT_PROCESS;
                break;  /* �յ��س�������whileѭ�� */
            } else if(p_at->p_cmd >= &cmd_line[CMD_MAX_LEN - 1]) {  /* ��ʱ�������� */
                p_at->sta = AT_STAT_IDLE;
                break;  /* ����̫��������whileѭ�� */
            }

            p_at->p_cmd++;
        }

        if(p_at->sta == AT_STAT_PROCESS) {
            at_cmd_match(p_at);
        }

        p_at->sta = AT_STAT_IDLE;
    }
}

/**
  * @brief  ATע�ᣬ����ע������������
  * @param  p_at AT����
  * @param  p_at_cmd_list AT�����б�
  * @param  cmd_num AT�������
  * @param  p_send_char ����һ���ַ��ӿڣ����ڻظ�AT���ͷ�
  * @retval 0��ʾ�ɹ���1��ʾ�Ѿ�ע�����
  */
uint8_t at_init(AT_S* p_at,
                const AT_CMD_S* p_at_cmd_list,
                uint8_t cmd_num,
                void(*p_send_char)(uint8_t c))
{
    AT_S* target = head_at;

    p_at->rx_buff.Front = 0;
    p_at->rx_buff.Rear = 0;
    p_at->sta = AT_STAT_IDLE;
    p_at->receive_flag = FALSE;
    p_at->send_char = p_send_char;
    p_at->cmd_list = p_at_cmd_list;
    p_at->cmd_max = cmd_num;

    while(target) {
        if(target == p_at) return 1; //already exist.

        target = target->next;
    }

    p_at->next = head_at;
    head_at = p_at;
    return 0;
}

/**
  * @brief  AT������������ɵ�����20ms����һ��
  * @param  null
  * @retval null
  */
void at_task(void)
{
    AT_S* target;

    for(target = head_at; target; target = target->next) {
        at_process(target);
    }
}


/*********************************************END OF FILE**********************/
