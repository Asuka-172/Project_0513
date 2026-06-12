MyFirstPlugin — UE5 编辑器综合工具插件
一个展示 Unreal Engine 5 高级插件开发能力的综合性编辑器工具，集成了自定义 Slate UI、后处理效果系统、网络通信（TCP/UDP）、实时性能监控等多种模块。
🎯 项目简介
MyFirstPlugin 是一个 UE5.6 编辑器插件，通过一个统一的工具窗口（Window → My First Tool）提供多种可视化与功能模块。项目旨在深入理解引擎的 Slate UI 框架、渲染管道扩展、网络通信及性能数据采集，并将这些技术整合为可复用的架构。
📦 功能模块一览
1. 自定义 Slate 控件与交互
SColorWheel：继承SLeafWidget，自绘制色相条，支持鼠标点击/拖拽选色，通过委托FOnColorChanged实时通知外部，演示了 Slate 输入事件、几何空间转换与自绘技术。
SFpsChart：继承SLeafWidget，使用环形缓冲区存储最近 200 帧的帧时间，在OnPaint中绘制深灰背景、30/60FPS 参考虚线、帧时间折线和当前帧率文本。演示了自定义控件绘制、数据缓冲与定时更新。
SPerformancePanel：继承SCompoundWidget，组合显示实时 FPS、物理内存、Draw Call 数量以及上述帧率趋势图。通过FTSTicker每帧更新，并根据阈值动态改变文本颜色（绿/黄/红）。演示了复合控件、性能数据采集与 Slate 绑定。
2. 后处理效果管理系统
UMPostProcessManager：运行时加载后处理材质并创建动态材质实例（UMaterialInstanceDynamic），向关卡中的无限范围PostProcessVolume注入 Blendable，实现多种滤镜叠加。
SPostProcessPanel：提供 Slate UI 面板，包含灰度化（强度/对比度/亮度）、颜色反转（权重/阈值）、老电影（棕褐/噪点/暗角）三组滑块，所有参数实时调节并立即反映在场景画面中。演示了材质参数集、编辑器 UI 与渲染管道结合。
3. 网络通信（TCP & UDP）
FTcpEchoServer/FTcpEchoClient：基于FRunnable的多线程 TCP 服务器/客户端，实现回显服务。演示了FSocket创建、Bind/Listen/Accept流程，以及 TCP 的面向连接通信。
FUdpChatRoom：基于FRunnable的 UDP 聊天室，使用FUdpSocketBuilder创建 Socket，支持发送消息和异步接收，通过线程安全队列 (TQueue) 将数据传递到 UI。演示了无连接 UDP 通信、多线程协调与 Slate 集成。
4. 编辑器窗口与菜单扩展
使用FLevelEditorModule和FExtender在 Window 菜单下注册“My First Tool”按钮，点击后弹出包含所有功能面板的独立SWindow，使用SScrollBox和SVerticalBox进行布局。演示了编辑器菜单扩展与 Slate 窗口管理。
🔧 关键技术亮点
自绘 Slate 控件：手动实现FArguments以解决插件模块中SLATE_BEGIN_ARGS宏的兼容问题；重写OnPaint、ComputeDesiredSize以及鼠标事件处理，展示完整的自定义控件开发能力。
多线程与网络：使用FRunnable创建独立线程处理 TCP/UDP 收发，避免阻塞游戏线程；正确处理线程安全队列和 Socket 生命周期；解决FUdpSocketBuilder与Send/RecvFrom的版本适配问题。
动态材质与后处理：通过PostProcessVolume和UMaterialInstanceDynamic实现运行时滤镜效果，材质参数可通过 Slate UI 实时调整，展示了材质系统、渲染管道与编辑器 UI 的深度结合。
性能数据可视化：利用FPlatformMemory::GetStats、GNumDrawCallsRHI、FApp::GetDeltaTime等原生 API 采集数据，结合环形缓冲区和FTSTicker实现趋势图绘制和颜色预警，体现性能监控工具的设计思想。
模块化架构：每个功能（颜色选择、后处理、网络、性能）都封装为独立的控件或类，通过插件模块统一调度，利于维护和扩展。
🚀 如何运行与使用
1.环境要求
oUnreal Engine 5.6（从 Epic Games Launcher 安装）
oVisual Studio 2022（需勾选“使用 C++ 的游戏开发”工作负载）
o已通过 Git 克隆或直接创建本项目Project_1_0513
2.启动插件
o打开项目，编辑器会自动加载插件。
o在编辑器菜单栏中点击Window→My First Tool打开主工具窗口。
3.使用各个模块
o颜色选择器：拖动色条选色，RGB 数值实时显示。
o后处理滤镜：在 Post Process Filters 分组中拖动滑块，场景画面会立即应用灰度、反转或老电影效果（需先在 Content 中放置对应的后处理材质，路径与MPostProcessManager中一致）。
o网络测试：点击 “Start Server” 启动 TCP 回显服务，再点击 “Send Echo” 发送消息并查看回显。点击 “Start Chat Room” 启动 UDP 聊天，在输入框输入消息发送，然后点 “Refresh” 查看接收内容。
o性能监控：Performance Monitor 区域会自动显示 FPS、内存、Draw Call 数值和帧率折线图，无需手动操作。
4.开发与调试
o所有插件源码位于Plugins/MyFirstPlugin/Source/MyFirstPlugin/。
o修改代码后需关闭编辑器，右键.uproject执行 “Generate Visual Studio project files”，然后编译。
o可在Output Log中查看插件日志（关键字MyFirstPlugin、FTcpEchoServer等）。
📁 项目结构（核心源码目录）
Plugins/MyFirstPlugin/Source/MyFirstPlugin/
├── MyFirstPlugin.Build.cs
├── Public/MyFirstPlugin.h
├── Private/
│   ├── MyFirstPlugin.cpp              	         # 插件入口、菜单、窗口创建
│   ├── SColorWheel.h/cpp             			# 颜色选择器控件
│   ├── SPostProcessPanel.h/cpp       			    # 后处理参数面板
│   ├── MPostProcessManager.h/cpp                # 后处理管理器
│   ├── SFpsChart/
│   │   ├── SFpsChart.h/cpp             			# 帧率趋势图控件
│   │   └── SPerformancePanel.h/cpp    			 # 综合性能监控面板
│   └── Network/
│       ├── FTcpEchoServer.h/cpp       			 # TCP Echo 服务器
│       ├── FTcpEchoClient.h/cpp        			# TCP Echo 客户端
│       └── FUdpChatRoom.h/cpp         			 # UDP 聊天室
└── Content/                              	# 可放置后处理材质资源（M_PP_*）