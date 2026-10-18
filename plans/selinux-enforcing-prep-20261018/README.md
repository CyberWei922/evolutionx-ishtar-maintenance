# SELinux 集中修复与 Enforcing 准备

## 1792300213已保留数据刷入并启动，自动读回通过（最新）

本次新flash01完成74动作/73完整下载，wire SHA `51111b2b3512c3673e64fb04e5df8545323067a1b4f7d0f462d408eb69342250` 核验通过；2026-10-18 13:50:13正常重启。没有userdata/metadata/底层固件写入，未锁BL。授权只涵盖此次刷入及首次正常重启，现已使用，不复用作额外重启或Enforcing。

postboot01 success/worker exit0：47文件、21分区范围SHA全匹配，数据目录及属主权限/第三方应用数10的本次新鲜基线保持；127次命令尝试、0重试。新增16组加载策略契约覆盖32项权限，与实际编译策略一致。蓝牙自动连接值true；热控启动属性为vendor_thermal_normal_prop、值1，热控进程存在。六类默认入口仍为MIUI；初始快照未见MIUI进程，不称本版相机运行域/算法回退已验收。没有读取Google账号或凭据内容。

磁盘策略 `1eaa4c3edd8b9125a436029a52c8f7d673135ec55d7b4a956cf9134181f944fe` /1,604,627 bytes；加载策略 `52cf610059c121ae3acd1023ccf01382ee673eba89c199822e1d151f76f5adec` /2,151,918 bytes。原9静态宽松域+ksu保持，仍全局Permissive/enforce0；未进行Enforcing试验，也未声称本版获得新的机主功能验收。本版按实际风险不重复要求麦克风/指纹全套测试。

24项运行证据已在Mac/Ubuntu归档，tar 1,518,646 bytes/SHA `3b27cdee9b78eb445a12491ebf02069ffca302fe910539028c3b067f41bf6f5a`，双方摘要及Mac逐文件匹配。有限构建/审计/刷写/读回worker均结束，无新增常驻采集/模块、自启或额外重启，未调整Ubuntu其他任务。保存的启动logcat有624条AVC/14域、kernel有15条，两份有重叠且audit_lost最大1；不是完整覆盖或修复完成比例。相机HAL属性枚举、Mtd DAC、私有属性及原宽松域等继续按必要功能处理，不能批量放行。

当前活动源码为control/ishtar-selinux-enforcing-prep-20261018-01/expected-source-files.json的68项，旧64/62清单仅描述中间/旧安装基线。正式ROM仍在Ubuntu同名delivery，Mac为54项薄交接，不含ROM；完整OTA签名未独立密码学核验。当前保留新1792300213及原1792266978/1792261801完整ROM和本轮暂存，尚未执行清理，待新版本正常使用反馈再按双版本规则收尾。

维护者设置页展示明确留到下一版，等机主提供显示名及资料后实现；本版没有为此重包，不假定Git用户名，也不将个人维护者展示表述为官方资格。

## 集中候选构建完成时的记录（刷入前历史）

统一enforcing-prep-rom-build-01 exit0，audit-worker-status和audit-status均complete。实际34个payload分区、68输入源码、策略1eaa4c3edd8b9125a436029a52c8f7d673135ec55d7b4a956cf9134181f944fe、默认入口和原保护载荷通过；实际system/vendor以及其余三分区的属性位置检查通过，蓝牙true只在system侧，音频600参数保持。静态保留数据升级检查通过，完整OTA签名未独立密码学核验。

Ubuntu正式目录：`/mnt/android-download/delivery/ishtar-selinux-enforcing-prep-permissive-candidate-20261018-01`，版本1792300213，ZIP 4,545,439,341 bytes/SHA `3b1e518603f2c3d911d7ffbca886df4c97531f419ac733ab201673c166deddec`。Mac同名薄交接54项，不含完整ROM。组件121项和实际有效权限增量32/0检查保持。原9静态宽松域未删除，KSU例外和实际Enforcing尚未验收。

