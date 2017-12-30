	.file	"hello.c"
	.option nopic
	.text
	.align	1
	.globl	foo
	.type	foo, @function
foo:
        #-- loads
        eld     a5,40(sp)
        elw     a5,40(sp)
        elh     a5,40(sp)
        elb     a5,40(sp)
        elhu    a5,40(sp)
        elbu    a5,40(sp)
        #-- stores
        esd     a5,40(sp)
        esw     a5,40(sp)
        esh     a5,40(sp)
        esb     a5,40(sp)
        #-- extended quad loads
        elq     a5,40(sp)
        ele     e10,40(sp)
        #-- extended quad stores
        esq     a5,40(sp)
        ese     e10,40(sp)
        #-- raw load/stores
        erld    a5,a6,e10
        erlw    a5,a6,e10
        erlh    a5,a6,e10
        erlb    a5,a6,e10
        erlhu   a5,a6,e10
        erlbu   a5,a6,e10
        ersd    a5,a5,e10
        ersw    a5,a5,e10
        ersh    a5,a5,e10
        ersb    a5,a5,e10
        erse    e10,a5,e11
        eaddi   a5,40,e10
        eaddie  e10,40,a5
        eaddix  e10,40,e11
	.size	foo, .-foo
	.ident	"GCC: (GNU) 7.1.1 20170509"
