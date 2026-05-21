/*
 * INITGUID must be defined before dxgi.h so that IID_IDXGIFactory
 * and IID_IDXGIAdapter are emitted as actual symbols, not just
 * extern declarations. Without this you get an undefined reference
 * at link time even with -ldxgi.
 *
 * Define it only once (here) — never in a header.
 */
#define INITGUID

#include "common.h"
#include "util.h"
#include "info_gpu.h"

#include <dxgi1_4.h>

void print_gpu(void) {
    IDXGIFactory4 *pFactory = NULL;

    /* C COM: pass IID by pointer, call through lpVtbl */
    if (FAILED(CreateDXGIFactory1(&IID_IDXGIFactory4, (void **)&pFactory)))
        return;

    IDXGIAdapter1 *pAdapter = NULL;
    UINT i = 0;
    print_separator();
    printf("  " LABEL("GPU") "\n");

    while (pFactory->lpVtbl->EnumAdapters1(pFactory, i, &pAdapter)
           != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        pAdapter->lpVtbl->GetDesc(pAdapter, &desc);

        /* DedicatedVideoMemory is 0 for integrated GPUs; show shared mem then */
        SIZE_T vram = desc.DedicatedVideoMemory
                    ? desc.DedicatedVideoMemory
                    : desc.SharedSystemMemory;
        const char *vram_label = desc.DedicatedVideoMemory
                               ? "dedicated" : "shared";


        printf("      " C_BLUE C_BOLD "[%u]" C_RESET "  %ls\n", i, desc.Description);
        printf("        %llu MB " C_RESET "(%s)\n", 
                (unsigned long long)vram / (1024 * 1024), vram_label);

        pAdapter->lpVtbl->Release(pAdapter);
        ++i;
    }

    pFactory->lpVtbl->Release(pFactory);
}
