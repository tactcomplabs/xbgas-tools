# Instructions that are valid
#
# RUN: llvm-mc %s -triple=riscv-unknown-linux -show-encoding -mcpu=xBGAS | FileCheck --check-prefix=CHECK64 %s

# CHECK64:	eld	x10, x11, 128           # encoding: [0x3f,0xb5,0x05,0x08]
# CHECK64:	elw	x10, x11, 128           # encoding: [0x3f,0xa5,0x05,0x08]
# CHECK64:	elh	x10, x11, 128           # encoding: [0x3f,0x95,0x05,0x08]
# CHECK64:	elhu	x10, x11, 128           # encoding: [0x3f,0xd5,0x05,0x08]
# CHECK64:	elb	x10, x11, 128           # encoding: [0x3f,0x85,0x05,0x08]
# CHECK64:	elbu	x10, x11, 128           # encoding: [0x3f,0xc5,0x05,0x08]
# CHECK64:	esd	x10, x11, 128           # encoding: [0x3e,0xb0,0xa5,0x08]
# CHECK64:	esw	x10, x11, 128           # encoding: [0x3e,0xa0,0xa5,0x08]
# CHECK64:	esh	x10, x11, 128           # encoding: [0x3e,0x90,0xa5,0x08]
# CHECK64:	esb	x10, x11, 128           # encoding: [0x3e,0x80,0xa5,0x08]
# CHECK64:	elq	x10, x11, 128           # encoding: [0x7f,0x85,0x05,0x08]
# CHECK64:	ele	e30, x11, 128           # encoding: [0x7f,0x9f,0x05,0x08]
# CHECK64:	esq	x10, x11, 128           # encoding: [0x7f,0xc0,0xa5,0x08]
# CHECK64:	ese	e11, x11, 128           # encoding: [0x7f,0xd0,0xb5,0x08]

# CHECK64:	erld	x10, x11, e11           # encoding: [0x3f,0xb5,0xb5,0x04]
# CHECK64:	erlw	x10, x11, e12           # encoding: [0x3f,0xa5,0xc5,0x04]
# CHECK64:	erlh	x10, x11, e13           # encoding: [0x3f,0x95,0xd5,0x04]
# CHECK64:	erlhu	x10, x11, e14           # encoding: [0x3f,0xd5,0xe5,0x04]
# CHECK64:	erlb	x10, x11, e15           # encoding: [0x3f,0x85,0xf5,0x04]
# CHECK64:	erlbu	x10, x11, e16           # encoding: [0x3f,0xc5,0x05,0x05]
# CHECK64:	erle	e30, x11, e17           # encoding: [0x3f,0xcf,0x15,0x17]
# CHECK64:	ersd	x10, x11, e18           # encoding: [0x3f,0xb5,0x25,0x09]
# CHECK64:	ersw	x10, x11, e19           # encoding: [0x3f,0xa5,0x35,0x09]
# CHECK64:	ersh	x10, x11, e20           # encoding: [0x3f,0x95,0x45,0x09]
# CHECK64:	ersb	x10, x11, e21           # encoding: [0x3f,0x85,0x55,0x09]
# CHECK64:	erse	e30, x11, e22           # encoding: [0x3f,0xbf,0x65,0x11]

# CHECK64:	eaddi	x10, e30, 128           # encoding: [0x7f,0x25,0x0f,0x08]
# CHECK64:	eaddie	e23, x11, 128           # encoding: [0xff,0xbb,0x05,0x08]
# CHECK64:	eaddix	e24, e25, 128           # encoding: [0x7f,0xec,0x0c,0x08]

#-- xBGAS Integer Load/Store Inst	
eld 	x10, x11, 128
elw	x10, x11, 128
elh	x10, x11, 128
elhu	x10, x11, 128
elb	x10, x11, 128
elbu	x10, x11, 128
esd	x10, x11, 128
esw	x10, x11, 128
esh	x10, x11, 128
esb	x10, x11, 128
elq	x10, x11, 128
ele	e30, x11, 128
esq	x10, x11, 128 
ese	e11, x11, 128

#-- xBGAS Raw Integer Load/Store Inst
erld	x10, x11, e11
erlw	x10, x11, e12
erlh	x10, x11, e13
erlhu	x10, x11, e14
erlb	x10, x11, e15
erlbu	x10, x11, e16
erle	e30, x11, e17
ersd	x10, x11, e18
ersw	x10, x11, e19
ersh	x10, x11, e20
ersb	x10, x11, e21
erse	e30, x11, e22

#-- xBGAS Address Management Inst
eaddi 	x10, e30, 128
eaddie	e23, x11, 128
eaddix	e24, e25, 128


#-- EOF
