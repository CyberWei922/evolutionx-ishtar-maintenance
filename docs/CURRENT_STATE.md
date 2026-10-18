# 当前维护状态

## 1792339544已安装、桌面和数据验收通过（当前状态）

本轮4路径OTA执行/错误传递/事务复制修复已完整编译并保留数据刷入。74动作/73完整下载及wire SHA ce68887bd0dd8249e4487ef1fb1cd9d88b127bb760600acc0f1d59c4d061e11c通过，2026-10-19 00:51:01首次正常重启。56文件/21分区范围、7组保留策略契约、六MIUI默认入口及三项数据基线均匹配；153次读回命令尝试、0重试。机主本次确认“桌面正常，应用和照片都在”，只记录桌面/应用照片保留验收，不补记相机、麦克风、指纹或全硬件验收。

实际1792339544/A槽/FBE，仍为全局Permissive/enforce0。磁盘策略694b6134dfd89758742f964df8d6f288203df5c766c75f2225b30af0fbaaed53与上一版相同；加载49db1c52b7d70b4060dabefea3b523bc6cc201b4a13433e965bb2b5e0711dbec/2,152,173 bytes。静态宽松域仅backuptool，加载另含动态ksu；内核/KSU保持。四路径改动改善OTA脚本调用/失败处理，不代表已运行真实OTA或backuptool已可Enforcing。挂载代理、inactive隔离/精确权限、持久标签/security xattr、V3/EROFS及optional处理仍待完成，再做必要Enforcing运行和冷启动验证。

有限启动采集已结束：uptime18.25–192.69秒，31样本，核心PID未观测到变化，采集传输失败0。合并缓冲快照与有限流保留709条唯一AVC行、audit_lost最大2；有速率/缓冲/窗口限制，不是全量开机拒绝、709个功能故障或修复进度百分比。初始快照无MIUI进程，不冒称初始域验证。读取监控666的host loop/caller超时保留，未影响独立刷写或触发分区重试。

私有运行归档30项/1,521,192 bytes/SHA536a8f9ba9c5e4fc510f3344bccbe377792fee924ac44541b886523f0d9769ba双端一致；归档早于本次机主答复，验收另存补充，不改冻结归档。正式完整ROM在Ubuntu delivery/ishtar-selinux-ota-contracts-permissive-candidate-20261019-01，4,545,425,494 bytes/SHA d0cd1eae3de97d54052c8262c5fea80b662eb32f7bc38de6efef1e741de9a2ad；Mac仅薄交接/运行证据，Git仅许可明确源码和脱敏摘要。完整OTA签名未独立密码学核验。活动源码为ota-contracts control/expected-source-files.json共87项，已重新核对；所有本轮worker退出，构建锁空闲。

按已有清理授权，在双端证据与本次数据验收后删除本轮完成的SSD刷写暂存及已核对同SHA的out重复ZIP，释放28,837,494,784 bytes（约26.86GiB），SSD空闲约108.88GiB。正式本版/上版1792333073保留，其他HDD旧档本轮未清；保留正式ZIP未重复全量哈希。源码/.repo/厂商依赖/当前增量缓存和其他Ubuntu任务未动，已删暂存禁止重放。

本次刷入及首次正常重启授权已用完，未做Enforcing、冷启动Enforcing或真实OTA；不复用为额外重启或切模式。有限采集结束，手机无需继续连接USB。下方各阶段记录为历史。


## OTA 错误与直接入口组件已完成，尚未安装（最新活动源码）

机主要求继续修复。Ubuntu活动入口改为 control/ishtar-selinux-ota-contracts-20261018-01/expected-source-files.json，共87项；已安装1792333073仍对应旧84项。4路径新批已合入：源分区失败/空路径检查；brillo直接执行已标记备份脚本，保留标准输出且无新输出管道；内层首错与清理、V3挂载失败守卫；helper/50-lineage错误传递及缺省文件成功。addon先复制pending目录再提交，覆盖“复制已写文件后报错”的情况。第三方脚本只传播实际返回码，不称其内部错误全已覆盖。

