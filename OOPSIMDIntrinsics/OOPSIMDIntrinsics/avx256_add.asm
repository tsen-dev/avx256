.code

Add64 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddq ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add64 endp

Add32 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddd ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add32 endp

Add16 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddw ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add16 endp

Add16Saturate proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddsw ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add16Saturate endp

Add16USaturate proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddusw ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add16USaturate endp

Add8 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddb ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add8 endp

Add8Saturate proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddsb ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add8Saturate endp

Add8USaturate proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpaddusb ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Add8USaturate endp

end