##UE5 编辑器综合工具插件

一个展示 Unreal Engine 5 高级插件开发能力的综合性编辑器工具，集成了自定义 Slate UI、后处理效果系统、网络通信（TCP/UDP）、实时性能监控、资产管理与批量处理等多种模块。该项目作为引擎开发岗位的技术作品集。

---

## 🎯 项目简介

MyFirstPlugin 是一个 UE5.6 编辑器插件，通过 **Window → My First Tool** 子菜单提供多个独立的功能窗口。项目旨在深入理解引擎的 Slate UI 框架、渲染管道扩展、网络通信、性能数据采集以及资产系统，并将这些技术整合为可复用的架构。

---

## 📦 功能模块一览

### 1. 自定义 Slate 控件与交互

- **SColorWheel**：继承 `SLeafWidget`，自绘制色相条，支持鼠标点击/拖拽选色，通过委托实时通知外部，演示了 Slate 输入事件、几何空间转换与自绘技术。
- **SFpsChart**：继承 `SLeafWidget`，使用环形缓冲区存储最近 200 帧的帧时间，在 `OnPaint` 中绘制深灰背景、30/60FPS 参考虚线、帧时间折线和当前帧率文本。
- **SPerformancePanel**：继承 `SCompoundWidget`，组合显示实时 FPS、物理内存、Draw Call 数量以及帧率趋势图。通过 `FTSTicker` 每帧更新，并根据阈值动态改变文本颜色（绿/黄/红）。

### 2. 后处理效果管理系统

- **UMPostProcessManager**：运行时加载后处理材质并创建动态材质实例（`UMaterialInstanceDynamic`），向关卡中的无限范围 `PostProcessVolume` 注入 Blendable，实现多种滤镜叠加。
- **SPostProcessPanel**：提供 Slate UI 面板，包含灰度化（强度/对比度/亮度）、颜色反转（权重/阈值）、老电影（棕褐/噪点/暗角）三组滑块，所有参数实时调节并立即反映在场景画面中。

### 3. 网络通信（TCP & UDP）

- **FTcpEchoServer / FTcpEchoClient**：基于 `FRunnable` 的多线程 TCP 服务器/客户端，实现回显服务。演示了 Socket 创建、Bind/Listen/Accept 流程。
- **FUdpChatRoom**：基于 `FRunnable` 的 UDP 聊天室，使用 `FUdpSocketBuilder` 创建 Socket，支持发送消息和异步接收，通过线程安全队列将数据传递到 UI。

### 4. 资产管理与批量处理

- **FAssetInfoCollector**：封装 `IAssetRegistry` 查询，按类型统计项目中的纹理、材质、蓝图、静态网格数量。
- **SAssetPieChart**：自定义 Slate 控件，用柱状图 + 图例可视化展示资产类型分布，支持动态更新数据。
- **FMyAssetActions**：继承 `FAssetTypeActions_Base`，为纹理和材质添加右键菜单（复制到指定目录、转换为材质实例占位）。支持多选资产批量操作。
- **SBatchAssetTool**：集成六种核心操作模式，是插件的核心生产力工具：
  - **批量复制**：将源目录的资产复制到目标目录，自动添加后缀，检测同名避免覆盖。
  - **批量重命名**：支持添加前缀、添加后缀、按序号重命名、**自动修正（Auto Fix）** 四种策略。
    - **Auto Fix**：根据资产类型自动推荐前缀（`T_`、`M_`、`SM_`、`BP_`），根据纹理压缩设置推断推荐后缀（`_D`、`_N`、`_M`），一键规范化命名。
  - **纹理压缩**：批量修改纹理的压缩格式、Mip 生成设置和 sRGB 开关。
    - **Auto Detect**：根据纹理名称后缀自动分析并推荐最佳压缩设置（法线→BC5，颜色→BC7，数据贴图→灰度压缩），自动设置 sRGB。
  - **静态网格设置**：批量修改光照贴图UV、自动计算LOD（提示）、Nanite、碰撞复杂度。
  - **预览功能**：执行前展示所有受影响的资产及目标名称/设置。
  - **异步分帧执行**：每帧处理一个资产，编辑器保持响应，进度条实时更新。
  - **撤销支持**：重命名、压缩和静态网格操作可通过 Ctrl+Z 撤销（使用 `FScopedTransaction` + `Modify()`）。
  - **操作日志**：每个资产的处理结果（成功/失败/跳过）记录到 Output Log，完成后打印汇总报告。
  - **操作历史与 CSV 导出**：记录每次批量操作的时间、类型、资产名、路径、状态，支持一键导出到 `Saved/BatchReports/` 目录。