56个隔离场景通过（26方法/15完整脚本/6helper/9内置消费者），未执行真实挂载、Android Toybox、OTA或手机操作。build02 01:48/exit0完成原生代码，最终build03 01:08/exit0及verified03通过。实际update_engine SHA f72dc968797b147fe5aa062d0b1a2e4d0e081652a7b8b6cc52b671439cdf4744，符号副本.text与安装ELF一致、MountPartition直接Run调用已核对。实际脚本/87源/保护载荷匹配，策略694b6134…/1,604,882 bytes与安装版字节一致，无allow/标签改变，静态backuptool及动态KSU例外保持。

正式组件双方delivery/ishtar-selinux-ota-contracts-components-20261018-01，38项/1,827,850 bytes/SHA a4d045d7b66e6359d44093bf1fe15ecb4d18f0e6f4d7b910e973bf29c92d2901，非完整ROM/可刷包。未新打整包或刷机；手机仍1792333073/全局P。本轮无新采集、重启、切E或真实OTA，没有这些新授权，Ubuntu其他任务未调整，正式新旧ROM/增量缓存保留。

保留过程失败：测试01根路径重定位错误（修正后原候选tests02通过）；Ubuntu缺rg的读取失败后用限定扫描；build01版本pin遗留1792325510，在编译前停止，修正build02；verified01不识别mangled PLT，按实际符号verified02通过。随后发现半途复制问题才形成候选02及最终build03/verified03，旧中间态不当正式交付。

剩余挂载代理、inactive标签/安全xattr/最终标签、精确执行权限、V3/EROFS及optional升级契约、必要E/冷启动验证仍待完成，不把56个主机场景当OTA/Enforcing通过。


## 1792333073已安装、桌面和数据验收通过（当前状态）

本轮74动作/73完整下载成功，wire SHA `0e96eb62ede64068b3ecf973734f60564ef40bb462971d9d476702efcd6b28f8` 已核对，23:07:24首次正常重启。53个文件、21个分区范围、7组保留策略契约及六MIUI默认入口读回一致，150次命令尝试、0重试；数据三项基线保持。机主确认“桌面正常，应用和照片都在”，仅记录桌面/应用照片保留验收，不补记本版相机、麦克风、指纹或全硬件验收。

实际1792333073/A槽/FBE，仍为全局Permissive/enforce0。磁盘策略694b6134dfd89758742f964df8d6f288203df5c766c75f2225b30af0fbaaed53；加载策略49db1c52b7d70b4060dabefea3b523bc6cc201b4a13433e965bb2b5e0711dbec/2,152,173 bytes。静态宽松域仅backuptool，实际加载backuptool及动态ksu两域，内核/KSU保持。本轮累计8个静态域已去宽松声明；全局仍P，不据域数量宣称完成比例或Enforcing就绪。未做Enforcing/冷启动Enforcing/真实OTA。

有限启动observer及所有本轮worker已结束：实际uptime16.31–194.67秒、31样本，核心PID无观测到变化；有1次已恢复的传输/root响应失败。合并缓冲快照与有限流保留514条唯一AVC行、audit_lost最大970，含速率/缓冲限制，不是全量开机拒绝、零拒绝或514个功能故障。初始快照未见MIUI进程，后段日志有该域，不混同为初始域验证。后续继续按必要调用链排查，不批量放开相机枚举或诊断探针。

私有运行证据31项/1,520,496 bytes/SHA `2ee429a7b62b31fc5078f81f2dd61b0e09e48641cf35ba6f5601e07e2fc8c6bc` 已在Mac/Ubuntu一致保存；原始日志、加载策略及ROM不入Git。完整ROM仍取Ubuntu正式delivery/ishtar-selinux-debug-ota-permissive-candidate-20261018-02（4,545,424,895 bytes/SHA813f87bd…），冻结交付不改；Mac只有薄交接和私有运行归档。活动输入为02 control/expected-source-files.json共84项。

按已有清理授权，在双端证据/数据验收后仅删除本轮已完成SSD刷写暂存及同SHA的out重复ZIP，释放28,837,486,592 bytes（约26.86GiB），SSD空闲约108.88GiB。本版和上一版1792325510正式ROM保留，其他HDD旧档本轮未清；未重新全量哈希保留的正式ZIP，重复输出ZIP删除前已哈希。源码/.repo/厂商依赖/当前增量缓存、其他任务未动。旧runner暂存已删除，禁止重放。

