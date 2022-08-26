#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>  //���荞�݂��g�p���邽��
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "drvOutLed_inc.h"
#include "drvOutLed.h"

//LINK���Z�b�g�����
static DRV_OUT_LED		preDrvOutLed;

//********************************************************************************
// ������
//********************************************************************************
void initDrvOutLed( void )
{
	// AVR128DA28/32/48/64
	// 23.3.2 ������

	// 1. ����A(TCDn.CTRLA)ڼ޽��Ÿۯ����ƑO�u�������I��ł��������B
	TCD0.CTRLA	= TCD_CLKSEL_OSCHF_gc | TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV8_gc;
	// 2. ����B(TCDn.CTRLB)ڼ޽��Ŕg�`���������I��ł��������B
	TCD0.CTRLB	= TCD_WGMODE_ONERAMP_gc;
	// 3. �C�ӑI��: ���̐ÓIڼ޽���]�ދ@�\�ɍ\���ݒ肵�Ă��������B
	TCD0.CTRLC	= TCD_CMPCSEL_PWMA_gc | TCD_CMPDSEL_PWMB_gc;
	_PROTECTED_WRITE( TCD0.FAULTCTRL , 0x30 );		// �G���b�^�Ή�
	// 4. ��r(TCDn.CMPxSET/CLR)ڼ޽��ɏ����l�������Ă��������B
	TCD0.CMPASET	= TCD0.CMPACLR	= PWM_TOP;		// TOP��WOB��OFF�J�E���g��CMPBCLR�ŋ��ʂ̂��߁ATOP�l���N�_�ɂ���
	TCD0.CMPBSET	= TCD0.CMPBCLR	= PWM_TOP;
	// 5. �C�ӑI��: ����2�d�ɏՂ��ꂽڼ޽��ɖ]�ޒl�������Ă��������B
	// �����Ȃ�
	// 6. ���(TCDn.STATUS)ڼ޽��̋�������(ENRDY)�ޯĂ��f1�f�ɐݒ肳��Ă���̂��m���ɂ��Ă��������B
	while( !(TCD0.STATUS & TCD_ENRDY_bm) ){
		;
	}
	// 7. ����A(TCDn.CTRLA)ڼ޽��̋���(ENABLE)�ޯĂɁf1�f���������Ƃɂ����TCD�������Ă��������B
	TCD0.CTRLA	|= TCD_ENABLE_bm;

	TCD0.CTRLE	|= TCD_SYNC_bm;		// �l���Z�b�g�����̂Ń_�u���o�b�t�@�X�V�w���BCMPxSET�A
}

//********************************************************************************
// Lnk����Z�b�g
//********************************************************************************
void setDrvOutLed( DRV_OUT_LED *inP )
{
	if( inP->green != preDrvOutLed.green ){
		TCD0.CMPASET	= PWM_TOP - (((uint32_t)inP->green * PWM_TOP) / 100);
	}

	if( inP->blue != preDrvOutLed.blue ){
		TCD0.CMPBSET	= PWM_TOP - (((uint32_t)inP->blue * PWM_TOP) / 100);
	}

	TCD0.CTRLE	|= TCD_SYNC_bm;		// �l���Z�b�g�����̂Ń_�u���o�b�t�@�X�V�w���BCMPxSET
	preDrvOutLed = *inP;
}

//********************************************************************************
// ���C������
//********************************************************************************
void drvOutLedMain( void )
{
	
}

