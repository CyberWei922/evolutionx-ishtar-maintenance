# ishtar maintenance repository

## 1792347689 已安装、自动读回通过（当前状态）

本轮Backuptool原生挂载代理及旧升级入口兼容候选已按机主“刷入 手机已连接”保留数据刷入并正常启动。74动作/73完整下载及wire SHA 9dcc1441a36325c6f909b25561ebb5eaf1fa2cd6892ac20543ba09a351fab765通过。57文件/21分区范围、7组保留策略契约和5组代理相关策略契约、broker执行标签/0:2000:755、六MIUI默认入口及三项数据基线匹配；158次读回命令尝试、0次只读重试。机主桌面/应用照片验收待回复；不补记相机、麦克风、指纹或全硬件验收。

实际1792347689/A槽/FBE，仍为全局Permissive/enforce0。磁盘策略597305253046a890c46339828658775648e0f0f28f8d2597c5d14d3a54f08f75；加载b450c7d4760ffdf0bd9abfe47457fbc91fdb54756047621fa1b108144ed5b083/2,152,552 bytes。静态宽松域仅backuptool，实际加载另有动态ksu，内核保持。首轮只读检查把代理文件组误写为root，实际镜像及手机均为shell组2000；原postboot01失败保留，postboot02仅继续剩余读回，未重刷或重启。本次文件及策略读回不代表原生代理真实OTA执行、backuptool已可Enforcing或Enforcing冷启动通过。

有限启动采集已结束：uptime12.18–192.15秒/32样本，核心PID观测变化0次，采集传输失败0次。缓冲快照和有限流合并保留620条唯一AVC行、audit_lost最大1064；不是全量开机拒绝、功能故障个数或完成百分比。初始MIUI进程域是否见到：False。

私有运行证据双端一致：40项/3,011,906 bytes/SHA 110ea6328bc72aad5f308c56fad838c37b2ab21f4428ea189492d1334377eb82。正式完整ROM仍为Ubuntu delivery/ishtar-selinux-backuptool-broker-permissive-candidate-20261019-02（4,545,345,933 bytes/SHA32df7de2…），Mac仅薄交接及私有证据，Git只存许可明确源码与脱敏摘要。94项活动源码重新匹配；本轮构建/刷写/读回及有限observer均结束，不再进行手机采集或切模式。旧正式ROM、当前SSD暂存与增量缓存保留；未调整其他Ubuntu任务。

本次刷入与首次正常重启授权已用完。未做Enforcing、额外重启或真实OTA，不复用本次授权。下一步仍需inactive路径/符号链接隔离、持久标签/security xattr、精确权限、V3/EROFS及optional处理，然后做另获授权的必要Enforcing运行/冷启动验证。下方候选未安装及刷入现场描述为历史。


## 1792347689完整下一候选已编译核验，尚未安装（当前工作源码）

本轮原生Backuptool挂载代理与旧升级入口兼容候选已完整编译（10:06/exit0），实际34个payload分区/94项源码/9份新或保留执行与init文件、broker inode执行标签和保护载荷检查通过。72个隔离主机场景通过；构建使用CPU0–6/Ninja7/highmem2/nice5/I/O4，一次实测payload生成进程约695%CPU，留CPU7余量，未调整或停止Ubuntu其他任务。

正式完整ROM：Ubuntu delivery/ishtar-selinux-backuptool-broker-permissive-candidate-20261019-02/EvolutionX-16.0-20261019-ishtar-11.12-Unofficial.zip，4,545,345,933 bytes，SHA 32df7de2c2648c2ed01b96bb2ffc42fa93f9e79ff3404b4db87ff3967590ff0a。Mac同名目录仅薄交接，不含完整ROM；双端薄交接72项/383,639 bytes/SHA 138f9d8d8847669e376bd1cb013a559d7071109bb19a49a99239c2ee5467c2c1匹配，组件交接147项/2,568,245 bytes另行保留。正式本候选、已安装1792339544与上一1792333073回退ROM均保留，本轮未清旧正式包/增量缓存或触碰其他任务。

