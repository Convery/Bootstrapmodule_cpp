/*
    Initial author: Convery (tcn@ayria.se)
    Started: 28-12-2017
    License: MIT
    Notes:
        ELF files hook libc rather than the app.
*/

#include "../Stdinclude.hpp"

#if !defined(_WIN32)

int Callback(int (*main) (int, char**, char**), int argc, char **ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (*stack_end));
uint8_t Originaltext[20]{};
size_t Entrypoint{};

// ELF file properties.
size_t GetELFEntrypoint()
{
    // dlopen(NULL) gets the host application.
    void *Modulehandle = dlopen(NULL, RTLD_LAZY);
    if(!Modulehandle) return 0;

    return size_t(dlsym(Modulehandle, "__libc_start_main"));
}

// Bootstrapping.
void InstallELFCallback()
{
    Printfunction();

    Entrypoint = GetELFEntrypoint();
    if(!Entrypoint) return;

    auto Protection = Memprotect::Unprotectrange((void *)Entrypoint, 20);
    {
        // Take a backup of the text segment.
        std::memcpy(Originaltext, (void *)Entrypoint, 20);

        // Per architecture patching.
        #if defined (ENVIRONMENT64)
            *(uint8_t *)(Entrypoint + 0) = 0x48;                   // mov
            *(uint8_t *)(Entrypoint + 1) = 0xB8;                   // rax
            *(uint64_t *)(Entrypoint + 2) = (uint64_t)Callback;    // Address
            *(uint8_t *)(Entrypoint + 10) = 0xFF;                  // jmp reg
            *(uint8_t *)(Entrypoint + 11) = 0xE0;                  // rax
        #else
            *(uint8_t *)(Entrypoint + 0) = 0xE9;                   // jmp
            *(uint32_t *)(Entrypoint + 1) = ((uint32_t)Callback - (Entrypoint + 5));
        #endif
    }
    Memprotect::Protectrange((void *)Entrypoint, 20, Protection);
}
void Restoreentrypoint()
{
    if(!Entrypoint) return;

    auto Protection = Memprotect::Unprotectrange((void *)Entrypoint, 20);
    {
        // Restore the text segment.
        std::memcpy((void *)Entrypoint, Originaltext, 20);
    }
    Memprotect::Protectrange((void *)Entrypoint, 20, Protection);

    Printfunction();
}

// The callback from the games entrypoint.
int Callback(int (*main) (int, char**, char**), int argc, char **ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (*stack_end))
{
    Printfunction();

    // Remove our hackery.
    Restoreentrypoint();

    // Open our module to increase refcount.
    auto Module = getenv("LD_PRELOAD");
    dlopen(Module, RTLD_LAZY);

    // Do what we came here for.
    //Loadallplugins();

    // Call the original libc.
    auto Libc = reinterpret_cast<int (*) (int (*) (int, char**, char**), int, char **, void (*) (void), void (*) (void), void (*) (void), void (*))>(Entrypoint);
    return Libc(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}

#endif