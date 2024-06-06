use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rustc-link-lib=static=eldenRingUtils");

    println!("cargo:rustc-link-search=native=lib-cpp/bin");
}
