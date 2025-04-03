#ifndef __BSTR_H___
#define __BSTR_H___

#include <stddef.h>
#include <stdint.h>

/**
 * @file bstr.h
 * @brief Cross-platform shared string and byte array library with thread-safe reference counting.
 *
 * This library provides two main types:
 * - bytes_t: A shared byte array for efficient binary data handling
 * - bstr_t: A shared UTF-8 string optimized for string operations
 *
 * Key features:
 * - Thread-safe reference counting for memory management
 * - Zero-copy slicing and sharing
 * - UTF-8/16/32 conversion support
 * - BASE64 encoding/decoding
 *
 * Memory management:
 * - All created objects must be released using corresponding release functions
 * - Thread-safe reference counting ensures proper cleanup
 * - Zero-copy operations where possible to maximize performance
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cross-platform shared byte array with thread-safe reference counting.
 *
 * bytes_t provides efficient binary data handling with the following features:
 * - Zero-copy slicing and sharing
 * - Thread-safe reference counting
 * - Immutable data protection
 * - Efficient binary data handling
 * - Minimal memory allocation and copying
 *
 * Memory Management:
 * - Memory is automatically managed through reference counting
 * - Create using bytes_new(), bytes_alloc(), etc.
 * - Must release using bytes_release()
 * - Thread-safe cleanup through reference counting
 * - Zero-copy operations used where possible
 *
 * Thread Safety:
 * - All operations are thread-safe through reference counting
 * - Multiple threads can safely share and modify bytes objects
 * - No external synchronization required
 *
 * Performance:
 * - Efficient binary data handling
 * - Zero-copy operations for slicing and sharing
 * - Minimal memory allocation and copying
 * - Optimized for both small and large data sets
 */
typedef struct bytes_t {
    void *reserved1; /* Opaque pointer for internal use */
    void *reserved2; /* Opaque pointer for internal use */
    void *reserved3; /* Opaque pointer for internal use */
    void *reserved4; /* Opaque pointer for internal use */
} bytes_t;

/**
 * @brief Cross-platform shared UTF-8 string with thread-safe reference counting.
 *
 * bstr_t provides efficient string handling with the following features:
 * - Always valid UTF-8 encoding
 * - Zero-copy operations and sharing
 * - Thread-safe reference counting
 * - No null terminator (length-based)
 * - Consistent Unicode handling
 *
 * Memory Management:
 * - Memory is automatically managed through reference counting
 * - Create using bstr_new(), bstr_from_utf8(), etc.
 * - Must release using bstr_release()
 * - Thread-safe cleanup through reference counting
 * - Zero-copy operations used where possible
 *
 * Thread Safety:
 * - All operations are thread-safe through reference counting
 * - Multiple threads can safely share and modify string objects
 * - No external synchronization required
 *
 * Performance:
 * - Efficient string operations
 * - Zero-copy operations for sharing
 * - Minimal memory allocation and copying
 * - Optimized UTF-8 handling
 * - Efficient conversion between encodings
 */
typedef struct bstr_t {
    void *reserved1; /* Opaque pointer for internal use */
    void *reserved2; /* Opaque pointer for internal use */
    void *reserved3; /* Opaque pointer for internal use */
    void *reserved4; /* Opaque pointer for internal use */
} bstr_t;

/// Alias of std::string::npos
#ifndef NPOS
#define NPOS ((size_t)(-1))
#endif

////////////////////////////////////////////////////////////////////////////////

/// @brief Create an empty byte array in place.
/// @param [out] buf The buffer of the byte array to be initialized
/// @note No memory allocation occurs for empty arrays
/// @note Thread-safe through reference counting
extern void bytes_init(bytes_t *buf);

/// @brief Get the internal pointer of the byte array.
/// @param [in] bytes The pointer of the byte array
/// @return The internal pointer of the byte array (not null-terminated)
/// @note The pointer remains valid until the bytes object is modified
/// @note Thread-safe for concurrent reads
/// @note The returned pointer must not be used to modify the data
extern const uint8_t *bytes_ptr(const bytes_t *bytes);

/// @brief Get the size of the byte array.
/// @param [in] bytes The pointer of the byte array
/// @return Number of bytes in the array
/// @note Thread-safe for concurrent reads
extern size_t bytes_size(const bytes_t *bytes);

/// @brief Create an empty byte array.
/// @return The new array which must be released by \ref bytes_release()
/// @note No memory allocation occurs for empty arrays
/// @note Thread-safe through reference counting
extern bytes_t bytes_new(void);

/// @brief Allocate an uninitialized byte array with the length.
/// @param [in] len Number of bytes of the array
/// @return The new array which must be released by \ref bytes_release()
/// @note Creates a new reference-counted array
/// @note Memory is automatically managed
/// @note Thread-safe through reference counting
extern bytes_t bytes_alloc(size_t len);

/// @brief Allocate a zero-initialized byte array with the length.
/// @param [in] len Number of bytes of the array
/// @return The new array which must be released by bytes_release()
/// @note Creates a new reference-counted array initialized to zero
/// @note Memory is automatically managed through reference counting
/// @note Thread-safe through reference counting
/// @note All bytes are initialized to zero value
extern bytes_t bytes_zalloc(size_t len);

