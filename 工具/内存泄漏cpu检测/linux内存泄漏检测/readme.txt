Valgrind Memcheck һ��ǿ��Դ�ĳ����⹤��

���ص�ַ��http://valgrind.org/downloads/current.html

Valgrind��������ָ�ϣ�http://valgrind.org/docs/manual/QuickStart.html

 

��װ

cd valgrind

./autogen.sh

./configure

make

make install

ʹ��

1��������ĳ��� debug �汾./TestMem

2��ִ�У�valgrind --tool=memcheck --leak-check=full --log-file=./log.txt  ./TestMem

 

mtrace GNU��չ����������malloc��mtraceΪ�ڴ���亯��(malloc,rellaoc,memalign,free)��װhook����
dmalloc ���ڼ��C/C++�ڴ�й©�Ĺ��ߣ����Ǽ���Ƿ���ڳ������н�����û���ͷŵ��ڴ棬��һ�����пⷢ��
memwatch ��dmallocһ�������ܼ��δ�ͷŵ��ڴ桢ͬһ���ڴ汻�ͷŶ�Ρ�λַ��ȡ���󼰲���ʹ��δ����֮�ڴ�����
mpatrol һ����ƽ̨�� C++ �ڴ�й©�����
dbgmem Ҳ��һ����̬�ⷢ������ʽ���ŵ�����dmalloc���������֮�£������ص�����һЩ
Electric Fence �������ܹ�����malloc()��free(),ͬʱ�ܹ����������Լ�д�룬�ܹ�׼ȷָ�����´����ָ��