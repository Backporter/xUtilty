#pragma once

#include <switch.h>

// we only need these
Result svcSetHeapSize(void** out_addr, uint64_t size);
Result svcSetMemoryPermission(void* addr, uint64_t size, uint32_t perm);
Result svcSetMemoryAttribute(void* addr, uint64_t size, uint32_t val0, uint32_t val1);
Result svcMapMemory(void* dst_addr, void* src_addr, uint64_t size);
Result svcUnmapMemory(void* dst_addr, void* src_addr, uint64_t size);
Result svcQueryMemory(MemoryInfo* meminfo_ptr, uint32_t* pageinfo, uint64_t addr);
Result svcMapSharedMemory(Handle handle, void* addr, size_t size, uint32_t perm);
Result svcUnmapSharedMemory(Handle handle, void* addr, size_t size);
Result svcMapPhysicalMemoryUnsafe(void* address, uint64_t size);
Result svcUnmapPhysicalMemoryUnsafe(void* address, uint64_t size);
Result svcSetProcessMemoryPermission(Handle proc, uint64_t addr, uint64_t size, uint32_t perm);
Result svcMapProcessMemory(void* dst, Handle proc, uint64_t src, uint64_t size);
Result svcUnmapProcessMemory(void* dst, Handle proc, uint64_t src, uint64_t size);
Result svcQueryProcessMemory(MemoryInfo* meminfo_ptr, uint32_t* pageinfo, Handle proc, uint64_t addr);
Result svcMapProcessCodeMemory(Handle proc, uint64_t dst, uint64_t src, uint64_t size);
Result svcUnmapProcessCodeMemory(Handle proc, uint64_t dst, uint64_t src, uint64_t size);
Result svcMapPhysicalMemory(void* address, uint64_t size);
Result svcUnmapPhysicalMemory(void* address, uint64_t size);
Result svcQueryDebugProcessMemory(MemoryInfo* meminfo_ptr, uint32_t* pageinfo, Handle debug, uint64_t addr);
Result svcReadDebugProcessMemory(void* buffer, Handle debug, uint64_t addr, uint64_t size);
Result svcWriteDebugProcessMemory(Handle debug, const void* buffer, uint64_t addr, uint64_t size);
Result svcOutputDebugString(const char* str, uint64_t size);
void NORETURN svcExitProcess(void);

