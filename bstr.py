#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import builtins
import ctypes
import itertools
import os
import sys
from ctypes import (
    CDLL,
    CFUNCTYPE,
    POINTER,
    ArgumentError,
    c_char,
    c_char_p,
    c_size_t,
    c_uint8,
    c_uint16,
    c_uint32,
    c_void_p,
    c_wchar,
    c_wchar_p,
    pointer,
    string_at,
)
from typing import List, Optional, Union

__all__ = ('Bytes', 'Bstr', 'c_bytes_p', 'c_bstr_p', 'bstr_api')


class Bytes(ctypes.Structure):
    """A memory-safe byte array wrapper for the native bstr library.

    This class provides a Python interface to the native byte array implementation,
    with automatic memory management and safe operations. It supports various
    initialization methods and common byte array operations.

    Memory Management:
        - Automatically manages memory allocation and deallocation
        - Uses reference counting for safe memory handling
        - Thread-safe for basic operations

    Examples:
        >>> # Create from bytes
        >>> b = Bytes(b'Hello')
        >>> print(len(b))
        5

        >>> # Create empty array with size
        >>> b = Bytes(10)
        >>> print(len(b))
        10

        >>> # Create from base64 string
        >>> b = Bytes.base64_decode('SGVsbG8=')
        >>> print(b.bytes())
        b'Hello'
    """

    _fields_ = [
        ('reserved1', c_void_p),
        ('reserved2', c_void_p),
        ('reserved3', c_void_p),
        ('reserved4', c_void_p),
    ]

    def __init__(
        self, s: Optional[Union[int, 'Bytes', 'Bstr', c_char_p, bytes]] = None
    ):
        """Initialize a new Bytes instance.

        Args:
            s: The source to initialize from. Can be:
                - int: Create a new byte array with the specified size
                - Bytes: Clone from another Bytes instance
                - Bstr: Convert from a Bstr instance
                - c_char_p: Create from a C char pointer
                - bytes: Create from Python bytes
                - None: Create an empty byte array

        Raises:
            ArgumentError: If size is negative or too large
            TypeError: If source type is not supported
        """
        api.bytes_init(pointer(self))
        if isinstance(s, int):
            if s < 0 or s >= 0xFFFFFFFF:
                raise ArgumentError('Invalid array length: {}'.format(s))
            api.bytes_swap(pointer(self), pointer(api.bytes_alloc(s)))
        elif isinstance(s, Bytes):
            api.bytes_swap(pointer(self), pointer(api.bytes_clone(pointer(s))))
        elif isinstance(s, c_bytes_p):
            api.bytes_swap(pointer(self), pointer(api.bytes_clone(s)))
        elif isinstance(s, Bstr):
            api.bytes_swap(pointer(self), pointer(api.bytes_from_bstr(pointer(s))))
        elif isinstance(s, c_bstr_p):
            api.bytes_swap(pointer(self), pointer(api.bytes_from_bstr(s)))
        elif isinstance(s, c_char_p):
            api.bytes_swap(
                pointer(self),
                pointer(api.bytes_copy_from_slice(s, len(s.value) if s.value else 0)),
            )
        elif isinstance(s, bytes):
            api.bytes_swap(pointer(self), pointer(api.bytes_copy_from_slice(s, len(s))))
        elif s is None:
            pass
        else:
            raise TypeError('Invalid type: {}'.format(type(s)))

    @staticmethod
    def clone_from_address(address: Union[c_void_p, int]) -> 'Bytes':
        """Create a new Bytes instance by cloning from a memory address.

        Args:
            address: Memory address as void pointer or integer

        Returns:
            A new Bytes instance containing a copy of the data
        """
        return api.bytes_clone(ctypes.cast(address, c_bstr_p))

    def __del__(self):
        """Release the underlying memory when the object is destroyed."""
        api.bytes_release(pointer(self))

    def __len__(self) -> int:
        """Get the length of the byte array in bytes.

        Returns:
            The number of bytes in the array
        """
        return api.bytes_size(pointer(self))

    def __getitem__(self, key):
        """Get item or slice from the byte array.

        Args:
            key: Integer index or slice object

        Returns:
            For integer index: A single byte as integer
            For slice: A new Bytes instance containing the sliced data

        Raises:
            IndexError: If index is out of range or key type is invalid
        """
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
                raise IndexError('The index ({}) is out of range.'.format(key))
            return c_uint8.from_address((self.ptr().value or 0) + key).value
        else:
            raise IndexError('Invalid argument type.')

    def __copy__(self) -> 'Bytes':
        """Create a shallow copy of the byte array.

        Returns:
            A new Bytes instance with copied data
        """
        return api.bytes_clone(pointer(self))

    def __deepcopy__(self) -> 'Bytes':
        """Create a deep copy of the byte array.

        Returns:
            A new Bytes instance with copied data
        """
        return api.bytes_clone(pointer(self))

    def __str__(self) -> str:
        """Convert to string representation.

        Returns:
            String representation of the byte array
        """
        return str(self.bytes())

    def ptr(self) -> c_void_p:
        """Get pointer to the underlying byte array.

        Returns:
            Void pointer to the byte array data
        """
        return api.bytes_ptr(pointer(self))

    def copy(self) -> 'Bytes':
        """Create a copy of the byte array.

        Returns:
            A new Bytes instance with copied data
        """
        return api.bytes_clone(pointer(self))

    def bytes(self) -> bytes:
        """Convert to Python bytes object.

        Returns:
            A copy of the data as Python bytes
        """
        return string_at(self.ptr(), len(self))

    def base64_encode(self) -> 'Bstr':
        """Encode the byte array as base64.

        Returns:
            A new Bstr instance containing the base64 encoded data
        """
        return api.bytes_base64_encode(pointer(self))

    @staticmethod
    def base64_decode(
        s: Union['Bstr', 'Bytes', c_char_p, builtins.bytes, c_wchar_p, str],
    ) -> 'Bytes':
        """Decode base64 data into a byte array.

        Args:
            s: Base64 encoded data in various formats

        Returns:
            A new Bytes instance containing the decoded data
        """
        return api.bytes_base64_decode(pointer(Bstr(s)))


