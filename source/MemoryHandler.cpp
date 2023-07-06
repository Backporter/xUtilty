#include "../include/MemoryHandler.h"

//
#include "../include/SystemWrapper.h"
#include "../include/MessageHandler.h"
#include "../include/Macro.h"

namespace xUtilty
{
	void DumpRegisters()
	{
#if __clang__
		// float  ymm0 = 0, ymm1 = 0, ymm2 = 0, ymm3 = 0, ymm4 = 0, ymm5 = 0, ymm6 = 0, ymm7 = 0, ymm8 = 0, ymm9 = 0, ymm10 = 0, ymm11 = 0, ymm12 = 0, ymm13 = 0, ymm14 = 0, ymm15 = 0;
		double xmm0 = 0, xmm1 = 0, xmm2 = 0, xmm3 = 0, xmm4 = 0, xmm5 = 0, xmm6 = 0, xmm7 = 0, xmm8 = 0, xmm9 = 0, xmm10 = 0, xmm11 = 0, xmm12 = 0, xmm13 = 0, xmm14 = 0, xmm15 = 0;
		int64_t rax = 0, rcx = 0, rdx = 0, rbx = 0, rsi = 0, rdi = 0, rsp = 0, rbp = 0, r8 = 0, r9 = 0, r10 = 0, r11 = 0, r12 = 0, r13 = 0, r14 = 0, r15 = 0, rip = 0, BrF = 0, BrT = 0;
		int32_t eax = 0, ecx = 0, edx = 0, ebx = 0, esi = 0, edi = 0, esp = 0, ebp = 0, r8d = 0, r9d = 0, r10d = 0, r11d = 0, r12d = 0, r13d = 0, r14d = 0, r15d = 0, eflags = 0;
		int16_t ax = 0, cx = 0, dx = 0, bx = 0, si = 0, di = 0, sp = 0, bp = 0, r8w = 0, r9w = 0, r10w = 0, r11w = 0, r12w = 0, r13w = 0, r14w = 0, r15w = 0;
		int8_t  al = 0, cl = 0, dl = 0, bl = 0, sil = 0, dil = 0, spl = 0, bpl = 0, r8b = 0, r9b = 0, r10b = 0, r11b = 0, r12b = 0, r13b = 0, r14b = 0, r15b = 0;

#if __ORBIS__
		__asm volatile("mov %%xmm0, %[Var]" : [Var] "=r" (xmm0));
		__asm volatile("mov %%xmm1, %[Var]" : [Var] "=r" (xmm1));
		__asm volatile("mov %%xmm2, %[Var]" : [Var] "=r" (xmm2));
		__asm volatile("mov %%xmm3, %[Var]" : [Var] "=r" (xmm3));
		__asm volatile("mov %%xmm4, %[Var]" : [Var] "=r" (xmm4));
		__asm volatile("mov %%xmm5, %[Var]" : [Var] "=r" (xmm5));
		__asm volatile("mov %%xmm6, %[Var]" : [Var] "=r" (xmm6));
		__asm volatile("mov %%xmm7, %[Var]" : [Var] "=r" (xmm7));
		__asm volatile("mov %%xmm8, %[Var]" : [Var] "=r" (xmm8));
		__asm volatile("mov %%xmm9, %[Var]" : [Var] "=r" (xmm9));
		__asm volatile("mov %%xmm10, %[Var]" : [Var] "=r" (xmm10));
		__asm volatile("mov %%xmm11, %[Var]" : [Var] "=r" (xmm11));
		__asm volatile("mov %%xmm12, %[Var]" : [Var] "=r" (xmm12));
		__asm volatile("mov %%xmm13, %[Var]" : [Var] "=r" (xmm13));
		__asm volatile("mov %%xmm14, %[Var]" : [Var] "=r" (xmm14));
		__asm volatile("mov %%xmm15, %[Var]" : [Var] "=r" (xmm15));
#endif

		__asm volatile("mov %%rax, %[Var]" : [Var] "=r" (rax));
		__asm volatile("mov %%rcx, %[Var]" : [Var] "=r" (rcx));
		__asm volatile("mov %%rdx, %[Var]" : [Var] "=r" (rdx));
		__asm volatile("mov %%rbx, %[Var]" : [Var] "=r" (rbx));
		__asm volatile("mov %%rsi, %[Var]" : [Var] "=r" (rsi));
		__asm volatile("mov %%rdi, %[Var]" : [Var] "=r" (rdi));
		__asm volatile("mov %%rsp, %[Var]" : [Var] "=r" (rsp));
		__asm volatile("mov %%rbp, %[Var]" : [Var] "=r" (rbp));
		__asm volatile("mov %%r8, %[Var]"  : [Var] "=r" (r8));
		__asm volatile("mov %%r9, %[Var]"  : [Var] "=r" (r9));
		__asm volatile("mov %%r10, %[Var]" : [Var] "=r" (r10));
		__asm volatile("mov %%r11, %[Var]" : [Var] "=r" (r11));
		__asm volatile("mov %%r12, %[Var]" : [Var] "=r" (r12));
		__asm volatile("mov %%r13, %[Var]" : [Var] "=r" (r13));
		__asm volatile("mov %%r14, %[Var]" : [Var] "=r" (r14));
		__asm volatile("mov %%r15, %[Var]" : [Var] "=r" (r15));

		__asm volatile("mov %%eax, %[Var]" : [Var] "=r" (eax));
		__asm volatile("mov %%ecx, %[Var]" : [Var] "=r" (ecx));
		__asm volatile("mov %%edx, %[Var]" : [Var] "=r" (edx));
		__asm volatile("mov %%ebx, %[Var]" : [Var] "=r" (ebx));
		__asm volatile("mov %%esi, %[Var]" : [Var] "=r" (esi));
		__asm volatile("mov %%edi, %[Var]" : [Var] "=r" (edi));
		__asm volatile("mov %%esp, %[Var]" : [Var] "=r" (esp));
		__asm volatile("mov %%ebp, %[Var]" : [Var] "=r" (ebp));
		__asm volatile("mov %%r8d, %[Var]" : [Var] "=r" (r8d));
		__asm volatile("mov %%r9d, %[Var]" : [Var] "=r" (r9d));
		__asm volatile("mov %%r10d, %[Var]" : [Var] "=r" (r10d));
		__asm volatile("mov %%r11d, %[Var]" : [Var] "=r" (r11d));
		__asm volatile("mov %%r12d, %[Var]" : [Var] "=r" (r12d));
		__asm volatile("mov %%r13d, %[Var]" : [Var] "=r" (r13d));
		__asm volatile("mov %%r14d, %[Var]" : [Var] "=r" (r14d));
		__asm volatile("mov %%r15d, %[Var]" : [Var] "=r" (r15d));

		__asm volatile("mov %%ax, %[Var]" : [Var] "=r" (ax));
		__asm volatile("mov %%cx, %[Var]" : [Var] "=r" (cx));
		__asm volatile("mov %%dx, %[Var]" : [Var] "=r" (dx));
		__asm volatile("mov %%bx, %[Var]" : [Var] "=r" (bx));
		__asm volatile("mov %%si, %[Var]" : [Var] "=r" (si));
		__asm volatile("mov %%di, %[Var]" : [Var] "=r" (di));
		__asm volatile("mov %%sp, %[Var]" : [Var] "=r" (sp));
		__asm volatile("mov %%bp, %[Var]" : [Var] "=r" (bp));
		__asm volatile("mov %%r8w, %[Var]" : [Var] "=r" (r8w));
		__asm volatile("mov %%r9w, %[Var]" : [Var] "=r" (r9w));
		__asm volatile("mov %%r10w, %[Var]" : [Var] "=r" (r10w));
		__asm volatile("mov %%r11w, %[Var]" : [Var] "=r" (r11w));
		__asm volatile("mov %%r12w, %[Var]" : [Var] "=r" (r12w));
		__asm volatile("mov %%r13w, %[Var]" : [Var] "=r" (r13w));
		__asm volatile("mov %%r14w, %[Var]" : [Var] "=r" (r14w));
		__asm volatile("mov %%r15w, %[Var]" : [Var] "=r" (r15w));

		__asm volatile("mov %%al, %[Var]" : [Var] "=r" (al));
		__asm volatile("mov %%cl, %[Var]" : [Var] "=r" (cl));
		__asm volatile("mov %%dl, %[Var]" : [Var] "=r" (dl));
		__asm volatile("mov %%bl, %[Var]" : [Var] "=r" (bl));
		__asm volatile("mov %%sil, %[Var]" : [Var] "=r" (sil));
		__asm volatile("mov %%dil, %[Var]" : [Var] "=r" (dil));
		__asm volatile("mov %%spl, %[Var]" : [Var] "=r" (spl));
		__asm volatile("mov %%bpl, %[Var]" : [Var] "=r" (bpl));
		__asm volatile("mov %%r8b, %[Var]" : [Var] "=r" (r8b));
		__asm volatile("mov %%r9b, %[Var]" : [Var] "=r" (r9b));
		__asm volatile("mov %%r10b, %[Var]" : [Var] "=r" (r10b));
		__asm volatile("mov %%r11b, %[Var]" : [Var] "=r" (r11b));
		__asm volatile("mov %%r12b, %[Var]" : [Var] "=r" (r12b));
		__asm volatile("mov %%r13b, %[Var]" : [Var] "=r" (r13b));
		__asm volatile("mov %%r14b, %[Var]" : [Var] "=r" (r14b));
		__asm volatile("mov %%r15b, %[Var]" : [Var] "=r" (r15b));

		KernelPrintOut(
			"\n\n# rax: %016lx rbx: %016lx\n# rcx: %016lx rdx: %016lx\n# rsi: %016lx rdi: %016lx\n# rbp: %016lx rsp: %016lx\n# r8:  %016lx r9:  %016lx\n# r10: %016lx r11: %016lx\n# r12: %016lx r13: %016lx\n# r14: %016lx r15: %016lx\n# rip: %016lx eflags: %08x\n# BrF: %016lx BrT: %016lx"
			"\n\n# eax: %016x ebx: %016x\n# ecx: %016x edx: %016x\n# esi: %016x edi: %016x\n# ebp: %016x esp: %016x\n# r8d: %016x r9d: %016x\n# r10d: %016x r11d: %016x\n# r12d: %016x r13d: %016x\n# r14d: %016x r15d: %016x\n\n"
			"\n\n# xmm0: %f xmm1: %f\n# xmm2: %f xmm3: %f\n# xmm4: %f xmm5: %f\n# xmm6: %f xmm7: %f\n# xmm8: %f xmm9: %f\n# xmm10: %f xmm11: %f\n# xmm12: %f xmm13: %f\n# xmm14: %f xmm15: %f",
			rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15, rip, eflags, BrF, BrT, // 64-bit
			eax, ebx, ecx, edx, esi, edi, ebp, esp, r8d, r9d, r10d, r11d, r12d, r13d, r14d, r15d,				 // 32-bit
			xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15 // xmmX
		);
#endif
	}

