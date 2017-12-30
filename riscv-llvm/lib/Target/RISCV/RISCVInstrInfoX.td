//===- RISCVInstrInfoB.td - OpenHPC RISCV instructions -*- tblgen-*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//xBGAS Integer Load Instructions[I-type]
def ELQ  : InstX<"elq"  ,  0b1111111, 0b000, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ELE  : InstX<"ele"  ,  0b1111111, 0b001, ER64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ELD  : InstX<"eld"  ,  0b0111111, 0b011, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ELW  : InstX<"elw"  ,  0b0111111, 0b010, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ELH  : InstX<"elh"  ,  0b0111111, 0b001, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ELHU : InstX<"elhu" ,  0b0111111, 0b101, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ELB  : InstX<"elb"  ,  0b0111111, 0b000, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ELBU : InstX<"elbu" ,  0b0111111, 0b100, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;

//xBGAS Integer Store Instructions[S-type]
def ESQ  : InstXS<"esq"  ,  0b1111111, 0b100, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ESE  : InstXS<"ese"  ,  0b1111111, 0b101, ER64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ESD  : InstXS<"esd"  ,  0b0111110, 0b011, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ESW  : InstXS<"esw"  ,  0b0111110, 0b010, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ESH  : InstXS<"esh"  ,  0b0111110, 0b001, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def ESB  : InstXS<"esb"  ,  0b0111110, 0b000, GR64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;


//Raw Integer Load Instructions[R-type]
def ERLD  :  InstXR<"erld"   , 0b0111111, 0b0000010, 0b011, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERLW  :  InstXR<"erlw"   , 0b0111111, 0b0000010, 0b010, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERLH  :  InstXR<"erlh"   , 0b0111111, 0b0000010, 0b001, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERLHU :  InstXR<"erlhu"  , 0b0111111, 0b0000010, 0b101, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERLB  :  InstXR<"erlb"   , 0b0111111, 0b0000010, 0b000, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERLBU :  InstXR<"erlbu"  , 0b0111111, 0b0000010, 0b100, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERLE  :  InstXR<"erle"   , 0b0111111, 000000011, 0b100, ER64, GR64, ER64>, Requires<[IsRV64, HasX]>;


//Raw Integer Store Instructions[R-type]
def ERSD  :  InstXR<"ersd"   , 0b0111111, 0b0000100, 0b011, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERSW  :  InstXR<"ersw"   , 0b0111111, 0b0000100, 0b010, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERSH  :  InstXR<"ersh"   , 0b0111111, 0b0000100, 0b001, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERSB  :  InstXR<"ersb"   , 0b0111111, 0b0000100, 0b000, GR64, GR64, ER64>, Requires<[IsRV64, HasX]>;
def ERSE  :  InstXR<"erse"   , 0b0111111, 0b0001000, 0b011, ER64, GR64, ER64>, Requires<[IsRV64, HasX]>;

//Address Management Instruction[I-type]
def EADDI  : InstX<"eaddi"  ,  0b1111111, 0b010, GR64, ER64, imm64sx12>, Requires<[IsRV64, HasX]>;
def EADDIE : InstX<"eaddie" ,  0b1111111, 0b011, ER64, GR64, imm64sx12>, Requires<[IsRV64, HasX]>;
def EADDIX : InstX<"eaddix" ,  0b1111111, 0b110, ER64, ER64, imm64sx12>, Requires<[IsRV64, HasX]>;






