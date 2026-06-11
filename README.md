Project_1_0513 - UE5 Plugin Development Learning Project
基于 Unreal Engine 5.6.1 的编辑器插件学习项目，涵盖了 Slate UI、后处理、网络通信和性能监控等多个模块。
环境要求
Unreal Engine：5.6.1
IDE：Visual Studio 2022
平台：Windows 10/11
插件功能概览
插件名称：MyFirstPlugin
功能模块	说明
编辑器菜单扩展	在 Window 菜单中添加 "My First Tool" 按钮，打开自定义工具窗口
颜色选择器	基于 Slate 的自定义色相条控件（SColorWheel），支持鼠标拖拽和实时 RGB 显示
后处理滤镜系统	灰度化、颜色反转、老电影滤镜，支持实时参数调整，通过 PostProcessVolume 应用
TCP Echo 服务	基于 FRunnable 的多线程 TCP 服务器/客户端，演示面向连接的通信
UDP 聊天室	基于 FRunnable 的 UDP 消息收发，演示无连接通信
FPS 趋势图	自定义 Slate 图表控件（SFpsChart），绘制帧率折线图和参考线
性能监控面板	实时显示 FPS、内存使用、Draw Call 数量，支持颜色警告（绿/黄/红）
项目结构
Project_1_0513/
├── Plugins/
│ └── MyFirstPlugin/
│ ├── MyFirstPlugin.uplugin
│ ├── Resources/
│ └── Source/
│ └── MyFirstPlugin/
│ ├── MyFirstPlugin.Build.cs
│ ├── Public/
│ │ └── MyFirstPlugin.h
│ └── Private/
│ ├── MyFirstPlugin.cpp # 插件入口、菜单、窗口
│ ├── MPostProcessManager.h/cpp # 后处理管理器
│ ├── SColorWheel.h/cpp # 颜色选择器控件
│ ├── SPostProcessPanel.h/cpp # 后处理 UI 面板
│ ├── SFpsChart/
│ │ ├── SFpsChart.h/cpp # 帧率图表控件
│ │ ├── SPerformancePanel.h/cpp # 性能监控面板
│ └── Network/
│ ├── FTcpEchoServer.h/cpp # TCP 服务器
│ ├── FTcpEchoClient.h/cpp # TCP 客户端
│ └── FUdpChatRoom.h/cpp # UDP 聊天室

构建与运行
1.克隆项目到本地:git clone <repository-url>
2.右键.uproject文件，选择Generate Visual Studio project files。
3.打开生成的.sln文件，使用Development Editor配置编译。
4.启动编辑器后，在顶部菜单栏Window → My First Tool打开插件窗口。
关键技术点
Slate 声明式 UI
自定义控件继承SLeafWidget或SCompoundWidget
使用手动编写的FArguments结构体替代SLATE_BEGIN_ARGS宏（解决插件模块中宏兼容性问题）
通过FSlateDrawElement::MakeBox/MakeLines/MakeText进行自定义绘制
后处理系统
使用PostProcessVolume的WeightedBlendables数组动态管理后处理材质
通过UMaterialInstanceDynamic实现运行时参数修改
材质域设置为Post Process，输出连接到Emissive Color
网络通信
TCP：CreateSocket(NAMEStream)+Bind+Listen+Accept+Send/Recv
UDP：FUdpSocketBuilder创建非阻塞 Socket +SendTo+RecvFrom
使用FRunnable在独立线程运行网络逻辑，通过线程安全队列与 UI 通信
性能监控
使用FTSTicker注册每帧更新
数据来源：FApp::GetDeltaTime()（帧时间）、FPlatformMemory::GetStats()（内存）、GNumDrawCallsRHI[0]（Draw Call）
环形缓冲区存储历史帧数据，用于绘制趋势图
踩坑记录
问题	原因	解决
SLATE_ARGUMENT编译错误	宏在插件模块中展开失败	改用手写FArguments结构体
Send参数数量不匹配	UE 的FSocket::Send需要 3 个参数	添加int32& BytesSent输出参数
GNumDrawCallsRHI链接错误	缺少 RHI 模块依赖	在Build.cs中添加"RHI"
GNumDrawCallsRHI类型错误	UE5.6 中为int32[8]数组	使用GNumDrawCallsRHI[0]
UDPRecvFrom错误码 26	CreateSocket创建的 Socket 无效	改用FUdpSocketBuilder
SVerticalBox.h找不到	UE5.6 中头文件路径变化	使用Widgets/Slate.h或Widgets/SBoxPanel.h
控件创建顺序导致 Lambda 捕获空指针	SAssignNew赋值晚于SNew的 Lambda 捕获	先创建控件，再创建依赖它的控件
许可证
This project is for learning purposes only.
