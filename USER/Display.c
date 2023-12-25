#include "includes.h"

uint8_t    GuB_ItemMsk;
uint8_t    GuB_PwrupdisFlg;
uint8_t    GuB_DisStatus;//0:Auto display; 1:key:2:Inset user card;
                                           // 3:inset check card
//modify 231225--01
//modify 231225--02
//modify 231225--03a

uint16_t GuW_Item;
uint8_t    GuB_SubItem;
uint8_t    GuB_ItemKey;
uint8_t    GuB_ItemAuto;

StrDef_AutoDisDat	Gstr_AutoDisDat;
uint8_t    GuB_AutoDisSum;
uint8_t    GuB_KeyDisSum;

uint8_t GuB_DisIntrmCnt;
uint8_t   GuB_DispReFlCnt;
uint8_t   GAuB_AutoDisItmsBf[99*5+1];
uint8_t   GAuB_KeyDisItmsBf[99*5+1];
StrDef_DisItemInfo 	Gstr_PrsItmInfo1;
StrDef_DisItemInfo 	Gstr_AutoItmInfo1;
StrDef_DisItemInfo 	Gstr_KeyItmInfo1;

void Task_Display(void)
{

	if(GF_SecAlter2Dis)
		{
		GF_SecAlter2Dis=0;
	
		GuB_DisIntrmCnt--;
		if(GuB_DisIntrmCnt==0)
			{
			GuB_DisIntrmCnt=ByteDec_To_Hex(Gstr_AutoDisDat.uB_Time);
			switch(GuB_DisStatus)
				{
				case 0:					
					Get_NxtAutoItem_Dat();			
					String_Copy(&(Gstr_PrsItmInfo1.uB_PstCnt),
						&(Gstr_AutoItmInfo1.uB_PstCnt),sizeof(StrDef_DisItemInfo));																						
					Display_Screen();
					break;
				case 1:
					//String_Copy(&(Gstr_PrsItmInfo1.uB_PstCnt),
						//&(Gstr_AutoItmInfo1.uB_PstCnt),18);
					Gstr_AutoItmInfo1.uB_PstCnt=GuB_AutoDisSum-1;
					Gstr_AutoItmInfo1.uB_SubItm=0;
					Gstr_AutoItmInfo1.uB_Prpt=0;
					Get_NxtAutoItem_Dat();	
					String_Copy(&(Gstr_PrsItmInfo1.uB_PstCnt),&(Gstr_AutoItmInfo1.uB_PstCnt),sizeof(StrDef_DisItemInfo));
					
					GuB_DisStatus=0;
					Display_Screen();
					break;
			      case 2:
					if(1==Gstr_PrsItmInfo1.uB_SubItm)
						{
						Gstr_PrsItmInfo1.uB_SubItm=0;
						Get_ID34_DisInfo(&Gstr_PrsItmInfo1);
						}
					else
						{
						Gstr_AutoItmInfo1.uB_PstCnt=GuB_AutoDisSum-1;
						Gstr_AutoItmInfo1.uB_SubItm=0;
						Gstr_AutoItmInfo1.uB_Prpt=0;
						Get_NxtAutoItem_Dat();	
						String_Copy(&(Gstr_PrsItmInfo1.uB_PstCnt),&(Gstr_AutoItmInfo1.uB_PstCnt),sizeof(StrDef_DisItemInfo));	
						GuB_DisStatus=0;
						Display_Screen();
						}
					break;
					
				case 3:
					Gstr_AutoItmInfo1.uB_PstCnt=GuB_AutoDisSum-1;
					Gstr_AutoItmInfo1.uB_SubItm=0;
					Gstr_AutoItmInfo1.uB_Prpt=0;
					Get_NxtAutoItem_Dat();	
					String_Copy(&(Gstr_PrsItmInfo1.uB_PstCnt),&(Gstr_AutoItmInfo1.uB_PstCnt),sizeof(StrDef_DisItemInfo));	
					GuB_DisStatus=0;
					Display_Screen();
					break;

				default:
					GuB_DisStatus=0;
					break;
				}
			
			
			////////////////////////////	
			
			GuB_PwrupdisFlg=0;	
			}		
		}
	if(GF_DisReFlash)
		{
		GF_DisReFlash=0;
		//LED_Indiction();
		Display_Screen(); 
		}
}

uint8_t Get_ID30_DisInfo(StrDef_DisItemInfo *PStr_DisCmmInfo)
{
	uint8_t i,LuB_Dat;
	uint16_t LuW_Addr;
	//if((*(uint16_t *)(&(PStr_DisCmmInfo->Str_ID.uB_ID0)))==0xffff){return 1;}
	if((PStr_DisCmmInfo->Str_ID.uB_ID0==0xff)&&(PStr_DisCmmInfo->Str_ID.uB_ID1==0xff)){return 1;}
	if((PStr_DisCmmInfo->Str_ID.uB_ID0!=0xff)&&
		(PStr_DisCmmInfo->Str_ID.uB_ID0>12)){return 1;}
	if(PStr_DisCmmInfo->Str_ID.uB_ID2==0x90)
		{
		#ifdef Remote
		return 1;
		#else
		//PStr_DisCmmInfo->uB_Prpt=0x40;
		PStr_DisCmmInfo->uB_Prpt=0x20;
		
		if(CVrAddr2uW(&(PStr_DisCmmInfo->Str_ID.uB_ID0))==0x200)
			{
			//(*(uint32_t *)(PStr_DisCmmInfo->AuB_DisBuf))=Hex_To_CmpDec(GU_Remain.u32);	
			PStr_DisCmmInfo->uW_Addr=(GU_Remain.AuB);
			}
		else if(CVrAddr2uW(&(PStr_DisCmmInfo->Str_ID.uB_ID0))==0x201)
			{
			//(*(uint32_t *)(PStr_DisCmmInfo->AuB_DisBuf))=Hex_To_CmpDec(GU_TouZhiPwr.u32);	
			//PStr_DisCmmInfo->uW_Addr=(uint16_t)(PStr_DisCmmInfo->AuB_DisBuf);
			PStr_DisCmmInfo->uW_Addr=(GU_TouZhiPwr.AuB);
			}
		else {return 1;}	
		PStr_DisCmmInfo->uB_FuPst_FrmL=0x03;
		PStr_DisCmmInfo->uB_SubItm=0;
		return 0;
		#endif
		}
		
	if(PStr_DisCmmInfo->Str_ID.uB_ID0)
		{//history
		for(i=0;i<Max_HstEInfoItm;i++)
			{
			if(PStr_DisCmmInfo->Str_ID.uB_ID2==GStr_HstEInfo[i].uW_ID2)
				{
				break;
				}
			}
		if(i>=Max_HstEInfoItm){return 1;}
        if((PStr_DisCmmInfo->Str_ID.uB_ID1)>GStr_HstEInfo[i].uB_TrfSm)
            {
            if(PStr_DisCmmInfo->Str_ID.uB_ID2>0x0c){return 1;}
            else if(0xff!=PStr_DisCmmInfo->Str_ID.uB_ID1){return 1;}
            }           
		if(PStr_DisCmmInfo->Str_ID.uB_ID0==0xff)
		    {
		    if(0x0d==PStr_DisCmmInfo->Str_ID.uB_ID2){return 1;}
            else {return Gcst_Success;}
		    }         
        else if(PStr_DisCmmInfo->Str_ID.uB_ID0>(GStr_HstEInfo[i].uB_IdxSum+1)){return 1;}
		LuW_Addr=GStr_HstEInfo[i].uW_Addr;
		LuB_Dat=*(GStr_HstEInfo[i].PuB_IdxPntr)+ 
			PStr_DisCmmInfo->Str_ID.uB_ID0-1;
		
		if(LuB_Dat>GStr_HstEInfo[i].uB_IdxSum)
			{LuB_Dat-=(GStr_HstEInfo[i].uB_IdxSum+1);}
		
		if(LuB_Dat>GStr_HstEInfo[i].uB_IdxSum){return 1;}
		LuW_Addr+=((uint16_t)LuB_Dat)*((uint16_t)(GStr_HstEInfo[i].uB_Lenth));
		if(PStr_DisCmmInfo->Str_ID.uB_ID2>2)
			{
			Recover_EeprDat(GAuB_I2cBuf, 
				(GStr_HstEInfo[i].uB_Lenth)-1,LuW_Addr);		
			}
		else
			{
			//Recover_EeprDat(GAuB_I2cBuf, 
				//(GuB_TrfSm+1)*4,LuW_Addr);	
			Recover_EeprDat(GAuB_I2cBuf, 
				(Cst_MaxTariffNum+1)*4,LuW_Addr);	
			}		
		//if((PStr_DisCmmInfo->Str_ID.uB_ID1)<=GuB_TrfSm)
			{
			String_Copy(PStr_DisCmmInfo->AuB_DisBuf,
				(GAuB_I2cBuf+PStr_DisCmmInfo->Str_ID.uB_ID1*4),4);	
			}
		PStr_DisCmmInfo->uB_Prpt=0x40;
		}
	else
		{//present
		for(i=0;i<Max_PrsEAddrItm;i++)
			{
			if(PStr_DisCmmInfo->Str_ID.uB_ID2==GStr_PrsEAddr[i].uB_ID2)
				{
				break;
				}
			}
		if(i>=Max_PrsEAddrItm){return 1;}
		if(PStr_DisCmmInfo->Str_ID.uB_ID1==0xff)
			{
			if(0x0d==PStr_DisCmmInfo->Str_ID.uB_ID2){return 1;}
			else PStr_DisCmmInfo->uW_Addr=GStr_PrsEAddr[i].uW_Addr;
			}
        else
            {
           // if((PStr_DisCmmInfo->Str_ID.uB_ID1)>GuB_TrfSm){return 1;}
            if((PStr_DisCmmInfo->Str_ID.uB_ID1)>GStr_PrsEAddr[i].uB_TrfSm){return 1;}
            else
			{
			PStr_DisCmmInfo->uW_Addr=GStr_PrsEAddr[i].uW_Addr+
				PStr_DisCmmInfo->Str_ID.uB_ID1*4;
			}	
			}
		PStr_DisCmmInfo->uB_Prpt=0x00;
		}	
	PStr_DisCmmInfo->uB_FuPst_FrmL=0x03;
	PStr_DisCmmInfo->uB_SubItm=0;
	return 0;
} 

