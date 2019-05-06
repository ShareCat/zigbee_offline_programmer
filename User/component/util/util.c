

#include "./util.h"


/**
  * @brief  ���ַ��������еĿո��ַ�ȥ��
  * @param  str Ҫת�����ַ���
  * @retval null
  */
void my_str_remove_blank(char *str)
{
	char *c;//ָ���ַ������ַ���
	while (*str != ' ') {
		if (*str == '\0') {
			//�ַ�����û�пո�,ֱ�ӷ���
			return;
		}
		str++;
	}//��blankָ��ָ���һ���ո�λ��
	//��cָ��ָ����ֿո��ĵ�һ���ַ���
	c = str + 1;
	while (*c == ' ') {
		c++;
	}
	while (*c != '\0') {
		if (*c != ' ') {
			//���ո�Ϊ������ֵ��ַ�������ȥ���ַ���Ϊ�ո�
			*str = *c;
			*c = ' ';
			str++;
		}
		c++;
	}
	*str = '\0';
}


/**
  * @brief  ���ַ���ת����������������
  * @param  str Ҫת�����ַ���
  * @retval ����
  */
int my_atoi(char *str)
{
    int result = 0;
    int neg_multiplier = 1;

    // Scrub leading whitespace
    while(*str && (
                (*str == ' ') ||
                (*str == '\t')))
        str++;

    // Check for negative
    if(*str && *str == '-') {
        neg_multiplier = -1;
        str++;
    }

    // Do number
    for(; (*str) && (*str >= '0') && (*str <= '9'); str++) {
        result = (result * 10) + (*str - '0');
    }

    return result * neg_multiplier;
}


/**
  * @brief  ������ת�����ַ�����������
  * @param  value   ��ת��������
  * @param  string  ����ת�����ַ������
  * @param  radix   ת���Ľ��ƣ���2��8��10��16����
  * @retval ����ת�����ַ�����ַ��0��ʾ����
  */
char* my_itoa(int value, char* string, int radix)
{
    char tmp[33];
    char* tp = tmp;
    int i;
    unsigned v;
    int sign;
    char* sp;

    if (0 == string) return 0;

    if(radix > 36 || radix <= 1) {
        return 0;
    }

    sign = (radix == 10 && value < 0);

    if(sign)
        v = -value;
    else
        v = (unsigned)value;

    while(v || tp == tmp) {
        i = v % radix;
        v = v / radix;

        if(i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    sp = string;

    if(sign)
        *sp++ = '-';

    while(tp > tmp)
        *sp++ = *--tp;

    *sp = 0;
    return string;
}


#if 0   /* microsoft version, used malloc() func. */

#include <stdlib.h>

char* _itoa(int value, char* string, int radix)
{
    char tmp[33];
    char* tp = tmp;
    int i;
    unsigned v;
    int sign;
    char* sp;

    if(radix > 36 || radix <= 1) {
        __set_errno(EDOM);
        return 0;
    }

    sign = (radix == 10 && value < 0);

    if(sign)
        v = -value;
    else
        v = (unsigned)value;

    while(v || tp == tmp) {
        i = v % radix;
        v = v / radix;

        if(i < 10)
            *tp++ = i + '0';
        else
            *tp++ = i + 'a' - 10;
    }

    if(string == 0)
        string = (char*)malloc((tp - tmp) + sign + 1);

    sp = string;

    if(sign)
        *sp++ = '-';

    while(tp > tmp)
        *sp++ = *--tp;

    *sp = 0;
    return string;
}

#endif


/*********************************************END OF FILE**********************/
