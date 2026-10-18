# 调试域收紧与 OTA 保留路径安全检查

基线手机为 **1792325510 / Permissive**。前一 boot-contracts 批次的78项源码/4个静态宽松域未单独安装；本批继续合入，活动输入共 **84项**，在前批之上修改6个路径。

## 本批内容

- 删除目标脚本缺席的 `qti-testscripts` 服务和启动行，保留 `sys.boot_completed` 下的 `qcom-post-boot`。两个测试脚本域仅删除 permissive 声明，保留域、手工转域、已有权限及 `diag_use(radio)`。
- AOSP `su` 仅删除 permissive 声明、改正注释；全部已有 allow/dontaudit 保持。它与动态 `ksu` 域不同，未改 KernelSU 源码/内核。AOSP adb-root/VTS 调试工作流尚未实机验证，不能声称完全无影响。
- update_engine 只在system/ext4查询源槽，检查6字节读取/源ext4魔数及小端解码、RW挂载结果、正常退出状态，并在失败时尽力清理 next-exec 上下文/挂载/块设备写状态。顶层备份脚本保持 backup→restore→sync 调用顺序与次数，返回收到的首个失败码。

组件正常编译 **01:46 / exit0**。策略 `694b6134dfd89758742f964df8d6f288203df5c766c75f2225b30af0fbaaed53`，1,604,882 bytes；完整导出差异严格仅3条 typepermissive 删除，其他导出行一致，有效allow增删均0、标签/类型/genfs/ioctl保持。实际 update_engine ELF包含新检查，实际init字节/解析、radio diag规则和既有保护载荷通过。

23个实际候选 MountPartition 方法的依赖隔离测试，以及16个顶层脚本新旧失败组合测试通过。它们不执行真实挂载、OTA、SELinux转域或手机操作。完整源方法未改写后放入测试夹具；脚本仅在测试时替换绝对调用路径。旧顶层脚本的失败被吞掉行为已重现。

01整包在发布前发现EROFS适用范围问题，已仅停止自己的构建/等待器并留存记录，未形成交付ROM。修正后的02组件增加vendor/EROFS继续常规postinstall等测试并通过，当前完整包以02控制目录为准。

## 明确未完成的部分

候选静态宽松域剩 **backuptool一个**，另有保持原样的KernelSU动态例外。这不是SELinux完成百分比，也不是已完成Enforcing：手机仍是原9个静态宽松域/Permissive，未安装本批。最终完整候选 **1792333073** 已构建并通过34个实际payload分区及保留数据升级静态检查，见rom-build-summary.json。ROM为4,545,424,895 bytes，SHA `813f87bd22c32cf8d59b0287a01128e28969164cd5dfcf60b4f033c25983f503`，仅保存在Ubuntu正式delivery；Mac有78项薄交接。

backuptool 当前还会挂载分区、修改 inactive 系统文件；其执行/写入路径与 neverallow 存在实际冲突。完整收紧需要让 update_engine 代理挂载和块设备操作、隔离 inactive 标签视图，并处理 `cp --preserve=a` 的安全上下文/xattr和新增文件最终持久标签。不能只换context挂载就称修完，也不关闭OTA/addon.d。

内层 backuptool_ab.sh 尚未汇总所有 addon 返回码，外层只能保真它实际收到的状态；现有 postinstall_optional=true 还允许按既有流程处理可选阶段失败。本批不是全链路错误闭环或真实OTA通过。

## 复现与许可

先复现 `plans/selinux-boot-contracts-20261018/`，核对本计划 `source-baselines.json` 的before SHA，再应用 `source-changes.patch` 或复制6份 sources，最后核对84项 working-source-files.json。上游项目HEAD另见 project-heads.json；之前两份专有原文仍由前计划的哈希门禁生成器处理，不随本计划上传。

原Linux Foundation/Qualcomm Innovation Center的完整BSD式许可、专利免责声明均保留。AOSP C++/LineageOS脚本沿用Apache许可；AOSP su的public-domain NOTICE全文单独保留，新增维护内容按Apache-2.0。相邻 .license 是维护元数据，不复制进Android构建输入。不包含ROM、APK、策略二进制、私有手机日志或反汇编。

机主本轮选择“先继续源码修复，暂不切换”。新刷入、重启、Enforcing和真实OTA均未授权；完整构建和审计已完成、所有本轮worker已退出。包仍保持Permissive，未刷入手机。Ubuntu其他任务未调整。
