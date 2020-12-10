#ifndef CCL_H
#define CCL_H
 
#define KEYVALLEN 256
 
/*   删除左边的空格   */
char * l_trim(char * szOutput, const char *szInput);
 
/*   删除右边的空格   */
char *r_trim(char *szOutput, const char *szInput);
 
/*   删除两边的空格   */
char * a_trim(char * szOutput, const char * szInput);
 
/*   
函数 GetProfileString:	获取字符串键值
参数	profile:	文件描述符
参数	KeyName:	键值名
参数	KeyVal：		键值存储地址
返回 int:		0=正常，-1=错误
*/
int GetProfileString(char *profile, char *KeyName, char *KeyVal );


/*   
函数 GetProfileInt:	获取整型键值
参数	profile:	文件描述符
参数	KeyName:	键值名
参数	KeyVal：		键值存储地址
返回 int:		0=正常，-1=错误
*/
int GetProfileInt(char *profile, char *KeyName, int *Keyval );
 
 
#endif //CCL_H