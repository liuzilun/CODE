	.586p
	ifdef ??version
	if    ??version GT 500H
	.mmx
	endif
	endif
	model flat
	ifndef	??version
	?debug	macro
	endm
	endif
	?debug	S "D:\MYDATA\IDPROG\GETID\MForm.cpp"
	?debug	T "D:\MYDATA\IDPROG\GETID\MForm.cpp"
_TEXT	segment dword public use32 'CODE'
_TEXT	ends
_DATA	segment dword public use32 'DATA'
_DATA	ends
_BSS	segment dword public use32 'BSS'
_BSS	ends
 ?debug  C FB0C0100600000
DGROUP	group	_BSS,_DATA
_BSS	segment dword public use32 'BSS'
	align	4
_Form1	label	dword
	db	4	dup(?)
_BSS	ends
_DATA	segment dword public use32 'DATA'
@_$DCJE$@TForm1@$bctr$qqrp18Classes@TComponent	segment virtual
	align	2
@@_$DCJE$@TForm1@$bctr$qqrp18Classes@TComponent	label	dword
	dd	@@$xt$p6TForm1
	dd	1029
	dd	-4
	dd	0
@_$DCJE$@TForm1@$bctr$qqrp18Classes@TComponent	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTKE$@TForm1@$bctr$qqrp18Classes@TComponent	segment virtual
	align	2
@@_$ECTKE$@TForm1@$bctr$qqrp18Classes@TComponent	label	dword
	dd	0
	dd	-40
	dw	0
	dw	5
	dd	0
	dd	@@_$DCJE$@TForm1@$bctr$qqrp18Classes@TComponent
@_$ECTKE$@TForm1@$bctr$qqrp18Classes@TComponent	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@TForm1@$bctr$qqrp18Classes@TComponent	segment virtual
	align	2
@@TForm1@$bctr$qqrp18Classes@TComponent	proc	near
?live16385@0:
	push      ebp
	mov       ebp,esp
	add       esp,-44
	push      ebx
	mov       byte ptr [ebp-8],dl
	test      dl,dl
	jle       short @1
	call      __ClassCreate
@1:
@2:
	mov       ebx,ecx
	mov       byte ptr [ebp-41],dl
	mov       dword ptr [ebp-4],eax
	mov       eax,offset @@_$ECTKE$@TForm1@$bctr$qqrp18Classes@TComponent
	call      @__InitExceptBlockLDTC
?live16385@16: ; EBX = Owner
	mov       word ptr [ebp-24],8
	mov       ecx,ebx
	xor       edx,edx
	mov       eax,dword ptr [ebp-4]
	call      @@Forms@TForm@$bctr$qqrp18Classes@TComponent
	add       dword ptr [ebp-12],15
?live16385@32: ; 
	mov       edx,dword ptr [ebp-40]
	mov       dword ptr fs:[0],edx
	mov       eax,dword ptr [ebp-4]
@4:
@3:
	mov       dl,byte ptr [ebp-41]
	test      dl,dl
	je        short @5
	call      __AfterConstruction
@5:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret 
@@TForm1@$bctr$qqrp18Classes@TComponent	endp
@TForm1@$bctr$qqrp18Classes@TComponent	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject	segment virtual
	align	2
@@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject	label	dword
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-4
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-8
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-12
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-16
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-20
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-24
	dd	0
@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTME$@TForm1@Button1Click$qqrp14System@TObject	segment virtual
	align	2
@@_$ECTME$@TForm1@Button1Click$qqrp14System@TObject	label	dword
	dd	0
	dd	-60
	dw	0
	dw	5
	dd	0
	dd	@@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject
	dw	0
	dw	5
	dd	0
	dd	@@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject+16
	dw	0
	dw	5
	dd	0
	dd	@@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject+32
	dw	0
	dw	5
	dd	0
	dd	@@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject+48
	dw	0
	dw	5
	dd	0
	dd	@@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject+64
	dw	0
	dw	5
	dd	0
	dd	@@_$DCLE$@TForm1@Button1Click$qqrp14System@TObject+80
@_$ECTME$@TForm1@Button1Click$qqrp14System@TObject	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@TForm1@Button1Click$qqrp14System@TObject	segment virtual
	align	2
@@TForm1@Button1Click$qqrp14System@TObject	proc	near
?live16390@0:
	push      ebp
	mov       ebp,esp
	add       esp,-2084
	push      ebx
	push      esi
	mov       esi,eax
@6:
	mov       eax,offset @@_$ECTME$@TForm1@Button1Click$qqrp14System@TObject
	call      @__InitExceptBlockLDTC
	push	 eax
	push	 ebx
	push	 ecx
	push	 edx
	xor	 eax,eax
	xor	 ebx,ebx
	xor	 ecx,ecx
	xor	 edx,edx
	mov	 eax,0
	cpuid	
	mov	 dword ptr [ebp-64],eax
	mov	 dword ptr [ebp-68],ebx
	mov	 dword ptr [ebp-72],ecx
	mov	 dword ptr [ebp-76],edx
	pop	 edx
	pop	 ecx
	pop	 ebx
	pop	 eax
?live16390@304: ; ESI = this
	push      360
	push      0
	lea       edx,dword ptr [ebp-444]
	push      edx
	call      @_memset
	add       esp,12
	mov       ecx,dword ptr [ebp-64]
	mov       dword ptr [ebp-432],ecx
	mov       eax,dword ptr [ebp-68]
	mov       dword ptr [ebp-428],eax
	mov       edx,dword ptr [ebp-72]
	mov       dword ptr [ebp-424],edx
	mov       ecx,dword ptr [ebp-76]
	mov       dword ptr [ebp-420],ecx
	mov       al,byte ptr [esi+740]
	test      al,al
	je        short @7
	mov       edx,dword ptr [ebp-76]
	push      edx
	mov       ecx,dword ptr [ebp-72]
	push      ecx
	lea       ecx,dword ptr [ebp-1444]
	mov       eax,dword ptr [ebp-68]
	push      eax
	mov       edx,dword ptr [ebp-64]
	push      edx
	push      offset s@
	push      ecx
	call      @_sprintf
	mov       word ptr [ebp-44],8
	add       esp,24
	lea       edx,dword ptr [ebp-1444]
	lea       eax,dword ptr [ebp-4]
	call      @@System@AnsiString@$bctr$qqrpxc
	inc       dword ptr [ebp-32]
	mov       eax,dword ptr [eax]
	call      @@Dialogs@ShowMessage$qqrx17System@AnsiString
	dec       dword ptr [ebp-32]
	lea       eax,dword ptr [ebp-4]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
@7:
	lea       ecx,dword ptr [ebp-336]
	push      10
	push      ecx
	lea       eax,dword ptr [ebp-436]
	push      eax
	lea       edx,dword ptr [ebp-440]
	push      edx
	lea       ecx,dword ptr [ebp-444]
	push      ecx
	lea       eax,dword ptr [ebp-366]
	push      40
	push      eax
	push      offset s@+36
	call      @GetVolumeInformationA
	mov       dl,byte ptr [esi+740]
	test      dl,dl
	je        short @8
	mov       ecx,dword ptr [ebp-440]
	lea       edx,dword ptr [ebp-336]
	push      ecx
	mov       eax,dword ptr [ebp-436]
	push      eax
	push      edx
	mov       ecx,dword ptr [ebp-444]
	lea       eax,dword ptr [ebp-366]
	push      ecx
	push      eax
	lea       edx,dword ptr [ebp-1444]
	push      offset s@+40
	push      edx
	call      @_sprintf
	mov       word ptr [ebp-44],20
	add       esp,28
	lea       edx,dword ptr [ebp-1444]
	lea       eax,dword ptr [ebp-8]
	call      @@System@AnsiString@$bctr$qqrpxc
	inc       dword ptr [ebp-32]
	mov       eax,dword ptr [eax]
	call      @@Dialogs@ShowMessage$qqrx17System@AnsiString
	dec       dword ptr [ebp-32]
	lea       eax,dword ptr [ebp-8]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