本次刷入及首次正常重启授权已用完，不复用作额外重启、切模式或OTA。手机无需继续连接USB。下一步仍是backuptool挂载代理、inactive标签/安全xattr/最终标签、内层错误汇总及optional处理，随后才是必要Enforcing运行/冷启动验证；当前P启动不替代这些测试。


## 1792333073完整候选已构建核验，未安装（最新）

最终02整包debug-ota-rom-build-02 exit0（20:28），audit-status及worker complete；34实际payload分区、84项源码、累计16个未安装变更路径、新update_engine/顶层脚本/3个init输出及保护载荷检查通过。静态保留数据升级：与已安装1792325510的OTA证书字节/SDK/补丁/fstab保持，时间戳向前，无userdata/metadata及wipe/downgrade标记；完整OTA签名未独立密码学核验。候选仍为全局Permissive、静态宽松域仅backuptool，KSU动态例外/内核保持；不是Enforcing或真实OTA验收。

Ubuntu正式delivery/ishtar-selinux-debug-ota-permissive-candidate-20261018-02，ZIP4,545,424,895 bytes/SHA813f87bd22c32cf8d59b0287a01128e28969164cd5dfcf60b4f033c25983f503，incremental1792333073。Mac同名薄交接78项/SHA51e3f5ad068db397e50eeeb378ec9bceb757e40d39b387220e82e7454c43bce8/386,562 bytes，不含完整ROM；小型证据和源码均双方一致。活动源码仍02 control/expected-source-files.json共84项；已再次匹配，全部本轮worker退出、构建锁空闲，不重跑01/02构建或旧刷写runner。

手机最后确认1792325510/Permissive、原9静态宽松域，本轮无手机操作/重启/刷写/E/真实OTA；新刷入及首次正常重启尚未授权。backuptool的挂载代理/inactive标签/安全xattr/最终标签恢复、内层错误与optional处理及必要冷启动验证仍待办。静态域9→1不是完成百分比。当前普通硬件无需重复全量验收。打包后段测得CPU4/5闲置95%以上，仅将本任务及线程扩到4–7，保留nice10/I/O7/Ninja2，未调整其他任务。01整包因发现EROFS适用范围风险在交付前停止；修正后的02为唯一新交付。


## 02组件与文件系统范围检查通过，最终完整候选构建中（最新）

活动源码control/ishtar-selinux-debug-ota-next-20261018-02共84项；debug-ota-components-build-02 01:46/exit0、verified01/audit worker complete。策略694b6134…/1,604,882 bytes仍严格仅3条typepermissive删除，候选静态域仅backuptool，KSU/既有allow/标签及保护载荷保持。23方法场景+16脚本组合通过，新增system/ext4适用范围/源魔数核验及vendor/EROFS常规postinstall覆盖。实际update_engine SHA07a1a2070b15347e81f92f9fc764bc4e7c3a137de3ca4b0ba2138423f57d306a。

双方149项组件归档SHAc83b359a2ccdd5ff83315bb4080fb9d220977ca3b70db866e9c477ede29752bc/3,219,623 bytes，delivery/ishtar-selinux-debug-ota-components-20261018-02。01整包已停止且不交付；新的02完整候选control/rom-candidate01，真实job debug-ota-rom-build-02/build1513333/audit1513334正在运行。dispatch最初job字段误写01，已保留原记录并按实际shell PREFIX/started修正元数据，没有重启构建或改流水线脚本。只观察这份新job/audit，不重派发。

手机仍1792325510/Permissive、原9静态宽松域；本轮未读写手机、刷机、重启、E或真实OTA，也无这些新授权。backuptool全链路/标签/挂载及必要冷启动验证仍待完成，不把域数量当完成比例。其他Ubuntu任务保持。


## EROFS后处理修正已合入，02组件构建中（覆盖下方01构建状态）

发现旧备份分支对vendor/EROFS也读取ext4 mount_count；新的挂载错误检查可能令其常规postinstall被跳过。因此已核对归属并仅停止自己的01整包build1450807/audit1450808及同会话子组，全部退出且锁空闲；未停其他任务，01不是可发布ROM。02只在system/ext4上查询源槽并检查6字节中的ext4魔数；EROFS/其他分区继续正常只读postinstall，非ext4源不写目标。23个实际方法隔离测试及16个新旧脚本组合通过。首个02测试请求早于候选目录创建完成而停止（无源码改动），待准备实际结束后新的测试通过，原失败留存。