uint8_t Get_ID32_DisInfo(StrDef_DisItemInfo *PStr_DisCmmInfo)
{
	uint8_t LuB_ItmN;

	LuB_ItmN=CVr2uchar(GetCmmDisNmb32((&(PStr_DisCmmInfo->Str_ID.uB_ID0)),
		(StrDef_CmmDisID32 *)(&(GStr_CmmDisID32[0])),Max_CmmDisID32));
	if(LuB_ItmN>=Max_CmmDisID32){	return 1;}
	
	PStr_DisCmmInfo->uB_CmmL=GStr_CmmDisID32[LuB_ItmN].uB_Lenth;
	PStr_DisCmmInfo->uB_FuPst_FrmL=GStr_CmmDisID32[LuB_ItmN].uB_FuPst_FrmL;
	
	PStr_DisCmmInfo->uB_S0=GStr_CmmDisID32[LuB_ItmN].uB_S0;
	String_Copy(PStr_DisCmmInfo->AuB_DisBuf, 
			(uint8_t *)(GStr_CmmDisID32[LuB_ItmN].uW_Addr),4);
	PStr_DisCmmInfo->uW_Addr=
		GStr_CmmDisID32[LuB_ItmN].uW_Addr;
	PStr_DisCmmInfo->uB_Prpt=GStr_CmmDisID32[LuB_ItmN].uB_Prpt;
	PStr_DisCmmInfo->uB_SubItm=0;
	return 0; 
}

uint8_t Get_ID33_DisInfo(StrDef_DisItemInfo *PStr_DisCmmInfo)
{
	uint8_t i,j,LuB_Dat,LuB_Lenth;
	uint16_t LuW_Addr;
	UDef_u32 LUuL_Tms;
	if(PStr_DisCmmInfo->Str_ID.uB_ID0)  
		{//history
		for(i=0;i<Max_CmmID33H;i++)
			{
			if(CVrAddr2uW(&(PStr_DisCmmInfo->Str_ID.uB_ID1))==
				GStr_CmmID33H[i].uW_ID2ID1){break;}
			}
		if(i>=Max_CmmID33H){return 1;}
		
		if((PStr_DisCmmInfo->Str_ID.uB_ID0-1)>GStr_CmmID33H[i].uB_IdxSum){return 1;}
		LuW_Addr=GStr_CmmID33H[i].uW_Addr;
		LuB_Dat=*(GStr_CmmID33H[i].PuB_IdxPntr)+
			PStr_DisCmmInfo->Str_ID.uB_ID0-1;
	
		if(LuB_Dat>GStr_CmmID33H[i].uB_IdxSum)
			{LuB_Dat-=(GStr_CmmID33H[i].uB_IdxSum+1);}
		
		LuW_Addr+=((uint16_t)LuB_Dat)*((uint16_t)(GStr_CmmID33H[i].uB_Lenth));
		
		if(CVrAddr2uW(&(PStr_DisCmmInfo->Str_ID.uB_ID1))!=0x3005)
			{
			LuB_Lenth=(GStr_CmmID33H[i].uB_Lenth-Cst_EvnChksumSize);
			}
		else
			{
			LuB_Lenth=Max_TmSctnNum*6+10;
			}
		Recover_EeprDat(LUuL_Tms.AuB,3,GStr_CmmID33H[i].uW_SmEeAddr);
		LUuL_Tms.AuB[3]=0x00;
		if((uint32_t)(PStr_DisCmmInfo->Str_ID.uB_ID0)<=LUuL_Tms.u32)
			{
			Recover_EeprDat(GAuB_I2cBuf,LuB_Lenth,LuW_Addr);
			GStr_CmmID33H[i].PF_FrmtProc();
			}
		else
			{
			//for(LuB_Dat=0;LuB_Dat<LuB_Lenth;LuB_Dat++)GAuB_I2cBuf[LuB_Dat]=0;
			if(GStr_CmmID33H[i].uB_CmmL>LuB_Lenth)
				{
				for(LuB_Dat=0;LuB_Dat<GStr_CmmID33H[i].uB_CmmL;LuB_Dat++){GAuB_I2cBuf[LuB_Dat]=0;}
				}
			else
				{
				for(LuB_Dat=0;LuB_Dat<LuB_Lenth;LuB_Dat++){GAuB_I2cBuf[LuB_Dat]=0;}
				}			
			}		
		//GStr_CmmID33H[i].PF_FrmtProc();
		PStr_DisCmmInfo->uB_CmmL=GStr_CmmID33H[i].uB_CmmL;
		}
	else
		{//present
		for(i=0;i<Max_CmmID33P;i++)
			{
			if(CVrAddr2uW(&(PStr_DisCmmInfo->Str_ID.uB_ID1))==
				GStr_CmmID33P[i].uW_ID2ID1){break;}
			}
		if(i>=Max_CmmID33P){return 1;}
		LuB_Dat=0;
		for(j=0;j<3;j++)
			{
			if(GStr_CmmID33P[i].AuB_EeLnth[j])
				{
				Recover_EeprDat(GAuB_I2cBuf+LuB_Dat, 
					GStr_CmmID33P[i].AuB_EeLnth[j], 
					GStr_CmmID33P[i].AuW_EeAddr[j]);
				LuB_Dat+=GStr_CmmID33P[i].AuB_EeLnth[j];
				}
			}
		PStr_DisCmmInfo->uB_CmmL=GStr_CmmID33P[i].uB_CmmL;
		}
	return 0;
}

