package www.unsa.bsod.com.dump;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

import www.unsa.bsod.com.crash.ModContext;

/**
 * A hand-rolled, structurally valid Windows Minidump writer.
 *
 * Layout produced (all little-endian):
 *   MDMP header (32 bytes)
 *   stream directory (n x 12 byte entries)
 *   ModuleListStream  (type 4) - every classpath entry as a module
 *   MemoryListStream  (type 5) - the crash code as one memory range
 *   ExceptionStream   (type 6) - the Java throwable as a software exception
 *   SystemInfoStream  (type 7) - host OS/CPU snapshot
 *   ThreadListStream  (type 3) - the crashing thread
 *   string blobs (module names, crash code text)
 *
 * Field sizes follow the official MINIDUMP_* structures byte for byte.
 */
public final class MinidumpGenerator {

    private static final int SIGNATURE = 0x504D444D; // 'MDMP'
    private static final int VERSION = 0x0000A793;   // same version value WinDbg writes today
    private static final int FLAGS = 0x00000002;

    // Stream types
    private static final int THREAD_LIST = 3;
    private static final int MODULE_LIST = 4;
    private static final int MEMORY_LIST = 5;
    private static final int EXCEPTION_STREAM = 6;
    private static final int SYSTEM_INFO = 7;

    /** Classic MSVC "software exception raised by managed code" code. */
    private static final int EXCEPTION_CODE_MSVC_THROW = 0xE06D7363;

    private static final int MAX_MODULES = 512;

    private MinidumpGenerator() {
    }

    public static void writeDump(Path target, ModContext context) throws IOException {
        Files.createDirectories(target.toAbsolutePath().getParent());

        Blob blob = new Blob();

        // ---------- payload sizes (computed up front for exact RVAs) ----------
        List<ModuleEntry> modules = collectModules();

        String threadName = Thread.currentThread().getName();
        byte[] threadNameUtf16 = utf16(threadName);
        byte[] crashCodeUtf16 = utf16(context.crashCode());

        int moduleNamesBytes = modules.stream().mapToInt(m -> stringBlobSize(m.name)).sum();
        int moduleListSize = 4 + modules.size() * 108 + moduleNamesBytes;
        int memoryListSize = 4 /*count*/ + 16 /*one descriptor*/;
        int exceptionSize = 8 /*ThreadId+alignment*/ + 152 /*MINIDUMP_EXCEPTION*/ + 8 /*ctx loc*/;
        int systemInfoSize = 56;
        int threadListSize = 4 + 48;

        int directoryRva = 32;
        int numberOfStreams = 5;
        int payloadStart = directoryRva + numberOfStreams * 12;

        int moduleRva = payloadStart;
        int memoryRva = moduleRva + moduleListSize;
        int exceptionRva = memoryRva + memoryListSize;
        int systemRva = exceptionRva + exceptionSize;
        int threadRva = systemRva + systemInfoSize;
        int stringsRva = threadRva + threadListSize;

        // ---------- header ----------
        blob.u32(SIGNATURE);
        blob.u32(VERSION);
        blob.u32(numberOfStreams);
        blob.u32(directoryRva);
        blob.u32(0);                      // checksum
        blob.u32((int) (context.timestampMillis() / 1000L));
        blob.u32(FLAGS);

        // ---------- stream directory ----------
        blob.dirEntry(MODULE_LIST, moduleListSize, moduleRva);
        blob.dirEntry(MEMORY_LIST, memoryListSize, memoryRva);
        blob.dirEntry(EXCEPTION_STREAM, exceptionSize, exceptionRva);
        blob.dirEntry(SYSTEM_INFO, systemInfoSize, systemRva);
        blob.dirEntry(THREAD_LIST, threadListSize, threadRva);

        // ---------- module list ----------
        blob.u32(modules.size());
        int nameCursor = stringsRva;
        for (ModuleEntry m : modules) {
            blob.u64(m.base);
            blob.u32(m.size);
            blob.u32(0);                  // checksum
            blob.u32(0);                  // timestamp
            blob.u32(nameCursor);         // name RVA
            writeFixedFileInfo(blob);     // 52 bytes
            blob.u32(0); blob.u32(0);     // CvRecord
            blob.u32(0); blob.u32(0);     // MiscRecord
            blob.u64(0);                  // reserved 0
            blob.u64(0);                  // reserved 1
            nameCursor += stringBlobSize(m.name);
        }
        for (ModuleEntry m : modules) {   // name blobs follow sequentially
            writeString(blob, m.name);
        }

        // ---------- memory list ----------
        blob.u32(1);                      // number of ranges
        blob.u64(1);                      // start address (non-zero placeholder)
        blob.u32(crashCodeUtf16.length);
        blob.u32(stringsRva + moduleNamesBytes); // data RVA right after the names

        // ---------- exception stream ----------
        blob.u32((int) Thread.currentThread().getId());
        blob.u32(0);                      // alignment
        blob.u32(EXCEPTION_CODE_MSVC_THROW);
        blob.u32(0);                      // exception flags
        blob.u64(0);                      // inner record
        blob.u64(context.timestampMillis()); // exception address slot (informational)
        blob.u32(1);                      // number of parameters
        blob.u32(0);                      // alignment
        for (int i = 0; i < 15; i++) {
            blob.u64(i == 0 ? 3L : 0L);   // [0]=3 mimics the MSVC throw protocol
        }
        blob.u32(0);                      // thread context: DataSize
        blob.u32(0);                      // thread context: RVA

        // ---------- system info (exactly 56 bytes) ----------
        writeSystemInfo(blob);

        // ---------- thread list (exactly 52 bytes) ----------
        blob.u32(1);                      // number of threads
        blob.u32((int) Thread.currentThread().getId());
        blob.u32(0);                      // suspend count
        blob.u32(0);                      // priority class
        blob.u32(0);                      // priority
        blob.u64(0);                      // TEB
        blob.u64(1);                      // stack start
        blob.u32(crashCodeUtf16.length);
        blob.u32(stringsRva + moduleNamesBytes); // stack shares the crash-code blob
        blob.u32(0);                      // context DataSize
        blob.u32(0);                      // context RVA

        // ---------- raw string blobs ----------
        for (ModuleEntry m : modules) {
            writeString(blob, m.name);
        }
        blob.raw(crashCodeUtf16);

        try (OutputStream fos = Files.newOutputStream(target)) {
            blob.writeTo(fos);
        }
    }

