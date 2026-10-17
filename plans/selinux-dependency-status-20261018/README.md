# 公共库依赖与 Mtd 精确状态

基线为已安装1792257806。本轮五路径增量已合入、局部编译和独立权限核验通过；累计完整候选正在构建，未安装。当前手机仍全局Permissive。机主表示相机当前又无明显问题，先按现状使用，流畅度后续优化；没有更改相机参数或性能逻辑，也不宣称间歇变暗永久修好。

## 修改与来源

- 精确标记现有64位libc++_shared.so为vendor_public_lib_file。当前mialgo/SNPE动态表和运行AVC表明这是现有公共库的传递依赖；不改公共库列表、namespace、ELF或APK。此类型有标准应用共享加载语义，不能称MIUI专享。
- MIUI专用域只读common已声明为vendor_public的sensor属性；不增加set或通用属性读取。
- Mtd实际写入persist.vendor.sys.rkp.status，实际libmt@1.3.so含精确key和rkp_get_status符号。新增vendor_restricted专用类型与精确string映射，只增加Mtd标准set/get；实际消费者仍需运行审计。
- Mtd按实际restorecon/读取AVC增加file_contexts_file三项只读，不增加relabel、持久目录访问或Unix DAC绕过。原DAC身份问题仍未解决。

candidate.patch相对1792257806源码；source-baselines.json保存五路径前后摘要，working-source-files.json为新的59项活动输入。四个完整原创维护策略源保存在source，旧file_contexts完整文本仅在本地交付留存；此处只上传新增片段及最小补丁。既有作者/许可仍适用。没有厂商实现、编译策略或原始手机日志。

## 组件结果

dependency-status-policy-build-01已02:24/exit0。verified01首轮通过，策略544dd102b0cfb1dc51db49d0091dfc380dfc303eaf60e656ac21846ea3699302，1,604,243 bytes。22路径、59源码、合并属性数据库与精确属性契约通过；已有类型仅2项只读allow契约新增、0项删除，xperm和原9静态宽松域保持。新RKP契约与原Mtd feature_state类型一致。

Mac/Ubuntu组件delivery/ishtar-selinux-dependency-status-components-20261018-01共116项通过；归档1,210,884 bytes，SHA973d234be9f17927b4b0b52717f9c665b8870c6d43418bd7ab56eec8b31ffc87。它不是单独刷写包。

## 验证边界

secure XDSP、通用vendor文件/属性及默认未知MiSys服务仍禁止；可选PSI/私有属性、HAL枚举、Mtd DAC/消费者、原9宽松域和KSU例外分开处理。原neverallow不变。DSP注册不等于已证明实际HAL回退。此轮没有手机日志采集、设置、重启、刷写或Enforcing。构建只限制本任务资源，不调整Ubuntu其他任务；新候选刷入/重启和短时Enforcing均需各自具体新授权。

## 原有宽松域的下一阶段入口

original-permissive-origins.json按实际生成conf定位九个不同源文件，未复制其完整厂商文本。6个声明位于userdebug_or_eng中，su/backuptool/vendor_hal_debugutils_default语句位于宏外，不能假定改user构建就全部消失。实际产品纳入、组件用途和KSU动态例外仍待审查；未更改构建变体或移除声明。
