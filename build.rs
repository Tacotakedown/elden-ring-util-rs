extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    let header = "lib-cpp/include/eldenRingUtils.h";

    println!("cargo:rustc-link-lib=static=eldenRingUtils");
    println!("cargo:rustc-link-search=native=lib-cpp/bin");

    env::set_var("BINDGEN_EXTRA_CLANG_ARGS_x86_64-pc-windows-msvc", "-I\"C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Tools/MSVC/14.29.30133/include\"");

    let bindings = bindgen::Builder::default()
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .clang_args(&["-x", "c++", "-std=c++17"])
        .header(header)
        .clang_arg("-v")
        .clang_arg("-Wno-static-in-inline")
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