### 5. 编辑器窗口与菜单扩展

- 使用 `FLevelEditorModule` 和 `FExtender` 在 Window 菜单下注册 **My First Tool** 子菜单，每个功能模块拥有独立的 `SWindow`，布局清晰，便于扩展。

---

## 🛠 技术栈

- **Unreal Engine 5.6**
- **C++ (Unreal Build Tool)**
- **Slate UI 框架**：自定义控件、声明式语法、自绘（`OnPaint`）
- **FRunnable**：多线程网络服务器
- **FSocket / FUdpSocketBuilder**：TCP/UDP 通信
- **IAssetRegistry**：资产数据库查询
- **FAssetTools**：资产批量操作（复制/重命名/修改属性）
- **FScopedTransaction**：撤销系统
- **FTSTicker**：异步分帧处理
- **UMaterialInstanceDynamic**：动态材质实例
- **PostProcessVolume**：后处理效果应用
- **FPlatformMemory**：内存统计
- **FSlateDrawElement**：自定义绘制（矩形、线条、文本、图表）
- **FFileHelper**：CSV 文件导出
- **FAssetNamingRules**：命名规范自动修正规则引擎
- **FTextureCompressionAutomator**：纹理压缩智能推荐

---

## 🔧 关键技术亮点

- **自绘 Slate 控件**：手动实现 `FArguments` 以解决插件模块中 `SLATE_BEGIN_ARGS` 宏的兼容问题；重写 `OnPaint`、`ComputeDesiredSize` 以及鼠标事件处理。
- **多线程与网络**：使用 `FRunnable` 创建独立线程处理 TCP/UDP 收发，避免阻塞游戏线程；解决 `FUdpSocketBuilder` 与 `Send/RecvFrom` 的版本适配问题。
- **动态材质与后处理**：通过 `PostProcessVolume` 和 `UMaterialInstanceDynamic` 实现运行时滤镜效果，材质参数通过 Slate UI 实时调整。
- **性能数据可视化**：利用 `FPlatformMemory::GetStats`、`GNumDrawCallsRHI`、`FApp::GetDeltaTime` 等原生 API 采集数据，结合环形缓冲区和 `FTSTicker` 实现趋势图绘制和颜色预警。
- **资产系统深度集成**：`Asset Registry` 查询 + `Asset Tools` 操作 + `Asset Type Actions` 右键菜单，形成完整的资产工作流。
- **智能化规则引擎**：`FAssetNamingRules` 和 `FTextureCompressionAutomator` 实现基于规则的自动化决策，从"手动设置"升级为"一键规范"。
- **异步与撤销**：`FTSTicker` 实现分帧处理，防止编辑器卡顿；`FScopedTransaction` 提供安全回滚。
- **数据持久化与报告**：CSV 格式导出操作历史，生成可追溯的操作报告。
- **模块化架构**：每个功能封装为独立的控件或类，通过插件模块统一调度，易于维护和扩展。

---

## 🚀 如何运行与使用

### 环境要求

- Unreal Engine 5.6（从 Epic Games Launcher 安装）
- Visual Studio 2022（需勾选"使用 C++ 的游戏开发"工作负载）
- 已通过 Git 克隆或直接创建本项目 `Project_1_0513`

### 启动插件

1. 打开项目，编辑器会自动加载插件。
2. 在编辑器菜单栏中点击 `Window` → `My First Tool`，展开子菜单：
   - **Color Picker**：打开颜色选择器窗口。
   - **Post Process Filters**：打开后处理滤镜面板（需先在 Content 中放置对应的后处理材质）。
   - **Network Tools**：打开网络工具窗口（TCP Echo 测试 + UDP 聊天室）。
   - **Performance Monitor**：打开性能监控面板（FPS、内存、Draw Call、趋势图）。
   - **Asset Statistics**：打开资产统计窗口，点击 Refresh 查看项目资产数量和柱状分布图。
   - **Batch Asset Tool**：打开批量资产处理工具，支持复制、重命名、纹理压缩、静态网格设置。

