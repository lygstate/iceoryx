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
#ifndef IOX_HOOFS_POSIX_WRAPPER_TYPES_HPP
#define IOX_HOOFS_POSIX_WRAPPER_TYPES_HPP

#include <cstdint>

namespace iox
{
namespace posix
{
enum class AccessMode : uint64_t
{
    READ_ONLY = 0U,
    READ_WRITE = 1U
};
static constexpr const char* ACCESS_MODE_STRING[] = {"AccessMode::READ_ONLY", "AccessMode::READ_WRITE"};

/// @brief describes how the shared memory is opened or created
enum class OpenMode : uint64_t
{
    /// @brief creates the shared memory, if it exists already the construction will fail
    EXCLUSIVE_CREATE = 0U,
    /// @brief creates the shared memory, if it exists it will be deleted and recreated
    PURGE_AND_CREATE = 1U,
    /// @brief creates the shared memory, if it does not exist otherwise it opens it
    OPEN_OR_CREATE = 2U,
    /// @brief opens the shared memory, if it does not exist it will fail
    OPEN_EXISTING = 3U
};
static constexpr const char* OPEN_MODE_STRING[] = {
    "OpenMode::EXCLUSIVE_CREATE", "OpenMode::PURGE_AND_CREATE", "OpenMode::OPEN_OR_CREATE", "OpenMode::OPEN_EXISTING"};

/// @brief converts the AccessMode into the corresponding O_** flags.
/// @param[in] accessMode the accessMode which should be converted
int convertToOflags(const AccessMode accessMode) noexcept;

/// @brief converts the OpenMode into the corresponding O_** flags.
/// @param[in] openMode the openMode which should be converted
int convertToOflags(const OpenMode openMode) noexcept;

/// @brief converts the AccessMode and OpenMode into the corresponding O_** flags
/// @param[in] accessMode the accessMode which should be converted
/// @param[in] openMode the openMode which should be converted
int convertToOflags(const AccessMode accessMode, const OpenMode openMode) noexcept;

} // namespace posix
} // namespace iox

#endif
