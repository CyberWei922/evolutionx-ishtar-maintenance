# Evolution X · Xiaomi 13 Ultra (ishtar) maintenance

最新工作：[`plans/selinux-enforcing-prep-20261018/`](plans/selinux-enforcing-prep-20261018/) 为10路径集中修复、68项输入；组件已核验，完整ROM构建中，未安装或进行Enforcing。手机仍为已验收1792266978。

小米13 Ultra / ishtar，Evolution X Android 16个人维护源码。**这是混合许可证的维护增量仓库，不是完整Android源码或ROM编译结果。**

## 当前版本

当前已安装Unofficial incremental **1792266978**，SELinux仍为 **Permissive**。44文件、21分区范围、新标签与实际加载策略核对通过；机主确认所问Google账号/原数据、MIUI拍照和新录像收音、指纹正常。原9静态宽松域及KSU例外保持，尚无Enforcing试验。完整回退ROM保留1792266978与1792261801。

本轮集中修复组件已核验、完整ROM构建中，见`plans/selinux-enforcing-prep-20261018/`。10路径/68输入覆盖相机只读、QSPM、蓝牙属性归属、QCC、版本元数据和热控，尚未刷入或Enforcing。相机流畅度优化按机主要求暂缓。

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

完整Android基线、原厂依赖、MIUI APK、Recovery镜像、Google软件、ROM及签名材料单独保存；**克隆本仓库不能直接编译完整ROM，也不代表全部ROM组件已获再分发授权。** 当前1792266978 ROM SHA256为`5021325057439cbbca4a35f89cb32c24575ebc5ebeefe2da3994f96e3c28b19b`，上一版1792261801为`2bff039e93f5bec2ddc1e6c255d3300e4c7f65afa48354769cfb3456748b5ca0`。

## 本次许可整理

302份原厂/生成文本与5份授权未确认的相机实现配方从上传内容排除，其中4份Qualcomm文本带明确专有标记。它们保留于机主本地原始成果，不删除Ubuntu构建所需文件。原厂亮度XML改为外部输入，原创SHA门禁生成器仍可复现已验收参数。

旧历史先做本地备份，再重建干净主分支/版本标签。不要把此仓库许可证套用到外部依赖。详见[许可与分发说明](docs/LICENSE_AND_DISTRIBUTION.md)。

## 使用入口

先读[当前状态](docs/CURRENT_STATE.md)、[目录](docs/DIRECTORY_LAYOUT.md)、[构建依赖](docs/BUILD_AND_DEPENDENCIES.md)。运行`python3 scripts/verify_bundle.py`仅核对归档；导出工具只接受已审查路径，不再自动上传任意文本。vendor/gms因缺少git-lfs未归档自定义差异的历史限制保持。
