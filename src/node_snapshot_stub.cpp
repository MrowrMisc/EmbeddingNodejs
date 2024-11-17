// This file is required to get things working :)
//
// See more:
// https://github.com/nodejs/node/blob/main/tools/snapshot/README.md

#define NODE_WANT_INTERNALS 1

#include "node.h"
#include "node_snapshot_builder.h"

namespace node {
    const SnapshotData* SnapshotBuilder::GetEmbeddedSnapshotData() { return nullptr; }
}