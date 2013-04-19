/****************************************Copyright (c)**************************************************
**IniFile.c - ini�ļ����ʿ⺯��������linux��ʹ��
**Copyright (C) 2005 XiaoJianXin, xjx-jx@163.com
**
**�˿���������������Ա��޸Ļ����·��������뱣�����˵��!
********************************************************************************************************/
#ifndef _IniFile_H
#define _IniFile_H

struct Stu_Idents
{
	char *Ident_name ;                  /*��һ���Ⱥ�ǰ���ַ���*/
	char *Ident_value ;                 /*��һ���Ⱥź���ַ���*/
	short flag_invalidIdent ;           /*������Ч��־;0 ����Ч��1 ����Ч*/
	struct Stu_Idents *next ;			/*ָ����һ��������*/
	struct Stu_Idents *prior ;			/*ָ��ǰһ��������*/
};

struct Stu_Sections
{
	char *Section_name ;				/*���������ż���ַ���*/
	short flag_invalidSection ;         /*�ε���Ч��־*/
	struct Stu_Idents *idents ;
	struct Stu_Sections *next ;
	struct Stu_Sections *prior ;
};

extern struct Stu_Sections *IniFileData;

int IniFileInitialize(const char *IniFileName) ;	    /*INI�ļ������ʼ��*/

char *IniFileGetStr(const char *Section_name,const char *Ident_name,const char *default_value) ;		/*ȡINI�ļ���Section_name����Ident_name�����ַ���ֵ������ֵ��ȡ�����ַ����ͼ�ֵ��default_value��ָ��ȱʡֵ*/
int IniFileGetInteger(const char *Section_name,const char *Ident_name,const int default_value) ;      /*ȡINI�ļ���Section_name����Ident_name��������ֵ������ֵ��ȡ�������ͼ�ֵ��default_value��ָ��ȱʡֵ*/
long IniFileGetlong(const char *Section_name,const char *Ident_name,const long default_value) ;
float IniFileGetFloat(const char *Section_name,const char *Ident_name,const float default_value) ;	/*ȡINI�ļ���Section_name����Ident_name���ĸ�����ֵ������ֵ��ȡ���ĸ����ͼ�ֵ��default_value��ָ��ȱʡֵ*/
double IniFileGetdouble(const char *Section_name,const char *Ident_name,const double default_value) ;
void IniFilePrint() ;           /*����Ļ���INI�ļ�������*/

int IniFileCleanUp() ;          /*INI�ļ���������ر��ļ�ǰ��������*/


#endif