// Result svcCreateThread(Handle* out, void* entry, void* arg, void* stack_top, int prio, int cpuid);
// Result svcStartThread(Handle handle);
// void NORETURN svcExitThread(void);
// void svcSleepThread(s64 nano);
// Result svcGetThreadPriority(uint32_t* priority, Handle handle);
// Result svcSetThreadPriority(Handle handle, uint32_t priority);
// Result svcGetThreadCoreMask(s32* preferred_core, uint32_t* affinity_mask, Handle handle);
// Result svcSetThreadCoreMask(Handle handle, s32 preferred_core, uint32_t affinity_mask);
// uint32_t svcGetCurrentProcessorNumber(void);
// Result svcSignalEvent(Handle handle);
// Result svcClearEvent(Handle handle);
// 
// Result svcCreateTransferMemory(Handle* out, void* addr, size_t size, uint32_t perm);
// Result svcCloseHandle(Handle handle);
// Result svcResetSignal(Handle handle);
// Result svcWaitSynchronization(s32* index, const Handle* handles, s32 handleCount, uint64_t timeout);
// static inline Result svcWaitSynchronizationSingle(Handle handle, uint64_t timeout) {
//     s32 tmp;
//     return svcWaitSynchronization(&tmp, &handle, 1, timeout);
// }
// 
// Result svcCancelSynchronization(Handle thread);
// Result svcArbitrateLock(uint32_t wait_tag, uint32_t* tag_location, uint32_t self_tag);
// Result svcArbitrateUnlock(uint32_t* tag_location);
// Result svcWaitProcessWideKeyAtomic(uint32_t* key, uint32_t* tag_location, uint32_t self_tag, uint64_t timeout);
// void svcSignalProcessWideKey(uint32_t* key, s32 num);
// uint64_t svcGetSystemTick(void);
// Result svcConnectToNamedPort(Handle* session, const char* name);
// Result svcSendSyncRequest(Handle session);
// Result svcSendSyncRequestWithUserBuffer(void* usrBuffer, uint64_t size, Handle session);
// Result svcSendAsyncRequestWithUserBuffer(Handle* handle, void* usrBuffer, uint64_t size, Handle session);
// Result svcGetProcessId(uint64_t* processID, Handle handle);
// Result svcGetThreadId(uint64_t* threadID, Handle handle);
// Result svcBreak(uint32_t breakReason, uint64_t inval1, uint64_t inval2);
// 
// void NORETURN svcReturnFromException(Result res);
// Result svcGetInfo(uint64_t* out, uint32_t id0, Handle handle, uint64_t id1);
// Result svcGetResourceLimitLimitValue(uint64_t* out, Handle reslimit_h, LimitableResource which);
// Result svcGetResourceLimitCurrentValue(uint64_t* out, Handle reslimit_h, LimitableResource which);
// Result svcSetThreadActivity(Handle thread, bool paused);
// Result svcGetThreadContext3(ThreadContext* ctx, Handle thread);
// Result svcCreateSession(Handle* server_handle, Handle* client_handle, uint32_t unk0, uint64_t unk1);  // unk* are normally 0?
// Result svcAcceptSession(Handle* session_handle, Handle port_handle);
// Result svcReplyAndReceive(s32* index, const Handle* handles, s32 handleCount, Handle replyTarget, uint64_t timeout);
// Result svcReplyAndReceiveWithUserBuffer(s32* index, void* usrBuffer, uint64_t size, const Handle* handles, s32 handleCount, Handle replyTarget, uint64_t timeout);
// Result svcCreateEvent(Handle* server_handle, Handle* client_handle);
// Result svcSetUnsafeLimit(uint64_t size);
// Result svcCreateCodeMemory(Handle* code_handle, void* src_addr, uint64_t size);
// Result svcControlCodeMemory(Handle code_handle, CodeMapOperation op, void* dst_addr, uint64_t size, uint64_t perm);
// Result svcReadWriteRegister(uint32_t* outVal, uint64_t regAddr, uint32_t rwMask, uint32_t inVal);
// Result svcCreateSharedMemory(Handle* out, size_t size, uint32_t local_perm, uint32_t other_perm);
// Result svcMapTransferMemory(Handle tmem_handle, void* addr, size_t size, uint32_t perm);
// Result svcUnmapTransferMemory(Handle tmem_handle, void* addr, size_t size);
// Result svcCreateInterruptEvent(Handle* handle, uint64_t irqNum, uint32_t flag);
// Result svcQueryPhysicalAddress(uint64_t out[3], uint64_t virtaddr);
// Result svcQueryIoMapping(uint64_t* virtaddr, uint64_t physaddr, uint64_t size);
// Result svcCreateDeviceAddressSpace(Handle* handle, uint64_t dev_addr, uint64_t dev_size);
// Result svcAttachDeviceAddressSpace(uint64_t device, Handle handle);
// Result svcDetachDeviceAddressSpace(uint64_t device, Handle handle);
// Result svcMapDeviceAddressSpaceByForce(Handle handle, Handle proc_handle, uint64_t map_addr, uint64_t dev_size, uint64_t dev_addr, uint32_t perm);
// Result svcMapDeviceAddressSpaceAligned(Handle handle, Handle proc_handle, uint64_t map_addr, uint64_t dev_size, uint64_t dev_addr,uint32_t perm);
// Result svcUnmapDeviceAddressSpace(Handle handle, Handle proc_handle, uint64_t map_addr, uint64_t map_size, uint64_t dev_addr);
// Result svcDebugActiveProcess(Handle* debug, uint64_t processID);
// Result svcBreakDebugProcess(Handle debug);
// Result svcTerminateDebugProcess(Handle debug);
// Result svcGetDebugEvent(u8* event_out, Handle debug);
// Result svcContinueDebugEvent(Handle debug, uint32_t flags, uint64_t* tid_list, uint32_t num_tids);
// Result svcLegacyContinueDebugEvent(Handle debug, uint32_t flags, uint64_t threadID);
// Result svcGetDebugThreadContext(ThreadContext* ctx, Handle debug, uint64_t threadID, uint32_t flags);
// Result svcSetDebugThreadContext(Handle debug, uint64_t threadID, const ThreadContext* ctx, uint32_t flags);
// Result svcGetProcessList(uint32_t* num_out, uint64_t* pids_out, uint32_t max_pids);
// Result svcGetThreadList(uint32_t* num_out, uint64_t* tids_out, uint32_t max_tids, Handle debug);
// Result svcGetDebugThreadParam(uint64_t* out_64, uint32_t* out_32, Handle debug, uint64_t threadID, DebugThreadParam param);
// Result svcGetSystemInfo(uint64_t* out, uint64_t id0, Handle handle, uint64_t id1);
// Result svcCreatePort(Handle* portServer, Handle* portClient, s32 max_sessions, bool is_light, const char* name);
// Result svcManageNamedPort(Handle* portServer, const char* name, s32 maxSessions);
// Result svcConnectToPort(Handle* session, Handle port);
// Result svcCreateProcess(Handle* out, const void* proc_info, const uint32_t* caps, uint64_t cap_num);
// Result svcStartProcess(Handle proc, s32 main_prio, s32 default_cpu, uint32_t stack_size);
// Result svcTerminateProcess(Handle proc);
// Result svcGetProcessInfo(uint64_t* out, Handle proc, ProcessInfoType which);
// Result svcCreateResourceLimit(Handle* out);
// Result svcSetResourceLimitLimitValue(Handle reslimit, LimitableResource which, uint64_t value);
// uint64_t svcCallSecureMonitor(SecmonArgs* regs);