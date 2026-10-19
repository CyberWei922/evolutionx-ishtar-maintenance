# Backuptool 暂存与离线恢复

已安装 **1792357362 / Permissive**：61文件/21分区范围、实际策略与执行标签读回通过；静态宽松域为0，加载仅动态ksu。机主随后回复“没问题 一切正常”，桌面、原有应用和照片及日常使用验收通过。见[安装结果](installed-readback-summary.json)及[有限启动观察](early-boot-observation-summary.json)。未做Enforcing/冷启动Enforcing或真实OTA，以下候选构建描述为历史。

本批基线1792347689，104项活动输入/19路径修改。完整候选 **1792357362** 已编译并通过实际镜像检查，尚未安装，全局仍Permissive，静态宽松域为0。见[整包摘要](rom-build-summary.json)及[双端归档](rom-archive-summary.json)。

## 修改

- Worker只暂存文件/元数据；由系统shell解释临时addon脚本，不授予临时文件执行权限。
- Broker提供只读inactive视图和受保护的新目标标签规则。继承seccomp固定进程组，回收工作进程及后台后代、卸载全部目标视图后，再调用原生Store离线恢复ext4。内核不变，不依赖未启用的PID命名空间。
- 按目标规则重建SELinux标签，保留uid/gid/mode/纳秒时间及非SELinux xattr原始字节；不直接向worker开放system写、块I/O或挂载。保留原有域基础权限，未宣称完整worker安全沙箱。
- Generic vendor私有文件仍受Treble限制，只读所需明确配置类型；EROFS恢复明确拒绝，普通只读postinstall/原有optional逻辑保持。不保证任意绕开标准helper的第三方脚本兼容。
- 原版Settings、MIUI默认入口、相机、亮度、刷新率、Recovery及内核/KSU作为保护载荷保持。

## 验证及待办

220项隔离主机场景通过：代理70、父方法33、脚本10、实际ext4镜像40、过滤器59及主机生命周期8。实际编译策略静态宽松域0，既有类型权限删除0，系统/厂商文件写、块设备与挂载权限检查通过。genfs/ioctl、property/seapp及保护载荷保持。

这些不是Android实际挂载、SELinux域切换/强制执行、真实OTA或Enforcing冷启动验收。全局仍Permissive，KSU动态例外不变；后续需另获授权进行必要实机验证。

## 复现及许可

先复现上一broker计划的1792347689基线，核对source-baselines.json中的10个原文件SHA及9个新文件缺席，应用source-changes.patch，再核对104项working-source-files.json。补丁已在隔离目录应用并逐文件匹配19项。完整Android树及外部依赖仍需另行提供。

Store的6份文件/构建描述/完整COPYING为GPL-2.0-only；其余broker、worker、脚本及集成策略为Apache-2.0，保留原始版权、NOTICE和LineageOS REUSE声明。依赖准确HEAD见provenance/backuptool-final-source-files.json。Git不含ROM、ELF、策略二进制、原始手机日志或签名材料，不据此声明整个ROM均可再分发。
