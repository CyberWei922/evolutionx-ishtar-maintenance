# 下一批：MIUI 默认相机与 SELinux

2026-10-17。基线是已安装的1792246397；本目录是下一版源码，尚未刷入。Aperture继续保留备用，MIUI camera03的APK、签名、HAL和配套载荷不改。

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

当前工作源码入口为Ubuntu新control目录`ishtar-selinux-camera-next-20261017-01/expected-source-files.json`，59项；本仓库原accepted清单仍是1792226532，前一候选清单及累计补丁描述1792246397，三者不混用。

## 后续验证

仍保持全局Permissive。Mtd persist身份/DAC、属性消费者、MIUI ODM库边界/MiSys缺失服务、camera HAL属性枚举及原9宽松域继续待修。此批没有修改persist内容/权限或给出安全Enforcing结论。

打包后核对辅助APK签名/权限、出厂XML、seapp映射和全部保护载荷；获新刷入/重启授权后，再验证旧偏好迁移、普通/锁屏/双击电源、外部拍照及录像结果回传。现有桌面显式Aperture图标仍打开Aperture；直接使用Camera2的应用保留自身拍摄界面。

Ubuntu并行运行其他任务，本轮仅降低自己的构建优先级并限定两个CPU，不停止或调整其他进程。
