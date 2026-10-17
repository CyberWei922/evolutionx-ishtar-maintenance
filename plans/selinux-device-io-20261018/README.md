# 设备IO与attestation精确状态

基线1792261801已保留数据刷入，机主确认此前所问数据、MIUI拍照/有声新录像及指纹没有问题。手机继续全局Permissive。本轮六路径/三新增文件已合入并通过策略组件编译和独立权限核验，累计整包1792266978已完整构建、实际镜像和静态升级检查通过；尚未安装本增量。

## 本轮修改

- Mtd：实际libmt@1.3.so及运行拒绝对应persist.vendor.sys.attestation.status，新增vendor_restricted专用类型和exact string，只给vendor_mtd标准set/get；继承现有精确状态类型契约，不放宽vendor_system_prop。
- GNSS：INS读取现有vendor_sensors_prop，仅getattr/open/read/map，整个现有类型可读，不能称单key隔离。不加set。
- Goodix：实际库NetlinkRoutingThread与内核私有协议25、运行四项拒绝对应；self netlink_socket仅新增create/bind/write/read。此SELinux class不能按25编号进一步隔离，不加网络或capability。
- system_suspend：只给实际SPI设备下/wakeup一个genfs前缀使用sysfs_wakeup，复用已有权限，不放行通用sysfs。

## 证据与限制

non-mtd-analysis-next.json和mtd-attestation-analysis-next.json为筛选元数据，包含固定库摘要、源码定位和LLVM地址，不含厂商实现或原始手机日志。REA后端已知不可用，未重试安装；不能把LLVM证据称REA反编译或真实功能调用。

Mtd主程序检查/restorecon失败后仍注册服务，不能称该DAC拒绝必导致开机失败。attestation线程stat失败会写skip并返回，可能跳过该可选重载；修状态上报不等于修硬件认证。不改UID、persist属主/0700、DAC绕过权限，不触发provisioning/密钥操作。静态扫描没有排除动态构造属性key的其他消费者。

candidate.patch相对1792261801；source-baselines.json为六路径前后摘要，working-source-files.json为活动62输入。source中的六份维护策略为原创Apache-2.0；原始闭源载荷、编译策略/镜像/ROM和原始运行日志不进Git。

已有相机/默认入口、Recovery、亮度、显示同步、内核/KSU保持，按机主要求暂不调相机流畅度。secure XDSP禁止、未知属性/HAL枚举、原9静态宽松域和KSU及Enforcing功能/冷启动仍待分别处理。新刷入/重启和短时Enforcing分别需具体确认，不复用已消耗授权。

## 完整候选已留存

版本1792266978，ROM 4,545,439,425 bytes，SHA256 5021325057439cbbca4a35f89cb32c24575ebc5ebeefe2da3994f96e3c28b19b。完整包仅在Ubuntu：/mnt/android-download/delivery/ishtar-selinux-device-io-permissive-candidate-20261018-01/EvolutionX-16.0-20261018-ishtar-11.12-Unofficial.zip。Mac同名delivery为薄交接，不含ROM。34实际payload分区和62源码、实际策略、GNSS/Goodix/Mtd库及原保护载荷通过；静态保留数据升级检查通过。完整OTA签名未独立密码学核验。

所有本轮电脑构建/审计worker已结束。手机仍机主已测1792261801/Permissive，下一候选刷入/重启待单独确认，Enforcing另确认。完整ROM1792261801与1792257806继续作为回退，本新候选未验收前不提前清除它们。

电脑工具修正记录：首次审计因旧任务名前缀正则停止；新rom-audit02的38步骤实际镜像审计通过，随后的首次preflight仍指向旧审计目录而停止。仅修该路径后独立preflight02通过，最终以candidate-finalization-status complete为准；原failed worker记录保持，不写成它已成功。未重编ROM或改策略。
