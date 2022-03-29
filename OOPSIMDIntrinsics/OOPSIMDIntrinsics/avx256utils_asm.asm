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

HasCPUIDExtendedFeatures proc	
	push rbx
	; Call CPUID function #0 (Highest function parameter and Manufacturer ID)
	mov rax, 0
	cpuid

	; Return whether CPIUD function #7 (Extended Features) is supported
	mov rcx, 1
	mov rdx, 0
	cmp rax, 7
	cmovge rax, rcx
	cmovl rax, rdx
	pop rbx
	ret
HasCPUIDExtendedFeatures endp

HasAVX2Support proc
	; Return false if CPU does not support CPUID
	call HasCPUIDSupport
	cmp rax, 0
	jne SupportsCPUID
	ret

	SupportsCPUID:
	; Return false if CPU does not support CPUID function #7 (Extended Features)
	call HasCPUIDExtendedFeatures
	cmp rax, 0
	jne SupportsCPUIDExtendedFeatures
	ret

	SupportsCPUIDExtendedFeatures:	
	; Call CPUID function #7.0 (Extended features)
	push rbx
	mov rax, 7
	mov rcx, 0
	cpuid

	; Return feature bit RBX[5] (AVX2 Support)
	mov rax, 0
	mov rcx, 1
	bt rbx, 5
	cmovc rax, rcx
	pop rbx
	ret
HasAVX2Support endp

end
