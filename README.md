simple_flow
===========

simple realtime flow computation 

## 简介
simple_flow 是一个简单的实时流计算框架,由两部分组成,一个是agent,一个是flow_server, agent负责将数据实时传输给flow_server,flow_server负责对实时流的处理

## 特点
* linux only
* 继承simple系列的传统,简洁实用
* 安全,可靠 -- 采用拉模式,对数据源系统依赖,影响小

## 依赖
 * [simple_log](https://github.com/hongliuliao/simple_log) 日志组件

## 构建 && 测试
```
  make && make test # 此步骤会生成一个agent和一个测试用的flow_server
  ./bin/flow_server_test # start flow_server to receive flow
  ./bin/fileagent localhost 3491 /tmp/test.log # usage: ./bin/fileagent flow_server_ip flow_server_port file_path
```
