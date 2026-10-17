/* Copyright (C) 2026 CyberWei922; SPDX-License-Identifier: Apache-2.0 */
package org.lineageos.ishtar.cameradefaults;

import java.util.HashSet;
import java.util.Set;

/** Host-only behavioral tests; never launches or changes a phone application. */
public final class CameraDefaultsPolicyTest {
    static final class Fake implements CameraDefaultsPolicy.Backend {
        Set<String> done = new HashSet<>();
        String currentPackage = "android";
        boolean available = true;
        boolean effective = true;
        int writes;
        int logs;
        Set<String> applied = new HashSet<>();
        public boolean completed(CameraDefaultsPolicy.Route r) { return done.contains(r.action); }
        public boolean targetAvailable(CameraDefaultsPolicy.Route r) { return available; }
        public String resolvedComponent(CameraDefaultsPolicy.Route r) {
            return applied.contains(r.action) ? r.component : currentPackage + "/.Camera";
        }
        public String resolvedPackage(CameraDefaultsPolicy.Route r) { return currentPackage; }
        public void prefer(CameraDefaultsPolicy.Route r) {
            writes++;
            if (effective) applied.add(r.action);
        }
        public void complete(CameraDefaultsPolicy.Route r) { done.add(r.action); }
        public void log(CameraDefaultsPolicy.Route r, String result) { logs++; }
    }
    static void check(boolean value, String name) {
        if (!value) throw new AssertionError(name);
    }
    public static void main(String[] args) {
        Fake chooser = new Fake();
        CameraDefaultsPolicy.migrate(chooser);
        check(chooser.writes == 6 && chooser.done.size() == 6, "chooser -> all six defaults");
        CameraDefaultsPolicy.migrate(chooser);
        check(chooser.writes == 6, "second boot changes nothing");
        chooser.currentPackage = "third.party.camera";
        chooser.applied.clear();
        CameraDefaultsPolicy.migrate(chooser);
        check(chooser.writes == 6, "later owner choice preserved");

        Fake aperture = new Fake(); aperture.currentPackage = CameraDefaultsPolicy.APERTURE_PACKAGE;
        CameraDefaultsPolicy.migrate(aperture);
        check(aperture.writes == 6 && aperture.done.size() == 6, "old Aperture choices migrate");

        Fake thirdParty = new Fake(); thirdParty.currentPackage = "third.party.camera";
        CameraDefaultsPolicy.migrate(thirdParty);
        check(thirdParty.writes == 0 && thirdParty.done.size() == 6, "explicit third party retained");

        Fake unavailable = new Fake(); unavailable.available = false;
        CameraDefaultsPolicy.migrate(unavailable);
        check(unavailable.writes == 0 && unavailable.done.isEmpty(), "missing/disabled target retryable");
        unavailable.available = true;
        CameraDefaultsPolicy.migrate(unavailable);
        check(unavailable.writes == 6 && unavailable.done.size() == 6, "target restored later");

        Fake failure = new Fake(); failure.effective = false;
        CameraDefaultsPolicy.migrate(failure);
        check(failure.done.isEmpty(), "do not record success when preference fails");
        failure.effective = true;
        CameraDefaultsPolicy.migrate(failure);
        check(failure.done.size() == 6, "unconfirmed actions retry");

        Fake partial = new Fake(); partial.done.add(CameraDefaultsPolicy.ROUTES[0].action);
        CameraDefaultsPolicy.migrate(partial);
        check(partial.writes == 5 && partial.done.size() == 6, "partial transaction resumes only unfinished actions");

        Fake already = new Fake();
        for (CameraDefaultsPolicy.Route r : CameraDefaultsPolicy.ROUTES) already.applied.add(r.action);
        CameraDefaultsPolicy.migrate(already);
        check(already.writes == 0 && already.done.size() == 6, "already correct -> no PM writes");

        Set<String> names = new HashSet<>();
        for (CameraDefaultsPolicy.Route r : CameraDefaultsPolicy.ROUTES) {
            check(names.add(r.action), "unique action");
            check(r.component.startsWith("com.android.camera/com.android.camera."), "explicit MIUI component");
        }
        check(CameraDefaultsPolicy.ROUTES[1].component.endsWith(".Camera"), "secure action selects dedicated activity");
        check(CameraDefaultsPolicy.ROUTES[2].component.endsWith(".OneShotImageCapture"), "capture retains output contract");
        check(CameraDefaultsPolicy.ROUTES[5].component.endsWith(".OneShotVideoCapture"), "video capture retains output contract");
        System.out.println("PASS: 11 migration checks, three route-contract checks and six unique MIUI routes");
    }
}
