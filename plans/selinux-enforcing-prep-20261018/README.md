# SELinux 集中修复与 Enforcing 准备

## 当前状态

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

## 源码复现注意

选择一种方式：在已复现1792266978源码的树根应用累计补丁（`git apply --unidiff-zero /路径/candidate.patch`），或复制九份已审查source快照并运行一次属性迁移脚本；不要两种方式重复应用。补丁中的外部vendor.prop仅一行零上下文删除，故需要--unidiff-zero；其全文件不上传。使用冻结before文件在独立临时树应用补丁后，10个输出SHA均与实际编译源码匹配。