class Bstr(ctypes.Structure):
    """A memory-safe string wrapper for the native bstr library.

    This class provides a Python interface to the native string implementation,
    with automatic memory management and safe operations. It supports various
    string encodings (UTF-8, UTF-16, UTF-32) and conversion methods.

    Memory Management:
        - Automatically manages memory allocation and deallocation
        - Uses reference counting for safe memory handling
        - Thread-safe for basic operations

    Examples:
        >>> # Create from string
        >>> s = Bstr('Hello')
        >>> print(len(s))
        5

        >>> # Create from bytes
        >>> s = Bstr(b'Hello')
        >>> print(s.str())
        'Hello'

        >>> # Convert between string types
        >>> b = Bytes(b'Hello')
        >>> s = Bstr(b)
        >>> print(s.str())
        'Hello'
    """

    NPOS = c_size_t(-1)

    _fields_ = [
        ('reserved1', c_void_p),
        ('reserved2', c_void_p),
        ('reserved3', c_void_p),
        ('reserved4', c_void_p),
    ]

    def __init__(
        self,
        s: Optional[Union['Bstr', 'Bytes', c_char_p, bytes, c_wchar_p, str]] = None,
    ):
        """Initialize a new Bstr instance.

        Args:
            s: The source to initialize from. Can be:
                - Bstr: Clone from another Bstr instance
                - Bytes: Convert from a Bytes instance
                - c_char_p: Create from a C char pointer (UTF-8)
                - bytes: Create from Python bytes (UTF-8)
                - c_wchar_p: Create from a C wide char pointer
                - str: Create from Python string
                - None: Create an empty string

        Raises:
            TypeError: If source type is not supported
        """
        api.bstr_init(pointer(self))
        if isinstance(s, Bstr):
            api.bstr_swap(pointer(self), pointer(api.bstr_clone(pointer(s))))
        elif isinstance(s, c_bstr_p):
            api.bstr_swap(pointer(self), pointer(api.bstr_clone(s)))
        elif isinstance(s, Bytes):
            api.bstr_swap(pointer(self), pointer(api.bstr_from_bytes(pointer(s))))
        elif isinstance(s, c_bytes_p):
            api.bstr_swap(pointer(self), pointer(api.bstr_from_bytes(s)))
        elif isinstance(s, c_char_p):
            api.bstr_swap(pointer(self), pointer(api.bstr_from_utf8(s, Bstr.NPOS)))
        elif isinstance(s, bytes):
            api.bstr_swap(pointer(self), pointer(api.bstr_from_utf8(s, len(s))))
        elif isinstance(s, c_wchar_p):
            api.bstr_swap(pointer(self), pointer(api.bstr_from_wchar(s, Bstr.NPOS)))
        elif isinstance(s, str):
            api.bstr_swap(pointer(self), pointer(api.bstr_from_wchar(s, len(s))))
        elif s is None:
            pass
        else:
            raise TypeError('Invalid type: {}'.format(type(s)))

    @staticmethod
    def clone_from_address(address: Union[c_void_p, int]) -> 'Bstr':
        """Create a new Bstr instance by cloning from a memory address.

        Args:
            address: Memory address as void pointer or integer

        Returns:
            A new Bstr instance containing a copy of the data
        """
        return api.bstr_clone(ctypes.cast(address, c_bstr_p))

    def __del__(self):
        """Release the underlying memory when the object is destroyed."""
        api.bstr_release(pointer(self))

    def __len__(self) -> int:
        """Get the length of the string in bytes.

        Returns:
            The number of bytes in the string
        """
        return api.bstr_size(pointer(self))

    def __copy__(self) -> 'Bstr':
        """Create a shallow copy of the string.

        Returns:
            A new Bstr instance with copied data
        """
        return api.bstr_clone(pointer(self))

    def __deepcopy__(self) -> 'Bstr':
        """Create a deep copy of the string.

        Returns:
            A new Bstr instance with copied data
        """
        return api.bstr_clone(pointer(self))

    def __str__(self) -> str:
        """Convert to Python string.

        Returns:
            The string data decoded as UTF-8
        """
        return self.str()

    def ptr(self) -> c_void_p:
        """Get pointer to the underlying string data.

        Returns:
            Void pointer to the string data
        """
        return api.bstr_ptr(pointer(self))

    def copy(self) -> 'Bstr':
        """Create a copy of the string.

        Returns:
            A new Bstr instance with copied data
        """
        return api.bstr_clone(pointer(self))

    def bytes(self) -> bytes:
        """Get the raw bytes of the string.

        Returns:
            The underlying data as Python bytes
        """
        return string_at(self.ptr(), len(self))

    def str(self) -> str:
        """Convert to Python string.

        Returns:
            The string data decoded as UTF-8
        """
        return string_at(self.ptr(), len(self)).decode('utf-8')


