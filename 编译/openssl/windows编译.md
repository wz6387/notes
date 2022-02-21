编译环境：win10 64位系统

编译工具：vs2017



1、首先安装Perl

+ 安装包链接： [http://strawberryperl.com](http://strawberryperl.com/)

+ 下载32bit
+ 双击打开，按安装向导一步步安装即可

2、打开“VS 2017的开发人员命令提示符”，以管理员身份运行，依次执行下面命令

+ perl Configure VC-WIN32
+ nmake
+ nmake test
+ nmake install