静态保留数据升级检查：与已安装1792339544归档的OTA证书字节、SDK36、补丁2026-10-01及fstab保持，时间戳向前，无userdata/metadata/wipe/downgrade载荷；完整OTA签名未独立密码学核验。以上为静态结果，不冒称刷入、冷启动或真实OTA通过。

候选策略597305253046a890c46339828658775648e0f0f28f8d2597c5d14d3a54f08f75/1,605,261 bytes，静态宽松域仍仅backuptool，KSU动态例外/内核保持，全局仍P。本轮新增broker执行/临时目录两类型；现有类型只有5项update_engine权限新增、删除0，其他原权限及genfs/ioctl边界保持。代理在自身私有挂载命名空间建立tmpfs临时路径与inactive system视图，将单独子进程切到原backuptool域执行旧流程，尝试全部清理，父进程恢复system块设备只读；新broker路径不由worker挂临时盘；旧更新器直接运行新顶层脚本仍有经标志选择的过渡临时盘流程，5个兼容场景通过。未给worker新增SYS_ADMIN或一般system_file写权限。命名空间不是worker安全沙箱，V3挂载、inactive符号链接/路径隔离、持久标签/security xattr、精确权限、optional及必要Enforcing运行/冷启动仍待完成。

当前活动源码仍broker control/expected-source-files.json共94项，最终source-candidate04/source-apply03、组件build04/verified03/component-audit04为准；前序host计数回滚、merged-policy断言冲突和优化字符串检查器失败均保留，不改写成成功。仅许可明确10份Apache-2.0源文件/NOTICE/REUSE及脱敏构建摘要进入维护仓库；ROM、ELF、策略二进制和原始日志不入Git。

手机最近确认版本仍1792339544/Permissive、磁盘694b6134…/加载49db1c52…，本轮无手机读写、新采集、重启、刷入、切E或真实OTA。新候选未安装，没有本轮新的刷入/重启/Enforcing/OTA授权；旧授权已用完，不重放已删暂存/旧worker。下文描述历史阶段。


## 94-input broker candidate building (current working source)

Active source is plans/selinux-backuptool-broker-20261019/working-source-files.json (94 inputs/9 changed paths), final source-candidate03/source-apply02. Component build03 exit0, verified02 and component-audit03 complete;67 isolated source cases pass. Actual broker/updater ELFs, contexts and policy are checked. New policy59730525…/1,605,261 bytes retains backuptool permissive and the existing KSU exception; existing-type permission delta is5 updater-only additions/0 removals. No upstream neverallow change and no worker SYS_ADMIN or general system_file write grant.

Complete ROM job backuptool-broker-rom-build-01 is running (own build1704835/audit1704836), CPU0–6,7 Ninja jobs/highmem2/nice5/I/O4, per owner's new resource instruction. Do not alter active source while locked; do not stop unrelated Ubuntu work. Broker namespace is not a worker security sandbox: precise rights, inactive symlink paths, final labels/security xattrs,V3/EROFS and optional OTA semantics remain pending. No Enforcing/cold startup/real OTA validation claimed.

Phone remains installed1792339544/P on the87-input previous batch. New94-input components are not installed; no new flash/reboot/Enforcing/OTA authorization. The143-file private component handoff matches on both hosts. Git contains only9 reviewed Apache-2.0 source files/notices and summaries; never import its binaries, private policy or logs. Prior failed intermediate build/checker records remain local and are historical.


## 1792339544已安装、桌面和数据验收通过（当前状态）

本轮4路径OTA执行/错误传递/事务复制修复已完整编译并保留数据刷入。74动作/73完整下载及wire SHA ce68887bd0dd8249e4487ef1fb1cd9d88b127bb760600acc0f1d59c4d061e11c通过，2026-10-19 00:51:01首次正常重启。56文件/21分区范围、7组保留策略契约、六MIUI默认入口及三项数据基线均匹配；153次读回命令尝试、0重试。机主本次确认“桌面正常，应用和照片都在”，只记录桌面/应用照片保留验收，不补记相机、麦克风、指纹或全硬件验收。

