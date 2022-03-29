.code

Sub64 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubq ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub64 endp

Sub32 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubd ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub32 endp

Sub16 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubw ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub16 endp

Sub16SaturateSigned proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubsw ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub16SaturateSigned endp

Sub16SaturateUnsigned proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubusw ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub16SaturateUnsigned endp

Sub8 proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubb ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub8 endp

Sub8SaturateSigned proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubsb ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub8SaturateSigned endp

Sub8SaturateUnsigned proc
	vmovdqu ymm0, ymmword ptr [rcx]
	vpsubusb ymm0, ymm0, ymmword ptr [rdx]
	vmovdqu ymmword ptr [rcx], ymm0
	ret
Sub8SaturateUnsigned endp

end