uint8_t Get_ID34_DisInfo(StrDef_DisItemInfo *PStr_DisCmmInfo)
{
	uint8_t LuB_ItmN,LuB_Dat;
	LuB_ItmN=CVr2uchar(GetCmmDisNmb((&(PStr_DisCmmInfo->Str_ID.uB_ID0)),
		(StrDef_CmmDisID *)(&(GStr_CmmDisID34[0])),Max_CmmDisID34));
	if(LuB_ItmN>=Max_CmmDisID34){	return 1;}
	PStr_DisCmmInfo->uB_Prpt=GStr_CmmDisID34[LuB_ItmN].uB_Prpt;
	PStr_DisCmmInfo->uB_FuPst_FrmL=GStr_CmmDisID34[LuB_ItmN].uB_FuPst_FrmL;
	PStr_DisCmmInfo->uB_S0=GStr_CmmDisID34[LuB_ItmN].uB_S0;
	PStr_DisCmmInfo->uB_S1=GStr_CmmDisID34[LuB_ItmN].uB_S1;
	
	LuB_Dat=Get_ItmPstn(PStr_DisCmmInfo->uB_SubItm,
				PStr_DisCmmInfo->uB_FuPst_FrmL);	
	if(GStr_CmmDisID34[LuB_ItmN].uB_Prpt&0x40)
		{//data in eeprom
		Recover_EeprDat(GAuB_I2cBuf, 
			GStr_CmmDisID34[LuB_ItmN].uB_Lenth,(uint16_t)((uint32_t)(GStr_CmmDisID34[LuB_ItmN].uW_Addr)));		
		String_Copy(PStr_DisCmmInfo->AuB_DisBuf,
			(GAuB_I2cBuf+GStr_CmmDisID34[LuB_ItmN].uB_Pst+LuB_Dat),4);
		}
	
	else
		{//data in ram
		//String_Copy(PStr_DisCmmInfo->AuB_DisBuf, 
			//((uint8_t *)(GStr_CmmDisID34[LuB_ItmN].uW_Addr)+LuB_Dat),4);
		PStr_DisCmmInfo->uW_Addr=
			GStr_CmmDisID34[LuB_ItmN].uW_Addr;
		}
	
		return 0;
	
}


uint8_t Get_ID35_DisInfo(StrDef_DisItemInfo *PStr_DisCmmInfo)
{
	uint8_t i;
	uint16_t LuW_Addr,LuB_Dat;
	//if(PStr_DisCmmInfo->Str_ID.uB_ID0> 12)
		//{return 1;}
	if(!(PStr_DisCmmInfo->Str_ID.uB_ID0)) 
		{return 1;}
	for(i=0;i<Max_CmmDisID35;i++)
		{
		if(CVrAddr2uW(&(PStr_DisCmmInfo->Str_ID.uB_ID1))==
			GStr_CmmID35[i].uW_ID2ID1)
			{
			break;
			}
		}
	if(i>=Max_CmmDisID35){return 1;}
	if(PStr_DisCmmInfo->Str_ID.uB_ID0> (GStr_CmmID35[i].uB_IdxSum+1))
		{return 1;}
	LuW_Addr=GStr_CmmID35[i].uW_Addr;
	LuB_Dat=(uint16_t)(*(GStr_CmmID35[i].PuB_IdxPntr));
	LuB_Dat+=(uint16_t)(PStr_DisCmmInfo->Str_ID.uB_ID0-1);
	
	if(LuB_Dat>((uint16_t)(GStr_CmmID35[i].uB_IdxSum)))
		{LuB_Dat-=((uint16_t)(GStr_CmmID35[i].uB_IdxSum+1));}
	
	if(LuB_Dat>(uint16_t)(GStr_CmmID35[i].uB_IdxSum)){return 1;}
	LuW_Addr+=((uint16_t)LuB_Dat)*((uint16_t)(GStr_CmmID35[i].uB_Lenth));
	Recover_EeprDat(GAuB_I2cBuf, 
			(GStr_CmmID35[i].uB_Lenth-Cst_FrzChksumSize),LuW_Addr);
	if(GStr_CmmID35[i].uB_CmmL&0x80)
		{
		String_Copy(GAuB_I2cBuf,( GAuB_I2cBuf+GStr_CmmID35[i].uB_Pst),
			(GuB_TrfSm+1)*4);
		PStr_DisCmmInfo->uB_CmmL=(GuB_TrfSm+1)*4;
		}
	else
		{
		
		String_Copy(GAuB_I2cBuf,( GAuB_I2cBuf+GStr_CmmID35[i].uB_Pst),
			GStr_CmmID35[i].uB_CmmL);
		PStr_DisCmmInfo->uB_CmmL=GStr_CmmID35[i].uB_CmmL;
		}
	
	
	return 0;
}
uint16_t GetCmmDisNmb(uint8_t *PPuL_ID,StrDef_CmmDisID *PPStr_CDID,uint16_t PuW_H)
{
	int16_t LW_L,LW_M,LW_H;
	UDef_u32 LU_IDCmp,LU_IDDat;
	LU_IDCmp.u32=CVrAddr2uL(PPuL_ID);
	LU_IDCmp.AuB[3]=0;
	LW_L=0;LW_H=PuW_H-1;
	while(LW_L<=LW_H)
		{
		LW_M=(LW_L+LW_H)/2;
		LU_IDDat.u32=CVrAddr2uL(&((PPStr_CDID+LW_M)->Str_ID.uB_ID0));
		LU_IDDat.AuB[3]=0;
		if(LU_IDCmp.u32==LU_IDDat.u32)return LW_M;
		if(LU_IDCmp.u32>LU_IDDat.u32)LW_L=LW_M+1;
		else LW_H=LW_M-1;
		}
	return PuW_H;
}
uint16_t GetCmmDisNmb32(uint8_t *PPuL_ID,StrDef_CmmDisID32 *PPStr_CDID,uint16_t PuW_H)
{
	int16_t LW_L,LW_M,LW_H;
	UDef_u32 LU_IDCmp,LU_IDDat;
	LU_IDCmp.u32=CVrAddr2uL(PPuL_ID);
	LU_IDCmp.AuB[3]=0;
	LW_L=0;LW_H=PuW_H-1;
	while(LW_L<=LW_H)
		{
		LW_M=(LW_L+LW_H)/2;
		LU_IDDat.u32=CVrAddr2uL(&((PPStr_CDID+LW_M)->Str_ID.uB_ID0));
		LU_IDDat.AuB[3]=0;
		if(LU_IDCmp.u32==LU_IDDat.u32)return LW_M;
		if(LU_IDCmp.u32>LU_IDDat.u32)LW_L=LW_M+1;
		else LW_H=LW_M-1;
		}
	return PuW_H;
}


uint16_t GetCmmNmb3400(uint8_t *PPuL_ID,StrDef_CmmID3400 *PPStr_CDID,uint16_t PuW_H)
{
	int16_t  LW_L,LW_M,LW_H;
	UDef_u32 LU_IDCmp,LU_IDDat;
	LU_IDCmp.u32=CVrAddr2uL(PPuL_ID);LU_IDCmp.AuB[3]=0;
	LW_L=0;LW_H=PuW_H-1;
	while(LW_L<=LW_H)
		{
		LW_M=(LW_L+LW_H)/2;
		LU_IDDat.u32=CVrAddr2uL(&((PPStr_CDID+LW_M)->Str_ID.uB_ID0));
		LU_IDDat.AuB[3]=0;
		if(LU_IDCmp.u32==LU_IDDat.u32)return LW_M;
		if(LU_IDCmp.u32>LU_IDDat.u32)LW_L=LW_M+1;
		else LW_H=LW_M-1;
		}
	return PuW_H;
}


uint8_t Get_ItmPstn(uint8_t puB_SubItm,uint8_t PuB_Frm)
{
	uint8_t LuB_Dat;
	puB_SubItm&=0x03;
	LuB_Dat=0;
	while(puB_SubItm>0)
		{
		LuB_Dat+=(PuB_Frm&0x03);
		LuB_Dat++;
		PuB_Frm>>=2;
		puB_SubItm--;
		}
	return LuB_Dat;
}

