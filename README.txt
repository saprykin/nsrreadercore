Use the following parameters for CMake to build using cross-compiling:
-DCMAKE_TOOLCHAIN_FILE=../nsrreadercore/platforms/blackberry.cmake -DCMAKE_ASM_NASM_COMPILER=/opt/local/bin/nasm -DCMAKE_BUILD_TYPE=Release

NASM requirements (for libjpeg-turbo):
   * NASM 0.98, or 2.01 or later is required for an x86 build (0.99 and 2.00 do
     not work properly with libjpeg-turbo's x86 SIMD code.)
   * NASM 2.00 or later is required for an x86-64 build.
   * NASM 2.07, or 2.11.09 or later is required for an x86-64 Mac build
     (2.11.08 does not work properly with libjpeg-turbo's x86-64 SIMD code when
     building macho64 objects.)  NASM or YASM can be obtained from MacPorts
     (http://www.macports.org/).
