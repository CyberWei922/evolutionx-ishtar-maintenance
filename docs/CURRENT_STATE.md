# 当前维护状态

## 最新集中候选（构建中，未安装）

机主要求一次多修后统一打包。本轮10路径/68源码已合入并通过策略组件编译及verified01；策略1eaa4c3e…/1,604,627 bytes，有效权限新增32项/0删除。范围为MIUI导航键/PSI只读、开机QSPM、蓝牙true移system、QCC启动属性、ROM版本只读、热控两cap和精确状态。原9静态宽松域、文件MAC/genfs/ioctl、相机/音频/显示参数及保护二进制保持；热控cap不能按路径细分。

活动输入是plans/selinux-enforcing-prep-20261018/working-source-files.json。统一整包enforcing-prep-rom-build-01及电脑审计等待器已派发；实际build.prop和34分区检查仍待成功后执行。中间camera-qspm只做组件、不单独打ROM。以下62/59项工作树入口是各历史基线。

手机仍为已验收1792266978/Permissive，无本轮手机操作或Enforcing；旧刷写授权已消耗。目标再约2–3版进入稳定Enforcing是计划估计，不是保证。原宽松域/KSU及真实功能、受控Enforcing、冷启动须分别报告。按直接受影响风险安排手工验收，避免重复麦克风/指纹。

2026-10-18，Xiaomi 13 Ultra / ishtar，Evolution X Android 16 / SDK36，当前已安装版本1792266978，Unofficial。上一验收源码基线为1792226532。

## 当前安装与验收入口（2026-10-18）

1792266978已保留数据刷入并正常启动，44文件/21分区范围匹配，新Mtd精确属性、GNSS/Goodix实际加载权限及SPI wakeup标签通过。机主确认所问Google账号/原应用照片、MIUI拍照新录像收音、指纹正常。仍全局Permissive，原9静态宽松域+ksu保持，没有Enforcing试验。当前62项源码入口为plans/selinux-device-io-20261018/working-source-files.json，安装/验收摘要见同plan的installed-acceptance-summary.json。

本轮所有有限worker已结束。完整ROM只保留1792266978和1792261801，旧1792257806 ZIP、自己的已完成刷写暂存及重复输出已删；SSD/HDD空闲154.52/243.79GiB，保留源码/闭源依赖/增量缓存。以下旧阶段状态保留历史，当前入口以上述本节为准。机主要求只测直接受影响项，避免无依据重复麦克风/全套硬件验收。

## 已完成并保留

最新工作增量见 `plans/selinux-dependency-status-20261018/`，活动59项清单以该plan为准。五路径公共库传递依赖、MIUI公开sensor属性只读及Mtd精确状态/标签数据库修复已合入，策略544dd102…；完整1792261801经过实际34分区和静态升级检查，并按机主新授权保留数据刷入、正常启动，39文件/21分区范围及新库/5项Mtd属性标签读回通过。六默认相机解析、DSP进程/注册、FBE/空模块/快照none和数据基线保持，112次命令、0重试。机主已确认所问数据、MIUI拍照/新录像收音和指纹无问题。仍Permissive，原9静态宽松域+KSU保持，实际DSP回退和Enforcing未验证；Mtd DAC及新增attestation.status精确属性写拒绝待下一批。

- MIUI camera03原生集成；机主确认日常拍照/录像正常。Aperture录像无声为旧UID录音ignore，已恢复foreground并获新录像有声确认。
- Recovery触控修复：补载配套PM8008供电驱动，使用ROM配套触控固件。验收镜像SHA c39e0236…；当前固定配套内核，换内核需重新核对。
- 自动亮度渐变：fast升/降0.301001098767，slow升/降0.240813087535，机主体感验收通过；入口是厂商两份displayconfig XML中的screenBrightnessRamp*。
- 开机低刷：Scheduler在SurfaceFlinger启动完成后重新计算/提交当前模式，保护启动状态同步；保留原30–120Hz策略。新版已保留数据完整刷入并启动，机主确认首次桌面第一段滑动流畅。
- SELinux两批10个设备策略文件已编译、整包静态核验、前版实机标签和3个新服务域验证。当前版本保留这些修复。

## 验证限制与待办

- SELinux仍全局Permissive，TEE/属性/persist/QRTR等余项及原宽松域仍需处理；未进行Enforcing功能/冷启动验收。
- 开机修复：新4项、双模式切换24项测试通过；选定57项中51通过/6旧用例失败。机主仅明确首次滑动通过，随后要求结束采集；新完整trace分析、物理频率完整同步打点与21分区回读未完成。
- MIUI默认六类入口及机主User0一次性偏好迁移已在下一版源码实现，策略/辅助APK/出厂XML及1792252694整包编译、实际OTA静态核验、保留数据刷入和六默认解析/新域读回通过，机主确认所问相机入口和拍摄收音正常；外部结果回传仍未单独验收，Aperture备用。
- 快充留待后续。不得把当前日常体验正常扩写为所有功能在Enforcing下正常。
- 本轮32文件/21范围读回及机主验收后，按既有要求只保留1792252694与1792246397两个完整ROM；旧1792226532 ZIP、自己的本轮刷写暂存和重复输出已清理。元数据/旧验收、源码/闭源依赖/增量缓存保持，当前实际解包镜像为后续库分析保留。SSD154.52GiB/HDD278.75GiB空闲；新旧已删flash暂存路径均不可直接重跑。

