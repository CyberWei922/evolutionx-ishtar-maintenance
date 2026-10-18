# 原版设置恢复与实际 Enforcing 日志修复

## 1792325510设置回退验收、开机日志收尾（最新）

机主确认“已恢复原版界面，正常”，只记设置回退通过。有限开机采集已 complete 并结束：32 状态样本至 uptime195.97秒，核心三进程PID无变化、无传输失败/流截断；缓冲区含更早日志，但 audit_lost最大844和速率限制使覆盖不全，不称零拒绝。Mac/Ubuntu的30文件运行证据归档一致（1,524,234 bytes/SHA ee86c70d4999d6f729cfe0c326eaa287f89ea2f6de0d7c8b046eb460b868bbe9）。所有本轮有限worker已结束，手机无需继续连接USB。

下一批先查三条legacy开机属性归属、指纹/RIL精确属性消费者及必要冷启动依赖；相机HAL大量属性遍历、第三方/诊断探针不批量放行。两个剩余vendor.prop定义已定位，尚未据此改源。初次MIUI进程快照缺席，后段安全日志见专用域，两者区分。当前71源与1792325510已安装策略保持，全局Permissive；新策略/冷启动Enforcing仍未验收，原9宽松域/KSU保持。详见维护仓库 plans/selinux-runtime-camera-stock-settings-20261018/boot-security-review.md。

本轮不再刷入、重启或切模式，旧ROM/暂存未清理，Ubuntu其他任务未调整。首次本地归档下载脚本根路径多了一层recovery，读取回执前停止；修正后一次只读下载及30文件校验通过，未因此重跑远端归档或手机操作。


## 1792325510已保留数据刷入、正常启动并归档读回（最新）

74动作/73完整下载及wire核对通过，首次正常重启后48文件/21分区范围与7组新加载策略契约匹配，数据基线保持；141命令尝试/0重试。自定义维护者载荷实际缺席、原版Settings摘要正确，六MIUI默认入口保持。磁盘策略ade0b38afe44ecd4a0afbd364978ffa6479c58b9ad5dbba8958420f801dfb818，加载策略7d32c21a5832b15bf88b6694047c4756dcf2b31baedb6d4746428873372a649c，原9静态宽松域+ksu保持，仍全局Permissive/enforce0。没有新策略E/冷启动验收，不重复麦克风/指纹全套测试。

有限开机安全observer状态complete，首次实际读到uptime=15.76秒；覆盖范围与速率限制/丢失另见early-boot-observation-summary，不能声称全量或零拒绝。原始有限日志、读回和刷写证据已在Mac/Ubuntu一致归档，原始日志不入Git。机主已确认“已恢复原版界面，正常”，只记设置回退验收。该候选刷入及首次正常重启授权已使用，不复用作额外重启或切Enforcing；旧完整ROM/暂存暂未清理，未调整其他任务。


## 最终候选1792325510完整构建与实际镜像核对通过（最新）

最终02整包及audit/worker均complete：34实际payload分区、71源/17路径、策略ade0b38afe44ecd4a0afbd364978ffa6479c58b9ad5dbba8958420f801dfb818、13项有效权限新增与保护载荷通过。自定义维护者APP/RRO在实际镜像缺席，Settings与固定上游恢复组件一致；五条旧值仅在system。相机HAL QSPM客户端与MIUI CPU/IO PSI只读合入，没有放开安全DSP或缺失MiSys。仍Permissive，原9静态宽松域/KSU保持，新策略尚未在手机运行/E实测。

Ubuntu正式delivery/ishtar-selinux-stock-settings-permissive-candidate-20261018-02，incremental1792325510，ZIP4,545,440,761 bytes/SHA25ac87915377aa8e914e4951bf170528a822df807127f6afccd0994e7e645796；Mac同名薄交接不含ROM。保留数据升级静态检查通过，完整OTA签名未独立密码学核验。机主已授权该下一候选现场检查后保留数据刷入、完整核验后首次正常重启，不重复询问。随后将用有限电脑端observer尽早读取该次正常开机的安全日志，约180秒/每流8MiB，不修改手机日志速率或启动项；实际覆盖以first_observed_uptime和丢失记录为准。

当前手机仍1792311835/Permissive；旧基线实际短时E功能验收与回退兜底限制另见试验摘要，不外推本候选或冷启动。未停其他任务、未清理旧ROM/暂存。


基线1792311835；71项输入、17路径累计改动，含11个自定义维护者页面文件删除。恢复固定上游Settings控制器，保留MIUI默认相机和已验收载荷；两条音频/蓝牙旧值迁至system初始化。新增相机HAL的标准QSPM HIDL客户端，以及MIUI域CPU/IO PSI read/open/getattr，无写/map/ioctl。

组件已通过neverallow编译与完整有效权限增量核对；最终ROM/安装状态以build-status.json及后续摘要为准。原9静态宽松域和KSU保持，新包仍Permissive。

此前安装基线实际短时Enforcing中，机主确认桌面/MIUI打开/1×与5×切换正常，关键进程及boot保持。已恢复Permissive；电脑回退兜底也参与，不能证明独立guard单独完成真实恢复；冻结自动结果仍为false。新策略及冷启动尚未在Enforcing下测试。两次切换前的脚本格式失败保持真实记录，未算成功。

上传五份已审查Apache-2.0源文件及最小补丁、删除记录和元数据。完整vendor.prop、APK/镜像/策略、原始手机日志、ELF反汇编、二维码图片和密钥不入仓库。旧计划保留历史，不重新应用维护者UI。复现外部属性变化使用SHA门禁脚本。

复现删除时，文本补丁移除九份页面源码/资源；两张原私有JPEG未放入补丁或仓库。若本地基线仍有它们，按source-baselines.json的deleted与before_sha256核对后移除对应两条项目内路径。它们没有Android.bp打包入口，下一ROM不需要提供这两张图片。
