# 1792252694验收后的基础框架服务权限修复

基线为已保留数据刷入、读回且获机主功能确认的1792252694，仍Permissive。此处仅修改一个private策略文件，为MIUI专用域补app_api_service和radio_service的service_manager find；对应Activity、音频、传感器等15类已观察到的标准API目标，继承原platform_app已有的基础调用边界。没有复制platform_app全部权限。

工作清单59项入口Ubuntu control/ishtar-selinux-framework-api-20261018-01/expected-source-files.json，不能拿已安装版本旧59清单校验现活动树。本目录candidate.patch是1792252694基础上的一文件增量；它尚未进入新整包/手机，当前状态见build-status.json。

原MIUI ODM库执行、XDSP ioctl、QSPM、Mtd DAC/属性消费者及其他必要拒绝仍需分析；默认未知服务find、通用vendor库执行等没有放行。原9静态宽松域和KSU工程例外保持，未进行Enforcing运行或冷启动验收。

## 回退脚本的准确状态

附带原创enforcing-return-guard.sh只是临时诊断源码，没有开机服务或模块，永远不调用setenforce 1。已完成25秒dry-run：独立会话/PPID1/ksu域，在启动ADB命令退出后仍存活并按时写标记；全程未调用setenforce，Enforcing状态下的实际还原动作尚未验证。真实restore-permissive模式必须另获机主明确授权，重新核对上下文/策略和新token/时限，不能复用旧PID或计时窗口。它依赖本工程保留的ksu宽松域，不能描述为严格全域Enforcing方案。

第一次组件编译因绝对OUT_DIR导致Soong fsgen路径panic，未到策略编译；第二次沿用上一轮成功的OUT_DIR=out/SSD链接和完整构建环境。自己的CPU6/7、nice10/I/O7/Ninja2，其他任务保持运行。完整ROM和日志留机主本地，不进Git。

## 组件完成

build02 08:01/exit0，策略1,604,069 bytes/SHA3202ea7d5bed51f1a5b6c34d958d9ecc82bf057f76db733e4efc0727dd6eea08。15类实际框架目标精确find、保留Mtd/P3/后处理边界、默认未知服务/通用vendor库执行/QSPM负测、59源码摘要和原宽松域/平台应用权限检查通过。只改变4份策略输出，未新建完整ROM或安装。Mac/Ubuntu同名组件交接11项，tar535,469 bytes/SHAcdef5f2d…；不含刷写计划。