void Get_NxtAutoItem_Dat(void)
{
	uint8_t LuB_Dat,LuB_Sum;
	LuB_Sum=0;
	for(;;)
		{
		Gstr_AutoItmInfo1.uB_PstCnt++;
		if(Gstr_AutoItmInfo1.uB_PstCnt>=GuB_AutoDisSum)
			{
			Gstr_AutoItmInfo1.uB_PstCnt=0;
			
			}			
		//Recover_EeprDat(&(Gstr_AutoItmInfo1.Str_ID.uB_ID0),5,
			//(Cst_AutoDisItmsEeAddr+Gstr_AutoItmInfo1.uB_PstCnt*6));
	    String_Copy(&(Gstr_AutoItmInfo1.Str_ID.uB_ID0), 
            GAuB_AutoDisItmsBf+((uint16_t)(Gstr_AutoItmInfo1.uB_PstCnt))*Cst_DisItmsEeSize,5);
		LuB_Sum++;
		if(!(LuB_Sum&0x07)){GuW_WatchDogCnt=399;}
		if((LuB_Sum>GuB_AutoDisSum)||(LuB_Sum>MaxAutoDisItms))
			{
			PutData2AddruL((&(Gstr_AutoItmInfo1.Str_ID.uB_ID0)),0);
			Gstr_AutoItmInfo1.uB_SubItm=0;
			//Save_EeprDat(&(Gstr_AutoItmInfo1.Str_ID.uB_ID0),5,
				//Cst_AutoDisItmsEeAddr);
            String_Copy(GAuB_AutoDisItmsBf,&(Gstr_AutoItmInfo1.Str_ID.uB_ID0),5);
			GuB_AutoDisSum=2;
			}
		LuB_Dat=Gstr_AutoItmInfo1.Str_ID.uB_ID3;
		if(LuB_Dat==0)
			{
			if(Get_ID30_DisInfo(&Gstr_AutoItmInfo1))
				{(Gstr_AutoItmInfo1.uB_Prpt)&=0xfc;continue;}		
			}
		else if(LuB_Dat==2)
			{
			if(Get_ID32_DisInfo(&Gstr_AutoItmInfo1)) 
				{(Gstr_AutoItmInfo1.uB_Prpt)&=0xfc;continue;}				
			}
		else if(LuB_Dat==4)
			{
			if(Get_ID34_DisInfo(&Gstr_AutoItmInfo1)) 
				{(Gstr_AutoItmInfo1.uB_Prpt)&=0xfc;continue;}				
			}
		
		else {(Gstr_AutoItmInfo1.uB_Prpt)&=0xfc;continue;}
		break;
	}
}

void Display_Screen(void)
{
	uint8_t LuB_Dat;

	
	Clr_LcdBuf();
	Dis_MainDatArea();
	LuB_Dat=Gstr_PrsItmInfo1.Str_ID.uB_ID3;
	if(LuB_Dat==0){Dis_SignID30();}
	else if(LuB_Dat==2){Dis_SignID32();}
	else if(LuB_Dat==4){Dis_SignID34();}
	//else if(LuB_Dat==7){Dis_SignID37();}
	else if(LuB_Dat==8){Dis_SignID38();}
	else {nop();}
	/*
	if(0x900200==CVrDAddr2uL(Gstr_PrsItmInfo1.Str_ID.uB_ID0))
		{
		if(GU_TouZhiPwr.u32){Dis_MinusS;Dis_SheQian;}
		}
*/
	switch(GuB_CardInOutSta)
		{
		case 1:
			Dis_Chakazhong;
			break;
		case 2:
			Dis_Shibai;
			if(0x36==GuB_ICErr){Dis_Tunji;}
			break;
		case 3:			
			Dis_Chenggong;
			break;
		default:
			break;
		}

	if(GuB_MtrBatCnt>2){Dis_Bat;}
	//if((GuB_ClrPsWdErrCnt>2)||(GuB_ProPsWdErrCnt>2)){Dis_PsWLck;}
	//if(GuB_RlyPwrCmmSta){Dis_Lazha;}
	#ifdef NoneProKey
	if((GuB_AuthMacErrCnt>=Max_WrMacErrNum)||
		((GuB_ClrPsWdErrCnt>2)||(GuB_ProPsWdErrCnt>2)))
	#else
	if((GuB_ClrPsWdErrCnt>2)||(GuB_ProPsWdErrCnt>2))
	#endif	
		{	
		Dis_PsWLck;
		}
	
	if(GU_SysDateTime.Str.Second&0x01)
		{
		#ifndef	Remote
		if((GStr_RelayCntl.u8_Alert)||(GU_Remain.u32<=GuL_BaojingLv1Hex))
			{Dis_QingGoudian;}
		#else
		if(GStr_RelayCntl.u8_Alert)
			{Dis_QingGoudian;}
		#endif
		if(((GStr_RelayCntl.u8_TurnOffDly)&&(GStr_RelayCntl.u8_Cmmd))||
			(Gcst_SetOvrCrrDlyWait==GuB_OvrCrrDlyWait))
			{
			Dis_Lazha; 
			}
		}
	if((GStr_RelayStas.RelayEndCmm==Cst_RelayPwrOff)&&
		(GStr_RelayCntl.u8_Cmmd||Gu8_PrepayRlyCmmd||Gu8_TmPwrRlyCmmd)){Dis_Lazha;}
	
	//if(GuB_StartProg){Dis_Biancheng;}
	 
	if(Gu8_RmtPrgFlg==0xaa)
		{
		if(GU_SysDateTime.Str.Second&0x01){Dis_Biancheng;}
		}
	else {if(GuB_IRAuthentDlyCnt){Dis_Biancheng;}}
	if(GF_RvrsOnHold){Dis_FanxiangSign;}
	if(GuB_Dis485TongxunCnt){Dis_Tongxun;}				
	if(GuB_DisZBTongxunCnt){Dis_ZBTongxun;}
	if((0xaa==GuB_RmtMtrRgst)||(0x00==GuB_RmtMtrRgst))
		{
		if(((GU_Remain.u32<=GuL_BaojingLv1Hex)||(GuB_AlmCmmSta))
			&&(GU_SysDateTime.Str.Second&0x01))
			{
			Dis_QingGoudian;
			}	
		
		if(GU_TouZhiPwr.u32>GuL_SheQianLvHex)
			{
			Dis_Touzhi;
			}
		else if((GU_TouZhiPwr.u32>=GuL_SheQianLvHex)&&(GuL_SheQianLvHex))
			{
			Dis_Touzhi;
			}
		else if((GU_TouZhiPwr.u32>0)&&(GU_SysDateTime.Str.Second&0x01)){Dis_Touzhi;}
		}
	else
		{
		if((GuB_AlmCmmSta)&&(GU_SysDateTime.Str.Second&0x01))
			{
			Dis_QingGoudian;
			}	
		}
	if(IS_ProPermit){Dis_EnKey;}
	//////////////////////
	if(GF_LowPwrUp)
		{			
		#ifndef Remote
		if(GuB_StpMode)			
			{	
			if(GuB_StpPrcTblChngFlg_Num&0x80){Dis_Stptbl2;}
			Dis_Stptbl1;
			if(GuB_PrsEStp==0){Dis_StpPrc1;}
			else if(GuB_PrsEStp==1){Dis_StpPrc2;}
			else if(GuB_PrsEStp==2){Dis_StpPrc3;}
			else if(GuB_PrsEStp==3){Dis_StpPrc4;}		
			else if(GuB_PrsEStp==4){Dis_StpPrc1;Dis_StpPrc4;}
			else if(GuB_PrsEStp==5){Dis_StpPrc2;Dis_StpPrc4;}
			else if(GuB_PrsEStp==6){Dis_StpPrc3;Dis_StpPrc4;}
			}	
		#endif
		
		#ifndef Remote
		if(GuB_MulTrffMode)
		#endif
			{
			switch(GuB_PrsntPhsStl)
				{
				case 1:
					Dis_PhStl_Jian;
					break;
				case 2:
					Dis_PhStl_feng;
					break;
				case 3:
					Dis_PhStl_Ping;
					break;
				case 4:
					Dis_PhStl_Gu;
					break;
				default:
					Dis_PhStl_Jian;
					break;
				}
			
			#ifndef Remote
			if(GuB_RunPtrt&0x02)
				{
				if(GuB_TrfTblChngFlg_Num&0x80){Dis_TS2;}
				Dis_TS1;
				}
			else
				{
				if(GuB_TDTSSta&0x01){Dis_TS2;}
				else {Dis_TS1;}
				}
            #else
            if(GuB_TDTSSta&0x01){Dis_TS2;}
				else {Dis_TS1;}
            #endif
			
			
			}
		}
	Screen_convert();
	
}