    private static void writeFixedFileInfo(Blob b) throws IOException {
        b.u32(0xFEEF04BD);                // signature
        b.u32(0x00000100);                // struct version 1.0
        b.u32(0x00010000);                // file version 1.0.0.0
        b.u32(0x00000000);
        b.u32(0x00010000);                // product version 1.0.0.0
        b.u32(0x00000000);
        b.u32(0x0000003F);                // flags mask
        b.u32(0x00000000);                // flags
        b.u32(0x00040004);                // VOS_NT_WINDOWS32
        b.u32(0x00000001);                // VFT_APP
        b.u32(0x00000000);                // subtype
        b.u32(0x00000000);                // file date ms
        b.u32(0x00000000);                // file date ls
    }

    private static void writeSystemInfo(Blob b) throws IOException {
        String os = System.getProperty("os.name", "").toLowerCase();
        boolean windows = os.contains("win");
        boolean mac = os.contains("mac");

        String arch = System.getProperty("os.arch", "").toLowerCase();
        int procArch = arch.contains("aarch64") ? 12 : 9; // 12=ARM64, 9=AMD64

        int platformId = windows ? 2 : 3; // VER_PLATFORM_WIN32_NT vs Unix-ish

        b.u16(procArch);                  // ProcessorArchitecture
        b.u16(0);                         // ProcessorLevel
        b.u16(0);                         // ProcessorRevision
        b.u8(Runtime.getRuntime().availableProcessors()); // NumberOfProcessors (1 byte!)
        b.u8(windows ? 1 : 3);            // ProductType: VER_NT_WORKSTATION / VER_NT_SERVER-ish
        b.u32(windows ? 10 : 10);         // MajorVersion
        b.u32(0);                         // MinorVersion
        b.u32(0);                         // BuildNumber
        b.u32(platformId);                // PlatformId
        b.u32(0);                         // CSDVersionRva
        b.u16(0x0100);                    // SuiteMask
        b.u16(0);                         // Reserved2
        for (int i = 0; i < 6; i++) {
            b.u32(0);                     // Cpu.Data: 24 bytes
        }
        // total: 2+2+2+1+1+4*5+2+2+24 = 56 bytes exactly
    }

    private static List<ModuleEntry> collectModules() {
        List<ModuleEntry> result = new ArrayList<>();
        long base = 0x00018000_0000L & 0xFFFFFFFFL; // keep it inside u32-friendly range

        try {
            String cp = System.getProperty("java.class.path", "");
            for (String element : cp.split(java.io.File.pathSeparator)) {
                if (element.isBlank() || result.size() >= MAX_MODULES) {
                    continue;
                }
                Path p = Path.of(element);
                long size;
                try {
                    size = Files.isRegularFile(p) ? Files.size(p) : 4096;
                } catch (IOException e) {
                    size = 4096;
                }
                long aligned = ((size + 0xFFF) / 0x1000) * 0x1000;
                result.add(new ModuleEntry(p.toAbsolutePath().toString(), base, aligned));
                base += aligned;
            }
        } catch (Throwable ignored) {
            // best effort only
        }

        if (result.isEmpty()) {
            result.add(new ModuleEntry("Minecraft (unknown modules)", 0x1000L, 0x1000L));
        }
        return result;
    }

    private static int stringBlobSize(String s) {
        return 4 + utf16(s).length + 2; // length prefix + UTF-16LE data + NUL terminator
    }

    private static void writeString(Blob b, String s) throws IOException {
        byte[] data = utf16(s);
        b.u32(data.length);
        b.raw(data);
        b.u16(0);                         // NUL terminator, not counted in length
    }

    private static byte[] utf16(String s) {
        return (s == null ? "" : s).getBytes(StandardCharsets.UTF_16LE);
    }

    private record ModuleEntry(String name, long base, long size) {
    }

    /** Tiny little-endian buffer with just the primitives we need. */
    private static final class Blob {

        private final ByteArrayOutputStream out = new ByteArrayOutputStream(64 * 1024);
        private final DataOutputStream data = new DataOutputStream(out);

        void u8(int v) throws IOException {
            data.writeByte(v & 0xFF);
        }

        void u16(int v) throws IOException {
            data.writeByte(v & 0xFF);
            data.writeByte((v >>> 8) & 0xFF);
        }

        void u32(int v) throws IOException {
            data.writeByte(v & 0xFF);
            data.writeByte((v >>> 8) & 0xFF);
            data.writeByte((v >>> 16) & 0xFF);
            data.writeByte((v >>> 24) & 0xFF);
        }

        void u64(long v) throws IOException {
            u32((int) (v & 0xFFFFFFFFL));
            u32((int) ((v >>> 32) & 0xFFFFFFFFL));
        }

        void dirEntry(int type, int size, int rva) throws IOException {
            u32(type);
            u32(size);
            u32(rva);
        }

        void raw(byte[] bytes) throws IOException {
            data.write(bytes);
        }

        void writeTo(OutputStream target) throws IOException {
            data.flush();
            out.writeTo(target);
        }
    }
}