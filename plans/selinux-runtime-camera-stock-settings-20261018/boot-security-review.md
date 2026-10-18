# 1792325510 开机安全日志复核与下一步

## 本轮已完成

- 保留数据刷入及首次正常启动完成；48 文件、21 分区范围、7 组新加载策略契约核对通过，141 次命令尝试、0 重试。
- 机主确认“已恢复原版界面，正常”。只记录设置页面回退验收，没有追加全硬件验收。
- 本版新增相机 HAL 的标准 QSPM 客户端与 MIUI CPU/IO PSI 只读权限，累计有效权限新增 13 项、删除 0 项。所有新增都已读回加载策略核对。
- 全局仍为 Permissive，原 9 个静态宽松域与动态 KSU 域保持。新策略没有做 Enforcing 或 Enforcing 冷启动测试。

## 本轮记录的覆盖

电脑端有限采集已自动结束，32 次状态样本覆盖 uptime 15.90–195.97 秒，首次识别新启动为 15.76 秒；26.83 秒样本已出现 boot_completed=1。保留的内核缓冲区还包含更早事件，但这不等同从内核启动零秒起连续采集。

system_server、SurfaceFlinger、audioserver 的采样 PID 没有变化；无传输失败，两个流未触及 8 MiB 上限。早期 observer 保留内核安全行 105 条、logcat 安全行 115 条；启动后读回另有有限快照。跨流和快照有重复，不能直接相加当独立事件数。

观察到累计 audit_lost 最大 844、audit_rate_limit=5 和 rate limit exceeded，所以日志明确不全，AVC 数量不能用作修复百分比。没有更改手机审计速率、添加模块或启动项。所有本轮有限构建、审计、刷写、读回与观察进程均已结束。

## 剩余拒绝的处理

| 类别 | 本轮证据及下一步 |
|---|---|
| 开机属性归属 | vendor_init 仍尝试写 bluetooth.profile.vc.server.enabled、ro.mediaserver.64b.enable，并读 persist.radio.restart_rild。前两条在 common 的 vendor.prop 实际定义；本轮定向 git grep 未在 frameworks/av、Bluetooth 项目中找到对应键，尚不能由此断言无人使用。下一批先查精确消费者、分区初始化归属和默认值，再决定迁移或删除，不给 vendor_init 泛化读写权限。 |
| 指纹与 RIL | hal_fingerprint_default→system_prop、rild→default_prop 只定位到属性区，尚未定位精确键；ro.vendor.oem.psno 输出有精确名字但必要消费者仍未证明。先查读取调用与失败分支，不能给整个 system_prop/default_prop/vendor_default_prop 放行。 |
| 传感器、安全元件、模块加载 | 复现旧默认属性或私有 init 状态读取，以及 vendor_modprobe→block_device 目录探测。沿用已有回退分析，必要冷启动依赖仍要实测；不能因为 Permissive 启动成功就宣称安全可忽略。 |
| 相机 HAL 属性遍历 | 保留快照中 172 个属性文件类型组出现读取/打开/映射等拒绝，含大量与相机无关的系统私有项。先区分遍历与必要键读取，不据数量逐条加 allow。 |
| 相机应用与 Google 私有属性 | 后段安全日志实际出现 MIUI 专用域，但只证明发生了该域访问；初次 ps 快照未见 MIUI，不能据日志称所有拍摄模式或算法回退通过。精确厂商键及必要性仍待确认。 |
| 诊断和第三方探针 | shell getprop 拒绝与本轮自动读回时间重叠，属于诊断产生的访问；第三方监测工具的电池/proc、Google 私有属性与设备探针、/data/adb、音频 debugfs 均不直接转成通用权限。 |

## 进入 Enforcing 的条件

必要启动依赖需要有精确修复或可验证的失败回退；可选探针可以继续拒绝。还必须单独审查原 9 个静态宽松域与 KSU 例外，并在具备回退条件后进行另获授权的 Enforcing 冷启动及相关功能验证。旧 1792311835 的短时运行态测试不能替代本版冷启动验证。

本候选刷入与首次正常重启授权已使用。没有额外重启、再刷写或切换 Enforcing；没有清理旧 ROM/暂存，没有调整 Ubuntu 的其他任务。原始日志和加载策略仅保留在机主两台主机，Git 仅保存脱敏摘要与已审查源码。
