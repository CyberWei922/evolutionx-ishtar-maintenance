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

#include <algorithm>
#include <csignal>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "linkerconfig/apexconfig.h"
#include "linkerconfig/configwriter.h"
#include "linkerconfig/sectionbuilder.h"
#include "mockenv.h"

using namespace android::linkerconfig::contents;
using namespace android::linkerconfig::modules;

namespace {
ApexInfo MediaApex(std::vector<std::string> required_libs =
                       {":sphal", "libvndksupport.so"}) {
  return ApexInfo("com.android.media", "/apex/com.android.media", {},
                  std::move(required_libs), {}, {}, true, true, true, false);
}

struct SphalContract : testing::Test {
  Context ctx;

  void SetUp() override {
    MockGenericVariables();
    Variables::AddValue("ro.vndk.version", "");
    Variables::AddValue("ro.product.vndk.version", "");
    ctx.SetStrictMode(true);
    ctx.SetApexModules({MediaApex()});
    android::linkerconfig::proto::LinkerConfig system;
    for (const char* lib : {"llndk_libraries", "libft2.so", "libvndksupport.so"}) {
      system.add_providelibs(lib);
    }
    ctx.SetSystemConfig(system);
  }
};

bool HasLink(const Namespace& ns, const std::string& name) {
  return std::any_of(ns.Links().begin(), ns.Links().end(),
                     [&name](const auto& link) { return link.To() == name; });
}

size_t NamespaceCount(Section& section, const std::string& name) {
  size_t count = 0;
  section.ForEachNamespaces([&](const auto& ns) {
    if (ns.GetName() == name) ++count;
  });
  return count;
}

Section FocusedSection(Context& ctx, bool sphal_visible, bool vendor_visible,
                       bool default_visible, bool system_loader,
                       std::vector<std::string> required_libs =
                           {":sphal", "libvndksupport.so"},
                       const LibProviders& providers = {}) {
  ctx.SetCurrentSection(SectionType::Other);
  ctx.SetApexModules({});
  std::vector<Namespace> namespaces;
  namespaces.emplace_back("default", true, default_visible);
  if (sphal_visible) namespaces.emplace_back("sphal", true, true);
  if (vendor_visible) namespaces.emplace_back("vendor", true, true);
  namespaces.emplace_back("system");
  if (system_loader) {
    namespaces.back().AddProvides(std::vector<std::string>{"libvndksupport.so"});
  }
  namespaces.emplace_back("consumer").AddRequires(required_libs);
  return BuildSection(ctx, "focused", std::move(namespaces), {}, providers);
}
}  // namespace

TEST_F(SphalContract, system_reuses_visible_sphal_without_adding_access) {
  auto section = BuildSystemSection(ctx);
  ASSERT_NE(nullptr, section.GetNamespace("sphal"));
  EXPECT_TRUE(section.GetNamespace("sphal")->IsVisible());
  EXPECT_EQ(1u, NamespaceCount(section, "sphal"));
  EXPECT_TRUE(HasLink(*section.GetNamespace("com_android_media"), "sphal"));
  EXPECT_TRUE(section.GetNamespace("com_android_media")->GetLink("sphal").Empty());
  EXPECT_FALSE(HasLink(*section.GetNamespace("default"), "sphal"));
}

TEST_F(SphalContract, unrestricted_reuses_existing_sphal) {
  auto section = BuildUnrestrictedSection(ctx);
  ASSERT_NE(nullptr, section.GetNamespace("sphal"));
  EXPECT_EQ(1u, NamespaceCount(section, "sphal"));
  EXPECT_TRUE(section.GetNamespace("com_android_media")->GetLink("sphal").Empty());
}

TEST_F(SphalContract, vendor_uses_its_exported_default_not_a_second_vendor_namespace) {
  auto section = BuildVendorSection(ctx);
  EXPECT_TRUE(section.GetNamespace("default")->IsVisible());
  EXPECT_EQ(nullptr, section.GetNamespace("sphal"));
  EXPECT_EQ(nullptr, section.GetNamespace("vendor"));
  EXPECT_TRUE(HasLink(*section.GetNamespace("com_android_media"), "default"));
  EXPECT_TRUE(section.GetNamespace("com_android_media")->GetLink("default").Empty());
}

TEST_F(SphalContract, product_preserves_actual_default_fallback_and_no_vendor_access) {
  auto section = BuildProductSection(ctx);
  EXPECT_TRUE(section.GetNamespace("default")->IsVisible());
  EXPECT_EQ(nullptr, section.GetNamespace("sphal"));
  EXPECT_EQ(nullptr, section.GetNamespace("vendor"));
  EXPECT_THAT(section.GetNamespace("default")->SearchPaths(),
              testing::ElementsAre("product/${LIB}"));
  EXPECT_TRUE(section.GetNamespace("com_android_media")->GetLink("default").Empty());
}

