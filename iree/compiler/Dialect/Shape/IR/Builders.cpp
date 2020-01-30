// Copyright 2020 Google LLC
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

#include "iree/compiler/Dialect/Shape/IR/Builders.h"

#include "iree/compiler/Dialect/Shape/IR/ShapeOps.h"
#include "iree/compiler/Dialect/Shape/IR/ShapeTypes.h"

namespace mlir {
namespace iree_compiler {
namespace Shape {

Value buildCastInputsToResultShape(Location loc,
                                   RankedShapeType resultShapeType,
                                   ArrayRef<Value> inputs, OpBuilder &builder) {
  llvm::SmallVector<Value, 4> inputShapes;
  for (auto inputOperand : inputs) {
    auto inputOperandType = inputOperand.getType().dyn_cast<RankedTensorType>();
    RankedShapeType inputOperandShape = RankedShapeType::getChecked(
        inputOperandType.getShape(), resultShapeType.getDimType(),
        inputOperand.getLoc());
    if (!inputOperandShape) return nullptr;

    inputShapes.push_back(
        builder.create<GetRankedShapeOp>(loc, inputOperandShape, inputOperand));
  }

  // Assert compatible.
  return builder.create<CastCompatibleShapeOp>(loc, resultShapeType,
                                               inputShapes);
}

}  // namespace Shape
}  // namespace iree_compiler
}  // namespace mlir