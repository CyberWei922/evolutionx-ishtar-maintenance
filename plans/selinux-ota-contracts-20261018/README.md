# OTA 备份执行入口与错误传递

基线为已安装 **1792333073 / Permissive**。本批4路径源码已合入Ubuntu，工作输入87项；组件编译及verified03通过，**尚未打完整ROM、未安装、未做真实OTA或Enforcing**。手机模式没有改变。

## 已完成

- update_engine检查源分区解析失败及空路径，避免将解析失败误当成无需备份。
- 直接运行已标记的backuptool_postinstall.sh，首个exec不再是system()的sh入口。沿用brillo进程实现，保留stdout/stderr且不引入输出管道或无限日志缓存；每个失败分支仍执行next-exec清理、卸载与恢复块设备只读。
- backuptool_ab.sh记录收到的首个失败，继续必要阶段及清理；工作目录准备失败不执行addon，额外挂载失败不执行对应V3脚本，临时盘卸载失败不删除仍挂载的临时树。
- addon目录先完整复制到同一临时盘上的pending目录，复制成功后再提交，避免复制内容后返回错误时执行半成品。覆盖了“文件已写完但复制返回失败”的场景。
- helper保真mkdir/copy/move失败；恢复后继文件失败时不删除旧替代文件。内置50-lineage脚本向上传递helper失败，缺少可选备份仍正常返回成功。其余第三方脚本只检测实际收到的退出码，不能推断内部吞错全部消失。

## 核验

56个隔离主机场景：26个实际C++方法分支、15个完整脚本场景、6个helper场景、9个实际50-lineage消费者场景。挂载、块设备和运行路径使用明确测试替身；没有真实挂载/OTA，也没有Android Toybox或SELinux运行验证。

原生C++及初版组件build02 **01:48/exit0**；加入事务复制后的最终build03 **01:08/exit0**。实际脚本复制字节、update_engine符号副本与安装ELF的.text一致性及MountPartition内brillo Run调用通过。内核/KSU、相机/亮度/Recovery/刷新率/原版Settings保护载荷保持。

策略仍为 `694b6134dfd89758742f964df8d6f288203df5c766c75f2225b30af0fbaaed53`，与安装基线字节完全相同。没有新增allow、标签或例外，仍保留backuptool静态宽松域及既有KSU动态例外。

第一轮host gate遗留旧版本号，在进入编译前停止；第二轮正常编译。首次机器码核验未识别mangled PLT符号，verified02按实际符号复核通过，没有因此改二进制或策略。后续独立发现半途复制问题后补入候选02，最终以build03/verified03为准。旧失败/中间证据保留在机主本地。

## 仍未解决

update_engine挂载代理、inactive视图隔离、执行所需精确文件/FD权限、持久标签和security xattr恢复、V3与EROFS兼容仍需实现。现有postinstall_optional=true保持，本批不保证备份失败会阻止整个OTA；第三方脚本内部吞错也无法靠外层完全发现。没有关闭addon.d或以neverallow例外绕过。

下一步先实现受限挂载与标签方案，再做策略编译及独立授权的OTA/Enforcing运行和冷启动验证。这份组件归档不能直接刷机，当前无需机主重复硬件全量测试。

## 复现与许可

先按前一debug-ota计划复现1792333073源码，核对source-baselines.json的before SHA，应用source-changes.patch或复制4份sources，再核对working-source-files.json的87项。补丁已在独立目录应用并逐文件匹配。完整Android树与依赖仍需外部提供。

4份完整修改后源文件沿用Apache-2.0，原版权及上游LICENSE/NOTICE保留，相邻.license仅为元数据。不上传ELF、策略二进制、ROM或原始手机日志。
