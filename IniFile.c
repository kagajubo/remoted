/****************************************Copyright (c)**************************************************
**IniFile.c - ini文件访问库函数，方便linux下使用
**Copyright (C) 2005 XiaoJianXin, xjx-jx@163.com
**
**此库是自由软件，可以被修改或重新发布，但请保留这段说明!
********************************************************************************************************/
#include <stdio.h>
#include <ctype.h>
/*
#include <unistd.h>
*/
#include <string.h>
#include <stdlib.h>
//#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tools.h"
#include "IniFile.h"

struct Stu_Sections *IniFileData;

/*AUTHOR    : XinJianXin*/
/* Return the integer value of a positive integer written in S, or 0
   if an error was encountered.  */
int myatoi(const char *s)
{
  int res;
  const char *orig = s;

  for (res = 0; *s && isdigit (*s); s++)
    res = 10 * res + (*s - '0');
  if (*s || orig == s)
    return 0;
  else
    return res;
}

/*AUTHOR    : XinJianXin*/
/* Return the integer value of a positive integer written in S, or 0
   if an error was encountered.  */
long myatol(const char *s)
{
  long res;
  const char *orig = s;

  for (res = 0; *s && isdigit (*s); s++)
    res = 10 * res + (*s - '0');
  if (*s || orig == s)
    return 0;
  else
    return res;
}

/*
AUTHOR    : XinJianXin
功能:保存INI文件的段名行，此行以字符'['开始，字符']'结束
参数：
    lineStr : 字符串型 ；需处理的一行字符串
返回值：
    返回值:整型,当处理过程中有异常时返回非0异常代码；否则返回0 ；
*/
int parse_sectionline (const char *lineStr)
{
    int val_ret ;
    struct Stu_Sections *IniFileData_p,*newSection ;
    val_ret = 0 ;
    if(lineStr[0]=='[' && lineStr[strlen(lineStr)-1]==']')
    {
        newSection = xmalloc("parse_sectionline", sizeof(struct Stu_Sections)) ;
	    newSection->flag_invalidSection =0 ;

        newSection->Section_name=xmalloc("parse_sectionline", strlen(lineStr)+1);
        strcpy(newSection->Section_name,lineStr+1);
        newSection->Section_name[strlen(newSection->Section_name)-1]='\0' ;
        trim(newSection->Section_name);

        newSection->idents = NULL ;
        newSection->next = NULL ;
        newSection->prior = NULL ;
        if(!IniFileData)    /*刚开始无数据时*/
        {
            IniFileData = newSection ;
        }
        else
        {
            IniFileData_p = IniFileData ;
            while(IniFileData_p->next)
                IniFileData_p = IniFileData_p->next ;
	        IniFileData_p->next = newSection ;
        }

    }
    else
	val_ret = 100 ;     /*非段名错误代码*/
    return val_ret ;
}

