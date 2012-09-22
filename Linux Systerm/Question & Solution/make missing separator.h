makefile:2: *** missing separator. Stop.
//错误：
//makefile内容:    
main.o   :main.c   
    gcc   -g   -c   main.c   
make   回车 
  
makefile:2:   ***   missing   separator.     Stop.   

//解决方式：
gcc   -g   -c   main.c,这行前面用tab键，不能用空格.

