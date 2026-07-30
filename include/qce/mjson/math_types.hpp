// Copyright (c) 2026, Yakov Usoltsev
// Email: yakovmen62@gmail.com
//
// License: MIT

#pragma once

#include <qce/math/math.hpp>
#include <macrojson/fundamental.hpp>

namespace macrojson {
    static inline void write_to_json(
            const char* name, const QCE::float3d& value, Document::AllocatorType& alloc, Value& root) {
        Value jarr(kArrayType);

        Value jx(value.x());
        jarr.PushBack(jx, alloc);
        Value jy(value.y());
        jarr.PushBack(jy, alloc);
        Value jz(value.z());
        jarr.PushBack(jz, alloc);

        write_to_json(name, std::move(jarr), alloc, root);
    }

    static inline MJsonErrorCode read_from_json(const char* name, const Value& root, QCE::float3d& val) {
        if (name && !root.HasMember(name)) {
            return E_MJSON_NOT_EXISTS;
        }

        const Value& jval = name ? root[name] : root;
        if (!jval.IsArray()) {
            return E_MJSON_TYPE_MISMATCH;
        }

        const auto& arr = jval.GetArray();
        if (arr.Size() != 3) {
            return E_MJSON_TYPE_MISMATCH;
        }

        if (arr[0].IsFloat() && arr[1].IsFloat() && arr[2].IsFloat()) {
            val.x() = arr[0].GetFloat();
            val.y() = arr[1].GetFloat();
            val.z() = arr[2].GetFloat();
            return E_MJSON_OK;
        }

        return E_MJSON_TYPE_MISMATCH;
    }

    template<
        typename Float3dT,
        std::enable_if_t<std::is_same_v<Float3dT, QCE::float3d>, bool> = true>
    inline void generate_schema(
            const char* name, const char* title, const char* description,
            Document::AllocatorType& alloc, Value& schema) {
        generate_schema_base(name, title, description, "array", alloc, schema);
        Value& jobj = name ? schema[name] : schema;
        add_validation_fields<std::vector<float>>(alloc, jobj, ArrayParams{
            .minItems = 3,
            .maxItems = 3
        });
        generate_schema_base("items", nullptr, nullptr, "number", alloc, schema);
    }
}