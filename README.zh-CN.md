# 小米 13 Ultra（ishtar）Evolution X 12.3 / Android 17 源码归档

[English](README.md) | **简体中文**

这是小米 13 Ultra / ishtar 的 Android 17 维护源码归档，保存设备树、Xiaomi
硬件源码、完整 DTS、固定平台清单、内核增量补丁和最终平台补丁。它不是完整
ROM，也不是可直接独立编译的整套 AOSP。

**状态：** 私有归档，快照日期 2026-11-09。Build 305 已按记录完成安装和
限定范围验收，但尚未发布，仍为 testkey 签名。

## 体量与用途

- 原完整检出约 **110.4 GiB**，其中 `.repo` 约 **36.3 GiB**。
- 本归档约 **45 MiB**，适合放 GitHub。
- 完整上游源码由固定 manifest 重新同步，不需要把整套源码塞进仓库。
- Recovery、KernelSU `init_boot`、vendor 专有文件、GMS、固件、target-files
  和签名材料不入库，后续统一放 SourceForge；目前没有虚构下载地址。

## 分支规则

| 分支 | 用途 |
| --- | --- |
| `main` | Android 17 主基线。以后每次 Android 17 升级都在这条历史上继续提交。 |
| `android-16` | 原 Android 16 归档，固定在 `cefdce3ddcc13cb04ab4b0183933c72f6dddcea2`，不再改写。 |

Android 17 不另打标签；仓库原有历史标签保留不动。

## 仓库内容

```text
device/xiaomi/           ishtar 与 SM8550 公共设备树
hardware/xiaomi/         Xiaomi 硬件支持源码
kernel/xiaomi/           ishtar DTS/DTBO
kernel/patches/          基于 Linux 5.15.224 的内核增量与模块增量
manifests/               平台及系统固定到完整 40 位 SHA 的清单
patches/platform/        最终累计平台补丁
vendor/xiaomi/           仅构建配方，不含 proprietary 载荷
provenance/              305 的机器可读证据与逐项修复历史
releases/                每个版本的元数据
scripts/                 只读预检、安全应用与归档审计脚本
docs/                    构建、架构、内核、验收和维护说明
```

## 正确理解当前系统

- 系统是 **Evolution X 12.3 / Android 17**，不是纯 AOSP。
- 产品为 `lineage_ishtar`，继承
  `vendor/lineage/config/common_full_phone.mk`，因此包含
  Evolution X/LineageOS 产品配置。
- 在 Evolution X/LineageOS 之上，确实有为 ishtar 做的适配：设备树、音频、
  显示、相机、Recovery、SELinux、SurfaceFlinger、热控、telephony、netd、
  update_engine、GameSpace、SystemUI/Launcher 和振动 HAL 等均有项目特定改动。
- 内核可以称为“**基于 Linux 5.15.224 的 ishtar 内核**”，但不能称为未经修改的
  上游 5.15.224；还有 ACK/vendor/ishtar 适配，未做官方 GKI ABI 认证。
- 211→224 的约 2,661 处上游改动已经合并，不需要逐条上传。仓库用一个经过审查
  的累计补丁记录完整结果。

## Build 305

| 字段 | 值 |
| --- | --- |
| ROM | `EvolutionX-17.0-20261107-ishtar-12.3-Unofficial` |
| Incremental | `1794048426` |
| 内核 | 基于 Linux `5.15.224` |
| ROM SHA256 | `205107f2548f7f7c239130cd89dec6fd7987ed28f7d9487919152e319ec2ee7e` |
| target-files SHA256 | `dc68886525608feb11ac46893f892217ed2ea547ba016e15f0437f9afd7e4f7e` |

305 的静态审计、保数据 sideload、34 个实际分区核对、选定回归、WiFi PNO、
OIS 协议、合成缓存、有限 UI 矩阵和 180 秒观察均有记录。真实通话/VoLTE/WFC、
拔线长时待机、暗室 ALS、物理防抖画质、真实碰撞轮转、NFC 实体交易、蓝牙附件、
触感和完整图形压力测试仍未物理验收。

## 使用方法

先按 [Build](docs/BUILD.md) 同步固定源码，然后在归档外执行：

```bash
"$ISHTAR_ARCHIVE/scripts/apply.sh" /path/to/android17
"$ISHTAR_ARCHIVE/scripts/apply.sh" --apply /path/to/android17
```

第一条命令只预检，不改源码；有任何补丁冲突就停止。第二条才复制维护树并应用
平台补丁。内核对齐与模块补丁需要按 [Kernel](docs/KERNEL.md) 单独处理。

## 文档

- [构建](docs/BUILD.md)
- [目录结构](docs/SOURCE_LAYOUT.md)
- [架构与 EvoX/ishtar 适配](docs/ARCHITECTURE.md)
- [平台补丁](docs/PLATFORM_PATCHES.md)
- [内核说明](docs/KERNEL.md)
- [外置输入](docs/EXTERNAL_INPUTS.md)
- [验收记录](docs/VERIFICATION.md)
- [后续维护](docs/MAINTENANCE.md)
- [许可证与分发](docs/LICENSE_AND_DISTRIBUTION.md)

## 许可证

本仓库是多个独立授权项目的组合，不对全部文件套用单一许可证。每个源码组件
保留原版权、SPDX 标识和许可证。详见 [LICENSE.md](LICENSE.md) 和
[NOTICE](NOTICE)。