@8:
	mov       dword ptr [ebp-80],640
	lea       ecx,dword ptr [ebp-80]
	push      ecx
	lea       eax,dword ptr [ebp-2084]
	push      eax
	call      @GetAdaptersInfo
	push      8
	lea       edx,dword ptr [ebp-1680]
	push      edx
	lea       ecx,dword ptr [ebp-416]
	push      ecx
	call      @_memcpy
	add       esp,12
	lea       eax,dword ptr [ebp-80]
	mov       dword ptr [ebp-80],40
	push      eax
	lea       edx,dword ptr [ebp-406]
	push      edx
	call      @GetComputerNameA
	lea       ecx,dword ptr [ebp-2076]
	push      ecx
	lea       eax,dword ptr [ebp-326]
	push      eax
	call      @_strcat
	add       esp,8
	lea       edx,dword ptr [ebp-1816]
	push      edx
	lea       ecx,dword ptr [ebp-226]
	push      ecx
	call      @_strcat
	add       esp,8
	mov       eax,dword ptr [ebp-1668]
	mov       dword ptr [ebp-88],eax
	mov       dl,byte ptr [esi+740]
	test      dl,dl
	je        @9
	xor       ecx,ecx
	xor       eax,eax
	mov       cl,byte ptr [ebp-1673]
	xor       edx,edx
	and       ecx,255
	push      ecx
	xor       ecx,ecx
	mov       al,byte ptr [ebp-1674]
	and       eax,255
	push      eax
	xor       eax,eax
	mov       dl,byte ptr [ebp-1675]
	and       edx,255
	push      edx
	xor       edx,edx
	mov       cl,byte ptr [ebp-1676]
	and       ecx,255
	push      ecx
	xor       ecx,ecx
	mov       al,byte ptr [ebp-1677]
	and       eax,255
	push      eax
	xor       eax,eax
	mov       dl,byte ptr [ebp-1678]
	and       edx,255
	push      edx
	lea       edx,dword ptr [ebp-1444]
	mov       cl,byte ptr [ebp-1679]
	and       ecx,255
	push      ecx
	mov       al,byte ptr [ebp-1680]
	and       eax,255
	push      eax
	push      offset s@+139
	push      edx
	call      @_sprintf
	mov       word ptr [ebp-44],32
	add       esp,40
	lea       edx,dword ptr [ebp-1444]
	lea       eax,dword ptr [ebp-12]
	call      @@System@AnsiString@$bctr$qqrpxc
	inc       dword ptr [ebp-32]
	mov       eax,dword ptr [eax]
	call      @@Dialogs@ShowMessage$qqrx17System@AnsiString
	dec       dword ptr [ebp-32]
	lea       eax,dword ptr [ebp-12]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	mov       ecx,dword ptr [ebp-1668]
	lea       eax,dword ptr [ebp-226]
	push      ecx
	push      eax
	lea       edx,dword ptr [ebp-326]
	lea       ecx,dword ptr [ebp-406]
	push      edx
	push      ecx
	lea       eax,dword ptr [ebp-1444]
	push      offset s@+184
	push      eax
	call      @_sprintf
	mov       word ptr [ebp-44],44
	add       esp,24
	lea       edx,dword ptr [ebp-1444]
	lea       eax,dword ptr [ebp-16]
	call      @@System@AnsiString@$bctr$qqrpxc
	inc       dword ptr [ebp-32]
	mov       eax,dword ptr [eax]
	call      @@Dialogs@ShowMessage$qqrx17System@AnsiString
	dec       dword ptr [ebp-32]
	lea       eax,dword ptr [ebp-16]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
@9:
	push      offset s@+250
	xor       ecx,ecx
	mov       word ptr [ebp-44],56
	mov       dword ptr [ebp-20],ecx
	lea       edx,dword ptr [ebp-20]
	inc       dword ptr [ebp-32]
	mov       eax,dword ptr [esi+728]
	call      @@Controls@TControl@GetText$qqrv
	mov       ecx,dword ptr [ebp-20]
	test      ecx,ecx
	je        short @10
	mov       eax,dword ptr [ebp-20]
	jmp       short @11
@10:
	mov       eax,offset s@+254
@11:
	push      eax
	call      @_fopen
	add       esp,8
	mov       ebx,eax
	dec       dword ptr [ebp-32]
	lea       eax,dword ptr [ebp-20]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
?live16390@864: ; EBX = FP
	test      ebx,ebx
	jne       short @12
?live16390@880: ; 
	push      0
	mov       edx,offset s@+255
	mov       word ptr [ebp-44],68
	lea       eax,dword ptr [ebp-24]
	call      @@System@AnsiString@$bctr$qqrpxc
	inc       dword ptr [ebp-32]
	mov       ecx,dword ptr [eax]
	push      ecx
	push      2
	push      0
	lea       eax,dword ptr [ebp-82]
	push      eax
	call      @_memset
	add       esp,12
	lea       edx,dword ptr [ebp-82]
	or        byte ptr [ebp-82],1
	mov       cx,word ptr [edx]
	mov       dl,2
	pop       eax
	call      @@Dialogs@MessageDlg$qqrx17System@AnsiString19Dialogs@TMsgDlgType47System@%Set$t18Dialogs@TMsgDlgBtn$iuc$0$iuc$10%i
	dec       dword ptr [ebp-32]
	lea       eax,dword ptr [ebp-24]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	mov       ecx,dword ptr [ebp-60]
	mov       dword ptr fs:[0],ecx
	jmp       short @13
?live16390@912: ; EBX = FP
@12:
	push      ebx
	push      1
	push      360
	lea       eax,dword ptr [ebp-444]
	push      eax
	call      @_fwrite
	add       esp,16
	push      ebx
	call      @_fclose
	pop       ecx
?live16390@928: ; 
	mov       edx,dword ptr [ebp-60]
	mov       dword ptr fs:[0],edx
@14:
@13:
	pop       esi
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret 
@@TForm1@Button1Click$qqrp14System@TObject	endp
@TForm1@Button1Click$qqrp14System@TObject	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@TForm1@Button2Click$qqrp14System@TObject	segment virtual
	align	2
@@TForm1@Button2Click$qqrp14System@TObject	proc	near
?live16394@0:
@15:
	call      @@Forms@TCustomForm@Close$qqrv
@16:
	ret 
@@TForm1@Button2Click$qqrp14System@TObject	endp
@TForm1@Button2Click$qqrp14System@TObject	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@TForm1@FormKeyDown$qqrp14System@TObjectrus46System@%Set$t18Classes@Classes__1$iuc$0$iuc$6%	segment virtual
	align	2
@@TForm1@FormKeyDown$qqrp14System@TObjectrus46System@%Set$t18Classes@Classes__1$iuc$0$iuc$6%	proc	near
?live16395@0:
@17:
	push      ebp
	mov       ebp,esp
?live16395@16: ; EAX = this, ECX = Key
	cmp       word ptr [ecx],112
	jne       short @18
?live16395@32: ; EAX = this
	mov       byte ptr [eax+740],1
?live16395@48: ; 
@18:
@19:
	pop       ebp
	ret       4
@@TForm1@FormKeyDown$qqrp14System@TObjectrus46System@%Set$t18Classes@Classes__1$iuc$0$iuc$6%	endp
@TForm1@FormKeyDown$qqrp14System@TObjectrus46System@%Set$t18Classes@Classes__1$iuc$0$iuc$6%	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@_$DCNE$@Forms@TForm@$bctr$qqrp18Classes@TComponent	segment virtual
	align	2
@@_$DCNE$@Forms@TForm@$bctr$qqrp18Classes@TComponent	label	dword
	dd	@@$xt$p11Forms@TForm
	dd	1029
	dd	-4
	dd	0
@_$DCNE$@Forms@TForm@$bctr$qqrp18Classes@TComponent	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTOE$@Forms@TForm@$bctr$qqrp18Classes@TComponent	segment virtual
	align	2
@@_$ECTOE$@Forms@TForm@$bctr$qqrp18Classes@TComponent	label	dword
	dd	0
	dd	-40
	dw	0
	dw	5
	dd	0
	dd	@@_$DCNE$@Forms@TForm@$bctr$qqrp18Classes@TComponent
@_$ECTOE$@Forms@TForm@$bctr$qqrp18Classes@TComponent	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@Forms@TForm@$bctr$qqrp18Classes@TComponent	segment virtual
	align	2
@@Forms@TForm@$bctr$qqrp18Classes@TComponent	proc	near
?live16386@0:
	push      ebp
	mov       ebp,esp
	add       esp,-44
	push      ebx
	mov       byte ptr [ebp-8],dl
	test      dl,dl
	jle       short @20
	call      __ClassCreate
@20:
@21:
	mov       ebx,ecx
	mov       byte ptr [ebp-41],dl
	mov       dword ptr [ebp-4],eax
	mov       eax,offset @@_$ECTOE$@Forms@TForm@$bctr$qqrp18Classes@TComponent
	call      @__InitExceptBlockLDTC
	mov       word ptr [ebp-24],8
	mov       ecx,ebx
	xor       edx,edx
	mov       eax,dword ptr [ebp-4]
	call      @@Forms@TCustomForm@$bctr$qqrp18Classes@TComponent
	add       dword ptr [ebp-12],14
	mov       edx,dword ptr [ebp-40]
	mov       dword ptr fs:[0],edx
	mov       eax,dword ptr [ebp-4]
@23:
@22:
	mov       dl,byte ptr [ebp-41]
	test      dl,dl
	je        short @24
	call      __AfterConstruction
@24:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret 
@@Forms@TForm@$bctr$qqrp18Classes@TComponent	endp
@Forms@TForm@$bctr$qqrp18Classes@TComponent	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$p11Forms@TForm	segment virtual
	align	2