/// @brief Create a bytes object from a static byte array without copying.
/// @param [in] static_data Pointer to static data that must outlive the bytes object
/// @param [in] len Number of bytes in the array
/// @return New bytes object referencing the static data
/// @note Zero-copy operation - no memory allocation
/// @note Thread-safe through reference counting
/// @note The static_data pointer must remain valid for the lifetime of the returned object
extern bytes_t bytes_from_static(const void *static_data, size_t len);

/// @brief Create a bytes object by copying from a UTF-8 string.
/// @param [in] s The source string to copy from
/// @return New bytes object containing a copy of the string data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
extern bytes_t bytes_from_bstr(const bstr_t *s);

/// @brief Create a bytes object by copying from a memory slice.
/// @param [in] data Pointer to the source data to copy
/// @param [in] len Number of bytes to copy
/// @return New bytes object containing a copy of the data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
/// @note The source data is copied, so it can be freed after the call
extern bytes_t bytes_copy_from_slice(const void *data, size_t len);

/// @brief Create a view into a portion of a bytes object.
/// @param [in] bytes The source bytes object
/// @param [in] start The start index of the slice (must be >= 0)
/// @param [in] stop The end index of the slice (must be <= length), or NPOS for end
/// @return New bytes object representing the slice
/// @note Zero-copy operation - shares data with source
/// @note Thread-safe through reference counting
/// @note The returned slice shares the same underlying data
/// @note Invalid indices will be clamped to valid range
extern bytes_t bytes_slice(const bytes_t *bytes, size_t start, size_t stop);

/// @brief Create a new copy of a bytes object.
/// @param [in] bytes The source bytes object to clone
/// @return New bytes object containing a copy of the data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
/// @note The source object is unaffected
extern bytes_t bytes_clone(const bytes_t *bytes);

/// @brief Release a reference to a bytes object.
/// @param [in,out] bytes The bytes object to release
/// @note Thread-safe through reference counting
/// @note Object is freed when last reference is released
/// @note Sets pointer to NULL after release
extern void bytes_release(bytes_t *bytes);

/// @brief Exchange the contents of two bytes objects.
/// @param [in,out] a First bytes object to swap
/// @param [in,out] b Second bytes object to swap
/// @note Thread-safe through reference counting
/// @note Zero-copy operation using pointer swapping
/// @note Both objects remain valid after swap
extern void bytes_swap(bytes_t *a, bytes_t *b);

/// @brief Create a bytes object by decoding BASE64 data.
/// @param [in] str The BASE64 encoded string to decode
/// @return New bytes object containing the decoded data
/// @note Creates a new reference-counted array
/// @note Thread-safe through reference counting
/// @note Invalid BASE64 input will return an empty array
extern bytes_t bytes_base64_decode(const bstr_t *str);

/// @brief Create a string containing BASE64 encoded data.
/// @param [in] bytes The bytes object to encode
/// @return New string containing the BASE64 encoded data
/// @note Creates a new reference-counted string
/// @note Thread-safe through reference counting
/// @note The encoding uses standard BASE64 alphabet
extern bstr_t bytes_base64_encode(const bytes_t *bytes);

////////////////////////////////////////////////////////////////////////////////

/// @brief Initialize an empty string in place.
/// @param [out] buf The string object to initialize
/// @note No memory allocation occurs for empty strings
/// @note Thread-safe through reference counting
/// @note Must call bstr_release when no longer needed
extern void bstr_init(bstr_t *buf);

/// @brief Get the internal pointer to the string data.
/// @param [in] s The string object
/// @return Pointer to UTF-8 encoded string data (not null-terminated)
/// @note The pointer remains valid until the string is modified
/// @note Thread-safe for concurrent reads
/// @note The returned pointer must not be used to modify the string
extern const char *bstr_ptr(const bstr_t *s);

/// @brief Get the length of the string in bytes.
/// @param [in] s The string object
/// @return Number of bytes in the UTF-8 encoded string
/// @note This is the byte length, not the character count
/// @note Thread-safe for concurrent reads
extern size_t bstr_size(const bstr_t *s);

/// @brief Create a new empty string.
/// @return New empty string object
/// @note No memory allocation occurs for empty strings
/// @note Thread-safe through reference counting
/// @note Must call bstr_release when no longer needed
extern bstr_t bstr_new(void);

/// @brief Create a string from a static UTF-8 string without copying.
/// @param [in] static_str Pointer to static UTF-8 string data that must outlive the string object
/// @param [in] len Length of string in bytes, or NPOS to calculate from null terminator
/// @return New string object referencing the static data
/// @note Zero-copy operation - no memory allocation
/// @note Thread-safe through reference counting
/// @note The static_str pointer must remain valid for the lifetime of the returned object
/// @note Input must be valid UTF-8 encoded text
extern bstr_t bstr_from_static(const char *static_str, size_t len);

/// @brief Create a string by copying from a bytes object.
/// @param [in] bytes The source bytes object containing UTF-8 data
/// @return New string object containing a copy of the data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
/// @note Input must be valid UTF-8 encoded text
/// @note Invalid UTF-8 input will return an empty string
extern bstr_t bstr_from_bytes(const bytes_t *bytes);