实际1792339544/A槽/FBE，仍为全局Permissive/enforce0。磁盘策略694b6134dfd89758742f964df8d6f288203df5c766c75f2225b30af0fbaaed53与上一版相同；加载49db1c52b7d70b4060dabefea3b523bc6cc201b4a13433e965bb2b5e0711dbec/2,152,173 bytes。静态宽松域仅backuptool，加载另含动态ksu；内核/KSU保持。四路径改动改善OTA脚本调用/失败处理，不代表已运行真实OTA或backuptool已可Enforcing。挂载代理、inactive隔离/精确权限、持久标签/security xattr、V3/EROFS及optional处理仍待完成，再做必要Enforcing运行和冷启动验证。

有限启动采集已结束：uptime18.25–192.69秒，31样本，核心PID未观测到变化，采集传输失败0。合并缓冲快照与有限流保留709条唯一AVC行、audit_lost最大2；有速率/缓冲/窗口限制，不是全量开机拒绝、709个功能故障或修复进度百分比。初始快照无MIUI进程，不冒称初始域验证。读取监控666的host loop/caller超时保留，未影响独立刷写或触发分区重试。

私有运行归档30项/1,521,192 bytes/SHA536a8f9ba9c5e4fc510f3344bccbe377792fee924ac44541b886523f0d9769ba双端一致；归档早于本次机主答复，验收另存补充，不改冻结归档。正式完整ROM在Ubuntu delivery/ishtar-selinux-ota-contracts-permissive-candidate-20261019-01，4,545,425,494 bytes/SHA d0cd1eae3de97d54052c8262c5fea80b662eb32f7bc38de6efef1e741de9a2ad；Mac仅薄交接/运行证据，Git仅许可明确源码和脱敏摘要。完整OTA签名未独立密码学核验。活动源码为ota-contracts control/expected-source-files.json共87项，已重新核对；所有本轮worker退出，构建锁空闲。

按已有清理授权，在双端证据与本次数据验收后删除本轮完成的SSD刷写暂存及已核对同SHA的out重复ZIP，释放28,837,494,784 bytes（约26.86GiB），SSD空闲约108.88GiB。正式本版/上版1792333073保留，其他HDD旧档本轮未清；保留正式ZIP未重复全量哈希。源码/.repo/厂商依赖/当前增量缓存和其他Ubuntu任务未动，已删暂存禁止重放。

本次刷入及首次正常重启授权已用完，未做Enforcing、冷启动Enforcing或真实OTA；不复用为额外重启或切模式。有限采集结束，手机无需继续连接USB。下方各阶段记录为历史。


## OTA contract components (current source; not installed)

Active input is plans/selinux-ota-contracts-20261018/working-source-files.json,87 files; Ubuntu control/ishtar-selinux-ota-contracts-20261018-01. Four paths add source resolution checks, direct labeled script execution, truthful received-error propagation/cleanup and transactional addon-directory publication.56 isolated host cases passed; finalbuild03/verified03 complete. Policy bytes694b6134… are unchanged; no permission/context/KSU change. Components are on both hosts, not a flashable ROM or phone installation. Keep1792333073/P and its84-input installed record distinct from this87-input working source.

Use provenance/ota-contracts-source-files.json for four Apache-2.0 full sources and retained notices; no compiled/private payloads in Git. Optional postinstall stays optional; arbitrary addons can still swallow internal errors. Broker mounts, inactive persistent labels/security xattrs, precise execution rights, V3/EROFS and necessary Enforcing cold startup remain unvalidated. No phone logs, reboot, flash, mode switch or actual OTA was performed or newly authorized. Leave unrelated Ubuntu work and current caches alone. Do not replay older workers or deleted staging.


## Installed1792333073 and owner desktop/data acceptance (current)

The authorized preserve-data flash completed74 operations/73 downloads, wire0e96eb62… verified, then the first normal reboot. Installed53-file/21-range/7 retained-contract/six MIUI-route checks and data baselines passed;150 command attempts/0 retries. Owner confirmed normal desktop and existing apps/photos only. Do not expand this to new hardware, camera, microphone, fingerprint, Enforcing or OTA acceptance.

Current source is the84-input final02 debug-ota plan; phone is1792333073, globally Permissive/enforce0. Static permissive list is only backuptool; loaded list is backuptool and dynamicksu. Disk policy694b6134…; loaded49db1c52…/2,152,173 bytes. All own build/flash/readback/finite observers ended; startup observation16.31–194.67s had31 samples, no observed core PID change and one recovered transport failure. Combined buffered AVC counts are incomplete and are not a list of necessary allows.

