# Backuptool 剩余收紧边界

本计划对应84项源码和最终02组件。它仍保留 `permissive backuptool`，不表示全部Enforcing已经完成。下面是后续实现约束，均不是已安装或已验收的功能。

## 已落地

- 非system/ext4分区继续常规postinstall；源ext4魔数与读取长度检查通过，避免将vendor/EROFS误入RW保留流程。
- 原子顺序保持：备份、恢复、sync。顶层返回它实际收到的首错；读取/挂载/退出状态和清理失败均有检查。
- 不新增通用系统文件写权限、SYS_ADMIN、neverallow例外；不关闭OTA/addon.d，不改内核/KSU。

## 仍需实现与验证

1. **明确备份启动阶段。** 当前只在system/ext4通过旧mount-count启发式启动，`GetPartitionDevice`的返回值与源文件系统适配还需完整处理。不能将挂载次数等同于全部修改或全部addon需求。
2. **让update_engine代理挂载。** 工作域不获得任意挂载/块设备写权限；需要为inactive目标和临时备份树安排受限视图。system现有`defcontext=postinstall_file`只是缺省，不会覆盖磁盘原标签。
3. **精确的执行与文件权限。** 当前`system()`先执行shell，与仅针对postinstall_exec的入口契约不一致。直接脚本启动可以处理入口，但不能一并解决挂载、fd/pipe、目标路径及标签问题。
4. **处理持久标签。** `cp --preserve=a`包含安全上下文及xattr；强制context挂载视图不能被当作新增文件最终磁盘标签正确的证据。需要验证最终inode模式、属主、标签、其他安全xattr，并验证下次启动后的可用性。
5. **内层错误与清理闭环。** backuptool_ab.sh尚未汇总addon各阶段失败；默认postinstall_optional=true也没有改变。必须保留顺序和清理，并明确失败如何影响升级，不能一律吞错或直接使用set-e而跳过清理。
6. **处理V2/V3差异。** 内置50-lineage为V2；第三方V3可能修改vendor/product/system_ext。EROFS不能靠普通RW挂载恢复任意文件，需明确能力与错误处理，不静默丢掉addon。
7. **再做单独授权的实测。** 先编译并验证隔离/允许边界，再安排真实OTA路径和Enforcing。普通Fastboot刷入、Permissive开机或短时桌面操作都不能替代这些验证。

文件入口：`device/lineage/sepolicy/common/private/backuptool.te`、同目录`update_engine.te`、`system/update_engine/payload_consumer/postinstall_runner_action.cc`、`vendor/lineage/prebuilt/common/bin/backuptool_ab.sh`及`.functions`。本轮修改了C++入口和顶层backuptool_postinstall.sh，未修改backuptool策略、ab.sh或.functions。