TEST_F(SphalContract, isolated_preserves_helper_and_caller_fallback_without_exporter) {
  auto section = BuildIsolatedSection(ctx);
  EXPECT_FALSE(section.GetNamespace("default")->IsVisible());
  EXPECT_EQ(nullptr, section.GetNamespace("sphal"));
  EXPECT_EQ(nullptr, section.GetNamespace("vendor"));
  EXPECT_FALSE(HasLink(*section.GetNamespace("com_android_media"), "default"));
  EXPECT_THAT(section.GetNamespace("com_android_media")
                  ->GetLink("system").GetSharedLibs(),
              testing::Contains("libvndksupport.so"));
  EXPECT_TRUE(HasLink(*section.GetNamespace("com_android_media"), "com_android_media"));
  EXPECT_TRUE(section.GetNamespace("com_android_media")
                  ->GetLink("com_android_media").Empty());
  EXPECT_TRUE(section.GetNamespace("default")->SearchPaths().empty());
  EXPECT_TRUE(section.GetNamespace("default")->PermittedPaths().empty());
}

TEST_F(SphalContract, exported_lookup_prefers_sphal_before_vendor_and_default) {
  auto section = FocusedSection(ctx, true, true, true, true);
  EXPECT_TRUE(HasLink(*section.GetNamespace("consumer"), "sphal"));
  EXPECT_FALSE(HasLink(*section.GetNamespace("consumer"), "vendor"));
  EXPECT_FALSE(HasLink(*section.GetNamespace("consumer"), "default"));
}

TEST_F(SphalContract, exported_lookup_prefers_vendor_before_default) {
  auto section = FocusedSection(ctx, false, true, true, true);
  EXPECT_TRUE(HasLink(*section.GetNamespace("consumer"), "vendor"));
  EXPECT_FALSE(HasLink(*section.GetNamespace("consumer"), "default"));
}

TEST_F(SphalContract, no_marker_adds_no_namespace_only_link) {
  auto section = FocusedSection(ctx, true, true, true, true, {"libvndksupport.so"});
  EXPECT_FALSE(HasLink(*section.GetNamespace("consumer"), "sphal"));
  EXPECT_FALSE(HasLink(*section.GetNamespace("consumer"), "vendor"));
  EXPECT_FALSE(HasLink(*section.GetNamespace("consumer"), "default"));
}

TEST_F(SphalContract, invisible_default_without_loader_rejects_marker) {
  ASSERT_EXIT(FocusedSection(ctx, false, false, false, false),
              testing::KilledBySignal(SIGABRT),
              "not found: :sphal is required by consumer");
}

TEST_F(SphalContract, invisible_sphal_does_not_count_as_an_exporter) {
  ctx.SetCurrentSection(SectionType::Other);
  ctx.SetApexModules({});
  std::vector<Namespace> namespaces;
  namespaces.emplace_back("default", true, false);
  namespaces.emplace_back("sphal", true, false);
  namespaces.emplace_back("consumer").AddRequires(std::vector<std::string>{":sphal"});
  ASSERT_EXIT(BuildSection(ctx, "focused", std::move(namespaces), {}),
              testing::KilledBySignal(SIGABRT),
              "not found: :sphal is required by consumer");
}

TEST_F(SphalContract, fallback_without_hard_loader_dependency_rejects_marker) {
  ASSERT_EXIT(FocusedSection(ctx, false, false, false, true, {":sphal"}),
              testing::KilledBySignal(SIGABRT),
              "not found: :sphal is required by consumer");
}

TEST_F(SphalContract, fallback_in_a_non_system_provider_is_not_assumed) {
  ctx.SetCurrentSection(SectionType::Other);
  ctx.SetApexModules({});
  std::vector<Namespace> namespaces;
  namespaces.emplace_back("default", true, false);
  namespaces.emplace_back("another_loader")
      .AddProvides(std::vector<std::string>{"libvndksupport.so"});
  namespaces.emplace_back("consumer")
      .AddRequires(std::vector<std::string>{":sphal", "libvndksupport.so"});
  ASSERT_EXIT(BuildSection(ctx, "focused", std::move(namespaces), {}),
              testing::KilledBySignal(SIGABRT),
              "not found: :sphal is required by consumer");
}

