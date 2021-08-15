# LAN-File-Transfer-Frontend

## 概述

本程序是应用在Linux的[文件传输工具](https://github.com/alvkeke/LAN-File-Transfer-Java )的命令行控制前端，本程序所使用的协议在该仓库中已经进行了说明，此处不再进行赘述，如有需要请自行查阅[文档](https://github.com/alvkeke/LAN-File-Transfer-Java/blob/master/README.md#控制协议 )。

## 使用方法

```shell
lft-ctrl [-p port] [-d ip] [cmd]
```

**目前只有raw模式可用**

### 命令行参数
* `-p` : 指定控制端口,默认10001
* `-d` : 指定设备IP,默认为本机(127.0.0.1)
* `cmd` : 要运行的命令, 若留空则进入repl