@@$xt$p11Forms@TForm	label	dword
	dd	4
	dw	144
	dw	12
	dd	@@$xt$11Forms@TForm
	db	84
	db	70
	db	111
	db	114
	db	109
	db	32
	db	42
	db	0
@$xt$p11Forms@TForm	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$17System@AnsiString	segment virtual
	align	2
@@$xt$17System@AnsiString	label	byte
	dd	4
	dw	3
	dw	48
	dd	-1
	dd	3
	dw	68
	dw	72
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@System@AnsiString@$bdtr$qqrv
	dw	3
	dw	76
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	58
	db	58
	db	65
	db	110
	db	115
	db	105
	db	83
	db	116
	db	114
	db	105
	db	110
	db	103
	db	0
	db	0
	dd	0
	dd	0
	dd	0
@$xt$17System@AnsiString	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$p6TForm1	segment virtual
	align	2
@@$xt$p6TForm1	label	dword
	dd	4
	dw	144
	dw	12
	dd	@@$xt$6TForm1
	db	84
	db	70
	db	111
	db	114
	db	109
	db	49
	db	32
	db	42
	db	0
@$xt$p6TForm1	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@TForm1@	segment virtual
	align	2
@@TForm1@	label	byte
	dd	@@TForm1@+76
	dd	0
	dd	0
	dd	0
	dd	@@$xp$6TForm1+4
	dd	@@TForm1@+300
	dd	@@TForm1@+375
	dd	0
	dd	@@TForm1@+433
	dd	744
	dd	@Forms@TForm@
	dd	@@Classes@TComponent@SafeCallException$qqrp14System@TObjectpv
	dd	@@Forms@TCustomForm@AfterConstruction$qqrv
	dd	@@Forms@TCustomForm@BeforeDestruction$qqrv
	dd	@@System@TObject@Dispatch$qqrpv
	dd	@@Forms@TCustomForm@DefaultHandler$qqrpv
	dd	@@System@TObject@NewInstance$qqrp17System@TMetaClass
	dd	@@System@TObject@FreeInstance$qqrv
	dd	@@TForm1@$bdtr$qqrv
	dd	@@Controls@TWinControl@AssignTo$qqrp19Classes@TPersistent
	dd	@@Forms@TCustomForm@DefineProperties$qqrp14Classes@TFiler
	dd	@@Classes@TPersistent@Assign$qqrp19Classes@TPersistent
	dd	@@Forms@TCustomForm@Loaded$qqrv
	dd	@@Forms@TCustomForm@Notification$qqrp18Classes@TComponent18Classes@TOperation
	dd	@@Forms@TCustomForm@ReadState$qqrp15Classes@TReader
	dd	@@Controls@TControl@SetName$qqrx17System@AnsiString
	dd	@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dd	@@Forms@TCustomForm@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2
	dd	@@Classes@TComponent@WriteState$qqrp15Classes@TWriter
	dd	@@Forms@TCustomForm@QueryInterface$qqsrx5_GUIDpv
	dd	@@TForm1@$bctr$qqrp18Classes@TComponent
	dd	@@Controls@TWinControl@CanResize$qqrrit1
	dd	@@Controls@TWinControl@CanAutoSize$qqrrit1
	dd	@@Controls@TWinControl@ConstrainedResize$qqrrit1t1t1
	dd	@@Controls@TWinControl@GetClientOrigin$qqrv
	dd	@@Forms@TCustomForm@GetClientRect$qqrv
	dd	@@Controls@TWinControl@GetDeviceContext$qqrrpv
	dd	@@Controls@TControl@GetDragImages$qqrv
	dd	@@Controls@TControl@GetEnabled$qqrv
	dd	@@Forms@TCustomForm@GetFloating$qqrv
	dd	@@Controls@TControl@GetFloatingDockSiteClass$qqrv
	dd	@@Controls@TControl@SetDragMode$qqr18Controls@TDragMode
	dd	@@Controls@TControl@SetEnabled$qqro
	dd	@@Forms@TCustomForm@SetParent$qqrp20Controls@TWinControl
	dd	@@Forms@TCustomForm@SetParentBiDiMode$qqro
	dd	@@Controls@TControl@SetBiDiMode$qqr17Classes@TBiDiMode
	dd	@@Forms@TCustomForm@WndProc$qqrr17Messages@TMessage
	dd	@@Controls@TControl@InitiateAction$qqrv
	dd	@@Controls@TWinControl@Invalidate$qqrv
	dd	@@Controls@TWinControl@Repaint$qqrv
	dd	@@Controls@TWinControl@SetBounds$qqriiii
	dd	@@Controls@TWinControl@Update$qqrv
	dd	@@Forms@TScrollingWinControl@AdjustClientRect$qqrr13Windows@TRect
	dd	@@Forms@TCustomForm@AlignControls$qqrp17Controls@TControlr13Windows@TRect
	dd	@@Controls@TWinControl@CreateHandle$qqrv
	dd	@@Forms@TCustomForm@CreateParams$qqrr22Controls@TCreateParams
	dd	@@Forms@TCustomForm@CreateWindowHandle$qqrrx22Controls@TCreateParams
	dd	@@Forms@TCustomForm@CreateWnd$qqrv
	dd	@@Forms@TCustomForm@DestroyWindowHandle$qqrv
	dd	@@Controls@TWinControl@DestroyWnd$qqrv
	dd	@@Controls@TWinControl@GetControlExtents$qqrv
	dd	@@Forms@TCustomForm@PaintWindow$qqrpv
	dd	@@Controls@TWinControl@ShowControl$qqrp17Controls@TControl
	dd	@@Forms@TCustomForm@SetFocus$qqrv
	dd	@@Forms@TScrollingWinControl@AutoScrollEnabled$qqrv
	dd	@@Forms@TScrollingWinControl@AutoScrollInView$qqrp17Controls@TControl
	dd	@@Forms@TCustomForm@Resizing$qqr18Forms@TWindowState
	dd	@@Forms@TCustomForm@DoCreate$qqrv
	dd	@@Forms@TCustomForm@DoDestroy$qqrv
	dd	@@Forms@TCustomForm@UpdateActions$qqrv
	dd	@@Forms@TForm@$bctr$qqrp18Classes@TComponenti
	dd	@@Forms@TCustomForm@CloseQuery$qqrv
	dd	@@Forms@TCustomForm@SetFocusedControl$qqrp20Controls@TWinControl
	dd	@@Forms@TCustomForm@ShowModal$qqrv
	dd	@@Forms@TCustomForm@WantChildKey$qqrp17Controls@TControlr17Messages@TMessage
	dw	5
	dd	@@TForm1@+440
	dd	720
	dw	0
	db	9
	db        71,114,111,117,112,66,111,120,49
	dd	724
	dw	1
	db	6
	db        76,97,98,101,108,49
	dd	728
	dw	2
	db	5
	db        69,100,105,116,49
	dd	732
	dw	3
	db	7
	db        66,117,116,116,111,110,49
	dd	736
	dw	3
	db	7
	db        66,117,116,116,111,110,50
	dw	3
	dw	19
	dd	@@TForm1@Button1Click$qqrp14System@TObject
	db	12
	db        66,117,116,116,111,110,49,67,108,105,99,107
	dw	19
	dd	@@TForm1@Button2Click$qqrp14System@TObject
	db	12
	db        66,117,116,116,111,110,50,67,108,105,99,107
	dw	18
	dd	@@TForm1@FormKeyDown$qqrp14System@TObjectrus46System@%Set$t18Classes@Classes__1$iuc$0$iuc$6%
	db	11
	db        70,111,114,109,75,101,121,68,111,119,110
	db	6
	db        84,70,111,114,109,49
	dw	4
	dd	@Stdctrls@TGroupBox@
	dd	@Stdctrls@TLabel@
	dd	@Stdctrls@TEdit@
	dd	@Stdctrls@TButton@
@TForm1@	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@$xp$6TForm1	segment virtual
	align	2
@@$xp$6TForm1	label	byte
	dd	@@$xp$6TForm1+4
	db	7
	db	6
	db        84,70,111,114,109,49
	dd	@@TForm1@+76
	dd	@$xp$11Forms@TForm
	dw	86
	db	5
	db        77,70,111,114,109
	dw	0
@$xp$6TForm1	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$6TForm1	segment virtual
	align	2
@@$xt$6TForm1	label	byte
	dd	744
	dw	3
	dw	48
	dd	0
	dd	183
	dw	56
	dw	72
	dd	0
	dw	0
	dw	0
	dd	0
	dd	16
	dd	16
	dd	@@TForm1@$bdtr$qqrv
	dw	3
	dw	76
	db	84
	db	70
	db	111
	db	114
	db	109
	db	49
	db	0
	db	0
	dd	@@$xt$11Forms@TForm
	dd	0
	dd	3
	dd	0
	dd	0
	dd	0
@$xt$6TForm1	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$11Forms@TForm	segment virtual
	align	2