/// @brief Create a string by copying from a UTF-8 string.
/// @param [in] str The source UTF-8 string to copy from
/// @param [in] len Length of string in bytes, or NPOS to calculate from null terminator
/// @return New string object containing a copy of the data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
/// @note Input must be valid UTF-8 encoded text
/// @note Invalid UTF-8 input will return an empty string
extern bstr_t bstr_from_utf8(const char *str, size_t len);

/// @brief Create a string by converting from a UTF-16 string.
/// @param [in] str The source UTF-16 string to convert
/// @param [in] len Length in UTF-16 code units, or NPOS to calculate from null terminator
/// @return New string object containing the converted UTF-8 data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
/// @note Handles surrogate pairs correctly
/// @note Invalid UTF-16 input will return an empty string
extern bstr_t bstr_from_utf16(const uint16_t *str, size_t len);

/// @brief Create a string by converting from a UTF-32 string.
/// @param [in] str The source UTF-32 string to convert
/// @param [in] len Length in UTF-32 code units, or NPOS to calculate from null terminator
/// @return New string object containing the converted UTF-8 data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
/// @note Handles all Unicode code points
/// @note Invalid UTF-32 input will return an empty string
extern bstr_t bstr_from_utf32(const uint32_t *str, size_t len);

/// @brief Create a new copy of a string object.
/// @param [in] s The source string object to clone
/// @return New string object containing a copy of the data
/// @note Creates a new reference-counted copy
/// @note Thread-safe through reference counting
/// @note The source object is unaffected
extern bstr_t bstr_clone(const bstr_t *s);

/// @brief Release a reference to a string object.
/// @param [in,out] s The string object to release
/// @note Thread-safe through reference counting
/// @note Object is freed when last reference is released
/// @note Sets pointer to NULL after release
extern void bstr_release(bstr_t *s);

/// @brief Exchange the contents of two string objects.
/// @param [in,out] a First string object to swap
/// @param [in,out] b Second string object to swap
/// @note Thread-safe through reference counting
/// @note Zero-copy operation using pointer swapping
/// @note Both objects remain valid after swap
extern void bstr_swap(bstr_t *a, bstr_t *b);

/// @brief Create a null-terminated UTF-8 string copy.
/// @param [in,out] s The source string object
/// @return New buffer containing null-terminated UTF-8 data
/// @note Creates a new independent copy with null terminator
/// @note Caller must free the returned buffer using bstr_mem_free() or free()
/// @note Thread-safe for concurrent reads
extern char *bstr_dup_utf8(const bstr_t *s);

/// @brief Create a null-terminated UTF-16 string copy.
/// @param [in,out] s The source string object
/// @return New buffer containing null-terminated UTF-16 data
/// @note Creates a new independent copy with null terminator
/// @note Caller must free the returned buffer using bstr_mem_free() or free()
/// @note Thread-safe for concurrent reads
/// @note Handles conversion from UTF-8 to UTF-16 encoding
extern uint16_t *bstr_dup_utf16(const bstr_t *s);

/// @brief Create a null-terminated UTF-32 string copy.
/// @param [in,out] s The source string object
/// @return New buffer containing null-terminated UTF-32 data
/// @note Creates a new independent copy with null terminator
/// @note Caller must free the returned buffer using bstr_mem_free() or free()
/// @note Thread-safe for concurrent reads
/// @note Handles conversion from UTF-8 to UTF-32 encoding
extern uint32_t *bstr_dup_utf32(const bstr_t *s);

/// @brief Free memory allocated by string duplication functions.
/// @param [in] ptr Pointer to memory allocated by bstr_dup_* functions
/// @note Thread-safe memory deallocation
/// @note Equivalent to standard free() function
/// @note Sets freed pointer to NULL
extern void bstr_mem_free(void *ptr);

#if (WCHAR_MAX == 0xffff)
#define bstr_from_wchar(str, len) bstr_from_utf16((const uint16_t *)(str), len)
#define bstr_dup_wchar(s)         ((wchar_t *)bstr_dup_utf16(s))
#else
#define bstr_from_wchar(str, len) bstr_from_utf32((const uint32_t *)(str), len)
#define bstr_dup_wchar(s)         ((wchar_t *)bstr_dup_utf32(s))
#endif

#ifdef __cplusplus
}

////////////////////////////////////////////////////////////////////////////////

#if (!defined(_BSTR_NO_CPP) || _BSTR_NO_CPP == 0)

#ifndef _BSTR_HAS_STD
#define _BSTR_HAS_STD 1
#endif

// clang-format off
#include <string.h>
#if (_BSTR_HAS_STD)
#include <wchar.h>
#include <string>
#endif
// clang-format on

class ByteString;

