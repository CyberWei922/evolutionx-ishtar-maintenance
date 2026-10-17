# Component recipes

- recovery/: exact modified initialization rc and its patch, with the original BSD-style grant preserved. Recovery kernel, modules, firmware and image remain external dependencies and require their own license/source review.
- brightness/: original four-parameter smoothing recipe and SHA-gated generator for separately obtained XML inputs. Full vendor XMLs are not uploaded without a verified redistribution grant.
- miui-camera03/: factual integration notes only. Derived MIUI code/patch fragments and the APK remain in the local accepted handoff, outside this Git history.

Open-source project changes are in patches/, exact licensed modified files in source-files/, and newly authored integration/SELinux files in source-overrides/.