@@$xt$11Forms@TForm	label	byte
	dd	720
	dw	3
	dw	48
	dd	0
	dd	183
	dw	64
	dw	80
	dd	0
	dw	0
	dw	0
	dd	0
	dd	15
	dd	15
	dd	@@Forms@TForm@$bdtr$qqrv
	dw	3
	dw	84
	db	70
	db	111
	db	114
	db	109
	db	115
	db	58
	db	58
	db	84
	db	70
	db	111
	db	114
	db	109
	db	0
	db	0
	db	0
	db	0
	dd	@@$xt$17Forms@TCustomForm
	dd	0
	dd	3
	dd	0
	dd	0
	dd	0
@$xt$11Forms@TForm	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@_$ECTPE$@TForm1@$bdtr$qqrv	segment virtual
	align	2
@@_$ECTPE$@TForm1@$bdtr$qqrv	label	dword
	dd	0
	dd	-36
@_$ECTPE$@TForm1@$bdtr$qqrv	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@TForm1@$bdtr$qqrv	segment virtual
	align	2
@@TForm1@$bdtr$qqrv	proc	near
?live16403@0:
@25:
	push      ebp
	mov       ebp,esp
	add       esp,-40
	push      ebx
	call      __BeforeDestruction
	mov       byte ptr [ebp-37],dl
	mov       ebx,eax
	mov       eax,offset @@_$ECTPE$@TForm1@$bdtr$qqrv
	call      @__InitExceptBlockLDTC
	mov       dl,byte ptr [ebp-37]
	test      dl,dl
	jl        short @26
	sub       dword ptr [ebp-8],15
	xor       edx,edx
	mov       eax,ebx
	call      @@Forms@TForm@$bdtr$qqrv
@26:
@27:
	mov       ecx,dword ptr [ebp-36]
	mov       dword ptr fs:[0],ecx
@29:
@28:
	mov       al,byte ptr [ebp-37]
	test      al,al
	jle       short @30
	mov       eax,ebx
	call      __ClassDestroy
@30:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret 
@@TForm1@$bdtr$qqrv	endp
@TForm1@$bdtr$qqrv	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@_$DCQE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	segment virtual
	align	2
@@_$DCQE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	label	dword
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-4
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	8
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-8
	dd	@@$xt$17System@AnsiString
	dd	4
	dd	-12
	dd	0
@_$DCQE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTRE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	segment virtual
	align	2
@@_$ECTRE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	label	dword
	dd	0
	dd	-48
	dw	0
	dw	5
	dd	2
	dd	@@_$DCQE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	dw	8
	dw	5
	dd	4
	dd	@@_$DCQE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2+28
@_$ECTRE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	segment virtual
	align	2
@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	proc	near
?live16404@0:
@31:
	push      ebp
	mov       ebp,esp
	add       esp,-48
	mov       eax,offset @@_$ECTRE$@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2
	push      ebx
	mov       ebx,edx
	mov       dword ptr [ebp-4],ecx
	call      @__InitExceptBlockLDTC
	mov       dword ptr [ebp-20],2
	lea       edx,dword ptr [ebp-4]
	lea       eax,dword ptr [ebp-4]
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
?live16404@16: ; EBX = Register
	lea       edx,dword ptr [ebp+8]
	mov       word ptr [ebp-32],8
	lea       eax,dword ptr [ebp+8]
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
	lea       edx,dword ptr [ebp+8]
	lea       eax,dword ptr [ebp-12]
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
	mov       edx,dword ptr [eax]
	lea       eax,dword ptr [ebp-8]
	push      edx
	lea       edx,dword ptr [ebp-4]
	mov       word ptr [ebp-32],20
	call      @@System@AnsiString@$bctr$qqrrx17System@AnsiString
	inc       dword ptr [ebp-20]
	mov       ecx,dword ptr [eax]
	mov       edx,ebx
	mov       eax,dword ptr [@Classes@TComponent@]
	call      @@Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp-12]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp-8]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp+8]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	dec       dword ptr [ebp-20]
	lea       eax,dword ptr [ebp-4]
	mov       edx,2
	call      @@System@AnsiString@$bdtr$qqrv
	mov       ecx,dword ptr [ebp-48]
	mov       dword ptr fs:[0],ecx
?live16404@32: ; 
@32:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret       4
@@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	endp
@Classes@TComponent@UpdateRegistry$qqrox17System@AnsiStringt2	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@_$DCSE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti	segment virtual
	align	2
@@_$DCSE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti	label	dword
	dd	@@$xt$p11Forms@TForm
	dd	1029
	dd	-4
	dd	0
@_$DCSE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTTE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti	segment virtual
	align	2
@@_$ECTTE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti	label	dword
	dd	0
	dd	-40
	dw	0
	dw	5
	dd	0
	dd	@@_$DCSE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti
@_$ECTTE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@Forms@TForm@$bctr$qqrp18Classes@TComponenti	segment virtual
	align	2
@@Forms@TForm@$bctr$qqrp18Classes@TComponenti	proc	near
?live16405@0:
	push      ebp
	mov       ebp,esp
	add       esp,-44
	push      ebx
	mov       byte ptr [ebp-8],dl
	test      dl,dl
	jle       short @33
	call      __ClassCreate
@33:
@34:
	mov       ebx,ecx
	mov       byte ptr [ebp-41],dl
	mov       dword ptr [ebp-4],eax
	mov       eax,offset @@_$ECTTE$@Forms@TForm@$bctr$qqrp18Classes@TComponenti
	call      @__InitExceptBlockLDTC
?live16405@16: ; EBX = AOwner
	mov       word ptr [ebp-24],8
	mov       edx,dword ptr [ebp+8]
	mov       ecx,ebx
	push      edx
	xor       edx,edx
	mov       eax,dword ptr [ebp-4]
	call      @@Forms@TCustomForm@$bctr$qqrp18Classes@TComponenti
	add       dword ptr [ebp-12],14
	mov       ecx,dword ptr [ebp-40]
	mov       dword ptr fs:[0],ecx
	mov       eax,dword ptr [ebp-4]
@36:
@35:
	mov       dl,byte ptr [ebp-41]
	test      dl,dl
	je        short @37
	call      __AfterConstruction
?live16405@32: ; 
@37:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret       4
@@Forms@TForm@$bctr$qqrp18Classes@TComponenti	endp
@Forms@TForm@$bctr$qqrp18Classes@TComponenti	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
@_$DCUE$@Forms@TForm@$bdtr$qqrv	segment virtual
	align	2
@@_$DCUE$@Forms@TForm@$bdtr$qqrv	label	dword
	dd	@@$xt$p11Forms@TForm
	dd	1029
	dd	-4
	dd	0
@_$DCUE$@Forms@TForm@$bdtr$qqrv	ends
_DATA	ends
_DATA	segment dword public use32 'DATA'
@_$ECTVE$@Forms@TForm@$bdtr$qqrv	segment virtual
	align	2
@@_$ECTVE$@Forms@TForm@$bdtr$qqrv	label	dword
	dd	0
	dd	-40
	dw	0
	dw	5
	dd	0
	dd	@@_$DCUE$@Forms@TForm@$bdtr$qqrv
@_$ECTVE$@Forms@TForm@$bdtr$qqrv	ends
_DATA	ends
_TEXT	segment dword public use32 'CODE'
@Forms@TForm@$bdtr$qqrv	segment virtual
	align	2
@@Forms@TForm@$bdtr$qqrv	proc	near
?live16406@0:
@38:
	push      ebp
	mov       ebp,esp
	add       esp,-40
	push      ebx
	call      __BeforeDestruction
	mov       ebx,edx
	mov       dword ptr [ebp-4],eax
	mov       eax,offset @@_$ECTVE$@Forms@TForm@$bdtr$qqrv
	call      @__InitExceptBlockLDTC
	mov       dword ptr [ebp-12],14
	test      bl,bl
	jl        short @39
	mov       word ptr [ebp-24],8
	sub       dword ptr [ebp-12],14
	xor       edx,edx
	mov       eax,dword ptr [ebp-4]
	call      @@Forms@TCustomForm@$bdtr$qqrv
@39:
	mov       edx,dword ptr [ebp-40]
	mov       dword ptr fs:[0],edx
@40:
	test      bl,bl
	jle       short @41
	mov       eax,dword ptr [ebp-4]
	call      __ClassDestroy
@41:
	pop       ebx
	mov       esp,ebp
	pop       ebp
	ret 
@@Forms@TForm@$bdtr$qqrv	endp
@Forms@TForm@$bdtr$qqrv	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$17Forms@TCustomForm	segment virtual
	align	2
