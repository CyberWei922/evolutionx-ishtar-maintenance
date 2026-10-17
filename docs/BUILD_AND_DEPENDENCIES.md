# 构建基线与外部依赖

1. 依据manifest及provenance/local-projects.json取得固定上游版本；不自动repo sync、升级或重置现有工作树。
2. patches对应原HEAD，source-files是其中23份明确许可文件的修改后完整字节，旁置.license不参与编译。按补丁或完整文件择一恢复，不能重复应用。
3. source-overrides.json列22份选定文件；external-text-dependencies.json列302份不上传内容的原厂/生成文本及SHA。生成vendor构建文件需要从合法取得的依赖重新提取生成，或使用机主本地现有文件。
4. binary-dependencies.json列1261项二进制引用。相机APK、厂商库、固件、Recovery及相关授权分别管理；仓库元数据不授予其再分发许可。
5. 亮度生成器仅读取单独取得的原始/已验收XML，写入新的输出目录；全部SHA须匹配。它不安装手机、不修改活动源树。新参数应另建版本。
6. accepted-source-files.json保留当前45项集成文件的SHA，包括外部文件；不能把记录存在理解为载荷已上传或授权已确认。
7. vendor/gms缺少git-lfs导致自定义变化未导出，仍是已知备份边界；所有项目精确revision保持。签名密钥独立保存。
8. 当前Ubuntu活动源树/mnt/android-download/git-cache/evolution，输出/mnt/android-build/Android/out/evolution-bka；完整构建前至少100GiB可用空间并保留KSU/AVB/Permissive基线。本次仓库整理没有编译或访问手机。

GPL二进制向他人分发前需另外准备完整对应源码/配置/构建脚本及可用获取方式；本仓库尚不是完整对应源码交付。
