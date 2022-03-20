.code
?HasAVXSupport@AVX256@@SA_NXZ proc
	; Call CPUID function #1 (Processor Info and feature bits)
	mov eax, 1 
	cpuid

	; Return feature bit ECX[28] (AVX support)
	mov rax, 0  
	mov rdx, 1
	bt ecx, 28
	cmovc rax, rdx
	ret
?HasAVXSupport@AVX256@@SA_NXZ endp
end