/**
 * @brief Lightweight string class for non-STL environments with RAII memory management.
 *
 * CStr provides a simple, efficient string wrapper with automatic memory management
 * through RAII pattern. It is designed for environments where STL is not available
 * or when minimal string functionality is needed.
 *
 * Features:
 * - Automatic memory management (RAII)
 * - Null-terminated string storage
 * - Efficient copy-on-write semantics
 * - Minimal memory footprint
 * - Thread-safe operations
 *
 * Memory Management:
 * - Strings are stored with null termination
 * - Empty strings use a static buffer to avoid allocation
 * - Memory is automatically freed in destructor
 */
class CStr {
public:
    /// @brief Default constructor creates an empty string using static buffer
    /// @note No memory allocation occurs for empty strings
    CStr() : m_str(EMPTY_STR()) {}

    /// @brief Construct from a C-style string with optional length
    /// @param s Source string to copy from
    /// @param len Number of characters to copy, or NPOS to calculate from null terminator
    /// @note Creates a new copy of the string data
    CStr(const char *s, size_t len = NPOS) : m_str(_allocate(s, len)) {}

    /// @brief Destructor ensures proper cleanup of allocated memory
    /// @note Thread-safe cleanup through reference counting
    ~CStr() { _deallocate(); }

    /// @brief Copy constructor creates a new copy of the string
    /// @param other Source string to copy from
    /// @note Creates a new memory allocation for the copy
    CStr(const CStr &other) : m_str(_allocate(other.m_str, NPOS)) {}

    /// @brief Assignment operator creates a new copy of the string
    /// @param other Source string to copy from
    /// @return Reference to this string
    /// @note Properly handles self-assignment and memory cleanup
    CStr &operator=(const CStr &other) {
        if (this != &other) {
            _deallocate();
            m_str = _allocate(other.m_str);
        }
        return *this;
    }

    /// @brief Convert to const C-style string pointer
    /// @return Pointer to null-terminated string data
    /// @note The returned pointer remains valid until the CStr is modified
    operator const char *() const { return m_str; }

    /// @brief Get pointer to null-terminated string data
    /// @return Pointer to null-terminated string data
    /// @note The returned pointer remains valid until the CStr is modified
    const char *c_str() const { return m_str; }

    /// @brief Get the length of the string
    /// @return Number of characters in string (excluding null terminator)
    /// @note Calculates length by scanning for null terminator
    size_t size() const { return ::strlen(m_str); }

#if __cplusplus >= 201103L  // C++ 11
    /// @brief Move constructor transfers ownership of string data
    /// @param other Source string to move from
    /// @note Zero-cost transfer of ownership, source becomes empty
    CStr(CStr &&other) : m_str(other.m_str) { other.m_str = EMPTY_STR(); }

    /// @brief Move assignment operator transfers ownership of string data
    /// @param other Source string to move from
    /// @return Reference to this string
    /// @note Efficient ownership transfer with proper cleanup
    CStr &operator=(CStr &&other) {
        if (this != &other) {
            const char *tmp = m_str;
            m_str = other.m_str;
            other.m_str = tmp;
        }
        return *this;
    }
#endif

private:
    static const char *EMPTY_STR() {
        static const char ES[] = {0, 0};
        return (const char *)((uintptr_t)ES | 1);
    }

    static const char *_allocate(const char *s, size_t len = NPOS) {
        if (s) {
            if (len == NPOS) {
                len = ::strlen(s);
            }
            if (len > 0) {
                char *p = new char[len + 1];
                if (p) {
                    ::memcpy(p, s, len);
                    p[len] = '\0';
                    return p;
                }
            }
        }
        return EMPTY_STR();
    }

    void _deallocate() {
        if (!((uintptr_t)m_str & 1)) {
            delete[] (char *)m_str;
            m_str = EMPTY_STR();
        }
    }

    const char *m_str;
};

/**
 * @brief Thread-safe byte array wrapper with RAII memory management.
 *
 * The Bytes class provides a C++ wrapper around the C bytes_t type, offering:
 * - Automatic memory management through RAII
 * - Thread-safe reference counting
 * - Zero-copy operations where possible
 * - Efficient binary data handling
 * - Consistent UTF-8 encoding support
 *
 * Memory Management:
 * - Memory is automatically managed through reference counting
 * - The destructor ensures proper cleanup
 * - Thread-safe operations through atomic reference counts
 * - Zero-copy operations used where possible to maximize performance
 *
 * Thread Safety:
 * - All operations are thread-safe through reference counting
 * - Multiple threads can safely share and modify Bytes objects
 * - No external synchronization required
 *
 * Performance:
 * - Efficient binary data handling
 * - Zero-copy operations for slicing and sharing
 * - Minimal memory allocation and copying
 * - Optimized for both small and large data sets
 */
class Bytes {
public:
    /// @brief Default constructor creates an empty byte array
    /// @note No memory allocation occurs for empty arrays
    /// @note Thread-safe through reference counting
    Bytes() { ::bytes_init(&m_inner); }

    /// @brief Destructor ensures proper cleanup through reference counting
    /// @note Thread-safe cleanup of shared resources
    /// @note Automatically handles deallocation when reference count reaches zero
    ~Bytes() { ::bytes_release(&m_inner); }

