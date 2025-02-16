/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=8 sts=4 et sw=4 tw=99:
 *
 * Copyright 2016 Mozilla Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wasm/WasmCompartment.h"

#include "jscompartment.h"

#include "wasm/WasmInstance.h"

#include "vm/Debugger-inl.h"

using namespace js;
using namespace wasm;

Compartment::Compartment(Zone* zone)
  : mutatingInstances_(false),
    interruptedCount_(0)
{}

Compartment::~Compartment()
{
    MOZ_ASSERT(interruptedCount_ == 0);
    MOZ_ASSERT(instances_.empty());
    MOZ_ASSERT(!mutatingInstances_);
}

struct InstanceComparator
{
    const Instance& target;
    explicit InstanceComparator(const Instance& target) : target(target) {}

    int operator()(const Instance* instance) const {
        if (instance == &target)
            return 0;
        MOZ_ASSERT(!target.codeSegment().containsCodePC(instance->codeBase()));
        MOZ_ASSERT(!instance->codeSegment().containsCodePC(target.codeBase()));
        return target.codeBase() < instance->codeBase() ? -1 : 1;
    }
};

void
Compartment::trace(JSTracer* trc)
{
    // A WasmInstanceObject that was initially reachable when called can become
    // unreachable while executing on the stack. When execution in a compartment
    // is interrupted inside wasm code, wasm::TraceActivations() may miss frames
    // due to its use of FrameIterator which assumes wasm has exited through an
    // exit stub. This could be fixed by changing wasm::TraceActivations() to
    // use a ProfilingFrameIterator, which inspects register state, but for now
    // just mark everything in the compartment in this super-rare case.

    if (interruptedCount_) {
        for (Instance* i : instances_)
            i->trace(trc);
    }
}

bool
Compartment::registerInstance(JSContext* cx, HandleWasmInstanceObject instanceObj)
{
    Instance& instance = instanceObj->instance();
    MOZ_ASSERT(this == &instance.compartment()->wasm);

    instance.code().ensureProfilingLabels(cx->runtime()->geckoProfiler().enabled());

    size_t index;
    if (BinarySearchIf(instances_, 0, instances_.length(), InstanceComparator(instance), &index))
        MOZ_CRASH("duplicate registration");

    {
        AutoMutateInstances guard(*this);
        if (!instances_.insert(instances_.begin() + index, &instance)) {
            ReportOutOfMemory(cx);
            return false;
        }
    }

    Debugger::onNewWasmInstance(cx, instanceObj);
    return true;
}

void
Compartment::unregisterInstance(Instance& instance)
{
    size_t index;
    if (!BinarySearchIf(instances_, 0, instances_.length(), InstanceComparator(instance), &index))
        return;

    AutoMutateInstances guard(*this);
    instances_.erase(instances_.begin() + index);
}

struct PCComparator
{
    const void* pc;
    explicit PCComparator(const void* pc) : pc(pc) {}

    int operator()(const Instance* instance) const {
        if (instance->codeSegment().containsCodePC(pc))
            return 0;
        return pc < instance->codeBase() ? -1 : 1;
    }
};

Code*
Compartment::lookupCode(const void* pc) const
{
    Instance* instance = lookupInstanceDeprecated(pc);
    return instance ? &instance->code() : nullptr;
}

Instance*
Compartment::lookupInstanceDeprecated(const void* pc) const
{
    // lookupInstanceDeprecated can be called asynchronously from the interrupt
    // signal handler. In that case, the signal handler is just asking whether
    // the pc is in wasm code. If instances_ is being mutated then we can't be
    // executing wasm code so returning nullptr is fine.
    if (mutatingInstances_)
        return nullptr;

    size_t index;
    if (!BinarySearchIf(instances_, 0, instances_.length(), PCComparator(pc), &index))
        return nullptr;

    return instances_[index];
}

void
Compartment::setInterrupted(bool interrupted)
{
    if (interrupted) {
        interruptedCount_++;
    } else {
        MOZ_ASSERT(interruptedCount_ > 0);
        interruptedCount_--;
    }
}

void
Compartment::ensureProfilingLabels(bool profilingEnabled)
{
    for (Instance* instance : instances_)
        instance->code().ensureProfilingLabels(profilingEnabled);
}

void
Compartment::addSizeOfExcludingThis(MallocSizeOf mallocSizeOf, size_t* compartmentTables)
{
    *compartmentTables += instances_.sizeOfExcludingThis(mallocSizeOf);
}
