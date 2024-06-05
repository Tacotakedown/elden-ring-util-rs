use std::ffi::c_void;

pub type HWND = *mut std::ffi::c_void;
pub type DWORD_PTR = usize;

pub const PAGE_EXECUTE_READWRITE: u32 = 0x40;
pub const PROCESS_ALL_ACCESS: u32 = 0x1F0FFF;

#[repr(C)]
pub struct MEMORY_BASIC_INFORMATION {
    pub BaseAddress: *mut std::ffi::c_void,
    pub AllocationBase: *mut std::ffi::c_void,
    pub AllocationProtect: u32,
    pub RegionSize: usize,
    pub State: u32,
    pub Protect: u32,
    pub Type: u32,
}

extern "C" {
    pub fn GetModuleHandleA(lpModuleName: *const i8) -> *mut std::ffi::c_void;
    pub fn GetModuleHandleExA(
        dwFlags: u32,
        lpModuleName: *const i8,
        phModule: *mut *mut std::ffi::c_void,
    ) -> i32;
    pub fn GetModuleFileNameA(
        hModule: *mut std::ffi::c_void,
        lpFilename: *mut i8,
        nSize: u32,
    ) -> u32;
    pub fn MessageBoxA(hWnd: HWND, lpText: *const i8, lpCaption: *const i8, uType: u32) -> i32;
    pub fn OpenProcess(
        dwDesiredAccess: u32,
        bInheritHandle: i32,
        dwProcessId: u32,
    ) -> *mut std::ffi::c_void;
    pub fn EnumProcessModules(
        hProcess: *mut std::ffi::c_void,
        lphModule: *mut *mut std::ffi::c_void,
        cb: u32,
        lpcbNeeded: *mut u32,
    ) -> i32;
    pub fn EnumWindows(lpEnumFunc: *const std::ffi::c_void, lParam: *const std::ffi::c_void)
        -> i32;
    pub fn FindWindowExA(
        hwndParent: HWND,
        hwndChildAfter: HWND,
        lpszClass: *const i8,
        lpszWindow: *const i8,
    ) -> HWND;
    pub fn GetWindowThreadProcessId(hwnd: HWND, lpdwProcessId: *mut u32) -> u32;
    pub fn GetWindowTextA(hwnd: HWND, lpString: *mut i8, nMaxCount: i32) -> i32;
    pub fn VirtualQuery(
        lpAddress: *const std::ffi::c_void,
        lpBuffer: *mut MEMORY_BASIC_INFORMATION,
        dwLength: usize,
    ) -> usize;
    pub fn VirtualProtect(
        lpAddress: *mut std::ffi::c_void,
        dwSize: usize,
        flNewProtect: u32,
        lpflOldProtect: *mut u32,
    ) -> i32;
    pub fn GetCurrentProcessId() -> u32;
    pub fn GetLastError() -> u32;
    pub fn LocalAlloc(uFlags: u32, uBytes: usize) -> *mut std::ffi::c_void;
    pub fn LocalFree(hMem: *mut std::ffi::c_void);
    pub fn CloseHandle(hObject: *mut std::ffi::c_void) -> i32;
    pub fn Sleep(dwMilliseconds: u32);
    pub fn memcpy(
        dest: *mut std::ffi::c_void,
        src: *const std::ffi::c_void,
        count: usize,
    ) -> *mut std::ffi::c_void;
    pub fn memset(dest: *mut std::ffi::c_void, val: i32, count: usize) -> *mut std::ffi::c_void;
    pub fn GetCurrentProcess() -> *mut std::ffi::c_void;
    pub fn GetProcessId(hProcess: *mut std::ffi::c_void) -> u32;
    pub fn EnumProcessModules(
        hProcess: *mut std::ffi::c_void,
        lphModule: *mut *mut std::ffi::c_void,
        cb: u32,
        lpcbNeeded: *mut u32,
    ) -> i32;
}

pub fn get_module_handle(lp_module_name: &str) -> Option<*mut c_void> {
    let module_name = CString::new(lp_module_name).expect("CString::new failed");
    let result = unsafe { GetModuleHandleA(module_name.as_ptr()) };
    if result.is_null() {
        None
    } else {
        Some(result)
    }
}

pub fn get_module_file_name(h_module: *mut c_void) -> Option<String> {
    let mut filename_buf = vec![0i8; 260];
    let result = unsafe {
        GetModuleFileNameA(
            h_module,
            filename_buf.as_mut_ptr(),
            filename_buf.len() as u32,
        )
    };
    if result == 0 {
        None
    } else {
        let filename = CString::from_vec_unchecked(filename_buf)
            .into_string()
            .expect("Failed to convert filename to String");
        Some(filename)
    }
}