/*
AUTHOR    : XinJianXin
功能:保存INI文件的键值行，所有不是段名的行都被当做键值行
参数：
    lineStr : 字符串型 ；需处理的一行字符串
返回值：
    返回值:整型,当处理过程中有异常时返回非0异常代码；否则返回0 ；
*/
int parse_identline(const char *lineStr)
{
    int val_ret,i ;
    struct Stu_Sections *IniFileData_p,*newSection ;
    struct Stu_Idents *PIdents,*newIdent ;
    val_ret = 0 ;
    if(!IniFileData)    /*刚开始无数据时添加一个段名为空的段*/
    {
        newSection = xmalloc("parse_identline4", sizeof(struct Stu_Sections)) ;
        newSection->flag_invalidSection = 1 ;

        newSection->Section_name=xmalloc("parse_sectionline",1);
        newSection->Section_name[0]='\0' ;

        newSection->idents = NULL ;
        newSection->next = NULL ;
        newSection->prior = NULL ;
        IniFileData_p = newSection ;
        IniFileData = newSection ;

    }
    else
    {
        IniFileData_p = IniFileData ;
        while(IniFileData_p->next)
            IniFileData_p = IniFileData_p->next ;
    }
    newIdent = xmalloc ("parse_identline3",sizeof(struct Stu_Idents));
    newIdent->flag_invalidIdent = 0 ;
    newIdent->Ident_name = NULL ;
    newIdent->Ident_value = NULL ;
    newIdent->next = NULL ;
    newIdent->prior = NULL ;

    if(IniFileData_p->idents)	/*段下有键值数据时*/
    {
        PIdents=IniFileData_p->idents ;
        while(PIdents->next)
            PIdents=PIdents->next ;
        PIdents->next = newIdent ;
        newIdent->prior = PIdents ;
    }
    else
        IniFileData_p->idents = newIdent ;

    for(i=0;i<strlen(lineStr);i++)
    {
        if(lineStr[i]=='=')
            break ;
    }
    if(lineStr[0]=='#')         /*字符'#'号开头的行作为注释行处理（被当作一种特殊的无效键行）*/
        i=strlen(lineStr) ;
    newIdent->Ident_name = xmalloc ("parse_identline1",i+1);
    strncpy(newIdent->Ident_name,lineStr,i) ;
    newIdent->Ident_name[i]='\0' ;
    trim(newIdent->Ident_name);
    printf("%d\n",strlen(lineStr)-i) ;
    newIdent->Ident_value = xmalloc ("parse_identline2",strlen(lineStr)-i+1);
    if(strlen(lineStr)>i)
    {
        printf("lineStr long=%d\n",strlen(lineStr)-i-1) ;
        strcpy(newIdent->Ident_value,lineStr+i+1) ;
    }
    else
    {
        newIdent->Ident_value[0]='\0' ;
        newIdent->flag_invalidIdent = 1 ;
    }
    trim(newIdent->Ident_value);
    return val_ret ;
}


/*
AUTHOR    : XinJianXin
功能:处理INI文件的每行字符串
参数：
    lineStr : 字符串型 ；需处理的一行字符串
返回值：
    返回值:整型,当处理过程中有异常时返回非0异常代码；否则返回0 ；
*/
int parse_line(const char *lineStr)
{
    int flag_section,val_ret;
    char *templineStr ;
    val_ret = 0 ;
    templineStr = (char *) xmalloc ("file", strlen(lineStr)+ 1);
    strcpy(templineStr,lineStr) ;
    strstrip(templineStr) ;
    /*头尾分别为字符'['和']'时是段名行，或文件开始不在段名下的行，作为空段名下的无效键值来处理;其它作为键值行处理*/
    /*if((lineStr[0]=='[' && lineStr[strlen(templineStr)-1]==']')||IniFileData==NULL)
        flag_section = 1 ;
    else
        flag_section = 0 ;
    if(flag_section)
        val_ret = parse_sectionline(lineStr) ;  /*作段名处理过程*/
    /*else
        val_ret = parse_identline(lineStr) ;    /*作键值处理过程*/
    val_ret = parse_sectionline(lineStr);
    if(val_ret)
        val_ret = parse_identline(lineStr) ;
    return val_ret ;
}

/*
AUTHOR    : XinJianXin
功能:此函数是INI文件处理的初始化过程，必须在其它INI文件操作前调用；
参数:
    IniFileName ：字符串型，INI文件的名称,可带路径;
返回值:整型,当处理过程中有异常时返回非0异常代码；否则返回0 ；
*/
int IniFileInitialize(const char *IniFileName)
{
    char  *logfile;
    FILE *fp;
    char *line;
    int ln;
    IniFileData = NULL ;
    fp = fopen (IniFileName, "rb");
    if (fp == NULL)
	err_quit ("Cannot read file %s !", IniFileName);
    ln = 1;
    while ((line=read_whole_line (fp)))
    {
        char *com, *val;
        int status;
        int length = strlen (line);
        if (length && line[length - 1] == '\r')
            line[length - 1] = '\0';
        /* Parse the line.  */
	    status = parse_line(line);
        free (line);
        /* If everything is OK, set the value.  */
	    if (status)
		    printf ("Error in %s at line %d.\n", IniFileName, ln);
	    /*   logprintf ("Error in %s at line %d.\n", IniFileName, ln);*/
        ++ln;
    }
    fclose (fp);
}

