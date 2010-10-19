/*
 * Semaphore
 *
 * Copyright (C) 2009-2010 Udo Steinberg <udo@hypervisor.org>
 * Economic rights: Technische Universitaet Dresden (Germany)
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

#include "initprio.h"
#include "sm.h"

INIT_PRIORITY (PRIO_SLAB)
Slab_cache Sm::cache (sizeof (Sm), 32);

Sm::Sm (Pd *own, mword sel, mword cnt) : Kobject (own, sel, SM), counter (cnt)
{
    trace (TRACE_SYSCALL, "SM:%p created (CNT:%lu)", this, cnt);
}
