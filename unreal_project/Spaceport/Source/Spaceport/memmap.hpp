#pragma once

#if defined(_MSC_VER)

#if defined(BUILD_FOR_TEST)
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <Windows.h>
#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif
#else
#include <Windows/WindowsHWrapper.h>
#endif

#include <fileapi.h>

#else

#error "memmap has no implementation for this platform."

#endif

#include <exception>
#include <filesystem>
#include <span>


struct failed_memmap : std::exception
{
    failed_memmap(std::string msg) : msg_(std::move(msg)) {}

    char const * what() const noexcept { return msg_.c_str(); }

private:
    std::string msg_;
};

struct memmap
{
    memmap() = default;
    memmap(std::filesystem::path path);
    ~memmap();

    std::span<std::byte> bytes() const
    {
        return {static_cast<std::byte *>(ptr_), (std::size_t)size_};
    }

private:
    std::filesystem::path path_;
    void * ptr_ = nullptr;
    std::ptrdiff_t size_ = 0;
#if defined(_MSC_VER)
    HANDLE file_h_ = 0;
    HANDLE map_h_ = 0;
#endif
};

#if defined(_MSC_VER)
inline memmap::memmap(std::filesystem::path path) : path_(std::move(path))
{
    if (!std::filesystem::exists(path_)) {
        throw failed_memmap(std::format(
            "memmap: Cannot memmap file {}; it does not exist.",
            path_.generic_string()));
    }

    file_h_ = CreateFile(
        path_.c_str(),         // File name
        GENERIC_READ,          // Desired access (read-only)
        FILE_SHARE_READ,       // Share mode (allow other reads)
        nullptr,               // Security attributes
        OPEN_EXISTING,         // Creation disposition
        FILE_ATTRIBUTE_NORMAL, // Flags and attributes
        nullptr                // Template file
    );
    if (!file_h_) {
        throw failed_memmap(std::format(
            "memmap: Failed to get a Win32 HANDLE to {}.",
            path_.generic_string()));
    }

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(file_h_, &file_size)) {
        throw failed_memmap(std::format(
            "memmap: Failed to get file size from Win32 HANDLE to {}.",
            path_.generic_string()));
    }
    if (std::numeric_limits<size_t>::max() < file_size.QuadPart) {
        throw failed_memmap(std::format(
            "memmap: Size of file {} is greater than a 64-bit int.",
            path_.generic_string()));
    }
    size_ = static_cast<std::ptrdiff_t>(file_size.QuadPart);

    map_h_ = CreateFileMapping(
        file_h_,       // File handle
        nullptr,       // Security attributes
        PAGE_READONLY, // Protection flags (read-only)
        0,             // Maximum size high (0 for same as file)
        0,             // Maximum size low (0 for same as file)
        nullptr        // Name of mapping object (can be NULL for unnamed)
    );
    if (!map_h_) {
        throw failed_memmap(std::format(
            "memmap: Failed to get a Win32 mapping HANDLE for {}.",
            path_.generic_string()));
    }

    ptr_ = MapViewOfFile(
        map_h_,        // File mapping object handle
        FILE_MAP_READ, // Desired access (read-only)
        0,             // File offset high
        0,             // File offset low
        0              // Number of bytes to map (0 for entire file)
    );
    if (!ptr_) {
        throw failed_memmap(std::format(
            "memmap: Failed to get a pointer to the Win32 mapping HANDLE for "
            "{}.",
            path_.generic_string()));
    }
}

inline memmap::~memmap()
{
    UnmapViewOfFile(ptr_);
    CloseHandle(map_h_);
    CloseHandle(file_h_);
}
#endif
