//===--- CGStmtApprox.cpp - Emit LLVM Code from Statements ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This contains code to emit Approx nodes as LLVM code.
//
//===----------------------------------------------------------------------===//

#include "CodeGenFunction.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtApprox.h"
#include "clang/Basic/Approx.h"
#include "llvm/Support/Debug.h"

using namespace clang;
using namespace CodeGen;
using namespace llvm;

void CodeGenFunction::EmitApproxDirective(const ApproxDirective &AD) {
  CGApproxRuntime &RT = CGM.getApproxRuntime();
  CapturedStmt *CStmt = cast_or_null<CapturedStmt>(AD.getAssociatedStmt());

  RT.CGApproxRuntimeEnterRegion(*this, *CStmt);

  for (auto C : AD.clauses()) {
    if (ApproxIfClause *IfClause = dyn_cast_or_null<ApproxIfClause>(C)) {
      RT.CGApproxRuntimeEmitIfInit(*this, *IfClause);
    }
    else if (ApproxPerfoClause *PerfoClause = dyn_cast_or_null<ApproxPerfoClause>(C)){
      RT.CGApproxRuntimeEmitPerfoInit(*this, *CStmt, *PerfoClause, AD.LoopExprs);
    }
    else if (ApproxInClause *InClause = dyn_cast_or_null<ApproxInClause>(C)){
      RT.CGApproxRuntimeRegisterInputs(*InClause);
    }
    else if (ApproxOutClause *OutClause = dyn_cast_or_null<ApproxOutClause>(C)){
      RT.CGApproxRuntimeRegisterOutputs(*OutClause);
    }
    else if (ApproxInOutClause *InOutClause = dyn_cast_or_null<ApproxInOutClause>(C)){
      RT.CGApproxRuntimeRegisterInputsOutputs(*InOutClause);
    }
    else if ( ApproxMemoClause *MemoClause = dyn_cast_or_null<ApproxMemoClause>(C)){
      RT.CGApproxRuntimeEmitMemoInit(*this, *MemoClause);
    }
    else if ( ApproxLabelClause *LabelClause = dyn_cast_or_null<ApproxLabelClause>(C)){
      RT.CGApproxRuntimeEmitLabelInit(*this, *LabelClause);
    }
    else if ( ApproxPetrubateClause *PetrubateClause = dyn_cast_or_null<ApproxPetrubateClause>(C)){
      RT.CGApproxRuntimeEmitPetrubateInit(*this, *PetrubateClause);
    } else if (ApproxMLClause *MLClause = dyn_cast_or_null<ApproxMLClause>(C)) {
      RT.CGApproxRuntimeEmitMLInit(*this, *MLClause);
    } else if (ApproxDeclClause *DeclClause =
                   dyn_cast_or_null<ApproxDeclClause>(C)) {
      RT.CGApproxRuntimeEmitDeclInit(*this, *DeclClause);
    } else {
      dbgs() << "Clause Not Handled Yet" << C->getAsString() << "\n";
    }
  }

  RT.CGApproxRuntimeEmitDataValues(*this);
  RT.CGApproxRuntimeExitRegion(*this);
}