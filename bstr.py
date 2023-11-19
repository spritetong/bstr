#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import itertools
from ctypes import *
from typing import *


__all__ = ('Bytes', 'Bstr', 'bstr_api')


class Bytes(Structure):
    _fields_ = [('reserved1', c_void_p),
                ('reserved2', c_void_p),
                ('reserved3', c_void_p),
                ('reserved4', c_void_p), ]

    def __init__(self, s: Optional[Union[int, 'Bytes', 'Bstr', c_char_p, bytes]] = None):
        api.bytes_init(pointer(self))
        if isinstance(s, int):
            if s < 0 or s >= 0xFFFFFFFF:
                raise ArgumentError('Invalid array length: {}'.format(s))
            api.bytes_swap(pointer(self), pointer(api.bytes_alloc(s)))
        elif isinstance(s, Bytes):
            api.bytes_swap(pointer(self), pointer(api.bytes_clone(pointer(s))))
        elif isinstance(s, POINTER(Bytes)):
            api.bytes_swap(pointer(self), pointer(api.bytes_clone(s)))
        elif isinstance(s, Bstr):
            api.bytes_swap(pointer(self), pointer(
                api.bytes_from_bstr(pointer(s))))
        elif isinstance(s, POINTER(Bstr)):
            api.bytes_swap(pointer(self), pointer(api.bytes_from_bstr(s)))
        elif isinstance(s, c_char_p):
            api.bytes_swap(pointer(self), pointer(
                api.bytes_copy_from_slice(s, len(s.value))))
        elif isinstance(s, bytes):
            api.bytes_swap(pointer(self), pointer(
                api.bytes_copy_from_slice(s, len(s))))
        elif s is None:
            pass
        else:
            raise TypeError('Invalid type: {}'.format(type(s)))

    def __del__(self):
        api.bytes_release(pointer(self))

    def __len__(self):
        return api.bytes_size(pointer(self))

    def __getitem__(self, key):
        if isinstance(key, slice):
            # Get the start, stop and step from the slice.
            if key.step == 1:
                (start, stop, _) = key.indices(len(self))
                return api.bytes_slice(pointer(self), start, stop)
            return Bytes(string_at(self.ptr(), len(self)).__getitem__(key))
        elif isinstance(key, int):
            if key < 0:  # Handle negative indices
                key += len(self)
            if key < 0 or key >= len(self):
                raise IndexError("The index ({}) is out of range.".format(key))
            return c_uint8.from_address(self.ptr() + key).value
        else:
            raise IndexError("Invalid argument type.")

    def __copy__(self) -> 'Bytes':
        return api.bytes_clone(pointer(self))

    def __deepcopy__(self) -> 'Bytes':
        return api.bytes_clone(pointer(self))

    def __str__(self) -> str:
        return str(self.bytes())

    def ptr(self) -> c_void_p:
        return api.bytes_ptr(pointer(self))

    def copy(self) -> 'Bytes':
        return api.bytes_clone(pointer(self))

    def bytes(self) -> bytes:
        return string_at(self.ptr(), len(self))

    def base64_encode(self) -> 'Bstr':
        return api.bytes_base64_encode(pointer(self))

    @staticmethod
    def base64_decode(s: Union['Bstr', 'Bytes', c_char_p, bytes, c_wchar_p, str]) -> 'Bytes':
        return api.bytes_base64_decode(pointer(Bstr(s)))


class Bstr(Structure):
    NPOS = c_size_t(-1)

    _fields_ = [('reserved1', c_void_p),
                ('reserved2', c_void_p),
                ('reserved3', c_void_p),
                ('reserved4', c_void_p), ]

    def __init__(self, s: Optional[Union['Bstr', 'Bytes', c_char_p, bytes, c_wchar_p, str]] = None):
        api.bstr_init(pointer(self))
        if isinstance(s, Bstr):
            api.bstr_swap(pointer(self), pointer(api.bstr_clone(pointer(s))))
        elif isinstance(s, POINTER(Bstr)):
            api.bstr_swap(pointer(self), pointer(api.bstr_clone(s)))
        elif isinstance(s, Bytes):
            api.bstr_swap(pointer(self), pointer(
                api.bstr_from_bytes(pointer(s))))
        elif isinstance(s, POINTER(Bytes)):
            api.bstr_swap(pointer(self), pointer(api.bstr_from_bytes(s)))
        elif isinstance(s, c_char_p):
            api.bstr_swap(pointer(self), pointer(
                api.bstr_from_utf8(s, Bstr.NPOS)))
        elif isinstance(s, bytes):
            api.bstr_swap(pointer(self), pointer(
                api.bstr_from_utf8(s, len(s))))
        elif isinstance(s, c_wchar_p):
            api.bstr_swap(pointer(self), pointer(
                api.bstr_from_wchar(s, Bstr.NPOS)))
        elif isinstance(s, str):
            api.bstr_swap(pointer(self), pointer(
                api.bstr_from_wchar(s, len(s))))
        elif s is None:
            pass
        else:
            raise TypeError('Invalid type: {}'.format(type(s)))

    def __del__(self):
        api.bstr_release(pointer(self))

    def __len__(self):
        return api.bstr_size(pointer(self))

    def __copy__(self) -> 'Bstr':
        return api.bstr_clone(pointer(self))

    def __deepcopy__(self) -> 'Bstr':
        return api.bstr_clone(pointer(self))

    def __str__(self) -> str:
        return self.str()

    def ptr(self) -> c_void_p:
        return api.bstr_ptr(pointer(self))

    def copy(self) -> 'Bstr':
        return api.bstr_clone(pointer(self))

    def bytes(self) -> bytes:
        return string_at(self.ptr(), len(self))

    def str(self) -> str:
        return string_at(self.ptr(), len(self)).decode('utf-8')