class BstrApi:
    """API wrapper for the native bstr library.

    This class manages the loading of the native library and provides access to
    its functions. It ensures the library is loaded only once and handles platform-specific
    library names and search paths.

    Memory Management:
        - Manages library loading and function binding
        - Maintains a single global instance
        - Thread-safe for initialization

    Examples:
        >>> # Load the library
        >>> api = bstr_api()
        >>> api.load_library('bstr')

        >>> # Load from specific directory
        >>> api.load_library('bstr', dirs=['path/to/lib'])
    """

    def __init__(self):
        """Initialize the API wrapper."""
        self.dll_path = ''
        self.dll_name = ''
        self.dll: Optional[CDLL] = None

    def load_library(
        self, dll: Union[str, CDLL], *, dirs: Optional[Union[List[str], str]] = None
    ) -> CDLL:
        """Load the native library and initialize API functions.

        Args:
            dll: Library name (e.g. 'bstr') or CDLL instance
            dirs: Optional directory or list of directories to search for the library

        Returns:
            The loaded library instance

        Raises:
            RuntimeError: If library cannot be loaded or is already initialized
            TypeError: If arguments have invalid types
        """
        if self.dll is None:
            if isinstance(dll, str):
                dll = self._load_dll(dll, dirs)
            elif isinstance(dll, CDLL):
                self.dll = dll
            else:
                raise TypeError('Invalid argument type {}'.format(type(dll)))
        elif isinstance(dll, CDLL) and dll is not self.dll:
            raise RuntimeError(
                'Can not initialize API library {} more than once'.format(dll)
            )
        else:
            dll = self.dll

        ####################################################################

        prototype = CFUNCTYPE(None, c_bytes_p)
        self.bytes_init = prototype(('bytes_init', dll))

        prototype = CFUNCTYPE(c_void_p, c_bytes_p)
        self.bytes_ptr = prototype(('bytes_ptr', dll))

        prototype = CFUNCTYPE(c_size_t, c_bytes_p)
        self.bytes_size = prototype(('bytes_size', dll))

        prototype = CFUNCTYPE(Bytes)
        self.bytes_new = prototype(('bytes_new', dll))

        prototype = CFUNCTYPE(Bytes, c_size_t)
        self.bytes_alloc = prototype(('bytes_alloc', dll))

        prototype = CFUNCTYPE(Bytes, c_size_t)
        self.bytes_zalloc = prototype(('bytes_zalloc', dll))

        prototype = CFUNCTYPE(Bytes, c_void_p, c_size_t)
        self.bytes_from_static = prototype(('bytes_from_static', dll))

        prototype = CFUNCTYPE(Bytes, c_bstr_p)
        self.bytes_from_bstr = prototype(('bytes_from_bstr', dll))

        prototype = CFUNCTYPE(Bytes, c_void_p, c_size_t)
        self.bytes_copy_from_slice = prototype(('bytes_copy_from_slice', dll))

        prototype = CFUNCTYPE(Bytes, c_bytes_p, c_size_t, c_size_t)
        self.bytes_slice = prototype(('bytes_slice', dll))

        prototype = CFUNCTYPE(Bytes, c_bytes_p)
        self.bytes_clone = prototype(('bytes_clone', dll))

        prototype = CFUNCTYPE(c_bytes_p)
        self.bytes_release = prototype(('bytes_release', dll))

        prototype = CFUNCTYPE(None, c_bytes_p, c_bytes_p)
        self.bytes_swap = prototype(('bytes_swap', dll))

        prototype = CFUNCTYPE(Bytes, c_bstr_p)
        self.bytes_base64_decode = prototype(('bytes_base64_decode', dll))

        prototype = CFUNCTYPE(Bstr, c_bytes_p)
        self.bytes_base64_encode = prototype(('bytes_base64_encode', dll))

        ####################################################################

        prototype = CFUNCTYPE(None, c_bstr_p)
        self.bstr_init = prototype(('bstr_init', dll))

        prototype = CFUNCTYPE(c_void_p, c_bstr_p)
        self.bstr_ptr = prototype(('bstr_ptr', dll))

        prototype = CFUNCTYPE(c_size_t, c_bstr_p)
        self.bstr_size = prototype(('bstr_size', dll))

        prototype = CFUNCTYPE(Bstr)
        self.bstr_new = prototype(('bstr_new', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_char), c_size_t)
        self.bstr_from_static = prototype(('bstr_from_static', dll))

        prototype = CFUNCTYPE(Bstr, c_bytes_p)
        self.bstr_from_bytes = prototype(('bstr_from_bytes', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_char), c_size_t)
        self.bstr_from_utf8 = prototype(('bstr_from_utf8', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_uint16), c_size_t)
        self.bstr_from_utf16 = prototype(('bstr_from_utf16', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_uint32), c_size_t)
        self.bstr_from_utf32 = prototype(('bstr_from_utf32', dll))

        prototype = CFUNCTYPE(Bstr, POINTER(c_wchar), c_size_t)
        if ctypes.sizeof(c_wchar) == 2:
            self.bstr_from_wchar = prototype(('bstr_from_utf16', dll))
        elif ctypes.sizeof(c_wchar) == 4:
            self.bstr_from_wchar = prototype(('bstr_from_utf32', dll))

        prototype = CFUNCTYPE(Bstr, c_bstr_p)
        self.bstr_clone = prototype(('bstr_clone', dll))

        prototype = CFUNCTYPE(c_bstr_p)
        self.bstr_release = prototype(('bstr_release', dll))

        prototype = CFUNCTYPE(None, c_bstr_p, c_bstr_p)
        self.bstr_swap = prototype(('bstr_swap', dll))

        prototype = CFUNCTYPE(c_char_p, c_bstr_p)
        self.bstr_dup_utf8 = prototype(('bstr_dup_utf8', dll))

        prototype = CFUNCTYPE(POINTER(c_uint16), c_bstr_p)
        self.bstr_dup_utf16 = prototype(('bstr_dup_utf16', dll))

        prototype = CFUNCTYPE(POINTER(c_uint32), c_bstr_p)
        self.bstr_dup_utf32 = prototype(('bstr_dup_utf32', dll))

        prototype = CFUNCTYPE(c_wchar_p, c_bstr_p)
        if ctypes.sizeof(c_wchar) == 2:
            self.bstr_dup_wchar = prototype(('bstr_dup_utf16', dll))
        elif ctypes.sizeof(c_wchar) == 4:
            self.bstr_dup_wchar = prototype(('bstr_dup_utf32', dll))

        prototype = CFUNCTYPE(None, c_void_p)
        self.bstr_mem_free = prototype(('bstr_mem_free', dll))

        return dll

    def _load_dll(
        self, dll_name: str, dirs: Optional[Union[List[str], str]] = None
    ) -> CDLL:
        if self.dll is not None:
            return self.dll

        if os.path.splitext(dll_name)[1] == '':
            if sys.platform == 'win32':
                dll_name += '.dll'
            elif sys.platform == 'darwin':
                dll_name += '.dylib'
            else:
                dll_name += '.so'
                if not dll_name.startswith('lib'):
                    dll_name = 'lib' + dll_name

        if not dirs:
            triple_dirs: List[Optional[str]] = []
        elif isinstance(dirs, str):
            triple_dirs = [dirs]
        elif isinstance(dirs, (list, tuple)):
            triple_dirs = list(dirs)
        else:
            raise TypeError('`triple_dirs` must be str or list.')
        if 'target' not in triple_dirs:
            triple_dirs.append('target')

        def search_path():
            # '' and '.'
            yield ''
            yield os.path.realpath('.')
            # in <workspace dir>/target/
            path = os.path.realpath(os.path.dirname(__file__))
            in_workspace = False
            while path and not in_workspace:
                for i, triple_dir in enumerate(triple_dirs):
                    if not triple_dir:
                        continue
                    if os.path.isabs(triple_dir):
                        target = triple_dir
                        triple_dirs[i] = triple_dir = None
                    else:
                        target = os.path.join(path, triple_dir)
                    if os.path.isdir(target):
                        if triple_dir is not None:
                            # Found the relative path in the workspace.
                            in_workspace = True
                        dirs = [target]
                        for item in os.listdir(target):
                            dir = os.path.join(target, item)
                            # matches triple like "x86_64-pc-windows-msvc"
                            if os.path.isdir(dir) and len(item.split('-')) in (3, 4):
                                dirs.append(dir)
                        for suffix in itertools.product(
                            dirs, ['debug', 'release', 'bin', 'lib']
                        ):
                            dir = os.path.join(*suffix)
                            if os.path.isdir(dir):
                                yield dir
                parent = os.path.dirname(path)
                if path == parent:
                    break
                path = parent

        for dir in search_path():
            try:
                if self.dll is None:
                    dll_path = os.path.join(dir, dll_name)
                    self.dll = ctypes.cdll.LoadLibrary(dll_path)
                    self.dll_path = dll_path
                    self.dll_name = dll_name
                    break
            except OSError:
                pass

        if self.dll is None:
            raise RuntimeError('Can not load library {}'.format(dll_name))
        return self.dll


c_bytes_p = POINTER(Bytes)
c_bstr_p = POINTER(Bstr)

api = BstrApi()


def bstr_api() -> BstrApi:
    return api


if __name__ == '__main__':
    import copy

    bstr_api().load_library('bstr')

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
        print(
            a.base64_encode(), Bytes.base64_decode(c_char_p(a.base64_encode().bytes()))
        )
        print(
            a.base64_encode(), Bytes.base64_decode(c_wchar_p(a.base64_encode().str()))
        )