当前活动control/ishtar-selinux-debug-ota-next-20261018-02，共84项。新component任务debug-ota-components-build-02/build1510622，audit1512355；仅看02状态，不重跑01。代码仍保留backuptool P、原allow/标签/KSU边界，未做手机操作。修正后的完整ROM尚未派发；新刷机/重启/Enforcing均未授权。


## 调试域收紧组件通过；84项源码完整候选构建中（最新）

机主要求继续源码修复、暂不切模式。活动入口已转为Ubuntu control/ishtar-selinux-debug-ota-next-20261018-01/expected-source-files.json，共84项；旧78项是中间组件，71项描述已安装1792325510。新6路径组件03:06/exit0，verified01与audit worker complete。策略694b6134dfd89758742f964df8d6f288203df5c766c75f2225b30af0fbaaed53/1,604,882 bytes；完整CIL严格仅qti-testscripts、vendor-qti-testscripts、su三条typepermissive删除，其他行一致，allow增删0。候选只剩backuptool静态宽松域；KSU动态例外及内核不动。手机仍1792325510/Permissive及原9静态宽松域，不能混为候选状态。

删除缺失脚本的qti测试服务/启动行，保留qcom-post-boot及radio diag_use；AOSP su与ksu不同，旧allow/dontaudit保持，但adb-root/VTS工作流未实机验证。update_engine加入读取长度/小端、挂载、WIFEXITED/退出码及全部清理步骤检查；顶层backup→restore→sync次序/次数保持，保真收到的首错。16个实际方法依赖隔离测试+16个脚本新旧组合通过，无真实挂载/OTA。内层addon错误汇总、optional阶段容忍、挂载代理/inactive标签/安全xattr/最终持久标签仍待修，backuptool不称Enforcing就绪。

149项组件交接双方SHA一致，delivery/ishtar-selinux-debug-ota-components-20261018-01，归档3,217,734 bytes/SHA9e73b6fcb298f811b6df798b563ba7469c1b8be73c4050620471b2e5bea266b1。完整候选在新control/rom-candidate01构建，debug-ota-rom-build-01，build1450807/audit1450808，CPU6/7 nice10 I/O7 Ninja2。只看此目录exit/audit状态，不重复派发。新刷入/重启/Enforcing/真实OTA均未授权；未动Ubuntu其他任务。首次本地ROM流水线准备因父目录不存在停止，已用缓存输入继续生成；未因此改Android源码/重跑组件。


## 开机属性/测试域组件核验完成；手机模式未切换（最新）

78源/10路径新批正常编译07:53/exit0，verified01与审计worker complete。策略0527e36204650c33bccb04ee6dee41436f8613c982a68a4ea18d02130e9059da/1,604,918 bytes；完整CIL差异仅5条typepermissive删除，allow增删0、上下文/类型/genfs/ioctl保持。两条旧true值system初始化、radio两触发器core-init归属、实际复制字节及7条system-only值通过；其他保护载荷保持。候选剩4静态宽松域，KSU未改，不称全部修完。

正式组件交接Mac/Ubuntu delivery/ishtar-selinux-boot-contracts-components-20261018-01，125项一致，归档1,884,308 bytes/SHA eb52ea9c8914312c07baaaf2996f9e15349e732cffc0e5d181c54239fba9eb8e。它不是完整ROM或可刷写包。活动源码是boot-contracts控制目录78项；旧71项只描述已安装1792325510。机主选择暂不切换，手机仍1792325510/Permissive及原9静态宽松域；新组件未安装/未做E或冷启动，禁止把候选4域写成手机状态。本轮所有有限worker已结束，没有额外刷写/重启授权。

