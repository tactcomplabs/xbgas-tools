// See LICENSE for license details.

#include "disasm.h"

const char* xpr_name[] = {
  "x0",   "x1",   "x2",   "x3",   "x4",   "x5",   "x6",   "x7",  
  "x8",   "x9",   "x10",  "x11",  "x12",  "x13",  "x14",  "x15",  
  "x16",  "x17",  "x18",  "x19",  "x20",  "x21",  "x22",  "x23", 
  "x24",  "x25",  "x26",  "x27",  "x28",  "x29",  "x30",  "x31"
};

const char* fpr_name[] = {
  "ft0", "ft1", "ft2",  "ft3",  "ft4", "ft5", "ft6",  "ft7",
  "fs0", "fs1", "fa0",  "fa1",  "fa2", "fa3", "fa4",  "fa5",
  "fa6", "fa7", "fs2",  "fs3",  "fs4", "fs5", "fs6",  "fs7",
  "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"
};


const char* xbr_name[] = {
	  "e0",   "e1",   "e2",   "e3",   "e4",   "e5",   "e6",   "e7",   "e8",
		"e9",   "e10",  "e11",  "e12",  "e13",  "e14",  "e15",  "e16",  "e17",
		"e18",  "e19",  "e20",  "e21",  "e22",  "e23",  "e24",  "e25",  "e26",
	  "e27",  "e28",  "e29",  "e30",  "e31"
};
