/*
 * Copyright (C) 2026 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <csignal>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "linkerconfig/sectionbuilder.h"
#include "mockenv.h"

using android::linkerconfig::contents::BuildSection;
using android::linkerconfig::contents::Context;
using android::linkerconfig::modules::ApexInfo;
using android::linkerconfig::modules::Namespace;
using android::linkerconfig::modules::Section;

namespace {
ApexInfo ArtApex(std::vector<std::string> exports) {
  return ApexInfo("com.android.art", "/apex/com.android.art",
                  std::move(exports), {}, {}, {}, true, true, false, false);
}

Section BuildStrictSection(Context& ctx,
                           const std::vector<std::string>& required_libs) {
  MockGenericVariables();
  ctx.SetStrictMode(true);
  std::vector<Namespace> namespaces;
  namespaces.emplace_back("default").AddRequires(required_libs);
  return BuildSection(ctx, "system", std::move(namespaces), {});
}
}  // namespace

TEST(linkerconfig_art_debug, release_only_art_keeps_required_release_link) {
  Context ctx;
  ctx.SetApexModules({ArtApex({"libdexfile.so"})});
  auto section = BuildStrictSection(
      ctx, {"libdexfile.so", ":optional-art-debug"});
  EXPECT_THAT(section.GetNamespace("default")
                  ->GetLink("com_android_art").GetSharedLibs(),
              testing::ContainerEq(std::vector<std::string>{"libdexfile.so"}));
}

TEST(linkerconfig_art_debug, debug_export_adds_only_the_debug_probe_link) {
  Context ctx;
  ctx.SetApexModules({ArtApex({"libdexfile.so", "libdexfiled.so"})});
  auto section = BuildStrictSection(
      ctx, {"libdexfile.so", ":optional-art-debug"});
  EXPECT_THAT(section.GetNamespace("default")
                  ->GetLink("com_android_art").GetSharedLibs(),
              testing::UnorderedElementsAre("libdexfile.so", "libdexfiled.so"));
}

TEST(linkerconfig_art_debug, no_alias_does_not_add_debug_link) {
  Context ctx;
  ctx.SetApexModules({ArtApex({"libdexfile.so", "libdexfiled.so"})});
  auto section = BuildStrictSection(ctx, {"libdexfile.so"});
  EXPECT_THAT(section.GetNamespace("default")
                  ->GetLink("com_android_art").GetSharedLibs(),
              testing::ContainerEq(std::vector<std::string>{"libdexfile.so"}));
}

TEST(linkerconfig_art_debug, unrelated_apex_cannot_supply_optional_art_probe) {
  Context ctx;
  ctx.SetApexModules({ArtApex({"libdexfile.so"}),
                     ApexInfo("com.other.debug", "/apex/com.other.debug",
                              {"libdexfiled.so"}, {}, {}, {},
                              true, true, false, false)});
  auto section = BuildStrictSection(
      ctx, {"libdexfile.so", ":optional-art-debug"});
  EXPECT_EQ(nullptr, section.GetNamespace("com_other_debug"));
  EXPECT_THAT(section.GetNamespace("default")
                  ->GetLink("com_android_art").GetSharedLibs(),
              testing::ContainerEq(std::vector<std::string>{"libdexfile.so"}));
}

TEST(linkerconfig_art_debug, missing_required_release_still_fails_strict) {
  Context ctx;
  ctx.SetApexModules({ArtApex({"libdexfiled.so"})});
  ASSERT_EXIT(BuildStrictSection(ctx, {"libdexfile.so", ":optional-art-debug"}),
              testing::KilledBySignal(SIGABRT),
              "not found: libdexfile\\.so is required by default");
}

TEST(linkerconfig_art_debug, explicit_debug_requirement_still_fails_strict) {
  Context ctx;
  ctx.SetApexModules({ArtApex({"libdexfile.so"})});
  ASSERT_EXIT(BuildStrictSection(ctx, {"libdexfiled.so", ":optional-art-debug"}),
              testing::KilledBySignal(SIGABRT),
              "not found: libdexfiled\\.so is required by default");
}

TEST(linkerconfig_art_debug, unrelated_missing_requirement_still_fails_strict) {
  Context ctx;
  ctx.SetApexModules({ArtApex({"libdexfile.so"})});
  ASSERT_EXIT(BuildStrictSection(ctx, {"missing.so", ":optional-art-debug"}),
              testing::KilledBySignal(SIGABRT),
              "not found: missing\\.so is required by default");
}
