// Copyright (c) 2022 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "iceoryx_hoofs/posix_wrapper/types.hpp"

#include "iceoryx_hoofs/platform/fcntl.hpp"

namespace iox
{
namespace posix
{
int convertToOflags(const AccessMode accessMode) noexcept
{
    switch (accessMode)
    {
    case AccessMode::READ_ONLY:
        return O_RDONLY;
    case AccessMode::READ_WRITE:
        return O_RDWR;
    }

    return 0;
}

int convertToOflags(const OpenMode openMode) noexcept
{
    switch (openMode)
    {
    case OpenMode::OPEN_EXISTING:
        return 0;
    case OpenMode::OPEN_OR_CREATE:
        return O_CREAT;
    case OpenMode::EXCLUSIVE_CREATE:
        return O_CREAT | O_EXCL;
    case OpenMode::PURGE_AND_CREATE:
        return O_CREAT | O_EXCL;
    }

    return 0;
}

int convertToOflags(const AccessMode accessMode, const OpenMode openMode) noexcept
{
    return convertToOflags(accessMode) | convertToOflags((openMode));
}
} // namespace posix
} // namespace iox

