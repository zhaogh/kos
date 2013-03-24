/*!
 *	@file	kos_arch.c
 *	@brief	archtecuture part.
 *
 *	Copyright (c) 2013 puchinya All rights reserved.<br>
 *	@b License BSD 2-Clause license
 *	@b Create 2013/03/23
 *	@author	puchinya
 */

#include "kos_arch_if.h"

/*-----------------------------------------------------------------------------
	initialize rutine
-----------------------------------------------------------------------------*/
int main(void)
{
	kos_start_kernel();
	
	return 0;
}

__asm void kos_arch_idle(void)
{
LOOP
	B	LOOP
}

void kos_arch_setup_systick_handler(void)
{
	uint32_t period;
	
	period = SystemCoreClock / 100;	
	SysTick_Config(period);
	
	NVIC_SetPriority(PendSV_IRQn, 255);
	NVIC_SetPriority(SysTick_IRQn, 254);
}

/*-----------------------------------------------------------------------------
	CPU lock API's
-----------------------------------------------------------------------------*/
kos_er_t kos_loc_cpu(void)
{
	__disable_irq();
	return KOS_E_OK;
}

kos_er_t kos_iloc_cpu(void)
{
	__disable_irq();
	return KOS_E_OK;
}

kos_er_t kos_unl_cpu(void)
{
	__enable_irq();
	return KOS_E_OK;
}

kos_er_t kos_iunl_cpu(void)
{
	__enable_irq();
	return KOS_E_OK;
}

__asm kos_bool_t kos_sns_loc(void)
{
	MRS	R0, PRIMASK
	AND R0, #1
	BX	LR
}

/*-----------------------------------------------------------------------------
	archtecuture inplement API
-----------------------------------------------------------------------------*/
kos_er_t kos_dis_int(kos_intno_t intno)
{
#ifdef KOS_CFG_ENA_PAR_CHK
	if(intno > KOS_MAX_INTNO) {
		return KOS_E_PAR;
	}
#endif
	
	NVIC_DisableIRQ((IRQn_Type)intno);
	
	return KOS_E_OK;
}

kos_er_t kos_ena_int(kos_intno_t intno)
{
#ifdef KOS_CFG_ENA_PAR_CHK
	if(intno > KOS_MAX_INTNO) {
		return KOS_E_PAR;
	}
#endif
	
	NVIC_EnableIRQ((IRQn_Type)intno);
	
	return KOS_E_OK;
}

/*-----------------------------------------------------------------------------
	interrupt rutines
-----------------------------------------------------------------------------*/
void kos_arch_inthdr(void)
{
	kos_intno_t intno = (__get_IPSR() & 0x1FF) - 0x10;
	kos_hal_if_exe_inth(intno);
}

void SysTick_Handler(void)
{
	kos_isig_tim();
}

/* do schedule */
__asm void PendSV_Handler(void)
{
	extern kos_schedule_impl_nolock
	
	CPSID		i
	
	/* store R4-R11 to PSP */
	MRS			R0, PSP
	STMDB		R0!, {R4-R11}
	MSR			PSP, R0
	
	MOV			R11, LR
	BL			kos_schedule_impl_nolock
	MOV			LR, R11
	
	/* load R4-R11 from PSP */
	MRS			R0, PSP
	LDMIA		R0!, {R4-R11}
	MSR			PSP, R0
	
	CPSIE		i
	
	BX			LR
}

void NMI_Handler(void)
{
	__NOP();
}

void HardFault_Handler(void)
{
	__NOP();
}

void MemManage_Handler(void)
{
	__NOP();
}

void BusFault_Handler(void)
{
	__NOP();
}

void UsageFault_Handler(void)
{
	__NOP();
}

void USB0_Handler(void) {
	kos_arch_inthdr();
}

void USB0F_Handler(void) {
	kos_arch_inthdr();
}

void USB1_Handler(void) {
	kos_arch_inthdr();
}

void USB1F_Handler(void) {
	kos_arch_inthdr();
}