	void safe_write(uint64_t dst, const void* src, size_t len)
	{
		if (dst)
		{
			int32_t prot = VM_PROT_ALL;
			int32_t ret = SystemWrapper::QueryMemoryProtection((void*)dst, NULL, NULL, &prot);
			if (ret)
			{
				KernelPrintOut("SystemWrapper::QueryMemoryProtection(0x%lx, NULL, NULL, &prot) failed with 0x%x", dst, ret);
				return;
			}

			SystemWrapper::Mprotect((const void*)dst, len, VM_PROT_ALL);
			memcpy((void *)dst, src, len);
			SystemWrapper::Mprotect((void*)dst, len, prot);
		}
		else
		{
			PRINT_RETURN_N("Failed to write memory, destination address is invalid (0x%lx, %p, 0x%lx)", dst, src, len);
		}
	}

	void safe_read(const void* dst, uint64_t src, size_t len)
	{
		if (src)
		{
			int32_t prot = VM_PROT_ALL;
			int32_t ret = SystemWrapper::QueryMemoryProtection((void*)src, NULL, NULL, &prot);

			if (ret)
			{
				KernelPrintOut("SystemWrapper::QueryMemoryProtection(0x%lx, NULL, NULL, &prot) failed with 0x%x", src, ret);
				return;
			}

			SystemWrapper::Mprotect((void*)src, len, VM_PROT_ALL);
			memcpy((void *)src, dst, len);
			SystemWrapper::Mprotect((void*)src, len, prot);
		}
		else
		{
			PRINT_RETURN_N("Failed to write memory, destination address is invalid (0x%lx, %p, 0x%lx", src, dst, len);
		}
	}
}