/****************/
/*name Dis_MainDatArea*/
/*Input:1.Gstr_PrsItmInfo1.uB_SubItm;2.Gstr_PrsItmInfo1.uB_FuPst_FrmL*/
/*         3.Gstr_PrsItmInfo1.AuB_DisBuf*/
/**********************/
void Dis_MainDatArea(void)
{
	uint8_t i,LuB_Sum,LuB_Dat,LuB_Dat1,LuB_FrmL;
	if(0x900200==FCVrDAddr2uL(Gstr_PrsItmInfo1.Str_ID.uB_ID0))
		{
		if(GU_TouZhiPwr.u32){Gstr_PrsItmInfo1.uW_Addr=(GU_TouZhiPwr.AuB);}
		else {Gstr_PrsItmInfo1.uW_Addr=(GU_Remain.AuB);}
		}
	i=Gstr_PrsItmInfo1.uB_SubItm;
	LuB_FrmL=Gstr_PrsItmInfo1.uB_Prpt&0x03;
	if(LuB_FrmL){if(LuB_FrmL>=i){i=LuB_FrmL-i;}}
	
	LuB_Sum=Translate_SBItmSum(i,(Gstr_PrsItmInfo1.uB_FuPst_FrmL))+1;	
	
	if(!(Gstr_PrsItmInfo1.uB_Prpt&0x40))
		{	
		LuB_Dat=0;
		LuB_Dat1=Gstr_PrsItmInfo1.uB_FuPst_FrmL;		
		while(i>0)
			{
			LuB_Dat+=(LuB_Dat1&0x03)+1;
			LuB_Dat1>>=2;
			i--;
			}		
		String_Copy(Gstr_PrsItmInfo1.AuB_DisBuf,
			((uint8_t *)(Gstr_PrsItmInfo1.uW_Addr)+LuB_Dat),4);	

		}	
     if(0x04010001==FCVrDAddr2uL(Gstr_PrsItmInfo1.Str_ID.uB_ID0))
        {
        String_Copy(Gstr_PrsItmInfo1.AuB_DisBuf,
			((uint8_t *)(Gstr_PrsItmInfo1.uW_Addr)+3*ByteDec_To_Hex(Gstr_PrsItmInfo1.uB_SubItm)),4);
        LuB_Sum=3;
        }
	if(Gstr_PrsItmInfo1.uB_Prpt&0x20)
		{
		if(LuB_Sum==1)
			{
			Gstr_PrsItmInfo1.AuB_DisBuf[0]=
				GACh_HexToDec100[Gstr_PrsItmInfo1.AuB_DisBuf[0]];
			}
		else if(LuB_Sum==2)
			{
			//*((uint16_t *)Gstr_PrsItmInfo1.AuB_DisBuf)=
				//WHex_To_CmpDec(*((uint16_t *)Gstr_PrsItmInfo1.AuB_DisBuf));
			PutData2AddruW(Gstr_PrsItmInfo1.AuB_DisBuf,WHex_To_CmpDec(Fun_CVrAddr2uW(Gstr_PrsItmInfo1.AuB_DisBuf)));
			}
		else
			{
			//*((uint32_t*)Gstr_PrsItmInfo1.AuB_DisBuf)=
				//Hex_To_CmpDec(*((uint32_t*)Gstr_PrsItmInfo1.AuB_DisBuf));		
			PutData2AddruL(Gstr_PrsItmInfo1.AuB_DisBuf,Hex_To_CmpDec(Fun_CVrAddr2uL(Gstr_PrsItmInfo1.AuB_DisBuf)));
			}
		}
	/*
	if(GPStr_DisID->uB_Prpt&0x20)
		{//hex;
		
			
		}
		*/

	
	LuB_Sum*=2;
	for(i=LuB_Sum;i>0;i--)
		{
		LuB_Dat=Gstr_PrsItmInfo1.AuB_DisBuf[(i-1)>>1];
		if(i&0x01)
			{
			LuB_Dat&=0x0f;			
			}
		else
			{			
			LuB_Dat>>=4;	
			}
		Dis_1char(i-1,LuB_Dat);
		}	
	if(CVrAddr2uW(&(Gstr_PrsItmInfo1.Str_ID.uB_ID2))==0)
		{
		if(Gstr_PrsItmInfo1.AuB_DisBuf[3]&0x80)
			{
			LuB_Dat=Gstr_PrsItmInfo1.AuB_DisBuf[3]>>4;
			Dis_1char(7,0x07&LuB_Dat); 
			}
		}
}

