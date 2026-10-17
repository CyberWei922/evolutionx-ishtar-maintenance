# 许可范围与发布边界

## 本次修正

2026-10-17，机主授权按许可核查结果重新整理并上传。先前单提交/标签有4个带 Qualcomm Confidential and Proprietary 标记的文本配置，以及其他授权未确认的原厂文本。本次备份保留在机主本地，不把旧提交作为新主分支的祖先，也不上传备份。

当前保留12个原项目补丁、23份明确许可的修改后完整源文件、22份选定文本override及固定manifest。302份原厂/生成文本和5份相机实现配方改为外部依赖元数据；其中4份具有明确专有标记。没有删除或更改Ubuntu活动源码、原厂依赖、冻结delivery和ROM成果。

## 许可分类

- Apache源码：保留原版权、许可证、适用NOTICE和修改说明。完整修改后源文件保持当前构建字节，旁置.license及NOTICE记录修改；补丁显著注明修改及原始基线。
- GPL内核/驱动/KernelSU内核目录：保留GPLv2声明与源码差异。当前仓库是维护增量，不是完整对应源码交付；向他人发布二进制前，须另外提供实际对应源码、配置和构建脚本，不能只给一个收件人无权访问的私有仓库。
- 两份颜色XML和Recovery rc：保留文件内完整BSD类许可及免责声明；没有将其重新许可为Apache。
- MIUI、Google、厂商闭源库/固件、无明确授权的原厂文本：这里只保存路径/版本/尺寸/哈希/必要参数及原创集成代码，不包含载荷，也没有声称拥有其再分发权。即使免费或仅支持小米手机，也不自动获得授权。
- 其他预装应用、字体、资源和完整ROM须在发布前按对应版本继续核查。Evolution X团队的预装/维护者批准不替代第三方版权授权。

## 上游与标准来源

- Device tree: https://github.com/ishtar-aosp/android_device_xiaomi_ishtar/tree/8605aa5b3749f70b062fae1932db5c5e531a21b1
- Common tree: https://github.com/ishtar-aosp/android_device_xiaomi_sm8550-common/tree/a27e05da9dea27a593deb11ba49e17dab39018c5
- Apache 2.0: https://www.apache.org/licenses/LICENSE-2.0
- Linux licensing: https://www.kernel.org/doc/html/latest/process/license-rules.html
- AOSP licenses: https://source.android.com/docs/setup/contribute/licenses
- Xiaomi EULA: https://terms.miui.com/doc/eula/cn.html
- Google GMS: https://www.android.com/gms/

上述网页是本次核查依据，不代表已取得小米或Google授权。上游精确项目名/版本见manifest及licenses/component-map.json；各文件原声明优先。

## Git历史范围

本次将以带租约保护的方式，仅替换原来由本项目创建的main和同名验收标签，清除这些正常引用指向旧提交的路径。旧提交SHA仅作为事实记载，不携带其对象。GitHub服务端缓存、直接SHA访问、已有克隆或平台保留对象的彻底清除不能由一次push保证；因此保持私有，不宣称服务端所有副本已删除。
