#ifndef __BSTR_H___
#define __BSTR_H___

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Cross-platform shared byte array.
typedef struct bytes_t {
    uint8_t *ptr;
    size_t len;
    void *reserved1;
    void *reserved2;
} bytes_t;

/// @brief Cross-platform shared UTF-8 string without nul terminator.
typedef struct bstr_t {
    char *ptr;
    size_t len;
    void *reserved1;
    void *reserved2;
} bstr_t;

/// Alias of std::string::npos
#ifndef NPOS
#define NPOS ((size_t)(-1))
#endif

////////////////////////////////////////////////////////////////////////////////

/// @brief Create an empty byte array in place.
/// @param [out] buf The buffer of the byte array to be intialized.
extern void bytes_init(bytes_t *buf);

/// @brief Create an empty byte array.
/// @return The new array which must be released by \ref bytes_release().
extern bytes_t bytes_new(void);

/// @brief Create by a static byte array.
/// @param [in] static_data Pointer to the static byte array.
/// @param [in] len Number of bytes of the array.
/// @return The new array which must be released by \ref bytes_release().
extern bytes_t bytes_from_static(const void *static_data, size_t len);

/// @brief Create by copying from a UTF-8 byte array.
/// @param [in] s The source string.
/// @return The new array which must be released by \ref bytes_release().
extern bytes_t bytes_from_bstr(const bstr_t *s);

/// @brief Create by copying from a bytes slice.
/// @param [in] data Pointer to the bytes slice.
/// @param [in] len Number of bytes of the array.
/// @return The new array which must be released by \ref bytes_release().
extern bytes_t bytes_copy_from_slice(const void *data, size_t len);

/// @brief Get a slice [start, stop) from a byte array. [0, bytes->len) represents the
/// whole array.
/// @param [in] bytes The byte array.
/// @param [in] start The start position (>= 0).
/// @param [in] stop The stop position (< bytes->len). NPOS indicates the end of
/// the array.
/// @return The slice which must be released by \ref bytes_release().
extern bytes_t bytes_slice(const bytes_t *bytes, size_t start, size_t stop);

/// @brief Clone a byte array.
/// @param [in] bytes The source byte array.
/// @return The new array which must be released by \ref bytes_release().
extern bytes_t bytes_clone(const bytes_t *bytes);

/// @brief Release a byte array.
/// @param [in,out] bytes The array to be released.
extern void bytes_release(bytes_t *bytes);

/// @brief Swap two byte arrays.
/// @param [in,out] a
/// @param [in,out] b
extern void bytes_swap(bytes_t *a, bytes_t *b);

/// @brief Decode from a BAS64 string.
/// @param [in] str The BASE64 string.
/// @param [in] len if len != NPOS, it's length of the string; otherwise,
/// the string must be nul-terminated.
/// @return The new array which must be released by \ref bytes_release().
extern bytes_t bytes_base64_decode(const char *str, size_t len);

/// @brief Encode into a BAS64 string.
/// @param [in] bytes The BASE64 encoded string.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bytes_base64_encode(const bytes_t *bytes);

////////////////////////////////////////////////////////////////////////////////

/// @brief Create an empty string in place.
/// @param [out] buf The buffer of the string to be intialized.
extern void bstr_init(bstr_t *buf);

/// @brief Create an empty string.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bstr_new(void);

/// @brief Create by a static string.
/// @param [in] static_str Pointer to the static UTF-8 string.
/// @param [in] len if len != NPOS, it's length of the string; otherwise,
/// the string must be nul-terminated.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bstr_from_static(const char *static_str, size_t len);

/// @brief Create by copying from a UTF-8 byte array.
/// @param [in] bytes The source byte array.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bstr_from_bytes(const bytes_t *bytes);

/// @brief Create by copying from a UTF-8 string.
/// @param [in] str The source string.
/// @param [in] len if len != NPOS, it's length of the source string; otherwise,
/// the source string must be nul-terminated.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bstr_from_utf8(const char *str, size_t len);

/// @brief Create by copying from a UTF-16 string.
/// @param [in] str The source string.
/// @param [in] len if len != NPOS, it's length of the source string; otherwise,
/// the source string must be nul-terminated.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bstr_from_utf16(const uint16_t *str, size_t len);

/// @brief Create by copying from a UTF-32 string.
/// @param [in] str The source string.
/// @param [in] len if len != NPOS, it's length of the source string; otherwise,
/// the source string must be nul-terminated.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bstr_from_utf32(const uint32_t *str, size_t len);

