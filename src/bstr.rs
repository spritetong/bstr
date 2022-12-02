#![allow(dead_code)]
#![allow(non_camel_case_types)]
#![allow(improper_ctypes_definitions)]
#![allow(clippy::missing_safety_doc)]

use ::base64;
use ::bytes::Bytes;
use ::bytestring::ByteString;
use ::libc;
use ::std::{
    mem,
    os::raw::*,
    ptr::{self, null_mut},
    slice,
};

////////////////////////////////////////////////////////////////////////////////

pub type bytes_t = Bytes;

#[no_mangle]
pub unsafe extern "C" fn bytes_init(buf: *mut bytes_t) {
    if !buf.is_null() {
        let s = Bytes::new();
        ptr::copy_nonoverlapping(&s, buf, 1);
        mem::forget(s);
    }
}

#[no_mangle]
pub extern "C" fn bytes_new() -> bytes_t {
    Bytes::new()
}

#[no_mangle]
pub unsafe extern "C" fn bytes_from_static(static_data: *const c_void, len: usize) -> bytes_t {
    if static_data.is_null() {
        Bytes::new()
    } else {
        Bytes::from_static(slice::from_raw_parts(static_data as _, len))
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_from_bstr(s: *const bstr_t) -> bytes_t {
    if s.is_null() {
        Bytes::new()
    } else {
        (*s).clone().into_bytes()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_copy_from_slice(data: *const c_void, len: usize) -> bytes_t {
    if data.is_null() || len == 0 {
        Bytes::new()
    } else {
        Bytes::copy_from_slice(slice::from_raw_parts(data as _, len))
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_slice(bytes: *const bytes_t, start: usize, stop: usize) -> bytes_t {
    if bytes.is_null() {
        Bytes::new()
    } else {
        let bytes = &*bytes;
        let stop = stop.min(bytes.len());
        bytes.slice(start.min(stop)..stop)
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_clone(bytes: *const bytes_t) -> bytes_t {
    if bytes.is_null() {
        Bytes::new()
    } else {
        (*bytes).clone()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_release(s: *mut bytes_t) {
    if !s.is_null() {
        drop(mem::replace(&mut *s, Bytes::new()));
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_swap(a: *mut bytes_t, b: *mut bytes_t) {
    if !a.is_null() && !b.is_null() && !ptr::eq(a, b) {
        // Swap operation is safe because there's no overlap.
        #[allow(clippy::swap_ptr_to_ref)]
        mem::swap(&mut *a, &mut *b);
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_base64_decode(str: *const bstr_t) -> bytes_t {
    if str.is_null() {
        Bytes::new()
    } else {
        base64::decode(&*str).unwrap_or_default().into()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bytes_base64_encode(bytes: *const bytes_t) -> bstr_t {
    if bytes.is_null() {
        ByteString::new()
    } else {
        base64::encode(&*bytes).into()
    }
}

////////////////////////////////////////////////////////////////////////////////

pub type bstr_t = ByteString;

#[no_mangle]
pub unsafe extern "C" fn bstr_init(buf: *mut bstr_t) {
    if !buf.is_null() {
        let s = ByteString::new();
        ptr::copy_nonoverlapping(&s, buf, 1);
        mem::forget(s);
    }
}

#[no_mangle]
pub extern "C" fn bstr_new() -> bstr_t {
    ByteString::new()
}

#[no_mangle]
pub unsafe extern "C" fn bstr_from_bytes(bytes: *const bytes_t) -> bstr_t {
    if bytes.is_null() {
        ByteString::new()
    } else {
        let bytes = &*bytes;
        if let Ok(s) = ByteString::try_from(bytes.clone()) {
            return s;
        }
        String::from_utf8_lossy(bytes).into_owned().into()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_from_static(static_str: *const c_char, len: usize) -> bstr_t {
    if static_str.is_null() {
        ByteString::new()
    } else {
        let len = if len == usize::MAX {
            libc::strlen(static_str)
        } else {
            len
        };
        ByteString::from_static(mem::transmute(slice::from_raw_parts(static_str, len)))
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_from_utf8(str: *const c_char, len: usize) -> bstr_t {
    if str.is_null() {
        ByteString::new()
    } else {
        let len = if len == usize::MAX {
            libc::strlen(str)
        } else {
            len
        };
        String::from_utf8_lossy(slice::from_raw_parts(str as _, len as _))
            .into_owned()
            .into()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_from_utf16(str: *const u16, mut len: usize) -> bstr_t {
    if str.is_null() {
        ByteString::new()
    } else {
        if len == usize::MAX {
            len = 0;
            while *str.add(len as _) != 0 {
                len += 1;
            }
        }
        String::from_utf16_lossy(slice::from_raw_parts(str, len as _)).into()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_from_utf32(str: *const u32, mut len: usize) -> bstr_t {
    if str.is_null() {
        ByteString::new()
    } else {
        if len == usize::MAX {
            len = 0;
            while *str.add(len as _) != 0 {
                len += 1;
            }
        }
        slice::from_raw_parts(str as *const char, len as _)
            .iter()
            .collect::<String>()
            .into()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_clone(s: *const bstr_t) -> bstr_t {
    if s.is_null() {
        ByteString::new()
    } else {
        (*s).clone()
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_release(s: *mut bstr_t) {
    if !s.is_null() {
        drop(mem::replace(&mut *s, ByteString::new()));
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_swap(a: *mut bstr_t, b: *mut bstr_t) {
    if !a.is_null() && !b.is_null() && !ptr::eq(a, b) {
        // Swap operation is safe because there's no overlap.
        #[allow(clippy::swap_ptr_to_ref)]
        mem::swap(&mut *a, &mut *b);
    }
}

#[no_mangle]
pub unsafe extern "C" fn bstr_dup_utf8(s: *const bstr_t) -> *mut c_char {
    if !s.is_null() {
        let s = &*s;
        let p = libc::malloc(s.len() + 1) as *mut u8;
        if !p.is_null() {
            ptr::copy_nonoverlapping(s.as_ptr(), p, s.len());
            *p.add(s.len()) = 0;
            return p as _;
        }
    }
    null_mut()
}

#[no_mangle]
pub unsafe extern "C" fn bstr_dup_utf16(s: *const bstr_t) -> *mut u16 {
    if !s.is_null() {
        let s = &*s;
        let p = libc::malloc((s.len() + 1) * 2) as *mut u16;
        if !p.is_null() {
            let mut i = 0;
            for x in s.encode_utf16() {
                *p.add(i) = x;
                i += 1;
            }
            *p.add(i) = 0;
            return if s.len() >= i + 64 / 2 {
                return libc::realloc(p as _, (i + 1) * 2) as _;
            } else {
                p
            };
        }
    }
    null_mut()
}

#[no_mangle]
pub unsafe extern "C" fn bstr_dup_utf32(s: *const bstr_t) -> *mut u32 {
    if !s.is_null() {
        let s = &*s;
        let p = libc::malloc((s.len() + 1) * 4) as *mut u32;
        if !p.is_null() {
            let mut i = 0;
            for x in s.chars() {
                *p.add(i) = x as _;
                i += 1;
            }
            *p.add(i) = 0;
            return if s.len() >= i + 64 / 4 {
                libc::realloc(p as _, (i + 1) * 4) as _
            } else {
                p
            };
        }
    }
    null_mut()
}

#[no_mangle]
pub unsafe extern "C" fn bstr_mem_free(ptr: *mut c_void) {
    libc::free(ptr)
}