@@$xt$17Forms@TCustomForm	label	byte
	dd	720
	dw	3
	dw	48
	dd	0
	dd	183
	dw	68
	dw	84
	dd	0
	dw	0
	dw	0
	dd	0
	dd	14
	dd	14
	dd	@@Forms@TCustomForm@$bdtr$qqrv
	dw	3
	dw	88
	db	70
	db	111
	db	114
	db	109
	db	115
	db	58
	db	58
	db	84
	db	67
	db	117
	db	115
	db	116
	db	111
	db	109
	db	70
	db	111
	db	114
	db	109
	db	0
	db	0
	dd	@@$xt$26Forms@TScrollingWinControl
	dd	0
	dd	3
	dd	0
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	548
	dd	@@$xt$43System@%DelphiInterface$t15Forms@IDesigner%
	dd	568
	dd	@@$xt$42System@%DelphiInterface$t14Forms@IOleForm%
	dd	588
	dd	0
@$xt$17Forms@TCustomForm	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$42System@%DelphiInterface$t14Forms@IOleForm%	segment virtual
	align	2
@@$xt$42System@%DelphiInterface$t14Forms@IOleForm%	label	byte
	dd	4
	dw	3
	dw	48
	dd	-1
	dd	3
	dw	92
	dw	96
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@System@%DelphiInterface$t14Forms@IOleForm%@$bdtr$qqrv
	dw	3
	dw	100
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	58
	db	58
	db	68
	db	101
	db	108
	db	112
	db	104
	db	105
	db	73
	db	110
	db	116
	db	101
	db	114
	db	102
	db	97
	db	99
	db	101
	db	60
	db	70
	db	111
	db	114
	db	109
	db	115
	db	58
	db	58
	db	73
	db	79
	db	108
	db	101
	db	70
	db	111
	db	114
	db	109
	db	62
	db	0
	db	0
	db	0
	db	0
	dd	0
	dd	0
	dd	0
@$xt$42System@%DelphiInterface$t14Forms@IOleForm%	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$43System@%DelphiInterface$t15Forms@IDesigner%	segment virtual
	align	2
@@$xt$43System@%DelphiInterface$t15Forms@IDesigner%	label	byte
	dd	4
	dw	3
	dw	48
	dd	-1
	dd	3
	dw	92
	dw	96
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@System@%DelphiInterface$t15Forms@IDesigner%@$bdtr$qqrv
	dw	3
	dw	100
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	58
	db	58
	db	68
	db	101
	db	108
	db	112
	db	104
	db	105
	db	73
	db	110
	db	116
	db	101
	db	114
	db	102
	db	97
	db	99
	db	101
	db	60
	db	70
	db	111
	db	114
	db	109
	db	115
	db	58
	db	58
	db	73
	db	68
	db	101
	db	115
	db	105
	db	103
	db	110
	db	101
	db	114
	db	62
	db	0
	db	0
	db	0
	dd	0
	dd	0
	dd	0
@$xt$43System@%DelphiInterface$t15Forms@IDesigner%	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$26Forms@TScrollingWinControl	segment virtual
	align	2
@@$xt$26Forms@TScrollingWinControl	label	byte
	dd	520
	dw	3
	dw	48
	dd	0
	dd	183
	dw	76
	dw	92
	dd	0
	dw	0
	dw	0
	dd	0
	dd	10
	dd	10
	dd	@@Forms@TScrollingWinControl@$bdtr$qqrv
	dw	3
	dw	96
	db	70
	db	111
	db	114
	db	109
	db	115
	db	58
	db	58
	db	84
	db	83
	db	99
	db	114
	db	111
	db	108
	db	108
	db	105
	db	110
	db	103
	db	87
	db	105
	db	110
	db	67
	db	111
	db	110
	db	116
	db	114
	db	111
	db	108
	db	0
	dd	@@$xt$20Controls@TWinControl
	dd	0
	dd	3
	dd	0
	dd	0
	dd	0
@$xt$26Forms@TScrollingWinControl	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@System@%DelphiInterface$t14Forms@IOleForm%@$bdtr$qqrv	segment virtual
	align	2
@@System@%DelphiInterface$t14Forms@IOleForm%@$bdtr$qqrv	proc	near
?live16419@0:
@42:
	push      ebx
	push      esi
	mov       esi,edx
	mov       ebx,eax
	test      ebx,ebx
	je        short @43
?live16419@16: ; EBX = this, ESI = $delflag
	mov       eax,dword ptr [ebx]
	test      eax,eax
	je        short @44
?live16419@32: ; EBX = this, EAX = @temp1, ESI = $delflag
	push      eax
	mov       edx,dword ptr [eax]
	call      dword ptr [edx+8]
?live16419@48: ; EBX = this, ESI = $delflag
@44:
	xor       ecx,ecx
	test      esi,1
	mov       dword ptr [ebx],ecx
	je        short @45
	push      ebx
	call      @$bdele$qpv
	pop       ecx
?live16419@64: ; 
@45:
@43:
@46:
	pop       esi
	pop       ebx
	ret 
@@System@%DelphiInterface$t14Forms@IOleForm%@$bdtr$qqrv	endp
@System@%DelphiInterface$t14Forms@IOleForm%@$bdtr$qqrv	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@System@%DelphiInterface$t15Forms@IDesigner%@$bdtr$qqrv	segment virtual
	align	2
@@System@%DelphiInterface$t15Forms@IDesigner%@$bdtr$qqrv	proc	near
?live16420@0:
@47:
	push      ebx
	push      esi
	mov       esi,edx
	mov       ebx,eax
	test      ebx,ebx
	je        short @48
?live16420@16: ; EBX = this, ESI = $delflag
	mov       eax,dword ptr [ebx]
	test      eax,eax
	je        short @49
?live16420@32: ; EBX = this, EAX = @temp1, ESI = $delflag
	push      eax
	mov       edx,dword ptr [eax]
	call      dword ptr [edx+8]
?live16420@48: ; EBX = this, ESI = $delflag
@49:
	xor       ecx,ecx
	test      esi,1
	mov       dword ptr [ebx],ecx
	je        short @50
	push      ebx
	call      @$bdele$qpv
	pop       ecx
?live16420@64: ; 
@50:
@48:
@51:
	pop       esi
	pop       ebx
	ret 
@@System@%DelphiInterface$t15Forms@IDesigner%@$bdtr$qqrv	endp
@System@%DelphiInterface$t15Forms@IDesigner%@$bdtr$qqrv	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$20Controls@TWinControl	segment virtual
	align	2
@@$xt$20Controls@TWinControl	label	byte
	dd	496
	dw	3
	dw	48
	dd	0
	dd	183
	dw	72
	dw	88
	dd	0
	dw	0
	dw	0
	dd	0
	dd	9
	dd	9
	dd	@@Controls@TWinControl@$bdtr$qqrv
	dw	3
	dw	92
	db	67
	db	111
	db	110
	db	116
	db	114
	db	111
	db	108
	db	115
	db	58
	db	58
	db	84
	db	87
	db	105
	db	110
	db	67
	db	111
	db	110
	db	116
	db	114
	db	111
	db	108
	db	0
	db	0
	db	0
	dd	@@$xt$17Controls@TControl
	dd	0
	dd	3
	dd	0
	dd	0
	dd	@@$xt$49System@%DelphiInterface$t21Controls@IDockManager%
	dd	328
	dd	@@$xt$17System@AnsiString
	dd	344
	dd	0
@$xt$20Controls@TWinControl	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$49System@%DelphiInterface$t21Controls@IDockManager%	segment virtual
	align	2
@@$xt$49System@%DelphiInterface$t21Controls@IDockManager%	label	byte
	dd	4
	dw	3
	dw	48
	dd	-1
	dd	3
	dw	96
	dw	100
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@System@%DelphiInterface$t21Controls@IDockManager%@$bdtr$qqrv
	dw	3
	dw	104
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	58
	db	58
	db	68
	db	101
	db	108
	db	112
	db	104
	db	105
	db	73
	db	110
	db	116
	db	101
	db	114
	db	102
	db	97
	db	99
	db	101
	db	60
	db	67
	db	111
	db	110
	db	116
	db	114
	db	111
	db	108
	db	115
	db	58
	db	58
	db	73
	db	68
	db	111
	db	99
	db	107
	db	77
	db	97
	db	110
	db	97
	db	103
	db	101
	db	114
	db	62
	db	0
	dd	0
	dd	0
	dd	0
@$xt$49System@%DelphiInterface$t21Controls@IDockManager%	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$17Controls@TControl	segment virtual
	align	2
@@$xt$17Controls@TControl	label	byte
	dd	288
	dw	3
	dw	48
	dd	0
	dd	183
	dw	68
	dw	84
	dd	0
	dw	0
	dw	0
	dd	0
	dd	6
	dd	6
	dd	@@Controls@TControl@$bdtr$qqrv
	dw	3
	dw	88
	db	67
	db	111
	db	110
	db	116
	db	114
	db	111
	db	108
	db	115
	db	58
	db	58
	db	84
	db	67
	db	111
	db	110
	db	116
	db	114
	db	111
	db	108
	db	0
	db	0
	dd	@@$xt$18Classes@TComponent
	dd	0
	dd	3
	dd	0
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	112
	dd	0
@$xt$17Controls@TControl	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@System@%DelphiInterface$t21Controls@IDockManager%@$bdtr$qqrv	segment virtual
	align	2