/*AUTHOR    : XinJianXin*/
void IniFilePrint()
{
	struct Stu_Idents *PIdent ;
	struct Stu_Sections *PSection ;
	PSection=IniFileData ;
	printf("===================start print ini===================\n") ;
	while(PSection)
	{
	    if(!PSection->flag_invalidSection)
		    printf("[%s]\n",PSection->Section_name) ;
	    else
	        if(strlen(PSection->Section_name)>0)
		        printf("%s\n",PSection->Section_name) ;

		PIdent = PSection->idents ;
		while(PIdent)
		{
			if(!PIdent->flag_invalidIdent)
				printf("%s=%s\n",PIdent->Ident_name,PIdent->Ident_value) ;
			else
				printf("%s\n",PIdent->Ident_name) ;
			PIdent=PIdent->next ;
		}
		PSection=PSection->next ;
	}
	printf("====================end print ini====================\n") ;
}

/*AUTHOR    : XinJianXin*/
char *IniFileGetStr(const char *Section_name,const char *Ident_name,const char *default_value)
{
    struct Stu_Sections *PSection ;
    struct Stu_Idents *PIdent ;
    char *val_ret ;
    PSection = IniFileData ;
    PIdent = NULL ;
    val_ret = NULL ;
    while(PSection)
    {
        if(!strcmp(Section_name,PSection->Section_name)&&!PSection->flag_invalidSection)
        {
            PIdent=PSection->idents ;
            break ;
        }
        PSection=PSection->next ;
    }
    while(PIdent)
    {
        if(!strcmp(Ident_name,PIdent->Ident_name)&&!PIdent->flag_invalidIdent)
        {
            val_ret=xmalloc ("IniFileGetStr",strlen(PIdent->Ident_value)+1);
            strcpy(val_ret,PIdent->Ident_value) ;
            break ;
        }
        PIdent=PIdent->next ;
    }
    if(!val_ret)
    {
        val_ret=xmalloc ("IniFileGetStr",strlen(default_value)+1);
        strcpy(val_ret,default_value) ;
    }
    return val_ret ;
}

/*AUTHOR    : XinJianXin*/
int IniFileGetInteger(const char *Section_name,const char *Ident_name,const int default_value)
{
    struct Stu_Sections *PSection ;
    struct Stu_Idents *PIdent ;
    int val_ret , i ;
    PSection = IniFileData ;
    PIdent = NULL ;
    val_ret = default_value ;
    while(PSection)
    {
        if(!strcmp(Section_name,PSection->Section_name)&&!PSection->flag_invalidSection)
        {
            PIdent=PSection->idents ;
            break ;
        }
        PSection=PSection->next ;
    }
    while(PIdent)
    {
        if(!strcmp(Ident_name,PIdent->Ident_name)&&!PIdent->flag_invalidIdent)
        {
            val_ret= atoi(PIdent->Ident_value);
            if(val_ret==0)
            {
                for(i=0;i<strlen(PIdent->Ident_value);i++)
                    if(PIdent->Ident_value[i]!='0')
                    {
                        val_ret = default_value ;
                        break ;
                    }
            }
            break ;
        }
        PIdent=PIdent->next ;
    }
    return val_ret ;
}

