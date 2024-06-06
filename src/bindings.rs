use libc::{c_char, c_int, c_uchar, c_uint, c_void, size_t};
use std::ffi::{CStr, CString};
use winapi::shared::basetsd::DWORD_PTR;
use winapi::shared::minwindef::LPARAM;
use winapi::shared::minwindef::{BOOL, DWORD, HINSTANCE};

#[link(name = "eldenRingUtils")]
extern "C" {
    fn CreateTimer(intervalMs: c_uint) -> *mut c_void;
    fn CheckTimer(timer: *mut c_void) -> c_int;
    //  fn ResetTimer(timer: *mut c_void);
    //  fn DestroyTimer(timer: *mut c_void);

    fn _GetModuleName(mainProcessModule: BOOL) -> *const c_char;
    fn GetCurrentProcessName() -> *const c_char;
    fn GetCurrentModName() -> *const c_char;
    fn GetModFolderPath() -> *const c_char;

    fn FindDLL(base_folder: *const c_char, dll_name: *const c_char) -> *const c_char;
    fn replace(str: *mut c_char, from: *const c_char, to: *const c_char) -> c_int;
    fn ShowErrorPopup(error: *const c_char);

    fn RelativeToAbsoluteAddress(relativeAddressLocation: DWORD_PTR) -> DWORD_PTR;
    fn GetProcessBaseAddress(processId: DWORD) -> DWORD_PTR;
    fn ToggleMemoryProtection(protectionEnabled: c_int, address: DWORD_PTR, size: size_t);
    fn MemCopy(destination: DWORD_PTR, source: DWORD_PTR, numBytes: size_t);
    fn MemSet(address: DWORD_PTR, byte: c_uchar, numBytes: size_t);

    fn TokenifyAobString(aob: *const c_char) -> *const *const c_char;
    fn IsAobValid(aobTokens: *const *const c_char) -> c_int;
    fn VerifyAob(aob: *const c_char) -> c_int;
    fn VerifyAobs(aobs: *const *const c_char) -> c_int;

    fn NumberToHexString(number: DWORD_PTR) -> *const c_char;
    fn AobScan(aob: *const c_char) -> DWORD_PTR;

    fn StringAobToRawAob(aob: *const c_char) -> *mut c_uchar;
    fn RawAobToStringAob(rawAob: *mut c_uchar) -> *const c_char;

    fn CheckIfAobsMatch(aob1: *const c_char, aob2: *const c_char) -> c_int;
    fn ReplaceExpectedBytesAtAddress(
        address: DWORD_PTR,
        expectedBytes: *const c_char,
        newBytes: *const c_char,
    ) -> c_int;

    fn GetWindowHandleByName(windowName: *const c_char);
    fn EnumWindowHandles(hwnd: HINSTANCE, lParam: LPARAM) -> BOOL;
    fn GetWindowHandleByEnumeration();
    fn GetWindowHandle() -> c_int;
    fn AttemptToGetWindowHandle();

    //  fn AreKeysPressed(keys: *const c_ushort, keysCount: c_int, trueWhileHolding: c_int, checkController: c_int) -> c_int;
    fn Hook(address: DWORD_PTR, destination: DWORD_PTR, extraClearance: size_t);
}

pub struct Timer {
    ptr: *mut c_void,
}

impl Timer {
    pub fn new(interval_ms: u32) -> Self {
        unsafe {
            let ptr = CreateTimer(interval_ms);
            Timer { ptr }
        }
    }

    pub fn check(&self) -> bool {
        unsafe { CheckTimer(self.ptr) != 0 }
    }
}

impl Drop for Timer {
    fn drop(&mut self) {
        //TODO: Reset() or Destroy(), needs to be implemented in cpp then c then we can do it here
    }
}

fn c_str_to_string(c_str: *const c_char) -> String {
    unsafe {
        if c_str.is_null() {
            String::new()
        } else {
            CStr::from_ptr(c_str).to_string_lossy().into_owned()
        }
    }
}

pub fn get_module_name(main_process_module: bool) -> String {
    unsafe { c_str_to_string(_GetModuleName(main_process_module as BOOL)) }
}

pub fn get_current_process_name() -> String {
    unsafe { c_str_to_string(GetCurrentProcessName()) }
}

pub fn get_current_mod_name() -> String {
    unsafe { c_str_to_string(GetCurrentModName()) }
}

pub fn get_mod_folder_path() -> String {
    unsafe { c_str_to_string(GetModFolderPath()) }
}

pub fn find_dll(base_folder: &str, dll_name: &str) -> String {
    let base_folder = CString::new(base_folder).unwrap();
    let dll_name = CString::new(dll_name).unwrap();
    unsafe { c_str_to_string(FindDLL(base_folder.as_ptr(), dll_name.as_ptr())) }
}

pub fn replace_in_string(str: &mut String, from: &str, to: &str) -> bool {
    let from = CString::new(from).unwrap();
    let to = CString::new(to).unwrap();
    let mut c_str = CString::new(str.as_str()).unwrap();
    unsafe {
        let result = replace(c_str.as_ptr() as *mut c_char, from.as_ptr(), to.as_ptr());
        if result != 0 {
            str.clear();
            str.push_str(c_str.to_str().unwrap());
            true
        } else {
            false
        }
    }
}

pub fn show_error_popup(error: &str) {
    let error = CString::new(error).unwrap();
    unsafe {
        ShowErrorPopup(error.as_ptr());
    }
}