/*Input:1.Gstr_PrsItmInfo1.Str_ID**/
void Dis_SignID30(void)
{
	uint8_t i,LuB_FrctNum,LuB_SprsPst;
	/*
	for(i=7;i>2;i--)	
		{
		if((GStr_LcdBuf.LCD_LgcBuf[i]&(~LcdSingMask))==Char_0)
			{GStr_LcdBuf.LCD_LgcBuf[i]=Char_space;}
		else {break;}
		}
	*/
	LuB_SprsPst=SupressUp0(2);
	//Dis_DLPoint;
	if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x90)
		{
		if(CVrAddr2uW(&(Gstr_PrsItmInfo1.Str_ID.uB_ID0))==0x200)
			{
			if(GU_TouZhiPwr.u32){Dis_1char(LuB_SprsPst+1,Char_RowPstn);Dis_SheQian;/*Dis_MinusS;*/}
			Dis_Shengyu;Dis_Jin_e;Dis_Yuan;			
			}
		Dis_DLPoint;
		return;
		}
	//////////////////////////////
	if(GuB_EngFrct>2){LuB_FrctNum=0;}
	else {LuB_FrctNum=2-GuB_EngFrct;}		
	//(*(uint32_t *)(GStr_LcdBuf.LCD_LgcBuf))=(*(uint32_t *)(GStr_LcdBuf.LCD_LgcBuf+LuB_FrctNum*2));
   // (*(uint32_t *)(GStr_LcdBuf.LCD_LgcBuf+4))=(*(uint32_t *)(GStr_LcdBuf.LCD_LgcBuf+LuB_FrctNum*2+4));
	switch(LuB_FrctNum)
		{
		case 0:
			Dis_Point1;
			break;
		case 1:						
            for(i=0;i<7;i++)
                	{
			GStr_LcdBuf.LCD_LgcBuf[2*i]=GStr_LcdBuf.LCD_LgcBuf	[2+2*i];
			GStr_LcdBuf.LCD_LgcBuf[1+2*i]=GStr_LcdBuf.LCD_LgcBuf[3+2*i];
			}
			GStr_LcdBuf.LCD_LgcBuf[14]=0;GStr_LcdBuf.LCD_LgcBuf[15]=0;
			Dis_Point0;
			break;
		case 2:			
           for(i=0;i<6;i++)
                	{
			GStr_LcdBuf.LCD_LgcBuf[2*i]=GStr_LcdBuf.LCD_LgcBuf[4+2*i];
			GStr_LcdBuf.LCD_LgcBuf[1+2*i]=GStr_LcdBuf.LCD_LgcBuf[5+2*i];
			}
			PutData2AddruL(GStr_LcdBuf.LCD_LgcBuf+12,0);
			break;
		default:				
			break;
		}
	///////////////////////////////
	if(Gstr_PrsItmInfo1.Str_ID.uB_ID0==0){Dis_Dangqian;}
	else
		{
		Dis_Shangyue;
		if(Gstr_PrsItmInfo1.Str_ID.uB_ID0<10)
			{
			Dis_1char(8,Gstr_PrsItmInfo1.Str_ID.uB_ID0);
			}
		else
			{
			i=GACh_HexToDec100[(Gstr_PrsItmInfo1.Str_ID.uB_ID0-10)];
			Dis_1char(8,i);Dis_Shang_1;
			}
		}
	Dis_Dianlang;
	if(Gstr_PrsItmInfo1.Str_ID.uB_ID1==0){Dis_Zong;}
	if(Gstr_PrsItmInfo1.Str_ID.uB_ID1==1){Dis_Jian;}	
	else if(Gstr_PrsItmInfo1.Str_ID.uB_ID1==2){Dis_feng;}
	else if(Gstr_PrsItmInfo1.Str_ID.uB_ID1==3){Dis_Ping;}
	else
		{
		if(Gstr_PrsItmInfo1.Str_ID.uB_ID1==4){Dis_Gu;}
		}
	
	//else if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x00){Dis_Zhuhe;}
	//else {Dis_Yong;}
		
	if(Gstr_PrsItmInfo1.Str_ID.uB_ID2<3)
		{Dis_Kwh;Dis_Yougong;
		if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==1){Dis_Zhengxiang;}
		else if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==2){Dis_Fanxiang;}
		else {if(Gstr_PrsItmInfo1.AuB_DisBuf[3]&0x80){Dis_MinusS;}}
		}
	else if((Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x0b)||(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x0c))
        {
         Dis_Yong;Dis_Kwh;Dis_Yue;
        if(Gstr_PrsItmInfo1.Str_ID.uB_ID0<2){Dis_1char(8,0x01);}
        }
}
void Dis_SignID32(void)
{	
	uint8_t i,LuB_FrctNum;
	if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x01)
		{		
		Dis_Point0;
		SupressUp0(1);
		Dis_Fu_V;
		}
	else if((Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x02)||
		(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x80))
		{
		if(Gstr_PrsItmInfo1.Str_ID.uB_ID0==0x0b)
			{
			SupressUp0(4);			
			Dis_Point3;Dis_Yuan;Dis_Dangqian;Dis_Dian;Dis_jia;
			Dis_Jieti;
			}
		else if(Gstr_PrsItmInfo1.Str_ID.uB_ID0==0x20)
			{
			SupressUp0(4);			
			Dis_Point3;Dis_Yuan;Dis_Dangqian;Dis_Dian;Dis_jia;
			}
		else if(Gstr_PrsItmInfo1.Str_ID.uB_ID0==0x02){Dis_Point1;}
		else 
			{		
			if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x02){Dis_Line_L;}
			else {Dis_Neutal_N;}
			i=Gstr_PrsItmInfo1.AuB_DisBuf[2]&0x7f;
			Dis_1Signchar(5,(i>>4));
			SupressUp0(3);
			if(Gstr_PrsItmInfo1.AuB_DisBuf[2]&0x80)
				{				
				//Dis_1char(LuB_SprsPst+1,Char_RowPstn);
				Dis_MinusS;
				}
			Dis_Point2;
			Dis_An_A;
			}

		}
	else if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x03) 
		{	
		i=Gstr_PrsItmInfo1.AuB_DisBuf[2]&0x7f;
		Dis_1Signchar(5,(i>>4));
		SupressUp0(4);
		if(Gstr_PrsItmInfo1.AuB_DisBuf[2]&0x80)
			{			
			//Dis_1char(LuB_SprsPst+1,Char_RowPstn);
			Dis_MinusS;
			}	
		if(GuB_PwrFrct>4){LuB_FrctNum=0;}
		else {LuB_FrctNum=4-GuB_PwrFrct;}		
		switch(LuB_FrctNum)
			{
			case 0:
				Dis_Point3;
				break;
			case 1:						
                for(i=0;i<7;i++)
                    	{
				GStr_LcdBuf.LCD_LgcBuf[2*i]=GStr_LcdBuf.LCD_LgcBuf[2+2*i];		
				GStr_LcdBuf.LCD_LgcBuf[1+2*i]=GStr_LcdBuf.LCD_LgcBuf[3+2*i];
				}
				GStr_LcdBuf.LCD_LgcBuf[14]=0;		
				GStr_LcdBuf.LCD_LgcBuf[15]=0;
				Dis_Point2;
				break;
			case 2:			
				for(i=0;i<6;i++)
                    		{
					GStr_LcdBuf.LCD_LgcBuf[2*i]=GStr_LcdBuf.LCD_LgcBuf[4+2*i];		
					GStr_LcdBuf.LCD_LgcBuf[1+2*i]=GStr_LcdBuf.LCD_LgcBuf[5+2*i];
					}
			        PutData2AddruL(GStr_LcdBuf.LCD_LgcBuf+12,0);
				Dis_Point1;
				break;
			case 3:
                		for(i=0;i<5;i++)
                    		{
					GStr_LcdBuf.LCD_LgcBuf[2*i]=GStr_LcdBuf.LCD_LgcBuf[6+2*i];		
					GStr_LcdBuf.LCD_LgcBuf[1+2*i]=GStr_LcdBuf.LCD_LgcBuf[7+2*i];
					}
			      PutData2AddruL(GStr_LcdBuf.LCD_LgcBuf+10,0);
				GStr_LcdBuf.LCD_LgcBuf[14]=0;		
				GStr_LcdBuf.LCD_LgcBuf[15]=0;
				Dis_Point0;
				break;
			case 4:	
				 for(i=0;i<4;i++)
                    		{
					GStr_LcdBuf.LCD_LgcBuf[2*i]=GStr_LcdBuf.LCD_LgcBuf[8+2*i];		
					GStr_LcdBuf.LCD_LgcBuf[1+2*i]=GStr_LcdBuf.LCD_LgcBuf[9+2*i];
					}
			    	PutData2AddruL(GStr_LcdBuf.LCD_LgcBuf+8,0);
				PutData2AddruL(GStr_LcdBuf.LCD_LgcBuf+12,0);
				break;
			default:				
				break;
			}
		Dis_Kw;	
		}
	else if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x06)
		{
		Dis_Point2;Dis_Cos;
		if(Gstr_PrsItmInfo1.AuB_DisBuf[1]&0x80)
			{
			//Dis_1char(4,Char_RowPstn);
			Dis_MinusS;
			i=Gstr_PrsItmInfo1.AuB_DisBuf[1]&0x7f;
			Dis_1Signchar(3,(i>>4));
			}
		}
	else {nop();}	
	
}


