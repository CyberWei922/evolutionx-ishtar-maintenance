# 开机属性归属与测试域收紧

本轮基线为已安装的 **1792325510 / Permissive**。机主选择“先继续源码修复，暂不切换”。

## 已完成的源码与组件

- 78 项工作输入、10 个修改路径。两条旧 true 值从 vendor 初始化迁到 system：`bluetooth.profile.vc.server.enabled`、`ro.mediaserver.64b.enable`。保留值，不新增厂商对系统属性的写权限；旧键未改名为当前 VCP 配置键。
- 两个 `persist.radio.restart_rild` 动作迁到 system_ext 的 core-init 脚本。原 vendor RIL 服务定义、其他启动动作和两组 stop/start 命令保持；处理 vendor_init 无权读取系统 radio 属性时的触发器解析问题。
- 只删除 5 个当前包无匹配测试载荷的静态 permissive 声明，保留域类型/映射/已有规则。AON 测试域不是实际 AON HAL，未删除相机 HAL 或库。
- 正常组件编译 07:53、exit0。完整导出策略差异严格等于 5 个 typepermissive 删除，其余导出行一致：allow 增删均 0，标签/类型/genfs/ioctl 边界保持。实际生成的 init 文件和 7 条 system-only 配置检查通过。
- 原 Settings 回退、相机、亮度、Recovery 与刷新率保护载荷摘要保持。

新编译策略为 `0527e36204650c33bccb04ee6dee41436f8613c982a68a4ea18d02130e9059da`，1,604,918 bytes。候选静态宽松域剩 `backuptool`、`qti-testscripts`、`vendor-qti-testscripts`、`su`；KSU 动态例外未修改。

**这只是已验证组件，未打完整 ROM、未安装。手机仍为原 1792325510，原 9 个静态宽松域保持，全局 Permissive。** 不切模式演练按时结束不能当成真正 Enforcing 回退验证，更不能当冷启动通过。

## 复现源码

1. 使用前一 runtime-camera-stock-settings 计划复现 1792325510 源码，按 `source-baselines.json` 的 before SHA 核对本批文件。
2. 在完整 Android 源树根应用 `licensed-source-changes.patch`，或按哈希复制 `sources/` 的 8 份许可明确源码。保留原版权及 BSD/Apache 许可。
3. 完整 `vendor.prop` 和专有 `qcrilNrd.rc` 不入仓库。取得匹配基线后运行 `reproduce_external_changes.py --source-tree <源树> --apply`，脚本先检查完整 before SHA，生成后再检查 after SHA。
4. `working-source-files.json` 是本轮 78 项实际输入。10 路径的完整组合已在临时目录重现并逐一匹配，不改活动源码。
5. 正常编译 selinux_policy、两份 build.prop 和两个 init 目标，再对照 `compiled-policy-checks.json`。组件归档不是可刷写包，不能直接拿来替代完整 ROM。

## 剩余工作

| 项目 | 下一步 |
|---|---|
| qti-testscripts | 有 boot_completed 显式服务启动，但目标测试脚本缺席。先处理陈旧启动项和调试用途，不能只凭进程未出现就判不可达。 |
| vendor-qti-testscripts | 有 vendor_shell 手工转域路径；需明确测试支持范围后再收紧。 |
| su | 与动态 ksu 域不同。需核对 adb-root/VTS 调试路径，不连带改现有 KernelSU 或救援控制。 |
| backuptool | A/B OTA、addon.d/postinstall 保留路径随包存在；普通启动和 Fastboot 刷写没有覆盖该路径，须独立验证。 |
| 指纹/RIL 私有属性 | 已保留 getter/失败分支证据，但部分 AVC 无精确键/栈。继续追必要消费者，不加泛化系统/默认属性权限。 |
| Enforcing | 机主本轮暂不切换。新候选与冷启动仍需具体新授权；不重复已拒绝的测试或复用过期 token。 |

本次清理仅释放已完成的 1792325510 SSD 刷写暂存及 out 重复 ZIP，约 26.86 GiB。正式 ROM、源码和增量缓存保留。旧成功 runner 的暂存已删除，不能重放。私有原始证据在 Mac/Ubuntu 一致保存，Git 只含已审查源码、版权信息和脱敏摘要。
