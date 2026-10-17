# SELinux 第三至五批累计候选

2026-10-17。已合入Ubuntu活动源码，三批独立策略编译和静态核验通过；整包1792246397已完成34分区静态核验、保留数据刷入并正常启动。29文件、21分区范围和实际标签/加载策略核对通过；仍全局Permissive，机主本版相机/录像/指纹单项确认待补。

## 改动

- Fidoca/Mtd：精确TEE INVOKE ioctl；Mtd精确QCE UNMAP；Fidoca已有FIDO属性权限。
- QRTR：专用数据类型，精确`/data/vendor/qrtr`子树权限；common init在mkdir后restorecon，覆盖保留数据升级。
- Mtd：TEE listener只读，persist父目录getattr/search；两个状态属性独立vendor restricted类型和exact string上下文。

`candidate.patch`相对于已安装1792226532对应的源码，8个路径，原/新摘要见`source-baselines.json`。补丁在独立临时树实际应用后8个文件字节全部匹配；不要在已经合入这些批次的Ubuntu工作树上重复应用。原`source-overrides/`、`source-files/`、`patches/`和`accepted-source-files.json`继续保留安装基线。

提供7份许可明确的完整候选源码与旁置许可说明；common init保留LineageOS版权和Apache-2.0 SPDX。完整file_contexts未重新归类为可上传文本，只保留本轮原创新增块的最小补丁及摘要。没有上传闭源程序、ROM、策略二进制、密钥或手机日志。

## 编译与验证

| 批次 | policy目标 | 策略bytes | SHA256 |
| --- | --- | --- | --- |
| 3 | 02:14，exit0 | 1600481 | e6095043e753966e8b18bc2607178c628a16986f68d32a3da071476d9b2ccde4 |
| 4 | 04:19，exit0 | 1600789 | 4e3bf5111bdc076c2ecc0522be45e8bad82021e50cebc52cebb54145a5ed0eb4 |
| 5 | 04:19，exit0 | 1601057 | b69311c6d9dcef93be9b25947af9b6e292bb2eaca552ed4acc734b5b844f2a69 |

neverallow及策略测试保持。核验的是生成二进制精确权限、实际生成上下文、域转换和50项工作源码摘要。原9个静态permissive域保持；KSU动态例外仍需运行态另查。编译耗时为构建工具输出，和包含启动脚本的墙钟耗时略有差异。

## 尚待解决

Mtd persist凭据/DAC、状态属性消费者、相机/其他服务AVC必要性、原宽松域及KSU例外仍待处理。Mtd闭源程序自身检查0700 system/system，未通过扩大目录权限或允许dac_read_search解决。当前候选保持Permissive，不是Enforcing成果。

新刷入/重启需要机主确认和独立计划。运行回归与Enforcing冷启动验收完成前，不更新已安装验收基线或其标签。快充及MIUI默认相机继续作为独立后续事项。

本轮已取得单独刷入/重启确认并完成执行；该授权不作为后续另一构建或额外重启的授权。旧静态policy摘要内的phone_installed=false描述对应编译时阶段，当前运行进度以source-baselines.json和releases/ishtar-1792246397-candidate.json为准。
