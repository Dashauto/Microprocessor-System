#include "init.h"

uint32_t var;
float f_var;

uint32_t load_and_add_1();
uint32_t load_and_add_2();
uint32_t mult_32_1();
float mult_fl_2();
uint32_t equation_var_int();
float equation_var_flo();
uint32_t equation_MAC_int();
float equation_MAC_flo();


int main()
{
	Sys_Init();
	//set 1
	printf("\033[0;0H\033[2JSet1\n\r");
    printf("load and add result (30 + 34): %u\r\n", load_and_add_1());
    printf("multiply of 32 result (8 * 11): %u\r\n", mult_32_1());
    printf("equation result (x = 10): %u\r\n", equation_var_int());
    printf("equation result with MAC (x = 10): %u\r\n", equation_MAC_int());


    //set 2
    printf("\r\nSet2\n\r");
    printf("load and add result (10 + 25): %u\r\n", load_and_add_2());
    printf("multiply of float result (7.2 * 3.7): %f\r\n", mult_fl_2());
    printf("equation result (x = 22.7): %f\r\n", equation_var_flo());
    printf("equation result with MAC (x = 22.7): %f\r\n", equation_MAC_flo());

}

uint32_t load_and_add_1()
{
    asm("LDR r0, = 30");
    asm("LDR r1, = 34");
    asm("ADD r2, r0, r1");
    asm("STR r2,%0" : "=m" (var));
    return var;
}

uint32_t mult_32_1()
{
    uint32_t variable_0 = 8;
    uint32_t variable_1 = 11;
    asm("MUL %0,%1,%2" : "=r" (var): "r" (variable_0), "r" (variable_1));
    return var;
}

uint32_t equation_var_int()
{
	uint32_t x = 10;
	asm("LDR r2, =2");
	asm("MUL r0, %0, r2" :: "r" (x));
	asm("LDR r3, =3");
	asm("UDIV r1, r0, r3");
	asm("ADD %0,r1, #5":"=r" (var));
	return var;
}

uint32_t equation_MAC_int()
{
	uint32_t x = 10;
	asm("LDR r2, =2");
	asm("LDR r1, =15");
	// Lab4 document and Programming_Manual P.112
    asm("MLA r0,%0,r2,r1" :: "r" (x));
	asm("LDR r3, =3");
	asm("UDIV %0, r0, r3": "=r" (var));
	return var;
}


uint32_t load_and_add_2()
{
    asm("LDR r0, = 25");
    asm("LDR r1, = 10");
    asm("ADD r2, r0, r1");
    asm("STR r2,%0" : "=m" (var));
    return var;
}

float mult_fl_2()
{
    float variable_0 = 7.2;
	float variable_1 = 3.7;
    asm volatile("VMUL.F32 %0,%1,%2" :"=t" (f_var): "t" (variable_0), "t" (variable_1));
    return f_var;
}

float equation_var_flo()
{
	float x = 22.7;
	//Programming_Manual P.162
    asm("VMOV s2, #2.0");
    //Programming_Manual P.166
	asm("VMUL.F32 s0, %0, s2" :: "t" (x));
	asm("VMOV s3, #3.0");
    //Programming_Manual P.157
	asm("VDIV.F32 s1, s0, s3");
	asm("VMOV s5, #5.0");
    //Programming_Manual P.153
	asm("VADD.F32 %0, s1, s5": "=t" (f_var));
	return f_var;
}

float equation_MAC_flo()
{
	float x = 22.7;
	asm volatile("VMOV s0, #2.0");
	asm volatile("VMOV s1, #3.0");
	asm volatile("VDIV.F32 s2, s0, s1");
	asm volatile("VMOV s3, #5.0");
	//Programming_Manual P.161
    asm volatile("VMLA.F32 s3,%0,s2" :: "w" (x));
	asm volatile("VSTR.F32 s3,%0" : "=m" (f_var));
	return f_var;
}
