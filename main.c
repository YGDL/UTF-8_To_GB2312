#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "cp936.h"

#define uint8_t unsigned char
#define uint16_t unsigned short int
#define uint32_t unsigned int

#define int8_t char

uint16_t SearchCodeTable(uint16_t unicodeKey)
{
	int first = 0;					//表头
	int end = CODE_TABLE_SIZE - 1;	//表尾
	int mid = 0;
	while (first <= end)			//二分法查表时间复杂度O(logn)
	{
		mid = (first + end) / 2;
		if (code_table[mid].unicode == unicodeKey)
			return code_table[mid].gb;
		else if (code_table[mid].unicode > unicodeKey)
			end = mid - 1;
		else
			first = mid + 1;
	}
	return 0;
}

uint8_t *UTF8_TO_GB2312(const uint8_t *str)
{
	uint8_t i = 0U;
	uint8_t number = 0U;
	int8_t head = 0;
	uint16_t str1 = 0U;
	uint8_t *str2 = NULL;
	uint8_t *str3 = NULL;
	uint8_t str_length = strlen(str);

	str2 = (uint8_t *)malloc(sizeof(uint8_t) * (str_length + 1)); //加1防止越界

	while (*(str + i) != 0) //判断字符串结束符
	{
		head = (*(str + i) & 0xF0) >> 4; //判断字符字节数3byte、2byte、1byte
		if (head > 7)
			if (head > 13)
				head = 3;
			else
				head = 2;
		else
			head = 1;
		//条件初步筛选，减轻查表复杂度
		switch (head)
		{
		case 1:
			str1 = *(str + i);
			*(str2 + number) = str1;
			printf("0x%X\n", *(str2 + number));
			i = i + 1;				//待转换字节数
			number = number + 1;	//转换后字符所占字节数
			break;

		case 2:
			str1 = ((*(str + i) & 0x1F) << 6) | (*(str + i + 1) & 0x3F);
			str1 = SearchCodeTable(str1);
			*(str2 + number) = str1 >> 8 & 0xFF;
			*(str2 + number + 1) = str1 & 0xFF;
			printf("0x%X%X\n", *(str2 + number), *(str2 + number + 1));
			i = i + 2;
			number = number + 2;
			break;

		case 3:
			str1 = ((*(str + i) & 0x0F) << 12) | ((*(str + i + 1) & 0x3F) << 6) | (*(str + i + 2) & 0x3F);
			str1 = SearchCodeTable(str1);			//只有三字节字符有表查
			*(str2 + number) = str1 >> 8 & 0xFF;
			*(str2 + number + 1) = str1 & 0xFF;
			printf("0x%X%X\n", *(str2 + number), *(str2 + number + 1));
			i = i + 3;
			number = number + 2;
			break;

		default:
			printf("Illegal characters!\n");
			i = i + 1;
			number = number + 1;
			break;
		}
	}
	*(str2 + number) = '\0';	//字符串结尾
	str3 = (uint8_t *)malloc(strlen(str2) + 1);
	strcpy(str3, str2);
	printf("The character length is %d.\n", strlen(str3));
	//长度判断是以字符串结束符\0为止，不将结束符算在内
	printf("%s\n", str3);
	free(str2);					//释放堆内存
	return str3;
}

int8_t main()
{
	uint8_t str[] = "你好呀！！！！";
	uint8_t *str1 = NULL;
	str1 = UTF8_TO_GB2312(str);
	free(str1);
	str1 = NULL;
	printf("%s", str1);
	//free之后堆内存中内容无变化，但是被交回系统中，一段时间过后可能会被系统改变
	//为了防止非法写入，可以在free之后将其指向空指针
	return 0;
}