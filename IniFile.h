/****************************************Copyright (c)**************************************************
**IniFile.c - ini文件访问库函数，方便linux下使用
**Copyright (C) 2005 XiaoJianXin, xjx-jx@163.com
**
**此库是自由软件，可以被修改或重新发布，但请保留这段说明!
********************************************************************************************************/
#ifndef _IniFile_H
#define _IniFile_H

struct Stu_Idents
{
	char *Ident_name ;                  /*第一个等号前的字符串*/
	char *Ident_value ;                 /*第一个等号后的字符串*/
	short flag_invalidIdent ;           /*键的有效标志;0 表有效，1 表无效*/
	struct Stu_Idents *next ;			/*指向下一个键内容*/
	struct Stu_Idents *prior ;			/*指向前一个键内容*/
};

struct Stu_Sections
{
	char *Section_name ;				/*左右中括号间的字符串*/
	short flag_invalidSection ;         /*段的有效标志*/
	struct Stu_Idents *idents ;
	struct Stu_Sections *next ;
	struct Stu_Sections *prior ;
};

extern struct Stu_Sections *IniFileData;

int IniFileInitialize(const char *IniFileName) ;	    /*INI文件处理初始化*/

char *IniFileGetStr(const char *Section_name,const char *Ident_name,const char *default_value) ;		/*取INI文件中Section_name段中Ident_name键的字符串值，返回值是取到的字符串型键值，default_value是指定缺省值*/
int IniFileGetInteger(const char *Section_name,const char *Ident_name,const int default_value) ;      /*取INI文件中Section_name段中Ident_name键的整型值，返回值是取到的整型键值，default_value是指定缺省值*/
long IniFileGetlong(const char *Section_name,const char *Ident_name,const long default_value) ;
float IniFileGetFloat(const char *Section_name,const char *Ident_name,const float default_value) ;	/*取INI文件中Section_name段中Ident_name键的浮点型值，返回值是取到的浮点型键值，default_value是指定缺省值*/
double IniFileGetdouble(const char *Section_name,const char *Ident_name,const double default_value) ;
void IniFilePrint() ;           /*在屏幕输出INI文件的内容*/

int IniFileCleanUp() ;          /*INI文件处理结束关闭文件前的清理功能*/


#endif