手机仍是机主已测1792266978/Permissive；本轮未刷入、重启、采集手机日志或切Enforcing。当前两版已安装完整回退ROM1792266978/1792261801保持，新候选可并存等待验收；不提前删回退版本。机主已授权本候选在新鲜预检通过后保留数据刷入及首次正常重启，Enforcing仍未授权；使用新runner，不复用旧刷写流程。维护者展示依机主指示留下一版，待其提供名字/资料。验收只按直接风险安排，本轮不默认重复麦克风/指纹。Ubuntu其他任务未调整。


## 源码合入时的状态（历史）

机主要求一次多修、减少反复刷包。本轮不对中间三路径单独打包，累计 **10 个源码路径** 后统一构建。

- 手机仍是机主已确认所问功能正常的 `1792266978`，全局 Permissive。没有本轮手机采集、刷写、重启或 Enforcing 操作。
- 新活动控制目录：`/mnt/android-download/control/ishtar-selinux-enforcing-prep-20261018-01`。
- 活动输入 `expected-source-files.json` **68 项**。旧 camera-qspm 64 项只描述中间组件；device-io 62 项描述已安装基线。
- `enforcing-prep-policy-build-01` 已派发 PID929195，CPU6/7、nice10、I/O7、Ninja2；不调整其他 Ubuntu 任务。

## 本轮合并内容

| 项目 | 修复 | 边界 |
| --- | --- | --- |
| MIUI 导航键配置 | 专用域读 `qemu_hw_prop`，当前对应 `qemu.hw.mainkeys` | 恢复原平台应用已拥有的只读能力；不写属性 |
| MIUI 内存压力检测 | 读 `/proc/pressure/memory` 的 read/open/getattr | 不写 PSI 监视器，不读进程内存，不扩大其他应用 |
| 开机 QSPM | SF 和 BootAnimation 使用标准 QSPM HAL 客户端契约 | 包括既有服务发现、Binder 回调及 fd 传递，不改性能参数 |
| 蓝牙自动重连 | 将 `bluetooth.auto_connect_profiles.enabled=true` 从 common vendor.prop 移至设备 system 属性文件 | 值不变；PhonePolicy 实际 getter 默认 false，修正属性加载者，避免 vendor_init 写系统私有类型 |
| QCC 启动配置 | 允许 vendor_init 写现有 `vendor_qcc_prop` | 实际 init.sm8550.rc 设置 oper.mode=3，复用现有 vendor 类型，不放开默认属性 |
| ROM 版本元数据 | system_server 只读现有 `custom_version_prop` | 仅现有 ro.lineage.* 类型，无写权限 |
| 热控文件初始化 | mi_thermald 增加 self 的 chown/fowner | 不增加文件权限、DAC 绕过或 sysfs 权限；能力无法按路径细分，仍受原文件 MAC 边界约束 |
| 热控启动状态 | 精确 `vendor.sys.thermal.bcl.boot_completed` 复用 `vendor_thermal_normal_prop` | 沿用已有 set/get；不新建类型、不改整个 thermal 前缀，不称改变 BCL 算法 |

10 条路径的累计补丁相对实际安装的1792266978；本次第二阶段新增7路径修改，承接已核验中间组件3路径。源文件前后 SHA 和原文件保存在新控制目录。本轮未修改相机/开机动画/SF 二进制、内核/KSU、Recovery、亮度、刷新率、相机性能设置或音频参数。

## 关键证据

中间 camera-qspm 组件 build exit0、verified02通过：策略82f66ac7…，64源码、完整相关有效权限新增21项/0删除、原9静态宽松域和上下文/genfs/ioctl保持。首轮核验器未正确处理 HAL client attribute 旧集合，失败证据保留；展开全量变化并消除重叠后通过，未为此修改策略。

蓝牙源位置 common/configs/properties/vendor.prop:56；PhonePolicy.java:73、100附近实际读取该键，默认 false。保留 true 并移到系统加载位置，完整ROM阶段还要检查实际 system/vendor build.prop，不能只凭源文件宣布移植成功。