Private31-file runtime archive matches on both hosts; only summaries enter Git. After evidence preservation and owner acceptance, this completed SSD flash scratch and matching duplicate output ZIP were removed; current/previous formal ROMs remain, other HDD archives were not cleaned. Do not replay deleted staging or successful runners. Authorization is consumed; no extra reboot/Enforcing/actual OTA is authorized. Remaining backuptool mount/labels/xattrs/error contracts and necessary Enforcing cold startup are separate. Leave unrelated Ubuntu jobs alone. Older entries below describe historical stages.


## Final1792333073 ROM (current, not installed)

Final02 full ROM and34 actual payload partitions/84 inputs/16 cumulative uninstalled paths passed static checks; build and audit workers are complete and no longer live, build lock free. Active source remains control/ishtar-selinux-debug-ota-next-20261018-02 and plans/selinux-debug-ota-20261018/working-source-files.json. ROM SHA813f87bd…/4,545,424,895 bytes; Mac78-file thin handoff only, full ROM on Ubuntu. Policy694b6134… static permissive list only backuptool; global boot remains Permissive. KSU/kernel and accepted payloads unchanged. See rom-build-summary.json; complete OTA signature not independently cryptographically verified.

Phone last confirmed1792325510/Permissive with its original9 static domains. No new phone operations, flash/reboot/Enforcing/real OTA authorized. Owner chose source repairs without mode switch. Do not replay completed builds, superseded01 or deleted flash staging. Remaining backuptool mount/label/xattr/inner-error contracts and necessary cold-start validation are separate. Ordinary hardware does not require another full regression by default. Only own packaging processes/threads were expanded to measured-idle CPUs4–7; nice10/I/O7/Ninja2 and unrelated tasks preserved.


## Final02 components and ROM build (current active source)

Active input:plans/selinux-debug-ota-20261018/working-source-files.json,84 files; control/ishtar-selinux-debug-ota-next-20261018-02.23 method and16 wrapper combinations passed; components01:46/exit0 and verified01 passed. Backups are scoped to system/ext4 and validate source magic; vendor/EROFS normal postinstall is retained. Compiled policy694b6134… has only backuptool static permissive; effective allow/context/type/ioctl lines unchanged, KSU/kernel unchanged. Source snapshots now describe final02. Phone remains1792325510/Permissive with original9 static domains, unmodified.

The01 full-ROM build was stopped before release after the EROFS issue; only verified owned sessions/children were terminated. Final02 ROM build1513333/audit1513334, actual job debug-ota-rom-build-02, is in progress. Do not replay01 or re-dispatch02. No flash/reboot/Enforcing/real OTA is authorized. Preserve existing payloads and unrelated Ubuntu work. Dispatch's initial job-name typo was corrected with original evidence preserved; no build restart or pipeline edits.


## Debug-domain / OTA safety batch (current working source)

- Use plans/selinux-debug-ota-20261018/working-source-files.json:84 pinned inputs,6 latest paths over the uninstalled78-input batch. Component policy694b6134… passed, with exactly3 typepermissive removals and no other exported CIL changes; candidate static permissive domains contain only backuptool. KernelSU/ksu and kernel are unchanged. The phone is still1792325510/Permissive with its original nine static domains.
- Keep qcom-post-boot, radio diag_use and all existing AOSP su allow/dontaudit rules. AOSP su differs from ksu; adb-root/VTS is not newly runtime validated. OTA safety checks do not make backuptool enforcement-ready: inner addon errors, optional handling, broker mounts, isolated inactive labels, preserved security xattrs and final labels remain pending.
- The complete Permissive ROM is building only on Ubuntu under the new debug-ota control/rom-candidate01; do not re-dispatch or alter unrelated tasks. No new flash/reboot/Enforcing/real OTA is authorized. The owner chose source fixes without a mode switch.
- Reviewed upload scope:provenance/debug-ota-source-files.json. Preserve original Linux Foundation/QIC BSD notices, AOSP/Lineage Apache notices and the sepolicy public-domain NOTICE. No binaries/raw logs/disassembly or proprietary original text in Git.