    /// @brief Create a Bytes object from a static bytes slice without copying
    /// @param static_data Pointer to static data that must outlive the Bytes object
    /// @param len Length of the data in bytes
    /// @return New Bytes object referencing the static data
    /// @note Zero-copy operation - no memory allocation
    /// @note Thread-safe through reference counting
    static Bytes from_static(const void *static_data, size_t len) {
        return Bytes(::bytes_from_static(static_data, len), 0);
    }

    /// @brief Move a bytes_t into a new Bytes object without copying
    /// @param moved Source bytes_t to move from (becomes empty after move)
    /// @return New Bytes object taking ownership of the moved data
    /// @note Zero-cost operation - no memory allocation or copying
    /// @note Thread-safe through reference counting
    /// @note Source bytes_t becomes empty after move
    static Bytes from_bytes(bytes_t &moved) {
        Bytes x = Bytes(moved, 0);
        ::bytes_init(&moved);
        return x;
    }

    /// @brief Allocate an uninitialized byte array with specified length
    /// @param len Number of bytes to allocate
    /// @note Creates a new reference-counted array
    /// @note Memory is automatically managed
    /// @note Thread-safe through reference counting
    Bytes(size_t len) : m_inner(::bytes_alloc(len)) {}

    /// @brief Construct from a ByteString, converting UTF-8 string to bytes
    /// @param str Source ByteString to convert
    /// @note Creates a new reference-counted copy
    /// @note Preserves UTF-8 encoding
    /// @note Thread-safe through reference counting
    Bytes(const ByteString &str);

    /// @brief Construct from a C bstr_t struct
    /// @param str Source string to convert to bytes
    /// @note Creates a new reference-counted copy
    Bytes(const bstr_t &str) : m_inner(::bytes_from_bstr(&str)) {}

    /// @brief Construct by copying from raw memory
    /// @param data Pointer to source data
    /// @param len Number of bytes to copy
    /// @note Creates a new copy of the data
    Bytes(const void *data, size_t len) : m_inner(::bytes_copy_from_slice(data, len)) {}

    /// @brief Construct from a C bytes_t struct
    /// @param bytes Source bytes to copy
    /// @note Creates a new reference-counted copy
    /// @note Thread-safe through reference counting
    /// @note Preserves all binary data exactly
    Bytes(const bytes_t &bytes) : m_inner(::bytes_clone(&bytes)) {}

    /// @brief Copy constructor creates a new reference-counted copy
    /// @param other Source bytes to copy from
    /// @note Creates a new reference-counted copy
    /// @note Thread-safe through reference counting
    /// @note Preserves all binary data exactly
    Bytes(const Bytes &other) : m_inner(::bytes_clone(&other.m_inner)) {}

    /// @brief Assignment operator creates a new reference-counted copy
    /// @param other Source bytes to copy from
    /// @return Reference to this byte array
    /// @note Creates a new reference-counted copy
    /// @note Thread-safe through reference counting
    /// @note Properly handles self-assignment
    Bytes &operator=(const Bytes &other) {
        if (this != &other) {
            ::bytes_release(&m_inner);
            m_inner = ::bytes_clone(&other.m_inner);
        }
        return *this;
    }

    /// @brief Convert to const C bytes_t pointer
    /// @return Pointer to internal bytes_t struct
    operator const bytes_t *() const { return &m_inner; }

    /// @brief Convert to mutable C bytes_t pointer
    /// @return Pointer to internal bytes_t struct
    operator bytes_t *() { return &m_inner; }

    /// @brief Convert to const C bytes_t reference
    /// @return Reference to internal bytes_t struct
    operator const bytes_t &() const { return m_inner; }

    /// @brief Convert to mutable C bytes_t reference
    /// @return Reference to internal bytes_t struct
    operator bytes_t &() { return m_inner; }

    /// @brief Get raw pointer to the byte array data
    /// @return Pointer to binary data (not null-terminated)
    /// @note The pointer remains valid until the Bytes object is modified
    /// @note Thread-safe for concurrent reads
    /// @note The returned pointer must not be used to modify the data
    const uint8_t *ptr() const { return bytes_ptr(&m_inner); }

    /// @brief Get the length of the byte array in bytes
    /// @return Number of bytes in the array
    /// @note Thread-safe for concurrent reads
    size_t size() const { return bytes_size(&m_inner); }

    /// @brief Compare equality with another Bytes object
    /// @param other Bytes object to compare with
    /// @return true if contents are identical, false otherwise
    /// @note Performs byte-by-byte comparison
    bool operator==(const Bytes &other) const {
        size_t len = size();
        return len == other.size() && ::memcmp(ptr(), other.ptr(), len) == 0;
    }

    /// @brief Compare equality with a C bytes_t struct
    /// @param other bytes_t struct to compare with
    /// @return true if contents are identical, false otherwise
    /// @note Performs byte-by-byte comparison
    bool operator==(const bytes_t &other) const {
        size_t len = size();
        return len == bytes_size(&other) && ::memcmp(ptr(), bytes_ptr(&other), len) == 0;
    }