pub fn message_box(hWnd: HWND, lp_text: &str, lp_caption: &str, u_type: u32) -> c_int {
    let text = CString::new(lp_text).expect("CString::new failed");
    let caption = CString::new(lp_caption).expect("CString::new failed");
    unsafe { MessageBoxA(hWnd, text.as_ptr(), caption.as_ptr(), u_type) }
}

pub fn enum_windows(
    lp_enum_func: Option<extern "C" fn(HWND, *const c_void) -> c_int>,
    lParam: *const c_void,
) -> c_int {
    let result = unsafe {
        EnumWindows(
            lp_enum_func.map_or(null_mut(), |f| f as *const c_void),
            lParam,
        )
    };
    result
}

pub fn find_window_ex(
    hwnd_parent: HWND,
    hwnd_child_after: HWND,
    lpsz_class: &str,
    lpsz_window: &str,
) -> HWND {
    let class_name = CString::new(lpsz_class).expect("CString::new failed");
    let window_name = CString::new(lpsz_window).expect("CString::new failed");
    let result = unsafe {
        FindWindowExA(
            hwnd_parent,
            hwnd_child_after,
            class_name.as_ptr(),
            window_name.as_ptr(),
        )
    };
    result
}

pub fn get_window_thread_process_id(hwnd: HWND) -> Option<u32> {
    let mut process_id: u32 = 0;
    let result = unsafe { GetWindowThreadProcessId(hwnd, &mut process_id as *mut u32) };
    if result == 0 {
        None
    } else {
        Some(process_id)
    }
}

pub fn get_window_text(hwnd: HWND) -> Option<String> {
    let mut text_buf = vec![0i8; 260];
    let result = unsafe { GetWindowTextA(hwnd, text_buf.as_mut_ptr(), text_buf.len() as i32) };
    if result == 0 {
        None
    } else {
        let text = CString::from_vec_unchecked(text_buf)
            .into_string()
            .expect("Failed to convert text to String");
        Some(text)
    }
}

pub fn virtual_query(
    lp_address: *const c_void,
    lp_buffer: *mut MEMORY_BASIC_INFORMATION,
    dw_length: usize,
) -> usize {
    unsafe { VirtualQuery(lp_address, lp_buffer, dw_length) }
}

pub fn virtual_protect(
    lp_address: *mut c_void,
    dw_size: usize,
    fl_new_protect: u32,
    lpfl_old_protect: *mut u32,
) -> c_int {
    unsafe { VirtualProtect(lp_address, dw_size, fl_new_protect, lpfl_old_protect) }
}

pub fn get_current_process_id() -> u32 {
    unsafe { GetCurrentProcessId() }
}

pub fn get_last_error() -> u32 {
    unsafe { GetLastError() }
}

pub fn local_alloc(u_flags: u32, u_bytes: usize) -> *mut c_void {
    unsafe { LocalAlloc(u_flags, u_bytes) }
}

pub fn local_free(h_mem: *mut c_void) {
    unsafe { LocalFree(h_mem) }
}

pub fn close_handle(h_object: *mut c_void) -> c_int {
    unsafe { CloseHandle(h_object) }
}

pub fn sleep(dw_milliseconds: u32) {
    unsafe { Sleep(dw_milliseconds) }
}

pub fn memcpy(dest: *mut c_void, src: *const c_void, count: usize) {
    unsafe { memcpy(dest, src, count) };
}

pub fn memset(dest: *mut c_void, val: c_int, count: usize) {
    unsafe { memset(dest, val, count) };
}

pub fn get_current_process() -> *mut c_void {
    unsafe { GetCurrentProcess() }
}

pub fn get_process_id(h_process: *mut c_void) -> u32 {
    unsafe { GetProcessId(h_process) }
}

pub fn enum_process_modules(h_process: *mut c_void) -> Option<Vec<*mut c_void>> {
    let mut module_array: Vec<*mut c_void> = vec![null_mut(); 1024];
    let mut bytes_required: u32 = 0;
    let result = unsafe {
        EnumProcessModules(
            h_process,
            module_array.as_mut_ptr(),
            (module_array.len() * std::mem::size_of::<*mut c_void>()) as u32,
            &mut bytes_required as *mut u32,
        )
    };
    if result == 0 {
        None
    } else {
        module_array.resize(
            bytes_required as usize / std::mem::size_of::<*mut c_void>(),
            null_mut(),
        );
        Some(module_array)
    }
}
