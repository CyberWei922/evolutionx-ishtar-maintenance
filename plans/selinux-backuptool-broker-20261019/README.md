# Backuptool 原生挂载代理

当前工作源码94项输入、10路径修改，基线为已安装1792339544。完整候选 **1792347689** 已编译并通过实际镜像静态核验，**尚未安装**；全局仍Permissive，保留backuptool静态宽松域及KSU动态例外。见[整包摘要](rom-build-summary.json)及[归档](rom-archive-summary.json)。

## 本轮修改

- 新增原生backuptool_broker，只有调用域精确为update_engine才进入设备检查；拒绝相同源/目标设备、非ext4块设备、已挂载或非目录工作入口。
- broker在自己的挂载命名空间关闭传播，准备tmpfs工作根、inactive system和固定system视图，再将单独子进程切到backuptool域执行原有backup→restore→sync。
- 临时目录只建在新tmpfs上，不向目标system建立tmp目录。tmpfs只设置rootcontext，保留逐inode安全xattr能力，未用context=把整盘强制同标签。
- 原生代理尝试全部已知挂载的清理，保留收到的首错；update_engine即使遇到启动失败或信号退出也尝试恢复system块设备只读。继承stdout/stderr，不增加无限输出缓存。
- 新broker调用时，shell只使用代理已准备的临时区，不自行挂载/卸载tmpfs。旧版已安装update_engine直接调用新顶层脚本时，保留带明确过渡标志的旧临时盘流程，并检查postinstall已挂载、失败卸载不删除树；直接调用ab脚本而没有两种入口标志会在修改前拒绝。V3额外挂载逻辑仍保留，尚未迁到代理。
- 新增两个类型及一条执行文件映射。现有类型新增5项权限，均限update_engine：rootfs目录mounton、tmpfs文件系统mount/unmount/relabelfrom、自进程setfscreate；未增加backuptool SYS_ADMIN或一般system_file写权限。上游neverallow未修改。

## 核验及边界

72个隔离场景通过：28个代理顺序/失败清理、23个实际MountPartition、2个实际代理脚本流程、5个旧更新器兼容流程、14个实际Validate函数的调用域/设备身份前置检查。所有挂载/设备/标签输入均为明确测试替身；没有真实Android挂载、OTA、Enforcing运行或冷启动验收。实际安装ELF与符号副本.text一致，实际文件标签映射及策略差异核验通过。相机/亮度/刷新率/Recovery/原版Settings/内核/KSU保护载荷保持。

本轮命名空间处理挂载传播，**不是worker安全沙箱**。剩余精确访问权限、inactive路径隔离（包括脚本/符号链接）、持久SELinux标签/security xattr、V3/EROFS以及optional OTA错误契约均未宣称解决。任意第三方脚本内部吞错仍不由外层完全发现。

首次host清单计数门禁失败后完整回滚；更正为94输入后构建。中间build02因新增断言与原有userdebug overlay_remounter规则冲突而失败，修改自身断言范围并保留原有上游规则后build03通过。首次产物检查要求完整调用域常量连续出现在ELF中，被编译优化打散而停止；按真实Validate函数测试与符号副本.text核对修正检查器，未为此改二进制。失败原始记录保留本地。

## 复现及许可

先按上一OTA-contracts计划复现1792339544，核对source-baselines.json中6个原文件的before SHA；4个新文件应不存在。应用source-changes.patch或按sources复制10文件，再核对working-source-files.json的94项。本补丁已在独立目录应用并逐文件匹配。完整ROM01虽然编译exit0，但打包期间发现旧调用方不提供代理标志，交付前停止审计并删除该不可交付ZIP；已补齐兼容源和5个失败/成功场景，最终以组件04与完整ROM02为准。完整Android树、专有依赖及既有构建环境仍需外部提供。

10份文件均为Apache-2.0，原版权/NOTICE与LineageOS REUSE声明保留；.license仅记录归属与本次修改。Git不包含ELF、策略二进制、ROM、原始运行日志或签名材料。