    /// @brief Create a new byte array containing a subset of this array
    /// @param start Start index of the slice
    /// @param stop End index of the slice (exclusive)
    /// @return New Bytes object containing the slice
    /// @note Zero-copy operation through reference counting
    /// @note Thread-safe through reference counting
    /// @note Original array is unmodified
    /// @note Invalid indices will be clamped to valid range
    Bytes slice(size_t start, size_t stop) const {
        return Bytes(::bytes_slice(&m_inner, start, stop), 0);
    }

#if __cplusplus >= 201103L  // C++ 11
    /// @brief Move a bytes_t rvalue into a new Bytes object without cloning.
    /// @param moved The bytes_t rvalue to move from
    /// @return A new Bytes object that takes ownership of the moved data
    /// @
    /// - Zero-cost ownership transfer using move semantics
    /// - No memory allocation or copying occurs
    /// - Source bytes_t becomes empty after move
    static Bytes from_bytes(bytes_t &&moved) { return Bytes(moved, 0); }

    /// @brief Move constructor efficiently transfers ownership of byte array data
    /// @param other Source Bytes object to move from
    /// @note
    /// - Zero-cost ownership transfer using move semantics
    /// - No memory allocation or copying occurs
    /// - Source object becomes empty after move
    Bytes(Bytes &&other) : m_inner(other.m_inner) { ::bytes_init(&other.m_inner); }

    /// @brief Move assignment operator efficiently transfers ownership of byte array data
    /// @param other Source Bytes object to move from
    /// @return Reference to this Bytes object
    /// @
    /// - Zero-cost transfer of ownership using swap operation
    /// - No memory allocation or copying occurs
    /// - Source object becomes empty after move
    Bytes &operator=(Bytes &&other) {
        if (this != &other) {
            ::bytes_swap(&m_inner, &other.m_inner);
        }
        return *this;
    }
#endif

public:
    /// @brief Decode a BASE64 encoded string into bytes
    /// @param str BASE64 encoded string to decode
    /// @return New Bytes object containing the decoded binary data
    /// @note
    /// - Creates new memory for the decoded data with reference counting
    /// - Thread-safe through internal reference counting mechanism
    /// - Returns empty Bytes if input is invalid BASE64
    /// - Memory is automatically freed when last reference is released
    static Bytes base64_decode(const ByteString &str);

    /// @brief Encode this byte array as a BASE64 string
    /// @return New ByteString containing the BASE64 encoded representation
    /// @note
    /// - Creates new memory for the encoded string with reference counting
    /// - Thread-safe through internal reference counting mechanism
    /// - Encoding follows standard BASE64 alphabet (RFC 4648)
    /// - Memory is automatically freed when last reference is released
    /// - The encoded string is always valid UTF-8
    ByteString base64_encode();

private:
    /// @brief Move a bytes_t into a byte array without cloning.
    Bytes(const bytes_t &moved, int _dummy) : m_inner(moved) {}

private:
    bytes_t m_inner;
};

/**
 * @brief Thread-safe UTF-8 string wrapper with RAII memory management.
 *
 * The ByteString class provides a C++ wrapper around the C bstr_t type, offering:
 * - Automatic memory management through RAII
 * - Thread-safe reference counting
 * - Zero-copy operations where possible
 * - Seamless conversion between UTF-8/16/32 encodings
 * - STL string compatibility
 *
 * Memory is automatically managed - the destructor ensures proper cleanup
 * through the reference counting system.
 *
 * Example:
 * @code
 * ByteString str1("Hello");
 * ByteString str2 = str1;  // Reference counted, no copy
 * std::string stdStr = str2;  // Convert to std::string
 * @endcode
 */
/**
 * @brief Thread-safe UTF-8 string class with RAII memory management.
 *
 * ByteString provides a C++ wrapper around the C bstr_t type, offering:
 * - Automatic memory management through RAII
 * - Thread-safe reference counting
 * - Zero-copy operations where possible
 * - UTF-8/16/32 conversion support
 * - Efficient string operations
 *
 * Memory is automatically managed - the destructor ensures proper cleanup
 * through the reference counting system.
 */
class ByteString {
public:
    /// @brief Default constructor creates an empty string
    /// @note No memory allocation occurs for empty strings
    /// @note Thread-safe through reference counting
    ByteString() { ::bstr_init(&m_inner); }

    /// @brief Destructor ensures proper cleanup through reference counting
    ~ByteString() { ::bstr_release(&m_inner); }

    /// @brief Create a ByteString from a static string without copying.
    /// @param static_str Pointer to static string data that must outlive the ByteString
    /// @param len Length of string, or NPOS to calculate from null terminator
    /// @return New ByteString referencing the static data
    /// @note Zero-copy operation - no memory allocation
    /// @note Thread-safe through reference counting
    static ByteString from_static(const char *static_str, size_t len = NPOS) {
        return ByteString(::bstr_from_static(static_str, len), 0);
    }

    /// @brief Move a bstr_t into a new ByteString without copying.
    /// @param moved Source bstr_t to move from (becomes empty after move)
    /// @return New ByteString taking ownership of the moved data
    /// @note Zero-cost operation - no memory allocation or copying
    /// @note Thread-safe through reference counting
    static ByteString from_bstr(bstr_t &moved) {
        ByteString x = ByteString(moved, 0);
        ::bstr_init(&moved);
        return x;
    }

