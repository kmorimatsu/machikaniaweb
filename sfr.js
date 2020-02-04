/****************************************
*  PIC32MX emulator written by Katsumi  *
*        This script is released        *
*          under the LGPL v2.1.         *
****************************************/

SFR=new Object();
SFR.WDTCON=function(num){
	if (num!=undefined) SFR.WDTCON.value=(SFR.WDTCON.value & 0xffff7ffc) + (num & 0x8003);
	return SFR.WDTCON.value & 0x807f;
};
SFR.WDTCONCLR=function(num){
	var val=SFR.WDTCON.value;
	SFR.WDTCON(val&(0xFFFFFFFF-num));
};
SFR.WDTCONSET=function(num){
	var val=SFR.WDTCON.value;
	SFR.WDTCON(val|num);
};
SFR.WDTCONINV=function(num){
	var val=SFR.WDTCON.value;
	SFR.WDTCON(val^num);
};
SFR[0xBF800000]=SFR.WDTCON;
SFR[0xBF800004]=SFR.WDTCONCLR;
SFR[0xBF800008]=SFR.WDTCONSET;
SFR[0xBF80000C]=SFR.WDTCONINV;
SFR.WDTCON(0x0);
SFR.RTCCON=function(num){
	if (num!=undefined) SFR.RTCCON.value=(SFR.RTCCON.value & 0xfc001f36) + (num & 0x3ffe0c9);
	return SFR.RTCCON.value & 0x3ffe0cf;
};
SFR.RTCCONCLR=function(num){
	var val=SFR.RTCCON.value;
	SFR.RTCCON(val&(0xFFFFFFFF-num));
};
SFR.RTCCONSET=function(num){
	var val=SFR.RTCCON.value;
	SFR.RTCCON(val|num);
};
SFR.RTCCONINV=function(num){
	var val=SFR.RTCCON.value;
	SFR.RTCCON(val^num);
};
SFR[0xBF800200]=SFR.RTCCON;
SFR[0xBF800204]=SFR.RTCCONCLR;
SFR[0xBF800208]=SFR.RTCCONSET;
SFR[0xBF80020C]=SFR.RTCCONINV;
SFR.RTCCON(0x0);
SFR.RTCALRM=function(num){
	if (num!=undefined) SFR.RTCALRM.value=(SFR.RTCALRM.value & 0xffff0000) + (num & 0xffff);
	return SFR.RTCALRM.value & 0xffff;
};
SFR.RTCALRMCLR=function(num){
	var val=SFR.RTCALRM.value;
	SFR.RTCALRM(val&(0xFFFFFFFF-num));
};
SFR.RTCALRMSET=function(num){
	var val=SFR.RTCALRM.value;
	SFR.RTCALRM(val|num);
};
SFR.RTCALRMINV=function(num){
	var val=SFR.RTCALRM.value;
	SFR.RTCALRM(val^num);
};
SFR[0xBF800210]=SFR.RTCALRM;
SFR[0xBF800214]=SFR.RTCALRMCLR;
SFR[0xBF800218]=SFR.RTCALRMSET;
SFR[0xBF80021C]=SFR.RTCALRMINV;
SFR.RTCALRM(0x0);
SFR.RTCTIME=function(num){
	if (num!=undefined) SFR.RTCTIME.value=(SFR.RTCTIME.value & 0xe00080ff) + (num & 0x1fff7f00);
	return SFR.RTCTIME.value & 0xffffff00;
};
SFR.RTCTIMECLR=function(num){
	var val=SFR.RTCTIME.value;
	SFR.RTCTIME(val&(0xFFFFFFFF-num));
};
SFR.RTCTIMESET=function(num){
	var val=SFR.RTCTIME.value;
	SFR.RTCTIME(val|num);
};
SFR.RTCTIMEINV=function(num){
	var val=SFR.RTCTIME.value;
	SFR.RTCTIME(val^num);
};
SFR[0xBF800220]=SFR.RTCTIME;
SFR[0xBF800224]=SFR.RTCTIMECLR;
SFR[0xBF800228]=SFR.RTCTIMESET;
SFR[0xBF80022C]=SFR.RTCTIMEINV;
SFR.RTCTIME(0x0);
SFR.RTCDATE=function(num){
	if (num!=undefined) SFR.RTCDATE.value=(SFR.RTCDATE.value & 0xe0c0f8) + (num & 0xff1f3f07);
	return SFR.RTCDATE.value & 0xffffff0f;
};
SFR.RTCDATECLR=function(num){
	var val=SFR.RTCDATE.value;
	SFR.RTCDATE(val&(0xFFFFFFFF-num));
};
SFR.RTCDATESET=function(num){
	var val=SFR.RTCDATE.value;
	SFR.RTCDATE(val|num);
};
SFR.RTCDATEINV=function(num){
	var val=SFR.RTCDATE.value;
	SFR.RTCDATE(val^num);
};
SFR[0xBF800230]=SFR.RTCDATE;
SFR[0xBF800234]=SFR.RTCDATECLR;
SFR[0xBF800238]=SFR.RTCDATESET;
SFR[0xBF80023C]=SFR.RTCDATEINV;
SFR.RTCDATE(0x0);
SFR.ALRMTIME=function(num){
	if (num!=undefined) SFR.ALRMTIME.value=(SFR.ALRMTIME.value & 0xe08080ff) + (num & 0x1f7f7f00);
	return SFR.ALRMTIME.value & 0xffffff00;
};
SFR.ALRMTIMECLR=function(num){
	var val=SFR.ALRMTIME.value;
	SFR.ALRMTIME(val&(0xFFFFFFFF-num));
};
SFR.ALRMTIMESET=function(num){
	var val=SFR.ALRMTIME.value;
	SFR.ALRMTIME(val|num);
};
SFR.ALRMTIMEINV=function(num){
	var val=SFR.ALRMTIME.value;
	SFR.ALRMTIME(val^num);
};
SFR[0xBF800240]=SFR.ALRMTIME;
SFR[0xBF800244]=SFR.ALRMTIMECLR;
SFR[0xBF800248]=SFR.ALRMTIMESET;
SFR[0xBF80024C]=SFR.ALRMTIMEINV;
SFR.ALRMTIME(0x0);
SFR.ALRMDATE=function(num){
	if (num!=undefined) SFR.ALRMDATE.value=(SFR.ALRMDATE.value & 0xffe0c0f8) + (num & 0x1f3f07);
	return SFR.ALRMDATE.value & 0xffff0f;
};
SFR.ALRMDATECLR=function(num){
	var val=SFR.ALRMDATE.value;
	SFR.ALRMDATE(val&(0xFFFFFFFF-num));
};
SFR.ALRMDATESET=function(num){
	var val=SFR.ALRMDATE.value;
	SFR.ALRMDATE(val|num);
};
SFR.ALRMDATEINV=function(num){
	var val=SFR.ALRMDATE.value;
	SFR.ALRMDATE(val^num);
};
SFR[0xBF800250]=SFR.ALRMDATE;
SFR[0xBF800254]=SFR.ALRMDATECLR;
SFR[0xBF800258]=SFR.ALRMDATESET;
SFR[0xBF80025C]=SFR.ALRMDATEINV;
SFR.ALRMDATE(0x0);
SFR.T1CON=function(num){
	if (num!=undefined) SFR.T1CON.value=(SFR.T1CON.value & 0xffff4749) + (num & 0xb8b6);
	return SFR.T1CON.value & 0xb8b6;
};
SFR.T1CONCLR=function(num){
	var val=SFR.T1CON.value;
	SFR.T1CON(val&(0xFFFFFFFF-num));
};
SFR.T1CONSET=function(num){
	var val=SFR.T1CON.value;
	SFR.T1CON(val|num);
};
SFR.T1CONINV=function(num){
	var val=SFR.T1CON.value;
	SFR.T1CON(val^num);
};
SFR[0xBF800600]=SFR.T1CON;
SFR[0xBF800604]=SFR.T1CONCLR;
SFR[0xBF800608]=SFR.T1CONSET;
SFR[0xBF80060C]=SFR.T1CONINV;
SFR.T1CON(0x0);
SFR.TMR1=function(num){
	if (num!=undefined) SFR.TMR1.value=(SFR.TMR1.value & 0xffff0000) + (num & 0xffff);
	return SFR.TMR1.value & 0xffff;
};
SFR.TMR1CLR=function(num){
	var val=SFR.TMR1.value;
	SFR.TMR1(val&(0xFFFFFFFF-num));
};
SFR.TMR1SET=function(num){
	var val=SFR.TMR1.value;
	SFR.TMR1(val|num);
};
SFR.TMR1INV=function(num){
	var val=SFR.TMR1.value;
	SFR.TMR1(val^num);
};
SFR[0xBF800610]=SFR.TMR1;
SFR[0xBF800614]=SFR.TMR1CLR;
SFR[0xBF800618]=SFR.TMR1SET;
SFR[0xBF80061C]=SFR.TMR1INV;
SFR.TMR1(0x0);
SFR.PR1=function(num){
	if (num!=undefined) SFR.PR1.value=(SFR.PR1.value & 0xffff0000) + (num & 0xffff);
	return SFR.PR1.value & 0xffff;
};
SFR.PR1CLR=function(num){
	var val=SFR.PR1.value;
	SFR.PR1(val&(0xFFFFFFFF-num));
};
SFR.PR1SET=function(num){
	var val=SFR.PR1.value;
	SFR.PR1(val|num);
};
SFR.PR1INV=function(num){
	var val=SFR.PR1.value;
	SFR.PR1(val^num);
};
SFR[0xBF800620]=SFR.PR1;
SFR[0xBF800624]=SFR.PR1CLR;
SFR[0xBF800628]=SFR.PR1SET;
SFR[0xBF80062C]=SFR.PR1INV;
SFR.PR1(0xffff);
SFR.T2CON=function(num){
	if (num!=undefined) SFR.T2CON.value=(SFR.T2CON.value & 0xffff5f05) + (num & 0xa0fa);
	return SFR.T2CON.value & 0xa0fa;
};
SFR.T2CONCLR=function(num){
	var val=SFR.T2CON.value;
	SFR.T2CON(val&(0xFFFFFFFF-num));
};
SFR.T2CONSET=function(num){
	var val=SFR.T2CON.value;
	SFR.T2CON(val|num);
};
SFR.T2CONINV=function(num){
	var val=SFR.T2CON.value;
	SFR.T2CON(val^num);
};
SFR[0xBF800800]=SFR.T2CON;
SFR[0xBF800804]=SFR.T2CONCLR;
SFR[0xBF800808]=SFR.T2CONSET;
SFR[0xBF80080C]=SFR.T2CONINV;
SFR.T2CON(0x0);
SFR.TMR2=function(num){
	if (num!=undefined) SFR.TMR2.value=(SFR.TMR2.value & 0xffff0000) + (num & 0xffff);
	return SFR.TMR2.value & 0xffff;
};
SFR.TMR2CLR=function(num){
	var val=SFR.TMR2.value;
	SFR.TMR2(val&(0xFFFFFFFF-num));
};
SFR.TMR2SET=function(num){
	var val=SFR.TMR2.value;
	SFR.TMR2(val|num);
};
SFR.TMR2INV=function(num){
	var val=SFR.TMR2.value;
	SFR.TMR2(val^num);
};
SFR[0xBF800810]=SFR.TMR2;
SFR[0xBF800814]=SFR.TMR2CLR;
SFR[0xBF800818]=SFR.TMR2SET;
SFR[0xBF80081C]=SFR.TMR2INV;
SFR.TMR2(0x0);
SFR.PR2=function(num){
	if (num!=undefined) SFR.PR2.value=(SFR.PR2.value & 0xffff0000) + (num & 0xffff);
	return SFR.PR2.value & 0xffff;
};
SFR.PR2CLR=function(num){
	var val=SFR.PR2.value;
	SFR.PR2(val&(0xFFFFFFFF-num));
};
SFR.PR2SET=function(num){
	var val=SFR.PR2.value;
	SFR.PR2(val|num);
};
SFR.PR2INV=function(num){
	var val=SFR.PR2.value;
	SFR.PR2(val^num);
};
SFR[0xBF800820]=SFR.PR2;
SFR[0xBF800824]=SFR.PR2CLR;
SFR[0xBF800828]=SFR.PR2SET;
SFR[0xBF80082C]=SFR.PR2INV;
SFR.PR2(0xffff);
SFR.T3CON=function(num){
	if (num!=undefined) SFR.T3CON.value=(SFR.T3CON.value & 0xffff5f05) + (num & 0xa0fa);
	return SFR.T3CON.value & 0xa0fa;
};
SFR.T3CONCLR=function(num){
	var val=SFR.T3CON.value;
	SFR.T3CON(val&(0xFFFFFFFF-num));
};
SFR.T3CONSET=function(num){
	var val=SFR.T3CON.value;
	SFR.T3CON(val|num);
};
SFR.T3CONINV=function(num){
	var val=SFR.T3CON.value;
	SFR.T3CON(val^num);
};
SFR[0xBF800A00]=SFR.T3CON;
SFR[0xBF800A04]=SFR.T3CONCLR;
SFR[0xBF800A08]=SFR.T3CONSET;
SFR[0xBF800A0C]=SFR.T3CONINV;
SFR.T3CON(0x0);
SFR.TMR3=function(num){
	if (num!=undefined) SFR.TMR3.value=(SFR.TMR3.value & 0xffff0000) + (num & 0xffff);
	return SFR.TMR3.value & 0xffff;
};
SFR.TMR3CLR=function(num){
	var val=SFR.TMR3.value;
	SFR.TMR3(val&(0xFFFFFFFF-num));
};
SFR.TMR3SET=function(num){
	var val=SFR.TMR3.value;
	SFR.TMR3(val|num);
};
SFR.TMR3INV=function(num){
	var val=SFR.TMR3.value;
	SFR.TMR3(val^num);
};
SFR[0xBF800A10]=SFR.TMR3;
SFR[0xBF800A14]=SFR.TMR3CLR;
SFR[0xBF800A18]=SFR.TMR3SET;
SFR[0xBF800A1C]=SFR.TMR3INV;
SFR.TMR3(0x0);
SFR.PR3=function(num){
	if (num!=undefined) SFR.PR3.value=(SFR.PR3.value & 0xffff0000) + (num & 0xffff);
	return SFR.PR3.value & 0xffff;
};
SFR.PR3CLR=function(num){
	var val=SFR.PR3.value;
	SFR.PR3(val&(0xFFFFFFFF-num));
};
SFR.PR3SET=function(num){
	var val=SFR.PR3.value;
	SFR.PR3(val|num);
};
SFR.PR3INV=function(num){
	var val=SFR.PR3.value;
	SFR.PR3(val^num);
};
SFR[0xBF800A20]=SFR.PR3;
SFR[0xBF800A24]=SFR.PR3CLR;
SFR[0xBF800A28]=SFR.PR3SET;
SFR[0xBF800A2C]=SFR.PR3INV;
SFR.PR3(0xffff);
SFR.T4CON=function(num){
	if (num!=undefined) SFR.T4CON.value=(SFR.T4CON.value & 0xffff5f05) + (num & 0xa0fa);
	return SFR.T4CON.value & 0xa0fa;
};
SFR.T4CONCLR=function(num){
	var val=SFR.T4CON.value;
	SFR.T4CON(val&(0xFFFFFFFF-num));
};
SFR.T4CONSET=function(num){
	var val=SFR.T4CON.value;
	SFR.T4CON(val|num);
};
SFR.T4CONINV=function(num){
	var val=SFR.T4CON.value;
	SFR.T4CON(val^num);
};
SFR[0xBF800C00]=SFR.T4CON;
SFR[0xBF800C04]=SFR.T4CONCLR;
SFR[0xBF800C08]=SFR.T4CONSET;
SFR[0xBF800C0C]=SFR.T4CONINV;
SFR.T4CON(0x0);
SFR.TMR4=function(num){
	if (num!=undefined) SFR.TMR4.value=(SFR.TMR4.value & 0xffff0000) + (num & 0xffff);
	return SFR.TMR4.value & 0xffff;
};
SFR.TMR4CLR=function(num){
	var val=SFR.TMR4.value;
	SFR.TMR4(val&(0xFFFFFFFF-num));
};
SFR.TMR4SET=function(num){
	var val=SFR.TMR4.value;
	SFR.TMR4(val|num);
};
SFR.TMR4INV=function(num){
	var val=SFR.TMR4.value;
	SFR.TMR4(val^num);
};
SFR[0xBF800C10]=SFR.TMR4;
SFR[0xBF800C14]=SFR.TMR4CLR;
SFR[0xBF800C18]=SFR.TMR4SET;
SFR[0xBF800C1C]=SFR.TMR4INV;
SFR.TMR4(0x0);
SFR.PR4=function(num){
	if (num!=undefined) SFR.PR4.value=(SFR.PR4.value & 0xffff0000) + (num & 0xffff);
	return SFR.PR4.value & 0xffff;
};
SFR.PR4CLR=function(num){
	var val=SFR.PR4.value;
	SFR.PR4(val&(0xFFFFFFFF-num));
};
SFR.PR4SET=function(num){
	var val=SFR.PR4.value;
	SFR.PR4(val|num);
};
SFR.PR4INV=function(num){
	var val=SFR.PR4.value;
	SFR.PR4(val^num);
};
SFR[0xBF800C20]=SFR.PR4;
SFR[0xBF800C24]=SFR.PR4CLR;
SFR[0xBF800C28]=SFR.PR4SET;
SFR[0xBF800C2C]=SFR.PR4INV;
SFR.PR4(0xffff);
SFR.T5CON=function(num){
	if (num!=undefined) SFR.T5CON.value=(SFR.T5CON.value & 0xffff5f05) + (num & 0xa0fa);
	return SFR.T5CON.value & 0xa0fa;
};
SFR.T5CONCLR=function(num){
	var val=SFR.T5CON.value;
	SFR.T5CON(val&(0xFFFFFFFF-num));
};
SFR.T5CONSET=function(num){
	var val=SFR.T5CON.value;
	SFR.T5CON(val|num);
};
SFR.T5CONINV=function(num){
	var val=SFR.T5CON.value;
	SFR.T5CON(val^num);
};
SFR[0xBF800E00]=SFR.T5CON;
SFR[0xBF800E04]=SFR.T5CONCLR;
SFR[0xBF800E08]=SFR.T5CONSET;
SFR[0xBF800E0C]=SFR.T5CONINV;
SFR.T5CON(0x0);
SFR.TMR5=function(num){
	if (num!=undefined) SFR.TMR5.value=(SFR.TMR5.value & 0xffff0000) + (num & 0xffff);
	return SFR.TMR5.value & 0xffff;
};
SFR.TMR5CLR=function(num){
	var val=SFR.TMR5.value;
	SFR.TMR5(val&(0xFFFFFFFF-num));
};
SFR.TMR5SET=function(num){
	var val=SFR.TMR5.value;
	SFR.TMR5(val|num);
};
SFR.TMR5INV=function(num){
	var val=SFR.TMR5.value;
	SFR.TMR5(val^num);
};
SFR[0xBF800E10]=SFR.TMR5;
SFR[0xBF800E14]=SFR.TMR5CLR;
SFR[0xBF800E18]=SFR.TMR5SET;
SFR[0xBF800E1C]=SFR.TMR5INV;
SFR.TMR5(0x0);
SFR.PR5=function(num){
	if (num!=undefined) SFR.PR5.value=(SFR.PR5.value & 0xffff0000) + (num & 0xffff);
	return SFR.PR5.value & 0xffff;
};
SFR.PR5CLR=function(num){
	var val=SFR.PR5.value;
	SFR.PR5(val&(0xFFFFFFFF-num));
};
SFR.PR5SET=function(num){
	var val=SFR.PR5.value;
	SFR.PR5(val|num);
};
SFR.PR5INV=function(num){
	var val=SFR.PR5.value;
	SFR.PR5(val^num);
};
SFR[0xBF800E20]=SFR.PR5;
SFR[0xBF800E24]=SFR.PR5CLR;
SFR[0xBF800E28]=SFR.PR5SET;
SFR[0xBF800E2C]=SFR.PR5INV;
SFR.PR5(0xffff);
SFR.IC1CON=function(num){
	if (num!=undefined) SFR.IC1CON.value=(SFR.IC1CON.value & 0xffff5c18) + (num & 0xa3e7);
	return SFR.IC1CON.value & 0xa3ff;
};
SFR.IC1CONCLR=function(num){
	var val=SFR.IC1CON.value;
	SFR.IC1CON(val&(0xFFFFFFFF-num));
};
SFR.IC1CONSET=function(num){
	var val=SFR.IC1CON.value;
	SFR.IC1CON(val|num);
};
SFR.IC1CONINV=function(num){
	var val=SFR.IC1CON.value;
	SFR.IC1CON(val^num);
};
SFR[0xBF802000]=SFR.IC1CON;
SFR[0xBF802004]=SFR.IC1CONCLR;
SFR[0xBF802008]=SFR.IC1CONSET;
SFR[0xBF80200C]=SFR.IC1CONINV;
SFR.IC1CON(0x0);
SFR.IC2CON=function(num){
	if (num!=undefined) SFR.IC2CON.value=(SFR.IC2CON.value & 0xffff5c18) + (num & 0xa3e7);
	return SFR.IC2CON.value & 0xa3ff;
};
SFR.IC2CONCLR=function(num){
	var val=SFR.IC2CON.value;
	SFR.IC2CON(val&(0xFFFFFFFF-num));
};
SFR.IC2CONSET=function(num){
	var val=SFR.IC2CON.value;
	SFR.IC2CON(val|num);
};
SFR.IC2CONINV=function(num){
	var val=SFR.IC2CON.value;
	SFR.IC2CON(val^num);
};
SFR[0xBF802200]=SFR.IC2CON;
SFR[0xBF802204]=SFR.IC2CONCLR;
SFR[0xBF802208]=SFR.IC2CONSET;
SFR[0xBF80220C]=SFR.IC2CONINV;
SFR.IC2CON(0x0);
SFR.IC3CON=function(num){
	if (num!=undefined) SFR.IC3CON.value=(SFR.IC3CON.value & 0xffff5c18) + (num & 0xa3e7);
	return SFR.IC3CON.value & 0xa3ff;
};
SFR.IC3CONCLR=function(num){
	var val=SFR.IC3CON.value;
	SFR.IC3CON(val&(0xFFFFFFFF-num));
};
SFR.IC3CONSET=function(num){
	var val=SFR.IC3CON.value;
	SFR.IC3CON(val|num);
};
SFR.IC3CONINV=function(num){
	var val=SFR.IC3CON.value;
	SFR.IC3CON(val^num);
};
SFR[0xBF802400]=SFR.IC3CON;
SFR[0xBF802404]=SFR.IC3CONCLR;
SFR[0xBF802408]=SFR.IC3CONSET;
SFR[0xBF80240C]=SFR.IC3CONINV;
SFR.IC3CON(0x0);
SFR.IC4CON=function(num){
	if (num!=undefined) SFR.IC4CON.value=(SFR.IC4CON.value & 0xffff5c18) + (num & 0xa3e7);
	return SFR.IC4CON.value & 0xa3ff;
};
SFR.IC4CONCLR=function(num){
	var val=SFR.IC4CON.value;
	SFR.IC4CON(val&(0xFFFFFFFF-num));
};
SFR.IC4CONSET=function(num){
	var val=SFR.IC4CON.value;
	SFR.IC4CON(val|num);
};
SFR.IC4CONINV=function(num){
	var val=SFR.IC4CON.value;
	SFR.IC4CON(val^num);
};
SFR[0xBF802600]=SFR.IC4CON;
SFR[0xBF802604]=SFR.IC4CONCLR;
SFR[0xBF802608]=SFR.IC4CONSET;
SFR[0xBF80260C]=SFR.IC4CONINV;
SFR.IC4CON(0x0);
SFR.IC5CON=function(num){
	if (num!=undefined) SFR.IC5CON.value=(SFR.IC5CON.value & 0xffff5c18) + (num & 0xa3e7);
	return SFR.IC5CON.value & 0xa3ff;
};
SFR.IC5CONCLR=function(num){
	var val=SFR.IC5CON.value;
	SFR.IC5CON(val&(0xFFFFFFFF-num));
};
SFR.IC5CONSET=function(num){
	var val=SFR.IC5CON.value;
	SFR.IC5CON(val|num);
};
SFR.IC5CONINV=function(num){
	var val=SFR.IC5CON.value;
	SFR.IC5CON(val^num);
};
SFR[0xBF802800]=SFR.IC5CON;
SFR[0xBF802804]=SFR.IC5CONCLR;
SFR[0xBF802808]=SFR.IC5CONSET;
SFR[0xBF80280C]=SFR.IC5CONINV;
SFR.IC5CON(0x0);
SFR.OC1CON=function(num){
	if (num!=undefined) SFR.OC1CON.value=(SFR.OC1CON.value & 0xffff5fd0) + (num & 0xa02f);
	return SFR.OC1CON.value & 0xa03f;
};
SFR.OC1CONCLR=function(num){
	var val=SFR.OC1CON.value;
	SFR.OC1CON(val&(0xFFFFFFFF-num));
};
SFR.OC1CONSET=function(num){
	var val=SFR.OC1CON.value;
	SFR.OC1CON(val|num);
};
SFR.OC1CONINV=function(num){
	var val=SFR.OC1CON.value;
	SFR.OC1CON(val^num);
};
SFR[0xBF803000]=SFR.OC1CON;
SFR[0xBF803004]=SFR.OC1CONCLR;
SFR[0xBF803008]=SFR.OC1CONSET;
SFR[0xBF80300C]=SFR.OC1CONINV;
SFR.OC1CON(0x0);
SFR.OC1R=function(num){
	if (num!=undefined) SFR.OC1R.value=(SFR.OC1R.value & 0x0) + (num & 0xffffffff);
	return SFR.OC1R.value & 0xffffffff;
};
SFR.OC1RCLR=function(num){
	var val=SFR.OC1R.value;
	SFR.OC1R(val&(0xFFFFFFFF-num));
};
SFR.OC1RSET=function(num){
	var val=SFR.OC1R.value;
	SFR.OC1R(val|num);
};
SFR.OC1RINV=function(num){
	var val=SFR.OC1R.value;
	SFR.OC1R(val^num);
};
SFR[0xBF803010]=SFR.OC1R;
SFR[0xBF803014]=SFR.OC1RCLR;
SFR[0xBF803018]=SFR.OC1RSET;
SFR[0xBF80301C]=SFR.OC1RINV;
SFR.OC1R(0x0);
SFR.OC1RS=function(num){
	if (num!=undefined) SFR.OC1RS.value=(SFR.OC1RS.value & 0x0) + (num & 0xffffffff);
	return SFR.OC1RS.value & 0xffffffff;
};
SFR.OC1RSCLR=function(num){
	var val=SFR.OC1RS.value;
	SFR.OC1RS(val&(0xFFFFFFFF-num));
};
SFR.OC1RSSET=function(num){
	var val=SFR.OC1RS.value;
	SFR.OC1RS(val|num);
};
SFR.OC1RSINV=function(num){
	var val=SFR.OC1RS.value;
	SFR.OC1RS(val^num);
};
SFR[0xBF803020]=SFR.OC1RS;
SFR[0xBF803024]=SFR.OC1RSCLR;
SFR[0xBF803028]=SFR.OC1RSSET;
SFR[0xBF80302C]=SFR.OC1RSINV;
SFR.OC1RS(0x0);
SFR.OC2CON=function(num){
	if (num!=undefined) SFR.OC2CON.value=(SFR.OC2CON.value & 0xffff5fd0) + (num & 0xa02f);
	return SFR.OC2CON.value & 0xa03f;
};
SFR.OC2CONCLR=function(num){
	var val=SFR.OC2CON.value;
	SFR.OC2CON(val&(0xFFFFFFFF-num));
};
SFR.OC2CONSET=function(num){
	var val=SFR.OC2CON.value;
	SFR.OC2CON(val|num);
};
SFR.OC2CONINV=function(num){
	var val=SFR.OC2CON.value;
	SFR.OC2CON(val^num);
};
SFR[0xBF803200]=SFR.OC2CON;
SFR[0xBF803204]=SFR.OC2CONCLR;
SFR[0xBF803208]=SFR.OC2CONSET;
SFR[0xBF80320C]=SFR.OC2CONINV;
SFR.OC2CON(0x0);
SFR.OC2R=function(num){
	if (num!=undefined) SFR.OC2R.value=(SFR.OC2R.value & 0x0) + (num & 0xffffffff);
	return SFR.OC2R.value & 0xffffffff;
};
SFR.OC2RCLR=function(num){
	var val=SFR.OC2R.value;
	SFR.OC2R(val&(0xFFFFFFFF-num));
};
SFR.OC2RSET=function(num){
	var val=SFR.OC2R.value;
	SFR.OC2R(val|num);
};
SFR.OC2RINV=function(num){
	var val=SFR.OC2R.value;
	SFR.OC2R(val^num);
};
SFR[0xBF803210]=SFR.OC2R;
SFR[0xBF803214]=SFR.OC2RCLR;
SFR[0xBF803218]=SFR.OC2RSET;
SFR[0xBF80321C]=SFR.OC2RINV;
SFR.OC2R(0x0);
SFR.OC2RS=function(num){
	if (num!=undefined) SFR.OC2RS.value=(SFR.OC2RS.value & 0x0) + (num & 0xffffffff);
	return SFR.OC2RS.value & 0xffffffff;
};
SFR.OC2RSCLR=function(num){
	var val=SFR.OC2RS.value;
	SFR.OC2RS(val&(0xFFFFFFFF-num));
};
SFR.OC2RSSET=function(num){
	var val=SFR.OC2RS.value;
	SFR.OC2RS(val|num);
};
SFR.OC2RSINV=function(num){
	var val=SFR.OC2RS.value;
	SFR.OC2RS(val^num);
};
SFR[0xBF803220]=SFR.OC2RS;
SFR[0xBF803224]=SFR.OC2RSCLR;
SFR[0xBF803228]=SFR.OC2RSSET;
SFR[0xBF80322C]=SFR.OC2RSINV;
SFR.OC2RS(0x0);
SFR.OC3CON=function(num){
	if (num!=undefined) SFR.OC3CON.value=(SFR.OC3CON.value & 0xffff5fd0) + (num & 0xa02f);
	return SFR.OC3CON.value & 0xa03f;
};
SFR.OC3CONCLR=function(num){
	var val=SFR.OC3CON.value;
	SFR.OC3CON(val&(0xFFFFFFFF-num));
};
SFR.OC3CONSET=function(num){
	var val=SFR.OC3CON.value;
	SFR.OC3CON(val|num);
};
SFR.OC3CONINV=function(num){
	var val=SFR.OC3CON.value;
	SFR.OC3CON(val^num);
};
SFR[0xBF803400]=SFR.OC3CON;
SFR[0xBF803404]=SFR.OC3CONCLR;
SFR[0xBF803408]=SFR.OC3CONSET;
SFR[0xBF80340C]=SFR.OC3CONINV;
SFR.OC3CON(0x0);
SFR.OC3R=function(num){
	if (num!=undefined) SFR.OC3R.value=(SFR.OC3R.value & 0x0) + (num & 0xffffffff);
	return SFR.OC3R.value & 0xffffffff;
};
SFR.OC3RCLR=function(num){
	var val=SFR.OC3R.value;
	SFR.OC3R(val&(0xFFFFFFFF-num));
};
SFR.OC3RSET=function(num){
	var val=SFR.OC3R.value;
	SFR.OC3R(val|num);
};
SFR.OC3RINV=function(num){
	var val=SFR.OC3R.value;
	SFR.OC3R(val^num);
};
SFR[0xBF803410]=SFR.OC3R;
SFR[0xBF803414]=SFR.OC3RCLR;
SFR[0xBF803418]=SFR.OC3RSET;
SFR[0xBF80341C]=SFR.OC3RINV;
SFR.OC3R(0x0);
SFR.OC3RS=function(num){
	if (num!=undefined) SFR.OC3RS.value=(SFR.OC3RS.value & 0x0) + (num & 0xffffffff);
	return SFR.OC3RS.value & 0xffffffff;
};
SFR.OC3RSCLR=function(num){
	var val=SFR.OC3RS.value;
	SFR.OC3RS(val&(0xFFFFFFFF-num));
};
SFR.OC3RSSET=function(num){
	var val=SFR.OC3RS.value;
	SFR.OC3RS(val|num);
};
SFR.OC3RSINV=function(num){
	var val=SFR.OC3RS.value;
	SFR.OC3RS(val^num);
};
SFR[0xBF803420]=SFR.OC3RS;
SFR[0xBF803424]=SFR.OC3RSCLR;
SFR[0xBF803428]=SFR.OC3RSSET;
SFR[0xBF80342C]=SFR.OC3RSINV;
SFR.OC3RS(0x0);
SFR.OC4CON=function(num){
	if (num!=undefined) SFR.OC4CON.value=(SFR.OC4CON.value & 0xffff5fd0) + (num & 0xa02f);
	return SFR.OC4CON.value & 0xa03f;
};
SFR.OC4CONCLR=function(num){
	var val=SFR.OC4CON.value;
	SFR.OC4CON(val&(0xFFFFFFFF-num));
};
SFR.OC4CONSET=function(num){
	var val=SFR.OC4CON.value;
	SFR.OC4CON(val|num);
};
SFR.OC4CONINV=function(num){
	var val=SFR.OC4CON.value;
	SFR.OC4CON(val^num);
};
SFR[0xBF803600]=SFR.OC4CON;
SFR[0xBF803604]=SFR.OC4CONCLR;
SFR[0xBF803608]=SFR.OC4CONSET;
SFR[0xBF80360C]=SFR.OC4CONINV;
SFR.OC4CON(0x0);
SFR.OC4R=function(num){
	if (num!=undefined) SFR.OC4R.value=(SFR.OC4R.value & 0x0) + (num & 0xffffffff);
	return SFR.OC4R.value & 0xffffffff;
};
SFR.OC4RCLR=function(num){
	var val=SFR.OC4R.value;
	SFR.OC4R(val&(0xFFFFFFFF-num));
};
SFR.OC4RSET=function(num){
	var val=SFR.OC4R.value;
	SFR.OC4R(val|num);
};
SFR.OC4RINV=function(num){
	var val=SFR.OC4R.value;
	SFR.OC4R(val^num);
};
SFR[0xBF803610]=SFR.OC4R;
SFR[0xBF803614]=SFR.OC4RCLR;
SFR[0xBF803618]=SFR.OC4RSET;
SFR[0xBF80361C]=SFR.OC4RINV;
SFR.OC4R(0x0);
SFR.OC4RS=function(num){
	if (num!=undefined) SFR.OC4RS.value=(SFR.OC4RS.value & 0x0) + (num & 0xffffffff);
	return SFR.OC4RS.value & 0xffffffff;
};
SFR.OC4RSCLR=function(num){
	var val=SFR.OC4RS.value;
	SFR.OC4RS(val&(0xFFFFFFFF-num));
};
SFR.OC4RSSET=function(num){
	var val=SFR.OC4RS.value;
	SFR.OC4RS(val|num);
};
SFR.OC4RSINV=function(num){
	var val=SFR.OC4RS.value;
	SFR.OC4RS(val^num);
};
SFR[0xBF803620]=SFR.OC4RS;
SFR[0xBF803624]=SFR.OC4RSCLR;
SFR[0xBF803628]=SFR.OC4RSSET;
SFR[0xBF80362C]=SFR.OC4RSINV;
SFR.OC4RS(0x0);
SFR.OC5CON=function(num){
	if (num!=undefined) SFR.OC5CON.value=(SFR.OC5CON.value & 0xffff5fd0) + (num & 0xa02f);
	return SFR.OC5CON.value & 0xa03f;
};
SFR.OC5CONCLR=function(num){
	var val=SFR.OC5CON.value;
	SFR.OC5CON(val&(0xFFFFFFFF-num));
};
SFR.OC5CONSET=function(num){
	var val=SFR.OC5CON.value;
	SFR.OC5CON(val|num);
};
SFR.OC5CONINV=function(num){
	var val=SFR.OC5CON.value;
	SFR.OC5CON(val^num);
};
SFR[0xBF803800]=SFR.OC5CON;
SFR[0xBF803804]=SFR.OC5CONCLR;
SFR[0xBF803808]=SFR.OC5CONSET;
SFR[0xBF80380C]=SFR.OC5CONINV;
SFR.OC5CON(0x0);
SFR.OC5R=function(num){
	if (num!=undefined) SFR.OC5R.value=(SFR.OC5R.value & 0x0) + (num & 0xffffffff);
	return SFR.OC5R.value & 0xffffffff;
};
SFR.OC5RCLR=function(num){
	var val=SFR.OC5R.value;
	SFR.OC5R(val&(0xFFFFFFFF-num));
};
SFR.OC5RSET=function(num){
	var val=SFR.OC5R.value;
	SFR.OC5R(val|num);
};
SFR.OC5RINV=function(num){
	var val=SFR.OC5R.value;
	SFR.OC5R(val^num);
};
SFR[0xBF803810]=SFR.OC5R;
SFR[0xBF803814]=SFR.OC5RCLR;
SFR[0xBF803818]=SFR.OC5RSET;
SFR[0xBF80381C]=SFR.OC5RINV;
SFR.OC5R(0x0);
SFR.OC5RS=function(num){
	if (num!=undefined) SFR.OC5RS.value=(SFR.OC5RS.value & 0x0) + (num & 0xffffffff);
	return SFR.OC5RS.value & 0xffffffff;
};
SFR.OC5RSCLR=function(num){
	var val=SFR.OC5RS.value;
	SFR.OC5RS(val&(0xFFFFFFFF-num));
};
SFR.OC5RSSET=function(num){
	var val=SFR.OC5RS.value;
	SFR.OC5RS(val|num);
};
SFR.OC5RSINV=function(num){
	var val=SFR.OC5RS.value;
	SFR.OC5RS(val^num);
};
SFR[0xBF803820]=SFR.OC5RS;
SFR[0xBF803824]=SFR.OC5RSCLR;
SFR[0xBF803828]=SFR.OC5RSSET;
SFR[0xBF80382C]=SFR.OC5RSINV;
SFR.OC5RS(0x0);
SFR.I2C1CON=function(num){
	if (num!=undefined) SFR.I2C1CON.value=(SFR.I2C1CON.value & 0xffff4000) + (num & 0xbfff);
	return SFR.I2C1CON.value & 0xbfff;
};
SFR.I2C1CONCLR=function(num){
	var val=SFR.I2C1CON.value;
	SFR.I2C1CON(val&(0xFFFFFFFF-num));
};
SFR.I2C1CONSET=function(num){
	var val=SFR.I2C1CON.value;
	SFR.I2C1CON(val|num);
};
SFR.I2C1CONINV=function(num){
	var val=SFR.I2C1CON.value;
	SFR.I2C1CON(val^num);
};
SFR[0xBF805000]=SFR.I2C1CON;
SFR[0xBF805004]=SFR.I2C1CONCLR;
SFR[0xBF805008]=SFR.I2C1CONSET;
SFR[0xBF80500C]=SFR.I2C1CONINV;
SFR.I2C1CON(0x1000);
SFR.I2C1STAT=function(num){
	if (num!=undefined) SFR.I2C1STAT.value=(SFR.I2C1STAT.value & 0xffffffff) + (num & 0x0);
	return SFR.I2C1STAT.value & 0xc7ff;
};
SFR.I2C1STATCLR=function(num){
	var val=SFR.I2C1STAT.value;
	SFR.I2C1STAT(val&(0xFFFFFFFF-num));
};
SFR.I2C1STATSET=function(num){
	var val=SFR.I2C1STAT.value;
	SFR.I2C1STAT(val|num);
};
SFR.I2C1STATINV=function(num){
	var val=SFR.I2C1STAT.value;
	SFR.I2C1STAT(val^num);
};
SFR[0xBF805010]=SFR.I2C1STAT;
SFR[0xBF805014]=SFR.I2C1STATCLR;
SFR[0xBF805018]=SFR.I2C1STATSET;
SFR[0xBF80501C]=SFR.I2C1STATINV;
SFR.I2C1STAT(0x0);
SFR.I2C1ADD=function(num){
	if (num!=undefined) SFR.I2C1ADD.value=(SFR.I2C1ADD.value & 0xfffffc00) + (num & 0x3ff);
	return SFR.I2C1ADD.value & 0x3ff;
};
SFR.I2C1ADDCLR=function(num){
	var val=SFR.I2C1ADD.value;
	SFR.I2C1ADD(val&(0xFFFFFFFF-num));
};
SFR.I2C1ADDSET=function(num){
	var val=SFR.I2C1ADD.value;
	SFR.I2C1ADD(val|num);
};
SFR.I2C1ADDINV=function(num){
	var val=SFR.I2C1ADD.value;
	SFR.I2C1ADD(val^num);
};
SFR[0xBF805020]=SFR.I2C1ADD;
SFR[0xBF805024]=SFR.I2C1ADDCLR;
SFR[0xBF805028]=SFR.I2C1ADDSET;
SFR[0xBF80502C]=SFR.I2C1ADDINV;
SFR.I2C1ADD(0x0);
SFR.I2C1MSK=function(num){
	if (num!=undefined) SFR.I2C1MSK.value=(SFR.I2C1MSK.value & 0xfffffc00) + (num & 0x3ff);
	return SFR.I2C1MSK.value & 0x3ff;
};
SFR.I2C1MSKCLR=function(num){
	var val=SFR.I2C1MSK.value;
	SFR.I2C1MSK(val&(0xFFFFFFFF-num));
};
SFR.I2C1MSKSET=function(num){
	var val=SFR.I2C1MSK.value;
	SFR.I2C1MSK(val|num);
};
SFR.I2C1MSKINV=function(num){
	var val=SFR.I2C1MSK.value;
	SFR.I2C1MSK(val^num);
};
SFR[0xBF805030]=SFR.I2C1MSK;
SFR[0xBF805034]=SFR.I2C1MSKCLR;
SFR[0xBF805038]=SFR.I2C1MSKSET;
SFR[0xBF80503C]=SFR.I2C1MSKINV;
SFR.I2C1MSK(0x0);
SFR.I2C1BRG=function(num){
	if (num!=undefined) SFR.I2C1BRG.value=(SFR.I2C1BRG.value & 0xfffff000) + (num & 0xfff);
	return SFR.I2C1BRG.value & 0xfff;
};
SFR.I2C1BRGCLR=function(num){
	var val=SFR.I2C1BRG.value;
	SFR.I2C1BRG(val&(0xFFFFFFFF-num));
};
SFR.I2C1BRGSET=function(num){
	var val=SFR.I2C1BRG.value;
	SFR.I2C1BRG(val|num);
};
SFR.I2C1BRGINV=function(num){
	var val=SFR.I2C1BRG.value;
	SFR.I2C1BRG(val^num);
};
SFR[0xBF805040]=SFR.I2C1BRG;
SFR[0xBF805044]=SFR.I2C1BRGCLR;
SFR[0xBF805048]=SFR.I2C1BRGSET;
SFR[0xBF80504C]=SFR.I2C1BRGINV;
SFR.I2C1BRG(0x0);
SFR.I2C1TRN=function(num){
	if (num!=undefined) SFR.I2C1TRN.value=(SFR.I2C1TRN.value & 0xffffff00) + (num & 0xff);
	return SFR.I2C1TRN.value & 0xff;
};
SFR.I2C1TRNCLR=function(num){
	var val=SFR.I2C1TRN.value;
	SFR.I2C1TRN(val&(0xFFFFFFFF-num));
};
SFR.I2C1TRNSET=function(num){
	var val=SFR.I2C1TRN.value;
	SFR.I2C1TRN(val|num);
};
SFR.I2C1TRNINV=function(num){
	var val=SFR.I2C1TRN.value;
	SFR.I2C1TRN(val^num);
};
SFR[0xBF805050]=SFR.I2C1TRN;
SFR[0xBF805054]=SFR.I2C1TRNCLR;
SFR[0xBF805058]=SFR.I2C1TRNSET;
SFR[0xBF80505C]=SFR.I2C1TRNINV;
SFR.I2C1TRN(0x0);
SFR.I2C2CON=function(num){
	if (num!=undefined) SFR.I2C2CON.value=(SFR.I2C2CON.value & 0xffff4000) + (num & 0xbfff);
	return SFR.I2C2CON.value & 0xbfff;
};
SFR.I2C2CONCLR=function(num){
	var val=SFR.I2C2CON.value;
	SFR.I2C2CON(val&(0xFFFFFFFF-num));
};
SFR.I2C2CONSET=function(num){
	var val=SFR.I2C2CON.value;
	SFR.I2C2CON(val|num);
};
SFR.I2C2CONINV=function(num){
	var val=SFR.I2C2CON.value;
	SFR.I2C2CON(val^num);
};
SFR[0xBF805100]=SFR.I2C2CON;
SFR[0xBF805104]=SFR.I2C2CONCLR;
SFR[0xBF805108]=SFR.I2C2CONSET;
SFR[0xBF80510C]=SFR.I2C2CONINV;
SFR.I2C2CON(0x1000);
SFR.I2C2STAT=function(num){
	if (num!=undefined) SFR.I2C2STAT.value=(SFR.I2C2STAT.value & 0xffffffff) + (num & 0x0);
	return SFR.I2C2STAT.value & 0xc7ff;
};
SFR.I2C2STATCLR=function(num){
	var val=SFR.I2C2STAT.value;
	SFR.I2C2STAT(val&(0xFFFFFFFF-num));
};
SFR.I2C2STATSET=function(num){
	var val=SFR.I2C2STAT.value;
	SFR.I2C2STAT(val|num);
};
SFR.I2C2STATINV=function(num){
	var val=SFR.I2C2STAT.value;
	SFR.I2C2STAT(val^num);
};
SFR[0xBF805110]=SFR.I2C2STAT;
SFR[0xBF805114]=SFR.I2C2STATCLR;
SFR[0xBF805118]=SFR.I2C2STATSET;
SFR[0xBF80511C]=SFR.I2C2STATINV;
SFR.I2C2STAT(0x0);
SFR.I2C2ADD=function(num){
	if (num!=undefined) SFR.I2C2ADD.value=(SFR.I2C2ADD.value & 0xfffffc00) + (num & 0x3ff);
	return SFR.I2C2ADD.value & 0x3ff;
};
SFR.I2C2ADDCLR=function(num){
	var val=SFR.I2C2ADD.value;
	SFR.I2C2ADD(val&(0xFFFFFFFF-num));
};
SFR.I2C2ADDSET=function(num){
	var val=SFR.I2C2ADD.value;
	SFR.I2C2ADD(val|num);
};
SFR.I2C2ADDINV=function(num){
	var val=SFR.I2C2ADD.value;
	SFR.I2C2ADD(val^num);
};
SFR[0xBF805120]=SFR.I2C2ADD;
SFR[0xBF805124]=SFR.I2C2ADDCLR;
SFR[0xBF805128]=SFR.I2C2ADDSET;
SFR[0xBF80512C]=SFR.I2C2ADDINV;
SFR.I2C2ADD(0x0);
SFR.I2C2MSK=function(num){
	if (num!=undefined) SFR.I2C2MSK.value=(SFR.I2C2MSK.value & 0xfffffc00) + (num & 0x3ff);
	return SFR.I2C2MSK.value & 0x3ff;
};
SFR.I2C2MSKCLR=function(num){
	var val=SFR.I2C2MSK.value;
	SFR.I2C2MSK(val&(0xFFFFFFFF-num));
};
SFR.I2C2MSKSET=function(num){
	var val=SFR.I2C2MSK.value;
	SFR.I2C2MSK(val|num);
};
SFR.I2C2MSKINV=function(num){
	var val=SFR.I2C2MSK.value;
	SFR.I2C2MSK(val^num);
};
SFR[0xBF805130]=SFR.I2C2MSK;
SFR[0xBF805134]=SFR.I2C2MSKCLR;
SFR[0xBF805138]=SFR.I2C2MSKSET;
SFR[0xBF80513C]=SFR.I2C2MSKINV;
SFR.I2C2MSK(0x0);
SFR.I2C2BRG=function(num){
	if (num!=undefined) SFR.I2C2BRG.value=(SFR.I2C2BRG.value & 0xfffff000) + (num & 0xfff);
	return SFR.I2C2BRG.value & 0xfff;
};
SFR.I2C2BRGCLR=function(num){
	var val=SFR.I2C2BRG.value;
	SFR.I2C2BRG(val&(0xFFFFFFFF-num));
};
SFR.I2C2BRGSET=function(num){
	var val=SFR.I2C2BRG.value;
	SFR.I2C2BRG(val|num);
};
SFR.I2C2BRGINV=function(num){
	var val=SFR.I2C2BRG.value;
	SFR.I2C2BRG(val^num);
};
SFR[0xBF805140]=SFR.I2C2BRG;
SFR[0xBF805144]=SFR.I2C2BRGCLR;
SFR[0xBF805148]=SFR.I2C2BRGSET;
SFR[0xBF80514C]=SFR.I2C2BRGINV;
SFR.I2C2BRG(0x0);
SFR.I2C2TRN=function(num){
	if (num!=undefined) SFR.I2C2TRN.value=(SFR.I2C2TRN.value & 0xffffff00) + (num & 0xff);
	return SFR.I2C2TRN.value & 0xff;
};
SFR.I2C2TRNCLR=function(num){
	var val=SFR.I2C2TRN.value;
	SFR.I2C2TRN(val&(0xFFFFFFFF-num));
};
SFR.I2C2TRNSET=function(num){
	var val=SFR.I2C2TRN.value;
	SFR.I2C2TRN(val|num);
};
SFR.I2C2TRNINV=function(num){
	var val=SFR.I2C2TRN.value;
	SFR.I2C2TRN(val^num);
};
SFR[0xBF805150]=SFR.I2C2TRN;
SFR[0xBF805154]=SFR.I2C2TRNCLR;
SFR[0xBF805158]=SFR.I2C2TRNSET;
SFR[0xBF80515C]=SFR.I2C2TRNINV;
SFR.I2C2TRN(0x0);
SFR.SPI1CON=function(num){
	if (num!=undefined) SFR.SPI1CON.value=(SFR.SPI1CON.value & 0x7c0000) + (num & 0xff83ffff);
	return SFR.SPI1CON.value & 0xff83ffff;
};
SFR.SPI1CONCLR=function(num){
	var val=SFR.SPI1CON.value;
	SFR.SPI1CON(val&(0xFFFFFFFF-num));
};
SFR.SPI1CONSET=function(num){
	var val=SFR.SPI1CON.value;
	SFR.SPI1CON(val|num);
};
SFR.SPI1CONINV=function(num){
	var val=SFR.SPI1CON.value;
	SFR.SPI1CON(val^num);
};
SFR[0xBF805800]=SFR.SPI1CON;
SFR[0xBF805804]=SFR.SPI1CONCLR;
SFR[0xBF805808]=SFR.SPI1CONSET;
SFR[0xBF80580C]=SFR.SPI1CONINV;
SFR.SPI1CON(0x0);
SFR.SPI1STAT=function(num){
	if (num!=undefined) SFR.SPI1STAT.value=(SFR.SPI1STAT.value & 0xffffefff) + (num & 0x1000);
	return SFR.SPI1STAT.value & 0x1f1f19eb;
};
SFR.SPI1STATCLR=function(num){
	var val=SFR.SPI1STAT.value;
	SFR.SPI1STAT(val&(0xFFFFFFFF-num));
};
SFR.SPI1STATSET=function(num){
	var val=SFR.SPI1STAT.value;
	SFR.SPI1STAT(val|num);
};
SFR.SPI1STATINV=function(num){
	var val=SFR.SPI1STAT.value;
	SFR.SPI1STAT(val^num);
};
SFR[0xBF805810]=SFR.SPI1STAT;
SFR[0xBF805814]=SFR.SPI1STATCLR;
SFR[0xBF805818]=SFR.SPI1STATSET;
SFR[0xBF80581C]=SFR.SPI1STATINV;
SFR.SPI1STAT(0x0);
SFR.SPI1BRG=function(num){
	if (num!=undefined) SFR.SPI1BRG.value=(SFR.SPI1BRG.value & 0xffffe000) + (num & 0x1fff);
	return SFR.SPI1BRG.value & 0x1fff;
};
SFR.SPI1BRGCLR=function(num){
	var val=SFR.SPI1BRG.value;
	SFR.SPI1BRG(val&(0xFFFFFFFF-num));
};
SFR.SPI1BRGSET=function(num){
	var val=SFR.SPI1BRG.value;
	SFR.SPI1BRG(val|num);
};
SFR.SPI1BRGINV=function(num){
	var val=SFR.SPI1BRG.value;
	SFR.SPI1BRG(val^num);
};
SFR[0xBF805830]=SFR.SPI1BRG;
SFR[0xBF805834]=SFR.SPI1BRGCLR;
SFR[0xBF805838]=SFR.SPI1BRGSET;
SFR[0xBF80583C]=SFR.SPI1BRGINV;
SFR.SPI1BRG(0x0);
SFR.SPI1CON2=function(num){
	if (num!=undefined) SFR.SPI1CON2.value=(SFR.SPI1CON2.value & 0xffff6074) + (num & 0x9f8b);
	return SFR.SPI1CON2.value & 0x9f8b;
};
SFR.SPI1CON2CLR=function(num){
	var val=SFR.SPI1CON2.value;
	SFR.SPI1CON2(val&(0xFFFFFFFF-num));
};
SFR.SPI1CON2SET=function(num){
	var val=SFR.SPI1CON2.value;
	SFR.SPI1CON2(val|num);
};
SFR.SPI1CON2INV=function(num){
	var val=SFR.SPI1CON2.value;
	SFR.SPI1CON2(val^num);
};
SFR[0xBF805840]=SFR.SPI1CON2;
SFR[0xBF805844]=SFR.SPI1CON2CLR;
SFR[0xBF805848]=SFR.SPI1CON2SET;
SFR[0xBF80584C]=SFR.SPI1CON2INV;
SFR.SPI1CON2(0x0);
SFR.SPI2CON=function(num){
	if (num!=undefined) SFR.SPI2CON.value=(SFR.SPI2CON.value & 0x7c0000) + (num & 0xff83ffff);
	return SFR.SPI2CON.value & 0xff83ffff;
};
SFR.SPI2CONCLR=function(num){
	var val=SFR.SPI2CON.value;
	SFR.SPI2CON(val&(0xFFFFFFFF-num));
};
SFR.SPI2CONSET=function(num){
	var val=SFR.SPI2CON.value;
	SFR.SPI2CON(val|num);
};
SFR.SPI2CONINV=function(num){
	var val=SFR.SPI2CON.value;
	SFR.SPI2CON(val^num);
};
SFR[0xBF805A00]=SFR.SPI2CON;
SFR[0xBF805A04]=SFR.SPI2CONCLR;
SFR[0xBF805A08]=SFR.SPI2CONSET;
SFR[0xBF805A0C]=SFR.SPI2CONINV;
SFR.SPI2CON(0x0);
SFR.SPI2STAT=function(num){
	if (num!=undefined) SFR.SPI2STAT.value=(SFR.SPI2STAT.value & 0xffffefff) + (num & 0x1000);
	return SFR.SPI2STAT.value & 0x1f1f19eb;
};
SFR.SPI2STATCLR=function(num){
	var val=SFR.SPI2STAT.value;
	SFR.SPI2STAT(val&(0xFFFFFFFF-num));
};
SFR.SPI2STATSET=function(num){
	var val=SFR.SPI2STAT.value;
	SFR.SPI2STAT(val|num);
};
SFR.SPI2STATINV=function(num){
	var val=SFR.SPI2STAT.value;
	SFR.SPI2STAT(val^num);
};
SFR[0xBF805A10]=SFR.SPI2STAT;
SFR[0xBF805A14]=SFR.SPI2STATCLR;
SFR[0xBF805A18]=SFR.SPI2STATSET;
SFR[0xBF805A1C]=SFR.SPI2STATINV;
SFR.SPI2STAT(0x0);
SFR.SPI2BRG=function(num){
	if (num!=undefined) SFR.SPI2BRG.value=(SFR.SPI2BRG.value & 0xffffe000) + (num & 0x1fff);
	return SFR.SPI2BRG.value & 0x1fff;
};
SFR.SPI2BRGCLR=function(num){
	var val=SFR.SPI2BRG.value;
	SFR.SPI2BRG(val&(0xFFFFFFFF-num));
};
SFR.SPI2BRGSET=function(num){
	var val=SFR.SPI2BRG.value;
	SFR.SPI2BRG(val|num);
};
SFR.SPI2BRGINV=function(num){
	var val=SFR.SPI2BRG.value;
	SFR.SPI2BRG(val^num);
};
SFR[0xBF805A30]=SFR.SPI2BRG;
SFR[0xBF805A34]=SFR.SPI2BRGCLR;
SFR[0xBF805A38]=SFR.SPI2BRGSET;
SFR[0xBF805A3C]=SFR.SPI2BRGINV;
SFR.SPI2BRG(0x0);
SFR.SPI2CON2=function(num){
	if (num!=undefined) SFR.SPI2CON2.value=(SFR.SPI2CON2.value & 0xffff6074) + (num & 0x9f8b);
	return SFR.SPI2CON2.value & 0x9f8b;
};
SFR.SPI2CON2CLR=function(num){
	var val=SFR.SPI2CON2.value;
	SFR.SPI2CON2(val&(0xFFFFFFFF-num));
};
SFR.SPI2CON2SET=function(num){
	var val=SFR.SPI2CON2.value;
	SFR.SPI2CON2(val|num);
};
SFR.SPI2CON2INV=function(num){
	var val=SFR.SPI2CON2.value;
	SFR.SPI2CON2(val^num);
};
SFR[0xBF805A40]=SFR.SPI2CON2;
SFR[0xBF805A44]=SFR.SPI2CON2CLR;
SFR[0xBF805A48]=SFR.SPI2CON2SET;
SFR[0xBF805A4C]=SFR.SPI2CON2INV;
SFR.SPI2CON2(0x0);
SFR.U1MODE=function(num){
	if (num!=undefined) SFR.U1MODE.value=(SFR.U1MODE.value & 0xffff4000) + (num & 0xbfff);
	return SFR.U1MODE.value & 0xbfff;
};
SFR.U1MODECLR=function(num){
	var val=SFR.U1MODE.value;
	SFR.U1MODE(val&(0xFFFFFFFF-num));
};
SFR.U1MODESET=function(num){
	var val=SFR.U1MODE.value;
	SFR.U1MODE(val|num);
};
SFR.U1MODEINV=function(num){
	var val=SFR.U1MODE.value;
	SFR.U1MODE(val^num);
};
SFR[0xBF806000]=SFR.U1MODE;
SFR[0xBF806004]=SFR.U1MODECLR;
SFR[0xBF806008]=SFR.U1MODESET;
SFR[0xBF80600C]=SFR.U1MODEINV;
SFR.U1MODE(0x0);
SFR.U1STA=function(num){
	if (num!=undefined) SFR.U1STA.value=(SFR.U1STA.value & 0xfe00031f) + (num & 0x1fffce0);
	return SFR.U1STA.value & 0x1ffffff;
};
SFR.U1STACLR=function(num){
	var val=SFR.U1STA.value;
	SFR.U1STA(val&(0xFFFFFFFF-num));
};
SFR.U1STASET=function(num){
	var val=SFR.U1STA.value;
	SFR.U1STA(val|num);
};
SFR.U1STAINV=function(num){
	var val=SFR.U1STA.value;
	SFR.U1STA(val^num);
};
SFR[0xBF806010]=SFR.U1STA;
SFR[0xBF806014]=SFR.U1STACLR;
SFR[0xBF806018]=SFR.U1STASET;
SFR[0xBF80601C]=SFR.U1STAINV;
SFR.U1STA(0x110);
SFR.U1BRG=function(num){
	if (num!=undefined) SFR.U1BRG.value=(SFR.U1BRG.value & 0xffff0000) + (num & 0xffff);
	return SFR.U1BRG.value & 0xffff;
};
SFR.U1BRGCLR=function(num){
	var val=SFR.U1BRG.value;
	SFR.U1BRG(val&(0xFFFFFFFF-num));
};
SFR.U1BRGSET=function(num){
	var val=SFR.U1BRG.value;
	SFR.U1BRG(val|num);
};
SFR.U1BRGINV=function(num){
	var val=SFR.U1BRG.value;
	SFR.U1BRG(val^num);
};
SFR[0xBF806040]=SFR.U1BRG;
SFR[0xBF806044]=SFR.U1BRGCLR;
SFR[0xBF806048]=SFR.U1BRGSET;
SFR[0xBF80604C]=SFR.U1BRGINV;
SFR.U1BRG(0x0);
SFR.U2MODE=function(num){
	if (num!=undefined) SFR.U2MODE.value=(SFR.U2MODE.value & 0xffff4000) + (num & 0xbfff);
	return SFR.U2MODE.value & 0xbfff;
};
SFR.U2MODECLR=function(num){
	var val=SFR.U2MODE.value;
	SFR.U2MODE(val&(0xFFFFFFFF-num));
};
SFR.U2MODESET=function(num){
	var val=SFR.U2MODE.value;
	SFR.U2MODE(val|num);
};
SFR.U2MODEINV=function(num){
	var val=SFR.U2MODE.value;
	SFR.U2MODE(val^num);
};
SFR[0xBF806200]=SFR.U2MODE;
SFR[0xBF806204]=SFR.U2MODECLR;
SFR[0xBF806208]=SFR.U2MODESET;
SFR[0xBF80620C]=SFR.U2MODEINV;
SFR.U2MODE(0x0);
SFR.U2STA=function(num){
	if (num!=undefined) SFR.U2STA.value=(SFR.U2STA.value & 0xfe00031f) + (num & 0x1fffce0);
	return SFR.U2STA.value & 0x1ffffff;
};
SFR.U2STACLR=function(num){
	var val=SFR.U2STA.value;
	SFR.U2STA(val&(0xFFFFFFFF-num));
};
SFR.U2STASET=function(num){
	var val=SFR.U2STA.value;
	SFR.U2STA(val|num);
};
SFR.U2STAINV=function(num){
	var val=SFR.U2STA.value;
	SFR.U2STA(val^num);
};
SFR[0xBF806210]=SFR.U2STA;
SFR[0xBF806214]=SFR.U2STACLR;
SFR[0xBF806218]=SFR.U2STASET;
SFR[0xBF80621C]=SFR.U2STAINV;
SFR.U2STA(0x110);
SFR.U2BRG=function(num){
	if (num!=undefined) SFR.U2BRG.value=(SFR.U2BRG.value & 0xffff0000) + (num & 0xffff);
	return SFR.U2BRG.value & 0xffff;
};
SFR.U2BRGCLR=function(num){
	var val=SFR.U2BRG.value;
	SFR.U2BRG(val&(0xFFFFFFFF-num));
};
SFR.U2BRGSET=function(num){
	var val=SFR.U2BRG.value;
	SFR.U2BRG(val|num);
};
SFR.U2BRGINV=function(num){
	var val=SFR.U2BRG.value;
	SFR.U2BRG(val^num);
};
SFR[0xBF806240]=SFR.U2BRG;
SFR[0xBF806244]=SFR.U2BRGCLR;
SFR[0xBF806248]=SFR.U2BRGSET;
SFR[0xBF80624C]=SFR.U2BRGINV;
SFR.U2BRG(0x0);
SFR.U3MODE=function(num){
	if (num!=undefined) SFR.U3MODE.value=(SFR.U3MODE.value & 0xffff4000) + (num & 0xbfff);
	return SFR.U3MODE.value & 0xbfff;
};
SFR.U3MODECLR=function(num){
	var val=SFR.U3MODE.value;
	SFR.U3MODE(val&(0xFFFFFFFF-num));
};
SFR.U3MODESET=function(num){
	var val=SFR.U3MODE.value;
	SFR.U3MODE(val|num);
};
SFR.U3MODEINV=function(num){
	var val=SFR.U3MODE.value;
	SFR.U3MODE(val^num);
};
SFR[0xBF806400]=SFR.U3MODE;
SFR[0xBF806404]=SFR.U3MODECLR;
SFR[0xBF806408]=SFR.U3MODESET;
SFR[0xBF80640C]=SFR.U3MODEINV;
SFR.U3MODE(0x0);
SFR.U3STA=function(num){
	if (num!=undefined) SFR.U3STA.value=(SFR.U3STA.value & 0xfe00031f) + (num & 0x1fffce0);
	return SFR.U3STA.value & 0x1ffffff;
};
SFR.U3STACLR=function(num){
	var val=SFR.U3STA.value;
	SFR.U3STA(val&(0xFFFFFFFF-num));
};
SFR.U3STASET=function(num){
	var val=SFR.U3STA.value;
	SFR.U3STA(val|num);
};
SFR.U3STAINV=function(num){
	var val=SFR.U3STA.value;
	SFR.U3STA(val^num);
};
SFR[0xBF806410]=SFR.U3STA;
SFR[0xBF806414]=SFR.U3STACLR;
SFR[0xBF806418]=SFR.U3STASET;
SFR[0xBF80641C]=SFR.U3STAINV;
SFR.U3STA(0x110);
SFR.U3BRG=function(num){
	if (num!=undefined) SFR.U3BRG.value=(SFR.U3BRG.value & 0xffff0000) + (num & 0xffff);
	return SFR.U3BRG.value & 0xffff;
};
SFR.U3BRGCLR=function(num){
	var val=SFR.U3BRG.value;
	SFR.U3BRG(val&(0xFFFFFFFF-num));
};
SFR.U3BRGSET=function(num){
	var val=SFR.U3BRG.value;
	SFR.U3BRG(val|num);
};
SFR.U3BRGINV=function(num){
	var val=SFR.U3BRG.value;
	SFR.U3BRG(val^num);
};
SFR[0xBF806440]=SFR.U3BRG;
SFR[0xBF806444]=SFR.U3BRGCLR;
SFR[0xBF806448]=SFR.U3BRGSET;
SFR[0xBF80644C]=SFR.U3BRGINV;
SFR.U3BRG(0x0);
SFR.U4MODE=function(num){
	if (num!=undefined) SFR.U4MODE.value=(SFR.U4MODE.value & 0xffff4000) + (num & 0xbfff);
	return SFR.U4MODE.value & 0xbfff;
};
SFR.U4MODECLR=function(num){
	var val=SFR.U4MODE.value;
	SFR.U4MODE(val&(0xFFFFFFFF-num));
};
SFR.U4MODESET=function(num){
	var val=SFR.U4MODE.value;
	SFR.U4MODE(val|num);
};
SFR.U4MODEINV=function(num){
	var val=SFR.U4MODE.value;
	SFR.U4MODE(val^num);
};
SFR[0xBF806600]=SFR.U4MODE;
SFR[0xBF806604]=SFR.U4MODECLR;
SFR[0xBF806608]=SFR.U4MODESET;
SFR[0xBF80660C]=SFR.U4MODEINV;
SFR.U4MODE(0x0);
SFR.U4STA=function(num){
	if (num!=undefined) SFR.U4STA.value=(SFR.U4STA.value & 0xfe00031f) + (num & 0x1fffce0);
	return SFR.U4STA.value & 0x1ffffff;
};
SFR.U4STACLR=function(num){
	var val=SFR.U4STA.value;
	SFR.U4STA(val&(0xFFFFFFFF-num));
};
SFR.U4STASET=function(num){
	var val=SFR.U4STA.value;
	SFR.U4STA(val|num);
};
SFR.U4STAINV=function(num){
	var val=SFR.U4STA.value;
	SFR.U4STA(val^num);
};
SFR[0xBF806610]=SFR.U4STA;
SFR[0xBF806614]=SFR.U4STACLR;
SFR[0xBF806618]=SFR.U4STASET;
SFR[0xBF80661C]=SFR.U4STAINV;
SFR.U4STA(0x110);
SFR.U4BRG=function(num){
	if (num!=undefined) SFR.U4BRG.value=(SFR.U4BRG.value & 0xffff0000) + (num & 0xffff);
	return SFR.U4BRG.value & 0xffff;
};
SFR.U4BRGCLR=function(num){
	var val=SFR.U4BRG.value;
	SFR.U4BRG(val&(0xFFFFFFFF-num));
};
SFR.U4BRGSET=function(num){
	var val=SFR.U4BRG.value;
	SFR.U4BRG(val|num);
};
SFR.U4BRGINV=function(num){
	var val=SFR.U4BRG.value;
	SFR.U4BRG(val^num);
};
SFR[0xBF806640]=SFR.U4BRG;
SFR[0xBF806644]=SFR.U4BRGCLR;
SFR[0xBF806648]=SFR.U4BRGSET;
SFR[0xBF80664C]=SFR.U4BRGINV;
SFR.U4BRG(0x0);
SFR.PMCON=function(num){
	if (num!=undefined) SFR.PMCON.value=(SFR.PMCON.value & 0xffff4004) + (num & 0xbffb);
	return SFR.PMCON.value & 0xbffb;
};
SFR.PMCONCLR=function(num){
	var val=SFR.PMCON.value;
	SFR.PMCON(val&(0xFFFFFFFF-num));
};
SFR.PMCONSET=function(num){
	var val=SFR.PMCON.value;
	SFR.PMCON(val|num);
};
SFR.PMCONINV=function(num){
	var val=SFR.PMCON.value;
	SFR.PMCON(val^num);
};
SFR[0xBF807000]=SFR.PMCON;
SFR[0xBF807004]=SFR.PMCONCLR;
SFR[0xBF807008]=SFR.PMCONSET;
SFR[0xBF80700C]=SFR.PMCONINV;
SFR.PMCON(0x0);
SFR.PMMODE=function(num){
	if (num!=undefined) SFR.PMMODE.value=(SFR.PMMODE.value & 0xffff0000) + (num & 0xffff);
	return SFR.PMMODE.value & 0xffff;
};
SFR.PMMODECLR=function(num){
	var val=SFR.PMMODE.value;
	SFR.PMMODE(val&(0xFFFFFFFF-num));
};
SFR.PMMODESET=function(num){
	var val=SFR.PMMODE.value;
	SFR.PMMODE(val|num);
};
SFR.PMMODEINV=function(num){
	var val=SFR.PMMODE.value;
	SFR.PMMODE(val^num);
};
SFR[0xBF807010]=SFR.PMMODE;
SFR[0xBF807014]=SFR.PMMODECLR;
SFR[0xBF807018]=SFR.PMMODESET;
SFR[0xBF80701C]=SFR.PMMODEINV;
SFR.PMMODE(0x0);
SFR.PMADDR=function(num){
	if (num!=undefined) SFR.PMADDR.value=(SFR.PMADDR.value & 0xffff0000) + (num & 0xffff);
	return SFR.PMADDR.value & 0xffff;
};
SFR.PMADDRCLR=function(num){
	var val=SFR.PMADDR.value;
	SFR.PMADDR(val&(0xFFFFFFFF-num));
};
SFR.PMADDRSET=function(num){
	var val=SFR.PMADDR.value;
	SFR.PMADDR(val|num);
};
SFR.PMADDRINV=function(num){
	var val=SFR.PMADDR.value;
	SFR.PMADDR(val^num);
};
SFR[0xBF807020]=SFR.PMADDR;
SFR[0xBF807024]=SFR.PMADDRCLR;
SFR[0xBF807028]=SFR.PMADDRSET;
SFR[0xBF80702C]=SFR.PMADDRINV;
SFR.PMADDR(0x0);
SFR.PMDOUT=function(num){
	if (num!=undefined) SFR.PMDOUT.value=(SFR.PMDOUT.value & 0x0) + (num & 0xffffffff);
	return SFR.PMDOUT.value & 0xffffffff;
};
SFR.PMDOUTCLR=function(num){
	var val=SFR.PMDOUT.value;
	SFR.PMDOUT(val&(0xFFFFFFFF-num));
};
SFR.PMDOUTSET=function(num){
	var val=SFR.PMDOUT.value;
	SFR.PMDOUT(val|num);
};
SFR.PMDOUTINV=function(num){
	var val=SFR.PMDOUT.value;
	SFR.PMDOUT(val^num);
};
SFR[0xBF807030]=SFR.PMDOUT;
SFR[0xBF807034]=SFR.PMDOUTCLR;
SFR[0xBF807038]=SFR.PMDOUTSET;
SFR[0xBF80703C]=SFR.PMDOUTINV;
SFR.PMDOUT(0x0);
SFR.PMDIN=function(num){
	if (num!=undefined) SFR.PMDIN.value=(SFR.PMDIN.value & 0x0) + (num & 0xffffffff);
	return SFR.PMDIN.value & 0xffffffff;
};
SFR.PMDINCLR=function(num){
	var val=SFR.PMDIN.value;
	SFR.PMDIN(val&(0xFFFFFFFF-num));
};
SFR.PMDINSET=function(num){
	var val=SFR.PMDIN.value;
	SFR.PMDIN(val|num);
};
SFR.PMDININV=function(num){
	var val=SFR.PMDIN.value;
	SFR.PMDIN(val^num);
};
SFR[0xBF807040]=SFR.PMDIN;
SFR[0xBF807044]=SFR.PMDINCLR;
SFR[0xBF807048]=SFR.PMDINSET;
SFR[0xBF80704C]=SFR.PMDININV;
SFR.PMDIN(0x0);
SFR.PMAEN=function(num){
	if (num!=undefined) SFR.PMAEN.value=(SFR.PMAEN.value & 0xffff0000) + (num & 0xffff);
	return SFR.PMAEN.value & 0xffff;
};
SFR.PMAENCLR=function(num){
	var val=SFR.PMAEN.value;
	SFR.PMAEN(val&(0xFFFFFFFF-num));
};
SFR.PMAENSET=function(num){
	var val=SFR.PMAEN.value;
	SFR.PMAEN(val|num);
};
SFR.PMAENINV=function(num){
	var val=SFR.PMAEN.value;
	SFR.PMAEN(val^num);
};
SFR[0xBF807050]=SFR.PMAEN;
SFR[0xBF807054]=SFR.PMAENCLR;
SFR[0xBF807058]=SFR.PMAENSET;
SFR[0xBF80705C]=SFR.PMAENINV;
SFR.PMAEN(0x0);
SFR.PMSTAT=function(num){
	if (num!=undefined) SFR.PMSTAT.value=(SFR.PMSTAT.value & 0xffffbfbf) + (num & 0x4040);
	return SFR.PMSTAT.value & 0xcfcf;
};
SFR.PMSTATCLR=function(num){
	var val=SFR.PMSTAT.value;
	SFR.PMSTAT(val&(0xFFFFFFFF-num));
};
SFR.PMSTATSET=function(num){
	var val=SFR.PMSTAT.value;
	SFR.PMSTAT(val|num);
};
SFR.PMSTATINV=function(num){
	var val=SFR.PMSTAT.value;
	SFR.PMSTAT(val^num);
};
SFR[0xBF807060]=SFR.PMSTAT;
SFR[0xBF807064]=SFR.PMSTATCLR;
SFR[0xBF807068]=SFR.PMSTATSET;
SFR[0xBF80706C]=SFR.PMSTATINV;
SFR.PMSTAT(0x8f);
SFR.AD1CON1=function(num){
	if (num!=undefined) SFR.AD1CON1.value=(SFR.AD1CON1.value & 0xffff5809) + (num & 0xa7f6);
	return SFR.AD1CON1.value & 0xa7f7;
};
SFR.AD1CON1CLR=function(num){
	var val=SFR.AD1CON1.value;
	SFR.AD1CON1(val&(0xFFFFFFFF-num));
};
SFR.AD1CON1SET=function(num){
	var val=SFR.AD1CON1.value;
	SFR.AD1CON1(val|num);
};
SFR.AD1CON1INV=function(num){
	var val=SFR.AD1CON1.value;
	SFR.AD1CON1(val^num);
};
SFR[0xBF809000]=SFR.AD1CON1;
SFR[0xBF809004]=SFR.AD1CON1CLR;
SFR[0xBF809008]=SFR.AD1CON1SET;
SFR[0xBF80900C]=SFR.AD1CON1INV;
SFR.AD1CON1(0x0);
SFR.AD1CON2=function(num){
	if (num!=undefined) SFR.AD1CON2.value=(SFR.AD1CON2.value & 0xffff0bc0) + (num & 0xf43f);
	return SFR.AD1CON2.value & 0xf4bf;
};
SFR.AD1CON2CLR=function(num){
	var val=SFR.AD1CON2.value;
	SFR.AD1CON2(val&(0xFFFFFFFF-num));
};
SFR.AD1CON2SET=function(num){
	var val=SFR.AD1CON2.value;
	SFR.AD1CON2(val|num);
};
SFR.AD1CON2INV=function(num){
	var val=SFR.AD1CON2.value;
	SFR.AD1CON2(val^num);
};
SFR[0xBF809010]=SFR.AD1CON2;
SFR[0xBF809014]=SFR.AD1CON2CLR;
SFR[0xBF809018]=SFR.AD1CON2SET;
SFR[0xBF80901C]=SFR.AD1CON2INV;
SFR.AD1CON2(0x0);
SFR.AD1CON3=function(num){
	if (num!=undefined) SFR.AD1CON3.value=(SFR.AD1CON3.value & 0xffff6000) + (num & 0x9fff);
	return SFR.AD1CON3.value & 0x9fff;
};
SFR.AD1CON3CLR=function(num){
	var val=SFR.AD1CON3.value;
	SFR.AD1CON3(val&(0xFFFFFFFF-num));
};
SFR.AD1CON3SET=function(num){
	var val=SFR.AD1CON3.value;
	SFR.AD1CON3(val|num);
};
SFR.AD1CON3INV=function(num){
	var val=SFR.AD1CON3.value;
	SFR.AD1CON3(val^num);
};
SFR[0xBF809020]=SFR.AD1CON3;
SFR[0xBF809024]=SFR.AD1CON3CLR;
SFR[0xBF809028]=SFR.AD1CON3SET;
SFR[0xBF80902C]=SFR.AD1CON3INV;
SFR.AD1CON3(0x0);
SFR.AD1CHS=function(num){
	if (num!=undefined) SFR.AD1CHS.value=(SFR.AD1CHS.value & 0x7070ffff) + (num & 0x8f8f0000);
	return SFR.AD1CHS.value & 0x8f8f0000;
};
SFR.AD1CHSCLR=function(num){
	var val=SFR.AD1CHS.value;
	SFR.AD1CHS(val&(0xFFFFFFFF-num));
};
SFR.AD1CHSSET=function(num){
	var val=SFR.AD1CHS.value;
	SFR.AD1CHS(val|num);
};
SFR.AD1CHSINV=function(num){
	var val=SFR.AD1CHS.value;
	SFR.AD1CHS(val^num);
};
SFR[0xBF809040]=SFR.AD1CHS;
SFR[0xBF809044]=SFR.AD1CHSCLR;
SFR[0xBF809048]=SFR.AD1CHSSET;
SFR[0xBF80904C]=SFR.AD1CHSINV;
SFR.AD1CHS(0x0);
SFR.AD1CSSL=function(num){
	if (num!=undefined) SFR.AD1CSSL.value=(SFR.AD1CSSL.value & 0xffff0000) + (num & 0xffff);
	return SFR.AD1CSSL.value & 0xffff;
};
SFR.AD1CSSLCLR=function(num){
	var val=SFR.AD1CSSL.value;
	SFR.AD1CSSL(val&(0xFFFFFFFF-num));
};
SFR.AD1CSSLSET=function(num){
	var val=SFR.AD1CSSL.value;
	SFR.AD1CSSL(val|num);
};
SFR.AD1CSSLINV=function(num){
	var val=SFR.AD1CSSL.value;
	SFR.AD1CSSL(val^num);
};
SFR[0xBF809050]=SFR.AD1CSSL;
SFR[0xBF809054]=SFR.AD1CSSLCLR;
SFR[0xBF809058]=SFR.AD1CSSLSET;
SFR[0xBF80905C]=SFR.AD1CSSLINV;
SFR.AD1CSSL(0x0);
SFR.CVRCON=function(num){
	if (num!=undefined) SFR.CVRCON.value=(SFR.CVRCON.value & 0xffff7880) + (num & 0x877f);
	return SFR.CVRCON.value & 0x877f;
};
SFR.CVRCONCLR=function(num){
	var val=SFR.CVRCON.value;
	SFR.CVRCON(val&(0xFFFFFFFF-num));
};
SFR.CVRCONSET=function(num){
	var val=SFR.CVRCON.value;
	SFR.CVRCON(val|num);
};
SFR.CVRCONINV=function(num){
	var val=SFR.CVRCON.value;
	SFR.CVRCON(val^num);
};
SFR[0xBF809800]=SFR.CVRCON;
SFR[0xBF809804]=SFR.CVRCONCLR;
SFR[0xBF809808]=SFR.CVRCONSET;
SFR[0xBF80980C]=SFR.CVRCONINV;
SFR.CVRCON(0x100);
SFR.CM1CON=function(num){
	if (num!=undefined) SFR.CM1CON.value=(SFR.CM1CON.value & 0xffff0e2c) + (num & 0xf1d3);
	return SFR.CM1CON.value & 0xf1d3;
};
SFR.CM1CONCLR=function(num){
	var val=SFR.CM1CON.value;
	SFR.CM1CON(val&(0xFFFFFFFF-num));
};
SFR.CM1CONSET=function(num){
	var val=SFR.CM1CON.value;
	SFR.CM1CON(val|num);
};
SFR.CM1CONINV=function(num){
	var val=SFR.CM1CON.value;
	SFR.CM1CON(val^num);
};
SFR[0xBF80A000]=SFR.CM1CON;
SFR[0xBF80A004]=SFR.CM1CONCLR;
SFR[0xBF80A008]=SFR.CM1CONSET;
SFR[0xBF80A00C]=SFR.CM1CONINV;
SFR.CM1CON(0xc3);
SFR.CM2CON=function(num){
	if (num!=undefined) SFR.CM2CON.value=(SFR.CM2CON.value & 0xffff0e2c) + (num & 0xf1d3);
	return SFR.CM2CON.value & 0xf1d3;
};
SFR.CM2CONCLR=function(num){
	var val=SFR.CM2CON.value;
	SFR.CM2CON(val&(0xFFFFFFFF-num));
};
SFR.CM2CONSET=function(num){
	var val=SFR.CM2CON.value;
	SFR.CM2CON(val|num);
};
SFR.CM2CONINV=function(num){
	var val=SFR.CM2CON.value;
	SFR.CM2CON(val^num);
};
SFR[0xBF80A010]=SFR.CM2CON;
SFR[0xBF80A014]=SFR.CM2CONCLR;
SFR[0xBF80A018]=SFR.CM2CONSET;
SFR[0xBF80A01C]=SFR.CM2CONINV;
SFR.CM2CON(0xc3);
SFR.CMSTAT=function(num){
	if (num!=undefined) SFR.CMSTAT.value=(SFR.CMSTAT.value & 0xffffdfc3) + (num & 0x203c);
	return SFR.CMSTAT.value & 0x203f;
};
SFR.CMSTATCLR=function(num){
	var val=SFR.CMSTAT.value;
	SFR.CMSTAT(val&(0xFFFFFFFF-num));
};
SFR.CMSTATSET=function(num){
	var val=SFR.CMSTAT.value;
	SFR.CMSTAT(val|num);
};
SFR.CMSTATINV=function(num){
	var val=SFR.CMSTAT.value;
	SFR.CMSTAT(val^num);
};
SFR[0xBF80A060]=SFR.CMSTAT;
SFR[0xBF80A064]=SFR.CMSTATCLR;
SFR[0xBF80A068]=SFR.CMSTATSET;
SFR[0xBF80A06C]=SFR.CMSTATINV;
SFR.CMSTAT(0x0);
SFR.CTMUCON=function(num){
	if (num!=undefined) SFR.CTMUCON.value=(SFR.CTMUCON.value & 0x30000) + (num & 0xfffcffff);
	return SFR.CTMUCON.value & 0xfffcffff;
};
SFR.CTMUCONCLR=function(num){
	var val=SFR.CTMUCON.value;
	SFR.CTMUCON(val&(0xFFFFFFFF-num));
};
SFR.CTMUCONSET=function(num){
	var val=SFR.CTMUCON.value;
	SFR.CTMUCON(val|num);
};
SFR.CTMUCONINV=function(num){
	var val=SFR.CTMUCON.value;
	SFR.CTMUCON(val^num);
};
SFR[0xBF80A200]=SFR.CTMUCON;
SFR[0xBF80A204]=SFR.CTMUCONCLR;
SFR[0xBF80A208]=SFR.CTMUCONSET;
SFR[0xBF80A20C]=SFR.CTMUCONINV;
SFR.CTMUCON(0x0);
SFR.OSCCON=function(num){
	if (num!=undefined) SFR.OSCCON.value=(SFR.OSCCON.value & 0xc0008800) + (num & 0x3fff77ff);
	return SFR.OSCCON.value & 0x3fff77ff;
};
SFR.OSCCONCLR=function(num){
	var val=SFR.OSCCON.value;
	SFR.OSCCON(val&(0xFFFFFFFF-num));
};
SFR.OSCCONSET=function(num){
	var val=SFR.OSCCON.value;
	SFR.OSCCON(val|num);
};
SFR.OSCCONINV=function(num){
	var val=SFR.OSCCON.value;
	SFR.OSCCON(val^num);
};
SFR[0xBF80F000]=SFR.OSCCON;
SFR[0xBF80F004]=SFR.OSCCONCLR;
SFR[0xBF80F008]=SFR.OSCCONSET;
SFR[0xBF80F00C]=SFR.OSCCONINV;
SFR.OSCCON(0x200007);
SFR.OSCTUN=function(num){
	if (num!=undefined) SFR.OSCTUN.value=(SFR.OSCTUN.value & 0xffffffc0) + (num & 0x3f);
	return SFR.OSCTUN.value & 0x3f;
};
SFR.OSCTUNCLR=function(num){
	var val=SFR.OSCTUN.value;
	SFR.OSCTUN(val&(0xFFFFFFFF-num));
};
SFR.OSCTUNSET=function(num){
	var val=SFR.OSCTUN.value;
	SFR.OSCTUN(val|num);
};
SFR.OSCTUNINV=function(num){
	var val=SFR.OSCTUN.value;
	SFR.OSCTUN(val^num);
};
SFR[0xBF80F010]=SFR.OSCTUN;
SFR[0xBF80F014]=SFR.OSCTUNCLR;
SFR[0xBF80F018]=SFR.OSCTUNSET;
SFR[0xBF80F01C]=SFR.OSCTUNINV;
SFR.OSCTUN(0x0);
SFR.REFOCON=function(num){
	if (num!=undefined) SFR.REFOCON.value=(SFR.REFOCON.value & 0x80000070) + (num & 0x7fffff8f);
	return SFR.REFOCON.value & 0x7fffff8f;
};
SFR.REFOCONCLR=function(num){
	var val=SFR.REFOCON.value;
	SFR.REFOCON(val&(0xFFFFFFFF-num));
};
SFR.REFOCONSET=function(num){
	var val=SFR.REFOCON.value;
	SFR.REFOCON(val|num);
};
SFR.REFOCONINV=function(num){
	var val=SFR.REFOCON.value;
	SFR.REFOCON(val^num);
};
SFR[0xBF80F020]=SFR.REFOCON;
SFR[0xBF80F024]=SFR.REFOCONCLR;
SFR[0xBF80F028]=SFR.REFOCONSET;
SFR[0xBF80F02C]=SFR.REFOCONINV;
SFR.REFOCON(0x0);
SFR.REFOTRIM=function(num){
	if (num!=undefined) SFR.REFOTRIM.value=(SFR.REFOTRIM.value & 0x7fffff) + (num & 0xff800000);
	return SFR.REFOTRIM.value & 0xff800000;
};
SFR.REFOTRIMCLR=function(num){
	var val=SFR.REFOTRIM.value;
	SFR.REFOTRIM(val&(0xFFFFFFFF-num));
};
SFR.REFOTRIMSET=function(num){
	var val=SFR.REFOTRIM.value;
	SFR.REFOTRIM(val|num);
};
SFR.REFOTRIMINV=function(num){
	var val=SFR.REFOTRIM.value;
	SFR.REFOTRIM(val^num);
};
SFR[0xBF80F030]=SFR.REFOTRIM;
SFR[0xBF80F034]=SFR.REFOTRIMCLR;
SFR[0xBF80F038]=SFR.REFOTRIMSET;
SFR[0xBF80F03C]=SFR.REFOTRIMINV;
SFR.REFOTRIM(0x0);
SFR.SYSKEY=function(num){
	if (num!=undefined) SFR.SYSKEY.value=(SFR.SYSKEY.value & 0xffffffff) + (num & 0x0);
	return SFR.SYSKEY.value & 0xfffffffd;
};
SFR.SYSKEYCLR=function(num){
	var val=SFR.SYSKEY.value;
	SFR.SYSKEY(val&(0xFFFFFFFF-num));
};
SFR.SYSKEYSET=function(num){
	var val=SFR.SYSKEY.value;
	SFR.SYSKEY(val|num);
};
SFR.SYSKEYINV=function(num){
	var val=SFR.SYSKEY.value;
	SFR.SYSKEY(val^num);
};
SFR[0xBF80F230]=SFR.SYSKEY;
SFR[0xBF80F234]=SFR.SYSKEYCLR;
SFR[0xBF80F238]=SFR.SYSKEYSET;
SFR[0xBF80F23C]=SFR.SYSKEYINV;
SFR.SYSKEY(0x0);
SFR.PMD1=function(num){
	if (num!=undefined) SFR.PMD1.value=(SFR.PMD1.value & 0xffffeefe) + (num & 0x1101);
	return SFR.PMD1.value & 0x1101;
};
SFR.PMD1CLR=function(num){
	var val=SFR.PMD1.value;
	SFR.PMD1(val&(0xFFFFFFFF-num));
};
SFR.PMD1SET=function(num){
	var val=SFR.PMD1.value;
	SFR.PMD1(val|num);
};
SFR.PMD1INV=function(num){
	var val=SFR.PMD1.value;
	SFR.PMD1(val^num);
};
SFR[0xBF80F240]=SFR.PMD1;
SFR[0xBF80F244]=SFR.PMD1CLR;
SFR[0xBF80F248]=SFR.PMD1SET;
SFR[0xBF80F24C]=SFR.PMD1INV;
SFR.PMD1(0x0);
SFR.PMD2=function(num){
	if (num!=undefined) SFR.PMD2.value=(SFR.PMD2.value & 0xfffffffc) + (num & 0x3);
	return SFR.PMD2.value & 0x3;
};
SFR.PMD2CLR=function(num){
	var val=SFR.PMD2.value;
	SFR.PMD2(val&(0xFFFFFFFF-num));
};
SFR.PMD2SET=function(num){
	var val=SFR.PMD2.value;
	SFR.PMD2(val|num);
};
SFR.PMD2INV=function(num){
	var val=SFR.PMD2.value;
	SFR.PMD2(val^num);
};
SFR[0xBF80F250]=SFR.PMD2;
SFR[0xBF80F254]=SFR.PMD2CLR;
SFR[0xBF80F258]=SFR.PMD2SET;
SFR[0xBF80F25C]=SFR.PMD2INV;
SFR.PMD2(0x0);
SFR.PMD3=function(num){
	if (num!=undefined) SFR.PMD3.value=(SFR.PMD3.value & 0xffe0ffe0) + (num & 0x1f001f);
	return SFR.PMD3.value & 0x1f001f;
};
SFR.PMD3CLR=function(num){
	var val=SFR.PMD3.value;
	SFR.PMD3(val&(0xFFFFFFFF-num));
};
SFR.PMD3SET=function(num){
	var val=SFR.PMD3.value;
	SFR.PMD3(val|num);
};
SFR.PMD3INV=function(num){
	var val=SFR.PMD3.value;
	SFR.PMD3(val^num);
};
SFR[0xBF80F260]=SFR.PMD3;
SFR[0xBF80F264]=SFR.PMD3CLR;
SFR[0xBF80F268]=SFR.PMD3SET;
SFR[0xBF80F26C]=SFR.PMD3INV;
SFR.PMD3(0x0);
SFR.PMD4=function(num){
	if (num!=undefined) SFR.PMD4.value=(SFR.PMD4.value & 0xffffffe0) + (num & 0x1f);
	return SFR.PMD4.value & 0x1f;
};
SFR.PMD4CLR=function(num){
	var val=SFR.PMD4.value;
	SFR.PMD4(val&(0xFFFFFFFF-num));
};
SFR.PMD4SET=function(num){
	var val=SFR.PMD4.value;
	SFR.PMD4(val|num);
};
SFR.PMD4INV=function(num){
	var val=SFR.PMD4.value;
	SFR.PMD4(val^num);
};
SFR[0xBF80F270]=SFR.PMD4;
SFR[0xBF80F274]=SFR.PMD4CLR;
SFR[0xBF80F278]=SFR.PMD4SET;
SFR[0xBF80F27C]=SFR.PMD4INV;
SFR.PMD4(0x0);
SFR.PMD5=function(num){
	if (num!=undefined) SFR.PMD5.value=(SFR.PMD5.value & 0xfffcfcf0) + (num & 0x3030f);
	return SFR.PMD5.value & 0x3030f;
};
SFR.PMD5CLR=function(num){
	var val=SFR.PMD5.value;
	SFR.PMD5(val&(0xFFFFFFFF-num));
};
SFR.PMD5SET=function(num){
	var val=SFR.PMD5.value;
	SFR.PMD5(val|num);
};
SFR.PMD5INV=function(num){
	var val=SFR.PMD5.value;
	SFR.PMD5(val^num);
};
SFR[0xBF80F280]=SFR.PMD5;
SFR[0xBF80F284]=SFR.PMD5CLR;
SFR[0xBF80F288]=SFR.PMD5SET;
SFR[0xBF80F28C]=SFR.PMD5INV;
SFR.PMD5(0x0);
SFR.PMD6=function(num){
	if (num!=undefined) SFR.PMD6.value=(SFR.PMD6.value & 0xfffefffc) + (num & 0x10003);
	return SFR.PMD6.value & 0x10003;
};
SFR.PMD6CLR=function(num){
	var val=SFR.PMD6.value;
	SFR.PMD6(val&(0xFFFFFFFF-num));
};
SFR.PMD6SET=function(num){
	var val=SFR.PMD6.value;
	SFR.PMD6(val|num);
};
SFR.PMD6INV=function(num){
	var val=SFR.PMD6.value;
	SFR.PMD6(val^num);
};
SFR[0xBF80F290]=SFR.PMD6;
SFR[0xBF80F294]=SFR.PMD6CLR;
SFR[0xBF80F298]=SFR.PMD6SET;
SFR[0xBF80F29C]=SFR.PMD6INV;
SFR.PMD6(0x0);
SFR.NVMCON=function(num){
	if (num!=undefined) SFR.NVMCON.value=(SFR.NVMCON.value & 0xffff07f0) + (num & 0xf80f);
	return SFR.NVMCON.value & 0xf80f;
};
SFR.NVMCONCLR=function(num){
	var val=SFR.NVMCON.value;
	SFR.NVMCON(val&(0xFFFFFFFF-num));
};
SFR.NVMCONSET=function(num){
	var val=SFR.NVMCON.value;
	SFR.NVMCON(val|num);
};
SFR.NVMCONINV=function(num){
	var val=SFR.NVMCON.value;
	SFR.NVMCON(val^num);
};
SFR[0xBF80F400]=SFR.NVMCON;
SFR[0xBF80F404]=SFR.NVMCONCLR;
SFR[0xBF80F408]=SFR.NVMCONSET;
SFR[0xBF80F40C]=SFR.NVMCONINV;
SFR.NVMCON(0x0);
SFR.NVMADDR=function(num){
	if (num!=undefined) SFR.NVMADDR.value=(SFR.NVMADDR.value & 0x0) + (num & 0xffffffff);
	return SFR.NVMADDR.value & 0xffffffff;
};
SFR.NVMADDRCLR=function(num){
	var val=SFR.NVMADDR.value;
	SFR.NVMADDR(val&(0xFFFFFFFF-num));
};
SFR.NVMADDRSET=function(num){
	var val=SFR.NVMADDR.value;
	SFR.NVMADDR(val|num);
};
SFR.NVMADDRINV=function(num){
	var val=SFR.NVMADDR.value;
	SFR.NVMADDR(val^num);
};
SFR[0xBF80F420]=SFR.NVMADDR;
SFR[0xBF80F424]=SFR.NVMADDRCLR;
SFR[0xBF80F428]=SFR.NVMADDRSET;
SFR[0xBF80F42C]=SFR.NVMADDRINV;
SFR.NVMADDR(0x0);
SFR.RCON=function(num){
	if (num!=undefined) SFR.RCON.value=(SFR.RCON.value & 0xdffffc20) + (num & 0x200003df);
	return SFR.RCON.value & 0x200003df;
};
SFR.RCONCLR=function(num){
	var val=SFR.RCON.value;
	SFR.RCON(val&(0xFFFFFFFF-num));
};
SFR.RCONSET=function(num){
	var val=SFR.RCON.value;
	SFR.RCON(val|num);
};
SFR.RCONINV=function(num){
	var val=SFR.RCON.value;
	SFR.RCON(val^num);
};
SFR[0xBF80F600]=SFR.RCON;
SFR[0xBF80F604]=SFR.RCONCLR;
SFR[0xBF80F608]=SFR.RCONSET;
SFR[0xBF80F60C]=SFR.RCONINV;
SFR.RCON(0x3);
SFR.RSWRST=function(num){
	if (num!=undefined) SFR.RSWRST.value=(SFR.RSWRST.value & 0xfffffffe) + (num & 0x1);
	return SFR.RSWRST.value & 0x1;
};
SFR.RSWRSTCLR=function(num){
	var val=SFR.RSWRST.value;
	SFR.RSWRST(val&(0xFFFFFFFF-num));
};
SFR.RSWRSTSET=function(num){
	var val=SFR.RSWRST.value;
	SFR.RSWRST(val|num);
};
SFR.RSWRSTINV=function(num){
	var val=SFR.RSWRST.value;
	SFR.RSWRST(val^num);
};
SFR[0xBF80F610]=SFR.RSWRST;
SFR[0xBF80F614]=SFR.RSWRSTCLR;
SFR[0xBF80F618]=SFR.RSWRSTSET;
SFR[0xBF80F61C]=SFR.RSWRSTINV;
SFR.RSWRST(0x0);
SFR.INTCON=function(num){
	if (num!=undefined) SFR.INTCON.value=(SFR.INTCON.value & 0xfffee8e0) + (num & 0x1171f);
	return SFR.INTCON.value & 0x1171f;
};
SFR.INTCONCLR=function(num){
	var val=SFR.INTCON.value;
	SFR.INTCON(val&(0xFFFFFFFF-num));
};
SFR.INTCONSET=function(num){
	var val=SFR.INTCON.value;
	SFR.INTCON(val|num);
};
SFR.INTCONINV=function(num){
	var val=SFR.INTCON.value;
	SFR.INTCON(val^num);
};
SFR[0xBF881000]=SFR.INTCON;
SFR[0xBF881004]=SFR.INTCONCLR;
SFR[0xBF881008]=SFR.INTCONSET;
SFR[0xBF88100C]=SFR.INTCONINV;
SFR.INTCON(0x0);
SFR.IPTMR=function(num){
	if (num!=undefined) SFR.IPTMR.value=(SFR.IPTMR.value & 0x0) + (num & 0xffffffff);
	return SFR.IPTMR.value & 0xffffffff;
};
SFR.IPTMRCLR=function(num){
	var val=SFR.IPTMR.value;
	SFR.IPTMR(val&(0xFFFFFFFF-num));
};
SFR.IPTMRSET=function(num){
	var val=SFR.IPTMR.value;
	SFR.IPTMR(val|num);
};
SFR.IPTMRINV=function(num){
	var val=SFR.IPTMR.value;
	SFR.IPTMR(val^num);
};
SFR[0xBF881020]=SFR.IPTMR;
SFR[0xBF881024]=SFR.IPTMRCLR;
SFR[0xBF881028]=SFR.IPTMRSET;
SFR[0xBF88102C]=SFR.IPTMRINV;
SFR.IPTMR(0x0);
SFR.IFS0=function(num){
	if (num!=undefined) SFR.IFS0.value=(SFR.IFS0.value & 0x0) + (num & 0xffffffff);
	return SFR.IFS0.value & 0xffffffff;
};
SFR.IFS0CLR=function(num){
	var val=SFR.IFS0.value;
	SFR.IFS0(val&(0xFFFFFFFF-num));
};
SFR.IFS0SET=function(num){
	var val=SFR.IFS0.value;
	SFR.IFS0(val|num);
};
SFR.IFS0INV=function(num){
	var val=SFR.IFS0.value;
	SFR.IFS0(val^num);
};
SFR[0xBF881030]=SFR.IFS0;
SFR[0xBF881034]=SFR.IFS0CLR;
SFR[0xBF881038]=SFR.IFS0SET;
SFR[0xBF88103C]=SFR.IFS0INV;
SFR.IFS0(0x0);
SFR.IFS1=function(num){
	if (num!=undefined) SFR.IFS1.value=(SFR.IFS1.value & 0x4) + (num & 0xfffffffb);
	return SFR.IFS1.value & 0xfffffffb;
};
SFR.IFS1CLR=function(num){
	var val=SFR.IFS1.value;
	SFR.IFS1(val&(0xFFFFFFFF-num));
};
SFR.IFS1SET=function(num){
	var val=SFR.IFS1.value;
	SFR.IFS1(val|num);
};
SFR.IFS1INV=function(num){
	var val=SFR.IFS1.value;
	SFR.IFS1(val^num);
};
SFR[0xBF881040]=SFR.IFS1;
SFR[0xBF881044]=SFR.IFS1CLR;
SFR[0xBF881048]=SFR.IFS1SET;
SFR[0xBF88104C]=SFR.IFS1INV;
SFR.IFS1(0x0);
SFR.IFS2=function(num){
	if (num!=undefined) SFR.IFS2.value=(SFR.IFS2.value & 0x7ffff070) + (num & 0x80000f8f);
	return SFR.IFS2.value & 0x80000f8f;
};
SFR.IFS2CLR=function(num){
	var val=SFR.IFS2.value;
	SFR.IFS2(val&(0xFFFFFFFF-num));
};
SFR.IFS2SET=function(num){
	var val=SFR.IFS2.value;
	SFR.IFS2(val|num);
};
SFR.IFS2INV=function(num){
	var val=SFR.IFS2.value;
	SFR.IFS2(val^num);
};
SFR[0xBF881050]=SFR.IFS2;
SFR[0xBF881054]=SFR.IFS2CLR;
SFR[0xBF881058]=SFR.IFS2SET;
SFR[0xBF88105C]=SFR.IFS2INV;
SFR.IFS2(0x0);
SFR.IEC0=function(num){
	if (num!=undefined) SFR.IEC0.value=(SFR.IEC0.value & 0x0) + (num & 0xffffffff);
	return SFR.IEC0.value & 0xffffffff;
};
SFR.IEC0CLR=function(num){
	var val=SFR.IEC0.value;
	SFR.IEC0(val&(0xFFFFFFFF-num));
};
SFR.IEC0SET=function(num){
	var val=SFR.IEC0.value;
	SFR.IEC0(val|num);
};
SFR.IEC0INV=function(num){
	var val=SFR.IEC0.value;
	SFR.IEC0(val^num);
};
SFR[0xBF881060]=SFR.IEC0;
SFR[0xBF881064]=SFR.IEC0CLR;
SFR[0xBF881068]=SFR.IEC0SET;
SFR[0xBF88106C]=SFR.IEC0INV;
SFR.IEC0(0x0);
SFR.IEC1=function(num){
	if (num!=undefined) SFR.IEC1.value=(SFR.IEC1.value & 0x4) + (num & 0xfffffffb);
	return SFR.IEC1.value & 0xfffffffb;
};
SFR.IEC1CLR=function(num){
	var val=SFR.IEC1.value;
	SFR.IEC1(val&(0xFFFFFFFF-num));
};
SFR.IEC1SET=function(num){
	var val=SFR.IEC1.value;
	SFR.IEC1(val|num);
};
SFR.IEC1INV=function(num){
	var val=SFR.IEC1.value;
	SFR.IEC1(val^num);
};
SFR[0xBF881070]=SFR.IEC1;
SFR[0xBF881074]=SFR.IEC1CLR;
SFR[0xBF881078]=SFR.IEC1SET;
SFR[0xBF88107C]=SFR.IEC1INV;
SFR.IEC1(0x0);
SFR.IEC2=function(num){
	if (num!=undefined) SFR.IEC2.value=(SFR.IEC2.value & 0xfffff070) + (num & 0xf8f);
	return SFR.IEC2.value & 0xf8f;
};
SFR.IEC2CLR=function(num){
	var val=SFR.IEC2.value;
	SFR.IEC2(val&(0xFFFFFFFF-num));
};
SFR.IEC2SET=function(num){
	var val=SFR.IEC2.value;
	SFR.IEC2(val|num);
};
SFR.IEC2INV=function(num){
	var val=SFR.IEC2.value;
	SFR.IEC2(val^num);
};
SFR[0xBF881080]=SFR.IEC2;
SFR[0xBF881084]=SFR.IEC2CLR;
SFR[0xBF881088]=SFR.IEC2SET;
SFR[0xBF88108C]=SFR.IEC2INV;
SFR.IEC2(0x0);
SFR.IPC0=function(num){
	if (num!=undefined) SFR.IPC0.value=(SFR.IPC0.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC0.value & 0x1f1f1f1f;
};
SFR.IPC0CLR=function(num){
	var val=SFR.IPC0.value;
	SFR.IPC0(val&(0xFFFFFFFF-num));
};
SFR.IPC0SET=function(num){
	var val=SFR.IPC0.value;
	SFR.IPC0(val|num);
};
SFR.IPC0INV=function(num){
	var val=SFR.IPC0.value;
	SFR.IPC0(val^num);
};
SFR[0xBF881090]=SFR.IPC0;
SFR[0xBF881094]=SFR.IPC0CLR;
SFR[0xBF881098]=SFR.IPC0SET;
SFR[0xBF88109C]=SFR.IPC0INV;
SFR.IPC0(0x0);
SFR.IPC1=function(num){
	if (num!=undefined) SFR.IPC1.value=(SFR.IPC1.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC1.value & 0x1f1f1f1f;
};
SFR.IPC1CLR=function(num){
	var val=SFR.IPC1.value;
	SFR.IPC1(val&(0xFFFFFFFF-num));
};
SFR.IPC1SET=function(num){
	var val=SFR.IPC1.value;
	SFR.IPC1(val|num);
};
SFR.IPC1INV=function(num){
	var val=SFR.IPC1.value;
	SFR.IPC1(val^num);
};
SFR[0xBF8810A0]=SFR.IPC1;
SFR[0xBF8810A4]=SFR.IPC1CLR;
SFR[0xBF8810A8]=SFR.IPC1SET;
SFR[0xBF8810AC]=SFR.IPC1INV;
SFR.IPC1(0x0);
SFR.IPC2=function(num){
	if (num!=undefined) SFR.IPC2.value=(SFR.IPC2.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC2.value & 0x1f1f1f1f;
};
SFR.IPC2CLR=function(num){
	var val=SFR.IPC2.value;
	SFR.IPC2(val&(0xFFFFFFFF-num));
};
SFR.IPC2SET=function(num){
	var val=SFR.IPC2.value;
	SFR.IPC2(val|num);
};
SFR.IPC2INV=function(num){
	var val=SFR.IPC2.value;
	SFR.IPC2(val^num);
};
SFR[0xBF8810B0]=SFR.IPC2;
SFR[0xBF8810B4]=SFR.IPC2CLR;
SFR[0xBF8810B8]=SFR.IPC2SET;
SFR[0xBF8810BC]=SFR.IPC2INV;
SFR.IPC2(0x0);
SFR.IPC3=function(num){
	if (num!=undefined) SFR.IPC3.value=(SFR.IPC3.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC3.value & 0x1f1f1f1f;
};
SFR.IPC3CLR=function(num){
	var val=SFR.IPC3.value;
	SFR.IPC3(val&(0xFFFFFFFF-num));
};
SFR.IPC3SET=function(num){
	var val=SFR.IPC3.value;
	SFR.IPC3(val|num);
};
SFR.IPC3INV=function(num){
	var val=SFR.IPC3.value;
	SFR.IPC3(val^num);
};
SFR[0xBF8810C0]=SFR.IPC3;
SFR[0xBF8810C4]=SFR.IPC3CLR;
SFR[0xBF8810C8]=SFR.IPC3SET;
SFR[0xBF8810CC]=SFR.IPC3INV;
SFR.IPC3(0x0);
SFR.IPC4=function(num){
	if (num!=undefined) SFR.IPC4.value=(SFR.IPC4.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC4.value & 0x1f1f1f1f;
};
SFR.IPC4CLR=function(num){
	var val=SFR.IPC4.value;
	SFR.IPC4(val&(0xFFFFFFFF-num));
};
SFR.IPC4SET=function(num){
	var val=SFR.IPC4.value;
	SFR.IPC4(val|num);
};
SFR.IPC4INV=function(num){
	var val=SFR.IPC4.value;
	SFR.IPC4(val^num);
};
SFR[0xBF8810D0]=SFR.IPC4;
SFR[0xBF8810D4]=SFR.IPC4CLR;
SFR[0xBF8810D8]=SFR.IPC4SET;
SFR[0xBF8810DC]=SFR.IPC4INV;
SFR.IPC4(0x0);
SFR.IPC5=function(num){
	if (num!=undefined) SFR.IPC5.value=(SFR.IPC5.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC5.value & 0x1f1f1f1f;
};
SFR.IPC5CLR=function(num){
	var val=SFR.IPC5.value;
	SFR.IPC5(val&(0xFFFFFFFF-num));
};
SFR.IPC5SET=function(num){
	var val=SFR.IPC5.value;
	SFR.IPC5(val|num);
};
SFR.IPC5INV=function(num){
	var val=SFR.IPC5.value;
	SFR.IPC5(val^num);
};
SFR[0xBF8810E0]=SFR.IPC5;
SFR[0xBF8810E4]=SFR.IPC5CLR;
SFR[0xBF8810E8]=SFR.IPC5SET;
SFR[0xBF8810EC]=SFR.IPC5INV;
SFR.IPC5(0x0);
SFR.IPC6=function(num){
	if (num!=undefined) SFR.IPC6.value=(SFR.IPC6.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC6.value & 0x1f1f1f1f;
};
SFR.IPC6CLR=function(num){
	var val=SFR.IPC6.value;
	SFR.IPC6(val&(0xFFFFFFFF-num));
};
SFR.IPC6SET=function(num){
	var val=SFR.IPC6.value;
	SFR.IPC6(val|num);
};
SFR.IPC6INV=function(num){
	var val=SFR.IPC6.value;
	SFR.IPC6(val^num);
};
SFR[0xBF8810F0]=SFR.IPC6;
SFR[0xBF8810F4]=SFR.IPC6CLR;
SFR[0xBF8810F8]=SFR.IPC6SET;
SFR[0xBF8810FC]=SFR.IPC6INV;
SFR.IPC6(0x0);
SFR.IPC7=function(num){
	if (num!=undefined) SFR.IPC7.value=(SFR.IPC7.value & 0xe0e0ffe0) + (num & 0x1f1f001f);
	return SFR.IPC7.value & 0x1f1f001f;
};
SFR.IPC7CLR=function(num){
	var val=SFR.IPC7.value;
	SFR.IPC7(val&(0xFFFFFFFF-num));
};
SFR.IPC7SET=function(num){
	var val=SFR.IPC7.value;
	SFR.IPC7(val|num);
};
SFR.IPC7INV=function(num){
	var val=SFR.IPC7.value;
	SFR.IPC7(val^num);
};
SFR[0xBF881100]=SFR.IPC7;
SFR[0xBF881104]=SFR.IPC7CLR;
SFR[0xBF881108]=SFR.IPC7SET;
SFR[0xBF88110C]=SFR.IPC7INV;
SFR.IPC7(0x0);
SFR.IPC8=function(num){
	if (num!=undefined) SFR.IPC8.value=(SFR.IPC8.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC8.value & 0x1f1f1f1f;
};
SFR.IPC8CLR=function(num){
	var val=SFR.IPC8.value;
	SFR.IPC8(val&(0xFFFFFFFF-num));
};
SFR.IPC8SET=function(num){
	var val=SFR.IPC8.value;
	SFR.IPC8(val|num);
};
SFR.IPC8INV=function(num){
	var val=SFR.IPC8.value;
	SFR.IPC8(val^num);
};
SFR[0xBF881110]=SFR.IPC8;
SFR[0xBF881114]=SFR.IPC8CLR;
SFR[0xBF881118]=SFR.IPC8SET;
SFR[0xBF88111C]=SFR.IPC8INV;
SFR.IPC8(0x0);
SFR.IPC9=function(num){
	if (num!=undefined) SFR.IPC9.value=(SFR.IPC9.value & 0xe0e0e0e0) + (num & 0x1f1f1f1f);
	return SFR.IPC9.value & 0x1f1f1f1f;
};
SFR.IPC9CLR=function(num){
	var val=SFR.IPC9.value;
	SFR.IPC9(val&(0xFFFFFFFF-num));
};
SFR.IPC9SET=function(num){
	var val=SFR.IPC9.value;
	SFR.IPC9(val|num);
};
SFR.IPC9INV=function(num){
	var val=SFR.IPC9.value;
	SFR.IPC9(val^num);
};
SFR[0xBF881120]=SFR.IPC9;
SFR[0xBF881124]=SFR.IPC9CLR;
SFR[0xBF881128]=SFR.IPC9SET;
SFR[0xBF88112C]=SFR.IPC9INV;
SFR.IPC9(0x0);
SFR.IPC10=function(num){
	if (num!=undefined) SFR.IPC10.value=(SFR.IPC10.value & 0xe0e0e0ff) + (num & 0x1f1f1f00);
	return SFR.IPC10.value & 0x1f1f1f00;
};
SFR.IPC10CLR=function(num){
	var val=SFR.IPC10.value;
	SFR.IPC10(val&(0xFFFFFFFF-num));
};
SFR.IPC10SET=function(num){
	var val=SFR.IPC10.value;
	SFR.IPC10(val|num);
};
SFR.IPC10INV=function(num){
	var val=SFR.IPC10.value;
	SFR.IPC10(val^num);
};
SFR[0xBF881130]=SFR.IPC10;
SFR[0xBF881134]=SFR.IPC10CLR;
SFR[0xBF881138]=SFR.IPC10SET;
SFR[0xBF88113C]=SFR.IPC10INV;
SFR.IPC10(0x0);
SFR.IPC11=function(num){
	if (num!=undefined) SFR.IPC11.value=(SFR.IPC11.value & 0xffffe0e0) + (num & 0x1f1f);
	return SFR.IPC11.value & 0x1f1f;
};
SFR.IPC11CLR=function(num){
	var val=SFR.IPC11.value;
	SFR.IPC11(val&(0xFFFFFFFF-num));
};
SFR.IPC11SET=function(num){
	var val=SFR.IPC11.value;
	SFR.IPC11(val|num);
};
SFR.IPC11INV=function(num){
	var val=SFR.IPC11.value;
	SFR.IPC11(val^num);
};
SFR[0xBF881140]=SFR.IPC11;
SFR[0xBF881144]=SFR.IPC11CLR;
SFR[0xBF881148]=SFR.IPC11SET;
SFR[0xBF88114C]=SFR.IPC11INV;
SFR.IPC11(0x0);
SFR.BMXCON=function(num){
	if (num!=undefined) SFR.BMXCON.value=(SFR.BMXCON.value & 0xfbe0ffb8) + (num & 0x41f0047);
	return SFR.BMXCON.value & 0x41f0047;
};
SFR.BMXCONCLR=function(num){
	var val=SFR.BMXCON.value;
	SFR.BMXCON(val&(0xFFFFFFFF-num));
};
SFR.BMXCONSET=function(num){
	var val=SFR.BMXCON.value;
	SFR.BMXCON(val|num);
};
SFR.BMXCONINV=function(num){
	var val=SFR.BMXCON.value;
	SFR.BMXCON(val^num);
};
SFR[0xBF882000]=SFR.BMXCON;
SFR[0xBF882004]=SFR.BMXCONCLR;
SFR[0xBF882008]=SFR.BMXCONSET;
SFR[0xBF88200C]=SFR.BMXCONINV;
SFR.BMXCON(0x1f0041);
SFR.BMXDKPBA=function(num){
	if (num!=undefined) SFR.BMXDKPBA.value=(SFR.BMXDKPBA.value & 0x0) + (num & 0xffffffff);
	return SFR.BMXDKPBA.value & 0xffffffff;
};
SFR.BMXDKPBACLR=function(num){
	var val=SFR.BMXDKPBA.value;
	SFR.BMXDKPBA(val&(0xFFFFFFFF-num));
};
SFR.BMXDKPBASET=function(num){
	var val=SFR.BMXDKPBA.value;
	SFR.BMXDKPBA(val|num);
};
SFR.BMXDKPBAINV=function(num){
	var val=SFR.BMXDKPBA.value;
	SFR.BMXDKPBA(val^num);
};
SFR[0xBF882010]=SFR.BMXDKPBA;
SFR[0xBF882014]=SFR.BMXDKPBACLR;
SFR[0xBF882018]=SFR.BMXDKPBASET;
SFR[0xBF88201C]=SFR.BMXDKPBAINV;
SFR.BMXDKPBA(0x0);
SFR.BMXDUDBA=function(num){
	if (num!=undefined) SFR.BMXDUDBA.value=(SFR.BMXDUDBA.value & 0x0) + (num & 0xffffffff);
	return SFR.BMXDUDBA.value & 0xffffffff;
};
SFR.BMXDUDBACLR=function(num){
	var val=SFR.BMXDUDBA.value;
	SFR.BMXDUDBA(val&(0xFFFFFFFF-num));
};
SFR.BMXDUDBASET=function(num){
	var val=SFR.BMXDUDBA.value;
	SFR.BMXDUDBA(val|num);
};
SFR.BMXDUDBAINV=function(num){
	var val=SFR.BMXDUDBA.value;
	SFR.BMXDUDBA(val^num);
};
SFR[0xBF882020]=SFR.BMXDUDBA;
SFR[0xBF882024]=SFR.BMXDUDBACLR;
SFR[0xBF882028]=SFR.BMXDUDBASET;
SFR[0xBF88202C]=SFR.BMXDUDBAINV;
SFR.BMXDUDBA(0x0);
SFR.BMXDUPBA=function(num){
	if (num!=undefined) SFR.BMXDUPBA.value=(SFR.BMXDUPBA.value & 0x0) + (num & 0xffffffff);
	return SFR.BMXDUPBA.value & 0xffffffff;
};
SFR.BMXDUPBACLR=function(num){
	var val=SFR.BMXDUPBA.value;
	SFR.BMXDUPBA(val&(0xFFFFFFFF-num));
};
SFR.BMXDUPBASET=function(num){
	var val=SFR.BMXDUPBA.value;
	SFR.BMXDUPBA(val|num);
};
SFR.BMXDUPBAINV=function(num){
	var val=SFR.BMXDUPBA.value;
	SFR.BMXDUPBA(val^num);
};
SFR[0xBF882030]=SFR.BMXDUPBA;
SFR[0xBF882034]=SFR.BMXDUPBACLR;
SFR[0xBF882038]=SFR.BMXDUPBASET;
SFR[0xBF88203C]=SFR.BMXDUPBAINV;
SFR.BMXDUPBA(0x0);
SFR.BMXPUPBA=function(num){
	if (num!=undefined) SFR.BMXPUPBA.value=(SFR.BMXPUPBA.value & 0x0) + (num & 0xffffffff);
	return SFR.BMXPUPBA.value & 0xffffffff;
};
SFR.BMXPUPBACLR=function(num){
	var val=SFR.BMXPUPBA.value;
	SFR.BMXPUPBA(val&(0xFFFFFFFF-num));
};
SFR.BMXPUPBASET=function(num){
	var val=SFR.BMXPUPBA.value;
	SFR.BMXPUPBA(val|num);
};
SFR.BMXPUPBAINV=function(num){
	var val=SFR.BMXPUPBA.value;
	SFR.BMXPUPBA(val^num);
};
SFR[0xBF882050]=SFR.BMXPUPBA;
SFR[0xBF882054]=SFR.BMXPUPBACLR;
SFR[0xBF882058]=SFR.BMXPUPBASET;
SFR[0xBF88205C]=SFR.BMXPUPBAINV;
SFR.BMXPUPBA(0x0);
SFR.DMACON=function(num){
	if (num!=undefined) SFR.DMACON.value=(SFR.DMACON.value & 0xffff67ff) + (num & 0x9800);
	return SFR.DMACON.value & 0x9800;
};
SFR.DMACONCLR=function(num){
	var val=SFR.DMACON.value;
	SFR.DMACON(val&(0xFFFFFFFF-num));
};
SFR.DMACONSET=function(num){
	var val=SFR.DMACON.value;
	SFR.DMACON(val|num);
};
SFR.DMACONINV=function(num){
	var val=SFR.DMACON.value;
	SFR.DMACON(val^num);
};
SFR[0xBF883000]=SFR.DMACON;
SFR[0xBF883004]=SFR.DMACONCLR;
SFR[0xBF883008]=SFR.DMACONSET;
SFR[0xBF88300C]=SFR.DMACONINV;
SFR.DMACON(0x0);
SFR.DMASTAT=function(num){
	if (num!=undefined) SFR.DMASTAT.value=(SFR.DMASTAT.value & 0xfffffff0) + (num & 0xf);
	return SFR.DMASTAT.value & 0xf;
};
SFR.DMASTATCLR=function(num){
	var val=SFR.DMASTAT.value;
	SFR.DMASTAT(val&(0xFFFFFFFF-num));
};
SFR.DMASTATSET=function(num){
	var val=SFR.DMASTAT.value;
	SFR.DMASTAT(val|num);
};
SFR.DMASTATINV=function(num){
	var val=SFR.DMASTAT.value;
	SFR.DMASTAT(val^num);
};
SFR[0xBF883010]=SFR.DMASTAT;
SFR[0xBF883014]=SFR.DMASTATCLR;
SFR[0xBF883018]=SFR.DMASTATSET;
SFR[0xBF88301C]=SFR.DMASTATINV;
SFR.DMASTAT(0x0);
SFR.DMAADDR=function(num){
	if (num!=undefined) SFR.DMAADDR.value=(SFR.DMAADDR.value & 0xffffffff) + (num & 0x0);
	return SFR.DMAADDR.value & 0xffffffff;
};
SFR.DMAADDRCLR=function(num){
	var val=SFR.DMAADDR.value;
	SFR.DMAADDR(val&(0xFFFFFFFF-num));
};
SFR.DMAADDRSET=function(num){
	var val=SFR.DMAADDR.value;
	SFR.DMAADDR(val|num);
};
SFR.DMAADDRINV=function(num){
	var val=SFR.DMAADDR.value;
	SFR.DMAADDR(val^num);
};
SFR[0xBF883020]=SFR.DMAADDR;
SFR[0xBF883024]=SFR.DMAADDRCLR;
SFR[0xBF883028]=SFR.DMAADDRSET;
SFR[0xBF88302C]=SFR.DMAADDRINV;
SFR.DMAADDR(0x0);
SFR.DCRCCON=function(num){
	if (num!=undefined) SFR.DCRCCON.value=(SFR.DCRCCON.value & 0xc6ffe018) + (num & 0x39001fe7);
	return SFR.DCRCCON.value & 0x39001fe7;
};
SFR.DCRCCONCLR=function(num){
	var val=SFR.DCRCCON.value;
	SFR.DCRCCON(val&(0xFFFFFFFF-num));
};
SFR.DCRCCONSET=function(num){
	var val=SFR.DCRCCON.value;
	SFR.DCRCCON(val|num);
};
SFR.DCRCCONINV=function(num){
	var val=SFR.DCRCCON.value;
	SFR.DCRCCON(val^num);
};
SFR[0xBF883030]=SFR.DCRCCON;
SFR[0xBF883034]=SFR.DCRCCONCLR;
SFR[0xBF883038]=SFR.DCRCCONSET;
SFR[0xBF88303C]=SFR.DCRCCONINV;
SFR.DCRCCON(0x0);
SFR.DCRCDATA=function(num){
	if (num!=undefined) SFR.DCRCDATA.value=(SFR.DCRCDATA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCRCDATA.value & 0xffffffff;
};
SFR.DCRCDATACLR=function(num){
	var val=SFR.DCRCDATA.value;
	SFR.DCRCDATA(val&(0xFFFFFFFF-num));
};
SFR.DCRCDATASET=function(num){
	var val=SFR.DCRCDATA.value;
	SFR.DCRCDATA(val|num);
};
SFR.DCRCDATAINV=function(num){
	var val=SFR.DCRCDATA.value;
	SFR.DCRCDATA(val^num);
};
SFR[0xBF883040]=SFR.DCRCDATA;
SFR[0xBF883044]=SFR.DCRCDATACLR;
SFR[0xBF883048]=SFR.DCRCDATASET;
SFR[0xBF88304C]=SFR.DCRCDATAINV;
SFR.DCRCDATA(0x0);
SFR.DCRCXOR=function(num){
	if (num!=undefined) SFR.DCRCXOR.value=(SFR.DCRCXOR.value & 0x0) + (num & 0xffffffff);
	return SFR.DCRCXOR.value & 0xffffffff;
};
SFR.DCRCXORCLR=function(num){
	var val=SFR.DCRCXOR.value;
	SFR.DCRCXOR(val&(0xFFFFFFFF-num));
};
SFR.DCRCXORSET=function(num){
	var val=SFR.DCRCXOR.value;
	SFR.DCRCXOR(val|num);
};
SFR.DCRCXORINV=function(num){
	var val=SFR.DCRCXOR.value;
	SFR.DCRCXOR(val^num);
};
SFR[0xBF883050]=SFR.DCRCXOR;
SFR[0xBF883054]=SFR.DCRCXORCLR;
SFR[0xBF883058]=SFR.DCRCXORSET;
SFR[0xBF88305C]=SFR.DCRCXORINV;
SFR.DCRCXOR(0x0);
SFR.DCH0CON=function(num){
	if (num!=undefined) SFR.DCH0CON.value=(SFR.DCH0CON.value & 0xffff7e0c) + (num & 0x81f3);
	return SFR.DCH0CON.value & 0x81f7;
};
SFR.DCH0CONCLR=function(num){
	var val=SFR.DCH0CON.value;
	SFR.DCH0CON(val&(0xFFFFFFFF-num));
};
SFR.DCH0CONSET=function(num){
	var val=SFR.DCH0CON.value;
	SFR.DCH0CON(val|num);
};
SFR.DCH0CONINV=function(num){
	var val=SFR.DCH0CON.value;
	SFR.DCH0CON(val^num);
};
SFR[0xBF883060]=SFR.DCH0CON;
SFR[0xBF883064]=SFR.DCH0CONCLR;
SFR[0xBF883068]=SFR.DCH0CONSET;
SFR[0xBF88306C]=SFR.DCH0CONINV;
SFR.DCH0CON(0x0);
SFR.DCH0ECON=function(num){
	if (num!=undefined) SFR.DCH0ECON.value=(SFR.DCH0ECON.value & 0xff0000c7) + (num & 0xffff38);
	return SFR.DCH0ECON.value & 0xfffff8;
};
SFR.DCH0ECONCLR=function(num){
	var val=SFR.DCH0ECON.value;
	SFR.DCH0ECON(val&(0xFFFFFFFF-num));
};
SFR.DCH0ECONSET=function(num){
	var val=SFR.DCH0ECON.value;
	SFR.DCH0ECON(val|num);
};
SFR.DCH0ECONINV=function(num){
	var val=SFR.DCH0ECON.value;
	SFR.DCH0ECON(val^num);
};
SFR[0xBF883070]=SFR.DCH0ECON;
SFR[0xBF883074]=SFR.DCH0ECONCLR;
SFR[0xBF883078]=SFR.DCH0ECONSET;
SFR[0xBF88307C]=SFR.DCH0ECONINV;
SFR.DCH0ECON(0xffff00);
SFR.DCH0INT=function(num){
	if (num!=undefined) SFR.DCH0INT.value=(SFR.DCH0INT.value & 0xff00ff00) + (num & 0xff00ff);
	return SFR.DCH0INT.value & 0xff00ff;
};
SFR.DCH0INTCLR=function(num){
	var val=SFR.DCH0INT.value;
	SFR.DCH0INT(val&(0xFFFFFFFF-num));
};
SFR.DCH0INTSET=function(num){
	var val=SFR.DCH0INT.value;
	SFR.DCH0INT(val|num);
};
SFR.DCH0INTINV=function(num){
	var val=SFR.DCH0INT.value;
	SFR.DCH0INT(val^num);
};
SFR[0xBF883080]=SFR.DCH0INT;
SFR[0xBF883084]=SFR.DCH0INTCLR;
SFR[0xBF883088]=SFR.DCH0INTSET;
SFR[0xBF88308C]=SFR.DCH0INTINV;
SFR.DCH0INT(0x0);
SFR.DCH0SSA=function(num){
	if (num!=undefined) SFR.DCH0SSA.value=(SFR.DCH0SSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH0SSA.value & 0xffffffff;
};
SFR.DCH0SSACLR=function(num){
	var val=SFR.DCH0SSA.value;
	SFR.DCH0SSA(val&(0xFFFFFFFF-num));
};
SFR.DCH0SSASET=function(num){
	var val=SFR.DCH0SSA.value;
	SFR.DCH0SSA(val|num);
};
SFR.DCH0SSAINV=function(num){
	var val=SFR.DCH0SSA.value;
	SFR.DCH0SSA(val^num);
};
SFR[0xBF883090]=SFR.DCH0SSA;
SFR[0xBF883094]=SFR.DCH0SSACLR;
SFR[0xBF883098]=SFR.DCH0SSASET;
SFR[0xBF88309C]=SFR.DCH0SSAINV;
SFR.DCH0SSA(0x0);
SFR.DCH0DSA=function(num){
	if (num!=undefined) SFR.DCH0DSA.value=(SFR.DCH0DSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH0DSA.value & 0xffffffff;
};
SFR.DCH0DSACLR=function(num){
	var val=SFR.DCH0DSA.value;
	SFR.DCH0DSA(val&(0xFFFFFFFF-num));
};
SFR.DCH0DSASET=function(num){
	var val=SFR.DCH0DSA.value;
	SFR.DCH0DSA(val|num);
};
SFR.DCH0DSAINV=function(num){
	var val=SFR.DCH0DSA.value;
	SFR.DCH0DSA(val^num);
};
SFR[0xBF8830A0]=SFR.DCH0DSA;
SFR[0xBF8830A4]=SFR.DCH0DSACLR;
SFR[0xBF8830A8]=SFR.DCH0DSASET;
SFR[0xBF8830AC]=SFR.DCH0DSAINV;
SFR.DCH0DSA(0x0);
SFR.DCH0SSIZ=function(num){
	if (num!=undefined) SFR.DCH0SSIZ.value=(SFR.DCH0SSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH0SSIZ.value & 0xffff;
};
SFR.DCH0SSIZCLR=function(num){
	var val=SFR.DCH0SSIZ.value;
	SFR.DCH0SSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH0SSIZSET=function(num){
	var val=SFR.DCH0SSIZ.value;
	SFR.DCH0SSIZ(val|num);
};
SFR.DCH0SSIZINV=function(num){
	var val=SFR.DCH0SSIZ.value;
	SFR.DCH0SSIZ(val^num);
};
SFR[0xBF8830B0]=SFR.DCH0SSIZ;
SFR[0xBF8830B4]=SFR.DCH0SSIZCLR;
SFR[0xBF8830B8]=SFR.DCH0SSIZSET;
SFR[0xBF8830BC]=SFR.DCH0SSIZINV;
SFR.DCH0SSIZ(0x0);
SFR.DCH0DSIZ=function(num){
	if (num!=undefined) SFR.DCH0DSIZ.value=(SFR.DCH0DSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH0DSIZ.value & 0xffff;
};
SFR.DCH0DSIZCLR=function(num){
	var val=SFR.DCH0DSIZ.value;
	SFR.DCH0DSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH0DSIZSET=function(num){
	var val=SFR.DCH0DSIZ.value;
	SFR.DCH0DSIZ(val|num);
};
SFR.DCH0DSIZINV=function(num){
	var val=SFR.DCH0DSIZ.value;
	SFR.DCH0DSIZ(val^num);
};
SFR[0xBF8830C0]=SFR.DCH0DSIZ;
SFR[0xBF8830C4]=SFR.DCH0DSIZCLR;
SFR[0xBF8830C8]=SFR.DCH0DSIZSET;
SFR[0xBF8830CC]=SFR.DCH0DSIZINV;
SFR.DCH0DSIZ(0x0);
SFR.DCH0SPTR=function(num){
	if (num!=undefined) SFR.DCH0SPTR.value=(SFR.DCH0SPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH0SPTR.value & 0xffff;
};
SFR.DCH0SPTRCLR=function(num){
	var val=SFR.DCH0SPTR.value;
	SFR.DCH0SPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH0SPTRSET=function(num){
	var val=SFR.DCH0SPTR.value;
	SFR.DCH0SPTR(val|num);
};
SFR.DCH0SPTRINV=function(num){
	var val=SFR.DCH0SPTR.value;
	SFR.DCH0SPTR(val^num);
};
SFR[0xBF8830D0]=SFR.DCH0SPTR;
SFR[0xBF8830D4]=SFR.DCH0SPTRCLR;
SFR[0xBF8830D8]=SFR.DCH0SPTRSET;
SFR[0xBF8830DC]=SFR.DCH0SPTRINV;
SFR.DCH0SPTR(0x0);
SFR.DCH0DPTR=function(num){
	if (num!=undefined) SFR.DCH0DPTR.value=(SFR.DCH0DPTR.value & 0xffffffff) + (num & 0x0);
	return SFR.DCH0DPTR.value & 0xffff;
};
SFR.DCH0DPTRCLR=function(num){
	var val=SFR.DCH0DPTR.value;
	SFR.DCH0DPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH0DPTRSET=function(num){
	var val=SFR.DCH0DPTR.value;
	SFR.DCH0DPTR(val|num);
};
SFR.DCH0DPTRINV=function(num){
	var val=SFR.DCH0DPTR.value;
	SFR.DCH0DPTR(val^num);
};
SFR[0xBF8830E0]=SFR.DCH0DPTR;
SFR[0xBF8830E4]=SFR.DCH0DPTRCLR;
SFR[0xBF8830E8]=SFR.DCH0DPTRSET;
SFR[0xBF8830EC]=SFR.DCH0DPTRINV;
SFR.DCH0DPTR(0x0);
SFR.DCH0CSIZ=function(num){
	if (num!=undefined) SFR.DCH0CSIZ.value=(SFR.DCH0CSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH0CSIZ.value & 0xffff;
};
SFR.DCH0CSIZCLR=function(num){
	var val=SFR.DCH0CSIZ.value;
	SFR.DCH0CSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH0CSIZSET=function(num){
	var val=SFR.DCH0CSIZ.value;
	SFR.DCH0CSIZ(val|num);
};
SFR.DCH0CSIZINV=function(num){
	var val=SFR.DCH0CSIZ.value;
	SFR.DCH0CSIZ(val^num);
};
SFR[0xBF8830F0]=SFR.DCH0CSIZ;
SFR[0xBF8830F4]=SFR.DCH0CSIZCLR;
SFR[0xBF8830F8]=SFR.DCH0CSIZSET;
SFR[0xBF8830FC]=SFR.DCH0CSIZINV;
SFR.DCH0CSIZ(0x0);
SFR.DCH0CPTR=function(num){
	if (num!=undefined) SFR.DCH0CPTR.value=(SFR.DCH0CPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH0CPTR.value & 0xffff;
};
SFR.DCH0CPTRCLR=function(num){
	var val=SFR.DCH0CPTR.value;
	SFR.DCH0CPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH0CPTRSET=function(num){
	var val=SFR.DCH0CPTR.value;
	SFR.DCH0CPTR(val|num);
};
SFR.DCH0CPTRINV=function(num){
	var val=SFR.DCH0CPTR.value;
	SFR.DCH0CPTR(val^num);
};
SFR[0xBF883100]=SFR.DCH0CPTR;
SFR[0xBF883104]=SFR.DCH0CPTRCLR;
SFR[0xBF883108]=SFR.DCH0CPTRSET;
SFR[0xBF88310C]=SFR.DCH0CPTRINV;
SFR.DCH0CPTR(0x0);
SFR.DCH0DAT=function(num){
	if (num!=undefined) SFR.DCH0DAT.value=(SFR.DCH0DAT.value & 0xffffff00) + (num & 0xff);
	return SFR.DCH0DAT.value & 0xff;
};
SFR.DCH0DATCLR=function(num){
	var val=SFR.DCH0DAT.value;
	SFR.DCH0DAT(val&(0xFFFFFFFF-num));
};
SFR.DCH0DATSET=function(num){
	var val=SFR.DCH0DAT.value;
	SFR.DCH0DAT(val|num);
};
SFR.DCH0DATINV=function(num){
	var val=SFR.DCH0DAT.value;
	SFR.DCH0DAT(val^num);
};
SFR[0xBF883110]=SFR.DCH0DAT;
SFR[0xBF883114]=SFR.DCH0DATCLR;
SFR[0xBF883118]=SFR.DCH0DATSET;
SFR[0xBF88311C]=SFR.DCH0DATINV;
SFR.DCH0DAT(0x0);
SFR.DCH1CON=function(num){
	if (num!=undefined) SFR.DCH1CON.value=(SFR.DCH1CON.value & 0xffff7e0c) + (num & 0x81f3);
	return SFR.DCH1CON.value & 0x81f7;
};
SFR.DCH1CONCLR=function(num){
	var val=SFR.DCH1CON.value;
	SFR.DCH1CON(val&(0xFFFFFFFF-num));
};
SFR.DCH1CONSET=function(num){
	var val=SFR.DCH1CON.value;
	SFR.DCH1CON(val|num);
};
SFR.DCH1CONINV=function(num){
	var val=SFR.DCH1CON.value;
	SFR.DCH1CON(val^num);
};
SFR[0xBF883120]=SFR.DCH1CON;
SFR[0xBF883124]=SFR.DCH1CONCLR;
SFR[0xBF883128]=SFR.DCH1CONSET;
SFR[0xBF88312C]=SFR.DCH1CONINV;
SFR.DCH1CON(0x0);
SFR.DCH1ECON=function(num){
	if (num!=undefined) SFR.DCH1ECON.value=(SFR.DCH1ECON.value & 0xff0000c7) + (num & 0xffff38);
	return SFR.DCH1ECON.value & 0xfffff8;
};
SFR.DCH1ECONCLR=function(num){
	var val=SFR.DCH1ECON.value;
	SFR.DCH1ECON(val&(0xFFFFFFFF-num));
};
SFR.DCH1ECONSET=function(num){
	var val=SFR.DCH1ECON.value;
	SFR.DCH1ECON(val|num);
};
SFR.DCH1ECONINV=function(num){
	var val=SFR.DCH1ECON.value;
	SFR.DCH1ECON(val^num);
};
SFR[0xBF883130]=SFR.DCH1ECON;
SFR[0xBF883134]=SFR.DCH1ECONCLR;
SFR[0xBF883138]=SFR.DCH1ECONSET;
SFR[0xBF88313C]=SFR.DCH1ECONINV;
SFR.DCH1ECON(0xffff00);
SFR.DCH1INT=function(num){
	if (num!=undefined) SFR.DCH1INT.value=(SFR.DCH1INT.value & 0xff00ff00) + (num & 0xff00ff);
	return SFR.DCH1INT.value & 0xff00ff;
};
SFR.DCH1INTCLR=function(num){
	var val=SFR.DCH1INT.value;
	SFR.DCH1INT(val&(0xFFFFFFFF-num));
};
SFR.DCH1INTSET=function(num){
	var val=SFR.DCH1INT.value;
	SFR.DCH1INT(val|num);
};
SFR.DCH1INTINV=function(num){
	var val=SFR.DCH1INT.value;
	SFR.DCH1INT(val^num);
};
SFR[0xBF883140]=SFR.DCH1INT;
SFR[0xBF883144]=SFR.DCH1INTCLR;
SFR[0xBF883148]=SFR.DCH1INTSET;
SFR[0xBF88314C]=SFR.DCH1INTINV;
SFR.DCH1INT(0x0);
SFR.DCH1SSA=function(num){
	if (num!=undefined) SFR.DCH1SSA.value=(SFR.DCH1SSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH1SSA.value & 0xffffffff;
};
SFR.DCH1SSACLR=function(num){
	var val=SFR.DCH1SSA.value;
	SFR.DCH1SSA(val&(0xFFFFFFFF-num));
};
SFR.DCH1SSASET=function(num){
	var val=SFR.DCH1SSA.value;
	SFR.DCH1SSA(val|num);
};
SFR.DCH1SSAINV=function(num){
	var val=SFR.DCH1SSA.value;
	SFR.DCH1SSA(val^num);
};
SFR[0xBF883150]=SFR.DCH1SSA;
SFR[0xBF883154]=SFR.DCH1SSACLR;
SFR[0xBF883158]=SFR.DCH1SSASET;
SFR[0xBF88315C]=SFR.DCH1SSAINV;
SFR.DCH1SSA(0x0);
SFR.DCH1DSA=function(num){
	if (num!=undefined) SFR.DCH1DSA.value=(SFR.DCH1DSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH1DSA.value & 0xffffffff;
};
SFR.DCH1DSACLR=function(num){
	var val=SFR.DCH1DSA.value;
	SFR.DCH1DSA(val&(0xFFFFFFFF-num));
};
SFR.DCH1DSASET=function(num){
	var val=SFR.DCH1DSA.value;
	SFR.DCH1DSA(val|num);
};
SFR.DCH1DSAINV=function(num){
	var val=SFR.DCH1DSA.value;
	SFR.DCH1DSA(val^num);
};
SFR[0xBF883160]=SFR.DCH1DSA;
SFR[0xBF883164]=SFR.DCH1DSACLR;
SFR[0xBF883168]=SFR.DCH1DSASET;
SFR[0xBF88316C]=SFR.DCH1DSAINV;
SFR.DCH1DSA(0x0);
SFR.DCH1SSIZ=function(num){
	if (num!=undefined) SFR.DCH1SSIZ.value=(SFR.DCH1SSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH1SSIZ.value & 0xffff;
};
SFR.DCH1SSIZCLR=function(num){
	var val=SFR.DCH1SSIZ.value;
	SFR.DCH1SSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH1SSIZSET=function(num){
	var val=SFR.DCH1SSIZ.value;
	SFR.DCH1SSIZ(val|num);
};
SFR.DCH1SSIZINV=function(num){
	var val=SFR.DCH1SSIZ.value;
	SFR.DCH1SSIZ(val^num);
};
SFR[0xBF883170]=SFR.DCH1SSIZ;
SFR[0xBF883174]=SFR.DCH1SSIZCLR;
SFR[0xBF883178]=SFR.DCH1SSIZSET;
SFR[0xBF88317C]=SFR.DCH1SSIZINV;
SFR.DCH1SSIZ(0x0);
SFR.DCH1DSIZ=function(num){
	if (num!=undefined) SFR.DCH1DSIZ.value=(SFR.DCH1DSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH1DSIZ.value & 0xffff;
};
SFR.DCH1DSIZCLR=function(num){
	var val=SFR.DCH1DSIZ.value;
	SFR.DCH1DSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH1DSIZSET=function(num){
	var val=SFR.DCH1DSIZ.value;
	SFR.DCH1DSIZ(val|num);
};
SFR.DCH1DSIZINV=function(num){
	var val=SFR.DCH1DSIZ.value;
	SFR.DCH1DSIZ(val^num);
};
SFR[0xBF883180]=SFR.DCH1DSIZ;
SFR[0xBF883184]=SFR.DCH1DSIZCLR;
SFR[0xBF883188]=SFR.DCH1DSIZSET;
SFR[0xBF88318C]=SFR.DCH1DSIZINV;
SFR.DCH1DSIZ(0x0);
SFR.DCH1SPTR=function(num){
	if (num!=undefined) SFR.DCH1SPTR.value=(SFR.DCH1SPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH1SPTR.value & 0xffff;
};
SFR.DCH1SPTRCLR=function(num){
	var val=SFR.DCH1SPTR.value;
	SFR.DCH1SPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH1SPTRSET=function(num){
	var val=SFR.DCH1SPTR.value;
	SFR.DCH1SPTR(val|num);
};
SFR.DCH1SPTRINV=function(num){
	var val=SFR.DCH1SPTR.value;
	SFR.DCH1SPTR(val^num);
};
SFR[0xBF883190]=SFR.DCH1SPTR;
SFR[0xBF883194]=SFR.DCH1SPTRCLR;
SFR[0xBF883198]=SFR.DCH1SPTRSET;
SFR[0xBF88319C]=SFR.DCH1SPTRINV;
SFR.DCH1SPTR(0x0);
SFR.DCH1DPTR=function(num){
	if (num!=undefined) SFR.DCH1DPTR.value=(SFR.DCH1DPTR.value & 0xffffffff) + (num & 0x0);
	return SFR.DCH1DPTR.value & 0xffff;
};
SFR.DCH1DPTRCLR=function(num){
	var val=SFR.DCH1DPTR.value;
	SFR.DCH1DPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH1DPTRSET=function(num){
	var val=SFR.DCH1DPTR.value;
	SFR.DCH1DPTR(val|num);
};
SFR.DCH1DPTRINV=function(num){
	var val=SFR.DCH1DPTR.value;
	SFR.DCH1DPTR(val^num);
};
SFR[0xBF8831A0]=SFR.DCH1DPTR;
SFR[0xBF8831A4]=SFR.DCH1DPTRCLR;
SFR[0xBF8831A8]=SFR.DCH1DPTRSET;
SFR[0xBF8831AC]=SFR.DCH1DPTRINV;
SFR.DCH1DPTR(0x0);
SFR.DCH1CSIZ=function(num){
	if (num!=undefined) SFR.DCH1CSIZ.value=(SFR.DCH1CSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH1CSIZ.value & 0xffff;
};
SFR.DCH1CSIZCLR=function(num){
	var val=SFR.DCH1CSIZ.value;
	SFR.DCH1CSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH1CSIZSET=function(num){
	var val=SFR.DCH1CSIZ.value;
	SFR.DCH1CSIZ(val|num);
};
SFR.DCH1CSIZINV=function(num){
	var val=SFR.DCH1CSIZ.value;
	SFR.DCH1CSIZ(val^num);
};
SFR[0xBF8831B0]=SFR.DCH1CSIZ;
SFR[0xBF8831B4]=SFR.DCH1CSIZCLR;
SFR[0xBF8831B8]=SFR.DCH1CSIZSET;
SFR[0xBF8831BC]=SFR.DCH1CSIZINV;
SFR.DCH1CSIZ(0x0);
SFR.DCH1CPTR=function(num){
	if (num!=undefined) SFR.DCH1CPTR.value=(SFR.DCH1CPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH1CPTR.value & 0xffff;
};
SFR.DCH1CPTRCLR=function(num){
	var val=SFR.DCH1CPTR.value;
	SFR.DCH1CPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH1CPTRSET=function(num){
	var val=SFR.DCH1CPTR.value;
	SFR.DCH1CPTR(val|num);
};
SFR.DCH1CPTRINV=function(num){
	var val=SFR.DCH1CPTR.value;
	SFR.DCH1CPTR(val^num);
};
SFR[0xBF8831C0]=SFR.DCH1CPTR;
SFR[0xBF8831C4]=SFR.DCH1CPTRCLR;
SFR[0xBF8831C8]=SFR.DCH1CPTRSET;
SFR[0xBF8831CC]=SFR.DCH1CPTRINV;
SFR.DCH1CPTR(0x0);
SFR.DCH1DAT=function(num){
	if (num!=undefined) SFR.DCH1DAT.value=(SFR.DCH1DAT.value & 0xffffff00) + (num & 0xff);
	return SFR.DCH1DAT.value & 0xff;
};
SFR.DCH1DATCLR=function(num){
	var val=SFR.DCH1DAT.value;
	SFR.DCH1DAT(val&(0xFFFFFFFF-num));
};
SFR.DCH1DATSET=function(num){
	var val=SFR.DCH1DAT.value;
	SFR.DCH1DAT(val|num);
};
SFR.DCH1DATINV=function(num){
	var val=SFR.DCH1DAT.value;
	SFR.DCH1DAT(val^num);
};
SFR[0xBF8831D0]=SFR.DCH1DAT;
SFR[0xBF8831D4]=SFR.DCH1DATCLR;
SFR[0xBF8831D8]=SFR.DCH1DATSET;
SFR[0xBF8831DC]=SFR.DCH1DATINV;
SFR.DCH1DAT(0x0);
SFR.DCH2CON=function(num){
	if (num!=undefined) SFR.DCH2CON.value=(SFR.DCH2CON.value & 0xffff7e0c) + (num & 0x81f3);
	return SFR.DCH2CON.value & 0x81f7;
};
SFR.DCH2CONCLR=function(num){
	var val=SFR.DCH2CON.value;
	SFR.DCH2CON(val&(0xFFFFFFFF-num));
};
SFR.DCH2CONSET=function(num){
	var val=SFR.DCH2CON.value;
	SFR.DCH2CON(val|num);
};
SFR.DCH2CONINV=function(num){
	var val=SFR.DCH2CON.value;
	SFR.DCH2CON(val^num);
};
SFR[0xBF8831E0]=SFR.DCH2CON;
SFR[0xBF8831E4]=SFR.DCH2CONCLR;
SFR[0xBF8831E8]=SFR.DCH2CONSET;
SFR[0xBF8831EC]=SFR.DCH2CONINV;
SFR.DCH2CON(0x0);
SFR.DCH2ECON=function(num){
	if (num!=undefined) SFR.DCH2ECON.value=(SFR.DCH2ECON.value & 0xff0000c7) + (num & 0xffff38);
	return SFR.DCH2ECON.value & 0xfffff8;
};
SFR.DCH2ECONCLR=function(num){
	var val=SFR.DCH2ECON.value;
	SFR.DCH2ECON(val&(0xFFFFFFFF-num));
};
SFR.DCH2ECONSET=function(num){
	var val=SFR.DCH2ECON.value;
	SFR.DCH2ECON(val|num);
};
SFR.DCH2ECONINV=function(num){
	var val=SFR.DCH2ECON.value;
	SFR.DCH2ECON(val^num);
};
SFR[0xBF8831F0]=SFR.DCH2ECON;
SFR[0xBF8831F4]=SFR.DCH2ECONCLR;
SFR[0xBF8831F8]=SFR.DCH2ECONSET;
SFR[0xBF8831FC]=SFR.DCH2ECONINV;
SFR.DCH2ECON(0xffff00);
SFR.DCH2INT=function(num){
	if (num!=undefined) SFR.DCH2INT.value=(SFR.DCH2INT.value & 0xff00ff00) + (num & 0xff00ff);
	return SFR.DCH2INT.value & 0xff00ff;
};
SFR.DCH2INTCLR=function(num){
	var val=SFR.DCH2INT.value;
	SFR.DCH2INT(val&(0xFFFFFFFF-num));
};
SFR.DCH2INTSET=function(num){
	var val=SFR.DCH2INT.value;
	SFR.DCH2INT(val|num);
};
SFR.DCH2INTINV=function(num){
	var val=SFR.DCH2INT.value;
	SFR.DCH2INT(val^num);
};
SFR[0xBF883200]=SFR.DCH2INT;
SFR[0xBF883204]=SFR.DCH2INTCLR;
SFR[0xBF883208]=SFR.DCH2INTSET;
SFR[0xBF88320C]=SFR.DCH2INTINV;
SFR.DCH2INT(0x0);
SFR.DCH2SSA=function(num){
	if (num!=undefined) SFR.DCH2SSA.value=(SFR.DCH2SSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH2SSA.value & 0xffffffff;
};
SFR.DCH2SSACLR=function(num){
	var val=SFR.DCH2SSA.value;
	SFR.DCH2SSA(val&(0xFFFFFFFF-num));
};
SFR.DCH2SSASET=function(num){
	var val=SFR.DCH2SSA.value;
	SFR.DCH2SSA(val|num);
};
SFR.DCH2SSAINV=function(num){
	var val=SFR.DCH2SSA.value;
	SFR.DCH2SSA(val^num);
};
SFR[0xBF883210]=SFR.DCH2SSA;
SFR[0xBF883214]=SFR.DCH2SSACLR;
SFR[0xBF883218]=SFR.DCH2SSASET;
SFR[0xBF88321C]=SFR.DCH2SSAINV;
SFR.DCH2SSA(0x0);
SFR.DCH2DSA=function(num){
	if (num!=undefined) SFR.DCH2DSA.value=(SFR.DCH2DSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH2DSA.value & 0xffffffff;
};
SFR.DCH2DSACLR=function(num){
	var val=SFR.DCH2DSA.value;
	SFR.DCH2DSA(val&(0xFFFFFFFF-num));
};
SFR.DCH2DSASET=function(num){
	var val=SFR.DCH2DSA.value;
	SFR.DCH2DSA(val|num);
};
SFR.DCH2DSAINV=function(num){
	var val=SFR.DCH2DSA.value;
	SFR.DCH2DSA(val^num);
};
SFR[0xBF883220]=SFR.DCH2DSA;
SFR[0xBF883224]=SFR.DCH2DSACLR;
SFR[0xBF883228]=SFR.DCH2DSASET;
SFR[0xBF88322C]=SFR.DCH2DSAINV;
SFR.DCH2DSA(0x0);
SFR.DCH2SSIZ=function(num){
	if (num!=undefined) SFR.DCH2SSIZ.value=(SFR.DCH2SSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH2SSIZ.value & 0xffff;
};
SFR.DCH2SSIZCLR=function(num){
	var val=SFR.DCH2SSIZ.value;
	SFR.DCH2SSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH2SSIZSET=function(num){
	var val=SFR.DCH2SSIZ.value;
	SFR.DCH2SSIZ(val|num);
};
SFR.DCH2SSIZINV=function(num){
	var val=SFR.DCH2SSIZ.value;
	SFR.DCH2SSIZ(val^num);
};
SFR[0xBF883230]=SFR.DCH2SSIZ;
SFR[0xBF883234]=SFR.DCH2SSIZCLR;
SFR[0xBF883238]=SFR.DCH2SSIZSET;
SFR[0xBF88323C]=SFR.DCH2SSIZINV;
SFR.DCH2SSIZ(0x0);
SFR.DCH2DSIZ=function(num){
	if (num!=undefined) SFR.DCH2DSIZ.value=(SFR.DCH2DSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH2DSIZ.value & 0xffff;
};
SFR.DCH2DSIZCLR=function(num){
	var val=SFR.DCH2DSIZ.value;
	SFR.DCH2DSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH2DSIZSET=function(num){
	var val=SFR.DCH2DSIZ.value;
	SFR.DCH2DSIZ(val|num);
};
SFR.DCH2DSIZINV=function(num){
	var val=SFR.DCH2DSIZ.value;
	SFR.DCH2DSIZ(val^num);
};
SFR[0xBF883240]=SFR.DCH2DSIZ;
SFR[0xBF883244]=SFR.DCH2DSIZCLR;
SFR[0xBF883248]=SFR.DCH2DSIZSET;
SFR[0xBF88324C]=SFR.DCH2DSIZINV;
SFR.DCH2DSIZ(0x0);
SFR.DCH2SPTR=function(num){
	if (num!=undefined) SFR.DCH2SPTR.value=(SFR.DCH2SPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH2SPTR.value & 0xffff;
};
SFR.DCH2SPTRCLR=function(num){
	var val=SFR.DCH2SPTR.value;
	SFR.DCH2SPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH2SPTRSET=function(num){
	var val=SFR.DCH2SPTR.value;
	SFR.DCH2SPTR(val|num);
};
SFR.DCH2SPTRINV=function(num){
	var val=SFR.DCH2SPTR.value;
	SFR.DCH2SPTR(val^num);
};
SFR[0xBF883250]=SFR.DCH2SPTR;
SFR[0xBF883254]=SFR.DCH2SPTRCLR;
SFR[0xBF883258]=SFR.DCH2SPTRSET;
SFR[0xBF88325C]=SFR.DCH2SPTRINV;
SFR.DCH2SPTR(0x0);
SFR.DCH2DPTR=function(num){
	if (num!=undefined) SFR.DCH2DPTR.value=(SFR.DCH2DPTR.value & 0xffffffff) + (num & 0x0);
	return SFR.DCH2DPTR.value & 0xffff;
};
SFR.DCH2DPTRCLR=function(num){
	var val=SFR.DCH2DPTR.value;
	SFR.DCH2DPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH2DPTRSET=function(num){
	var val=SFR.DCH2DPTR.value;
	SFR.DCH2DPTR(val|num);
};
SFR.DCH2DPTRINV=function(num){
	var val=SFR.DCH2DPTR.value;
	SFR.DCH2DPTR(val^num);
};
SFR[0xBF883260]=SFR.DCH2DPTR;
SFR[0xBF883264]=SFR.DCH2DPTRCLR;
SFR[0xBF883268]=SFR.DCH2DPTRSET;
SFR[0xBF88326C]=SFR.DCH2DPTRINV;
SFR.DCH2DPTR(0x0);
SFR.DCH2CSIZ=function(num){
	if (num!=undefined) SFR.DCH2CSIZ.value=(SFR.DCH2CSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH2CSIZ.value & 0xffff;
};
SFR.DCH2CSIZCLR=function(num){
	var val=SFR.DCH2CSIZ.value;
	SFR.DCH2CSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH2CSIZSET=function(num){
	var val=SFR.DCH2CSIZ.value;
	SFR.DCH2CSIZ(val|num);
};
SFR.DCH2CSIZINV=function(num){
	var val=SFR.DCH2CSIZ.value;
	SFR.DCH2CSIZ(val^num);
};
SFR[0xBF883270]=SFR.DCH2CSIZ;
SFR[0xBF883274]=SFR.DCH2CSIZCLR;
SFR[0xBF883278]=SFR.DCH2CSIZSET;
SFR[0xBF88327C]=SFR.DCH2CSIZINV;
SFR.DCH2CSIZ(0x0);
SFR.DCH2CPTR=function(num){
	if (num!=undefined) SFR.DCH2CPTR.value=(SFR.DCH2CPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH2CPTR.value & 0xffff;
};
SFR.DCH2CPTRCLR=function(num){
	var val=SFR.DCH2CPTR.value;
	SFR.DCH2CPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH2CPTRSET=function(num){
	var val=SFR.DCH2CPTR.value;
	SFR.DCH2CPTR(val|num);
};
SFR.DCH2CPTRINV=function(num){
	var val=SFR.DCH2CPTR.value;
	SFR.DCH2CPTR(val^num);
};
SFR[0xBF883280]=SFR.DCH2CPTR;
SFR[0xBF883284]=SFR.DCH2CPTRCLR;
SFR[0xBF883288]=SFR.DCH2CPTRSET;
SFR[0xBF88328C]=SFR.DCH2CPTRINV;
SFR.DCH2CPTR(0x0);
SFR.DCH2DAT=function(num){
	if (num!=undefined) SFR.DCH2DAT.value=(SFR.DCH2DAT.value & 0xffffff00) + (num & 0xff);
	return SFR.DCH2DAT.value & 0xff;
};
SFR.DCH2DATCLR=function(num){
	var val=SFR.DCH2DAT.value;
	SFR.DCH2DAT(val&(0xFFFFFFFF-num));
};
SFR.DCH2DATSET=function(num){
	var val=SFR.DCH2DAT.value;
	SFR.DCH2DAT(val|num);
};
SFR.DCH2DATINV=function(num){
	var val=SFR.DCH2DAT.value;
	SFR.DCH2DAT(val^num);
};
SFR[0xBF883290]=SFR.DCH2DAT;
SFR[0xBF883294]=SFR.DCH2DATCLR;
SFR[0xBF883298]=SFR.DCH2DATSET;
SFR[0xBF88329C]=SFR.DCH2DATINV;
SFR.DCH2DAT(0x0);
SFR.DCH3CON=function(num){
	if (num!=undefined) SFR.DCH3CON.value=(SFR.DCH3CON.value & 0xffff7e0c) + (num & 0x81f3);
	return SFR.DCH3CON.value & 0x81f7;
};
SFR.DCH3CONCLR=function(num){
	var val=SFR.DCH3CON.value;
	SFR.DCH3CON(val&(0xFFFFFFFF-num));
};
SFR.DCH3CONSET=function(num){
	var val=SFR.DCH3CON.value;
	SFR.DCH3CON(val|num);
};
SFR.DCH3CONINV=function(num){
	var val=SFR.DCH3CON.value;
	SFR.DCH3CON(val^num);
};
SFR[0xBF8832A0]=SFR.DCH3CON;
SFR[0xBF8832A4]=SFR.DCH3CONCLR;
SFR[0xBF8832A8]=SFR.DCH3CONSET;
SFR[0xBF8832AC]=SFR.DCH3CONINV;
SFR.DCH3CON(0x0);
SFR.DCH3ECON=function(num){
	if (num!=undefined) SFR.DCH3ECON.value=(SFR.DCH3ECON.value & 0xff0000c7) + (num & 0xffff38);
	return SFR.DCH3ECON.value & 0xfffff8;
};
SFR.DCH3ECONCLR=function(num){
	var val=SFR.DCH3ECON.value;
	SFR.DCH3ECON(val&(0xFFFFFFFF-num));
};
SFR.DCH3ECONSET=function(num){
	var val=SFR.DCH3ECON.value;
	SFR.DCH3ECON(val|num);
};
SFR.DCH3ECONINV=function(num){
	var val=SFR.DCH3ECON.value;
	SFR.DCH3ECON(val^num);
};
SFR[0xBF8832B0]=SFR.DCH3ECON;
SFR[0xBF8832B4]=SFR.DCH3ECONCLR;
SFR[0xBF8832B8]=SFR.DCH3ECONSET;
SFR[0xBF8832BC]=SFR.DCH3ECONINV;
SFR.DCH3ECON(0xffff00);
SFR.DCH3INT=function(num){
	if (num!=undefined) SFR.DCH3INT.value=(SFR.DCH3INT.value & 0xff00ff00) + (num & 0xff00ff);
	return SFR.DCH3INT.value & 0xff00ff;
};
SFR.DCH3INTCLR=function(num){
	var val=SFR.DCH3INT.value;
	SFR.DCH3INT(val&(0xFFFFFFFF-num));
};
SFR.DCH3INTSET=function(num){
	var val=SFR.DCH3INT.value;
	SFR.DCH3INT(val|num);
};
SFR.DCH3INTINV=function(num){
	var val=SFR.DCH3INT.value;
	SFR.DCH3INT(val^num);
};
SFR[0xBF8832C0]=SFR.DCH3INT;
SFR[0xBF8832C4]=SFR.DCH3INTCLR;
SFR[0xBF8832C8]=SFR.DCH3INTSET;
SFR[0xBF8832CC]=SFR.DCH3INTINV;
SFR.DCH3INT(0x0);
SFR.DCH3SSA=function(num){
	if (num!=undefined) SFR.DCH3SSA.value=(SFR.DCH3SSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH3SSA.value & 0xffffffff;
};
SFR.DCH3SSACLR=function(num){
	var val=SFR.DCH3SSA.value;
	SFR.DCH3SSA(val&(0xFFFFFFFF-num));
};
SFR.DCH3SSASET=function(num){
	var val=SFR.DCH3SSA.value;
	SFR.DCH3SSA(val|num);
};
SFR.DCH3SSAINV=function(num){
	var val=SFR.DCH3SSA.value;
	SFR.DCH3SSA(val^num);
};
SFR[0xBF8832D0]=SFR.DCH3SSA;
SFR[0xBF8832D4]=SFR.DCH3SSACLR;
SFR[0xBF8832D8]=SFR.DCH3SSASET;
SFR[0xBF8832DC]=SFR.DCH3SSAINV;
SFR.DCH3SSA(0x0);
SFR.DCH3DSA=function(num){
	if (num!=undefined) SFR.DCH3DSA.value=(SFR.DCH3DSA.value & 0x0) + (num & 0xffffffff);
	return SFR.DCH3DSA.value & 0xffffffff;
};
SFR.DCH3DSACLR=function(num){
	var val=SFR.DCH3DSA.value;
	SFR.DCH3DSA(val&(0xFFFFFFFF-num));
};
SFR.DCH3DSASET=function(num){
	var val=SFR.DCH3DSA.value;
	SFR.DCH3DSA(val|num);
};
SFR.DCH3DSAINV=function(num){
	var val=SFR.DCH3DSA.value;
	SFR.DCH3DSA(val^num);
};
SFR[0xBF8832E0]=SFR.DCH3DSA;
SFR[0xBF8832E4]=SFR.DCH3DSACLR;
SFR[0xBF8832E8]=SFR.DCH3DSASET;
SFR[0xBF8832EC]=SFR.DCH3DSAINV;
SFR.DCH3DSA(0x0);
SFR.DCH3SSIZ=function(num){
	if (num!=undefined) SFR.DCH3SSIZ.value=(SFR.DCH3SSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH3SSIZ.value & 0xffff;
};
SFR.DCH3SSIZCLR=function(num){
	var val=SFR.DCH3SSIZ.value;
	SFR.DCH3SSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH3SSIZSET=function(num){
	var val=SFR.DCH3SSIZ.value;
	SFR.DCH3SSIZ(val|num);
};
SFR.DCH3SSIZINV=function(num){
	var val=SFR.DCH3SSIZ.value;
	SFR.DCH3SSIZ(val^num);
};
SFR[0xBF8832F0]=SFR.DCH3SSIZ;
SFR[0xBF8832F4]=SFR.DCH3SSIZCLR;
SFR[0xBF8832F8]=SFR.DCH3SSIZSET;
SFR[0xBF8832FC]=SFR.DCH3SSIZINV;
SFR.DCH3SSIZ(0x0);
SFR.DCH3DSIZ=function(num){
	if (num!=undefined) SFR.DCH3DSIZ.value=(SFR.DCH3DSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH3DSIZ.value & 0xffff;
};
SFR.DCH3DSIZCLR=function(num){
	var val=SFR.DCH3DSIZ.value;
	SFR.DCH3DSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH3DSIZSET=function(num){
	var val=SFR.DCH3DSIZ.value;
	SFR.DCH3DSIZ(val|num);
};
SFR.DCH3DSIZINV=function(num){
	var val=SFR.DCH3DSIZ.value;
	SFR.DCH3DSIZ(val^num);
};
SFR[0xBF883300]=SFR.DCH3DSIZ;
SFR[0xBF883304]=SFR.DCH3DSIZCLR;
SFR[0xBF883308]=SFR.DCH3DSIZSET;
SFR[0xBF88330C]=SFR.DCH3DSIZINV;
SFR.DCH3DSIZ(0x0);
SFR.DCH3SPTR=function(num){
	if (num!=undefined) SFR.DCH3SPTR.value=(SFR.DCH3SPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH3SPTR.value & 0xffff;
};
SFR.DCH3SPTRCLR=function(num){
	var val=SFR.DCH3SPTR.value;
	SFR.DCH3SPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH3SPTRSET=function(num){
	var val=SFR.DCH3SPTR.value;
	SFR.DCH3SPTR(val|num);
};
SFR.DCH3SPTRINV=function(num){
	var val=SFR.DCH3SPTR.value;
	SFR.DCH3SPTR(val^num);
};
SFR[0xBF883310]=SFR.DCH3SPTR;
SFR[0xBF883314]=SFR.DCH3SPTRCLR;
SFR[0xBF883318]=SFR.DCH3SPTRSET;
SFR[0xBF88331C]=SFR.DCH3SPTRINV;
SFR.DCH3SPTR(0x0);
SFR.DCH3DPTR=function(num){
	if (num!=undefined) SFR.DCH3DPTR.value=(SFR.DCH3DPTR.value & 0xffffffff) + (num & 0x0);
	return SFR.DCH3DPTR.value & 0xffff;
};
SFR.DCH3DPTRCLR=function(num){
	var val=SFR.DCH3DPTR.value;
	SFR.DCH3DPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH3DPTRSET=function(num){
	var val=SFR.DCH3DPTR.value;
	SFR.DCH3DPTR(val|num);
};
SFR.DCH3DPTRINV=function(num){
	var val=SFR.DCH3DPTR.value;
	SFR.DCH3DPTR(val^num);
};
SFR[0xBF883320]=SFR.DCH3DPTR;
SFR[0xBF883324]=SFR.DCH3DPTRCLR;
SFR[0xBF883328]=SFR.DCH3DPTRSET;
SFR[0xBF88332C]=SFR.DCH3DPTRINV;
SFR.DCH3DPTR(0x0);
SFR.DCH3CSIZ=function(num){
	if (num!=undefined) SFR.DCH3CSIZ.value=(SFR.DCH3CSIZ.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH3CSIZ.value & 0xffff;
};
SFR.DCH3CSIZCLR=function(num){
	var val=SFR.DCH3CSIZ.value;
	SFR.DCH3CSIZ(val&(0xFFFFFFFF-num));
};
SFR.DCH3CSIZSET=function(num){
	var val=SFR.DCH3CSIZ.value;
	SFR.DCH3CSIZ(val|num);
};
SFR.DCH3CSIZINV=function(num){
	var val=SFR.DCH3CSIZ.value;
	SFR.DCH3CSIZ(val^num);
};
SFR[0xBF883330]=SFR.DCH3CSIZ;
SFR[0xBF883334]=SFR.DCH3CSIZCLR;
SFR[0xBF883338]=SFR.DCH3CSIZSET;
SFR[0xBF88333C]=SFR.DCH3CSIZINV;
SFR.DCH3CSIZ(0x0);
SFR.DCH3CPTR=function(num){
	if (num!=undefined) SFR.DCH3CPTR.value=(SFR.DCH3CPTR.value & 0xffff0000) + (num & 0xffff);
	return SFR.DCH3CPTR.value & 0xffff;
};
SFR.DCH3CPTRCLR=function(num){
	var val=SFR.DCH3CPTR.value;
	SFR.DCH3CPTR(val&(0xFFFFFFFF-num));
};
SFR.DCH3CPTRSET=function(num){
	var val=SFR.DCH3CPTR.value;
	SFR.DCH3CPTR(val|num);
};
SFR.DCH3CPTRINV=function(num){
	var val=SFR.DCH3CPTR.value;
	SFR.DCH3CPTR(val^num);
};
SFR[0xBF883340]=SFR.DCH3CPTR;
SFR[0xBF883344]=SFR.DCH3CPTRCLR;
SFR[0xBF883348]=SFR.DCH3CPTRSET;
SFR[0xBF88334C]=SFR.DCH3CPTRINV;
SFR.DCH3CPTR(0x0);
SFR.DCH3DAT=function(num){
	if (num!=undefined) SFR.DCH3DAT.value=(SFR.DCH3DAT.value & 0xffffff00) + (num & 0xff);
	return SFR.DCH3DAT.value & 0xff;
};
SFR.DCH3DATCLR=function(num){
	var val=SFR.DCH3DAT.value;
	SFR.DCH3DAT(val&(0xFFFFFFFF-num));
};
SFR.DCH3DATSET=function(num){
	var val=SFR.DCH3DAT.value;
	SFR.DCH3DAT(val|num);
};
SFR.DCH3DATINV=function(num){
	var val=SFR.DCH3DAT.value;
	SFR.DCH3DAT(val^num);
};
SFR[0xBF883350]=SFR.DCH3DAT;
SFR[0xBF883354]=SFR.DCH3DATCLR;
SFR[0xBF883358]=SFR.DCH3DATSET;
SFR[0xBF88335C]=SFR.DCH3DATINV;
SFR.DCH3DAT(0x0);
SFR.CHECON=function(num){
	if (num!=undefined) SFR.CHECON.value=(SFR.CHECON.value & 0xfffefcc8) + (num & 0x10337);
	return SFR.CHECON.value & 0x10337;
};
SFR.CHECONCLR=function(num){
	var val=SFR.CHECON.value;
	SFR.CHECON(val&(0xFFFFFFFF-num));
};
SFR.CHECONSET=function(num){
	var val=SFR.CHECON.value;
	SFR.CHECON(val|num);
};
SFR.CHECONINV=function(num){
	var val=SFR.CHECON.value;
	SFR.CHECON(val^num);
};
SFR[0xBF884000]=SFR.CHECON;
SFR[0xBF884004]=SFR.CHECONCLR;
SFR[0xBF884008]=SFR.CHECONSET;
SFR[0xBF88400C]=SFR.CHECONINV;
SFR.CHECON(0x7);
SFR.CHEACC=function(num){
	if (num!=undefined) SFR.CHEACC.value=(SFR.CHEACC.value & 0x0) + (num & 0xffffffff);
	return SFR.CHEACC.value & 0xffffffff;
};
SFR.CHEACCCLR=function(num){
	var val=SFR.CHEACC.value;
	SFR.CHEACC(val&(0xFFFFFFFF-num));
};
SFR.CHEACCSET=function(num){
	var val=SFR.CHEACC.value;
	SFR.CHEACC(val|num);
};
SFR.CHEACCINV=function(num){
	var val=SFR.CHEACC.value;
	SFR.CHEACC(val^num);
};
SFR[0xBF884010]=SFR.CHEACC;
SFR[0xBF884014]=SFR.CHEACCCLR;
SFR[0xBF884018]=SFR.CHEACCSET;
SFR[0xBF88401C]=SFR.CHEACCINV;
SFR.CHEACC(0x0);
SFR.CHETAG=function(num){
	if (num!=undefined) SFR.CHETAG.value=(SFR.CHETAG.value & 0x7f000001) + (num & 0x80fffffe);
	return SFR.CHETAG.value & 0x80fffffe;
};
SFR.CHETAGCLR=function(num){
	var val=SFR.CHETAG.value;
	SFR.CHETAG(val&(0xFFFFFFFF-num));
};
SFR.CHETAGSET=function(num){
	var val=SFR.CHETAG.value;
	SFR.CHETAG(val|num);
};
SFR.CHETAGINV=function(num){
	var val=SFR.CHETAG.value;
	SFR.CHETAG(val^num);
};
SFR[0xBF884020]=SFR.CHETAG;
SFR[0xBF884024]=SFR.CHETAGCLR;
SFR[0xBF884028]=SFR.CHETAGSET;
SFR[0xBF88402C]=SFR.CHETAGINV;
SFR.CHETAG(0x2);
SFR.CHEMSK=function(num){
	if (num!=undefined) SFR.CHEMSK.value=(SFR.CHEMSK.value & 0x0) + (num & 0xffffffff);
	return SFR.CHEMSK.value & 0xffffffff;
};
SFR.CHEMSKCLR=function(num){
	var val=SFR.CHEMSK.value;
	SFR.CHEMSK(val&(0xFFFFFFFF-num));
};
SFR.CHEMSKSET=function(num){
	var val=SFR.CHEMSK.value;
	SFR.CHEMSK(val|num);
};
SFR.CHEMSKINV=function(num){
	var val=SFR.CHEMSK.value;
	SFR.CHEMSK(val^num);
};
SFR[0xBF884030]=SFR.CHEMSK;
SFR[0xBF884034]=SFR.CHEMSKCLR;
SFR[0xBF884038]=SFR.CHEMSKSET;
SFR[0xBF88403C]=SFR.CHEMSKINV;
SFR.CHEMSK(0x0);
SFR.ANSELB=function(num){
	if (num!=undefined) SFR.ANSELB.value=(SFR.ANSELB.value & 0xffff0000) + (num & 0xffff);
	return SFR.ANSELB.value & 0xffff;
};
SFR.ANSELBCLR=function(num){
	var val=SFR.ANSELB.value;
	SFR.ANSELB(val&(0xFFFFFFFF-num));
};
SFR.ANSELBSET=function(num){
	var val=SFR.ANSELB.value;
	SFR.ANSELB(val|num);
};
SFR.ANSELBINV=function(num){
	var val=SFR.ANSELB.value;
	SFR.ANSELB(val^num);
};
SFR[0xBF886100]=SFR.ANSELB;
SFR[0xBF886104]=SFR.ANSELBCLR;
SFR[0xBF886108]=SFR.ANSELBSET;
SFR[0xBF88610C]=SFR.ANSELBINV;
SFR.ANSELB(0x0);
SFR.TRISB=function(num){
	if (num!=undefined) SFR.TRISB.value=(SFR.TRISB.value & 0xffff0000) + (num & 0xffff);
	return SFR.TRISB.value & 0xffff;
};
SFR.TRISBCLR=function(num){
	var val=SFR.TRISB.value;
	SFR.TRISB(val&(0xFFFFFFFF-num));
};
SFR.TRISBSET=function(num){
	var val=SFR.TRISB.value;
	SFR.TRISB(val|num);
};
SFR.TRISBINV=function(num){
	var val=SFR.TRISB.value;
	SFR.TRISB(val^num);
};
SFR[0xBF886110]=SFR.TRISB;
SFR[0xBF886114]=SFR.TRISBCLR;
SFR[0xBF886118]=SFR.TRISBSET;
SFR[0xBF88611C]=SFR.TRISBINV;
SFR.TRISB(0xffff);
SFR.PORTB=function(num){
	if (num!=undefined) SFR.PORTB.value=(SFR.PORTB.value & 0xffff0000) + (num & 0xffff);
	return SFR.PORTB.value & 0xffff;
};
SFR.PORTBCLR=function(num){
	var val=SFR.PORTB.value;
	SFR.PORTB(val&(0xFFFFFFFF-num));
};
SFR.PORTBSET=function(num){
	var val=SFR.PORTB.value;
	SFR.PORTB(val|num);
};
SFR.PORTBINV=function(num){
	var val=SFR.PORTB.value;
	SFR.PORTB(val^num);
};
SFR[0xBF886120]=SFR.PORTB;
SFR[0xBF886124]=SFR.PORTBCLR;
SFR[0xBF886128]=SFR.PORTBSET;
SFR[0xBF88612C]=SFR.PORTBINV;
SFR.PORTB(0x0);
SFR.LATB=function(num){
	if (num!=undefined) SFR.LATB.value=(SFR.LATB.value & 0xffff0000) + (num & 0xffff);
	return SFR.LATB.value & 0xffff;
};
SFR.LATBCLR=function(num){
	var val=SFR.LATB.value;
	SFR.LATB(val&(0xFFFFFFFF-num));
};
SFR.LATBSET=function(num){
	var val=SFR.LATB.value;
	SFR.LATB(val|num);
};
SFR.LATBINV=function(num){
	var val=SFR.LATB.value;
	SFR.LATB(val^num);
};
SFR[0xBF886130]=SFR.LATB;
SFR[0xBF886134]=SFR.LATBCLR;
SFR[0xBF886138]=SFR.LATBSET;
SFR[0xBF88613C]=SFR.LATBINV;
SFR.LATB(0x0);
SFR.ODCB=function(num){
	if (num!=undefined) SFR.ODCB.value=(SFR.ODCB.value & 0xffff0000) + (num & 0xffff);
	return SFR.ODCB.value & 0xffff;
};
SFR.ODCBCLR=function(num){
	var val=SFR.ODCB.value;
	SFR.ODCB(val&(0xFFFFFFFF-num));
};
SFR.ODCBSET=function(num){
	var val=SFR.ODCB.value;
	SFR.ODCB(val|num);
};
SFR.ODCBINV=function(num){
	var val=SFR.ODCB.value;
	SFR.ODCB(val^num);
};
SFR[0xBF886140]=SFR.ODCB;
SFR[0xBF886144]=SFR.ODCBCLR;
SFR[0xBF886148]=SFR.ODCBSET;
SFR[0xBF88614C]=SFR.ODCBINV;
SFR.ODCB(0x0);
SFR.CNPUB=function(num){
	if (num!=undefined) SFR.CNPUB.value=(SFR.CNPUB.value & 0xffff0000) + (num & 0xffff);
	return SFR.CNPUB.value & 0xffff;
};
SFR.CNPUBCLR=function(num){
	var val=SFR.CNPUB.value;
	SFR.CNPUB(val&(0xFFFFFFFF-num));
};
SFR.CNPUBSET=function(num){
	var val=SFR.CNPUB.value;
	SFR.CNPUB(val|num);
};
SFR.CNPUBINV=function(num){
	var val=SFR.CNPUB.value;
	SFR.CNPUB(val^num);
};
SFR[0xBF886150]=SFR.CNPUB;
SFR[0xBF886154]=SFR.CNPUBCLR;
SFR[0xBF886158]=SFR.CNPUBSET;
SFR[0xBF88615C]=SFR.CNPUBINV;
SFR.CNPUB(0x0);
SFR.CNPDB=function(num){
	if (num!=undefined) SFR.CNPDB.value=(SFR.CNPDB.value & 0xffff0000) + (num & 0xffff);
	return SFR.CNPDB.value & 0xffff;
};
SFR.CNPDBCLR=function(num){
	var val=SFR.CNPDB.value;
	SFR.CNPDB(val&(0xFFFFFFFF-num));
};
SFR.CNPDBSET=function(num){
	var val=SFR.CNPDB.value;
	SFR.CNPDB(val|num);
};
SFR.CNPDBINV=function(num){
	var val=SFR.CNPDB.value;
	SFR.CNPDB(val^num);
};
SFR[0xBF886160]=SFR.CNPDB;
SFR[0xBF886164]=SFR.CNPDBCLR;
SFR[0xBF886168]=SFR.CNPDBSET;
SFR[0xBF88616C]=SFR.CNPDBINV;
SFR.CNPDB(0x0);
SFR.CNCONB=function(num){
	if (num!=undefined) SFR.CNCONB.value=(SFR.CNCONB.value & 0xffff5fff) + (num & 0xa000);
	return SFR.CNCONB.value & 0xa000;
};
SFR.CNCONBCLR=function(num){
	var val=SFR.CNCONB.value;
	SFR.CNCONB(val&(0xFFFFFFFF-num));
};
SFR.CNCONBSET=function(num){
	var val=SFR.CNCONB.value;
	SFR.CNCONB(val|num);
};
SFR.CNCONBINV=function(num){
	var val=SFR.CNCONB.value;
	SFR.CNCONB(val^num);
};
SFR[0xBF886170]=SFR.CNCONB;
SFR[0xBF886174]=SFR.CNCONBCLR;
SFR[0xBF886178]=SFR.CNCONBSET;
SFR[0xBF88617C]=SFR.CNCONBINV;
SFR.CNCONB(0x0);
SFR.CNENB=function(num){
	if (num!=undefined) SFR.CNENB.value=(SFR.CNENB.value & 0xffff0000) + (num & 0xffff);
	return SFR.CNENB.value & 0xffff;
};
SFR.CNENBCLR=function(num){
	var val=SFR.CNENB.value;
	SFR.CNENB(val&(0xFFFFFFFF-num));
};
SFR.CNENBSET=function(num){
	var val=SFR.CNENB.value;
	SFR.CNENB(val|num);
};
SFR.CNENBINV=function(num){
	var val=SFR.CNENB.value;
	SFR.CNENB(val^num);
};
SFR[0xBF886180]=SFR.CNENB;
SFR[0xBF886184]=SFR.CNENBCLR;
SFR[0xBF886188]=SFR.CNENBSET;
SFR[0xBF88618C]=SFR.CNENBINV;
SFR.CNENB(0x0);
SFR.CNSTATB=function(num){
	if (num!=undefined) SFR.CNSTATB.value=(SFR.CNSTATB.value & 0xffff0000) + (num & 0xffff);
	return SFR.CNSTATB.value & 0xffff;
};
SFR.CNSTATBCLR=function(num){
	var val=SFR.CNSTATB.value;
	SFR.CNSTATB(val&(0xFFFFFFFF-num));
};
SFR.CNSTATBSET=function(num){
	var val=SFR.CNSTATB.value;
	SFR.CNSTATB(val|num);
};
SFR.CNSTATBINV=function(num){
	var val=SFR.CNSTATB.value;
	SFR.CNSTATB(val^num);
};
SFR[0xBF886190]=SFR.CNSTATB;
SFR[0xBF886194]=SFR.CNSTATBCLR;
SFR[0xBF886198]=SFR.CNSTATBSET;
SFR[0xBF88619C]=SFR.CNSTATBINV;
SFR.CNSTATB(0x0);
SFR.ANSELC=function(num){
	if (num!=undefined) SFR.ANSELC.value=(SFR.ANSELC.value & 0xffffffff) + (num & 0x0);
	return SFR.ANSELC.value & 0x0;
};
SFR.ANSELCCLR=function(num){
	var val=SFR.ANSELC.value;
	SFR.ANSELC(val&(0xFFFFFFFF-num));
};
SFR.ANSELCSET=function(num){
	var val=SFR.ANSELC.value;
	SFR.ANSELC(val|num);
};
SFR.ANSELCINV=function(num){
	var val=SFR.ANSELC.value;
	SFR.ANSELC(val^num);
};
SFR[0xBF886200]=SFR.ANSELC;
SFR[0xBF886204]=SFR.ANSELCCLR;
SFR[0xBF886208]=SFR.ANSELCSET;
SFR[0xBF88620C]=SFR.ANSELCINV;
SFR.ANSELC(0x0);
SFR.TRISC=function(num){
	if (num!=undefined) SFR.TRISC.value=(SFR.TRISC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.TRISC.value & 0xf000;
};
SFR.TRISCCLR=function(num){
	var val=SFR.TRISC.value;
	SFR.TRISC(val&(0xFFFFFFFF-num));
};
SFR.TRISCSET=function(num){
	var val=SFR.TRISC.value;
	SFR.TRISC(val|num);
};
SFR.TRISCINV=function(num){
	var val=SFR.TRISC.value;
	SFR.TRISC(val^num);
};
SFR[0xBF886210]=SFR.TRISC;
SFR[0xBF886214]=SFR.TRISCCLR;
SFR[0xBF886218]=SFR.TRISCSET;
SFR[0xBF88621C]=SFR.TRISCINV;
SFR.TRISC(0xf000);
SFR.PORTC=function(num){
	if (num!=undefined) SFR.PORTC.value=(SFR.PORTC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.PORTC.value & 0xf000;
};
SFR.PORTCCLR=function(num){
	var val=SFR.PORTC.value;
	SFR.PORTC(val&(0xFFFFFFFF-num));
};
SFR.PORTCSET=function(num){
	var val=SFR.PORTC.value;
	SFR.PORTC(val|num);
};
SFR.PORTCINV=function(num){
	var val=SFR.PORTC.value;
	SFR.PORTC(val^num);
};
SFR[0xBF886220]=SFR.PORTC;
SFR[0xBF886224]=SFR.PORTCCLR;
SFR[0xBF886228]=SFR.PORTCSET;
SFR[0xBF88622C]=SFR.PORTCINV;
SFR.PORTC(0x0);
SFR.LATC=function(num){
	if (num!=undefined) SFR.LATC.value=(SFR.LATC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.LATC.value & 0xf000;
};
SFR.LATCCLR=function(num){
	var val=SFR.LATC.value;
	SFR.LATC(val&(0xFFFFFFFF-num));
};
SFR.LATCSET=function(num){
	var val=SFR.LATC.value;
	SFR.LATC(val|num);
};
SFR.LATCINV=function(num){
	var val=SFR.LATC.value;
	SFR.LATC(val^num);
};
SFR[0xBF886230]=SFR.LATC;
SFR[0xBF886234]=SFR.LATCCLR;
SFR[0xBF886238]=SFR.LATCSET;
SFR[0xBF88623C]=SFR.LATCINV;
SFR.LATC(0x0);
SFR.ODCC=function(num){
	if (num!=undefined) SFR.ODCC.value=(SFR.ODCC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.ODCC.value & 0xf000;
};
SFR.ODCCCLR=function(num){
	var val=SFR.ODCC.value;
	SFR.ODCC(val&(0xFFFFFFFF-num));
};
SFR.ODCCSET=function(num){
	var val=SFR.ODCC.value;
	SFR.ODCC(val|num);
};
SFR.ODCCINV=function(num){
	var val=SFR.ODCC.value;
	SFR.ODCC(val^num);
};
SFR[0xBF886240]=SFR.ODCC;
SFR[0xBF886244]=SFR.ODCCCLR;
SFR[0xBF886248]=SFR.ODCCSET;
SFR[0xBF88624C]=SFR.ODCCINV;
SFR.ODCC(0x0);
SFR.CNPUC=function(num){
	if (num!=undefined) SFR.CNPUC.value=(SFR.CNPUC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.CNPUC.value & 0xf000;
};
SFR.CNPUCCLR=function(num){
	var val=SFR.CNPUC.value;
	SFR.CNPUC(val&(0xFFFFFFFF-num));
};
SFR.CNPUCSET=function(num){
	var val=SFR.CNPUC.value;
	SFR.CNPUC(val|num);
};
SFR.CNPUCINV=function(num){
	var val=SFR.CNPUC.value;
	SFR.CNPUC(val^num);
};
SFR[0xBF886250]=SFR.CNPUC;
SFR[0xBF886254]=SFR.CNPUCCLR;
SFR[0xBF886258]=SFR.CNPUCSET;
SFR[0xBF88625C]=SFR.CNPUCINV;
SFR.CNPUC(0x0);
SFR.CNPDC=function(num){
	if (num!=undefined) SFR.CNPDC.value=(SFR.CNPDC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.CNPDC.value & 0xf000;
};
SFR.CNPDCCLR=function(num){
	var val=SFR.CNPDC.value;
	SFR.CNPDC(val&(0xFFFFFFFF-num));
};
SFR.CNPDCSET=function(num){
	var val=SFR.CNPDC.value;
	SFR.CNPDC(val|num);
};
SFR.CNPDCINV=function(num){
	var val=SFR.CNPDC.value;
	SFR.CNPDC(val^num);
};
SFR[0xBF886260]=SFR.CNPDC;
SFR[0xBF886264]=SFR.CNPDCCLR;
SFR[0xBF886268]=SFR.CNPDCSET;
SFR[0xBF88626C]=SFR.CNPDCINV;
SFR.CNPDC(0x0);
SFR.CNCONC=function(num){
	if (num!=undefined) SFR.CNCONC.value=(SFR.CNCONC.value & 0xffff5fff) + (num & 0xa000);
	return SFR.CNCONC.value & 0xa000;
};
SFR.CNCONCCLR=function(num){
	var val=SFR.CNCONC.value;
	SFR.CNCONC(val&(0xFFFFFFFF-num));
};
SFR.CNCONCSET=function(num){
	var val=SFR.CNCONC.value;
	SFR.CNCONC(val|num);
};
SFR.CNCONCINV=function(num){
	var val=SFR.CNCONC.value;
	SFR.CNCONC(val^num);
};
SFR[0xBF886270]=SFR.CNCONC;
SFR[0xBF886274]=SFR.CNCONCCLR;
SFR[0xBF886278]=SFR.CNCONCSET;
SFR[0xBF88627C]=SFR.CNCONCINV;
SFR.CNCONC(0x0);
SFR.CNENC=function(num){
	if (num!=undefined) SFR.CNENC.value=(SFR.CNENC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.CNENC.value & 0xf000;
};
SFR.CNENCCLR=function(num){
	var val=SFR.CNENC.value;
	SFR.CNENC(val&(0xFFFFFFFF-num));
};
SFR.CNENCSET=function(num){
	var val=SFR.CNENC.value;
	SFR.CNENC(val|num);
};
SFR.CNENCINV=function(num){
	var val=SFR.CNENC.value;
	SFR.CNENC(val^num);
};
SFR[0xBF886280]=SFR.CNENC;
SFR[0xBF886284]=SFR.CNENCCLR;
SFR[0xBF886288]=SFR.CNENCSET;
SFR[0xBF88628C]=SFR.CNENCINV;
SFR.CNENC(0x0);
SFR.CNSTATC=function(num){
	if (num!=undefined) SFR.CNSTATC.value=(SFR.CNSTATC.value & 0xffff0fff) + (num & 0xf000);
	return SFR.CNSTATC.value & 0xf000;
};
SFR.CNSTATCCLR=function(num){
	var val=SFR.CNSTATC.value;
	SFR.CNSTATC(val&(0xFFFFFFFF-num));
};
SFR.CNSTATCSET=function(num){
	var val=SFR.CNSTATC.value;
	SFR.CNSTATC(val|num);
};
SFR.CNSTATCINV=function(num){
	var val=SFR.CNSTATC.value;
	SFR.CNSTATC(val^num);
};
SFR[0xBF886290]=SFR.CNSTATC;
SFR[0xBF886294]=SFR.CNSTATCCLR;
SFR[0xBF886298]=SFR.CNSTATCSET;
SFR[0xBF88629C]=SFR.CNSTATCINV;
SFR.CNSTATC(0x0);
SFR.ANSELD=function(num){
	if (num!=undefined) SFR.ANSELD.value=(SFR.ANSELD.value & 0xfffffff1) + (num & 0xe);
	return SFR.ANSELD.value & 0xe;
};
SFR.ANSELDCLR=function(num){
	var val=SFR.ANSELD.value;
	SFR.ANSELD(val&(0xFFFFFFFF-num));
};
SFR.ANSELDSET=function(num){
	var val=SFR.ANSELD.value;
	SFR.ANSELD(val|num);
};
SFR.ANSELDINV=function(num){
	var val=SFR.ANSELD.value;
	SFR.ANSELD(val^num);
};
SFR[0xBF886300]=SFR.ANSELD;
SFR[0xBF886304]=SFR.ANSELDCLR;
SFR[0xBF886308]=SFR.ANSELDSET;
SFR[0xBF88630C]=SFR.ANSELDINV;
SFR.ANSELD(0x0);
SFR.TRISD=function(num){
	if (num!=undefined) SFR.TRISD.value=(SFR.TRISD.value & 0xfffff000) + (num & 0xfff);
	return SFR.TRISD.value & 0xfff;
};
SFR.TRISDCLR=function(num){
	var val=SFR.TRISD.value;
	SFR.TRISD(val&(0xFFFFFFFF-num));
};
SFR.TRISDSET=function(num){
	var val=SFR.TRISD.value;
	SFR.TRISD(val|num);
};
SFR.TRISDINV=function(num){
	var val=SFR.TRISD.value;
	SFR.TRISD(val^num);
};
SFR[0xBF886310]=SFR.TRISD;
SFR[0xBF886314]=SFR.TRISDCLR;
SFR[0xBF886318]=SFR.TRISDSET;
SFR[0xBF88631C]=SFR.TRISDINV;
SFR.TRISD(0xfff);
SFR.PORTD=function(num){
	if (num!=undefined) SFR.PORTD.value=(SFR.PORTD.value & 0xfffff000) + (num & 0xfff);
	return SFR.PORTD.value & 0xfff;
};
SFR.PORTDCLR=function(num){
	var val=SFR.PORTD.value;
	SFR.PORTD(val&(0xFFFFFFFF-num));
};
SFR.PORTDSET=function(num){
	var val=SFR.PORTD.value;
	SFR.PORTD(val|num);
};
SFR.PORTDINV=function(num){
	var val=SFR.PORTD.value;
	SFR.PORTD(val^num);
};
SFR[0xBF886320]=SFR.PORTD;
SFR[0xBF886324]=SFR.PORTDCLR;
SFR[0xBF886328]=SFR.PORTDSET;
SFR[0xBF88632C]=SFR.PORTDINV;
SFR.PORTD(0x0);
SFR.LATD=function(num){
	if (num!=undefined) SFR.LATD.value=(SFR.LATD.value & 0xfffff000) + (num & 0xfff);
	return SFR.LATD.value & 0xfff;
};
SFR.LATDCLR=function(num){
	var val=SFR.LATD.value;
	SFR.LATD(val&(0xFFFFFFFF-num));
};
SFR.LATDSET=function(num){
	var val=SFR.LATD.value;
	SFR.LATD(val|num);
};
SFR.LATDINV=function(num){
	var val=SFR.LATD.value;
	SFR.LATD(val^num);
};
SFR[0xBF886330]=SFR.LATD;
SFR[0xBF886334]=SFR.LATDCLR;
SFR[0xBF886338]=SFR.LATDSET;
SFR[0xBF88633C]=SFR.LATDINV;
SFR.LATD(0x0);
SFR.ODCD=function(num){
	if (num!=undefined) SFR.ODCD.value=(SFR.ODCD.value & 0xfffff000) + (num & 0xfff);
	return SFR.ODCD.value & 0xfff;
};
SFR.ODCDCLR=function(num){
	var val=SFR.ODCD.value;
	SFR.ODCD(val&(0xFFFFFFFF-num));
};
SFR.ODCDSET=function(num){
	var val=SFR.ODCD.value;
	SFR.ODCD(val|num);
};
SFR.ODCDINV=function(num){
	var val=SFR.ODCD.value;
	SFR.ODCD(val^num);
};
SFR[0xBF886340]=SFR.ODCD;
SFR[0xBF886344]=SFR.ODCDCLR;
SFR[0xBF886348]=SFR.ODCDSET;
SFR[0xBF88634C]=SFR.ODCDINV;
SFR.ODCD(0x0);
SFR.CNPUD=function(num){
	if (num!=undefined) SFR.CNPUD.value=(SFR.CNPUD.value & 0xfffff000) + (num & 0xfff);
	return SFR.CNPUD.value & 0xfff;
};
SFR.CNPUDCLR=function(num){
	var val=SFR.CNPUD.value;
	SFR.CNPUD(val&(0xFFFFFFFF-num));
};
SFR.CNPUDSET=function(num){
	var val=SFR.CNPUD.value;
	SFR.CNPUD(val|num);
};
SFR.CNPUDINV=function(num){
	var val=SFR.CNPUD.value;
	SFR.CNPUD(val^num);
};
SFR[0xBF886350]=SFR.CNPUD;
SFR[0xBF886354]=SFR.CNPUDCLR;
SFR[0xBF886358]=SFR.CNPUDSET;
SFR[0xBF88635C]=SFR.CNPUDINV;
SFR.CNPUD(0x0);
SFR.CNPDD=function(num){
	if (num!=undefined) SFR.CNPDD.value=(SFR.CNPDD.value & 0xfffff000) + (num & 0xfff);
	return SFR.CNPDD.value & 0xfff;
};
SFR.CNPDDCLR=function(num){
	var val=SFR.CNPDD.value;
	SFR.CNPDD(val&(0xFFFFFFFF-num));
};
SFR.CNPDDSET=function(num){
	var val=SFR.CNPDD.value;
	SFR.CNPDD(val|num);
};
SFR.CNPDDINV=function(num){
	var val=SFR.CNPDD.value;
	SFR.CNPDD(val^num);
};
SFR[0xBF886360]=SFR.CNPDD;
SFR[0xBF886364]=SFR.CNPDDCLR;
SFR[0xBF886368]=SFR.CNPDDSET;
SFR[0xBF88636C]=SFR.CNPDDINV;
SFR.CNPDD(0x0);
SFR.CNCOND=function(num){
	if (num!=undefined) SFR.CNCOND.value=(SFR.CNCOND.value & 0xffff5fff) + (num & 0xa000);
	return SFR.CNCOND.value & 0xa000;
};
SFR.CNCONDCLR=function(num){
	var val=SFR.CNCOND.value;
	SFR.CNCOND(val&(0xFFFFFFFF-num));
};
SFR.CNCONDSET=function(num){
	var val=SFR.CNCOND.value;
	SFR.CNCOND(val|num);
};
SFR.CNCONDINV=function(num){
	var val=SFR.CNCOND.value;
	SFR.CNCOND(val^num);
};
SFR[0xBF886370]=SFR.CNCOND;
SFR[0xBF886374]=SFR.CNCONDCLR;
SFR[0xBF886378]=SFR.CNCONDSET;
SFR[0xBF88637C]=SFR.CNCONDINV;
SFR.CNCOND(0x0);
SFR.CNEND=function(num){
	if (num!=undefined) SFR.CNEND.value=(SFR.CNEND.value & 0xfffff000) + (num & 0xfff);
	return SFR.CNEND.value & 0xfff;
};
SFR.CNENDCLR=function(num){
	var val=SFR.CNEND.value;
	SFR.CNEND(val&(0xFFFFFFFF-num));
};
SFR.CNENDSET=function(num){
	var val=SFR.CNEND.value;
	SFR.CNEND(val|num);
};
SFR.CNENDINV=function(num){
	var val=SFR.CNEND.value;
	SFR.CNEND(val^num);
};
SFR[0xBF886380]=SFR.CNEND;
SFR[0xBF886384]=SFR.CNENDCLR;
SFR[0xBF886388]=SFR.CNENDSET;
SFR[0xBF88638C]=SFR.CNENDINV;
SFR.CNEND(0x0);
SFR.CNSTATD=function(num){
	if (num!=undefined) SFR.CNSTATD.value=(SFR.CNSTATD.value & 0xfffff000) + (num & 0xfff);
	return SFR.CNSTATD.value & 0xfff;
};
SFR.CNSTATDCLR=function(num){
	var val=SFR.CNSTATD.value;
	SFR.CNSTATD(val&(0xFFFFFFFF-num));
};
SFR.CNSTATDSET=function(num){
	var val=SFR.CNSTATD.value;
	SFR.CNSTATD(val|num);
};
SFR.CNSTATDINV=function(num){
	var val=SFR.CNSTATD.value;
	SFR.CNSTATD(val^num);
};
SFR[0xBF886390]=SFR.CNSTATD;
SFR[0xBF886394]=SFR.CNSTATDCLR;
SFR[0xBF886398]=SFR.CNSTATDSET;
SFR[0xBF88639C]=SFR.CNSTATDINV;
SFR.CNSTATD(0x0);
SFR.ANSELE=function(num){
	if (num!=undefined) SFR.ANSELE.value=(SFR.ANSELE.value & 0xffffff0b) + (num & 0xf4);
	return SFR.ANSELE.value & 0xf4;
};
SFR.ANSELECLR=function(num){
	var val=SFR.ANSELE.value;
	SFR.ANSELE(val&(0xFFFFFFFF-num));
};
SFR.ANSELESET=function(num){
	var val=SFR.ANSELE.value;
	SFR.ANSELE(val|num);
};
SFR.ANSELEINV=function(num){
	var val=SFR.ANSELE.value;
	SFR.ANSELE(val^num);
};
SFR[0xBF886400]=SFR.ANSELE;
SFR[0xBF886404]=SFR.ANSELECLR;
SFR[0xBF886408]=SFR.ANSELESET;
SFR[0xBF88640C]=SFR.ANSELEINV;
SFR.ANSELE(0x0);
SFR.TRISE=function(num){
	if (num!=undefined) SFR.TRISE.value=(SFR.TRISE.value & 0xffffff00) + (num & 0xff);
	return SFR.TRISE.value & 0xff;
};
SFR.TRISECLR=function(num){
	var val=SFR.TRISE.value;
	SFR.TRISE(val&(0xFFFFFFFF-num));
};
SFR.TRISESET=function(num){
	var val=SFR.TRISE.value;
	SFR.TRISE(val|num);
};
SFR.TRISEINV=function(num){
	var val=SFR.TRISE.value;
	SFR.TRISE(val^num);
};
SFR[0xBF886410]=SFR.TRISE;
SFR[0xBF886414]=SFR.TRISECLR;
SFR[0xBF886418]=SFR.TRISESET;
SFR[0xBF88641C]=SFR.TRISEINV;
SFR.TRISE(0xff);
SFR.PORTE=function(num){
	if (num!=undefined) SFR.PORTE.value=(SFR.PORTE.value & 0xffffff00) + (num & 0xff);
	return SFR.PORTE.value & 0xff;
};
SFR.PORTECLR=function(num){
	var val=SFR.PORTE.value;
	SFR.PORTE(val&(0xFFFFFFFF-num));
};
SFR.PORTESET=function(num){
	var val=SFR.PORTE.value;
	SFR.PORTE(val|num);
};
SFR.PORTEINV=function(num){
	var val=SFR.PORTE.value;
	SFR.PORTE(val^num);
};
SFR[0xBF886420]=SFR.PORTE;
SFR[0xBF886424]=SFR.PORTECLR;
SFR[0xBF886428]=SFR.PORTESET;
SFR[0xBF88642C]=SFR.PORTEINV;
SFR.PORTE(0x0);
SFR.LATE=function(num){
	if (num!=undefined) SFR.LATE.value=(SFR.LATE.value & 0xffffff00) + (num & 0xff);
	return SFR.LATE.value & 0xff;
};
SFR.LATECLR=function(num){
	var val=SFR.LATE.value;
	SFR.LATE(val&(0xFFFFFFFF-num));
};
SFR.LATESET=function(num){
	var val=SFR.LATE.value;
	SFR.LATE(val|num);
};
SFR.LATEINV=function(num){
	var val=SFR.LATE.value;
	SFR.LATE(val^num);
};
SFR[0xBF886430]=SFR.LATE;
SFR[0xBF886434]=SFR.LATECLR;
SFR[0xBF886438]=SFR.LATESET;
SFR[0xBF88643C]=SFR.LATEINV;
SFR.LATE(0x0);
SFR.ODCE=function(num){
	if (num!=undefined) SFR.ODCE.value=(SFR.ODCE.value & 0xffffff00) + (num & 0xff);
	return SFR.ODCE.value & 0xff;
};
SFR.ODCECLR=function(num){
	var val=SFR.ODCE.value;
	SFR.ODCE(val&(0xFFFFFFFF-num));
};
SFR.ODCESET=function(num){
	var val=SFR.ODCE.value;
	SFR.ODCE(val|num);
};
SFR.ODCEINV=function(num){
	var val=SFR.ODCE.value;
	SFR.ODCE(val^num);
};
SFR[0xBF886440]=SFR.ODCE;
SFR[0xBF886444]=SFR.ODCECLR;
SFR[0xBF886448]=SFR.ODCESET;
SFR[0xBF88644C]=SFR.ODCEINV;
SFR.ODCE(0x0);
SFR.CNPUE=function(num){
	if (num!=undefined) SFR.CNPUE.value=(SFR.CNPUE.value & 0xffffff00) + (num & 0xff);
	return SFR.CNPUE.value & 0xff;
};
SFR.CNPUECLR=function(num){
	var val=SFR.CNPUE.value;
	SFR.CNPUE(val&(0xFFFFFFFF-num));
};
SFR.CNPUESET=function(num){
	var val=SFR.CNPUE.value;
	SFR.CNPUE(val|num);
};
SFR.CNPUEINV=function(num){
	var val=SFR.CNPUE.value;
	SFR.CNPUE(val^num);
};
SFR[0xBF886450]=SFR.CNPUE;
SFR[0xBF886454]=SFR.CNPUECLR;
SFR[0xBF886458]=SFR.CNPUESET;
SFR[0xBF88645C]=SFR.CNPUEINV;
SFR.CNPUE(0x0);
SFR.CNPDE=function(num){
	if (num!=undefined) SFR.CNPDE.value=(SFR.CNPDE.value & 0xffffff00) + (num & 0xff);
	return SFR.CNPDE.value & 0xff;
};
SFR.CNPDECLR=function(num){
	var val=SFR.CNPDE.value;
	SFR.CNPDE(val&(0xFFFFFFFF-num));
};
SFR.CNPDESET=function(num){
	var val=SFR.CNPDE.value;
	SFR.CNPDE(val|num);
};
SFR.CNPDEINV=function(num){
	var val=SFR.CNPDE.value;
	SFR.CNPDE(val^num);
};
SFR[0xBF886460]=SFR.CNPDE;
SFR[0xBF886464]=SFR.CNPDECLR;
SFR[0xBF886468]=SFR.CNPDESET;
SFR[0xBF88646C]=SFR.CNPDEINV;
SFR.CNPDE(0x0);
SFR.CNCONE=function(num){
	if (num!=undefined) SFR.CNCONE.value=(SFR.CNCONE.value & 0xffff5fff) + (num & 0xa000);
	return SFR.CNCONE.value & 0xa000;
};
SFR.CNCONECLR=function(num){
	var val=SFR.CNCONE.value;
	SFR.CNCONE(val&(0xFFFFFFFF-num));
};
SFR.CNCONESET=function(num){
	var val=SFR.CNCONE.value;
	SFR.CNCONE(val|num);
};
SFR.CNCONEINV=function(num){
	var val=SFR.CNCONE.value;
	SFR.CNCONE(val^num);
};
SFR[0xBF886470]=SFR.CNCONE;
SFR[0xBF886474]=SFR.CNCONECLR;
SFR[0xBF886478]=SFR.CNCONESET;
SFR[0xBF88647C]=SFR.CNCONEINV;
SFR.CNCONE(0x0);
SFR.CNENE=function(num){
	if (num!=undefined) SFR.CNENE.value=(SFR.CNENE.value & 0xffffff00) + (num & 0xff);
	return SFR.CNENE.value & 0xff;
};
SFR.CNENECLR=function(num){
	var val=SFR.CNENE.value;
	SFR.CNENE(val&(0xFFFFFFFF-num));
};
SFR.CNENESET=function(num){
	var val=SFR.CNENE.value;
	SFR.CNENE(val|num);
};
SFR.CNENEINV=function(num){
	var val=SFR.CNENE.value;
	SFR.CNENE(val^num);
};
SFR[0xBF886480]=SFR.CNENE;
SFR[0xBF886484]=SFR.CNENECLR;
SFR[0xBF886488]=SFR.CNENESET;
SFR[0xBF88648C]=SFR.CNENEINV;
SFR.CNENE(0x0);
SFR.CNSTATE=function(num){
	if (num!=undefined) SFR.CNSTATE.value=(SFR.CNSTATE.value & 0xffffff00) + (num & 0xff);
	return SFR.CNSTATE.value & 0xff;
};
SFR.CNSTATECLR=function(num){
	var val=SFR.CNSTATE.value;
	SFR.CNSTATE(val&(0xFFFFFFFF-num));
};
SFR.CNSTATESET=function(num){
	var val=SFR.CNSTATE.value;
	SFR.CNSTATE(val|num);
};
SFR.CNSTATEINV=function(num){
	var val=SFR.CNSTATE.value;
	SFR.CNSTATE(val^num);
};
SFR[0xBF886490]=SFR.CNSTATE;
SFR[0xBF886494]=SFR.CNSTATECLR;
SFR[0xBF886498]=SFR.CNSTATESET;
SFR[0xBF88649C]=SFR.CNSTATEINV;
SFR.CNSTATE(0x0);
SFR.ANSELF=function(num){
	if (num!=undefined) SFR.ANSELF.value=(SFR.ANSELF.value & 0xffffffff) + (num & 0x0);
	return SFR.ANSELF.value & 0x0;
};
SFR.ANSELFCLR=function(num){
	var val=SFR.ANSELF.value;
	SFR.ANSELF(val&(0xFFFFFFFF-num));
};
SFR.ANSELFSET=function(num){
	var val=SFR.ANSELF.value;
	SFR.ANSELF(val|num);
};
SFR.ANSELFINV=function(num){
	var val=SFR.ANSELF.value;
	SFR.ANSELF(val^num);
};
SFR[0xBF886500]=SFR.ANSELF;
SFR[0xBF886504]=SFR.ANSELFCLR;
SFR[0xBF886508]=SFR.ANSELFSET;
SFR[0xBF88650C]=SFR.ANSELFINV;
SFR.ANSELF(0x0);
SFR.TRISF=function(num){
	if (num!=undefined) SFR.TRISF.value=(SFR.TRISF.value & 0xffffff80) + (num & 0x7f);
	return SFR.TRISF.value & 0x7f;
};
SFR.TRISFCLR=function(num){
	var val=SFR.TRISF.value;
	SFR.TRISF(val&(0xFFFFFFFF-num));
};
SFR.TRISFSET=function(num){
	var val=SFR.TRISF.value;
	SFR.TRISF(val|num);
};
SFR.TRISFINV=function(num){
	var val=SFR.TRISF.value;
	SFR.TRISF(val^num);
};
SFR[0xBF886510]=SFR.TRISF;
SFR[0xBF886514]=SFR.TRISFCLR;
SFR[0xBF886518]=SFR.TRISFSET;
SFR[0xBF88651C]=SFR.TRISFINV;
SFR.TRISF(0x7f);
SFR.PORTF=function(num){
	if (num!=undefined) SFR.PORTF.value=(SFR.PORTF.value & 0xffffff80) + (num & 0x7f);
	return SFR.PORTF.value & 0x7f;
};
SFR.PORTFCLR=function(num){
	var val=SFR.PORTF.value;
	SFR.PORTF(val&(0xFFFFFFFF-num));
};
SFR.PORTFSET=function(num){
	var val=SFR.PORTF.value;
	SFR.PORTF(val|num);
};
SFR.PORTFINV=function(num){
	var val=SFR.PORTF.value;
	SFR.PORTF(val^num);
};
SFR[0xBF886520]=SFR.PORTF;
SFR[0xBF886524]=SFR.PORTFCLR;
SFR[0xBF886528]=SFR.PORTFSET;
SFR[0xBF88652C]=SFR.PORTFINV;
SFR.PORTF(0x0);
SFR.LATF=function(num){
	if (num!=undefined) SFR.LATF.value=(SFR.LATF.value & 0xffffff80) + (num & 0x7f);
	return SFR.LATF.value & 0x7f;
};
SFR.LATFCLR=function(num){
	var val=SFR.LATF.value;
	SFR.LATF(val&(0xFFFFFFFF-num));
};
SFR.LATFSET=function(num){
	var val=SFR.LATF.value;
	SFR.LATF(val|num);
};
SFR.LATFINV=function(num){
	var val=SFR.LATF.value;
	SFR.LATF(val^num);
};
SFR[0xBF886530]=SFR.LATF;
SFR[0xBF886534]=SFR.LATFCLR;
SFR[0xBF886538]=SFR.LATFSET;
SFR[0xBF88653C]=SFR.LATFINV;
SFR.LATF(0x0);
SFR.ODCF=function(num){
	if (num!=undefined) SFR.ODCF.value=(SFR.ODCF.value & 0xffffff80) + (num & 0x7f);
	return SFR.ODCF.value & 0x7f;
};
SFR.ODCFCLR=function(num){
	var val=SFR.ODCF.value;
	SFR.ODCF(val&(0xFFFFFFFF-num));
};
SFR.ODCFSET=function(num){
	var val=SFR.ODCF.value;
	SFR.ODCF(val|num);
};
SFR.ODCFINV=function(num){
	var val=SFR.ODCF.value;
	SFR.ODCF(val^num);
};
SFR[0xBF886540]=SFR.ODCF;
SFR[0xBF886544]=SFR.ODCFCLR;
SFR[0xBF886548]=SFR.ODCFSET;
SFR[0xBF88654C]=SFR.ODCFINV;
SFR.ODCF(0x0);
SFR.CNPUF=function(num){
	if (num!=undefined) SFR.CNPUF.value=(SFR.CNPUF.value & 0xffffff80) + (num & 0x7f);
	return SFR.CNPUF.value & 0x7f;
};
SFR.CNPUFCLR=function(num){
	var val=SFR.CNPUF.value;
	SFR.CNPUF(val&(0xFFFFFFFF-num));
};
SFR.CNPUFSET=function(num){
	var val=SFR.CNPUF.value;
	SFR.CNPUF(val|num);
};
SFR.CNPUFINV=function(num){
	var val=SFR.CNPUF.value;
	SFR.CNPUF(val^num);
};
SFR[0xBF886550]=SFR.CNPUF;
SFR[0xBF886554]=SFR.CNPUFCLR;
SFR[0xBF886558]=SFR.CNPUFSET;
SFR[0xBF88655C]=SFR.CNPUFINV;
SFR.CNPUF(0x0);
SFR.CNPDF=function(num){
	if (num!=undefined) SFR.CNPDF.value=(SFR.CNPDF.value & 0xffffff80) + (num & 0x7f);
	return SFR.CNPDF.value & 0x7f;
};
SFR.CNPDFCLR=function(num){
	var val=SFR.CNPDF.value;
	SFR.CNPDF(val&(0xFFFFFFFF-num));
};
SFR.CNPDFSET=function(num){
	var val=SFR.CNPDF.value;
	SFR.CNPDF(val|num);
};
SFR.CNPDFINV=function(num){
	var val=SFR.CNPDF.value;
	SFR.CNPDF(val^num);
};
SFR[0xBF886560]=SFR.CNPDF;
SFR[0xBF886564]=SFR.CNPDFCLR;
SFR[0xBF886568]=SFR.CNPDFSET;
SFR[0xBF88656C]=SFR.CNPDFINV;
SFR.CNPDF(0x0);
SFR.CNCONF=function(num){
	if (num!=undefined) SFR.CNCONF.value=(SFR.CNCONF.value & 0xffff5fff) + (num & 0xa000);
	return SFR.CNCONF.value & 0xa000;
};
SFR.CNCONFCLR=function(num){
	var val=SFR.CNCONF.value;
	SFR.CNCONF(val&(0xFFFFFFFF-num));
};
SFR.CNCONFSET=function(num){
	var val=SFR.CNCONF.value;
	SFR.CNCONF(val|num);
};
SFR.CNCONFINV=function(num){
	var val=SFR.CNCONF.value;
	SFR.CNCONF(val^num);
};
SFR[0xBF886570]=SFR.CNCONF;
SFR[0xBF886574]=SFR.CNCONFCLR;
SFR[0xBF886578]=SFR.CNCONFSET;
SFR[0xBF88657C]=SFR.CNCONFINV;
SFR.CNCONF(0x0);
SFR.CNENF=function(num){
	if (num!=undefined) SFR.CNENF.value=(SFR.CNENF.value & 0xffffff80) + (num & 0x7f);
	return SFR.CNENF.value & 0x7f;
};
SFR.CNENFCLR=function(num){
	var val=SFR.CNENF.value;
	SFR.CNENF(val&(0xFFFFFFFF-num));
};
SFR.CNENFSET=function(num){
	var val=SFR.CNENF.value;
	SFR.CNENF(val|num);
};
SFR.CNENFINV=function(num){
	var val=SFR.CNENF.value;
	SFR.CNENF(val^num);
};
SFR[0xBF886580]=SFR.CNENF;
SFR[0xBF886584]=SFR.CNENFCLR;
SFR[0xBF886588]=SFR.CNENFSET;
SFR[0xBF88658C]=SFR.CNENFINV;
SFR.CNENF(0x0);
SFR.CNSTATF=function(num){
	if (num!=undefined) SFR.CNSTATF.value=(SFR.CNSTATF.value & 0xffffff80) + (num & 0x7f);
	return SFR.CNSTATF.value & 0x7f;
};
SFR.CNSTATFCLR=function(num){
	var val=SFR.CNSTATF.value;
	SFR.CNSTATF(val&(0xFFFFFFFF-num));
};
SFR.CNSTATFSET=function(num){
	var val=SFR.CNSTATF.value;
	SFR.CNSTATF(val|num);
};
SFR.CNSTATFINV=function(num){
	var val=SFR.CNSTATF.value;
	SFR.CNSTATF(val^num);
};
SFR[0xBF886590]=SFR.CNSTATF;
SFR[0xBF886594]=SFR.CNSTATFCLR;
SFR[0xBF886598]=SFR.CNSTATFSET;
SFR[0xBF88659C]=SFR.CNSTATFINV;
SFR.CNSTATF(0x0);
SFR.ANSELG=function(num){
	if (num!=undefined) SFR.ANSELG.value=(SFR.ANSELG.value & 0xfffffc3f) + (num & 0x3c0);
	return SFR.ANSELG.value & 0x3c0;
};
SFR.ANSELGCLR=function(num){
	var val=SFR.ANSELG.value;
	SFR.ANSELG(val&(0xFFFFFFFF-num));
};
SFR.ANSELGSET=function(num){
	var val=SFR.ANSELG.value;
	SFR.ANSELG(val|num);
};
SFR.ANSELGINV=function(num){
	var val=SFR.ANSELG.value;
	SFR.ANSELG(val^num);
};
SFR[0xBF886600]=SFR.ANSELG;
SFR[0xBF886604]=SFR.ANSELGCLR;
SFR[0xBF886608]=SFR.ANSELGSET;
SFR[0xBF88660C]=SFR.ANSELGINV;
SFR.ANSELG(0x0);
SFR.TRISG=function(num){
	if (num!=undefined) SFR.TRISG.value=(SFR.TRISG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.TRISG.value & 0x3cc;
};
SFR.TRISGCLR=function(num){
	var val=SFR.TRISG.value;
	SFR.TRISG(val&(0xFFFFFFFF-num));
};
SFR.TRISGSET=function(num){
	var val=SFR.TRISG.value;
	SFR.TRISG(val|num);
};
SFR.TRISGINV=function(num){
	var val=SFR.TRISG.value;
	SFR.TRISG(val^num);
};
SFR[0xBF886610]=SFR.TRISG;
SFR[0xBF886614]=SFR.TRISGCLR;
SFR[0xBF886618]=SFR.TRISGSET;
SFR[0xBF88661C]=SFR.TRISGINV;
SFR.TRISG(0x3cc);
SFR.PORTG=function(num){
	if (num!=undefined) SFR.PORTG.value=(SFR.PORTG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.PORTG.value & 0x3cc;
};
SFR.PORTGCLR=function(num){
	var val=SFR.PORTG.value;
	SFR.PORTG(val&(0xFFFFFFFF-num));
};
SFR.PORTGSET=function(num){
	var val=SFR.PORTG.value;
	SFR.PORTG(val|num);
};
SFR.PORTGINV=function(num){
	var val=SFR.PORTG.value;
	SFR.PORTG(val^num);
};
SFR[0xBF886620]=SFR.PORTG;
SFR[0xBF886624]=SFR.PORTGCLR;
SFR[0xBF886628]=SFR.PORTGSET;
SFR[0xBF88662C]=SFR.PORTGINV;
SFR.PORTG(0x0);
SFR.LATG=function(num){
	if (num!=undefined) SFR.LATG.value=(SFR.LATG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.LATG.value & 0x3cc;
};
SFR.LATGCLR=function(num){
	var val=SFR.LATG.value;
	SFR.LATG(val&(0xFFFFFFFF-num));
};
SFR.LATGSET=function(num){
	var val=SFR.LATG.value;
	SFR.LATG(val|num);
};
SFR.LATGINV=function(num){
	var val=SFR.LATG.value;
	SFR.LATG(val^num);
};
SFR[0xBF886630]=SFR.LATG;
SFR[0xBF886634]=SFR.LATGCLR;
SFR[0xBF886638]=SFR.LATGSET;
SFR[0xBF88663C]=SFR.LATGINV;
SFR.LATG(0x0);
SFR.ODCG=function(num){
	if (num!=undefined) SFR.ODCG.value=(SFR.ODCG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.ODCG.value & 0x3cc;
};
SFR.ODCGCLR=function(num){
	var val=SFR.ODCG.value;
	SFR.ODCG(val&(0xFFFFFFFF-num));
};
SFR.ODCGSET=function(num){
	var val=SFR.ODCG.value;
	SFR.ODCG(val|num);
};
SFR.ODCGINV=function(num){
	var val=SFR.ODCG.value;
	SFR.ODCG(val^num);
};
SFR[0xBF886640]=SFR.ODCG;
SFR[0xBF886644]=SFR.ODCGCLR;
SFR[0xBF886648]=SFR.ODCGSET;
SFR[0xBF88664C]=SFR.ODCGINV;
SFR.ODCG(0x0);
SFR.CNPUG=function(num){
	if (num!=undefined) SFR.CNPUG.value=(SFR.CNPUG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.CNPUG.value & 0x3cc;
};
SFR.CNPUGCLR=function(num){
	var val=SFR.CNPUG.value;
	SFR.CNPUG(val&(0xFFFFFFFF-num));
};
SFR.CNPUGSET=function(num){
	var val=SFR.CNPUG.value;
	SFR.CNPUG(val|num);
};
SFR.CNPUGINV=function(num){
	var val=SFR.CNPUG.value;
	SFR.CNPUG(val^num);
};
SFR[0xBF886650]=SFR.CNPUG;
SFR[0xBF886654]=SFR.CNPUGCLR;
SFR[0xBF886658]=SFR.CNPUGSET;
SFR[0xBF88665C]=SFR.CNPUGINV;
SFR.CNPUG(0x0);
SFR.CNPDG=function(num){
	if (num!=undefined) SFR.CNPDG.value=(SFR.CNPDG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.CNPDG.value & 0x3cc;
};
SFR.CNPDGCLR=function(num){
	var val=SFR.CNPDG.value;
	SFR.CNPDG(val&(0xFFFFFFFF-num));
};
SFR.CNPDGSET=function(num){
	var val=SFR.CNPDG.value;
	SFR.CNPDG(val|num);
};
SFR.CNPDGINV=function(num){
	var val=SFR.CNPDG.value;
	SFR.CNPDG(val^num);
};
SFR[0xBF886660]=SFR.CNPDG;
SFR[0xBF886664]=SFR.CNPDGCLR;
SFR[0xBF886668]=SFR.CNPDGSET;
SFR[0xBF88666C]=SFR.CNPDGINV;
SFR.CNPDG(0x0);
SFR.CNCONG=function(num){
	if (num!=undefined) SFR.CNCONG.value=(SFR.CNCONG.value & 0xffff5fff) + (num & 0xa000);
	return SFR.CNCONG.value & 0xa000;
};
SFR.CNCONGCLR=function(num){
	var val=SFR.CNCONG.value;
	SFR.CNCONG(val&(0xFFFFFFFF-num));
};
SFR.CNCONGSET=function(num){
	var val=SFR.CNCONG.value;
	SFR.CNCONG(val|num);
};
SFR.CNCONGINV=function(num){
	var val=SFR.CNCONG.value;
	SFR.CNCONG(val^num);
};
SFR[0xBF886670]=SFR.CNCONG;
SFR[0xBF886674]=SFR.CNCONGCLR;
SFR[0xBF886678]=SFR.CNCONGSET;
SFR[0xBF88667C]=SFR.CNCONGINV;
SFR.CNCONG(0x0);
SFR.CNENG=function(num){
	if (num!=undefined) SFR.CNENG.value=(SFR.CNENG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.CNENG.value & 0x3cc;
};
SFR.CNENGCLR=function(num){
	var val=SFR.CNENG.value;
	SFR.CNENG(val&(0xFFFFFFFF-num));
};
SFR.CNENGSET=function(num){
	var val=SFR.CNENG.value;
	SFR.CNENG(val|num);
};
SFR.CNENGINV=function(num){
	var val=SFR.CNENG.value;
	SFR.CNENG(val^num);
};
SFR[0xBF886680]=SFR.CNENG;
SFR[0xBF886684]=SFR.CNENGCLR;
SFR[0xBF886688]=SFR.CNENGSET;
SFR[0xBF88668C]=SFR.CNENGINV;
SFR.CNENG(0x0);
SFR.CNSTATG=function(num){
	if (num!=undefined) SFR.CNSTATG.value=(SFR.CNSTATG.value & 0xfffffc33) + (num & 0x3cc);
	return SFR.CNSTATG.value & 0x3cc;
};
SFR.CNSTATGCLR=function(num){
	var val=SFR.CNSTATG.value;
	SFR.CNSTATG(val&(0xFFFFFFFF-num));
};
SFR.CNSTATGSET=function(num){
	var val=SFR.CNSTATG.value;
	SFR.CNSTATG(val|num);
};
SFR.CNSTATGINV=function(num){
	var val=SFR.CNSTATG.value;
	SFR.CNSTATG(val^num);
};
SFR[0xBF886690]=SFR.CNSTATG;
SFR[0xBF886694]=SFR.CNSTATGCLR;
SFR[0xBF886698]=SFR.CNSTATGSET;
SFR[0xBF88669C]=SFR.CNSTATGINV;
SFR.CNSTATG(0x0);
SFR.IC1BUF=function(num){
	if (num!=undefined) SFR.IC1BUF.value=(SFR.IC1BUF.value & 0xffffffff) + (num & 0x0);
	return SFR.IC1BUF.value & 0xffffffff;
};
SFR[0xBF802010]=SFR.IC1BUF;
SFR.IC1BUF(0x0);
SFR.IC2BUF=function(num){
	if (num!=undefined) SFR.IC2BUF.value=(SFR.IC2BUF.value & 0xffffffff) + (num & 0x0);
	return SFR.IC2BUF.value & 0xffffffff;
};
SFR[0xBF802210]=SFR.IC2BUF;
SFR.IC2BUF(0x0);
SFR.IC3BUF=function(num){
	if (num!=undefined) SFR.IC3BUF.value=(SFR.IC3BUF.value & 0xffffffff) + (num & 0x0);
	return SFR.IC3BUF.value & 0xffffffff;
};
SFR[0xBF802410]=SFR.IC3BUF;
SFR.IC3BUF(0x0);
SFR.IC4BUF=function(num){
	if (num!=undefined) SFR.IC4BUF.value=(SFR.IC4BUF.value & 0xffffffff) + (num & 0x0);
	return SFR.IC4BUF.value & 0xffffffff;
};
SFR[0xBF802610]=SFR.IC4BUF;
SFR.IC4BUF(0x0);
SFR.IC5BUF=function(num){
	if (num!=undefined) SFR.IC5BUF.value=(SFR.IC5BUF.value & 0xffffffff) + (num & 0x0);
	return SFR.IC5BUF.value & 0xffffffff;
};
SFR[0xBF802810]=SFR.IC5BUF;
SFR.IC5BUF(0x0);
SFR.I2C1RCV=function(num){
	if (num!=undefined) SFR.I2C1RCV.value=(SFR.I2C1RCV.value & 0xffffff00) + (num & 0xff);
	return SFR.I2C1RCV.value & 0xff;
};
SFR[0xBF805060]=SFR.I2C1RCV;
SFR.I2C1RCV(0x0);
SFR.I2C2RCV=function(num){
	if (num!=undefined) SFR.I2C2RCV.value=(SFR.I2C2RCV.value & 0xffffff00) + (num & 0xff);
	return SFR.I2C2RCV.value & 0xff;
};
SFR[0xBF805160]=SFR.I2C2RCV;
SFR.I2C2RCV(0x0);
SFR.SPI1BUF=function(num){
	if (num!=undefined) SFR.SPI1BUF.value=(SFR.SPI1BUF.value & 0x0) + (num & 0xffffffff);
	return SFR.SPI1BUF.value & 0xffffffff;
};
SFR[0xBF805820]=SFR.SPI1BUF;
SFR.SPI1BUF(0x0);
SFR.SPI2BUF=function(num){
	if (num!=undefined) SFR.SPI2BUF.value=(SFR.SPI2BUF.value & 0x0) + (num & 0xffffffff);
	return SFR.SPI2BUF.value & 0xffffffff;
};
SFR[0xBF805A20]=SFR.SPI2BUF;
SFR.SPI2BUF(0x0);
SFR.U1TXREG=function(num){
	if (num!=undefined) SFR.U1TXREG.value=(SFR.U1TXREG.value & 0xfffffe00) + (num & 0x1ff);
	return SFR.U1TXREG.value & 0x1ff;
};
SFR[0xBF806020]=SFR.U1TXREG;
SFR.U1TXREG(0x0);
SFR.U1RXREG=function(num){
	if (num!=undefined) SFR.U1RXREG.value=(SFR.U1RXREG.value & 0xffffffff) + (num & 0x0);
	return SFR.U1RXREG.value & 0x1ff;
};
SFR[0xBF806030]=SFR.U1RXREG;
SFR.U1RXREG(0x0);
SFR.U2TXREG=function(num){
	if (num!=undefined) SFR.U2TXREG.value=(SFR.U2TXREG.value & 0xfffffe00) + (num & 0x1ff);
	return SFR.U2TXREG.value & 0x1ff;
};
SFR[0xBF806220]=SFR.U2TXREG;
SFR.U2TXREG(0x0);
SFR.U2RXREG=function(num){
	if (num!=undefined) SFR.U2RXREG.value=(SFR.U2RXREG.value & 0xffffffff) + (num & 0x0);
	return SFR.U2RXREG.value & 0x1ff;
};
SFR[0xBF806230]=SFR.U2RXREG;
SFR.U2RXREG(0x0);
SFR.U3TXREG=function(num){
	if (num!=undefined) SFR.U3TXREG.value=(SFR.U3TXREG.value & 0xfffffe00) + (num & 0x1ff);
	return SFR.U3TXREG.value & 0x1ff;
};
SFR[0xBF806420]=SFR.U3TXREG;
SFR.U3TXREG(0x0);
SFR.U3RXREG=function(num){
	if (num!=undefined) SFR.U3RXREG.value=(SFR.U3RXREG.value & 0xffffffff) + (num & 0x0);
	return SFR.U3RXREG.value & 0x1ff;
};
SFR[0xBF806430]=SFR.U3RXREG;
SFR.U3RXREG(0x0);
SFR.U4TXREG=function(num){
	if (num!=undefined) SFR.U4TXREG.value=(SFR.U4TXREG.value & 0xfffffe00) + (num & 0x1ff);
	return SFR.U4TXREG.value & 0x1ff;
};
SFR[0xBF806620]=SFR.U4TXREG;
SFR.U4TXREG(0x0);
SFR.U4RXREG=function(num){
	if (num!=undefined) SFR.U4RXREG.value=(SFR.U4RXREG.value & 0xffffffff) + (num & 0x0);
	return SFR.U4RXREG.value & 0x1ff;
};
SFR[0xBF806630]=SFR.U4RXREG;
SFR.U4RXREG(0x0);
SFR.ADC1BUF0=function(num){
	if (num!=undefined) SFR.ADC1BUF0.value=(SFR.ADC1BUF0.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF0.value & 0xffffffff;
};
SFR[0xBF809070]=SFR.ADC1BUF0;
SFR.ADC1BUF0(0x0);
SFR.ADC1BUF1=function(num){
	if (num!=undefined) SFR.ADC1BUF1.value=(SFR.ADC1BUF1.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF1.value & 0xffffffff;
};
SFR[0xBF809080]=SFR.ADC1BUF1;
SFR.ADC1BUF1(0x0);
SFR.ADC1BUF2=function(num){
	if (num!=undefined) SFR.ADC1BUF2.value=(SFR.ADC1BUF2.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF2.value & 0xffffffff;
};
SFR[0xBF809090]=SFR.ADC1BUF2;
SFR.ADC1BUF2(0x0);
SFR.ADC1BUF3=function(num){
	if (num!=undefined) SFR.ADC1BUF3.value=(SFR.ADC1BUF3.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF3.value & 0xffffffff;
};
SFR[0xBF8090A0]=SFR.ADC1BUF3;
SFR.ADC1BUF3(0x0);
SFR.ADC1BUF4=function(num){
	if (num!=undefined) SFR.ADC1BUF4.value=(SFR.ADC1BUF4.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF4.value & 0xffffffff;
};
SFR[0xBF8090B0]=SFR.ADC1BUF4;
SFR.ADC1BUF4(0x0);
SFR.ADC1BUF5=function(num){
	if (num!=undefined) SFR.ADC1BUF5.value=(SFR.ADC1BUF5.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF5.value & 0xffffffff;
};
SFR[0xBF8090C0]=SFR.ADC1BUF5;
SFR.ADC1BUF5(0x0);
SFR.ADC1BUF6=function(num){
	if (num!=undefined) SFR.ADC1BUF6.value=(SFR.ADC1BUF6.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF6.value & 0xffffffff;
};
SFR[0xBF8090D0]=SFR.ADC1BUF6;
SFR.ADC1BUF6(0x0);
SFR.ADC1BUF7=function(num){
	if (num!=undefined) SFR.ADC1BUF7.value=(SFR.ADC1BUF7.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF7.value & 0xffffffff;
};
SFR[0xBF8090E0]=SFR.ADC1BUF7;
SFR.ADC1BUF7(0x0);
SFR.ADC1BUF8=function(num){
	if (num!=undefined) SFR.ADC1BUF8.value=(SFR.ADC1BUF8.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF8.value & 0xffffffff;
};
SFR[0xBF8090F0]=SFR.ADC1BUF8;
SFR.ADC1BUF8(0x0);
SFR.ADC1BUF9=function(num){
	if (num!=undefined) SFR.ADC1BUF9.value=(SFR.ADC1BUF9.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUF9.value & 0xffffffff;
};
SFR[0xBF809100]=SFR.ADC1BUF9;
SFR.ADC1BUF9(0x0);
SFR.ADC1BUFA=function(num){
	if (num!=undefined) SFR.ADC1BUFA.value=(SFR.ADC1BUFA.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUFA.value & 0xffffffff;
};
SFR[0xBF809110]=SFR.ADC1BUFA;
SFR.ADC1BUFA(0x0);
SFR.ADC1BUFB=function(num){
	if (num!=undefined) SFR.ADC1BUFB.value=(SFR.ADC1BUFB.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUFB.value & 0xffffffff;
};
SFR[0xBF809120]=SFR.ADC1BUFB;
SFR.ADC1BUFB(0x0);
SFR.ADC1BUFC=function(num){
	if (num!=undefined) SFR.ADC1BUFC.value=(SFR.ADC1BUFC.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUFC.value & 0xffffffff;
};
SFR[0xBF809130]=SFR.ADC1BUFC;
SFR.ADC1BUFC(0x0);
SFR.ADC1BUFD=function(num){
	if (num!=undefined) SFR.ADC1BUFD.value=(SFR.ADC1BUFD.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUFD.value & 0xffffffff;
};
SFR[0xBF809140]=SFR.ADC1BUFD;
SFR.ADC1BUFD(0x0);
SFR.ADC1BUFE=function(num){
	if (num!=undefined) SFR.ADC1BUFE.value=(SFR.ADC1BUFE.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUFE.value & 0xffffffff;
};
SFR[0xBF809150]=SFR.ADC1BUFE;
SFR.ADC1BUFE(0x0);
SFR.ADC1BUFF=function(num){
	if (num!=undefined) SFR.ADC1BUFF.value=(SFR.ADC1BUFF.value & 0xffffffff) + (num & 0x0);
	return SFR.ADC1BUFF.value & 0xffffffff;
};
SFR[0xBF809160]=SFR.ADC1BUFF;
SFR.ADC1BUFF(0x0);
SFR.CFGCON=function(num){
	if (num!=undefined) SFR.CFGCON.value=(SFR.CFGCON.value & 0xffffcff2) + (num & 0x300d);
	return SFR.CFGCON.value & 0x300d;
};
SFR[0xBF80F200]=SFR.CFGCON;
SFR.CFGCON(0x8);
SFR.DEVID=function(num){
	if (num!=undefined) SFR.DEVID.value=(SFR.DEVID.value & 0xffffffff) + (num & 0x0);
	return SFR.DEVID.value & 0xffffffff;
};
SFR[0xBF80F220]=SFR.DEVID;
SFR.DEVID(0x0);
SFR.NVMKEY=function(num){
	if (num!=undefined) SFR.NVMKEY.value=(SFR.NVMKEY.value & 0x0) + (num & 0xffffffff);
	return SFR.NVMKEY.value & 0x0;
};
SFR[0xBF80F410]=SFR.NVMKEY;
SFR.NVMKEY(0x0);
SFR.NVMDATA=function(num){
	if (num!=undefined) SFR.NVMDATA.value=(SFR.NVMDATA.value & 0x0) + (num & 0xffffffff);
	return SFR.NVMDATA.value & 0xffffffff;
};
SFR[0xBF80F430]=SFR.NVMDATA;
SFR.NVMDATA(0x0);
SFR.NVMSRCADDR=function(num){
	if (num!=undefined) SFR.NVMSRCADDR.value=(SFR.NVMSRCADDR.value & 0x0) + (num & 0xffffffff);
	return SFR.NVMSRCADDR.value & 0xffffffff;
};
SFR[0xBF80F440]=SFR.NVMSRCADDR;
SFR.NVMSRCADDR(0x0);
SFR.INT1R=function(num){
	if (num!=undefined) SFR.INT1R.value=(SFR.INT1R.value & 0xfffffff0) + (num & 0xf);
	return SFR.INT1R.value & 0xf;
};
SFR[0xBF80FA04]=SFR.INT1R;
SFR.INT1R(0x0);
SFR.INT2R=function(num){
	if (num!=undefined) SFR.INT2R.value=(SFR.INT2R.value & 0xfffffff0) + (num & 0xf);
	return SFR.INT2R.value & 0xf;
};
SFR[0xBF80FA08]=SFR.INT2R;
SFR.INT2R(0x0);
SFR.INT3R=function(num){
	if (num!=undefined) SFR.INT3R.value=(SFR.INT3R.value & 0xfffffff0) + (num & 0xf);
	return SFR.INT3R.value & 0xf;
};
SFR[0xBF80FA0C]=SFR.INT3R;
SFR.INT3R(0x0);
SFR.INT4R=function(num){
	if (num!=undefined) SFR.INT4R.value=(SFR.INT4R.value & 0xfffffff0) + (num & 0xf);
	return SFR.INT4R.value & 0xf;
};
SFR[0xBF80FA10]=SFR.INT4R;
SFR.INT4R(0x0);
SFR.T2CKR=function(num){
	if (num!=undefined) SFR.T2CKR.value=(SFR.T2CKR.value & 0xfffffff0) + (num & 0xf);
	return SFR.T2CKR.value & 0xf;
};
SFR[0xBF80FA18]=SFR.T2CKR;
SFR.T2CKR(0x0);
SFR.T3CKR=function(num){
	if (num!=undefined) SFR.T3CKR.value=(SFR.T3CKR.value & 0xfffffff0) + (num & 0xf);
	return SFR.T3CKR.value & 0xf;
};
SFR[0xBF80FA1C]=SFR.T3CKR;
SFR.T3CKR(0x0);
SFR.T4CKR=function(num){
	if (num!=undefined) SFR.T4CKR.value=(SFR.T4CKR.value & 0xfffffff0) + (num & 0xf);
	return SFR.T4CKR.value & 0xf;
};
SFR[0xBF80FA20]=SFR.T4CKR;
SFR.T4CKR(0x0);
SFR.T5CKR=function(num){
	if (num!=undefined) SFR.T5CKR.value=(SFR.T5CKR.value & 0xfffffff0) + (num & 0xf);
	return SFR.T5CKR.value & 0xf;
};
SFR[0xBF80FA24]=SFR.T5CKR;
SFR.T5CKR(0x0);
SFR.IC1R=function(num){
	if (num!=undefined) SFR.IC1R.value=(SFR.IC1R.value & 0xfffffff0) + (num & 0xf);
	return SFR.IC1R.value & 0xf;
};
SFR[0xBF80FA28]=SFR.IC1R;
SFR.IC1R(0x0);
SFR.IC2R=function(num){
	if (num!=undefined) SFR.IC2R.value=(SFR.IC2R.value & 0xfffffff0) + (num & 0xf);
	return SFR.IC2R.value & 0xf;
};
SFR[0xBF80FA2C]=SFR.IC2R;
SFR.IC2R(0x0);
SFR.IC3R=function(num){
	if (num!=undefined) SFR.IC3R.value=(SFR.IC3R.value & 0xfffffff0) + (num & 0xf);
	return SFR.IC3R.value & 0xf;
};
SFR[0xBF80FA30]=SFR.IC3R;
SFR.IC3R(0x0);
SFR.IC4R=function(num){
	if (num!=undefined) SFR.IC4R.value=(SFR.IC4R.value & 0xfffffff0) + (num & 0xf);
	return SFR.IC4R.value & 0xf;
};
SFR[0xBF80FA34]=SFR.IC4R;
SFR.IC4R(0x0);
SFR.IC5R=function(num){
	if (num!=undefined) SFR.IC5R.value=(SFR.IC5R.value & 0xfffffff0) + (num & 0xf);
	return SFR.IC5R.value & 0xf;
};
SFR[0xBF80FA38]=SFR.IC5R;
SFR.IC5R(0x0);
SFR.OCFAR=function(num){
	if (num!=undefined) SFR.OCFAR.value=(SFR.OCFAR.value & 0xfffffff0) + (num & 0xf);
	return SFR.OCFAR.value & 0xf;
};
SFR[0xBF80FA48]=SFR.OCFAR;
SFR.OCFAR(0x0);
SFR.U1RXR=function(num){
	if (num!=undefined) SFR.U1RXR.value=(SFR.U1RXR.value & 0xffffffff) + (num & 0x0);
	return SFR.U1RXR.value & 0x1ff;
};
SFR[0xBF80FA50]=SFR.U1RXR;
SFR.U1RXR(0x0);
SFR.U1CTSR=function(num){
	if (num!=undefined) SFR.U1CTSR.value=(SFR.U1CTSR.value & 0xfffffff0) + (num & 0xf);
	return SFR.U1CTSR.value & 0xf;
};
SFR[0xBF80FA54]=SFR.U1CTSR;
SFR.U1CTSR(0x0);
SFR.U2RXR=function(num){
	if (num!=undefined) SFR.U2RXR.value=(SFR.U2RXR.value & 0xffffffff) + (num & 0x0);
	return SFR.U2RXR.value & 0x1ff;
};
SFR[0xBF80FA58]=SFR.U2RXR;
SFR.U2RXR(0x0);
SFR.U2CTSR=function(num){
	if (num!=undefined) SFR.U2CTSR.value=(SFR.U2CTSR.value & 0xfffffff0) + (num & 0xf);
	return SFR.U2CTSR.value & 0xf;
};
SFR[0xBF80FA5C]=SFR.U2CTSR;
SFR.U2CTSR(0x0);
SFR.U3RXR=function(num){
	if (num!=undefined) SFR.U3RXR.value=(SFR.U3RXR.value & 0xffffffff) + (num & 0x0);
	return SFR.U3RXR.value & 0x1ff;
};
SFR[0xBF80FA60]=SFR.U3RXR;
SFR.U3RXR(0x0);
SFR.U3CTSR=function(num){
	if (num!=undefined) SFR.U3CTSR.value=(SFR.U3CTSR.value & 0xfffffff0) + (num & 0xf);
	return SFR.U3CTSR.value & 0xf;
};
SFR[0xBF80FA64]=SFR.U3CTSR;
SFR.U3CTSR(0x0);
SFR.U4RXR=function(num){
	if (num!=undefined) SFR.U4RXR.value=(SFR.U4RXR.value & 0xffffffff) + (num & 0x0);
	return SFR.U4RXR.value & 0x1ff;
};
SFR[0xBF80FA68]=SFR.U4RXR;
SFR.U4RXR(0x0);
SFR.U4CTSR=function(num){
	if (num!=undefined) SFR.U4CTSR.value=(SFR.U4CTSR.value & 0xfffffff0) + (num & 0xf);
	return SFR.U4CTSR.value & 0xf;
};
SFR[0xBF80FA6C]=SFR.U4CTSR;
SFR.U4CTSR(0x0);
SFR.U5RXR=function(num){
	if (num!=undefined) SFR.U5RXR.value=(SFR.U5RXR.value & 0xfffffff0) + (num & 0xf);
	return SFR.U5RXR.value & 0xf;
};
SFR[0xBF80FA70]=SFR.U5RXR;
SFR.U5RXR(0x0);
SFR.U5CTSR=function(num){
	if (num!=undefined) SFR.U5CTSR.value=(SFR.U5CTSR.value & 0xfffffff0) + (num & 0xf);
	return SFR.U5CTSR.value & 0xf;
};
SFR[0xBF80FA74]=SFR.U5CTSR;
SFR.U5CTSR(0x0);
SFR.SDI1R=function(num){
	if (num!=undefined) SFR.SDI1R.value=(SFR.SDI1R.value & 0xfffffff0) + (num & 0xf);
	return SFR.SDI1R.value & 0xf;
};
SFR[0xBF80FA84]=SFR.SDI1R;
SFR.SDI1R(0x0);
SFR.SS1R=function(num){
	if (num!=undefined) SFR.SS1R.value=(SFR.SS1R.value & 0xfffffff0) + (num & 0xf);
	return SFR.SS1R.value & 0xf;
};
SFR[0xBF80FA88]=SFR.SS1R;
SFR.SS1R(0x0);
SFR.SDI2R=function(num){
	if (num!=undefined) SFR.SDI2R.value=(SFR.SDI2R.value & 0xfffffff0) + (num & 0xf);
	return SFR.SDI2R.value & 0xf;
};
SFR[0xBF80FA90]=SFR.SDI2R;
SFR.SDI2R(0x0);
SFR.SS2R=function(num){
	if (num!=undefined) SFR.SS2R.value=(SFR.SS2R.value & 0xfffffff0) + (num & 0xf);
	return SFR.SS2R.value & 0xf;
};
SFR[0xBF80FA94]=SFR.SS2R;
SFR.SS2R(0x0);
SFR.REFCLKIR=function(num){
	if (num!=undefined) SFR.REFCLKIR.value=(SFR.REFCLKIR.value & 0xfffffff0) + (num & 0xf);
	return SFR.REFCLKIR.value & 0xf;
};
SFR[0xBF80FAD0]=SFR.REFCLKIR;
SFR.REFCLKIR(0x0);
SFR.RPB0R=function(num){
	if (num!=undefined) SFR.RPB0R.value=(SFR.RPB0R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB0R.value & 0xf;
};
SFR[0xBF80FB40]=SFR.RPB0R;
SFR.RPB0R(0x0);
SFR.RPB1R=function(num){
	if (num!=undefined) SFR.RPB1R.value=(SFR.RPB1R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB1R.value & 0xf;
};
SFR[0xBF80FB44]=SFR.RPB1R;
SFR.RPB1R(0x0);
SFR.RPB2R=function(num){
	if (num!=undefined) SFR.RPB2R.value=(SFR.RPB2R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB2R.value & 0xf;
};
SFR[0xBF80FB48]=SFR.RPB2R;
SFR.RPB2R(0x0);
SFR.RPB3R=function(num){
	if (num!=undefined) SFR.RPB3R.value=(SFR.RPB3R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB3R.value & 0xf;
};
SFR[0xBF80FB4C]=SFR.RPB3R;
SFR.RPB3R(0x0);
SFR.RPB5R=function(num){
	if (num!=undefined) SFR.RPB5R.value=(SFR.RPB5R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB5R.value & 0xf;
};
SFR[0xBF80FB54]=SFR.RPB5R;
SFR.RPB5R(0x0);
SFR.RPB6R=function(num){
	if (num!=undefined) SFR.RPB6R.value=(SFR.RPB6R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB6R.value & 0xf;
};
SFR[0xBF80FB58]=SFR.RPB6R;
SFR.RPB6R(0x0);
SFR.RPB7R=function(num){
	if (num!=undefined) SFR.RPB7R.value=(SFR.RPB7R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB7R.value & 0xf;
};
SFR[0xBF80FB5C]=SFR.RPB7R;
SFR.RPB7R(0x0);
SFR.RPB8R=function(num){
	if (num!=undefined) SFR.RPB8R.value=(SFR.RPB8R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB8R.value & 0xf;
};
SFR[0xBF80FB60]=SFR.RPB8R;
SFR.RPB8R(0x0);
SFR.RPB9R=function(num){
	if (num!=undefined) SFR.RPB9R.value=(SFR.RPB9R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB9R.value & 0xf;
};
SFR[0xBF80FB64]=SFR.RPB9R;
SFR.RPB9R(0x0);
SFR.RPB10R=function(num){
	if (num!=undefined) SFR.RPB10R.value=(SFR.RPB10R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB10R.value & 0xf;
};
SFR[0xBF80FB68]=SFR.RPB10R;
SFR.RPB10R(0x0);
SFR.RPB14R=function(num){
	if (num!=undefined) SFR.RPB14R.value=(SFR.RPB14R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB14R.value & 0xf;
};
SFR[0xBF80FB78]=SFR.RPB14R;
SFR.RPB14R(0x0);
SFR.RPB15R=function(num){
	if (num!=undefined) SFR.RPB15R.value=(SFR.RPB15R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPB15R.value & 0xf;
};
SFR[0xBF80FB7C]=SFR.RPB15R;
SFR.RPB15R(0x0);
SFR.RPD0R=function(num){
	if (num!=undefined) SFR.RPD0R.value=(SFR.RPD0R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD0R.value & 0xf;
};
SFR[0xBF80FBC0]=SFR.RPD0R;
SFR.RPD0R(0x0);
SFR.RPD1R=function(num){
	if (num!=undefined) SFR.RPD1R.value=(SFR.RPD1R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD1R.value & 0xf;
};
SFR[0xBF80FBC4]=SFR.RPD1R;
SFR.RPD1R(0x0);
SFR.RPD2R=function(num){
	if (num!=undefined) SFR.RPD2R.value=(SFR.RPD2R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD2R.value & 0xf;
};
SFR[0xBF80FBC8]=SFR.RPD2R;
SFR.RPD2R(0x0);
SFR.RPD3R=function(num){
	if (num!=undefined) SFR.RPD3R.value=(SFR.RPD3R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD3R.value & 0xf;
};
SFR[0xBF80FBCC]=SFR.RPD3R;
SFR.RPD3R(0x0);
SFR.RPD4R=function(num){
	if (num!=undefined) SFR.RPD4R.value=(SFR.RPD4R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD4R.value & 0xf;
};
SFR[0xBF80FBD0]=SFR.RPD4R;
SFR.RPD4R(0x0);
SFR.RPD5R=function(num){
	if (num!=undefined) SFR.RPD5R.value=(SFR.RPD5R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD5R.value & 0xf;
};
SFR[0xBF80FBD4]=SFR.RPD5R;
SFR.RPD5R(0x0);
SFR.RPD8R=function(num){
	if (num!=undefined) SFR.RPD8R.value=(SFR.RPD8R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD8R.value & 0xf;
};
SFR[0xBF80FBE0]=SFR.RPD8R;
SFR.RPD8R(0x0);
SFR.RPD9R=function(num){
	if (num!=undefined) SFR.RPD9R.value=(SFR.RPD9R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD9R.value & 0xf;
};
SFR[0xBF80FBE4]=SFR.RPD9R;
SFR.RPD9R(0x0);
SFR.RPD10R=function(num){
	if (num!=undefined) SFR.RPD10R.value=(SFR.RPD10R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD10R.value & 0xf;
};
SFR[0xBF80FBE8]=SFR.RPD10R;
SFR.RPD10R(0x0);
SFR.RPD11R=function(num){
	if (num!=undefined) SFR.RPD11R.value=(SFR.RPD11R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPD11R.value & 0xf;
};
SFR[0xBF80FBEC]=SFR.RPD11R;
SFR.RPD11R(0x0);
SFR.RPF0R=function(num){
	if (num!=undefined) SFR.RPF0R.value=(SFR.RPF0R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPF0R.value & 0xf;
};
SFR[0xBF80FC40]=SFR.RPF0R;
SFR.RPF0R(0x0);
SFR.RPF1R=function(num){
	if (num!=undefined) SFR.RPF1R.value=(SFR.RPF1R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPF1R.value & 0xf;
};
SFR[0xBF80FC44]=SFR.RPF1R;
SFR.RPF1R(0x0);
SFR.RPF2R=function(num){
	if (num!=undefined) SFR.RPF2R.value=(SFR.RPF2R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPF2R.value & 0xf;
};
SFR[0xBF80FC48]=SFR.RPF2R;
SFR.RPF2R(0x0);
SFR.RPF3R=function(num){
	if (num!=undefined) SFR.RPF3R.value=(SFR.RPF3R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPF3R.value & 0xf;
};
SFR[0xBF80FC4C]=SFR.RPF3R;
SFR.RPF3R(0x0);
SFR.RPF4R=function(num){
	if (num!=undefined) SFR.RPF4R.value=(SFR.RPF4R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPF4R.value & 0xf;
};
SFR[0xBF80FC50]=SFR.RPF4R;
SFR.RPF4R(0x0);
SFR.RPF5R=function(num){
	if (num!=undefined) SFR.RPF5R.value=(SFR.RPF5R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPF5R.value & 0xf;
};
SFR[0xBF80FC54]=SFR.RPF5R;
SFR.RPF5R(0x0);
SFR.RPF6R=function(num){
	if (num!=undefined) SFR.RPF6R.value=(SFR.RPF6R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPF6R.value & 0xf;
};
SFR[0xBF80FC58]=SFR.RPF6R;
SFR.RPF6R(0x0);
SFR.RPG6R=function(num){
	if (num!=undefined) SFR.RPG6R.value=(SFR.RPG6R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPG6R.value & 0xf;
};
SFR[0xBF80FC98]=SFR.RPG6R;
SFR.RPG6R(0x0);
SFR.RPG7R=function(num){
	if (num!=undefined) SFR.RPG7R.value=(SFR.RPG7R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPG7R.value & 0xf;
};
SFR[0xBF80FC9C]=SFR.RPG7R;
SFR.RPG7R(0x0);
SFR.RPG8R=function(num){
	if (num!=undefined) SFR.RPG8R.value=(SFR.RPG8R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPG8R.value & 0xf;
};
SFR[0xBF80FCA0]=SFR.RPG8R;
SFR.RPG8R(0x0);
SFR.RPG9R=function(num){
	if (num!=undefined) SFR.RPG9R.value=(SFR.RPG9R.value & 0xfffffff0) + (num & 0xf);
	return SFR.RPG9R.value & 0xf;
};
SFR[0xBF80FCA4]=SFR.RPG9R;
SFR.RPG9R(0x0);
SFR.INTSTAT=function(num){
	if (num!=undefined) SFR.INTSTAT.value=(SFR.INTSTAT.value & 0xfffff8c0) + (num & 0x73f);
	return SFR.INTSTAT.value & 0x73f;
};
SFR[0xBF881010]=SFR.INTSTAT;
SFR.INTSTAT(0x0);
SFR.BMXDRMSZ=function(num){
	if (num!=undefined) SFR.BMXDRMSZ.value=(SFR.BMXDRMSZ.value & 0xffffffff) + (num & 0x0);
	return SFR.BMXDRMSZ.value & 0xffffffff;
};
SFR[0xBF882040]=SFR.BMXDRMSZ;
SFR.BMXDRMSZ(0x20000);
SFR.BMXPFMSZ=function(num){
	if (num!=undefined) SFR.BMXPFMSZ.value=(SFR.BMXPFMSZ.value & 0xffffffff) + (num & 0x0);
	return SFR.BMXPFMSZ.value & 0xffffffff;
};
SFR[0xBF882060]=SFR.BMXPFMSZ;
SFR.BMXPFMSZ(0x80000);
SFR.BMXBOOTSZ=function(num){
	if (num!=undefined) SFR.BMXBOOTSZ.value=(SFR.BMXBOOTSZ.value & 0xffffffff) + (num & 0x0);
	return SFR.BMXBOOTSZ.value & 0xffffffff;
};
SFR[0xBF882070]=SFR.BMXBOOTSZ;
SFR.BMXBOOTSZ(0x3000);
SFR.CHEW0=function(num){
	if (num!=undefined) SFR.CHEW0.value=(SFR.CHEW0.value & 0x0) + (num & 0xffffffff);
	return SFR.CHEW0.value & 0xffffffff;
};
SFR[0xBF884040]=SFR.CHEW0;
SFR.CHEW0(0x0);
SFR.CHEW1=function(num){
	if (num!=undefined) SFR.CHEW1.value=(SFR.CHEW1.value & 0x0) + (num & 0xffffffff);
	return SFR.CHEW1.value & 0xffffffff;
};
SFR[0xBF884050]=SFR.CHEW1;
SFR.CHEW1(0x0);
SFR.CHEW2=function(num){
	if (num!=undefined) SFR.CHEW2.value=(SFR.CHEW2.value & 0x0) + (num & 0xffffffff);
	return SFR.CHEW2.value & 0xffffffff;
};
SFR[0xBF884060]=SFR.CHEW2;
SFR.CHEW2(0x0);
SFR.CHEW3=function(num){
	if (num!=undefined) SFR.CHEW3.value=(SFR.CHEW3.value & 0x0) + (num & 0xffffffff);
	return SFR.CHEW3.value & 0xffffffff;
};
SFR[0xBF884070]=SFR.CHEW3;
SFR.CHEW3(0x0);
SFR.CHELRU=function(num){
	if (num!=undefined) SFR.CHELRU.value=(SFR.CHELRU.value & 0x0) + (num & 0xffffffff);
	return SFR.CHELRU.value & 0xffffffff;
};
SFR[0xBF884080]=SFR.CHELRU;
SFR.CHELRU(0x0);
SFR.CHEHIT=function(num){
	if (num!=undefined) SFR.CHEHIT.value=(SFR.CHEHIT.value & 0xffffffff) + (num & 0x0);
	return SFR.CHEHIT.value & 0xffffffff;
};
SFR[0xBF884090]=SFR.CHEHIT;
SFR.CHEHIT(0x0);
SFR.CHEMIS=function(num){
	if (num!=undefined) SFR.CHEMIS.value=(SFR.CHEMIS.value & 0xffffffff) + (num & 0x0);
	return SFR.CHEMIS.value & 0xffffffff;
};
SFR[0xBF8840A0]=SFR.CHEMIS;
SFR.CHEMIS(0x0);
SFR.RESERVED1=function(num){
	if (num!=undefined) SFR.RESERVED1.value=(SFR.RESERVED1.value & 0xffffffff) + (num & 0x0);
	return SFR.RESERVED1.value & 0x0;
};
SFR[0xBF8840B0]=SFR.RESERVED1;
SFR.RESERVED1(0x0);
SFR.CHEPFABT=function(num){
	if (num!=undefined) SFR.CHEPFABT.value=(SFR.CHEPFABT.value & 0xffffffff) + (num & 0x0);
	return SFR.CHEPFABT.value & 0xffffffff;
};
SFR[0xBF8840C0]=SFR.CHEPFABT;
SFR.CHEPFABT(0x0);
