# 相机公共库、QSPM 与 DSP HAL 客户端

基线为已安装 1792252694 加上一批 `selinux-framework-api-20261018`。本目录是两文件累计增量：三份现有公共算法库/HTP依赖的精确标签，以及 MIUI 专用 QSPM/DSP HAL 客户端。组件及累计完整ROM1792257806已完成编译、实际镜像核验和归档；机主已授权保留数据刷入及正常重启，已完成刷入、正常启动和38文件/21分区读回，机主功能验收待答，仍保持Permissive，不含Enforcing试验。

`candidate.patch` 相对上述基线；`source-baselines.json` 记录前后 SHA。完整原创 vendor 策略文件保存在 source，file_contexts 仅上传新增片段和补丁，其完整旧文本继续外部留存。59 项工作输入摘要见 working-source-files.json，只含路径、尺寸和 SHA，不含厂商实现。

## 已确认的边界

- mialgo 和 SNPE 已在现有 public.libraries 清单中；HTP stub 是被当前 app 运行链使用的依赖。三条仅匹配具体 64 位 `.so`，使用 vendor_public_lib_file；公共库名单、namespace、ELF/APK均不变。该类型具有标准应用共享加载语义，不能描述为仅 MIUI 可读取。
- MIUI 作为 QSPM HAL 与 DSP HAL 客户端；保留其现有 nonsecure DSP read/ioctl。配套驱动注明应用由 HAL 代开设备并保留 PID/unsigned-PD 检查，库中有相应 HAL 回退字符串；实际分支和算法影响仍待实测。
- secure DSP 节点保持 vendor_xdsp_device 和 Qualcomm neverallow。没有新增允许直接访问的例外，没有更换节点标签或放行任意厂商库。
- 原9静态宽松域、platform_app规则、Mtd/P3/音频/后处理保持。KSU动态例外和所有 Enforcing 验收仍单独待办。

## 组件结果

最终 `camera-dsp-policy-build-01` 编译和 `verified01` 通过。策略1,604,093 bytes / `88ff29aab9c50ef5c7e765fcec01b09413d647894dcab9981cb6c39818706f5b`。14项完整合并上下文的原生libselinux路径解析、QSPM/DSP HAL find/Binder、secure/generic vendor负向边界、59项源码摘要通过。详见 component-build-summary.json。

Mac/Ubuntu `delivery/ishtar-camera-dsp-components-20261018-01` 108项校验通过；组件tar 1,208,106 bytes / `5856e2c80c88d4152a1f27dd0ab5eba262cd84b84f6e4360457a2a0ad1922ecb`。编译策略和手机原始日志未上传。

前一组库/QSPM组件有三轮补充核验脚本失败，修复后通过；其整包在发现DSP客户端缺口后仅停止本任务进程并重新合批，没有交付旧包。其他Ubuntu任务持续运行。最终整包结果另记，不能把组件通过写成实机修好。

运行结果见runtime-summary.json；DSP服务存活和HIDL注册通过，实际HAL代开回退仍未证明。新日志揭示vendor libc++_shared依赖及其他余项，后续逐边界处理；不上传原始日志/策略二进制。