## Boot ownership/test-domain components (current working input)

- Current source is plans/selinux-boot-contracts-20261018/working-source-files.json:78 pinned inputs,10 paths over installed1792325510. Component policy0527e362… is verified; five optional test-domain permissive declarations are removed with all other exported CIL lines unchanged. Candidate static domains are backuptool,qti-testscripts,vendor-qti-testscripts,su. KernelSU is unchanged.
- Current phone remains1792325510/Permissive with the original nine static domains; the new batch is not a complete ROM or installed. The owner chose source repairs and declined another runtime switch for now. Do not start Enforcing, reboot or flash, and do not reuse expired guard tokens.
- Two legacy true values move to system initialization; two radio restart triggers move to core init while vendor services/commands remain unchanged. Preserve all accepted camera/brightness/Recovery/refresh/Settings payloads. Reviewed sources and Apache/BSD notices are in provenance/boot-contracts-source-files.json. Full vendor.prop/qcrilNrd.rc, binaries and original logs stay external.
- All finite build/audit/preflight workers ended. The completed1792325510 SSD flash scratch and duplicate output ZIP were deleted after both-host evidence verification; never replay that old runner. Complete ROMs and active build cache remain. Leave unrelated Ubuntu work alone.


## Stock Settings and runtime-camera batch (current working input)

- Use plans/selinux-runtime-camera-stock-settings-20261018/working-source-files.json:71 pinned inputs,17 paths over installed1792311835. Remove custom maintainer APP/RRO/resources and restore upstream Settings; preserve IshtarCameraDefaults and all accepted payloads.
- New policy adds the standard camera HAL QSPM client and dedicated MIUI CPU/IO PSI read-only contract. Complete delta/neverallow checks passed. Current ROM remains globally Permissive; original nine static permissive domains and KSU remain. Baseline short runtime Enforcing user checks passed within stated scope, with host-restoration fallback caveat; no cold-boot or new-policy Enforcing acceptance.
- Reviewed upload list is provenance/runtime-camera-stock-source-files.json. Keep full vendor.prop, owner images, compiled files and raw phone logs external. Old maintainer plan is historical and must not be reapplied after this cumulative patch.
- Candidate1792325510 is installed and owner-confirmed stock Settings normal. Readback48 files/21 ranges/7 loaded contracts and finite startup observer are complete; all own workers ended. The flash/first reboot authorization is consumed. No extra Enforcing attempt, persistent switch, extra reboot or unrelated-process change is authorized. See boot-security-review.md and installed-readback-summary.json in this plan.


This is an incremental maintenance source repository for Xiaomi 13 Ultra (ishtar), Evolution X Android 16. Read README.md and docs/CURRENT_STATE.md first.

- The owner has disabled Gemini delegation. Do not invoke Gemini Delegate MCP, delegate.py or AGY as an alternative. Older delegation preferences are superseded.

- Preserve exact upstream project revisions in manifests/. Patches are relative to their recorded project HEADs.
- Preserve existing device/common/kernel/KernelSU modifications, not only the newest fix.
- No ROMs, APKs, shared libraries, partition images, private keys, tokens, device-private logs or complete Android checkout in Git. Keep binary dependencies as sizes/hashes/locations.
- SELinux remains globally Permissive. Do not claim Enforcing validation or blindly generate allows from denials.
- Current startup refresh fix is accepted for one first-boot desktop swipe. Four new and 24 mode-switch tests pass; six older tests remain failing in the selected 57. Do not expand acceptance.
- MIUI default routes and the owner preference migration are installed as 1792252694: six resolver checks and the dedicated app domain passed. Owner confirmed requested data/default camera/photo-video sound/fingerprint checks normal. Preserve Aperture as fallback; external capture-result return and Enforcing validation are still separate.
- No device reboot, flashing, data wipe, bootloader relock, automatic log capture, repo sync/reset/clean, or production changes are implied by maintaining this repository.
- Review proposed source and metadata differences before applying. A repository snapshot is not a flash instruction.

## Boundary/maintainer next build (2026-10-18, current working input)