@@System@%DelphiInterface$t21Controls@IDockManager%@$bdtr$qqrv	proc	near
?live16424@0:
@52:
	push      ebx
	push      esi
	mov       esi,edx
	mov       ebx,eax
	test      ebx,ebx
	je        short @53
?live16424@16: ; EBX = this, ESI = $delflag
	mov       eax,dword ptr [ebx]
	test      eax,eax
	je        short @54
?live16424@32: ; EBX = this, EAX = @temp1, ESI = $delflag
	push      eax
	mov       edx,dword ptr [eax]
	call      dword ptr [edx+8]
?live16424@48: ; EBX = this, ESI = $delflag
@54:
	xor       ecx,ecx
	test      esi,1
	mov       dword ptr [ebx],ecx
	je        short @55
	push      ebx
	call      @$bdele$qpv
	pop       ecx
?live16424@64: ; 
@55:
@53:
@56:
	pop       esi
	pop       ebx
	ret 
@@System@%DelphiInterface$t21Controls@IDockManager%@$bdtr$qqrv	endp
@System@%DelphiInterface$t21Controls@IDockManager%@$bdtr$qqrv	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$18Classes@TComponent	segment virtual
	align	2
@@$xt$18Classes@TComponent	label	byte
	dd	36
	dw	3
	dw	48
	dd	0
	dd	183
	dw	68
	dw	84
	dd	0
	dw	0
	dw	0
	dd	0
	dd	4
	dd	4
	dd	@@Classes@TComponent@$bdtr$qqrv
	dw	3
	dw	88
	db	67
	db	108
	db	97
	db	115
	db	115
	db	101
	db	115
	db	58
	db	58
	db	84
	db	67
	db	111
	db	109
	db	112
	db	111
	db	110
	db	101
	db	110
	db	116
	db	0
	dd	@@$xt$19Classes@TPersistent
	dd	0
	dd	3
	dd	0
	dd	0
	dd	@@$xt$17System@AnsiString
	dd	8
	dd	0
@$xt$18Classes@TComponent	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$19Classes@TPersistent	segment virtual
	align	2
@@$xt$19Classes@TPersistent	label	byte
	dd	4
	dw	3
	dw	48
	dd	0
	dd	183
	dw	72
	dw	88
	dd	0
	dw	0
	dw	0
	dd	0
	dd	2
	dd	2
	dd	@@Classes@TPersistent@$bdtr$qqrv
	dw	3
	dw	92
	db	67
	db	108
	db	97
	db	115
	db	115
	db	101
	db	115
	db	58
	db	58
	db	84
	db	80
	db	101
	db	114
	db	115
	db	105
	db	115
	db	116
	db	101
	db	110
	db	116
	db	0
	db	0
	db	0
	db	0
	dd	@@$xt$14System@TObject
	dd	0
	dd	3
	dd	0
	dd	0
	dd	0
@$xt$19Classes@TPersistent	ends
_TEXT	ends
_TEXT	segment dword public use32 'CODE'
@$xt$14System@TObject	segment virtual
	align	2
@@$xt$14System@TObject	label	byte
	dd	4
	dw	3
	dw	48
	dd	0
	dd	179
	dw	64
	dw	68
	dd	0
	dw	0
	dw	0
	dd	0
	dd	1
	dd	1
	dd	@@System@TObject@$bdtr$qqrv
	dw	3
	dw	72
	db	83
	db	121
	db	115
	db	116
	db	101
	db	109
	db	58
	db	58
	db	84
	db	79
	db	98
	db	106
	db	101
	db	99
	db	116
	db	0
	dd	0
	dd	0
	dd	0
@$xt$14System@TObject	ends
_TEXT	ends
_DATA	segment dword public use32 'DATA'
s@	label	byte
	;	s@+0:
	db	"EAX=[%d],EBX=[%d],ECX=[%d],EDX=[%d]",0
	;	s@+36:
	db	"c:\",0
	;	s@+40:
	db	"VolumeName:=[%s],",10
	;	s@+58:
	db	"VolumeSerial:=[%d],",10
	;	s@+78:
	db	"FileSystemName:=[%s],",10
	;	s@+100:
	db	"FileSystemFlag:=[%d],",10
	;	s@+122:
	db	"MaxCompant:=[%d]",0
	;	s@+139:
	db	"NetAddress:=[%x][%x][%x][%x][%x][%x][%x][%x]",0
	;	s@+184:
	db	"ComputerName:=[%s]",10
	;	s@+203:
	db	"AdapterName:=[%s]",10
	;	s@+221:
	db	"Description:=[%s]",10
	;	s@+239:
	db	"Type:=[%d]",0
	;	s@+250:
	db	"w+b",0,0,182,212,178,187,198,240,163,161,178,187,196,220
	db	185,187,177,163,180,230,202,253,190,221,163,161,0
	align	4
_DATA	ends
_TEXT	segment dword public use32 'CODE'
_TEXT	ends
 ?debug  C FB04084054466F726D314004000000
 ?debug  C FB040C40247870243654466F726D3104000000
 ?debug  C FB040D40466F726D734054466F726D4004000000
 ?debug  C FB0414405374646374726C73405447726F7570426F784004000000
 ?debug  C FB0411405374646374726C7340544C6162656C4004000000
 ?debug  C FB0410405374646374726C734054456469744004000000
 ?debug  C FB0412405374646374726C734054427574746F6E4004000000
 ?debug  C FB041240247870243131466F726D734054466F726D04000000
 ?debug  C FB045140436C61737365734054436F6D706F6E656E744055706461746552656769737472792471717270313753797374656D40544D657461436C6173736F78313753797374656D40416E7369537472696E67743302000000
 ?debug  C FB041440436C61737365734054436F6D706F6E656E744004000000
	publicdll	_Form1
 extrn   __Exception_list:dword
 extrn   @__InitExceptBlockLDTC:near
 extrn   __ClassCreate:near
 extrn   __AfterConstruction:near
@_memset equ _memset
 extrn   _memset:near
@_sprintf equ _sprintf
 extrn   _sprintf:near
@@Dialogs@ShowMessage$qqrx17System@AnsiString equ @Dialogs@ShowMessage$qqrx17System@AnsiString
 extrn   @Dialogs@ShowMessage$qqrx17System@AnsiString:near
@@System@AnsiString@$bctr$qqrpxc equ @System@AnsiString@$bctr$qqrpxc
 extrn   @System@AnsiString@$bctr$qqrpxc:near
@@System@AnsiString@$bdtr$qqrv equ @System@AnsiString@$bdtr$qqrv
 extrn   @System@AnsiString@$bdtr$qqrv:near
@GetVolumeInformationA equ GetVolumeInformationA
 extrn   GetVolumeInformationA:near
@GetAdaptersInfo equ GetAdaptersInfo
 extrn   GetAdaptersInfo:near
@_memcpy equ _memcpy
 extrn   _memcpy:near
@GetComputerNameA equ GetComputerNameA
 extrn   GetComputerNameA:near
@_strcat equ _strcat
 extrn   _strcat:near
@_fopen equ _fopen
 extrn   _fopen:near
@@Controls@TControl@GetText$qqrv equ @Controls@TControl@GetText$qqrv
 extrn   @Controls@TControl@GetText$qqrv:near
@@Dialogs@MessageDlg$qqrx17System@AnsiString19Dialogs@TMsgDlgType47System@%Set$t18Dialogs@TMsgDlgBtn$iuc$0$iuc$10%i equ @Dialogs@MessageDlg$qqrx17System@AnsiString19Dialogs@TMsgDlgType47System@%Set$t18Dialogs@TMsgDlgBtn$iuc$0$iuc$10%i
 extrn   @Dialogs@MessageDlg$qqrx17System@AnsiString19Dialogs@TMsgDlgType47System@%Set$t18Dialogs@TMsgDlgBtn$iuc$0$iuc$10%i:near
@_fwrite equ _fwrite
 extrn   _fwrite:near
@_fclose equ _fclose
 extrn   _fclose:near
@@Forms@TCustomForm@Close$qqrv equ @Forms@TCustomForm@Close$qqrv
 extrn   @Forms@TCustomForm@Close$qqrv:near
@@Forms@TCustomForm@$bctr$qqrp18Classes@TComponent equ @Forms@TCustomForm@$bctr$qqrp18Classes@TComponent
 extrn   @Forms@TCustomForm@$bctr$qqrp18Classes@TComponent:near
 extrn   @Forms@TForm@:byte
@@Classes@TComponent@SafeCallException$qqrp14System@TObjectpv equ @Classes@TComponent@SafeCallException$qqrp14System@TObjectpv
 extrn   @Classes@TComponent@SafeCallException$qqrp14System@TObjectpv:near
@@Forms@TCustomForm@AfterConstruction$qqrv equ @Forms@TCustomForm@AfterConstruction$qqrv
 extrn   @Forms@TCustomForm@AfterConstruction$qqrv:near
