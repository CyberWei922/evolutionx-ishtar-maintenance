## 1792311835已刷入正常启动，自动读回通过（最新）

新flash01完成74动作/73完整下载，wire SHA `e79febfde6d2774750cf48022faf28f03166cfe642d62c09547e4d0d1d63ba14` 通过；核验后首次正常重启。无userdata/metadata/底层固件写入，未锁BL。刷入与首次正常重启授权已使用，不复用作额外重启或Enforcing。

postboot01及worker exit0：50文件、21分区范围SHA全匹配，数据目录/属主权限/第三方应用数10基线保持；142次命令尝试、0重试。新19组策略契约与实际编译策略一致；105项包含新类型结构继承，不是105个bug。两诊断目录实际标签和原属主模式、两精确属性标签、三条system参数及三维护者APK摘要/三个运行资源值通过，维护者名CyberWei。实机卡片/图片显示和主动保存仍待机主回复；未自动打开支付链接或做扫码/到账验收。

磁盘策略38cde3d5…/1,604,906 bytes；加载策略 `a603e2d5734cfb2c04a1632fe81ed5d85e1dc09293b1f4fdcb4292b199652b80` /2,152,197 bytes。原9静态宽松域+ksu保持，全局Permissive/enforce0，未做Enforcing。六MIUI默认入口保持；初始MIUI进程观察为True，不能据此扩写所有相机模式或算法回退通过。

运行归档24项、1,520,153 bytes/SHA `68499f399fc68dc1eea9eac7e02db4074e489ee3831660c33779762f35733676` 两主机匹配；有限worker结束，无手机自启/模块或新增常驻采集。AVC仅一次启动快照，不代表全量/完成比例。模块启动、原静态宽松域/KSU及短时Enforcing/冷启动仍单独验证；下一里程碑为先启动独立30秒回退guard的受控运行试验，需要具体新授权。

活动82源/20路径保持，正式ROM仍在Ubuntu冻结delivery，Mac68项薄交接无ROM。保留新包、三个旧完整ROM和当前暂存，待正常使用反馈后按双版本规则处理；本轮未清理或调整Ubuntu其他任务。完整OTA签名未独立密码学核验。

# SELinux边界修复与本地维护者页面

基线已安装1792300213/Permissive。当前82项输入清单、20路径累计改动；组件12:10/exit0及verified03通过，完整候选1792311835已通过实际34分区及升级静态检查，尚未安装或进行Enforcing。实际状态以build-status.json和后续ROM摘要为准。

修复保留600/true两条legacy配置并移至system初始化；两条相机/radio状态属性复用已有精确类型；RIL诊断目录复用radio标签，diag-router目录使用专用类型及8项明确目录/FIFO权限；重标记紧随原mkdir。原类型权限增删为0，新增类型105项有效权限包含结构属性继承，不是105个bug修复。

本地普通APP与system_ext RRO在设置卡片展示CyberWei，提供GitHub、QQ复制、TG和赞助入口；两张二维码原图本地展示和用户主动保存，PayPal为外链。APP不请求权限，非官方卡片不显示官方徽标。三个APK签名、原图字节与idmap映射检查通过；未有实机UI/相册保存/支付应用扫码验收。

上传16份Apache-2.0源码和最小补丁/元数据；完整file_contexts和vendor.prop沿用既有授权边界，两个机主图片不上传。外部配置SHA及复现器见external-inputs.json；图像必须单独提供。不要把本仓库许可证套用到图片、厂商依赖或ROM。

原9静态宽松域、KSU和全局Permissive保持。传感器可选probe有默认继续分支，不开放default_prop；Mtd可选重载/DAC、必要功能的受控Enforcing及冷启动仍待验证。新刷入/正常重启与Enforcing需要具体新授权，不重复麦克风/指纹全套测试。Ubuntu其他任务未调整。

## 维护者实际入口

设置 → 关于本机 → 型号 → CyberWei。此入口来自本机HardwareInfoFragment，手机实际中文标签和运行资源已核对。机主先在Evolution X → 关于ROM页查找未找到；已明确正确路径，机主随后确认“我看到这个页面了”；仅记页面已找到，图片显示/保存尚未反馈。

## Official发布边界

本机卡片原由ROM提供，本轮新增本地资源和独立页面并修改Settings控制器。当前Unofficial可保留；未来Official不能据此宣称自定义UI已获批准。官方CoC Build Act要求ROM源码改动获准，建议申请时切回官方OTA维护者字段入口，或先提交团队审核；微信/支付宝二维码页面没有据此获得批准。此UI改动与SELinux策略独立，调整UI不需重做策略修复。已只读核对[官方CoC](https://github.com/Evolution-X/wiki/blob/main/content/code-of-conduct.mdx)及[OTA字段](https://github.com/Evolution-X/OTA/blob/cnb/README.md)，2026-10-18；未改当前ROM或联系团队。
