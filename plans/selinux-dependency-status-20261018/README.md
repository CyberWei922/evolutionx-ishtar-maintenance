# 公共库依赖与 Mtd 精确状态

基线为此前已安装1792257806。本轮五路径增量已合入、局部编译和独立权限核验通过；累计完整候选1792261801已完成构建、实际34分区和静态升级检查，并按机主新授权保留数据刷入、正常启动，39文件/21分区范围读回通过，机主本版功能验收待答。当前手机仍全局Permissive。机主此前表示相机当前又无明显问题，先按现状使用，流畅度后续优化；没有更改相机参数或性能逻辑，也不宣称间歇变暗永久修好。

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

secure XDSP、通用vendor文件/属性及默认未知MiSys服务仍禁止；可选PSI/私有属性、HAL枚举、Mtd DAC/消费者、原9宽松域和KSU例外分开处理。原neverallow不变。DSP注册不等于已证明实际HAL回退。构建阶段未操作手机，后续按新授权刷入和首次正常重启，进行了有限读回与安全日志快照；未切Enforcing。构建只限制本任务资源，不调整Ubuntu其他任务；本轮刷入/首次重启授权已使用，额外重启及短时Enforcing仍需具体新授权。

## 原有宽松域的下一阶段入口

original-permissive-origins.json按实际生成conf定位九个不同源文件，未复制其完整厂商文本。6个声明位于userdebug_or_eng中，su/backuptool/vendor_hal_debugutils_default语句位于宏外，不能假定改user构建就全部消失。实际产品纳入、组件用途和KSU动态例外仍待审查；未更改构建变体或移除声明。

## 完整候选结果

完整构建28:01/exit0，audit-worker和audit-status均complete；34实际payload分区、59源码、新库实际inode标签/字节及与组件一致的策略通过，原相机/亮度/Recovery/开机显示同步/内核/KSU/AVB基线保持。版本1792261801，4,545,440,201 bytes，SHA2bff039e93f5bec2ddc1e6c255d3300e4c7f65afa48354769cfb3456748b5ca0。正式ROM仅在Ubuntu同名delivery，Mac为41项薄交接，Git不含ROM。

证书字节、SDK/补丁/fstab保持、时间向前、无userdata/metadata和wipe/downgrade标记的静态检查通过；完整OTA签名未独立密码学核验。以下运行结果覆盖构建阶段未安装快照，不重放旧runner。

## 刷入与运行结果

机主回复“直接开刷吧”，授权此候选保留数据刷入和核验成功后首次正常重启，不含Enforcing。新鲜40固件范围全部匹配、A槽/FBE/空模块/快照none、数据基线通过；28镜像、7动态回解、59分块重组通过。新runner8004c866…和9项身份负向检查通过。反回滚anti1沿用固定历史Bootloader回执及固件连续性，非本轮新查询。

新flash01完成74动作/73完整下载，wire SHA5078adfb27156dc1d0300693f4973b9f8f1e283f1376d49048c01fc369ed396b，未写固件或userdata/metadata。03:25:30正常重启，postboot01成功，39文件/21分区范围全匹配；新库实际标签及5项Mtd属性标签正确，六类默认相机解析、DSP进程/HIDL注册和数据基线通过。112次命令尝试、0重试。加载策略6b156e09…/2,151,534 bytes，原9静态宽松域+ksu保持，仍全局Permissive。

初始进程快照未见MIUI相机运行，不能称本轮实际MIUI进程域或算法回退已验证；机主新拍照/有声录像/指纹和数据验收待答。logcat317条AVC/18域、audit_lost最大1，包含HAL属性枚举和Mtd DAC/新增persist.vendor.sys.attestation.status写拒绝；不是全量或完成百分比，不能与前版不同测试窗口直接比较。新RKP类型本窗口未见拒绝，不等于所有消费者完成验收。具体摘要见runtime-summary.json，原始日志/加载策略留在本地。

有限构建、刷写和读回worker全部结束，未创建手机常驻采集/自启/模块，也未再次重启或切Enforcing。Mac/Ubuntu runtime22项及697,254 bytes归档匹配，SHAd9a35dc58c4b857d02372133e28679c1932e86095a186067aec9e0b2e9b7870a。新包及原回退包暂保留，等待机主功能验收后按既有双版本规则处理。

## 1792261801机主验收与清理

机主已确认此前所问原应用/照片、MIUI拍照/新录像收音、指纹无问题；原初始快照MIUI进程未出现的记录不改。owner-acceptance.json保存范围，仍Permissive，Enforcing未试。按双版本要求保留1792261801/1792257806完整ROM；老两ZIP和已完成暂存已删除，旧runner不可重放。下一版62项源入口为control/ishtar-selinux-device-io-20261018-01/expected-source-files.json，本目录59清单是已安装基线。