@@Forms@TCustomForm@BeforeDestruction$qqrv equ @Forms@TCustomForm@BeforeDestruction$qqrv
 extrn   @Forms@TCustomForm@BeforeDestruction$qqrv:near
@@System@TObject@Dispatch$qqrpv equ @System@TObject@Dispatch$qqrpv
 extrn   @System@TObject@Dispatch$qqrpv:near
@@Forms@TCustomForm@DefaultHandler$qqrpv equ @Forms@TCustomForm@DefaultHandler$qqrpv
 extrn   @Forms@TCustomForm@DefaultHandler$qqrpv:near
@@System@TObject@NewInstance$qqrp17System@TMetaClass equ @System@TObject@NewInstance$qqrp17System@TMetaClass
 extrn   @System@TObject@NewInstance$qqrp17System@TMetaClass:near
@@System@TObject@FreeInstance$qqrv equ @System@TObject@FreeInstance$qqrv
 extrn   @System@TObject@FreeInstance$qqrv:near
@@Controls@TWinControl@AssignTo$qqrp19Classes@TPersistent equ @Controls@TWinControl@AssignTo$qqrp19Classes@TPersistent
 extrn   @Controls@TWinControl@AssignTo$qqrp19Classes@TPersistent:near
@@Forms@TCustomForm@DefineProperties$qqrp14Classes@TFiler equ @Forms@TCustomForm@DefineProperties$qqrp14Classes@TFiler
 extrn   @Forms@TCustomForm@DefineProperties$qqrp14Classes@TFiler:near
@@Classes@TPersistent@Assign$qqrp19Classes@TPersistent equ @Classes@TPersistent@Assign$qqrp19Classes@TPersistent
 extrn   @Classes@TPersistent@Assign$qqrp19Classes@TPersistent:near
@@Forms@TCustomForm@Loaded$qqrv equ @Forms@TCustomForm@Loaded$qqrv
 extrn   @Forms@TCustomForm@Loaded$qqrv:near
@@Forms@TCustomForm@Notification$qqrp18Classes@TComponent18Classes@TOperation equ @Forms@TCustomForm@Notification$qqrp18Classes@TComponent18Classes@TOperation
 extrn   @Forms@TCustomForm@Notification$qqrp18Classes@TComponent18Classes@TOperation:near
@@Forms@TCustomForm@ReadState$qqrp15Classes@TReader equ @Forms@TCustomForm@ReadState$qqrp15Classes@TReader
 extrn   @Forms@TCustomForm@ReadState$qqrp15Classes@TReader:near
@@Controls@TControl@SetName$qqrx17System@AnsiString equ @Controls@TControl@SetName$qqrx17System@AnsiString
 extrn   @Controls@TControl@SetName$qqrx17System@AnsiString:near
@@Forms@TCustomForm@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2 equ @Forms@TCustomForm@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2
 extrn   @Forms@TCustomForm@ValidateRename$qqrp18Classes@TComponentx17System@AnsiStringt2:near
@@Classes@TComponent@WriteState$qqrp15Classes@TWriter equ @Classes@TComponent@WriteState$qqrp15Classes@TWriter
 extrn   @Classes@TComponent@WriteState$qqrp15Classes@TWriter:near
@@Forms@TCustomForm@QueryInterface$qqsrx5_GUIDpv equ @Forms@TCustomForm@QueryInterface$qqsrx5_GUIDpv
 extrn   @Forms@TCustomForm@QueryInterface$qqsrx5_GUIDpv:near
@@Controls@TWinControl@CanResize$qqrrit1 equ @Controls@TWinControl@CanResize$qqrrit1
 extrn   @Controls@TWinControl@CanResize$qqrrit1:near
@@Controls@TWinControl@CanAutoSize$qqrrit1 equ @Controls@TWinControl@CanAutoSize$qqrrit1
 extrn   @Controls@TWinControl@CanAutoSize$qqrrit1:near
@@Controls@TWinControl@ConstrainedResize$qqrrit1t1t1 equ @Controls@TWinControl@ConstrainedResize$qqrrit1t1t1
 extrn   @Controls@TWinControl@ConstrainedResize$qqrrit1t1t1:near
@@Controls@TWinControl@GetClientOrigin$qqrv equ @Controls@TWinControl@GetClientOrigin$qqrv
 extrn   @Controls@TWinControl@GetClientOrigin$qqrv:near
@@Forms@TCustomForm@GetClientRect$qqrv equ @Forms@TCustomForm@GetClientRect$qqrv
 extrn   @Forms@TCustomForm@GetClientRect$qqrv:near
@@Controls@TWinControl@GetDeviceContext$qqrrpv equ @Controls@TWinControl@GetDeviceContext$qqrrpv
 extrn   @Controls@TWinControl@GetDeviceContext$qqrrpv:near
@@Controls@TControl@GetDragImages$qqrv equ @Controls@TControl@GetDragImages$qqrv
 extrn   @Controls@TControl@GetDragImages$qqrv:near
@@Controls@TControl@GetEnabled$qqrv equ @Controls@TControl@GetEnabled$qqrv
 extrn   @Controls@TControl@GetEnabled$qqrv:near
@@Forms@TCustomForm@GetFloating$qqrv equ @Forms@TCustomForm@GetFloating$qqrv
 extrn   @Forms@TCustomForm@GetFloating$qqrv:near
@@Controls@TControl@GetFloatingDockSiteClass$qqrv equ @Controls@TControl@GetFloatingDockSiteClass$qqrv
 extrn   @Controls@TControl@GetFloatingDockSiteClass$qqrv:near
@@Controls@TControl@SetDragMode$qqr18Controls@TDragMode equ @Controls@TControl@SetDragMode$qqr18Controls@TDragMode
 extrn   @Controls@TControl@SetDragMode$qqr18Controls@TDragMode:near
@@Controls@TControl@SetEnabled$qqro equ @Controls@TControl@SetEnabled$qqro
 extrn   @Controls@TControl@SetEnabled$qqro:near
@@Forms@TCustomForm@SetParent$qqrp20Controls@TWinControl equ @Forms@TCustomForm@SetParent$qqrp20Controls@TWinControl
 extrn   @Forms@TCustomForm@SetParent$qqrp20Controls@TWinControl:near
@@Forms@TCustomForm@SetParentBiDiMode$qqro equ @Forms@TCustomForm@SetParentBiDiMode$qqro
 extrn   @Forms@TCustomForm@SetParentBiDiMode$qqro:near
@@Controls@TControl@SetBiDiMode$qqr17Classes@TBiDiMode equ @Controls@TControl@SetBiDiMode$qqr17Classes@TBiDiMode
 extrn   @Controls@TControl@SetBiDiMode$qqr17Classes@TBiDiMode:near
@@Forms@TCustomForm@WndProc$qqrr17Messages@TMessage equ @Forms@TCustomForm@WndProc$qqrr17Messages@TMessage
 extrn   @Forms@TCustomForm@WndProc$qqrr17Messages@TMessage:near
@@Controls@TControl@InitiateAction$qqrv equ @Controls@TControl@InitiateAction$qqrv
 extrn   @Controls@TControl@InitiateAction$qqrv:near
@@Controls@TWinControl@Invalidate$qqrv equ @Controls@TWinControl@Invalidate$qqrv
 extrn   @Controls@TWinControl@Invalidate$qqrv:near
@@Controls@TWinControl@Repaint$qqrv equ @Controls@TWinControl@Repaint$qqrv
 extrn   @Controls@TWinControl@Repaint$qqrv:near
@@Controls@TWinControl@SetBounds$qqriiii equ @Controls@TWinControl@SetBounds$qqriiii
 extrn   @Controls@TWinControl@SetBounds$qqriiii:near
@@Controls@TWinControl@Update$qqrv equ @Controls@TWinControl@Update$qqrv
 extrn   @Controls@TWinControl@Update$qqrv:near
@@Forms@TScrollingWinControl@AdjustClientRect$qqrr13Windows@TRect equ @Forms@TScrollingWinControl@AdjustClientRect$qqrr13Windows@TRect
 extrn   @Forms@TScrollingWinControl@AdjustClientRect$qqrr13Windows@TRect:near
@@Forms@TCustomForm@AlignControls$qqrp17Controls@TControlr13Windows@TRect equ @Forms@TCustomForm@AlignControls$qqrp17Controls@TControlr13Windows@TRect
 extrn   @Forms@TCustomForm@AlignControls$qqrp17Controls@TControlr13Windows@TRect:near
@@Controls@TWinControl@CreateHandle$qqrv equ @Controls@TWinControl@CreateHandle$qqrv
 extrn   @Controls@TWinControl@CreateHandle$qqrv:near
@@Forms@TCustomForm@CreateParams$qqrr22Controls@TCreateParams equ @Forms@TCustomForm@CreateParams$qqrr22Controls@TCreateParams
 extrn   @Forms@TCustomForm@CreateParams$qqrr22Controls@TCreateParams:near