## 下一轮SELinux源码已合入、策略编译通过

机主授权继续修复后，第三至五批累计8个路径已实际合入Ubuntu活动源码，见plans/selinux-next-20261017/。三批policy编译、精确权限/上下文/域转换及50项工作源码核对通过，最终策略SHA b69311c6…；整包1792246397已编译09:50/exit0并完成34分区实际重建与静态核验。ROM SHA 9c1f4c02…。

机主另行确认保留数据刷入并重启后，74动作/73完整下载成功，正常启动；29文件/21分区范围读回匹配，QRTR专用数据标签和2精确属性类型正确，3厂商服务域保持。FBE、目录inode/属主/权限、应用数9、空模块和快照none保持。实际加载策略含原9静态宽松域+ksu共10，仍Permissive。机主反馈正常启动；未逐项确认本版相机拍照/录像/指纹。

原accepted清单和source快照描述上一验收基线1792226532，标签保持。本轮累计补丁及运行进度单独保存，不能拿accepted清单校验已改变的Ubuntu工作源码。Mtd persist身份/DAC、另外两个状态属性provision.status/feature_state、属性消费者、相机和其他功能拒绝、原宽松域仍待处理，不宣称可安全Enforcing。新AVC缓冲仍有audit_lost773，不当作全量。

## 1792252694源码与安装（下一批见末节）

`plans/selinux-camera-next-20261017/`的14路径增量已实际合入，工作清单59项。Mtd另外两个状态属性改为精确restricted类型；MIUI app专用域以及后处理/P3/音频只读权限编译通过，原9静态宽松域不变。策略SHA76314da3…；实际辅助APK9af56163…平台签名通过；出厂XML42df062e…实际构建匹配。主机11迁移行为+3路由契约/6动作和独立补丁应用测试通过。新完整ROM1792252694已编译22:05/exit0，SHA999dd464…、4,545,399,102 bytes；34实际payload分区和全部保护载荷/新相机文件检查通过。不清数据静态升级检查通过。本包已保留数据刷入并正常启动，32文件/21分区范围和新域/6默认解析通过，机主确认所问功能正常；仍Permissive。

下一步仍需Mtd DAC/属性消费者、MIUI ODM库/MiSys、HAL属性枚举与原宽松域处理；静态检查不等于Enforcing可用。新工作清单不能与本节前50项1792246397或更旧accepted45项混用。Ubuntu其他任务持续运行；构建先限自身到两个CPU、低优先级，打包后段只将本任务及子进程放宽到四个经测空闲CPU，没有停止/调整其他任务。

## 1792252694本轮验收与下一批

新完整ROM已保留数据刷入并启动，32文件/21分区范围和数据基线匹配，六类相机解析到MIUI，实际新域进程正确。机主回复“正常 都是MIUI相机 声音指纹都正常”，仅记录所问数据/默认入口/拍摄收音/指纹正常。当前Permissive，9静态宽松域+ksu保持，没有Enforcing试验。

活动源码已继续单文件补app_api_service/radio_service find，见plans/selinux-framework-api-20261018/及其新59项清单；该增量未打包/安装。计时器25秒mark-only脱离ADB存活和到期演练通过，未验证Enforcing下实际还原。后续先补必要功能阻点，再安排单独获授权的短时Enforcing及冷启动验证。

## 1792257806累计包

完整ROM构建exit0、实际34分区/59源码及DSP产品契约检查通过。ROM4,545,427,001 bytes，SHA71d4e05f…；Mac40文件薄交接不含ROM。机主授权保留数据刷入和首次正常重启，不含Enforcing试验；现场1792252694/A槽/FBE/空模块/快照none和40固件范围通过，新74动作runner不写固件或数据分区。此时仅进入新flash01执行，运行结果需另记，不复用旧runner。

## 1792257806运行结果

本次74动作/73完整下载及wire核验通过，已正常重启；38文件/21范围SHA、数据基线、六默认相机解析、MIUI专用域及DSP进程/HIDL注册通过。加载策略cacf2720…，原9静态宽松域+ksu保持、全局Permissive。机主测试尚未答复；实际DSP HAL回退与Enforcing未验证。有限worker已结束，新库依赖libc++_shared和属性/MiSys、Mtd及HAL枚举等继续待办。新ROM及前两已安装ROM暂保留，等待新验收后按双版本规则清理。

## 1792257806机主反馈与Mac无线相机复测

机主其他请求项报告正常，新增按钮/切镜/模式卡顿和5倍间歇变暗。随后明亮固定场景Mac无线复测反馈没有闪烁、仍略卡；四次进5倍内部master交接435–549ms，5倍稳态约16–17fps，1/3.2倍约25fps。应用事件处理日志1–3ms不等于完整触摸延迟；现有数据支持相机流畅度待优化，未证明由本批SELinux引入，间歇变暗未证实永久修好。两个有界logger均结束，未改源码/偏好/相机参数或重启/切Enforcing，旧回退包暂保留。