- Working input is plans/selinux-boundary-maintainer-20261018/working-source-files.json,82 files,20 paths over installed1792300213. Component build/verified03 and candidate1792311835 full34-partition/static preserve-data audits passed. Installed as1792311835 after the separately authorized preserve-data flash and first normal reboot. Automated50-file/21-range/19-contract, live labels/owners, maintainer resources and data-baseline readbacks passed. No Enforcing or owner UI acceptance yet. Authorization is consumed; no further reboot or mode switch implied. Preserve original nine static permissive domains/KSU and accepted payloads.
- Reviewed new upload paths are in provenance/boundary-maintainer-source-files.json. Only16 Apache-2.0 source snapshots plus minimal deltas/metadata are included. Continue omitting full file_contexts/vendor.prop and both owner payment images; no APK/policy/ROM/raw phone log in Git.
- Local maintainer name CyberWei, GitHub CyberWei922, QQ1121408238 and owner-supplied TG/PayPal links are integrated. No official badge or official-build designation. Images are separate pinned inputs. Static signature/idmap/image-byte checks do not establish UI interaction or payment success.
- No new phone collection, flash, reboot or setenforce is authorized by this source upload. Preserve prior ROMs pending new normal-use feedback. Leave unrelated Ubuntu tasks alone.

## Current combined build 1792300213 (installed, latest; 2026-10-18)

- The owner requested a larger combined batch before the next ROM. Working input is now plans/selinux-enforcing-prep-20261018/working-source-files.json,68 files;10 source paths differ from installed1792266978. The old64-file camera-qspm component was an intermediate build only, never a separate ROM. The old62-file map describes the installed baseline.
- Combined component build and verified01 passed: policy1eaa4c3e…/1,604,627 bytes; complete effective allow delta32 additions/0 removals, with contexts/neverallow/retained payload checks. Camera read-only/QSPM, Bluetooth true relocated vendor→system, QCC initialization, core ROM metadata read, thermal chown/fowner and exact status mapping are combined. Capabilities cannot be limited by path; no file setattr or DAC bypass was added. Actual system/vendor build.prop placement and absence from the other three partitions passed the full ROM check.
- Cumulative candidate1792300213 full build and actual34-partition audit are complete; both audit worker/status passed, and static preserve-data checks passed. ROM SHA3b1e518603f2c3d911d7ffbca886df4c97531f419ac733ab201673c166deddec, 4545439341 bytes, stays in Ubuntu delivery/ishtar-selinux-enforcing-prep-permissive-candidate-20261018-01. Mac has a thin handoff only. Whole OTA signature was not independently verified cryptographically. Installed with a newly authorized preserve-data flash and first normal reboot; no Enforcing result. Never replay intermediate workers or deleted old flash staging. Other Ubuntu tasks were not adjusted.
- Phone is now1792300213/Permissive after74 actions/73 downloads and wire verification. Postboot01/worker passed47 files,21 partition ranges,16 new policy contracts and the fresh data baseline;127 command attempts,0 retries. All finite workers ended. This flash/first reboot authorization is consumed; do not reboot or replay flash. No Enforcing trial or new owner functional acceptance is claimed. Initial MIUI process snapshot was absent; six default routes passed. Original nine static permissive domains plus ksu remain. Short Enforcing/cold-boot trials require separate concrete authorization.
- Reviewed upload paths also include provenance/enforcing-prep-source-files.json. Full stock-derived vendor.prop stays external; the plan provides a pinned single-line reproduction step. Keep new1792300213 and the two earlier rollback ROMs1792266978/1792261801 pending normal-use feedback; no cleanup was performed. Maintainer display is deferred to the next release until the owner supplies name/details. Request only directly affected manual checks; do not repeat microphone/fingerprint by default.

## License-scoped upload (2026-10-17)

