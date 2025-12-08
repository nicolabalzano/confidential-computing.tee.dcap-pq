Intel(R) Software Guard Extensions Data Center Attestation Primitives (Intel(R) SGX DCAP) Rust TDQuote Generation SampleCode
================================================

## Linux
Supported operating systems:
* Ubuntu* 22.04 LTS Server 64bits
* Ubuntu* 24.04 LTS Server 64bits
* Red Hat Enterprise Linux Server release 9.4 64bits
* CentOS Stream 9 64bits
* SUSE Linux Enterprise Server 15.6 64bits
* Anolis OS 8.10 64bits
* Azure Linux 3.0 64bits
* Debian 10 64bits
* Debian 12 64bits

Requirements:
* make
* gcc
* g++
* bash shell
* clang
* Rust and Cargo

Prerequisite:
* Intel(R) SGX SDK

*Note that you need to install **libtdx-attest-dev** for this package.*

Build and run *RustTDQuoteGenerationSample* to generate a TD quote

```
$ cargo build
$ ./target/debug/app
```

You can also combine building and running with a single Cargo command:
```
$ cargo run
```