## 公共库依赖与Mtd精确状态（最新工作入口）

机主最新表示相机又无明显问题，先保持现状、性能后续优化，继续SELinux。plans/selinux-dependency-status-20261018五路径增量已应用、组件02:24/exit0及verified01首轮通过：22路径、59源码、属性精确匹配、权限和xperm边界及原9宽松域保持。组件116项本地交付校验通过，未上传策略二进制。CPU6/7/nice10/I/O7/Ninja2的整包任务dependency-status-rom-build-01已28:01/exit0，audit-worker/audit-status均complete，34实际分区、源码、策略/新库inode标签、原有修复及静态保留数据升级检查通过。候选1792261801，4,545,440,201 bytes/SHA2bff039e…；正式ROM在Ubuntu同名delivery，Mac为41项薄交接。完整OTA签名未独立密码学核验；现场手机预检查和运行验收待新授权后执行。所有本轮worker结束，未调整其他任务，本轮无新手机采集、刷写、重启或Enforcing。


## 1792261801运行结果（最新）

机主回复“直接开刷吧”，新flash01完成74动作/73完整下载，wire SHA5078adfb…核验通过；03:25:30正常重启，postboot01 success，39文件/21分区范围全匹配。加载策略6b156e09…/2,151,534 bytes，原9静态宽松域+ksu共10保持。初始快照未见MIUI相机运行，不能据此声称本轮实际相机域/算法回退验收通过；机主随后确认所问数据、MIUI拍照/有声新录像及指纹无问题，独立owner-acceptance.json保存范围。启动logcat317 AVC/18域、audit_lost1，非全量/完成百分比；新RKP类型本窗口未见拒绝，不是全部消费者验收。Mtd仍有DAC及persist.vendor.sys.attestation.status对vendor_system_prop的set拒绝，后续精确核查。

本次刷入及首次正常重启授权已使用，没有额外重启或Enforcing。有限worker全部结束，无手机常驻采集/模块。Mac/Ubuntu22项运行证据和归档摘要匹配，Git仅存筛选摘要，不含ROM/原始日志/策略二进制。原回退包与新旧暂存待机主验收后按保留两版要求处理，本轮尚未清理。

## 1792261801机主验收后的保留状态

机主确认所问功能正常并授权制作下一版。完整ROM现仅保留1792261801及1792257806；1792252694/1792246397 ZIP和两已完成刷机暂存已删除，旧runner不可重放。SSD释放49.48GiB/HDD8.47GiB，空闲154.52/255.43GiB；保留ZIP为元数据及先前实际读回证据核对，没有新一轮全ROM哈希。原始证据/源码/闭源依赖/增量缓存保留。

下一批六路径已合入62项活动源码清单control/ishtar-selinux-device-io-20261018-01/expected-source-files.json，策略组件正在编译。未刷入/重启/切Enforcing，Ubuntu其他任务未调整。旧59项清单仅用于已安装1792261801基线。

## 设备IO与attestation状态（当前源码入口）

plans/selinux-device-io-20261018/working-source-files.json共62项，覆盖旧59项活动入口。六路径修改Mtd精确attestation.status restricted属性、GNSS只读现有sensors、Goodix私有netlink四操作、指定SPI wakeup标签。策略组件1e158ee4…/1,604,531 bytes已编译核验；62源码/22路径、精确属性契约、2条已有类型allow增量、单个genfs新增、0删除、xperm/原9宽松域保持通过。Mac/Ubuntu组件116项通过。完整ROM device-io-rom-build-01正在CPU6/7低优先级构建，自动审计仅电脑。手机仍机主已测1792261801/Permissive，未操作手机。

Mtd检查失败仍注册服务，独立attestation线程stat失败则skip该可选重载；目录身份/实际功能仍待核查，不为消日志扩persist权限或DAC绕过。相机性能参数按机主要求保持，Enforcing未验证，下一候选需另确认刷入重启。

## 设备IO候选1792266978已完成（最新）

整包构建exit0，新rom-audit02的audit-status及candidate-finalization-status complete，实际34分区/62源码、策略1e158ee4…和原保护载荷/设备IO库检查通过。ROM 4,545,439,425 bytes/SHA5021325057439cbbca4a35f89cb32c24575ebc5ebeefe2da3994f96e3c28b19b，正式包在Ubuntu delivery/ishtar-selinux-device-io-permissive-candidate-20261018-01，Mac同名薄交接不含ROM。静态保留数据升级检查通过，OTA完整签名未独立密码学核验。源码见plans/selinux-device-io-20261018。

手机仍1792261801/Permissive、机主已测所问功能正常；新候选未安装/重启/Enforcing。Mtd DAC/可选认证重载、消费者、其他必要功能拒绝及原9静态宽松域/KSU、短时和冷启动Enforcing继续单列。构建/审计worker全部结束，未调整其他任务。新候选刷入/重启另确认。