指纹仅证实具体信息getter失败占位分支，不唯一归因所有AVC；RIL psno输出返回未检查，不等于所有消费者不存在。继续保持generic属性/DSP边界，剩4测试/调试/OTA域单独处理。首轮source查询在非独立Git目录停止；已补有界文件扫描，原记录保留。首次host_init_verifier按其帮助文案使用下划线选项被CLI拒绝；按实际源码换成连字符后verified02通过，没有因此放宽策略。已删除的旧SSD暂存不再可用，正式ROM/源码/增量缓存保持。


## 1792325510设置回退验收、开机日志收尾（最新）

机主确认“已恢复原版界面，正常”，只记设置回退通过。有限开机采集已 complete 并结束：32 状态样本至 uptime195.97秒，核心三进程PID无变化、无传输失败/流截断；缓冲区含更早日志，但 audit_lost最大844和速率限制使覆盖不全，不称零拒绝。Mac/Ubuntu的30文件运行证据归档一致（1,524,234 bytes/SHA ee86c70d4999d6f729cfe0c326eaa287f89ea2f6de0d7c8b046eb460b868bbe9）。所有本轮有限worker已结束，手机无需继续连接USB。

下一批先查三条legacy开机属性归属、指纹/RIL精确属性消费者及必要冷启动依赖；相机HAL大量属性遍历、第三方/诊断探针不批量放行。两个剩余vendor.prop定义已定位，尚未据此改源。初次MIUI进程快照缺席，后段安全日志见专用域，两者区分。当前71源与1792325510已安装策略保持，全局Permissive；新策略/冷启动Enforcing仍未验收，原9宽松域/KSU保持。详见维护仓库 plans/selinux-runtime-camera-stock-settings-20261018/boot-security-review.md。

本轮不再刷入、重启或切模式，旧ROM/暂存未清理，Ubuntu其他任务未调整。首次本地归档下载脚本根路径多了一层recovery，读取回执前停止；修正后一次只读下载及30文件校验通过，未因此重跑远端归档或手机操作。


## 1792325510已保留数据刷入、正常启动并归档读回（最新）

74动作/73完整下载及wire核对通过，首次正常重启后48文件/21分区范围与7组新加载策略契约匹配，数据基线保持；141命令尝试/0重试。自定义维护者载荷实际缺席、原版Settings摘要正确，六MIUI默认入口保持。磁盘策略ade0b38afe44ecd4a0afbd364978ffa6479c58b9ad5dbba8958420f801dfb818，加载策略7d32c21a5832b15bf88b6694047c4756dcf2b31baedb6d4746428873372a649c，原9静态宽松域+ksu保持，仍全局Permissive/enforce0。没有新策略E/冷启动验收，不重复麦克风/指纹全套测试。

有限开机安全observer状态complete，首次实际读到uptime=15.76秒；覆盖范围与速率限制/丢失另见early-boot-observation-summary，不能声称全量或零拒绝。原始有限日志、读回和刷写证据已在Mac/Ubuntu一致归档，原始日志不入Git。机主已确认“已恢复原版界面，正常”，只记设置回退验收。该候选刷入及首次正常重启授权已使用，不复用作额外重启或切Enforcing；旧完整ROM/暂存暂未清理，未调整其他任务。


## 最终候选1792325510完整构建与实际镜像核对通过（最新）

最终02整包及audit/worker均complete：34实际payload分区、71源/17路径、策略ade0b38afe44ecd4a0afbd364978ffa6479c58b9ad5dbba8958420f801dfb818、13项有效权限新增与保护载荷通过。自定义维护者APP/RRO在实际镜像缺席，Settings与固定上游恢复组件一致；五条旧值仅在system。相机HAL QSPM客户端与MIUI CPU/IO PSI只读合入，没有放开安全DSP或缺失MiSys。仍Permissive，原9静态宽松域/KSU保持，新策略尚未在手机运行/E实测。

Ubuntu正式delivery/ishtar-selinux-stock-settings-permissive-candidate-20261018-02，incremental1792325510，ZIP4,545,440,761 bytes/SHA25ac87915377aa8e914e4951bf170528a822df807127f6afccd0994e7e645796；Mac同名薄交接不含ROM。保留数据升级静态检查通过，完整OTA签名未独立密码学核验。机主已授权该下一候选现场检查后保留数据刷入、完整核验后首次正常重启，不重复询问。随后将用有限电脑端observer尽早读取该次正常开机的安全日志，约180秒/每流8MiB，不修改手机日志速率或启动项；实际覆盖以first_observed_uptime和丢失记录为准。