- Read LICENSE.md, NOTICE and docs/LICENSE_AND_DISTRIBUTION.md. Preserve per-file upstream licenses; original maintenance additions use Apache-2.0.
- Only export reviewed paths in provenance/source-files.json, provenance/source-overrides.json, provenance/candidate-source-files.json, provenance/next-source-files.json, provenance/framework-api-source-files.json, provenance/vendor-library-source-files.json or provenance/dependency-status-source-files.json or provenance/device-io-source-files.json. Candidate inventories are separate from installed acceptance. Never classify arbitrary text as safe to upload.
- Original vendor text and derived MIUI implementation with unconfirmed redistribution permission stay external. Keep hashes/paths, not payloads. Preserve local frozen deliveries.
- Full source snapshots preserve build-source bytes. Adjacent .license notices and project patches describe modifications; do not copy .license sidecars into build rules.
- Read each plan's actual status. SELinux phases 3–5 are applied, compiled and installed as 1792246397; 29 files, 21 partition ranges, live labels and loaded policy are verified. Owner reported normal boot; camera/audio/fingerprint acceptance for this build remains pending. SELinux remains Permissive.
- The accepted source snapshots/tag still describe the previous accepted 1792226532 baseline. Apply the separately recorded cumulative candidate patch to reproduce the previous installed 1792246397 baseline sources; do not relabel old acceptance as current feature validation.
- Next-wave working source has 59 pinned files and a further 14-path patch on top of 1792246397. Its policy/helper/native preferred XML and full ROM 1792252694 are built and checked against all 34 actual payload partitions; 1792252694 is installed and owner-tested within the recorded scope, and no Enforcing trial exists. Do not use either older source map to validate the changed Ubuntu checkout. Respect concurrent Ubuntu work: no process termination; keep own build low priority; this completed build initially used two CPUs and expanded only its own descendants to four measured-idle CPUs for packaging.
- Current main/tag replace the previous single-commit snapshot after a local-only backup. Do not merge the excluded historical commit back into this repository.

## Post-install next batch (2026-10-18)

- Current working input is plans/selinux-dependency-status-20261018/working-source-files.json (59 files), superseding the camera-vendor map. Cumulative ROM 1792261801 passed full build, actual images and static upgrade checks, then its newly authorized preserve-data flash and first normal reboot. Postboot01 passed 39 file/21 partition-range readbacks, new library/RKP labels, six defaults and data baseline; 112 command attempts, no retries. Phone is 1792261801/Permissive, loaded policy 6b156e09… retains original nine static permissive domains plus ksu. All bounded workers ended. This flash/reboot authorization is consumed; no further reboot or Enforcing trial is implied. Owner confirmed the requested data, MIUI photo/new video sound and fingerprint checks normal; see the separate owner acceptance record. MIUI process was absent in the initial snapshot; do not claim its domain/fallback was runtime-tested. Secure-XDSP neverallow and generic-access denials remain.
- Owner latest camera followup: it seems fine again; keep current behavior and defer fluidity optimization. Do not change camera performance settings or claim intermittent dimming permanently repaired. No logger is running.

- Installed-baseline input is plans/selinux-camera-vendor-20261018/working-source-files.json (59 files). It includes framework API plus exact public-library labels and MIUI QSPM/DSP HAL clients. Final component policy 88ff29aa… is compiled and verified, and cumulative ROM 1792257806 passed full actual-image checks. Its authorized preserve-data flash and first normal reboot completed; 38 file and 21 partition-range readbacks, live labels, MIUI domain/defaults, DSP process and HIDL registration passed. Owner reports requested checks normal and keeps current camera behavior, deferring fluidity optimization. No Enforcing test. Original secure-XDSP neverallow and generic vendor access denial remain; actual HAL fallback is not proven.
- The earlier partial full build was stopped only within this task's verified process sessions after discovering the missing DSP client. Preserve its stop/lock-refusal evidence. Do not replay it or interpret its stale waiting status as an active worker. That completed 1792257806 build used control/ishtar-camera-dsp-client-20261018-01/rom-candidate01 on Ubuntu, CPU6/7/nice10/I/O7/Ninja2; leave unrelated tasks alone.

- Historical framework API component: its one-file app_api_service/radio_service find change is included in the latest cumulative plan and ROM. Its old 59-file map is an intermediate baseline; use the latest dependency-status plan map for the working tree.
- A 25-second marker-only detached watchdog dry run passed in the retained ksu domain; it never called setenforce. Actual restoration from Enforcing is unverified and needs new explicit trial authorization. No startup hook or module exists. Never reuse an expired run token or PID.
- Postboot01 stopped on ADB error: closed; postboot02 succeeded with 98 command attempts, no retries, 32 file and 21 partition-range hashes, labels, all six defaults, data baseline and loaded policy checked. Keep failed evidence. No original phone logs or policy binaries in this repository.