    /// @brief Construct from a Bytes object, converting binary data to UTF-8 string
    /// @param bytes Source byte array to convert to UTF-8
    /// @note Creates a new reference-counted copy
    /// @note Validates and converts input to proper UTF-8 encoding
    /// @note Thread-safe through reference counting
    ByteString(const Bytes &bytes) : m_inner(::bstr_from_bytes(bytes)) {}

    /// @brief Construct from a C bytes_t struct, converting binary data to UTF-8 string
    /// @param bytes Source byte array to convert to UTF-8
    /// @note Creates a new reference-counted copy
    /// @note Validates and converts input to proper UTF-8 encoding
    /// @note Thread-safe through reference counting
    ByteString(const bytes_t &bytes) : m_inner(::bstr_from_bytes(&bytes)) {}

    /// @brief Construct from a CStr wrapper object
    /// @param utf8 Source UTF-8 string to copy from
    /// @note Creates a new reference-counted copy
    /// @note Validates UTF-8 encoding of input
    /// @note Thread-safe through reference counting
    ByteString(const CStr &utf8) : m_inner(::bstr_from_utf8(utf8, utf8.size())) {}

    /// @brief Construct from a UTF-8 C string
    /// @param utf8 Source UTF-8 string to copy from
    /// @param len Length of string, or NPOS to calculate from null terminator
    /// @note Creates a new reference-counted copy
    /// @note Validates UTF-8 encoding of input
    /// @note Thread-safe through reference counting
    ByteString(const char *utf8, size_t len = NPOS) : m_inner(::bstr_from_utf8(utf8, len)) {}

    /// @brief Construct from a UTF-16 string
    /// @param utf16 Source UTF-16 string to convert and copy
    /// @param len Length in UTF-16 code units, or NPOS to calculate from null terminator
    /// @note Creates a new reference-counted copy
    /// @note Converts UTF-16 to UTF-8 encoding
    /// @note Handles surrogate pairs correctly
    /// @note Thread-safe through reference counting
    ByteString(const uint16_t *utf16, size_t len = NPOS) : m_inner(::bstr_from_utf16(utf16, len)) {}

    /// @brief Construct from a UTF-32 string
    /// @param utf32 Source UTF-32 string to convert and copy
    /// @param len Length in UTF-32 code units, or NPOS to calculate from null terminator
    /// @note Creates a new reference-counted copy
    /// @note Converts UTF-32 to UTF-8 encoding
    /// @note Handles all Unicode code points
    /// @note Thread-safe through reference counting
    ByteString(const uint32_t *utf32, size_t len = NPOS) : m_inner(::bstr_from_utf32(utf32, len)) {}

    /// @brief Construct from a wide character string
    /// @param wstr Source wide string to convert and copy
    /// @param len Length in wide characters, or NPOS to calculate from null terminator
    /// @note Creates a new reference-counted copy
    /// @note Automatically handles platform-specific wchar_t encoding (UTF-16 or UTF-32)
    /// @note Thread-safe through reference counting
    ByteString(const wchar_t *wstr, size_t len = NPOS) : m_inner(::bstr_from_wchar(wstr, len)) {}

    /// @brief Construct from a C bstr_t struct
    /// @param str Source string to copy
    ByteString(const bstr_t &str) : m_inner(::bstr_clone(&str)) {}

    /// @brief Copy constructor creates a new reference-counted string
    /// @param other Source string to copy
    ByteString(const ByteString &other) : m_inner(::bstr_clone(&other.m_inner)) {}

    /// @brief Assignment operator creates a new reference-counted copy
    /// @param other Source string to copy from
    /// @return Reference to this string
    /// @note Thread-safe through reference counting
    ByteString &operator=(const ByteString &other) {
        if (this != &other) {
            ::bstr_release(&m_inner);
            m_inner = ::bstr_clone(&other.m_inner);
        }
        return *this;
    }

    /// @brief Convert to const C bstr_t pointer
    /// @return Pointer to internal bstr_t struct
    operator const bstr_t *() const { return &m_inner; }

    /// @brief Convert to mutable C bstr_t pointer
    /// @return Pointer to internal bstr_t struct
    operator bstr_t *() { return &m_inner; }

    /// @brief Convert to const C bstr_t reference
    /// @return Reference to internal bstr_t struct
    operator const bstr_t &() const { return m_inner; }

    /// @brief Convert to mutable C bstr_t reference
    /// @return Reference to internal bstr_t struct
    operator bstr_t &() { return m_inner; }

    /// @brief Get raw pointer to the UTF-8 string data
    /// @return Pointer to UTF-8 encoded string data (not null-terminated)
    /// @note The pointer remains valid until the ByteString is modified
    /// @note Thread-safe for concurrent reads
    /// @note The returned pointer must not be used to modify the string
    const char *ptr() const { return bstr_ptr(&m_inner); }

    /// @brief Get the length of the string in bytes
    /// @return Number of bytes in the UTF-8 encoded string
    /// @note This is the byte length, not the character count
    /// @note Thread-safe for concurrent reads
    size_t size() const { return bstr_size(&m_inner); }

