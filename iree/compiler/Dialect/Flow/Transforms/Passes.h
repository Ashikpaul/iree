// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IREE_COMPILER_DIALECT_FLOW_TRANSFORMS_PASSES_H_
#define IREE_COMPILER_DIALECT_FLOW_TRANSFORMS_PASSES_H_

#include "iree/compiler/Dialect/Flow/IR/FlowOps.h"
#include "llvm/ADT/StringMap.h"
#include "mlir/IR/Function.h"
#include "mlir/IR/Module.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Support/LLVM.h"

namespace mlir {
namespace iree_compiler {
namespace IREE {
namespace Flow {

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

// Adds a set of passes to the given pass manager that run the required flow
// transforms in the canonical order.
//
// Most translation code should prefer to use this instead of manually adding
// the passes themselves to ensure that expected pass ordering is observed.
//
// The expected usage is:
//   <run conversion from TF/HLO/etc to flow>
//   buildFlowTransformPassPipeline & run
//   <run conversion from flow to sequencer/hal/vm/etc>
void buildFlowTransformPassPipeline(OpPassManager &passManager);

//===----------------------------------------------------------------------===//
// Input canonicalization and legalization
//===----------------------------------------------------------------------===//

// Flattens tuple values in function signatures and blocks.
std::unique_ptr<OpPassBase<ModuleOp>> createFlattenTuplesInCFGPass();

//===----------------------------------------------------------------------===//
// Dispatches (flow.dispatch.region)
//===----------------------------------------------------------------------===//

// Analyzes a module to identify which functions are dispatchable.
// This information is cached on the module and is used by other FuncOp-scoped
// passes to quickly access the module-level dispatchability information.
std::unique_ptr<OpPassBase<ModuleOp>> createDispatchabilityAnalysisPass(
    std::shared_ptr<llvm::StringMap<FuncOp>> dispatchableFuncOps);

// Identifies dispatchable regions of functions and wraps them in
// flow.dispatch_regions.
std::unique_ptr<OpPassBase<FuncOp>> createIdentifyDispatchRegionsPass();

// Folds multiple dispatch regions together that have compatible workloads.
std::unique_ptr<OpPassBase<FuncOp>> createFoldCompatibleDispatchRegionsPass();

// Rematerializes small previously-CSE'd constants into dispatch regions.
std::unique_ptr<OpPassBase<FuncOp>> createRematerializeDispatchConstantsPass();

// Outlines dispatch regions into executables.
std::unique_ptr<OpPassBase<ModuleOp>> createOutlineDispatchRegionsPass(
    std::shared_ptr<llvm::StringMap<FuncOp>> dispatchableFuncOps);

//===----------------------------------------------------------------------===//
// Reductions (flow.reduction.region)
//===----------------------------------------------------------------------===//

// Identifies reduction regions and wraps them in flow.reduction_regions.
std::unique_ptr<OpPassBase<ModuleOp>> createIdentifyReductionRegionsPass();

// Outlines dispatch regions into executables.
std::unique_ptr<OpPassBase<ModuleOp>> createOutlineReductionRegionsPass(
    std::shared_ptr<llvm::StringMap<FuncOp>> dispatchableFuncOps);

//===----------------------------------------------------------------------===//
// Optimizations
//===----------------------------------------------------------------------===//

// TODO(benvanik): pass to dedupe similar executables (by making dynamically
// shaped, adjusting types, etc).

//===----------------------------------------------------------------------===//
// Module Analysis and Finalization
//===----------------------------------------------------------------------===//

// Assigns workload attributes to executable entry points based on dispatches.
std::unique_ptr<OpPassBase<ModuleOp>> createAssignExecutableWorkloadsPass();

}  // namespace Flow
}  // namespace IREE
}  // namespace iree_compiler
}  // namespace mlir

#endif  // IREE_COMPILER_DIALECT_FLOW_TRANSFORMS_PASSES_H_