- Latest installed build is 1792261801, globally Permissive. Runtime evidence is summarized in the dependency-status plan. The flash/reboot authorization is consumed; no repeat reboot or Enforcing trial is implied. Keep complete ROMs 1792261801 and 1792257806 only; older ZIPs and both completed flash scratch directories were removed after owner acceptance. No active postboot worker remains. Mtd still has DAC and an exact attestation.status write denial; audit consumers without granting generic vendor_system_prop writes.

- Previous 1792257806 owner followup: other requested checks normal; camera UI/lens transitions felt sluggish and intermittent 5x dimming was reported. Controlled Mac wireless retest did not reproduce flicker, then the owner chose to keep current camera behavior and defer fluidity work. Do not claim the intermittent issue permanently fixed or caused by this SELinux patch. Both old bounded loggers ended. See plans/selinux-camera-vendor-20261018/camera-preview-observation.json; keep raw logs local. New 1792261801 owner checks are now confirmed within the requested scope; this does not prove all camera modes or Enforcing.

## Device-I/O next candidate (historical input)

- Working input is now plans/selinux-device-io-20261018/working-source-files.json, 62 files. The old 59-file dependency-status map describes installed 1792261801 only. Six paths add exact Mtd attestation status, GNSS sensor read, Goodix four private-netlink operations and one specific SPI wakeup label. Component policy1e158ee4… is compiled and independently verified; full ROM device-io-rom-build-01 has completed in the matching Ubuntu control/rom-candidate01; actual-image audit and corrected final preflight passed in rom-audit02. Preserve both checker-failure records. Do not replay old workers or alter unrelated Ubuntu tasks.
- Phone remains owner-tested1792261801/Permissive. No new phone capture, flash, reboot or Enforcing. Preserve original nine static permissive domains/KSU, neverallow, camera behavior and all accepted payloads. Mtd DAC/optional attestation reload are not claimed fixed by granting the exact status output. New candidate flash/reboot and Enforcing each need specific fresh authorization.

## Device-I/O complete candidate (latest)

- Complete candidate 1792266978 is built and archived, with 34 actual payload partitions, 62 pinned sources, exact policy and retained GNSS/Goodix/Mtd plus accepted payload checks passed. Static preserve-data upgrade checks passed; whole OTA signature was not independently verified cryptographically. See plans/selinux-device-io-20261018/rom-build-summary.json. All computer workers ended. Phone is still owner-tested1792261801/Permissive; this candidate is not installed and no Enforcing trial occurred. New flash/reboot and Enforcing require separate specific authorization. Keep both installed rollback ROMs until new owner checks; never replay deleted flash scratch.

## Owner acceptance scope preference (2026-10-18)

- Request manual checks only for features directly affected by the current build and a concrete remaining runtime risk. Do not repeat microphone tests when audio was unchanged and no new recording issue exists. Fingerprint rechecks need a related policy/driver/kernel or Enforcing change and a brief reason. Use automated byte/label/loaded-policy/data-baseline readback for checks it can cover. Do not repeat the previously accepted full hardware checklist for every Permissive package.

## 1792266978 installed and accepted (previous build)

- Phone is1792266978/Permissive. The newly authorized flash and first normal reboot completed with 74 operations/73 downloads, 44 file and21 partition-range matches, exact new loaded-policy contracts and SPI wakeup label verified. Owner confirmed the requested Google-account/data, MIUI photo/new video audio and fingerprint checks normal. Do not expand to GNSS performance/all modes or Enforcing. All finite workers ended; raw runtime24-item archive stays local.
- Complete ROMs retained:1792266978 and1792261801. Older1792257806 ZIP, completed own flash staging and duplicate output ZIP were removed after both-host evidence verification and owner checks. Deleted staging cannot be replayed; source/.repo/blobs/current build cache remain. Authorization is consumed; no extra reboot or Enforcing is implied.
- Initial ps snapshot lacked MIUI process, while startup AVC shows the dedicated MIUI domain. Keep those observations distinct. Audit-lost775 means no complete denial coverage. Mtd DAC, camera enumeration/other necessary-function denials and original permissive domains still require scoped work.
