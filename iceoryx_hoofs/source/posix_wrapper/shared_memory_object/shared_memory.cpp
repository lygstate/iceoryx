// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
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

#include "iceoryx_hoofs/internal/posix_wrapper/shared_memory_object/shared_memory.hpp"
#include "iceoryx_hoofs/cxx/generic_raii.hpp"
#include "iceoryx_hoofs/cxx/helplets.hpp"
#include "iceoryx_hoofs/platform/fcntl.hpp"
#include "iceoryx_hoofs/platform/mman.hpp"
#include "iceoryx_hoofs/platform/stat.hpp"
#include "iceoryx_hoofs/platform/types.hpp"
#include "iceoryx_hoofs/platform/unistd.hpp"
#include "iceoryx_hoofs/posix_wrapper/posix_call.hpp"

#include <assert.h>
#include <bitset>
#include <limits>

namespace iox
{
namespace posix
{
SharedMemory::SharedMemory(const Name_t& name,
                           const AccessMode accessMode,
                           const OwnerShip ownerShip,
                           const mode_t permissions,
                           const uint64_t size) noexcept
    : m_ownerShip(ownerShip)
{
    m_isInitialized = true;
    // on qnx the current working directory will be added to the /dev/shmem path if the leading slash is missing
    if (name.empty())
    {
        std::cerr << "No shared memory name specified!" << std::endl;
        m_isInitialized = false;
        m_errorValue = SharedMemoryError::EMPTY_NAME;
    }
    else if (name.c_str()[0] != '/')
    {
        std::cerr << "Shared memory name must start with a leading slash!" << std::endl;
        m_isInitialized = false;
        m_errorValue = SharedMemoryError::NAME_WITHOUT_LEADING_SLASH;
    }

    if (m_isInitialized)
    {
        m_name = name;
        int oflags = 0;
        oflags |= (accessMode == AccessMode::READ_ONLY) ? O_RDONLY : O_RDWR;
        oflags |= (ownerShip == OwnerShip::MINE) ? O_CREAT | O_EXCL : 0;

        m_isInitialized = open(oflags, permissions, size);
    }

    if (!m_isInitialized)
    {
        std::cerr << "Unable to create shared memory with the following properties [ name = " << name
                  << ", access mode = " << ACCESS_MODE_STRING[static_cast<uint64_t>(accessMode)]
                  << ", ownership = " << OWNERSHIP_STRING[static_cast<uint64_t>(ownerShip)]
                  << ", mode = " << std::bitset<sizeof(mode_t)>(permissions) << ", sizeInBytes = " << size << " ]"
                  << std::endl;
        return;
    }
}

SharedMemory::~SharedMemory() noexcept
{
    destroy();
}

void SharedMemory::destroy() noexcept
{
    if (m_isInitialized)
    {
        close();
        unlink();
        reset();
    }
}

void SharedMemory::reset() noexcept
{
    m_isInitialized = false;
    m_name = Name_t();
    m_handle = -1;
}

SharedMemory::SharedMemory(SharedMemory&& rhs) noexcept
{
    *this = std::move(rhs);
}

SharedMemory& SharedMemory::operator=(SharedMemory&& rhs) noexcept
{
    if (this != &rhs)
    {
        destroy();

        CreationPattern_t::operator=(std::move(rhs));

        m_name = rhs.m_name;
        m_ownerShip = std::move(rhs.m_ownerShip);
        m_handle = std::move(rhs.m_handle);

        rhs.reset();
    }
    return *this;
}

int32_t SharedMemory::getHandle() const noexcept
{
    return m_handle;
}

bool SharedMemory::open(const int oflags, const mode_t permissions, const uint64_t size) noexcept
{
    cxx::Expects(static_cast<int64_t>(size) <= std::numeric_limits<int64_t>::max());


    // the mask will be applied to the permissions, therefore we need to set it to 0
    mode_t umaskSaved = umask(0U);
    {
        cxx::GenericRAII umaskGuard([&] { umask(umaskSaved); });

        // if we create the shm, cleanup old resources
        if (oflags & O_CREAT)
        {
            posixCall(shm_unlink)(m_name.c_str())
                .failureReturnValue(-1)
                .evaluateWithIgnoredErrnos(ENOENT)
                .and_then([this](auto& r) {
                    if (r.errnum != ENOENT)
                    {
                        std::cout << "SharedMemory still there, doing an unlink of " << m_name << std::endl;
                    }
                });
        }

        if (posixCall(iox_shm_open)(m_name.c_str(), oflags, permissions)
                .failureReturnValue(-1)
                .evaluate()
                .and_then([this](auto& r) { m_handle = r.value; })
                .or_else([this](auto& r) { m_errorValue = this->errnoToEnum(r.errnum); })
                .has_error())
        {
            return false;
        }
    }

    if (m_ownerShip == OwnerShip::MINE)
    {
        if (posixCall(ftruncate)(m_handle, static_cast<int64_t>(size))
                .failureReturnValue(-1)
                .evaluate()
                .or_else([this](auto& r) { m_errorValue = this->errnoToEnum(r.errnum); })
                .has_error())
        {
            return false;
        }
    }

    return true;
}

bool SharedMemory::unlink() noexcept
{
    if (m_isInitialized && m_ownerShip == OwnerShip::MINE)
    {
        if (posixCall(shm_unlink)(m_name.c_str())
                .failureReturnValue(-1)
                .evaluate()
                .or_else([](auto& r) {
                    std::cerr << "Unable to unlink SharedMemory (shm_unlink failed) : " << r.getHumanReadableErrnum()
                              << std::endl;
                })
                .has_error())
        {
            return false;
        }
    }
    return true;
}

bool SharedMemory::close() noexcept
{
    if (m_isInitialized)
    {
        auto call = posixCall(iox_close)(m_handle).failureReturnValue(-1).evaluate().or_else([](auto& r) {
            std::cerr << "Unable to close SharedMemory filedescriptor (close failed) : " << r.getHumanReadableErrnum()
                      << std::endl;
        });

        m_handle = -1;
        return !call.has_error();
    }
    return true;
}

SharedMemoryError SharedMemory::errnoToEnum(const int32_t errnum) const noexcept
{
    switch (errnum)
    {
    case EACCES:
        std::cerr << "No permission to modify, truncate or to access the shared memory!" << std::endl;
        return SharedMemoryError::INSUFFICIENT_PERMISSIONS;
    case EPERM:
        std::cerr << "Resizing a file beyond its current size is not supported by the filesystem!" << std::endl;
        return SharedMemoryError::NO_RESIZE_SUPPORT;
    case EFBIG:
        std::cerr << "Requested Shared Memory is larger then the maximum file size." << std::endl;
        return SharedMemoryError::REQUESTED_MEMORY_EXCEEDS_MAXIMUM_FILE_SIZE;
    case EINVAL:
        std::cerr << "Requested Shared Memory is larger then the maximum file size or the filedescriptor does not "
                     "belong to a regular file."
                  << std::endl;
        return SharedMemoryError::REQUESTED_MEMORY_EXCEEDS_MAXIMUM_FILE_SIZE;
    case EBADF:
        std::cerr << "Provided filedescriptor is not a valid filedescriptor." << std::endl;
        return SharedMemoryError::INVALID_FILEDESCRIPTOR;
    case EEXIST:
        std::cerr << "A Shared Memory with the given name already exists." << std::endl;
        return SharedMemoryError::DOES_EXIST;
    case EISDIR:
        std::cerr << "The requested Shared Memory file is a directory." << std::endl;
        return SharedMemoryError::PATH_IS_A_DIRECTORY;
    case ELOOP:
        std::cerr << "Too many symbolic links encountered while traversing the path." << std::endl;
        return SharedMemoryError::TOO_MANY_SYMBOLIC_LINKS;
    case EMFILE:
        std::cerr << "Process limit of maximum open files reached." << std::endl;
        return SharedMemoryError::PROCESS_LIMIT_OF_OPEN_FILES_REACHED;
    case ENFILE:
        std::cerr << "System limit of maximum open files reached." << std::endl;
        return SharedMemoryError::SYSTEM_LIMIT_OF_OPEN_FILES_REACHED;
    case ENOENT:
        std::cerr << "Shared Memory does not exist." << std::endl;
        return SharedMemoryError::DOES_NOT_EXIST;
    case ENOMEM:
        std::cerr << "Not enough memory available to create shared memory." << std::endl;
        return SharedMemoryError::NOT_ENOUGH_MEMORY_AVAILABLE;
    default:
        std::cerr << "This should never happen! An unknown error occurred!" << std::endl;
        return SharedMemoryError::UNKNOWN_ERROR;
    }
}

} // namespace posix
} // namespace iox
