#pragma once
#include "InMemoryDisk.hpp"
#include "PageReplacementPolicy.hpp"
#include "StubFileSystem.hpp"

using VFSImpl = StubFileSystem;
using PageReplacementPolicyImpl = FIFOPolicy;
using DiskImpl = InMemoryDisk;