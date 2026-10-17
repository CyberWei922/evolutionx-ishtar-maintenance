# SELinux phase 3 — historical proposal, now source-applied

更新：机主授权继续修复后，本目录两文件已合入Ubuntu活动源树、独立策略编译与二进制精确权限验证通过。当前源码叠加phase4/5，累计增量见 `../selinux-next-20261017/`，已随1792246397保留数据刷入并正常启动。仍Permissive，Enforcing未验收。以下未应用描述保留为本方案准备时的历史状态。

2026-10-17。当前手机1792226532，开机低刷已获机主首次滑动验收，SELinux仍全局Permissive。本目录只是下一小批方案，未写入Ubuntu活动源码、未编译、未刷机、未切Enforcing。旧日志用于定位，未恢复已停止的手机采集。

## 首批只改两个策略文件

| 改动 | 既有logcat缓冲内依据 | 当前驱动/策略定义 |
| --- | --- | --- |
| Fidoca和Mtd仅补smcinvoke 0x9801 | Fidoca 1条，Mtd 4条 | securemsm-kernel/smcinvoke/smcinvoke.h:83–92，INVOKE_REQ；原有0x9803为SERVER_REQ |
| Mtd仅补QCE 0x870b | 1条 | securemsm-kernel/linux/qcedev.h:358、378–381，UNMAP_BUF_REQ；原有0x870a为MAP_BUF_REQ |
| Fidoca设置既有FIDO能力属性 | persist.vendor.sys.pay.fido，1条 | common sepolicy/vendor/property_contexts:14已经是vendor_payment_security_prop，property.te:8为vendor_public_prop；set_prop宏含所需属性socket及set/get |

候选为candidate.patch，完整候选源文件旁置；source-baselines.json记录原/新SHA及机主本地证据摘要。没有上传手机原始日志。修改只针对已有专用设备类型和属性类型，不增加通用init、device、default_prop权限，不增加permissive域。

## 后续分开解决

1. QRTR：已有域已实际运行，/data/vendor/qrtr目前通用vendor_data_file。考虑专用数据类型与精确路径、日志对应目录/文件权限；common init创建目录后restorecon_recursive处理保留数据升级。现有mkdir不会给已存在目录自动重新标记，不能只改file_contexts就宣称修好。候选尚未加入此项。
2. Mtd属性：widevine落入vendor_system_prop，rpmb落入vendor_default_prop。先查消费者、值类型，再定精确标签和公开/受限范围；不直接允许写泛型属性。vendor_tee_listener_prop有明确读拒绝，可独立核对get_prop。
3. persist/DAC：persist/data有vendor_persist_data_file标签，init按0700 system system创建，Mtd是root UID。需确认具体子路径与凭据；不放行dac_read_search、不宽泛chmod、不批量开放persist。
4. 9个原静态宽松域：su、backuptool、qti-testscripts、vendor-qti-testscripts、vendor_pdt_app、aoncameraservice_app、vendor_hal_debugutils_default、vendor_cta_app、vendor_logkit_app。7个Qualcomm测试域来自userdebug/eng宏或test目录；su/backuptool还涉及构建允许清单，不能声称换user就全部消失。
5. KernelSU在kernel/selinux/rules.c:86–87动态创建并设ksu宽松。保留root时必须单独记录有效策略例外，不能只凭getenforce说所有域已强制。

## 实施顺序

先核对2个原文件SHA并备份，合入首批候选；独立编译selinux_policy、保持neverallow，核对生成二进制中的精确ioctl、属性权限及域转换，确认既有45项集成基线没有意外变化。该步骤不等于手机功能修复完成。

通过后再准备保持Permissive的验证构建；重新核对磁盘门槛、回退包和源码差异。刷入/重启按项目要求单独确认。按需要重新部署有时限的定向审计，不重跑已清理的历史采集脚本。逐批处理以上余项和原宽松域后，才安排有回退路径的Enforcing运行及冷启动验证。

日志audit_lost最大1612，现有拒绝不完整；这里没有承诺首批补丁就能安全Enforcing。相机、亮度、Recovery和已接受的启动刷新率修复应作为回归范围保留。