当前手机仍1792311835/Permissive；旧基线实际短时E功能验收与回退兜底限制另见试验摘要，不外推本候选或冷启动。未停其他任务、未清理旧ROM/暂存。


## 实际Enforcing新缺口已并入最终候选，02整包构建中（最新活动入口）

本轮实际短试机主确认桌面/MIUI打开/1×和5×均流畅，手机已恢复Permissive；独立guard和电脑兜底均参与，不能记独立回退单独成功。54份私有试验文件39,396 bytes归档SHA e9ee905d9bbc9269d659f0a50777cd94c3ce7b521997c59030e724e5f8f62f48，Mac/Ubuntu一致，原始日志不入Git。没有再次切E/重启，原9静态宽松域+ksu与冷启动边界保持。

实际E日志与独立契约核对后，新增两处：HAL camera标准QSPM客户端和MIUI CPU/IO PSI read/open/getattr（保留memory权限），无写/map/ioctl扩权。完整新增13项有效权限、0删除；类型/上下文/genfs/扩展ioctl/原宽松域保持。新策略ade0b38afe44ecd4a0afbd364978ffa6479c58b9ad5dbba8958420f801dfb818/1,604,930 bytes，组件03:33/exit0、verified01及audit worker通过；Settings仍50cedf20…且上游资源回退通过。该新策略未在手机运行或做E实测。

**活动71源清单改为Ubuntu control/ishtar-selinux-stock-settings-next-20261018-02/expected-source-files.json，共17路径（11删除）**。01目录的71清单及38cde组件仅是中间态；01整包/等待器在实际E发现缺口后已只对归属核验的自有会话1202864/1202865及Ninja子组发TERM，全部退出/锁空闲后才改源，没有形成交付ROM，禁止重跑旧worker。

最终02的rom-candidate01/stock-settings-rom-build-02正在构建，build1316867、audit-worker1316868；CPU6/7 nice10 I/O7 Ninja2。只看此新任务exit/audit状态，编译锁忙时不改活动源码；Ubuntu其他任务未调整，旧完整ROM/暂存未清理。私有仓库已准备5份许可明确源码、最小删除/属性差异与元数据，未提交/推送（main仍552d9f1…）。下一候选保留数据刷入、完整核验后首次正常重启已获机主授权，不重复问；没有额外E/持久模式/第二次重启授权。


## 1792311835已刷入正常启动，自动读回通过（最新）

新flash01完成74动作/73完整下载，wire SHA `e79febfde6d2774750cf48022faf28f03166cfe642d62c09547e4d0d1d63ba14` 通过；核验后首次正常重启。无userdata/metadata/底层固件写入，未锁BL。刷入与首次正常重启授权已使用，不复用作额外重启或Enforcing。

postboot01及worker exit0：50文件、21分区范围SHA全匹配，数据目录/属主权限/第三方应用数10基线保持；142次命令尝试、0重试。新19组策略契约与实际编译策略一致；105项包含新类型结构继承，不是105个bug。两诊断目录实际标签和原属主模式、两精确属性标签、三条system参数及三维护者APK摘要/三个运行资源值通过，维护者名CyberWei。实机卡片/图片显示和主动保存仍待机主回复；未自动打开支付链接或做扫码/到账验收。

磁盘策略38cde3d5…/1,604,906 bytes；加载策略 `a603e2d5734cfb2c04a1632fe81ed5d85e1dc09293b1f4fdcb4292b199652b80` /2,152,197 bytes。原9静态宽松域+ksu保持，全局Permissive/enforce0，未做Enforcing。六MIUI默认入口保持；初始MIUI进程观察为True，不能据此扩写所有相机模式或算法回退通过。

运行归档24项、1,520,153 bytes/SHA `68499f399fc68dc1eea9eac7e02db4074e489ee3831660c33779762f35733676` 两主机匹配；有限worker结束，无手机自启/模块或新增常驻采集。AVC仅一次启动快照，不代表全量/完成比例。模块启动、原静态宽松域/KSU及短时Enforcing/冷启动仍单独验证；下一里程碑为先启动独立30秒回退guard的受控运行试验，需要具体新授权。

