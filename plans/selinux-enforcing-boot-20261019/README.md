# 1792357362 开机 Enforcing 启动组件候选

当前：已按机主本轮“刷”完成 A 槽 vendor_boot/vbmeta 两项刷入，首次正常启动实际为 Enforcing。61 文件/21 分区范围/数据保持自动核对通过；静态无宽松域，保留 KSU 动态例外。有限启动记录已结束，机主本轮功能验收待答。详见 installed-boot-summary.json。

后续验证重点：日常通信、蓝牙、相机及长时使用；有限 AVC 不等于功能失败，不整体放宽 default 属性或 DAC 能力。断电冷启动和真实 OTA 尚未验证。下面候选准备说明为历史。

候选已构建并静态核验，尚未刷入或重启。机主要求重启之前告知，当前没有本候选刷入/重启授权。

## 变更与产物

仅修改 BoardConfig 的 androidboot.selinux / enforcing 启动参数及对应注释；104项源码输入中只有1路径变化。GKI 的 boot/init_boot 命令行为空，参数实际在 vendor_boot。

候选为 vendor_boot.img（96MiB）与匹配的 vbmeta.img（8KiB）；仅拟写当前A槽这两项。原 kernel、ramdisk、DTB、表及 SELinux 策略保持；root VBMeta 的28个描述符仅替换 vendor_boot 哈希，原 AVB flags3/回滚索引/签名公钥和属性保持，签名已验证。不是完整ROM或OTA。

正式组件及原Permissive回退在Mac/Ubuntu同名delivery目录，原完整1792357362 ROM和7个回退启动镜像继续保留。原Recovery触屏修复SHA保持。现有super只填A槽，B槽不是完整ROM回退。刷前需另做现场身份/版本/策略/数据/快照等检查。

## 复现

先复现已安装1792357362的104输入基线，确认BoardConfig原SHA83723616…，使用 `git apply --unidiff-zero source-change.patch` 应用不含空白上下文的等价补丁，再核对working-source-files.json。sources保留修改后完整Apache-2.0文件和原版权。

scripts里的构建/封装脚本为维护配方，使用现有完整Android树及外部已安装ROM输入；需要为新的控制目录调整路径，不应重放已完成任务。增量目标vendorbootimage，构建6:39/exit0。原生AVB接口先逐字复现原root VBMeta，再替换唯一vendor_boot哈希并重新签名；签名材料、基线ROM及工具依赖不入Git。完整树、外部依赖与密钥仍须由机主另行提供。

## 验收边界

此前短时运行Enforcing与自动回退、指定指纹/桌面/相机功能已经验收。本候选只通过主机镜像和签名核验；从开机开始的Enforcing、长时间运行及真实OTA仍待验证。当前手机最近确认P，准备阶段没有手机操作。详见component-candidate-summary.json。
