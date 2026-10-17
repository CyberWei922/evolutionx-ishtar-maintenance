/*
 * Copyright (C) 2020 The Android Open Source Project
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
#include "linkerconfig/sectionbuilder.h"

#include <algorithm>

#include "linkerconfig/common.h"
#include "linkerconfig/environment.h"
#include "linkerconfig/log.h"
#include "linkerconfig/namespace.h"
#include "linkerconfig/namespacebuilder.h"
#include "linkerconfig/section.h"

namespace android {
namespace linkerconfig {
namespace contents {

using modules::LibProvider;
using modules::LibProviders;
using modules::Namespace;
using modules::Section;
using modules::SharedLibs;

namespace {
// :sphal requests a dynamic-loader namespace, not an ELF dependency or an
// ordinary link. Base sections already implement libvndksupport's exported
// sphal -> vendor -> default lookup. Recognize that existing contract without
// adding paths, visibility or library access. Explicit APEX providers keep
// their existing namespace-creation policy.
void RegisterExistingSphalProvider(const Context& ctx,
                                   const std::vector<Namespace>& namespaces,
                                   LibProviders& providers) {
  if (!ctx.IsDefaultConfig() || providers.count(":sphal") != 0) {
    return;
  }
  bool required = std::any_of(namespaces.begin(), namespaces.end(),
                              [](const auto& ns) {
                                return ns.GetRequires().count(":sphal") != 0;
                              });
  if (!required) {
    return;
  }

  std::vector<std::string> targets;
  for (const char* name : {"sphal", "vendor", "default"}) {
    auto it = std::find_if(namespaces.begin(), namespaces.end(),
                           [name](const auto& ns) {
                             return ns.GetName() == name && ns.IsVisible();
                           });
    if (it != namespaces.end()) {
      targets.push_back(it->GetName());
      break;
    }
  }

  if (targets.empty()) {
    // With no exporter, libvndksupport falls back to plain dlopen. Its caller
    // address can belong to the helper or, after a tail call, to its caller.
    // Record the already-existing helper and requirement-consumer namespaces
    // as namespace-only providers; do not invent an exported default or choose
    // one static dlopen scope. Each consumer must require the unique external
    // system helper. No shared libraries or runtime links are granted here.
    const Namespace* loader = nullptr;
    for (const auto& ns : namespaces) {
      if (ns.GetProvides().count("libvndksupport.so") != 0) {
        if (loader != nullptr) {
          return;
        }
        loader = &ns;
      }
    }
    if (loader == nullptr || loader->GetName() != ctx.GetSystemNamespaceName() ||
        !loader->GetApexSource().name.empty()) {
      return;
    }
    for (const auto& ns : namespaces) {
      if (ns.GetRequires().count(":sphal") != 0 &&
          (ns.GetRequires().count("libvndksupport.so") == 0 ||
           ns.GetProvides().count("libvndksupport.so") != 0)) {
        return;
      }
    }
    targets.push_back(loader->GetName());
    for (const auto& ns : namespaces) {
      if (ns.GetRequires().count(":sphal") != 0 &&
          ns.GetName() != loader->GetName()) {
        targets.push_back(ns.GetName());
      }
    }
  }

  for (const auto& target : targets) {
    providers[":sphal"].push_back(LibProvider{
        target,
        [target]() {
          // Resolve must reuse the existing namespace validated above.
          LOG(FATAL) << "SPHAL contract lost its existing namespace: " << target;
          return Namespace(target);
        },
        SharedLibs{{}},
    });
  }
}
}  // namespace

Section BuildSection(const Context& ctx, const std::string& name,
                     std::vector<Namespace>&& namespaces,
                     const std::set<std::string>& visible_apexes,
                     const LibProviders& providers) {
  // add additional visible APEX namespaces
  for (const auto& apex : ctx.GetApexModules()) {
    if (visible_apexes.find(apex.name) == visible_apexes.end() &&
        !apex.visible) {
      continue;
    }
    if (auto it = std::find_if(
            namespaces.begin(),
            namespaces.end(),
            [&apex](auto& ns) { return ns.GetName() == apex.namespace_name; });
        it == namespaces.end()) {
      auto ns = ctx.BuildApexNamespace(apex, true);
      namespaces.push_back(std::move(ns));
    } else {
      // override "visible" when the apex is already created
      it->SetVisible(true);
    }
  }

  // libdexfile support first probes an already-loaded debug library and falls
  // back to the required release library. Keep that optional link when the
  // activated ART APEX exports it, including after an APEX update, without
  // making a release-only ART APEX require a nonexistent debug library.
  // This explicit alias is not a filter for any ordinary library requirement.
  CHECK(providers.find(":optional-art-debug") == providers.end());
  LibProviders providers_with_art_debug = providers;
  RegisterExistingSphalProvider(ctx, namespaces, providers_with_art_debug);
  auto& art_debug = providers_with_art_debug[":optional-art-debug"];
  for (const auto& apex : ctx.GetApexModules()) {
    if (apex.name == "com.android.art" &&
        std::find(apex.provide_libs.begin(), apex.provide_libs.end(),
                  "libdexfiled.so") != apex.provide_libs.end()) {
      art_debug.push_back(LibProvider{
          apex.namespace_name,
          [&ctx, apex]() { return ctx.BuildApexNamespace(apex, false); },
          SharedLibs{{"libdexfiled.so"}},
      });
    }
  }

  // resolve provide/require constraints; all ordinary missing dependencies
  // retain the existing strict failure behavior.
  Section section(std::move(name), std::move(namespaces));
  section.Resolve(ctx, providers_with_art_debug);

  AddStandardSystemLinks(ctx, &section);
  return section;
}

std::vector<LibProvider> GetVndkProvider(const Context& ctx,
                                         VndkUserPartition partition) {
  std::vector<LibProvider> provider;
  std::string partition_suffix =
      partition == VndkUserPartition::Vendor ? "VENDOR" : "PRODUCT";
  provider.push_back(LibProvider{
      "vndk",
      std::bind(BuildVndkNamespace, ctx, partition),
      SharedLibs{
          std::vector{Var("VNDK_SAMEPROCESS_LIBRARIES_" + partition_suffix),
                      Var("VNDK_CORE_LIBRARIES_" + partition_suffix)}},
  });
  if (modules::IsVndkInSystemNamespace()) {
    provider.push_back(LibProvider{
        "vndk_in_system",
        std::bind(BuildVndkInSystemNamespace, ctx),
        SharedLibs{std::vector{Var("VNDK_USING_CORE_VARIANT_LIBRARIES")}},
    });
  }
  return provider;
}

}  // namespace contents
}  // namespace linkerconfig
}  // namespace android