/*AUTHOR    : XinJianXin*/
long IniFileGetlong(const char *Section_name,const char *Ident_name,const long default_value)
{
    struct Stu_Sections *PSection ;
    struct Stu_Idents *PIdent ;
    long val_ret ;
    int i ;
    PSection = IniFileData ;
    PIdent = NULL ;
    val_ret = default_value ;
    while(PSection)
    {
        if(!strcmp(Section_name,PSection->Section_name)&&!PSection->flag_invalidSection)
        {
            PIdent=PSection->idents ;
            break ;
        }
        PSection=PSection->next ;
    }
    while(PIdent)
    {
        if(!strcmp(Ident_name,PIdent->Ident_name)&&!PIdent->flag_invalidIdent)
        {
            val_ret= atol(PIdent->Ident_value);
            if(val_ret==0)
            {
                for(i=0;i<strlen(PIdent->Ident_value);i++)
                    if(PIdent->Ident_value[i]!='0')
                    {
                        val_ret = default_value ;
                        break ;
                    }
            }
            break ;
        }
        PIdent=PIdent->next ;
    }
    return val_ret ;
}

/*AUTHOR    : XinJianXin*/
float IniFileGetFloat(const char *Section_name,const char *Ident_name,const float default_value)
{
    struct Stu_Sections *PSection ;
    struct Stu_Idents *PIdent ;
    float val_ret ;
    int i ;
    PSection = IniFileData ;
    PIdent = NULL ;
    val_ret = default_value ;
    while(PSection)
    {
        if(!strcmp(Section_name,PSection->Section_name)&&!PSection->flag_invalidSection)
        {
            PIdent=PSection->idents ;
            break ;
        }
        PSection=PSection->next ;
    }
    while(PIdent)
    {
        if(!strcmp(Ident_name,PIdent->Ident_name)&&!PIdent->flag_invalidIdent)
        {
            val_ret= atof(PIdent->Ident_value);
            if(val_ret==0)
            {
                for(i=0;i<strlen(PIdent->Ident_value);i++)
                    if(PIdent->Ident_value[i]!='0')
                    {
                        val_ret = default_value ;
                        break ;
                    }
            }
            break ;
        }
        PIdent=PIdent->next ;
    }
    return val_ret ;
}

/*AUTHOR    : XinJianXin*/
double IniFileGetdouble(const char *Section_name,const char *Ident_name,const double default_value)
{
    struct Stu_Sections *PSection ;
    struct Stu_Idents *PIdent ;
    float val_ret ;
    int i ;
    PSection = IniFileData ;
    PIdent = NULL ;
    val_ret = default_value ;
    while(PSection)
    {
        if(!strcmp(Section_name,PSection->Section_name)&&!PSection->flag_invalidSection)
        {
            PIdent=PSection->idents ;
            break ;
        }
        PSection=PSection->next ;
    }
    while(PIdent)
    {
        if(!strcmp(Ident_name,PIdent->Ident_name)&&!PIdent->flag_invalidIdent)
        {
            /*val_ret= strtod(PIdent->Ident_value);*/
            if(val_ret==0)
            {
                for(i=0;i<strlen(PIdent->Ident_value);i++)
                    if(PIdent->Ident_value[i]!='0')
                    {
                        val_ret = default_value ;
                        break ;
                    }
            }
            break ;
        }
        PIdent=PIdent->next ;
    }
    return val_ret ;
}

/*
AUTHOR    : XinJianXin
功能:此函数是INI文件操作结束后的清理；
参数:
    无;
返回值:整型,当处理过程中有异常时返回非0异常代码；否则返回0 ；
*/
int IniFileCleanUp()
{
    struct Stu_Idents *PIdent ;
	struct Stu_Sections *PSection ;
	PSection=IniFileData ;
	while(PSection)
	{
		PIdent = PSection->idents ;
		while(PIdent)
		{
		    if(PIdent->Ident_name)
		        free(PIdent->Ident_name) ;
		    if(PIdent->Ident_value)
		        free(PIdent->Ident_value) ;
		    PSection->idents=PIdent->next ;
		    free(PIdent) ;
			PIdent=PSection->idents ;
		}
		if(PSection->Section_name)
		    free(PSection->Section_name) ;
		IniFileData = PSection->next ;
		free(PSection) ;
		PSection=IniFileData ;
	}
}