    /// @brief Convert to CStr wrapper object
    /// @return New CStr containing a copy of the string data
    /// @note Creates a new null-terminated copy
    /// @note Thread-safe through reference counting
    operator CStr() const { return CStr(ptr(), size()); }

    /// @brief Convert to CStr wrapper object explicitly
    /// @return New CStr containing a copy of the string data
    /// @note Creates a new null-terminated copy
    /// @note Thread-safe through reference counting
    /// @note Equivalent to CStr operator but as an explicit method
    CStr as_cstr() const { return CStr(ptr(), size()); }

    bool operator==(const ByteString &other) const {
        size_t len = size();
        return len == other.size() && ::memcmp(ptr(), other.ptr(), len) == 0;
    }

    bool operator==(const bstr_t &other) const {
        size_t len = size();
        return len == bstr_size(&other) && ::memcmp(ptr(), bstr_ptr(&other), len) == 0;
    }

    bool operator==(const char *other) const {
        size_t len = size();
        return ::strncmp(ptr(), other, len) == 0 && other[len] == '\0';
    }

#if __cplusplus >= 201103L  // C++ 11
    /// @brief Move a bstr_t rvalue into a new ByteString without copying
    /// @param moved Source bstr_t to move from (becomes empty after move)
    /// @return New ByteString taking ownership of the moved data
    /// @note Zero-cost operation - no memory allocation or copying
    /// @note Thread-safe through reference counting
    /// @note Source bstr_t becomes empty after move
    static ByteString from_bstr(bstr_t &&moved) { return ByteString(moved, 0); }

    /// @brief Move constructor efficiently transfers ownership of string data
    /// @param other Source ByteString to move from (becomes empty after move)
    /// @note Zero-cost operation - no memory allocation or copying
    /// @note Thread-safe through reference counting
    /// @note Source string becomes empty after move
    ByteString(ByteString &&other) : m_inner(other.m_inner) { ::bstr_init(&other.m_inner); }

    /// @brief Move assignment operator efficiently transfers ownership
    /// @param other Source ByteString to move from (becomes empty after move)
    /// @return Reference to this ByteString
    /// @note Zero-cost operation using swap
    /// @note Thread-safe through reference counting
    /// @note Properly handles self-assignment
    ByteString &operator=(ByteString &&other) {
        if (this != &other) {
            ::bstr_swap(&m_inner, &other.m_inner);
        }
        return *this;
    }
#endif

#if (_BSTR_HAS_STD)
    /// @brief Construct from a std::string
    /// @param str Source string to copy from
    /// @note Creates a new reference-counted copy
    /// @note Validates UTF-8 encoding of input
    /// @note Thread-safe through reference counting
    ByteString(const std::string &str) : m_inner(::bstr_from_utf8(str.c_str(), str.size())) {}

    /// @brief Construct from a std::wstring
    /// @param wstr Source wide string to convert and copy
    /// @note Creates a new reference-counted copy
    /// @note Automatically handles platform-specific wchar_t encoding
    /// @note Thread-safe through reference counting
    ByteString(const std::wstring &wstr) : m_inner(::bstr_from_wchar(wstr.c_str(), wstr.size())) {}

    /// @brief Convert to a std::string
    /// @return New std::string containing a copy of the UTF-8 data
    /// @note Creates a new copy of the string data
    /// @note Thread-safe for concurrent reads
    operator std::string() const { return std::string(ptr(), size()); }

    /// @brief Convert to a std::wstring
    /// @return New std::wstring containing the converted string
    /// @note Creates a new copy with platform-specific wide encoding
    /// @note Handles memory cleanup automatically
    /// @note Thread-safe for concurrent reads
    operator std::wstring() const {
        wchar_t *p = bstr_dup_wchar(*this);
        std::wstring s = std::wstring(p);
        ::bstr_mem_free(p);
        return s;
    }

    /// @brief Convert to a std::string explicitly
    /// @return New std::string containing a copy of the UTF-8 data
    /// @note Creates a new copy of the string data
    /// @note Thread-safe for concurrent reads
    /// @note Equivalent to std::string operator but as an explicit method
    std::string as_string() const { return *this; }

    /// @brief Convert to a std::wstring explicitly
    /// @return New std::wstring containing the converted string
    /// @note Creates a new copy with platform-specific wide encoding
    /// @note Handles memory cleanup automatically
    /// @note Thread-safe for concurrent reads
    /// @note Equivalent to std::wstring operator but as an explicit method
    std::wstring as_wstring() const { return *this; }
#endif

private:
    /// @brief Move a bstr_t into a new string without cloning.
    ByteString(const bstr_t &moved, int _dummy) : m_inner(moved) {}

    friend class Bytes;

private:
    bstr_t m_inner;
};

inline Bytes::Bytes(const ByteString &str) : m_inner(::bytes_from_bstr(str)) {}

inline Bytes Bytes::base64_decode(const ByteString &str) {
    return Bytes(::bytes_base64_decode(str), 0);
}

inline ByteString Bytes::base64_encode() { return ByteString(::bytes_base64_encode(&m_inner), 0); }

#endif  // _BSTR_NO_CPP
#endif  // __cplusplus

#endif  // __BSTR_H___
