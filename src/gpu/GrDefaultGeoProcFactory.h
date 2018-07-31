/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrDefaultGeoProcFactory_DEFINED
#define GrDefaultGeoProcFactory_DEFINED

#include "GrColorSpaceXform.h"
#include "GrGeometryProcessor.h"
#include "GrShaderCaps.h"

/*
 * A factory for creating default Geometry Processors which simply multiply position by the uniform
 * view matrix and wire through color, coverage, UV coords if requested.
 */
namespace GrDefaultGeoProcFactory {
    // Structs for adding vertex attributes
    struct PositionAttr {
        SkPoint fPosition;
    };

    struct PositionCoverageAttr {
        SkPoint fPosition;
        float   fCoverage;
    };

    struct PositionColorAttr {
        SkPoint fPosition;
        SkColor fColor;
    };

    struct PositionColorCoverageAttr {
        SkPoint fPosition;
        SkColor fColor;
        float   fCoverage;
    };

    struct PositionLocalCoordAttr {
        SkPoint fPosition;
        SkPoint fLocalCoord;
    };

    struct PositionLocalCoordCoverageAttr {
        SkPoint fPosition;
        SkPoint fLocalCoord;
        float   fCoverage;
    };

    struct PositionColorLocalCoordAttr {
        SkPoint fPosition;
        GrColor fColor;
        SkPoint fLocalCoord;
    };

    struct PositionColorLocalCoordCoverageAttr {
        SkPoint fPosition;
        GrColor fColor;
        SkPoint fLocalCoord;
        float   fCoverage;
    };

    struct Color {
        enum Type {
            kPremulGrColorUniform_Type,
            kPremulGrColorAttribute_Type,
            kUnpremulSkColorAttribute_Type,
        };
        explicit Color(GrColor color)
                : fType(kPremulGrColorUniform_Type)
                , fColor(color)
                , fColorSpaceXform(nullptr) {}
        Color(Type type)
                : fType(type)
                , fColor(GrColor_ILLEGAL)
                , fColorSpaceXform(nullptr) {
            SkASSERT(type != kPremulGrColorUniform_Type);
        }

        Type fType;
        GrColor fColor;

        // This only applies to SkColor. Any GrColors are assumed to have been color converted
        // during paint conversion.
        sk_sp<GrColorSpaceXform> fColorSpaceXform;
    };

    struct Coverage {
        enum Type {
            kSolid_Type,
            kUniform_Type,
            kAttribute_Type,
        };
        explicit Coverage(uint8_t coverage) : fType(kUniform_Type), fCoverage(coverage) {}
        Coverage(Type type) : fType(type), fCoverage(0xff) {
            SkASSERT(type != kUniform_Type);
        }

        Type fType;
        uint8_t fCoverage;
    };

    struct LocalCoords {
        enum Type {
            kUnused_Type,
            kUsePosition_Type,
            kHasExplicit_Type,
            kHasTransformed_Type,
        };
        LocalCoords(Type type) : fType(type), fMatrix(nullptr) {}
        LocalCoords(Type type, const SkMatrix* matrix) : fType(type), fMatrix(matrix) {
            SkASSERT(kUnused_Type != type);
        }
        bool hasLocalMatrix() const { return nullptr != fMatrix; }

        Type fType;
        const SkMatrix* fMatrix;
    };

    struct Bones {
        Bones(const float bones[], int boneCount)
            : fBones(bones)
            , fBoneCount(boneCount) {}

        const float* fBones;
        int fBoneCount;
    };

    sk_sp<GrGeometryProcessor> Make(const GrShaderCaps*,
                                    const Color&,
                                    const Coverage&,
                                    const LocalCoords&,
                                    const SkMatrix& viewMatrix);

    /*
     * Use this factory to create a GrGeometryProcessor that expects a device space vertex position
     * attribute. The view matrix must still be provided to compute correctly transformed
     * coordinates for GrFragmentProcessors. It may fail if the view matrix is not invertible.
     */
    sk_sp<GrGeometryProcessor> MakeForDeviceSpace(const GrShaderCaps*,
                                                  const Color&,
                                                  const Coverage&,
                                                  const LocalCoords&,
                                                  const SkMatrix& viewMatrix);

    /*
     * Use this factory to create a GrGeometryProcessor that supports skeletal animation through
     * deformation of vertices using matrices that are passed in. This should only be called from
     * GrDrawVerticesOp.
     */
    sk_sp<GrGeometryProcessor> MakeWithBones(const GrShaderCaps*,
                                             const Color&,
                                             const Coverage&,
                                             const LocalCoords&,
                                             const Bones&,
                                             const SkMatrix& viewMatrix);
};

#endif