TEST_F(SphalContract, fallback_consumers_gain_no_nonempty_cross_namespace_links) {
  ctx.SetCurrentSection(SectionType::Other);
  ctx.SetApexModules({});
  std::vector<Namespace> namespaces;
  namespaces.emplace_back("default", true, false);
  namespaces.emplace_back("system")
      .AddProvides(std::vector<std::string>{"libvndksupport.so"});
  for (const char* name : {"consumer", "other_consumer"}) {
    namespaces.emplace_back(name)
        .AddRequires(std::vector<std::string>{":sphal", "libvndksupport.so"});
  }
  auto section = BuildSection(ctx, "focused", std::move(namespaces), {});
  EXPECT_EQ(1u, NamespaceCount(section, "system"));
  for (const char* name : {"consumer", "other_consumer"}) {
    EXPECT_TRUE(section.GetNamespace("consumer")->GetLink(name).Empty());
    EXPECT_TRUE(section.GetNamespace("other_consumer")->GetLink(name).Empty());
  }
  EXPECT_EQ(nullptr, section.GetNamespace("sphal"));
  EXPECT_EQ(nullptr, section.GetNamespace("vendor"));
}

TEST_F(SphalContract, ambiguous_loader_provider_remains_strictly_rejected) {
  ctx.SetCurrentSection(SectionType::Other);
  ctx.SetApexModules({});
  std::vector<Namespace> namespaces;
  namespaces.emplace_back("default", true, false);
  for (const char* name : {"system", "other_loader"}) {
    namespaces.emplace_back(name)
        .AddProvides(std::vector<std::string>{"libvndksupport.so"});
  }
  namespaces.emplace_back("consumer")
      .AddRequires(std::vector<std::string>{":sphal", "libvndksupport.so"});
  ASSERT_EXIT(BuildSection(ctx, "focused", std::move(namespaces), {}),
              testing::KilledBySignal(SIGABRT),
              "duplicate: libvndksupport\\.so is provided by system and other_loader");
}

TEST_F(SphalContract, ordinary_required_library_still_fails_strict) {
  ASSERT_EXIT(FocusedSection(ctx, true, false, true, true,
                            {":sphal", "libvndksupport.so", "missing.so"}),
              testing::KilledBySignal(SIGABRT),
              "not found: missing\\.so is required by consumer");
}

TEST_F(SphalContract, unknown_namespace_marker_still_fails_strict) {
  ASSERT_EXIT(FocusedSection(ctx, true, false, true, true,
                            {":sphal", ":unknown", "libvndksupport.so"}),
              testing::KilledBySignal(SIGABRT),
              "not found: :unknown is required by consumer");
}

TEST_F(SphalContract, explicit_provider_takes_precedence_over_base_fallback) {
  LibProviders providers;
  providers[":sphal"] = {LibProvider{
      "explicit_sphal", []() { return Namespace("explicit_sphal", true, true); },
      SharedLibs{{}},
  }};
  auto section = FocusedSection(ctx, true, true, true, true,
                                {":sphal", "libvndksupport.so"}, providers);
  EXPECT_NE(nullptr, section.GetNamespace("explicit_sphal"));
  EXPECT_TRUE(HasLink(*section.GetNamespace("consumer"), "explicit_sphal"));
  EXPECT_FALSE(HasLink(*section.GetNamespace("consumer"), "sphal"));
}

TEST_F(SphalContract, legacy_does_not_inherit_an_implicit_base_contract) {
  ctx.SetCurrentLinkerConfigType(LinkerConfigType::Legacy);
  ASSERT_EXIT(FocusedSection(ctx, true, true, true, true),
              testing::KilledBySignal(SIGABRT),
              "not found: :sphal is required by consumer");
}

TEST_F(SphalContract, system_apex_keeps_existing_explicit_sphal_creation) {
  auto apex = MediaApex();
  auto config = CreateApexConfiguration(ctx, apex);
  auto* section = config.GetSection(apex.name);
  ASSERT_NE(nullptr, section);
  EXPECT_NE(nullptr, section->GetNamespace("sphal"));
  EXPECT_EQ(nullptr, section->GetNamespace("vendor"));
}

TEST_F(SphalContract, vendor_apex_keeps_existing_vendor_not_sphal_policy) {
  Variables::AddValue("SANITIZER_LIBRARIES_LLNDK", "sanitizer_llndk_libraries");
  auto apex = MediaApex();
  apex.partition = "VENDOR";
  ctx.SetApexModules({apex});
  auto config = CreateApexConfiguration(ctx, apex);
  auto* section = config.GetSection(apex.name);
  ASSERT_NE(nullptr, section);
  EXPECT_NE(nullptr, section->GetNamespace("vendor"));
  EXPECT_EQ(nullptr, section->GetNamespace("sphal"));
}