void Dis_SignID34(void) 
{
	uint8_t i;
	if(Gstr_PrsItmInfo1.uB_Prpt&0x10){SupressUp0(2);}
	if((Gstr_PrsItmInfo1.uB_S0)&0x01)Dis_DLPoint;
	if((Gstr_PrsItmInfo1.uB_S0)&0x02){Dis_Point12;Dis_Point32;Dis_Shijian;Dis_Dangqian;}
	if((Gstr_PrsItmInfo1.uB_S0)&0x04)
		{
		Dis_Point1;Dis_Point3;Dis_Riqi;Dis_Dangqian;
		//Dis_1Signchar(6,0);Dis_1Signchar(7,2);
		}
	if((Gstr_PrsItmInfo1.uB_S0)&0x08){Dis_Biaohao;} 
	if((Gstr_PrsItmInfo1.uB_S0)&0x20){Dis_Changshu;}
	/*
	if((Gstr_PrsItmInfo1.uB_S0)&0x10)
		{
		Dis_1char(6,Char_RowPstn);
		if(Gstr_PrsItmInfo1.uB_SubItm==1){Dis_1char(7,Char_LPstn);}
		else
			{
			if(Gstr_PrsItmInfo1.uB_SubItm==2){Dis_1char(7,Char_HPstn);}
			}
		}
		*/
      if((Gstr_PrsItmInfo1.uB_S0)&0x40){Dis_Jin_e;Dis_Yuan;}
	if((Gstr_PrsItmInfo1.uB_S0)&0x80){Dis_Shengyu;Dis_Jin_e;Dis_Yuan;}
	if((Gstr_PrsItmInfo1.uB_S1)&0x80){Dis_Huhao;}
	if((Gstr_PrsItmInfo1.uB_S1)&0x40)
		{
		Dis_1char(2,Char_RowPstn);
		Dis_1char(3,Char_rPstn);
		Dis_1char(4,Char_rPstn);
		Dis_1char(5,0x0e);
		}
	if(CVrAddr2uW(&(Gstr_PrsItmInfo1.Str_ID.uB_ID1))==0x9060)
		{
		Set_LcdBuf();
		}
    if((0x0b==CVrDAddr2uW(Gstr_PrsItmInfo1.Str_ID.uB_ID1))&&
        (Gstr_PrsItmInfo1.Str_ID.uB_ID0<4)&&(Gstr_PrsItmInfo1.Str_ID.uB_ID0))
        {
        Dis_1char(8,Gstr_PrsItmInfo1.Str_ID.uB_ID0);
        }
    if((0x100==CVrDAddr2uW(Gstr_PrsItmInfo1.Str_ID.uB_ID1))&&
        (1==Gstr_PrsItmInfo1.Str_ID.uB_ID0))
        {
        Dis_Shiduan;Dis_Point32;Dis_Point1;
        i=DecimalInc_NCo(Gstr_PrsItmInfo1.uB_SubItm);
        Dis_1char(8,0x0f&i);
        if(0xf0&i){Dis_Shang_1;}
        }
        
    }

/*
void Dis_SignID37(void)
{
	if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==2)
		{
		Dis_Goudian;Dis_Cishu;
		}
	else if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x01)
		{
		if((Gstr_PrsItmInfo1.uB_S0)&0x01){Dis_Shengyu;Dis_Yuan;}
		else if((Gstr_PrsItmInfo1.uB_S0)&0x04){Dis_SheQian;}
		
		else {nop();}
		Dis_DLPoint;
		}

		
	else if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x03)
		{
		if((Gstr_PrsItmInfo1.uB_S0)&0x04){Dis_Tunji;}
		Dis_DLPoint;
		}
	else if(Gstr_PrsItmInfo1.Str_ID.uB_ID2==0x04)
		{
		Dis_Point3;Dis_Yuan;Dis_Dian;Dis_jia;
		Dis_Dangqian;//Dis_Jieti;
		}
	else {nop();} 
	
	
}

*/
void Dis_SignID38(void)
{
	if((Gstr_PrsItmInfo1.uB_S0)&0x01)
		{
		Dis_1char(2,Char_RowPstn);
		Dis_1char(3,Char_rPstn);
		Dis_1char(4,Char_rPstn);
		//Dis_1char(5,Char_spacePstn);
		};
	if(Gstr_PrsItmInfo1.uB_S1&0x01){Dis_Tunji;}
	/*
	if((Gstr_PrsItmInfo1.uB_S0)&0x02)
		{
		Dis_1char(0,Char_RowPstn);
		Dis_1char(5,Char_RowPstn);
		}
		*/
}


uint8_t Translate_SBItmSum(uint8_t PB_SubItm,uint8_t PuB_FrmL)
{
	uint8_t LuB_Sum;
	if(PB_SubItm==0)
			{
			LuB_Sum=PuB_FrmL&0x03;
			}
		else if(PB_SubItm==1)
			{
			LuB_Sum=PuB_FrmL&0x0c;
			LuB_Sum>>=2;
			}
		else if(PB_SubItm==2)
			{
			LuB_Sum=PuB_FrmL&0x30;
			LuB_Sum>>=4;
			}
		else
			{
			LuB_Sum=PuB_FrmL&0xc0;
			LuB_Sum>>=6;
			}
		return LuB_Sum;
}



void Get_PgUpDat(void)
{
	uint8_t LuB_Dat,LuB_Sum;		
	
	
	LuB_Sum=0;
	for(;;)
		{		
		Gstr_KeyItmInfo1.uB_PstCnt++;
		if(Gstr_KeyItmInfo1.uB_PstCnt>=GuB_KeyDisSum)
			{Gstr_KeyItmInfo1.uB_PstCnt=0;GuB_KeyErrDisFlg=1;}
		//Recover_EeprDat((uint8_t *)(&(Gstr_KeyItmInfo1.Str_ID.uB_ID0)),5,
			//(Cst_KeyDisItmsEeAddr+Gstr_KeyItmInfo1.uB_PstCnt*6));
        String_Copy((uint8_t *)(&(Gstr_KeyItmInfo1.Str_ID.uB_ID0)), 
            GAuB_KeyDisItmsBf+(((uint16_t)(Gstr_KeyItmInfo1.uB_PstCnt))*Cst_DisItmsEeSize),5);
				
		LuB_Sum++;
		if(!(LuB_Sum&0x07)){GuW_WatchDogCnt=399;}
		if((LuB_Sum>GuB_KeyDisSum)||(LuB_Sum>MaxKeyDisItms))
			{
			PutData2AddruL((&(Gstr_KeyItmInfo1.Str_ID.uB_ID0)),0);
			Gstr_KeyItmInfo1.uB_SubItm=0;
			//Save_EeprDat(&(Gstr_KeyItmInfo1.Str_ID.uB_ID0),5,
				//Cst_KeyDisItmsEeAddr);
            String_Copy(GAuB_KeyDisItmsBf,&(Gstr_KeyItmInfo1.Str_ID.uB_ID0),5);
			GuB_KeyDisSum=2;
			}
		LuB_Dat=Gstr_KeyItmInfo1.Str_ID.uB_ID3;
		if(LuB_Dat==0)
			{
			if(Get_ID30_DisInfo(&Gstr_KeyItmInfo1))
				{(Gstr_KeyItmInfo1.uB_Prpt)&=0xfc;continue;}
			}
		else if(LuB_Dat==2)
			{
			if(Get_ID32_DisInfo(&Gstr_KeyItmInfo1)) 
				{(Gstr_KeyItmInfo1.uB_Prpt)&=0xfc;continue;}				
			}
		else if(LuB_Dat==4)
			{
			if(Get_ID34_DisInfo(&Gstr_KeyItmInfo1))
				{(Gstr_KeyItmInfo1.uB_Prpt)&=0xfc;continue;}				
			}
		else {(Gstr_KeyItmInfo1.uB_Prpt)&=0xfc;continue;}			
		break;
		}
}
void Dis_KeyErr(void)
{
	while((!((1<<GuB_KeyErrDisCnt)&GuB_SysErr))&&(GuB_KeyErrDisCnt<8))
		{
		GuB_KeyErrDisCnt++;
		}
	if(GuB_KeyErrDisCnt<8)
		{
		GuB_SysErrDisNum=GuB_KeyErrDisCnt+1;
		PutData2AddruL((&(Gstr_KeyItmInfo1.Str_ID.uB_ID0)), 0x04900000);
		Gstr_KeyItmInfo1.uB_SubItm=0;						
		Get_ID34_DisInfo(&Gstr_KeyItmInfo1);
		}
	else if(8==GuB_KeyErrDisCnt)
		{
		if(GuB_EvenErr)
			{
			GuB_EvenErrDisCnt=1;
			GuB_EvenErrDisNum=0x56;
			PutData2AddruL((&(Gstr_KeyItmInfo1.Str_ID.uB_ID0)),0x04900001);
			Gstr_KeyItmInfo1.uB_SubItm=0;						
			Get_ID34_DisInfo(&Gstr_KeyItmInfo1);
			}
		else
			{
			GuB_KeyErrDisCnt++;
			}
		}
	else
		{
		GuB_KeyErrDisCnt++;
		}
	
}