@@Forms@TCustomForm@CreateWindowHandle$qqrrx22Controls@TCreateParams equ @Forms@TCustomForm@CreateWindowHandle$qqrrx22Controls@TCreateParams
 extrn   @Forms@TCustomForm@CreateWindowHandle$qqrrx22Controls@TCreateParams:near
@@Forms@TCustomForm@CreateWnd$qqrv equ @Forms@TCustomForm@CreateWnd$qqrv
 extrn   @Forms@TCustomForm@CreateWnd$qqrv:near
@@Forms@TCustomForm@DestroyWindowHandle$qqrv equ @Forms@TCustomForm@DestroyWindowHandle$qqrv
 extrn   @Forms@TCustomForm@DestroyWindowHandle$qqrv:near
@@Controls@TWinControl@DestroyWnd$qqrv equ @Controls@TWinControl@DestroyWnd$qqrv
 extrn   @Controls@TWinControl@DestroyWnd$qqrv:near
@@Controls@TWinControl@GetControlExtents$qqrv equ @Controls@TWinControl@GetControlExtents$qqrv
 extrn   @Controls@TWinControl@GetControlExtents$qqrv:near
@@Forms@TCustomForm@PaintWindow$qqrpv equ @Forms@TCustomForm@PaintWindow$qqrpv
 extrn   @Forms@TCustomForm@PaintWindow$qqrpv:near
@@Controls@TWinControl@ShowControl$qqrp17Controls@TControl equ @Controls@TWinControl@ShowControl$qqrp17Controls@TControl
 extrn   @Controls@TWinControl@ShowControl$qqrp17Controls@TControl:near
@@Forms@TCustomForm@SetFocus$qqrv equ @Forms@TCustomForm@SetFocus$qqrv
 extrn   @Forms@TCustomForm@SetFocus$qqrv:near
@@Forms@TScrollingWinControl@AutoScrollEnabled$qqrv equ @Forms@TScrollingWinControl@AutoScrollEnabled$qqrv
 extrn   @Forms@TScrollingWinControl@AutoScrollEnabled$qqrv:near
@@Forms@TScrollingWinControl@AutoScrollInView$qqrp17Controls@TControl equ @Forms@TScrollingWinControl@AutoScrollInView$qqrp17Controls@TControl
 extrn   @Forms@TScrollingWinControl@AutoScrollInView$qqrp17Controls@TControl:near
@@Forms@TCustomForm@Resizing$qqr18Forms@TWindowState equ @Forms@TCustomForm@Resizing$qqr18Forms@TWindowState
 extrn   @Forms@TCustomForm@Resizing$qqr18Forms@TWindowState:near
@@Forms@TCustomForm@DoCreate$qqrv equ @Forms@TCustomForm@DoCreate$qqrv
 extrn   @Forms@TCustomForm@DoCreate$qqrv:near
@@Forms@TCustomForm@DoDestroy$qqrv equ @Forms@TCustomForm@DoDestroy$qqrv
 extrn   @Forms@TCustomForm@DoDestroy$qqrv:near
@@Forms@TCustomForm@UpdateActions$qqrv equ @Forms@TCustomForm@UpdateActions$qqrv
 extrn   @Forms@TCustomForm@UpdateActions$qqrv:near
@@Forms@TCustomForm@CloseQuery$qqrv equ @Forms@TCustomForm@CloseQuery$qqrv
 extrn   @Forms@TCustomForm@CloseQuery$qqrv:near
@@Forms@TCustomForm@SetFocusedControl$qqrp20Controls@TWinControl equ @Forms@TCustomForm@SetFocusedControl$qqrp20Controls@TWinControl
 extrn   @Forms@TCustomForm@SetFocusedControl$qqrp20Controls@TWinControl:near
@@Forms@TCustomForm@ShowModal$qqrv equ @Forms@TCustomForm@ShowModal$qqrv
 extrn   @Forms@TCustomForm@ShowModal$qqrv:near
@@Forms@TCustomForm@WantChildKey$qqrp17Controls@TControlr17Messages@TMessage equ @Forms@TCustomForm@WantChildKey$qqrp17Controls@TControlr17Messages@TMessage
 extrn   @Forms@TCustomForm@WantChildKey$qqrp17Controls@TControlr17Messages@TMessage:near
 extrn   @Stdctrls@TGroupBox@:byte
 extrn   @Stdctrls@TLabel@:byte
 extrn   @Stdctrls@TEdit@:byte
 extrn   @Stdctrls@TButton@:byte
 extrn   @$xp$11Forms@TForm:byte
 extrn   __BeforeDestruction:near
 extrn   __ClassDestroy:near
@@System@AnsiString@$bctr$qqrrx17System@AnsiString equ @System@AnsiString@$bctr$qqrrx17System@AnsiString
 extrn   @System@AnsiString@$bctr$qqrrx17System@AnsiString:near
@@Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3 equ @Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3
 extrn   @Classes@TComponent@UpdateRegistry$qqrp17System@TMetaClassox17System@AnsiStringt3:near
 extrn   @Classes@TComponent@:byte
@@Forms@TCustomForm@$bctr$qqrp18Classes@TComponenti equ @Forms@TCustomForm@$bctr$qqrp18Classes@TComponenti
 extrn   @Forms@TCustomForm@$bctr$qqrp18Classes@TComponenti:near
@@Forms@TCustomForm@$bdtr$qqrv equ @Forms@TCustomForm@$bdtr$qqrv
 extrn   @Forms@TCustomForm@$bdtr$qqrv:near
@@Forms@TScrollingWinControl@$bdtr$qqrv equ @Forms@TScrollingWinControl@$bdtr$qqrv
 extrn   @Forms@TScrollingWinControl@$bdtr$qqrv:near
 extrn   @$bdele$qpv:near
@@Controls@TWinControl@$bdtr$qqrv equ @Controls@TWinControl@$bdtr$qqrv
 extrn   @Controls@TWinControl@$bdtr$qqrv:near
@@Controls@TControl@$bdtr$qqrv equ @Controls@TControl@$bdtr$qqrv
 extrn   @Controls@TControl@$bdtr$qqrv:near
@@Classes@TComponent@$bdtr$qqrv equ @Classes@TComponent@$bdtr$qqrv
 extrn   @Classes@TComponent@$bdtr$qqrv:near
@@Classes@TPersistent@$bdtr$qqrv equ @Classes@TPersistent@$bdtr$qqrv
 extrn   @Classes@TPersistent@$bdtr$qqrv:near
@@System@TObject@$bdtr$qqrv equ @System@TObject@$bdtr$qqrv
 extrn   @System@TObject@$bdtr$qqrv:near
 ?debug  C FB094D466F726D2E64666D00
 ?debug  C FB0A4D466F726D00
 ?debug  C 9F76636C6535302E6C6962
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\_nfile.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\stdio.h" 10303 10240
	?debug	D "D:\MYDATA\IDPROG\GETID\MForm.h" 11820 26881
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\qos.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winsock2.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\time.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\iptypes.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\ipexport.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\ipifcons.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\ras.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\lmcons.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\mprapi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\iprtrmib.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\iphlpapi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\extctrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\commdlg.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\CommDlg.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\StdCtrls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\shellapi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\ShellAPI.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\regstr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\RegStr.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\wininet.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\WinInet.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\objidl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\oaidl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\oleauto.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\ActiveX.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\UrlMon.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\ShlObj.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\dialogs.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\forms.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\multimon.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\MultiMon.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\Contnrs.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\Menus.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\Imm.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\CommCtrl.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\Commctrl.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\ImgList.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\ActnList.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\controls.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\graphics.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\classes.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\SysConst.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysutils.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\messages.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\dde.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\wincrypt.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\wintrust.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\SysInit.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\windows.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\unknwn.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysvari.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysopen.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\search.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\stdlib.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysdyn.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\systvar.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysset.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\syscomp.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\syscurr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\systdate.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\systobj.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\rpcnsip.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\rpcndr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\rpcnterr.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\rpcnsi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\rpcdcep.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\rpcdce.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\rpc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\wtypes.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\wstring.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysmac.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\dstring.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysclass.H" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\basetyps.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\prsht.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winspool.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\imm.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\mcx.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winsvc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winnetwk.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winreg.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winver.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\wincon.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winnls.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\tvout.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winuser.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\pshpack1.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\wingdi.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winerror.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winbase.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\pshpack8.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\pshpack2.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\poppack.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\pshpack4.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\_loc.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\locale.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\_str.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\string.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\guiddef.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\basetsd.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\mbctype.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\ctype.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\winnt.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\windef.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\stdarg.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\excpt.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\_null.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\_defs.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\_stddef.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\mem.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\windows.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\sysmac.H" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\system.hpp" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\vcl0.h" 10303 10240
	?debug	D "C:\PROGRAM FILES\BORLAND\CBUILDER5\INCLUDE\VCL\VCL.H" 10303 10240
	?debug	D "D:\MYDATA\IDPROG\GETID\MForm.cpp" 11889 24358
	end