热控固定载荷 `/vendor/bin/mi_thermald` SHA659fe2af…：4个 chown 调用在 main 中，目标为 /data/vendor/thermal 的四个报告/模式文件，uid/gid1000；8个 chmod 涉及这些文件和配置文件。配置路径也可能是 /odm/etc，原 MAC 没有 vendor/odm 文件 setattr，此次不增加。不能把能力描述成仅对四路径生效；须报告其对该域已有文件 MAC 边界的影响。

原 thermal 类型是 vendor_public_prop，已有键 vendor.sys.thermal.data.path，原 mi_thermald 策略已 set_prop。boot_completed 的实际 setter 在 main，写1后进入 pause 循环，返回值被忽略；未发现明确消费者，不把此状态写入当热控算法修复，保留动态构造消费者未排除的限制。

## 已分析而保留的拒绝

- Goodix：真实键 sys.panel.touch_vendor，失败填0xf信息位继续。可能正常初始化执行，非已证实纯工厂分支；真实生产者/认证依赖尚未确定，不开放 system_prop。
- 安全芯片：libGPTEE_vendor 读 init.svc.qteeconnector-hal-1-0，值为 running 才切换 GPQTEEC，否则保留 GPMTEEC，所分析分支不等待/中止。两后端及 qteeconnector 可执行文件存在，定向 init/vintf 未见该服务声明，不称服务绝不存在。私有 init 类型受 Treble neverallow 保护，不绕过。后端影响须实际 eSE/OMAPI/APDU 证据，普通 NFC 读卡不覆盖。
- Mtd：启动 DAC 检查失败仍继续服务注册，可选 attestation 重载失败则 skip；不盲加 dac_read_search 或改 persist/UID。
- 相机 HAL 属性枚举、缺少实现的 MiSys 服务、调试目录/私有属性探测不统一转换为 allow。
- audio.sys.noisy.broadcast.delay 和旧 radio 重启触发器的接口归属尚未确认，本轮不借机改音频或无线电逻辑。

分析 JSON 在本地 device-io recovery 目录的 fingerprint-property-analysis01、secure-element-property-analysis01、thermal-analysis01 中。均为静态/保存日志证据，没有本轮手机访问。厂商实现和原始日志不进 Git。

## Enforcing 路线及估计

按目前证据，目标是 **再约2–3版**：集中修复候选 → 有回退保护的短时 Enforcing 暴露必要功能阻点并修正 → Enforcing 冷启动和持续功能验收。此为估计，不是已测定的剩余版本数；出现新的启动/硬件阻点则需要调整。

全局 getenforce 显示 Enforcing、必要功能通过、原9静态宽松域/KSU例外处理清楚须分别报告。不能在尚未真实 Enforcing 试验时声称已经接近完成百分之几。中间和集中候选仍保持 Permissive；新候选刷入/首次正常重启、短时 Enforcing、Enforcing 冷启动分别需要具体授权。

工具负责字节/标签/加载策略/数据基线核对，手工只测直接受影响且仍有实机风险的功能；不再为每个 Permissive 包重复麦克风/指纹或全套硬件清单。

## 集中组件已通过，统一整包已启动

enforcing-prep-policy-build-01 exit0（约7分56秒），verified01首轮通过。最终策略`1eaa4c3edd8b9125a436029a52c8f7d673135ec55d7b4a956cf9134181f944fe`/1,604,627 bytes；68源码、相对安装基线10路径、有效权限新增32项/删除0项。32项均落在预定契约，额外文件权限未增加；上下文只新增一个热控状态精确映射，文件/seapp上下文、genfs、扩展ioctl及原9静态宽松域保持。蓝牙源侧迁移已核，实际build.prop待整包。

Mac/Ubuntu组件`delivery/ishtar-selinux-enforcing-prep-components-20261018-01`121项校验通过；tar1,215,178 bytes/SHA912f3108c2125506f4f272996677c40d4ed79643e1d3112460fb28bba00e2015。

统一整包新control下rom-candidate01：enforcing-prep-rom-build-01 PID933128，审计等待器933129。构建成功后只做电脑实际payload/分区及保留数据升级检查；不访问手机。CPU6/7、nice10、I/O7、Ninja2，不改其他任务。尚无本轮完整ROM/实机/Enforcing结果，禁止把组件通过当整包验收。

