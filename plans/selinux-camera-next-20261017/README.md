# 下一批：MIUI 默认相机与 SELinux

2026-10-17形成，2026-10-18刷入验收。基线为1792246397；本目录14路径增量现已安装为1792252694，后续未安装的基础API增量见相邻selinux-framework-api-20261018计划。Aperture继续保留备用，MIUI camera03的APK、签名、HAL和配套载荷不改。

## 实现范围

- Mtd另外两个状态输出改为独立vendor restricted类型：`persist.vendor.sys.provision.status`、`vendor.sys.feature_state`；仅Mtd精确set/get，不给通用vendor属性写权限。
- 用system_ext public/private策略声明`ishtar_miui_camera_app`，seapp仅匹配platform签名、priv-app、`com.android.camera`。保持原UID、`app_data_file`与`levelFrom=user`，不需要迁移相机数据。
- 将Xiaomi后处理IBGService的客户端限定为MIUI专用域，服务端为既有camera provider；补充P3和音频属性只读权限。没有给全部platform_app放行，也没有将新域设为permissive。
- 预装普通preferred-activities XML供首次配置；原生平台签名的`IshtarCameraDefaults`短暂接收系统启动/包更新事件，迁移机主User 0已有偏好。它没有桌面Activity或常驻Service，无KSU模块依赖。

| 标准动作 | MIUI组件 |
| --- | --- |
| STILL_IMAGE_CAMERA | AssistantCamera |
| STILL_IMAGE_CAMERA_SECURE | Camera |
| IMAGE_CAPTURE | OneShotImageCapture |
| IMAGE_CAPTURE_SECURE | OneShotCamera |
| VIDEO_CAMERA | VideoCamera |
| VIDEO_CAPTURE | OneShotVideoCapture |

### 为什么需要迁移

基线实机查询六类动作均返回选择器；不能把“预装Aperture”说成“已固定默认Aperture”。MIUI安全相机还匹配Camera/AssistantCamera两个组件，仅设置包名也可能留下选择器。当前SystemUI的cameraGesturePackage为空，普通与安全相机都使用标准全局Intent，因此逐动作的明确首选组件可供其解析。

当前PackageManager源码只在首次启动/特定旧版升级时读出厂preferred-apps XML，普通保留数据OTA不会重设。因此增加一次性迁移。通过系统签名的SET_PREFERRED_APPLICATIONS调用真实PM接口；每项解析确认后才持久化完成状态。Aperture/选择器/MIUI旧别名可迁移，明确第三方选择保留；完成后尊重用户后续修改。辅助程序仅处理User 0，不修改工作资料或次用户的旧偏好。

## 验证与状态

主机纯Java行为测试通过：11项迁移行为、3项路由契约和6类唯一动作；XML与Java组件映射一致。14路径增量补丁在独立临时目录实际应用后逐字节匹配。组件编译和生成策略检查状态见`build-status.json`，不能把源码测试当作实机默认迁移已验收。

本轮安装基线源码入口为Ubuntu`ishtar-selinux-camera-next-20261017-01/expected-source-files.json`，59项；目前活动树已继续变化，应使用下一基础API计划的新59清单；本仓库原accepted清单仍是1792226532，前一候选清单及累计补丁描述1792246397，三者不混用。

## 后续验证

仍保持全局Permissive。Mtd persist身份/DAC、属性消费者、MIUI ODM库边界/MiSys缺失服务、camera HAL属性枚举及原9宽松域继续待修。此批没有修改persist内容/权限或给出安全Enforcing结论。

打包后核对辅助APK签名/权限、出厂XML、seapp映射和全部保护载荷；获新刷入/重启授权后，再验证旧偏好迁移、普通/锁屏/双击电源、外部拍照及录像结果回传。现有桌面显式Aperture图标仍打开Aperture；直接使用Camera2的应用保留自身拍摄界面。

Ubuntu并行运行其他任务，本轮仅降低自己的构建优先级并限定两个CPU，不停止或调整其他进程。

## 2026-10-18整包完成

1792252694完整ROM已编译22:05/exit0，实际34分区、59源码摘要、新默认辅助APK/XML、Aperture和新专用策略通过；静态不清数据升级检查通过。ROM为4,545,399,102 bytes，SHA999dd46495f1d2a99b748ee757f1a9609892e53d2d25598688b385984d82075b。详见full-rom-status.json；Git只存源码及摘要，完整ROM留Ubuntu。已按机主另行授权保留数据刷入并正常重启，32文件/21范围和六默认/新域核对通过；机主确认所问数据、默认入口、拍摄收音、指纹正常。仍Permissive，未试切Enforcing。授权已经完成，后续新刷入/重启或Enforcing均需具体确认。

自己的构建初始限CPU6/7、nice10/I/O7/Ninja2；打包后段确认CPU4/5空闲后仅放宽本任务及子进程至CPU4–7，未停止/调整其他任务。原两个完整ROM未提前删除。