活动82源/20路径保持，正式ROM仍在Ubuntu冻结delivery，Mac68项薄交接无ROM。保留新包、三个旧完整ROM和当前暂存，待正常使用反馈后按双版本规则处理；本轮未清理或调整Ubuntu其他任务。完整OTA签名未独立密码学核验。


## 边界与维护者候选1792311835完整构建完成（2026-10-18，最新）

boundary-maintainer-rom-build-01 exit0，audit-worker-status/audit-status均complete。实际34个payload分区、82项源码、策略38cde3d5a478bc285ba63fdb16c39cea8a1961208633dc6cd36e9239dd4f8bdc及保护载荷通过；三条启动参数仅在system，诊断目录重标记紧随原mkdir。三个维护者APK与组件摘要相同；原9静态宽松域/KSU保持，全局仍Permissive。没有Enforcing或UI交互验收，未据此声称相机性能提升。

正式Ubuntu `delivery/ishtar-selinux-boundary-maintainer-permissive-candidate-20261018-01`，ZIP 4,546,311,909 bytes/SHA `4f40938189396d92f2fa2a294a82ef6f7c6bf954bf149abd54c2f47a0fda3bd1`。Mac同名薄交接68项，不含ROM；静态保留数据升级检查通过，手机现场预检仍待授权后进行，完整OTA签名未独立密码学核验。

手机最近读回仍1792300213/Permissive。本轮没有手机采集/刷写/重启/setenforce；新候选刷入及首次正常重启、短时Enforcing试验须具体另确认。短时试验计划以先启动独立30秒回退guard、核对就绪后才临时开启为前提；真实E到P回退尚未验证，原静态宽松域和KSU例外不等于全域强制。模块启动拒绝完成限定静态分析仍不能唯一归因，留给另行授权的受控冷启动，不随意开放块设备。原三版完整ROM及当前暂存保持，未清理或调整Ubuntu其他任务。

许可明确16份Apache-2.0源码及最小补丁/元数据准备上传；完整file_contexts/vendor.prop和两张收款图继续外置。原图仅本地整包内使用，三个APK/策略二进制/ROM/原始手机日志不进Git。组件194项和失败检查器记录仍保留，不重跑旧worker。


## 下一候选：SELinux边界与维护者页面（2026-10-18）

活动源码为plans/selinux-boundary-maintainer-20261018/working-source-files.json的82项，累计20路径相对已安装1792300213；原68项清单仅描述安装基线。组件12:10/exit0及verified03通过，策略38cde3d5…/1,604,906 bytes，与中间边界策略完全一致。原类型权限增删0；新diag类型105项有效权限含8项明确目录/FIFO权限和结构属性继承，不能称105个bug修复。

保留600/true的两条legacy启动配置并移至system初始化；虚拟相机状态/OEM serial精确复用现有相机/radio属性类型；RIL diag复用radio标签，diag-router日志目录使用专用类型，重标记紧随原mkdir。生成的五分区build.prop已确认三项值仅在system；完整payload检查仍待整包完成。

维护者显示CyberWei，独立普通APP和专用system_ext RRO提供GitHub、QQ复制、TG、微信/支付宝原图展示和主动保存、PayPal外链；非官方卡片不显示官方徽标。三个APK签名、idmap、原图字节检查通过，APP不请求权限；实机卡片/图片保存/支付应用扫码尚未验收。

boundary-maintainer-rom-build-01和电脑审计等待器正在Ubuntu同名control/rom-candidate01运行；CPU6/7 nice10 I/O7 Ninja2，未调整其他任务。本轮无手机操作或Enforcing，原9静态宽松域/KSU保持。新刷入/正常重启及受控Enforcing须具体新授权；待需要配合时再请求，不重复完整硬件清单。Git仅包含16份许可明确源码及最小补丁/元数据，完整file_contexts/vendor.prop及两张图片保持外部依赖。

## 1792300213已保留数据刷入并启动，自动读回通过（最新）

本次新flash01完成74动作/73完整下载，wire SHA `51111b2b3512c3673e64fb04e5df8545323067a1b4f7d0f462d408eb69342250` 核验通过；2026-10-18 13:50:13正常重启。没有userdata/metadata/底层固件写入，未锁BL。授权只涵盖此次刷入及首次正常重启，现已使用，不复用作额外重启或Enforcing。