/// @brief Clone a string.
/// @param [in] s The source string.
/// @return The new string which must be released by \ref bstr_release().
extern bstr_t bstr_clone(const bstr_t *s);

/// @brief Release a string.
/// @param [in,out] s The string to be released.
extern void bstr_release(bstr_t *s);

/// @brief Swap two strings.
/// @param [in,out] a
/// @param [in,out] b
extern void bstr_swap(bstr_t *a, bstr_t *b);

/// @brief Duplicate a string to a UTF-8 string with nul terminator.
/// @param [in,out] s The string to be duplicated.
/// @return A buffer contains a UTF-8 string with nul terminator.
///         This buffer must be deallocated by \ref bstr_mem_free() or free().
extern char *bstr_dup_utf8(const bstr_t *s);

/// @brief Duplicate a string to a UTF-16 string with nul terminator.
/// @param [in,out] s The string to be duplicated.
/// @return A buffer contains a UTF-16 string with nul terminator.
///         This buffer must be deallocated by \ref bstr_mem_free() or free().
extern uint16_t *bstr_dup_utf16(const bstr_t *s);

/// @brief Duplicate a string to a UTF-32 string with nul terminator.
/// @param [in,out] s The string to be duplicated.
/// @return A buffer contains a UTF-32 string with nul terminator.
///         This buffer must be deallocated by \ref bstr_mem_free() or free().
extern uint32_t *bstr_dup_utf32(const bstr_t *s);

/// @brief The wrapper function of C-language free()
/// @param [in] ptr
extern void bstr_mem_free(void *ptr);

#ifdef __cplusplus
}

////////////////////////////////////////////////////////////////////////////////

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

struct ByteString;

struct Bytes {
public:
    Bytes() : m_inner(::bytes_new()) {}
    ~Bytes() { ::bytes_release(&m_inner); }

    /// @brief Construct from a static bytes slice.
    /// @param static_data
    /// @param len
    /// @return Bytes
    static Bytes from_static(const void *static_data, size_t len) {
        return Bytes(::bytes_from_static(static_data, len), 0);
    }

    Bytes(const ByteString &str);

    Bytes(const bstr_t &str) : m_inner(::bytes_from_bstr(&str)) {}

    Bytes(const void *data, size_t len)
        : m_inner(::bytes_copy_from_slice(data, len)) {}

    Bytes(const bytes_t &bytes) : m_inner(::bytes_clone(&bytes)) {}

    Bytes(const Bytes &other) : m_inner(::bytes_clone(&other.m_inner)) {}

    Bytes &operator=(const Bytes &other) {
        if (this != &other) {
            ::bytes_release(&m_inner);
            m_inner = ::bytes_clone(&other.m_inner);
        }
        return *this;
    }

    // C++ 11
#if __cplusplus >= 201103L
    Bytes(Bytes &&other) : m_inner(::bytes_new()) {
        ::bytes_swap(&m_inner, &other.m_inner);
    }

    Bytes &operator=(Bytes &&other) {
        if (this != &other) {
            ::bytes_release(&m_inner);
            ::bytes_swap(&m_inner, &other.m_inner);
        }
        return *this;
    }
#endif

    operator const bytes_t *() const { return &m_inner; }
    operator bytes_t *() { return &m_inner; }

    operator const bytes_t &() const { return m_inner; }
    operator bytes_t &() { return m_inner; }

    /// @brief Get the pointer of the byte array.
    const uint8_t *ptr() const { return m_inner.ptr; }

    /// @brief Get number of bytes of the array.
    size_t size() const { return m_inner.len; }

    bool operator==(const Bytes &other) const {
        return m_inner.len == other.m_inner.len &&
               ::memcmp(m_inner.ptr, other.m_inner.ptr, m_inner.len) == 0;
    }

    bool operator==(const bytes_t &other) const {
        return m_inner.len == other.len &&
               ::memcmp(m_inner.ptr, other.ptr, m_inner.len) == 0;
    }

public:
    static Bytes base64_decode(const ByteString &str);
    ByteString base64_encode();

private:
    /// @brief Assign a byte array without clone.
    Bytes(const bytes_t &bytes, int _dummy) : m_inner(bytes) {}

private:
    bytes_t m_inner;
};

struct ByteString {
public:
    ByteString() : m_inner(::bstr_new()) {}
    ~ByteString() { ::bstr_release(&m_inner); }

    /// @brief Construct from a static string.
    /// @param static_str
    /// @return ByteString
    static ByteString from_static(const char *static_str, size_t len = NPOS) {
        return ByteString(::bstr_from_static(static_str, len), 0);
    }

