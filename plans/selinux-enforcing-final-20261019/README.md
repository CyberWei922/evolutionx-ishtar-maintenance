# 1792357362 完整 Enforcing 包及 clean flash

完整 signed A/B OTA 已生成，清空 userdata/metadata 后正常完成首次设置并进入桌面，实际读回 Enforcing/1。机主干净初始化相机、收音、指纹及显示效果验收待补充；本记录不提前记全功能通过。

## 成品

- `EvolutionX-16.0-20261019-ishtar-11.12-Unofficial-Enforcing.zip`
- 4,545,237,004 bytes；SHA256 `d604f00d55c647e49ca0767ae259e7b208e6a99ae55e54b7e3bf5b094ba4c63a`
- 完整包保存在 Ubuntu `delivery/ishtar-enforcing-final-1792357362-20261019-01`；Mac 同名目录为薄交接，Git 不含 ROM。
- 内部版本仍1792357362，仅 vendor_boot/vbmeta 相对最后 Permissive 整包变化，精确匹配已通过开机测试的 Enforcing 两镜像。其他32分区、内核/KSU、Recovery、相机、亮度和刷新率修复保持。

## 生成及复现范围

104项工作源码与上一启动组件记录一致，没有新的Android源码改动。先复现 `selinux-backuptool-final-20261019`，再叠加 `selinux-enforcing-boot-20261019`；本计划重新生成完整payload、OTA签名和属性，而非只替换ZIP外层文件，也没有全量重编Android。

`scripts/package-worker.py` 和 `scripts/audit-package.py` 为本次真实原创主机配方，固定目录和open-x记录用于保留已完成证据。**不得直接重放**：以后使用新的控制/工作目录，提供完整外部target-files、已验收的镜像和依赖。配方没有手机读写功能。

输入镜像按IMAGES/RADIO核对全部34分区；复制私有META/必要属性、VINTF/安装元数据，镜像只作只读硬链接输入，替换两镜像前先解除链接；不改原target-files。使用现有Android `ota_from_target_files --backup=true --max_threads=4` 和原证书对应的外部签名材料重新签名。CPU4–7/nice15/I/O7，不用GPU、不调整其他Ubuntu任务。不能仅克隆本维护增量仓库就构建完整ROM。

实际ZIP CRC、payload属性、整包CMS签名及payload RSA签名独立核对通过，全部34镜像回解并匹配。详细文件哈希见 actual-payload-verification.json。签名材料、ROM、镜像和原始日志不进入Git。

## 清刷及实际读回

独立新执行记录完成74个镜像动作、73次完整下载；清刷单独执行userdata和metadata各擦除、刷入已验证空F2FS，四项动作和两次下载完整核对。两数据分区容量来自实机，F2FS来自包内保持不变的vendor fstab；Recovery虽报raw，未让fastboot猜文件系统。没有擦frp/persist/misc、写底层固件或锁BL。首次正常Enforcing重启只执行一次，没有runtime setenforce。

干净系统为user构建（ro.debuggable=0），旧Root安装与授权已清除，su不可用。37个可读文件、磁盘策略、6个MIUI默认入口匹配；24个受保护文件、块设备、加载策略和bootconfig未重新读回，不把上一脏刷的61文件/21分区结果冒充本次结果。没有为检查安装Root、放权限或修改默认入口。7样本跨度约64秒全部E，核心PID变化0；开始于首次设置和ADB授权之后，不是全开机日志或零AVC证明。

首次检查因受保护相机库停止，原失败保留；第二份检查明确跳过不可读文件并记录边界。两次SSH连接超时只影响状态读取，自主刷写完整成功，没有重试分区；网络原因未证实。

正式包原构建清单在刷入前冻结，里面clean_flash_accepted=false是当时状态；后续结果见本计划clean-install-summary.json与独立机主补充，冻结清单不改写。

## 后续边界

真正Recovery/在线OTA安装、长时使用和断电冷启动尚未验收；既有快充问题本轮未处理。KSU内核保持，但本次没有Root可读的加载策略，不能复用上一轮动态ksu域读数作为本次实测。旧Permissive完整ROM、两项回退与修好Recovery均保留。B槽不是完整ROM回退。刷入、清数据与首次重启授权均已用完，不自动追加操作。