postboot01 success/worker exit0：47文件、21分区范围SHA全匹配，数据目录及属主权限/第三方应用数10的本次新鲜基线保持；127次命令尝试、0重试。新增16组加载策略契约覆盖32项权限，与实际编译策略一致。蓝牙自动连接值true；热控启动属性为vendor_thermal_normal_prop、值1，热控进程存在。六类默认入口仍为MIUI；初始快照未见MIUI进程，不称本版相机运行域/算法回退已验收。没有读取Google账号或凭据内容。

磁盘策略 `1eaa4c3edd8b9125a436029a52c8f7d673135ec55d7b4a956cf9134181f944fe` /1,604,627 bytes；加载策略 `52cf610059c121ae3acd1023ccf01382ee673eba89c199822e1d151f76f5adec` /2,151,918 bytes。原9静态宽松域+ksu保持，仍全局Permissive/enforce0；未进行Enforcing试验，也未声称本版获得新的机主功能验收。本版按实际风险不重复要求麦克风/指纹全套测试。

24项运行证据已在Mac/Ubuntu归档，tar 1,518,646 bytes/SHA `3b27cdee9b78eb445a12491ebf02069ffca302fe910539028c3b067f41bf6f5a`，双方摘要及Mac逐文件匹配。有限构建/审计/刷写/读回worker均结束，无新增常驻采集/模块、自启或额外重启，未调整Ubuntu其他任务。保存的启动logcat有624条AVC/14域、kernel有15条，两份有重叠且audit_lost最大1；不是完整覆盖或修复完成比例。相机HAL属性枚举、Mtd DAC、私有属性及原宽松域等继续按必要功能处理，不能批量放行。

当前活动源码为control/ishtar-selinux-enforcing-prep-20261018-01/expected-source-files.json的68项，旧64/62清单仅描述中间/旧安装基线。正式ROM仍在Ubuntu同名delivery，Mac为54项薄交接，不含ROM；完整OTA签名未独立密码学核验。当前保留新1792300213及原1792266978/1792261801完整ROM和本轮暂存，尚未执行清理，待新版本正常使用反馈再按双版本规则收尾。

维护者设置页展示明确留到下一版，等机主提供显示名及资料后实现；本版没有为此重包，不假定Git用户名，也不将个人维护者展示表述为官方资格。

## 集中候选构建完成时的记录（刷入前历史）

统一enforcing-prep-rom-build-01 exit0，audit-worker-status和audit-status均complete。实际34个payload分区、68输入源码、策略1eaa4c3edd8b9125a436029a52c8f7d673135ec55d7b4a956cf9134181f944fe、默认入口和原保护载荷通过；实际system/vendor以及其余三分区的属性位置检查通过，蓝牙true只在system侧，音频600参数保持。静态保留数据升级检查通过，完整OTA签名未独立密码学核验。

Ubuntu正式目录：`/mnt/android-download/delivery/ishtar-selinux-enforcing-prep-permissive-candidate-20261018-01`，版本1792300213，ZIP 4,545,439,341 bytes/SHA `3b1e518603f2c3d911d7ffbca886df4c97531f419ac733ab201673c166deddec`。Mac同名薄交接54项，不含完整ROM。组件121项和实际有效权限增量32/0检查保持。原9静态宽松域未删除，KSU例外和实际Enforcing尚未验收。

手机仍是机主已测1792266978/Permissive；本轮未刷入、重启、采集手机日志或切Enforcing。当前两版已安装完整回退ROM1792266978/1792261801保持，新候选可并存等待验收；不提前删回退版本。机主已授权本候选在新鲜预检通过后保留数据刷入及首次正常重启，Enforcing仍未授权；使用新runner，不复用旧刷写流程。维护者展示依机主指示留下一版，待其提供名字/资料。验收只按直接风险安排，本轮不默认重复麦克风/指纹。Ubuntu其他任务未调整。

以下为各阶段历史，当前已安装版本及自动检查范围以上方1792300213节为准；原accepted源码标签仍为1792226532。

## 1792266978历史安装与验收入口（2026-10-18）

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
