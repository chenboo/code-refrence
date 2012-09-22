//��һ���ַ���ת��Ϊһ��int
//������ַ���������һ����'\0'��β���ַ���������ת����������
//�����������string��ת������Ҫ����string��c_str()����

//Ϊ���ܹ��ڶ��ֽںͿ��ֽ�֮��ת��
//���ʹ��_ttoi

// crt_atoi.c
// This program shows how numbers 
// stored as strings can be converted to
// numeric values using the atoi functions.

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main( void )
{
    char    *str = NULL;
    int     value = 0;

    // An example of the atoi function.
    str = "  -2309 ";
    value = atoi( str );
    printf( "Function: atoi( \"%s\" ) = %d\n", str, value );

    // Another example of the atoi function.
    str = "31412764";
    value = atoi( str );
    printf( "Function: atoi( \"%s\" ) = %d\n", str, value );

    // Another example of the atoi function 
    // with an overflow condition occuring.
    str = "3336402735171707160320";
    value = atoi( str );
    printf( "Function: atoi( \"%s\" ) = %d\n", str, value );
    if (errno == ERANGE)
    {
       printf("Overflow condition occurred.\n");
    }
}