    ByteString(const Bytes &bytes) : m_inner(::bstr_from_bytes(bytes)) {}

    ByteString(const bytes_t &bytes) : m_inner(::bstr_from_bytes(&bytes)) {}

    ByteString(const char *utf8, size_t len = NPOS)
        : m_inner(::bstr_from_utf8(utf8, len)) {}

    ByteString(const uint16_t *utf16, size_t len = NPOS)
        : m_inner(::bstr_from_utf16(utf16, len)) {}

    ByteString(const uint32_t *utf32, size_t len = NPOS)
        : m_inner(::bstr_from_utf32(utf32, len)) {}

    ByteString(const wchar_t *wstr, size_t len = NPOS)
        : m_inner(
#if WCHAR_MAX == 0xffff
              ::bstr_from_utf16((const uint16_t *)wstr, len)
#else
              ::bstr_from_utf32((const uint32_t *)wstr, len)
#endif
          ) {
    }

#if (_BSTR_HAS_STD)
    ByteString(const std::string &str) : ByteString(str.c_str(), str.size()) {}

    ByteString(const std::wstring &wstr)
        : ByteString(wstr.c_str(), wstr.size()) {}
#endif

    ByteString(const bstr_t &str) : m_inner(::bstr_clone(&str)) {}

    ByteString(const ByteString &other)
        : m_inner(::bstr_clone(&other.m_inner)) {}

    ByteString &operator=(const ByteString &other) {
        if (this != &other) {
            ::bstr_release(&m_inner);
            m_inner = ::bstr_clone(&other.m_inner);
        }
        return *this;
    }

    // C++ 11
#if __cplusplus >= 201103L
    ByteString(ByteString &&other) : m_inner(::bstr_new()) {
        ::bstr_swap(&m_inner, &other.m_inner);
    }

    ByteString &operator=(ByteString &&other) {
        if (this != &other) {
            ::bstr_release(&m_inner);
            ::bstr_swap(&m_inner, &other.m_inner);
        }
        return *this;
    }
#endif

    operator const bstr_t *() const { return &m_inner; }
    operator bstr_t *() { return &m_inner; }

    operator const bstr_t &() const { return m_inner; }
    operator bstr_t &() { return m_inner; }

    /// @brief Get the pointer of the string, it's a C string and has no nul
    /// terminator.
    const char *ptr() const { return m_inner.ptr; }

    /// @brief Get the length of the string
    size_t size() const { return m_inner.len; }

#if (_BSTR_HAS_STD)
    /// @brief Convert to a std::string.
    operator std::string() const {
        return std::string(m_inner.ptr, m_inner.len);
    }

    /// @brief Convert to a std::wstring.
    operator std::wstring() const {
        wchar_t *p =
#if WCHAR_MAX == 0xffff
            (wchar_t *)::bstr_dup_utf16(*this);
#else
            (wchar_t *)::bstr_dup_utf32(*this);
#endif
        std::wstring s = std::wstring(p);
        ::bstr_mem_free(p);
        return s;
    }

    /// @brief Convert to a std::string.
    std::string as_string() const { return *this; }

    /// @brief Convert to a std::wstring.
    std::wstring as_wstring() const { return *this; }
#endif

    bool operator==(const ByteString &other) const {
        return m_inner.len == other.m_inner.len &&
               ::memcmp(m_inner.ptr, other.m_inner.ptr, m_inner.len) == 0;
    }

    bool operator==(const bstr_t &other) const {
        return m_inner.len == other.len &&
               ::memcmp(m_inner.ptr, other.ptr, m_inner.len) == 0;
    }

    bool operator==(const char *other) const {
        return ::strncmp(m_inner.ptr, other, m_inner.len) == 0 &&
               other[m_inner.len] == '\0';
    }

private:
    /// @brief Assign a string without clone.
    ByteString(const bstr_t &str, int _dummy) : m_inner(str) {}

    friend class Bytes;

private:
    bstr_t m_inner;
};

inline Bytes::Bytes(const ByteString &str) : m_inner(::bytes_from_bstr(str)) {}

inline Bytes Bytes::base64_decode(const ByteString &str) {
    return Bytes(::bytes_base64_decode(str.ptr(), str.size()), 0);
}

inline ByteString Bytes::base64_encode() {
    return ByteString(::bytes_base64_encode(&m_inner), 0);
}

#endif  // __cplusplus

#endif  // __BSTR_H___