### 使用各个模块

- **颜色选择器**：拖动色条选色，RGB 数值实时显示。
- **后处理滤镜**：在 Post Process Filters 窗口中拖动滑块，场景画面会立即应用灰度、反转或老电影效果（需确保材质路径与 `MPostProcessManager` 中一致）。
- **网络测试**：
  - 点击 "Start Server" 启动 TCP 回显服务，再点击 "Send Echo" 发送消息并查看回显。
  - 点击 "Start Chat Room" 启动 UDP 聊天，输入消息发送，然后点 "Refresh" 查看接收内容。
- **性能监控**：窗口自动显示实时 FPS、内存、Draw Call 数值和帧率折线图。
- **资产统计**：打开后点击 "Refresh Stats" 即可看到各类资产数量和柱状图分布。
- **资产右键菜单**：在内容浏览器中右键纹理或材质 → "My Tools" → 复制到指定目录。
- **批量处理工具**：
  - 选择操作模式（Copy / Rename / Compress / Mesh）。
  - 输入源路径（如 `/Game/Textures`），复制时需输入目标路径。
  - 勾选资产类型，填写重命名参数或纹理压缩/静态网格选项。
  - 点击 **Auto Fix**（重命名模式）自动规范化命名；点击 **Auto Detect**（压缩模式）自动推荐压缩设置。
  - 点击 **Preview** 查看即将影响的资产列表。
  - 点击 **Execute** 异步执行，观察进度条和 Output Log 中的日志。
  - 执行完成后可按 Ctrl+Z 撤销（重命名、压缩和静态网格操作）。
  - 点击 **Export Report** 导出操作历史为 CSV 文件。

---

## 📁 项目结构（核心源码目录）
 Plugins/MyFirstPlugin/Source/MyFirstPlugin/
├── MyFirstPlugin.Build.cs
├── Public/
│ ├── MyFirstPlugin.h # 插件入口、菜单、窗口声明
│ └── FAssetInfoCollector.h # 资产统计收集器
├── Private/ 
│ ├── MyFirstPlugin.cpp # 插件入口、菜单扩展、独立窗口实现
│ ├── SColorWheel.h/cpp # 颜色选择器控件
│ ├── SPostProcessPanel.h/cpp # 后处理参数面板
│ ├── MPostProcessManager.h/cpp # 后处理管理器
│ ├── SMyProgressBar.h/cpp # 练习SlateUI控件
│ ├── SFpsChart/
│ │ ├── SFpsChart.h/cpp # 帧率趋势图控件
│ │ └── SPerformancePanel.h/cpp # 综合性能监控面板
│ ├── Network/
│ │ ├── FTcpEchoServer.h/cpp # TCP Echo 服务器
│ │ ├── FTcpEchoClient.h/cpp # TCP Echo 客户端
│ │ └── FUdpChatRoom.h/cpp # UDP 聊天室
│ ├── AssetTools/
│ │	├── FAssetInfoCollector.h/cpp # 资产统计实现
│ │	├── FMyAssetActions.h/cpp # 资产右键菜单扩展
│ │   ├── SAssetPieChart.h/cpp # 资产分布柱状图控件
│ │	├── SBatchAssetTool.h/cpp # 批量资产处理工具（复制/重命名/压缩/静态网格）
│ │	├── FAssetNamingRules.h/cpp # 命名规范自动修正规则引擎
│ │	└── FTextureCompressionAutomator.h/cpp # 纹理压缩智能推荐
│ └BatchOperationHistory/
│ 	└──FBatchOperationHistory.h/cpp # 操作历史与CSV导出
│
└── Content/ # 可放置后处理材质资源（M_PP_*）

---

## 📝 许可证与贡献

本项目仅用于学习和作品集展示，未经严格测试，请勿直接用于商业生产环境。欢迎提出改进建议或提交 Pull Request。
