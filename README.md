# Evolution X · Xiaomi 13 Ultra (ishtar) maintenance

最新安装 **1792347689 / Permissive**：94项输入、10路径Backuptool原生挂载代理及旧升级入口兼容修复。已保留数据刷入并正常启动，57文件/21分区范围及实际策略/标签读回通过；机主桌面/应用照片验收待回复。未做真实OTA或Enforcing测试。见[代理修复与安装结果](plans/selinux-backuptool-broker-20261019/README.md)。

小米13 Ultra / ishtar，Evolution X Android 16个人维护源码。**这是混合许可证的维护增量仓库，不是完整Android源码或ROM编译结果。**

## 当前版本

当前已安装Unofficial incremental **1792347689**，全局仍为 **Permissive**。新代理已入实际system并读回执行标签，静态backuptool及动态ksu例外保持。剩余inactive路径隔离、持久标签/security xattr、精确权限、V3/EROFS及optional OTA处理；真实OTA/Enforcing运行和冷启动均未验收。

camera03、MIUI默认入口、Recovery触控、自动亮度平滑、开机刷新率修复以及后续已安装SELinux增量完整保留。原accepted源码清单和验收标签仍描述1792226532；当前工作树由它叠加各累计计划复现。详见`docs/CURRENT_STATE.md`及各plan实际状态，不把旧未安装快照当最新结果。

## 代码与依赖

- `source-files/`：23份明确许可的修改后完整源文件，可直接阅读；保留当前构建字节和原版权。
- `patches/`：12个项目的本地修改，对应固定上游HEAD。
- `source-overrides/`：22份选定的新代码、集成配置及有明确再分发许可的文本。
- `manifests/`：1187个上游项目的固定版本，不重复上传整个Android平台。
- `licenses/`、`NOTICE`、`LICENSE.md`：完整许可文本、原始声明、来源映射及修改记录。
- `component-recipes/`：有明确许可的Recovery rc、原创亮度参数生成器和相机集成说明。
- `artifacts/`、`provenance/`：二进制/外部文本的路径、尺寸、SHA和构建基线，不含其载荷。
- `docs/`、`releases/`：版本、验收范围、依赖和后续修复方案。

完整Android基线、原厂依赖、MIUI APK、Recovery镜像、Google软件、ROM及签名材料单独保存；**克隆本仓库不能直接编译完整ROM，也不代表全部ROM组件已获再分发授权。** 当前1792347689 ROM SHA256为`32df7de2c2648c2ed01b96bb2ffc42fa93f9e79ff3404b4db87ff3967590ff0a`，正式ROM保存在Ubuntu；上一1792339544及1792333073回退ROM保留。Mac仅薄交接和私有运行证据，Git仅存许可明确的维护源码与摘要。

## 本次许可整理

302份原厂/生成文本与5份授权未确认的相机实现配方从上传内容排除，其中4份Qualcomm文本带明确专有标记。它们保留于机主本地原始成果，不删除Ubuntu构建所需文件。原厂亮度XML改为外部输入，原创SHA门禁生成器仍可复现已验收参数。

旧历史先做本地备份，再重建干净主分支/版本标签。不要把此仓库许可证套用到外部依赖。详见[许可与分发说明](docs/LICENSE_AND_DISTRIBUTION.md)。

## 使用入口

先读[当前状态](docs/CURRENT_STATE.md)、[目录](docs/DIRECTORY_LAYOUT.md)、[构建依赖](docs/BUILD_AND_DEPENDENCIES.md)。运行`python3 scripts/verify_bundle.py`仅核对归档；导出工具只接受已审查路径，不再自动上传任意文本。vendor/gms因缺少git-lfs未归档自定义差异的历史限制保持。
