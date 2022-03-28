.code

HasCPUIDSupport proc
	; Read flags into RAX
	pushfq           
	pop rax          

	; Set ID (CPUID) flag and write back to rflags
	bts rax, 21  
	push rax         
	popfq            

	; Read flags back from rflags and check if the ID flag was cleared
	pushfq           
	pop rax          
	bt rax, 21
	
	; Return whether the ID flag persisted
	mov rax, 0
	mov rcx, 1
	cmovc rax, rcx 
	ret
HasCPUIDSupport endp


HasAVXSupport proc
	; Return early if CPU does not support CPUID
	call HasCPUIDSupport
	cmp rax, 0
	jne SupportsCPUID
	ret

	SupportsCPUID:
	; Call CPUID function #1 (Processor Info and feature bits)
	mov eax, 1 
	cpuid

	; Return feature bit ECX[28] (AVX support)
	mov rax, 0  
	mov rdx, 1
	bt ecx, 28
	cmovc rax, rdx
	ret
HasAVXSupport endp

end
