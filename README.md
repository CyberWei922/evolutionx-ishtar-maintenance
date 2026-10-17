# Evolution X · Xiaomi 13 Ultra (ishtar) maintenance

小米13 Ultra / ishtar，Evolution X Android 16个人维护源码。**这是混合许可证的维护增量仓库，不是完整Android源码或ROM编译结果。**

## 当前版本

当前已安装Unofficial incremental **1792252694**，SELinux仍为**Permissive**。32文件、21分区范围及运行标签/策略核对通过；六类相机动作解析到MIUI，实际专用域运行正确。机主确认所问数据/默认入口/拍摄收音/指纹正常。Enforcing及快充待修。

下一版的MIUI默认六入口/保留数据偏好迁移及下一批SELinux源码已实现，并完成策略、签名辅助程序和出厂规则XML的组件编译核验，见`plans/selinux-camera-next-20261017/`。新完整ROM **1792252694** 已编译22:05/exit0并通过实际34分区及新默认文件/策略静态检查（SHA999dd464…，约4.55GB），已保留数据刷入并获上述限定范围机主验收；Aperture保留备用。见该计划的`full-rom-status.json`。

上一验收版本 **1792226532** 的camera03、Recovery触控、亮度平滑和开机刷新率修复完整保留。第三至五批累计源码见`plans/selinux-next-20261017/`；原accepted源码清单及验收标签继续指1792226532，当前源码由原基线叠加累计候选补丁复现。

下一批单文件基础API权限与回退计时器源码见`plans/selinux-framework-api-20261018/`，其新工作清单59项已与1792252694安装基线分开；新权限尚未进入另一整包或手机。

## 代码与依赖

- `source-files/`：23份明确许可的修改后完整源文件，可直接阅读；保留当前构建字节和原版权。
- `patches/`：12个项目的本地修改，对应固定上游HEAD。
- `source-overrides/`：22份选定的新代码、集成配置及有明确再分发许可的文本。
- `manifests/`：1187个上游项目的固定版本，不重复上传整个Android平台。
- `licenses/`、`NOTICE`、`LICENSE.md`：完整许可文本、原始声明、来源映射及修改记录。
- `component-recipes/`：有明确许可的Recovery rc、原创亮度参数生成器和相机集成说明。
- `artifacts/`、`provenance/`：二进制/外部文本的路径、尺寸、SHA和构建基线，不含其载荷。
- `docs/`、`releases/`：版本、验收范围、依赖和后续修复方案。

完整Android基线、原厂依赖、MIUI APK、Recovery镜像、Google软件、ROM及签名材料单独保存；**克隆本仓库不能直接编译完整ROM，也不代表全部ROM组件已获再分发授权。** 当前ROM SHA256为`999dd46495f1d2a99b748ee757f1a9609892e53d2d25598688b385984d82075b`，前一安装基线1792246397为`9c1f4c0212aa7fd6613ad8e7a7123c94391b7e1a6c20a4a050d5d19532d84440`。

## 本次许可整理

302份原厂/生成文本与5份授权未确认的相机实现配方从上传内容排除，其中4份Qualcomm文本带明确专有标记。它们保留于机主本地原始成果，不删除Ubuntu构建所需文件。原厂亮度XML改为外部输入，原创SHA门禁生成器仍可复现已验收参数。

旧历史先做本地备份，再重建干净主分支/版本标签。不要把此仓库许可证套用到外部依赖。详见[许可与分发说明](docs/LICENSE_AND_DISTRIBUTION.md)。

## 使用入口

先读[当前状态](docs/CURRENT_STATE.md)、[目录](docs/DIRECTORY_LAYOUT.md)、[构建依赖](docs/BUILD_AND_DEPENDENCIES.md)。运行`python3 scripts/verify_bundle.py`仅核对归档；导出工具只接受已审查路径，不再自动上传任意文本。vendor/gms因缺少git-lfs未归档自定义差异的历史限制保持。
