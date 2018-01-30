/*
 * Host Page Table (HPT)
 *
 * Copyright (C) 2009-2011 Udo Steinberg <udo@hypervisor.org>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * Copyright (C) 2012 Udo Steinberg, Intel Corporation.
 * Copyright (C) 2015 Alexander Boettcher, Genode Labs GmbH
 *
 * This file is part of the NOVA microhypervisor.
 *
 * NOVA is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NOVA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License version 2 for more details.
 */

#pragma once

#include "arch.hpp"
#include "pte.hpp"

class Hpt : public Pte<Hpt, mword, PTE_LEV, PTE_BPL, false> {
private:

    ALWAYS_INLINE
    static inline void flush() {
        mword cr3;
        asm volatile ("mov %%cr3, %0; mov %0, %%cr3" : "=&r" (cr3));
    }

public:

    ALWAYS_INLINE
    static inline void flush(mword addr) {
        asm volatile ("invlpg %0" : : "m" (*reinterpret_cast<mword *> (addr)));
    }

    static mword ord;

    enum {
        HPT_P = 1UL << 0,
        HPT_W = 1UL << 1,
        HPT_U = 1UL << 2,
        HPT_PWT = 1UL << 3,
        HPT_UC = 1UL << 4,
        HPT_A = 1UL << 5,
        HPT_D = 1UL << 6,
        HPT_S = 1UL << 7,
        HPT_G = 1UL << 8,
        HPT_NX = 0,
        // PTE_COW marks copy-on-write page table entries.
        HPT_COW = 1UL << 11,

        PTE_P = HPT_P,
        PTE_S = HPT_S,
        PTE_N = HPT_A | HPT_U | HPT_W | HPT_P,
        PTE_COW = HPT_COW,
        PTE_COW_IO = PTE_COW >> 1,
        PTE_W = HPT_W,
        PTE_U = HPT_U,
    };

    ALWAYS_INLINE
    inline Paddr addr() const {
        return static_cast<Paddr> (val) & ~PAGE_MASK;
    }

    ALWAYS_INLINE
    static inline mword hw_attr(mword a) {
        return a ? a | HPT_D | HPT_A | HPT_U | HPT_P : 0;
    }

    ALWAYS_INLINE
    static inline mword current() {
        mword addr;
        asm volatile ("mov %%cr3, %0" : "=r" (addr));
        return addr;
    }

    ALWAYS_INLINE
    inline void make_current(mword pcid) {
        asm volatile ("mov %0, %%cr3" : : "r" (val | pcid) : "memory");
    }

    static inline void cow_flush() {
        flush();
    }

    bool sync_from(Quota &quota, Hpt, mword, mword);

    void sync_master_range(Quota &quota, mword, mword);

    Paddr replace(Quota &quota, mword, mword);
    Paddr replace_cow(Quota &quota, mword, mword);
    
    static void *remap(Quota &quota, Paddr);

    static bool dest_hpt(Paddr p, mword, unsigned) {
        return (p != reinterpret_cast<Paddr> (&FRAME_0) && p != reinterpret_cast<Paddr> (&FRAME_1));
    }

    static bool iter_hpt_lev(unsigned l, mword v) {
#ifdef __x86_64__
        if (sizeof (v) > 4 && (v & (1ULL << 47)))
            v |= ~((1ULL << 48) - 1);
#endif

        return l >= 2 || (l == 1 && v >= SPC_LOCAL_OBJ);
    }

    static bool dest_loc(Paddr, mword v, unsigned l) {
        return v >= USER_ADDR && l >= 3;
    }

    static bool iter_loc_lev(unsigned l, mword) {
        return l > 3;
    }

    static void *remap_cow(Quota &quota, Paddr, mword addr = 0);

    static inline void cow_flush(mword v) {
        flush(v);
    }
    
    bool is_cow_fault(Quota &quota, mword, mword);
    
    void print_table(Quota &quota, mword);
};

class Hptp : public Hpt {
public:

    ALWAYS_INLINE
    inline explicit Hptp(mword v = 0) {
        val = v;
    }
};