## 下一阶段受控Enforcing门禁（方案，未执行）

1. 集中ROM完整构建、实际34分区和属性归属检查通过，形成具体版本/ZIP摘要后，单独确认保留数据刷入及首次正常重启。当前旧授权不能使用。
2. 先读回新版本、磁盘/加载策略、精确属性和有效权限，确认数据/FBE/模块/快照基线；不以ROM文件已生成代替手机读回。
3. 另行给出具体短时Enforcing窗口和回退条件并获授权。看守进程必须使用全新token/目录/PID，确认实际uid0、ksu域、PPID/SID脱离ADB后存活，并在Permissive下先完成到期演练。旧mark-only演练不能当Enforcing回退证明，旧token不能复用。
4. 经授权的短时窗口结束自动恢复Permissive；主机也检查最终enforce=0和看守回执。窗口中按直接风险验证相机、解锁及连接等必要功能，记录失败和对应AVC，不按AVC数量决定是否成功。不额外改boot参数、常驻模块、自启动或重启。
5. 热启动短试不覆盖在Enforcing下初始化的HAL/TEE/模块与开机服务。需要后续具体Enforcing冷启动候选和明确回退镜像/Recovery路径，再单独获授权验证。
6. 原9静态宽松域与KSU例外须列出实际保留原因、进程/功能影响及处理方案；不能以全局显示Enforcing隐藏仍宽松的核心功能。现有证据尚未完成这一项。

若实际Enforcing发现功能阻点，先根据调用/标签/所有权合并修复和局部核验，再统一成下一整包；不恢复每几个allow单独刷版的节奏。

## 原9静态宽松域的实际入口核对（只读，未删策略）

完整证据device-io recovery/original-permissive-artifact-map01/analysis.json，17,447 bytes/SHA3e45f09efb2be20453d43a30ecbeb0fb98a8f78683625b8bc8cce1f5753f1184；Git只存裁剪的原创判断摘要，不存厂商脚本正文。

- backuptool：冻结system镜像有3个脚本，属于升级备份路径，不能称无用。
- qti-testscripts：冻结vendor init在正常boot_completed启动显式shell服务；冻结product镜像目标脚本缺席，同时该域shell入口权限尚有缺口，后续应一并审查启动入口和策略，不能仅删permissive。
- vendor-qti-testscripts：存在通用vendor shell的手动进入契约，虽未找到指定三脚本，也不能证明该域绝不会运行。
- 4个应用域的6个seapp目标：当前构建输出212个APK全部解析成功且无匹配。仅为该输出的负证据，不冒充冻结完整系统APK清点。
- debugutils/su：冻结镜像指定可执行路径缺席，策略入口仍在；AOSP su与KernelSU ksu分开报告。

选择性抽取的system/system_ext/product目录不能用缺文件作负证据；指定路径使用冻结镜像debugfs直接查询。没有因此修改源码、停止进程或操作手机。此表说明旧9域不是9个已确认的日常硬件故障，仍需按实际入口和必要功能决定处理方式。

## 本轮刷入前授权与下一版维护者展示（历史）

机主在整包build exit0后明确：“如果这个版本已经打包好了 那就先直接刷这个版本吧”。本轮对象为1792300213/SHA3b1e5186…；获授权在实际镜像审计和新鲜手机预检通过后保留数据刷入并首次正常重启，仍不含Enforcing。无需再次重复询问同一刷写授权；当前尚未写分区。

维护者展示留下一版，机主会提供名字及需要展示的资料，再接入设置页；本版不为此改源码/重包，不自行拿Git用户名替代展示名。

## 源码复现注意

选择一种方式：在已复现1792266978源码的树根应用累计补丁（`git apply --unidiff-zero /路径/candidate.patch`），或复制九份已审查source快照并运行一次属性迁移脚本；不要两种方式重复应用。补丁中的外部vendor.prop仅一行零上下文删除，故需要--unidiff-zero；其全文件不上传。使用冻结before文件在独立临时树应用补丁后，10个输出SHA均与实际编译源码匹配。
