# SZU_NetworkAutoConnector

## 目录
 - [功能概要](#功能概要)
 - [运行环境及依赖](#运行环境及依赖)
 - [使用说明](#使用说明)
 - [文件目录介绍](#文件目录介绍)

## 功能概要
  - 自动登录深大校园网，断连自动重连（自动识别教工区Wifi和宿舍WiFi）

## 运行环境及依赖
  - 本程序基于windows系统并依赖于curl实现校园网的登录

## 使用说明
  - 本程序不具备开机自启动的功能，需自行实现
  - 需要注意的是本程序需要读取配置文件以获得必要参数，故务必保证该程序在启动时能正确读取配置文件，否则程序将自动结束运行
  - 文件夹下`CampusNetwork AutoConnector.ini`文件即为上述的配置文件
  - 配置文件的内容格式如下
      - `需要连接的Wifi名称` `空格` `实现连接该Wifi的批处理脚本`
  - 例如
      - `SZU-Staff` `SZU-Staff.bat`
      - `SZU-Student` `SZU-Student.bat`
  - 在本程序运行时，会自动读取配置文件中的内容，然后根据配置文件中的内容进行连接Wifi的操作
## 文件目录介绍
  - `Resource`文件夹下包含配置文件`CampusNetwork AutoConnector.ini`、获取当前连接的wifi名称的比处理文件`GetWifiName.bat`、连接深大校园网的批处理文件`SZU-Staff.cmd`和`SZU-Student.bat`
  - `Source`文件夹存放着项目的源文件  `Main.cpp`