void Dis_KeyItmInfo(void)
{
	
	////////////////////////////
	String_Copy(Gstr_PrsItmInfo1.AuB_DisBuf,
				Gstr_KeyItmInfo1.AuB_DisBuf,(sizeof(StrDef_DisItemInfo)-1));
	Gstr_PrsItmInfo1.uB_PstCnt=Gstr_KeyItmInfo1.uB_PstCnt;
	/////////////////////////////
	Display_Screen();
}

/*
void Dis_CrdItmInfo(void)
{
	
	////////////////////////////
	String_Copy(Gstr_PrsItmInfo1.AuB_DisBuf,
				Gstr_CardItmInfo.AuB_DisBuf,17);
	Gstr_PrsItmInfo1.uB_PstCnt=Gstr_CardItmInfo.uB_PstCnt;
	/////////////////////////////
	Display_Screen();
}
*/
/*
void Get_NxtDisDat(StrDef_DisItemInfo *PStr_DisItmInfo,
	StrDef_ID1 *PStr_DisID,uint8_t PuB_MaxItm)
{
	uint8_t i,LuB_Dat,LuB_Dat1,LuB_Sum;		
	
	LuB_Sum=0;
	for(;;)
		{			
		PStr_DisItmInfo->uB_PstCnt++;
		if(PStr_DisItmInfo->uB_PstCnt>=PuB_MaxItm)PStr_DisItmInfo->uB_PstCnt=0;		
		String_Copy((uint8_t *)(&(PStr_DisItmInfo->Str_ID.uB_ID0)), 
			(uint8_t *)(PStr_DisID+PStr_DisItmInfo->uB_PstCnt),4);	
		LuB_Sum++;
		if(LuB_Sum>PuB_MaxItm){break;}
		LuB_Dat=PStr_DisItmInfo->Str_ID.uB_ID3;
		if(LuB_Dat==0)
			{
			if(Get_ID30_DisInfo(PStr_DisItmInfo))
				{continue;}
			}
		else if(LuB_Dat==4)
			{
			if(Get_ID34_DisInfo(PStr_DisItmInfo))
				{continue;}
			}
		
		else 
			{
			continue;
			}			
		break;
		}
}
*/

void Dis_ICX(uint8_t PuB_Num)
{
	Gstr_PrsItmInfo1.AuB_DisBuf[0]=PuB_Num;
	Gstr_PrsItmInfo1.AuB_DisBuf[1]=0xc0;
	Gstr_PrsItmInfo1.AuB_DisBuf[2]=0xe1;
	Gstr_PrsItmInfo1.AuB_DisBuf[3]=0x00;
	Gstr_PrsItmInfo1.uB_SubItm=0;
	Gstr_PrsItmInfo1.uB_FuPst_FrmL=0x02;
	Gstr_PrsItmInfo1.uB_Prpt=0x40;
	Gstr_PrsItmInfo1.Str_ID.uB_ID3=8;
	Gstr_PrsItmInfo1.uB_S0=0x01;
	Gstr_PrsItmInfo1.uB_S1=0x00;
	if(PuB_Num==0x14){Gstr_PrsItmInfo1.uB_S1|=0x01;}
}
void Dis_Remain(void)
{
	PutData2AddruL(&(Gstr_PrsItmInfo1.Str_ID.uB_ID0),0x900200);
	Gstr_PrsItmInfo1.uB_SubItm=0;
	Get_ID30_DisInfo(&Gstr_PrsItmInfo1);	
	GuB_DisStatus=3;GuB_DisIntrmCnt=2;
}
void Dis_LstGoudian(void)
{
	PutData2AddruL(&(Gstr_PrsItmInfo1.Str_ID.uB_ID0),0x04901001);
	Gstr_PrsItmInfo1.uB_SubItm=0;
	Get_ID34_DisInfo(&Gstr_PrsItmInfo1);	
	GuB_DisStatus=3;GuB_DisIntrmCnt=6;
}
void Dis_LstRemain(void)
{
	GU_LstRemain.u32=GU_Remain.u32;
	PutData2AddruL(&(Gstr_PrsItmInfo1.Str_ID.uB_ID0),0x04901002);
	Gstr_PrsItmInfo1.uB_SubItm=0;
	Get_ID34_DisInfo(&Gstr_PrsItmInfo1);	
	GuB_DisStatus=3;GuB_DisIntrmCnt=2;
}
/*
void Dis_MtrNo(uint8_t PuB_SubItm)
{
	CVrDAddr2uL(Gstr_PrsItmInfo1.Str_ID.uB_ID0)=0x4000402;
	Gstr_PrsItmInfo1.uB_SubItm=PuB_SubItm;
	Get_ID34_DisInfo(&Gstr_PrsItmInfo1);	
}
*/
uint8_t  SupressUp0(uint8_t PuB_Pstn)
{
	uint8_t i;
	uint16_t LuW_Dat;
	for(i=7;i>PuB_Pstn;i--)	
		{
		LuW_Dat=(CVrAddr2uW(GStr_LcdBuf.LCD_LgcBuf+(i<<1))&(~LcdSingMask));
		if((LuW_Dat==Char_0)||(LuW_Dat==Char_space))
			{
			PutData2AddruW(GStr_LcdBuf.LCD_LgcBuf+(i<<1),
				CVrAddr2uW(GStr_LcdBuf.LCD_LgcBuf+(i<<1))&LcdSingMask);
			}
		else {break;}
		}
	return i;
}

/*
void Dis_IC_Err(void)
{
	Gstr_PrsItmInfo1.AuB_DisBuf[0]=0xe0;
	Gstr_PrsItmInfo1.AuB_DisBuf[1]=0xee;
	Gstr_PrsItmInfo1.AuB_DisBuf[2]=0x0e;
	Gstr_PrsItmInfo1.AuB_DisBuf[3]=0x00;
	Gstr_PrsItmInfo1.uB_SubItm=1;
	Gstr_PrsItmInfo1.uB_FuPst_FrmL=0x02;
	Gstr_PrsItmInfo1.uB_Prpt=0x40;
	Gstr_PrsItmInfo1.Str_ID.uB_ID3=8;
	Gstr_PrsItmInfo1.uB_S0=0x02;
	Gstr_PrsItmInfo1.uB_S1=0x00;
}
*/
/*
void Get_LwpDisDat(void)
{
	uint8_t i,LuB_Dat,LuB_Dat1,LuB_Sum;		
	for(;;)
		{		
		Gstr_KeyItmInfo1.uB_PstCnt++;
		if(Gstr_KeyItmInfo1.uB_PstCnt>=Max_LwrDisID)Gstr_KeyItmInfo1.uB_PstCnt=0;		
		String_Copy((uint8_t *)(&(Gstr_KeyItmInfo1.Str_ID.uB_ID0)), 
			(uint8_t *)(&(GStr_LwrDisID[Gstr_KeyItmInfo1.uB_PstCnt].uB_ID0)),4);			
		LuB_Sum++;
		if(LuB_Sum>Max_LwrDisID){break;}
		LuB_Dat=Gstr_KeyItmInfo1.Str_ID.uB_ID3;
		if(LuB_Dat==0)
			{
			if(Get_ID30_DisInfo(&Gstr_KeyItmInfo1))
				{continue;}
			}
		else if(LuB_Dat==4)
			{
			if(Get_ID34_DisInfo(&Gstr_KeyItmInfo1))
				{continue;}
			}
		else if(LuB_Dat==7)
			{
			if(Get_ID37_DisInfo(&Gstr_KeyItmInfo1))
				{continue;}
			}
		else 
			{
			continue;
			}			
		break;
		}
}
*/

