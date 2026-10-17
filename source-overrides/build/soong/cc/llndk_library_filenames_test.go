// Copyright 2026 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package cc

import (
	"testing"

	"android/soong/android"
)

func TestMovedToApexLlndkLibraryFilenames(t *testing.T) {
	result := android.GroupFixturePreparers(
		prepareForCcTest,
		android.PrepareForTestAccessingMakeVars,
		android.FixtureRegisterWithContext(func(ctx android.RegistrationContext) {
			ctx.RegisterParallelSingletonType("movedToApexLlndkLibraries", movedToApexLlndkLibrariesFactory)
		}),
	).RunTestWithBp(t, `
		cc_library_shared {
			name: "librenamed",
			compile_multilib: "64",
			host_supported: true,
			stem: "libreal-architecture-name",
			target: {
				host: { stem: "libhost-only-name" },
			},
			llndk: {
				symbol_file: "libfoo.map.txt",
				moved_to_apex: true,
			},
		}
		cc_library_shared {
			name: "libplain",
			compile_multilib: "64",
			llndk: {
				symbol_file: "libfoo.map.txt",
				moved_to_apex: true,
			},
		}
		cc_library_shared {
			name: "libnotmoved",
			llndk: { symbol_file: "libfoo.map.txt" },
		}
	`)
	singleton := result.SingletonForTests(t, "movedToApexLlndkLibraries").Singleton().(*movedToApexLlndkLibraries)
	android.AssertDeepEquals(t, "legacy module names retained",
		[]string{"libplain", "librenamed"}, singleton.movedToApexLlndkLibraries)
	android.AssertDeepEquals(t, "device implementation filenames, not module or host names",
		[]string{"libplain.so", "libreal-architecture-name.so"}, singleton.movedToApexLlndkLibraryFilenames)
	output := result.SingletonForTests(t, "movedToApexLlndkLibraries").Output(".intermediates/moved_to_apex_llndk_libraries.txt")
	android.AssertStringEquals(t, "filesystem linkerconfig input uses real filenames",
		"libplain.so libreal-architecture-name.so\n",
		android.ContentFromFileRuleForTests(t, result.TestContext, output))
	vars := result.MakeVarsForTesting(t, func(variable android.MakeVarVariable) bool {
		return variable.Name() == "LLNDK_MOVED_TO_APEX_LIBRARY_FILENAMES"
	})
	if len(vars) != 1 {
		t.Fatalf("expected one filename Make variable, got %d", len(vars))
	}
	android.AssertStringEquals(t, "Make linkerconfig input uses real filenames",
		"libplain.so libreal-architecture-name.so", vars[0].Value())
}