pub fn relative_to_absolute_address(relative_address_location: DWORD_PTR) -> DWORD_PTR {
    unsafe { RelativeToAbsoluteAddress(relative_address_location) }
}

pub fn get_process_base_address(process_id: DWORD) -> DWORD_PTR {
    unsafe { GetProcessBaseAddress(process_id) }
}

pub fn toggle_memory_protection(protection_enabled: bool, address: DWORD_PTR, size: usize) {
    unsafe {
        ToggleMemoryProtection(protection_enabled as c_int, address, size);
    }
}

pub fn mem_copy(destination: DWORD_PTR, source: DWORD_PTR, num_bytes: usize) {
    unsafe {
        MemCopy(destination, source, num_bytes);
    }
}

pub fn mem_set(address: DWORD_PTR, byte: u8, num_bytes: usize) {
    unsafe {
        MemSet(address, byte, num_bytes);
    }
}

pub fn tokenify_aob_string(aob: &str) -> Vec<String> {
    let aob = CString::new(aob).unwrap();
    unsafe {
        let tokenified_aob = TokenifyAobString(aob.as_ptr());
        if tokenified_aob.is_null() {
            vec![]
        } else {
            let mut tokens = vec![];
            let mut current = tokenified_aob;
            while !(*current).is_null() {
                tokens.push(CStr::from_ptr(*current).to_string_lossy().into_owned());
                current = current.add(1);
            }
            tokens
        }
    }
}

pub fn is_aob_valid(aob_tokens: &[String]) -> bool {
    let c_aob_tokens: Vec<CString> = aob_tokens
        .iter()
        .map(|s| CString::new(s.as_str()).unwrap())
        .collect();
    let c_aob_tokens_ptrs: Vec<*const c_char> = c_aob_tokens.iter().map(|s| s.as_ptr()).collect();
    unsafe { IsAobValid(c_aob_tokens_ptrs.as_ptr()) != 0 }
}

pub fn verify_aob(aob: &str) -> bool {
    let aob = CString::new(aob).unwrap();
    unsafe { VerifyAob(aob.as_ptr()) != 0 }
}

pub fn verify_aobs(aobs: &[String]) -> bool {
    let c_aobs: Vec<CString> = aobs
        .iter()
        .map(|s| CString::new(s.as_str()).unwrap())
        .collect();
    let c_aobs_ptrs: Vec<*const c_char> = c_aobs.iter().map(|s| s.as_ptr()).collect();
    unsafe { VerifyAobs(c_aobs_ptrs.as_ptr()) != 0 }
}

pub fn number_to_hex_string(number: DWORD_PTR) -> String {
    unsafe { c_str_to_string(NumberToHexString(number)) }
}

pub fn aob_scan(aob: &str) -> DWORD_PTR {
    let aob = CString::new(aob).unwrap();
    unsafe { AobScan(aob.as_ptr()) }
}

pub fn string_aob_to_raw_aob(aob: &str) -> Vec<u8> {
    let aob = CString::new(aob).unwrap();
    unsafe {
        let raw_aob_ptr = StringAobToRawAob(aob.as_ptr());
        if raw_aob_ptr.is_null() {
            vec![]
        } else {
            let mut raw_aob = vec![];
            let mut current = raw_aob_ptr;
            while *current != 0 {
                raw_aob.push(*current);
                current = current.add(1);
            }
            raw_aob
        }
    }
}

pub fn raw_aob_to_string_aob(raw_aob: &[u8]) -> String {
    unsafe {
        let raw_aob_ptr = raw_aob.as_ptr() as *mut c_uchar;
        c_str_to_string(RawAobToStringAob(raw_aob_ptr))
    }
}

pub fn check_if_aobs_match(aob1: &str, aob2: &str) -> bool {
    let aob1 = CString::new(aob1).unwrap();
    let aob2 = CString::new(aob2).unwrap();
    unsafe { CheckIfAobsMatch(aob1.as_ptr(), aob2.as_ptr()) != 0 }
}

pub fn replace_expected_bytes_at_address(
    address: DWORD_PTR,
    expected_bytes: &str,
    new_bytes: &str,
) -> bool {
    let expected_bytes = CString::new(expected_bytes).unwrap();
    let new_bytes = CString::new(new_bytes).unwrap();
    unsafe {
        ReplaceExpectedBytesAtAddress(address, expected_bytes.as_ptr(), new_bytes.as_ptr()) != 0
    }
}

pub fn get_window_handle_by_name(window_name: &str) {
    let window_name = CString::new(window_name).unwrap();
    unsafe {
        GetWindowHandleByName(window_name.as_ptr());
    }
}

pub fn enum_window_handles(hwnd: HINSTANCE, l_param: LPARAM) -> bool {
    unsafe { EnumWindowHandles(hwnd, l_param) != 0 }
}

pub fn get_window_handle_by_enumeration() {
    unsafe {
        GetWindowHandleByEnumeration();
    }
}

pub fn get_window_handle() -> i32 {
    unsafe { GetWindowHandle() }
}

pub fn attempt_to_get_window_handle() {
    unsafe {
        AttemptToGetWindowHandle();
    }
}

pub fn hook(address: DWORD_PTR, destination: DWORD_PTR, extra_clearance: usize) {
    unsafe {
        Hook(address, destination, extra_clearance);
    }
}
