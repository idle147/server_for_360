## 行车记录仪服务端

### 版权说明

本项目为个人练手项目，代码均为本人手写，仅供技术交流，请勿用于商业用途。



### 项目说明

本项目是一个单机版的行车记录仪的服务端，用vs、C++语言编写，在 Linux 环境下运行。实现服务器的【数据交互】【文件传输】【安全通信】等功能。



### 功能介绍

![image-20210907161542424](C:\Users\61042\AppData\Roaming\Typora\typora-user-images\image-20210907161542424.png)

### 架构说明

![image-20210907161619255](C:\Users\61042\AppData\Roaming\Typora\typora-user-images\image-20210907161619255.png)



### IPC通信

![image-20210907161824802](C:\Users\61042\AppData\Roaming\Typora\typora-user-images\image-20210907161824802.png)



### 通信协议

![image-20210907161703906](C:\Users\61042\AppData\Roaming\Typora\typora-user-images\image-20210907161703906.png)



![image-20210907161939841](C:\Users\61042\AppData\Roaming\Typora\typora-user-images\image-20210907161939841.png)



### 软件环境说明

（1）软件语言：C/C++；

（2）开发工具：vs2019

（3）运行系统：linux；

（4）数据库：sqlite3；



### 配置文件说明

配置文件路径：/bin/config/serverConfig.json
配置文件说明:所有的路径文件,均相对运行程序而言

```
如:运行程序位于: /home/bin/程序.exe
日志路径: ../config/日志.txt
那日志文件实际位于: /home/config/日志.txt
```

配置文件详细描述:

![1](D:\Desktop\1.png)

![2](D:\Desktop\2.png)



