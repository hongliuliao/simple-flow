simple_flow
===========

simple realtime flow computation 

## 简介
simple_flow 是一个简单的实时流计算框架,通过拉取日志,自己定制flow_handler实现实时计算

## 特点
* linux only
* 继承simple系列的传统,简洁实用
* 安全,可靠 -- 采用拉模式,对数据源系统依赖,影响小

## 以下信息已过期,近期进行更新

## 依赖
 * [simple_log](https://github.com/hongliuliao/simple_log) 日志组件
 * [simple_server](https://github.com/hongliuliao/simple_server) use it epoll_socket

## 构建 && 测试
```
  make && make log_flow_server # 此步骤会生成一个agent和一个测试用的flow_server
  ./bin/log_flow_server # start flow_server to receive flow
  ./bin/fileagent localhost 3491 /tmp/test.log # usage: ./bin/fileagent flow_server_ip flow_server_port file_path
```

## 代码示例
```c++
#include <fstream>
#include "simple_log.h"
#include "flow_handler.h"
#include "flow_server.h"

class LogFlowHandler : public FlowHandler {

public:
	int do_handle(char *flow_bytes, int size) {
		std::string receive_flow = std::string(flow_bytes, size);
		LOG_DEBUG("start handle the size:%d, flows %s", size, receive_flow.c_str());
		return 0;
	}
};

int main() {
	FlowServer flow_server;
	flow_server.start(3491, new LogFlowHandler());
	return 1;
}

```
## TODO LIST
 * ~~fileagent实现当文件inode变化时,重新读取新文件~~ DONE at 2014-11-13 check the newest file in dir when no new data
 * ~~epoll方式实现 flow_server~~ DONE at 2014-11-10 