class BstrApi:
    def __init__(self):
        self.dll_path = ''
        self.dll_name = ''
        self.dll: CDLL = None

    def load_library(self, dll: Union[str, CDLL]) -> CDLL:
        if self.dll is None:
            if isinstance(dll, str):
                dll = self._load_dll(dll)
            elif isinstance(dll, CDLL):
                self.dll = dll
            else:
                raise TypeError('Invalid argument type {}'.format(type(dll)))
        elif isinstance(dll, CDLL) and dll is not self.dll:
            raise RuntimeError(
                'Can not initialize API library {} more than once'.format(dll))
        else:
            dll = self.dll

        ####################################################################

        prototype = CFUNCTYPE(None, POINTER(Bytes))
        self.bytes_init = prototype(('bytes_init', dll))

        prototype = CFUNCTYPE(c_void_p, POINTER(Bytes))
        self.bytes_ptr = prototype(('bytes_ptr', dll))

        prototype = CFUNCTYPE(c_size_t, POINTER(Bytes))
        self.bytes_size = prototype(('bytes_size', dll))

        prototype = CFUNCTYPE(Bytes)
        self.bytes_new = prototype(('bytes_new', dll))

        prototype = CFUNCTYPE(Bytes, c_size_t)
        self.bytes_alloc = prototype(('bytes_alloc', dll))

        prototype = CFUNCTYPE(Bytes, c_size_t)
        self.bytes_zalloc = prototype(('bytes_zalloc', dll))

        prototype = CFUNCTYPE(Bytes, c_void_p, c_size_t)
        self.bytes_from_static = prototype(
            ('bytes_from_static', dll))

        prototype = CFUNCTYPE(Bytes, POINTER(Bstr))
        self.bytes_from_bstr = prototype(
            ('bytes_from_bstr', dll))

        prototype = CFUNCTYPE(Bytes, c_void_p, c_size_t)
        self.bytes_copy_from_slice = prototype(
            ('bytes_copy_from_slice', dll))

        prototype = CFUNCTYPE(Bytes, POINTER(Bytes), c_size_t, c_size_t)
        self.bytes_slice = prototype(
            ('bytes_slice', dll))

        prototype = CFUNCTYPE(Bytes, POINTER(Bytes))
        self.bytes_clone = prototype(
            ('bytes_clone', dll))

        prototype = CFUNCTYPE(POINTER(Bytes))
        self.bytes_release = prototype(
            ('bytes_release', dll))

        prototype = CFUNCTYPE(None, POINTER(Bytes), POINTER(Bytes))
        self.bytes_swap = prototype(
            ('bytes_swap', dll))

        prototype = CFUNCTYPE(Bytes, POINTER(Bstr))
        self.bytes_base64_decode = prototype(
            ('bytes_base64_decode', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(Bytes))
        self.bytes_base64_encode = prototype(
            ('bytes_base64_encode', dll))

        ####################################################################

        prototype = CFUNCTYPE(None, POINTER(Bstr))
        self.bstr_init = prototype(('bstr_init', dll))

        prototype = CFUNCTYPE(c_void_p, POINTER(Bstr))
        self.bstr_ptr = prototype(('bstr_ptr', dll))

        prototype = CFUNCTYPE(c_size_t, POINTER(Bstr))
        self.bstr_size = prototype(('bstr_size', dll))

        prototype = CFUNCTYPE(Bstr)
        self.bstr_new = prototype(('bstr_new', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_char), c_size_t)
        self.bstr_from_static = prototype(
            ('bstr_from_static', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(Bytes))
        self.bstr_from_bytes = prototype(
            ('bstr_from_bytes', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_char), c_size_t)
        self.bstr_from_utf8 = prototype(
            ('bstr_from_utf8', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_uint16), c_size_t)
        self.bstr_from_utf16 = prototype(
            ('bstr_from_utf16', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_uint32), c_size_t)
        self.bstr_from_utf32 = prototype(
            ('bstr_from_utf32', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_wchar), c_size_t)
        if sizeof(c_wchar) == 2:
            self.bstr_from_wchar = prototype(
                ('bstr_from_utf16', dll))
        elif sizeof(c_wchar) == 4:
            self.bstr_from_wchar = prototype(
                ('bstr_from_utf32', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(Bstr))
        self.bstr_clone = prototype(
            ('bstr_clone', dll))

        prototype = CFUNCTYPE(POINTER(Bstr))
        self.bstr_release = prototype(
            ('bstr_release', dll))

        prototype = CFUNCTYPE(None, POINTER(Bstr), POINTER(Bstr))
        self.bstr_swap = prototype(
            ('bstr_swap', dll))

        prototype = CFUNCTYPE(c_char_p, POINTER(Bstr))
        self.bstr_dup_utf8 = prototype(
            ('bstr_dup_utf8', dll))

        prototype = CFUNCTYPE(POINTER(c_uint16), POINTER(Bstr))
        self.bstr_dup_utf16 = prototype(
            ('bstr_dup_utf16', dll))

        prototype = CFUNCTYPE(POINTER(c_uint32), POINTER(Bstr))
        self.bstr_dup_utf32 = prototype(
            ('bstr_dup_utf32', dll))

        prototype = CFUNCTYPE(c_wchar_p, POINTER(Bstr))
        if sizeof(c_wchar) == 2:
            self.bstr_dup_wchar = prototype(
                ('bstr_dup_utf16', dll))
        elif sizeof(c_wchar) == 4:
            self.bstr_dup_wchar = prototype(
                ('bstr_dup_utf32', dll))

        prototype = CFUNCTYPE(None, c_void_p)
        self.bstr_mem_free = prototype(
            ('bstr_mem_free', dll))

        return dll

    def _load_dll(self, dll_name: str) -> CDLL:
        if self.dll is not None:
            return self.dll

        def search_path():
            # '' and '.'
            yield ''
            yield os.path.realpath('.')
            # in <workspace dir>/target/
            path = os.path.realpath(os.path.dirname(__file__))
            while path:
                target = os.path.join(path, 'target')
                if os.path.isdir(target):
                    dirs = [target]
                    for item in os.listdir(target):
                        dir = os.path.join(target, item)
                        # matches triple like "x86_64-pc-windows-msvc"
                        if os.path.isdir(dir) and len(item.split('-')) in (3, 4):
                            dirs.append(dir)
                    for suffix in itertools.product(dirs, ['debug', 'release']):
                        dir = os.path.join(*suffix)
                        if os.path.isdir(dir):
                            yield dir
                    break
                parent = os.path.dirname(path)
                if path == parent:
                    break
                path = parent

        for dir in search_path():
            try:
                if self.dll is None:
                    dll_path = os.path.join(dir, dll_name)
                    self.dll = cdll.LoadLibrary(dll_path)
                    self.dll_path = dll_path
                    self.dll_name = dll_name
                    break
            except OSError:
                pass

        if self.dll is None:
            raise RuntimeError("Can not load library {}".format(dll_name))
        return self.dll


api = BstrApi()


def bstr_api() -> BstrApi:
    return api


if __name__ == '__main__':
    import copy

    bstr_api().load_library('bstr.dll')

    for _ in range(1):
        print(api.bstr_new())
        print(Bstr('134365'))
        print(Bstr('1343651'))
        print(Bstr(b'134365'))
        print(Bstr(c_char_p(b'134365')))
        print(api.bstr_clone(pointer(Bstr('1343651'))))

        print(api.bytes_new())
        assert len(Bytes(100)) == 100
        print(Bytes(b'134365'))
        print(Bytes(b'1343651'))
        print(Bytes(c_char_p(b'134365')))
        print(Bytes(Bstr(b'134365--')))
        print(api.bytes_clone(pointer(Bytes(b'1343651'))))
        a = copy.copy(Bytes(b'12345').copy())
        print(a, a[1], a[::2])
        print(a.base64_encode(), Bytes.base64_decode(a.base64_encode()))
        print(a.base64_encode(), Bytes.base64_decode(a.base64_encode().bytes()))
        print(a.base64_encode(), Bytes.base64_decode(a.base64_encode().str()))
        print(a.base64_encode(), Bytes.base64_decode(
            c_char_p(a.base64_encode().bytes())))
        print(a.base64_encode(), Bytes.base64_decode(
            c_wchar_p(a.base64_encode().str())))
