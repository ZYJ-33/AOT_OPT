
#ifndef __DISASSMBLER
#define __DISASSMBLER
#include <iostream>
#include <cstdlib>
#include <cassert>

typedef enum {
    OPC_INVALID,
    OPC_CLO_W,
    OPC_CLZ_W,
    OPC_CTO_W,
    OPC_CTZ_W,
    OPC_CLO_D,
    OPC_CLZ_D,
    OPC_CTO_D,
    OPC_CTZ_D,
    OPC_REVB_2H,
    OPC_REVB_4H,
    OPC_REVB_2W,
    OPC_REVB_D,
    OPC_REVH_2W,
    OPC_REVH_D,
    OPC_BITREV_4B,
    OPC_BITREV_8B,
    OPC_BITREV_W,
    OPC_BITREV_D,
    OPC_EXT_W_H,
    OPC_EXT_W_B,
    OPC_RDTIMEL_W,
    OPC_RDTIMEH_W,
    OPC_RDTIME_D,
    OPC_CPUCFG,
    OPC_ASRTLE_D,
    OPC_ASRTGT_D,
    OPC_ALSL_W,
    OPC_ALSL_WU,
    OPC_BYTEPICK_W,
    OPC_BYTEPICK_D,
    OPC_ADD_W,
    OPC_ADD_D,
    OPC_SUB_W,
    OPC_SUB_D,
    OPC_SLT,
    OPC_SLTU,
    OPC_MASKEQZ,
    OPC_MASKNEZ,
    OPC_NOR,
    OPC_AND,
    OPC_OR,
    OPC_XOR,
    OPC_ORN,
    OPC_ANDN,
    OPC_SLL_W,
    OPC_SRL_W,
    OPC_SRA_W,
    OPC_SLL_D,
    OPC_SRL_D,
    OPC_SRA_D,
    OPC_ROTR_W,
    OPC_ROTR_D,
    OPC_MUL_W,
    OPC_MULH_W,
    OPC_MULH_WU,
    OPC_MUL_D,
    OPC_MULH_D,
    OPC_MULH_DU,
    OPC_MULW_D_W,
    OPC_MULW_D_WU,
    OPC_DIV_W,
    OPC_MOD_W,
    OPC_DIV_WU,
    OPC_MOD_WU,
    OPC_DIV_D,
    OPC_MOD_D,
    OPC_DIV_DU,
    OPC_MOD_DU,
    OPC_CRC_W_B_W,
    OPC_CRC_W_H_W,
    OPC_CRC_W_W_W,
    OPC_CRC_W_D_W,
    OPC_CRCC_W_B_W,
    OPC_CRCC_W_H_W,
    OPC_CRCC_W_W_W,
    OPC_CRCC_W_D_W,
    OPC_BREAK,
    OPC_DBCL,
    OPC_SYSCALL,
    OPC_ALSL_D,
    OPC_SLLI_W,
    OPC_SLLI_D,
    OPC_SRLI_W,
    OPC_SRLI_D,
    OPC_SRAI_W,
    OPC_SRAI_D,
    OPC_ROTRI_W,
    OPC_ROTRI_D,
    OPC_BSTRINS_W,
    OPC_BSTRPICK_W,
    OPC_BSTRINS_D,
    OPC_BSTRPICK_D,
    OPC_FADD_S,
    OPC_FADD_D,
    OPC_FSUB_S,
    OPC_FSUB_D,
    OPC_FMUL_S,
    OPC_FMUL_D,
    OPC_FDIV_S,
    OPC_FDIV_D,
    OPC_FMAX_S,
    OPC_FMAX_D,
    OPC_FMIN_S,
    OPC_FMIN_D,
    OPC_FMAXA_S,
    OPC_FMAXA_D,
    OPC_FMINA_S,
    OPC_FMINA_D,
    OPC_FSCALEB_S,
    OPC_FSCALEB_D,
    OPC_FCOPYSIGN_S,
    OPC_FCOPYSIGN_D,
    OPC_FABS_S,
    OPC_FABS_D,
    OPC_FNEG_S,
    OPC_FNEG_D,
    OPC_FLOGB_S,
    OPC_FLOGB_D,
    OPC_FCLASS_S,
    OPC_FCLASS_D,
    OPC_FSQRT_S,
    OPC_FSQRT_D,
    OPC_FRECIP_S,
    OPC_FRECIP_D,
    OPC_FRSQRT_S,
    OPC_FRSQRT_D,
    OPC_FMOV_S,
    OPC_FMOV_D,
    OPC_MOVGR2FR_W,
    OPC_MOVGR2FR_D,
    OPC_MOVGR2FRH_W,
    OPC_MOVFR2GR_S,
    OPC_MOVFR2GR_D,
    OPC_MOVFRH2GR_S,
    OPC_MOVGR2FCSR,
    OPC_MOVFCSR2GR,
    OPC_MOVFR2CF,
    OPC_MOVCF2FR,
    OPC_MOVGR2CF,
    OPC_MOVCF2GR,
    OPC_FCVT_S_D,
    OPC_FCVT_D_S,
    OPC_FTINTRM_W_S,
    OPC_FTINTRM_W_D,
    OPC_FTINTRM_L_S,
    OPC_FTINTRM_L_D,
    OPC_FTINTRP_W_S,
    OPC_FTINTRP_W_D,
    OPC_FTINTRP_L_S,
    OPC_FTINTRP_L_D,
    OPC_FTINTRZ_W_S,
    OPC_FTINTRZ_W_D,
    OPC_FTINTRZ_L_S,
    OPC_FTINTRZ_L_D,
    OPC_FTINTRNE_W_S,
    OPC_FTINTRNE_W_D,
    OPC_FTINTRNE_L_S,
    OPC_FTINTRNE_L_D,
    OPC_FTINT_W_S,
    OPC_FTINT_W_D,
    OPC_FTINT_L_S,
    OPC_FTINT_L_D,
    OPC_FFINT_S_W,
    OPC_FFINT_S_L,
    OPC_FFINT_D_W,
    OPC_FFINT_D_L,
    OPC_FRINT_S,
    OPC_FRINT_D,
    OPC_SLTI,
    OPC_SLTUI,
    OPC_ADDI_W,
    OPC_ADDI_D,
    OPC_LU52I_D,
    OPC_ANDI,
    OPC_ORI,
    OPC_XORI,
    OPC_CSRRD,
    OPC_CSRWR,
    OPC_CSRXCHG,
    OPC_CACOP,
    OPC_LDDIR,
    OPC_LDPTE,
    OPC_IOCSRRD_B,
    OPC_IOCSRRD_H,
    OPC_IOCSRRD_W,
    OPC_IOCSRRD_D,
    OPC_IOCSRWR_B,
    OPC_IOCSRWR_H,
    OPC_IOCSRWR_W,
    OPC_IOCSRWR_D,
    OPC_TLBCLR,
    OPC_TLBFLUSH,
    OPC_TLBSRCH,
    OPC_TLBRD,
    OPC_TLBWR,
    OPC_TLBFILL,
    OPC_ERTN,
    OPC_IDLE,
    OPC_INVTLB,
    OPC_FMADD_S,
    OPC_FMADD_D,
    OPC_FMSUB_S,
    OPC_FMSUB_D,
    OPC_FNMADD_S,
    OPC_FNMADD_D,
    OPC_FNMSUB_S,
    OPC_FNMSUB_D,
    OPC_FCMP_CAF_S,
    OPC_FCMP_SAF_S,
    OPC_FCMP_CLT_S,
    OPC_FCMP_STL_S,
    OPC_FCMP_CEQ_S,
    OPC_FCMP_SEQ_S,
    OPC_FCMP_CLE_S,
    OPC_FCMP_SLE_S,
    OPC_FCMP_CUN_S,
    OPC_FCMP_SUN_S,
    OPC_FCMP_CULT_S,
    OPC_FCMP_SULT_S,
    OPC_FCMP_CUEQ_S,
    OPC_FCMP_SUEQ_S,
    OPC_FCMP_CULE_S,
    OPC_FCMP_SULE_S,
    OPC_FCMP_CNE_S,
    OPC_FCMP_SNE_S,
    OPC_FCMP_COR_S,
    OPC_FCMP_SOR_S,
    OPC_FCMP_CUNE_S,
    OPC_FCMP_SUNE_S,
    OPC_FCMP_CAF_D,
    OPC_FCMP_SAF_D,
    OPC_FCMP_CLT_D,
    OPC_FCMP_STL_D,
    OPC_FCMP_CEQ_D,
    OPC_FCMP_SEQ_D,
    OPC_FCMP_CLE_D,
    OPC_FCMP_SLE_D,
    OPC_FCMP_CUN_D,
    OPC_FCMP_SUN_D,
    OPC_FCMP_CULT_D,
    OPC_FCMP_SULT_D,
    OPC_FCMP_CUEQ_D,
    OPC_FCMP_SUEQ_D,
    OPC_FCMP_CULE_D,
    OPC_FCMP_SULE_D,
    OPC_FCMP_CNE_D,
    OPC_FCMP_SNE_D,
    OPC_FCMP_COR_D,
    OPC_FCMP_SOR_D,
    OPC_FCMP_CUNE_D,
    OPC_FCMP_SUNE_D,
    OPC_FSEL,
    OPC_ADDU16I_D,
    OPC_LU12I_W,
    OPC_LU32I_D,
    OPC_PCADDI,
    OPC_PCALAU12I,
    OPC_PCADDU12I,
    OPC_PCADDU18I,
    OPC_LL_W,
    OPC_SC_W,
    OPC_LL_D,
    OPC_SC_D,
    OPC_LDPTR_W,
    OPC_STPTR_W,
    OPC_LDPTR_D,
    OPC_STPTR_D,
    OPC_LD_B,
    OPC_LD_H,
    OPC_LD_W,
    OPC_LD_D,
    OPC_ST_B,
    OPC_ST_H,
    OPC_ST_W,
    OPC_ST_D,
    OPC_LD_BU,
    OPC_LD_HU,
    OPC_LD_WU,
    OPC_PRELD,
    OPC_FLD_S,
    OPC_FST_S,
    OPC_FLD_D,
    OPC_FST_D,
    OPC_LDX_B,
    OPC_LDX_H,
    OPC_LDX_W,
    OPC_LDX_D,
    OPC_STX_B,
    OPC_STX_H,
    OPC_STX_W,
    OPC_STX_D,
    OPC_LDX_BU,
    OPC_LDX_HU,
    OPC_LDX_WU,
    OPC_PRELDX,
    OPC_FLDX_S,
    OPC_FLDX_D,
    OPC_FSTX_S,
    OPC_FSTX_D,
    OPC_AMSWAP_W,
    OPC_AMSWAP_D,
    OPC_AMADD_W,
    OPC_AMADD_D,
    OPC_AMAND_W,
    OPC_AMAND_D,
    OPC_AMOR_W,
    OPC_AMOR_D,
    OPC_AMXOR_W,
    OPC_AMXOR_D,
    OPC_AMMAX_W,
    OPC_AMMAX_D,
    OPC_AMMIN_W,
    OPC_AMMIN_D,
    OPC_AMMAX_WU,
    OPC_AMMAX_DU,
    OPC_AMMIN_WU,
    OPC_AMMIN_DU,
    OPC_AMSWAP_DB_W,
    OPC_AMSWAP_DB_D,
    OPC_AMADD_DB_W,
    OPC_AMADD_DB_D,
    OPC_AMAND_DB_W,
    OPC_AMAND_DB_D,
    OPC_AMOR_DB_W,
    OPC_AMOR_DB_D,
    OPC_AMXOR_DB_W,
    OPC_AMXOR_DB_D,
    OPC_AMMAX_DB_W,
    OPC_AMMAX_DB_D,
    OPC_AMMIN_DB_W,
    OPC_AMMIN_DB_D,
    OPC_AMMAX_DB_WU,
    OPC_AMMAX_DB_DU,
    OPC_AMMIN_DB_WU,
    OPC_AMMIN_DB_DU,
    OPC_DBAR,
    OPC_IBAR,
    OPC_FLDGT_S,
    OPC_FLDGT_D,
    OPC_FLDLE_S,
    OPC_FLDLE_D,
    OPC_FSTGT_S,
    OPC_FSTGT_D,
    OPC_FSTLE_S,
    OPC_FSTLE_D,
    OPC_LDGT_B,
    OPC_LDGT_H,
    OPC_LDGT_W,
    OPC_LDGT_D,
    OPC_LDLE_B,
    OPC_LDLE_H,
    OPC_LDLE_W,
    OPC_LDLE_D,
    OPC_STGT_B,
    OPC_STGT_H,
    OPC_STGT_W,
    OPC_STGT_D,
    OPC_STLE_B,
    OPC_STLE_H,
    OPC_STLE_W,
    OPC_STLE_D,
    OPC_BEQZ,
    OPC_BNEZ,
    OPC_BCEQZ,
    OPC_BCNEZ,
    OPC_JIRL,
    OPC_B,
    OPC_BL,
    OPC_BEQ,
    OPC_BNE,
    OPC_BLT,
    OPC_BGE,
    OPC_BLTU,
    OPC_BGEU,
    OPC_GR2SCR,
    OPC_SRC2GR,
    OPC_X86MTTOP,
    OPC_X86MFTOP,
    OPC_SETX86LOOPE,
    OPC_SETX86LOOPNE,
    OPC_X86INC_B,
    OPC_X86INC_H,
    OPC_X86INC_W,
    OPC_X86INC_D,
    OPC_X86DEC_B,
    OPC_X86DEC_H,
    OPC_X86DEC_W,
    OPC_X86DEC_D,
    OPC_X86SETTM,
    OPC_X86CLRTM,
    OPC_X86INCTOP,
    OPC_X86DECTOP,
    OPC_ROTR_B,
    OPC_ROTR_H,
    OPC_ADDU12I_W,
    OPC_ADDU12I_D,
    OPC_HVCL,
    OPC_ADC_B,
    OPC_ADC_H,
    OPC_ADC_W,
    OPC_ADC_D,
    OPC_SBC_B,
    OPC_SBC_H,
    OPC_SBC_W,
    OPC_SBC_D,
    OPC_RCR_B,
    OPC_RCR_H,
    OPC_RCR_W,
    OPC_RCR_D,
    OPC_ARMMOVE,
    OPC_SETX86J,
    OPC_SETARMJ,
    OPC_ARMADD_W,
    OPC_ARMSUB_W,
    OPC_ARMADC_W,
    OPC_ARMSBC_W,
    OPC_ARMAND_W,
    OPC_ARMOR_W,
    OPC_ARMXOR_W,
    OPC_ARMSLL_W,
    OPC_ARMSRL_W,
    OPC_ARMSRA_W,
    OPC_ARMROTR_W,
    OPC_ARMSLLI_W,
    OPC_ARMSRLI_W,
    OPC_ARMSRAI_W,
    OPC_ARMROTRI_W,
    OPC_X86MUL_B,
    OPC_X86MUL_H,
    OPC_X86MUL_W,
    OPC_X86MUL_D,
    OPC_X86MUL_BU,
    OPC_X86MUL_HU,
    OPC_X86MUL_WU,
    OPC_X86MUL_DU,
    OPC_X86ADD_WU,
    OPC_X86ADD_DU,
    OPC_X86SUB_WU,
    OPC_X86SUB_DU,
    OPC_X86ADD_B,
    OPC_X86ADD_H,
    OPC_X86ADD_W,
    OPC_X86ADD_D,
    OPC_X86SUB_B,
    OPC_X86SUB_H,
    OPC_X86SUB_W,
    OPC_X86SUB_D,
    OPC_X86ADC_B,
    OPC_X86ADC_H,
    OPC_X86ADC_W,
    OPC_X86ADC_D,
    OPC_X86SBC_B,
    OPC_X86SBC_H,
    OPC_X86SBC_W,
    OPC_X86SBC_D,
    OPC_X86SLL_B,
    OPC_X86SLL_H,
    OPC_X86SLL_W,
    OPC_X86SLL_D,
    OPC_X86SRL_B,
    OPC_X86SRL_H,
    OPC_X86SRL_W,
    OPC_X86SRL_D,
    OPC_X86SRA_B,
    OPC_X86SRA_H,
    OPC_X86SRA_W,
    OPC_X86SRA_D,
    OPC_X86ROTR_B,
    OPC_X86ROTR_H,
    OPC_X86ROTR_D,
    OPC_X86ROTR_W,
    OPC_X86ROTL_B,
    OPC_X86ROTL_H,
    OPC_X86ROTL_W,
    OPC_X86ROTL_D,
    OPC_X86RCR_B,
    OPC_X86RCR_H,
    OPC_X86RCR_W,
    OPC_X86RCR_D,
    OPC_X86RCL_B,
    OPC_X86RCL_H,
    OPC_X86RCL_W,
    OPC_X86RCL_D,
    OPC_X86AND_B,
    OPC_X86AND_H,
    OPC_X86AND_W,
    OPC_X86AND_D,
    OPC_X86OR_B,
    OPC_X86OR_H,
    OPC_X86OR_W,
    OPC_X86OR_D,
    OPC_X86XOR_B,
    OPC_X86XOR_H,
    OPC_X86XOR_W,
    OPC_X86XOR_D,
    OPC_ARMNOT_W,
    OPC_ARMMOV_W,
    OPC_ARMMOV_D,
    OPC_ARMRRX_W,
    OPC_ROTRI_B,
    OPC_ROTRI_H,
    OPC_RCRI_B,
    OPC_RCRI_H,
    OPC_RCRI_W,
    OPC_RCRI_D,
    OPC_X86SLLI_B,
    OPC_X86SLLI_H,
    OPC_X86SLLI_W,
    OPC_X86SLLI_D,
    OPC_X86SRLI_B,
    OPC_X86SRLI_H,
    OPC_X86SRLI_W,
    OPC_X86SRLI_D,
    OPC_X86SRAI_B,
    OPC_X86SRAI_H,
    OPC_X86SRAI_W,
    OPC_X86SRAI_D,
    OPC_X86ROTRI_B,
    OPC_X86ROTRI_H,
    OPC_X86ROTRI_W,
    OPC_X86ROTRI_D,
    OPC_X86RCRI_B,
    OPC_X86RCRI_H,
    OPC_X86RCRI_W,
    OPC_X86RCRI_D,
    OPC_X86ROTLI_B,
    OPC_X86ROTLI_H,
    OPC_X86ROTLI_W,
    OPC_X86ROTLI_D,
    OPC_X86RCLI_B,
    OPC_X86RCLI_H,
    OPC_X86RCLI_W,
    OPC_X86RCLI_D,
    OPC_X86SETTAG,
    OPC_X86MFFLAG,
    OPC_X86MTFLAG,
    OPC_ARMMFFLAG,
    OPC_ARMMTFLAG,
    OPC_FCVT_LD_D,
    OPC_FCVT_UD_D,
    OPC_FCVT_D_LC,
    OPC_GCSRRD,
    OPC_GCSRWR,
    OPC_GCSRXCHG,
    OPC_GTLBCLR,
    OPC_GTLBFLUSH,
    OPC_GTLBSRCH,
    OPC_GTLBRD,
    OPC_GTLBWR,
    OPC_GTLBFILL,
    OPC_SCFGR_W,
    OPC_SCFGR_D,
    OPC_SCFGW_W,
    OPC_SCFGW_D,
    OPC_SNOP,
    OPC_LDL_W,
    OPC_LDR_W,
    OPC_LDL_D,
    OPC_LDR_D,
    OPC_STL_W,
    OPC_STR_W,
    OPC_STL_D,
    OPC_STR_D,
    OPC_JISCR0,
    OPC_JISCR1,
    OPC_VADD_B,
    OPC_VADD_H,
    OPC_VADD_W,
    OPC_VADD_D,
    OPC_VADD_Q,
    OPC_VSUB_B,
    OPC_VSUB_H,
    OPC_VSUB_W,
    OPC_VSUB_D,
    OPC_VSUB_Q,
    OPC_VADDI_BU,
    OPC_VADDI_HU,
    OPC_VADDI_WU,
    OPC_VADDI_DU,
    OPC_VSUBI_BU,
    OPC_VSUBI_HU,
    OPC_VSUBI_WU,
    OPC_VSUBI_DU,
    OPC_VNEG_B,
    OPC_VNEG_H,
    OPC_VNEG_W,
    OPC_VNEG_D,
    OPC_VSADD_B,
    OPC_VSADD_H,
    OPC_VSADD_W,
    OPC_VSADD_D,
    OPC_VSADD_BU,
    OPC_VSADD_HU,
    OPC_VSADD_WU,
    OPC_VSADD_DU,
    OPC_VSSUB_B,
    OPC_VSSUB_H,
    OPC_VSSUB_W,
    OPC_VSSUB_D,
    OPC_VSSUB_BU,
    OPC_VSSUB_HU,
    OPC_VSSUB_WU,
    OPC_VSSUB_DU,
    OPC_VHADDW_H_B,
    OPC_VHADDW_W_H,
    OPC_VHADDW_D_W,
    OPC_VHADDW_Q_D,
    OPC_VHADDW_HU_BU,
    OPC_VHADDW_WU_HU,
    OPC_VHADDW_DU_WU,
    OPC_VHADDW_QU_DU,
    OPC_VHSUBW_H_B,
    OPC_VHSUBW_W_H,
    OPC_VHSUBW_D_W,
    OPC_VHSUBW_Q_D,
    OPC_VHSUBW_HU_BU,
    OPC_VHSUBW_WU_HU,
    OPC_VHSUBW_DU_WU,
    OPC_VHSUBW_QU_DU,
    OPC_VADDWEV_H_B,
    OPC_VADDWEV_W_H,
    OPC_VADDWEV_D_W,
    OPC_VADDWEV_Q_D,
    OPC_VADDWOD_H_B,
    OPC_VADDWOD_W_H,
    OPC_VADDWOD_D_W,
    OPC_VADDWOD_Q_D,
    OPC_VSUBWEV_H_B,
    OPC_VSUBWEV_W_H,
    OPC_VSUBWEV_D_W,
    OPC_VSUBWEV_Q_D,
    OPC_VSUBWOD_H_B,
    OPC_VSUBWOD_W_H,
    OPC_VSUBWOD_D_W,
    OPC_VSUBWOD_Q_D,
    OPC_VADDWEV_H_BU,
    OPC_VADDWEV_W_HU,
    OPC_VADDWEV_D_WU,
    OPC_VADDWEV_Q_DU,
    OPC_VADDWOD_H_BU,
    OPC_VADDWOD_W_HU,
    OPC_VADDWOD_D_WU,
    OPC_VADDWOD_Q_DU,
    OPC_VSUBWEV_H_BU,
    OPC_VSUBWEV_W_HU,
    OPC_VSUBWEV_D_WU,
    OPC_VSUBWEV_Q_DU,
    OPC_VSUBWOD_H_BU,
    OPC_VSUBWOD_W_HU,
    OPC_VSUBWOD_D_WU,
    OPC_VSUBWOD_Q_DU,
    OPC_VADDWEV_H_BU_B,
    OPC_VADDWEV_W_HU_H,
    OPC_VADDWEV_D_WU_W,
    OPC_VADDWEV_Q_DU_D,
    OPC_VADDWOD_H_BU_B,
    OPC_VADDWOD_W_HU_H,
    OPC_VADDWOD_D_WU_W,
    OPC_VADDWOD_Q_DU_D,
    OPC_VAVG_B,
    OPC_VAVG_H,
    OPC_VAVG_W,
    OPC_VAVG_D,
    OPC_VAVG_BU,
    OPC_VAVG_HU,
    OPC_VAVG_WU,
    OPC_VAVG_DU,
    OPC_VAVGR_B,
    OPC_VAVGR_H,
    OPC_VAVGR_W,
    OPC_VAVGR_D,
    OPC_VAVGR_BU,
    OPC_VAVGR_HU,
    OPC_VAVGR_WU,
    OPC_VAVGR_DU,
    OPC_VABSD_B,
    OPC_VABSD_H,
    OPC_VABSD_W,
    OPC_VABSD_D,
    OPC_VABSD_BU,
    OPC_VABSD_HU,
    OPC_VABSD_WU,
    OPC_VABSD_DU,
    OPC_VADDA_B,
    OPC_VADDA_H,
    OPC_VADDA_W,
    OPC_VADDA_D,
    OPC_VMAX_B,
    OPC_VMAX_H,
    OPC_VMAX_W,
    OPC_VMAX_D,
    OPC_VMAXI_B,
    OPC_VMAXI_H,
    OPC_VMAXI_W,
    OPC_VMAXI_D,
    OPC_VMAX_BU,
    OPC_VMAX_HU,
    OPC_VMAX_WU,
    OPC_VMAX_DU,
    OPC_VMAXI_BU,
    OPC_VMAXI_HU,
    OPC_VMAXI_WU,
    OPC_VMAXI_DU,
    OPC_VMIN_B,
    OPC_VMIN_H,
    OPC_VMIN_W,
    OPC_VMIN_D,
    OPC_VMINI_B,
    OPC_VMINI_H,
    OPC_VMINI_W,
    OPC_VMINI_D,
    OPC_VMIN_BU,
    OPC_VMIN_HU,
    OPC_VMIN_WU,
    OPC_VMIN_DU,
    OPC_VMINI_BU,
    OPC_VMINI_HU,
    OPC_VMINI_WU,
    OPC_VMINI_DU,
    OPC_VMUL_B,
    OPC_VMUL_H,
    OPC_VMUL_W,
    OPC_VMUL_D,
    OPC_VMUH_B,
    OPC_VMUH_H,
    OPC_VMUH_W,
    OPC_VMUH_D,
    OPC_VMUH_BU,
    OPC_VMUH_HU,
    OPC_VMUH_WU,
    OPC_VMUH_DU,
    OPC_VMULWEV_H_B,
    OPC_VMULWEV_W_H,
    OPC_VMULWEV_D_W,
    OPC_VMULWEV_Q_D,
    OPC_VMULWOD_H_B,
    OPC_VMULWOD_W_H,
    OPC_VMULWOD_D_W,
    OPC_VMULWOD_Q_D,
    OPC_VMULWEV_H_BU,
    OPC_VMULWEV_W_HU,
    OPC_VMULWEV_D_WU,
    OPC_VMULWEV_Q_DU,
    OPC_VMULWOD_H_BU,
    OPC_VMULWOD_W_HU,
    OPC_VMULWOD_D_WU,
    OPC_VMULWOD_Q_DU,
    OPC_VMULWEV_H_BU_B,
    OPC_VMULWEV_W_HU_H,
    OPC_VMULWEV_D_WU_W,
    OPC_VMULWEV_Q_DU_D,
    OPC_VMULWOD_H_BU_B,
    OPC_VMULWOD_W_HU_H,
    OPC_VMULWOD_D_WU_W,
    OPC_VMULWOD_Q_DU_D,
    OPC_VMADD_B,
    OPC_VMADD_H,
    OPC_VMADD_W,
    OPC_VMADD_D,
    OPC_VMSUB_B,
    OPC_VMSUB_H,
    OPC_VMSUB_W,
    OPC_VMSUB_D,
    OPC_VMADDWEV_H_B,
    OPC_VMADDWEV_W_H,
    OPC_VMADDWEV_D_W,
    OPC_VMADDWEV_Q_D,
    OPC_VMADDWOD_H_B,
    OPC_VMADDWOD_W_H,
    OPC_VMADDWOD_D_W,
    OPC_VMADDWOD_Q_D,
    OPC_VMADDWEV_H_BU,
    OPC_VMADDWEV_W_HU,
    OPC_VMADDWEV_D_WU,
    OPC_VMADDWEV_Q_DU,
    OPC_VMADDWOD_H_BU,
    OPC_VMADDWOD_W_HU,
    OPC_VMADDWOD_D_WU,
    OPC_VMADDWOD_Q_DU,
    OPC_VMADDWEV_H_BU_B,
    OPC_VMADDWEV_W_HU_H,
    OPC_VMADDWEV_D_WU_W,
    OPC_VMADDWEV_Q_DU_D,
    OPC_VMADDWOD_H_BU_B,
    OPC_VMADDWOD_W_HU_H,
    OPC_VMADDWOD_D_WU_W,
    OPC_VMADDWOD_Q_DU_D,
    OPC_VDIV_B,
    OPC_VDIV_H,
    OPC_VDIV_W,
    OPC_VDIV_D,
    OPC_VDIV_BU,
    OPC_VDIV_HU,
    OPC_VDIV_WU,
    OPC_VDIV_DU,
    OPC_VMOD_B,
    OPC_VMOD_H,
    OPC_VMOD_W,
    OPC_VMOD_D,
    OPC_VMOD_BU,
    OPC_VMOD_HU,
    OPC_VMOD_WU,
    OPC_VMOD_DU,
    OPC_VSAT_B,
    OPC_VSAT_H,
    OPC_VSAT_W,
    OPC_VSAT_D,
    OPC_VSAT_BU,
    OPC_VSAT_HU,
    OPC_VSAT_WU,
    OPC_VSAT_DU,
    OPC_VEXTH_H_B,
    OPC_VEXTH_W_H,
    OPC_VEXTH_D_W,
    OPC_VEXTH_Q_D,
    OPC_VEXTH_HU_BU,
    OPC_VEXTH_WU_HU,
    OPC_VEXTH_DU_WU,
    OPC_VEXTH_QU_DU,
    OPC_VSIGNCOV_B,
    OPC_VSIGNCOV_H,
    OPC_VSIGNCOV_W,
    OPC_VSIGNCOV_D,
    OPC_VMSKLTZ_B,
    OPC_VMSKLTZ_H,
    OPC_VMSKLTZ_W,
    OPC_VMSKLTZ_D,
    OPC_VMSKGEZ_B,
    OPC_VMSKNZ_B,
    OPC_VLDI,
    OPC_VAND_V,
    OPC_VOR_V,
    OPC_VXOR_V,
    OPC_VNOR_V,
    OPC_VANDN_V,
    OPC_VORN_V,
    OPC_VANDI_B,
    OPC_VORI_B,
    OPC_VXORI_B,
    OPC_VNORI_B,
    OPC_VSLL_B,
    OPC_VSLL_H,
    OPC_VSLL_W,
    OPC_VSLL_D,
    OPC_VSLLI_B,
    OPC_VSLLI_H,
    OPC_VSLLI_W,
    OPC_VSLLI_D,
    OPC_VSRL_B,
    OPC_VSRL_H,
    OPC_VSRL_W,
    OPC_VSRL_D,
    OPC_VSRLI_B,
    OPC_VSRLI_H,
    OPC_VSRLI_W,
    OPC_VSRLI_D,
    OPC_VSRA_B,
    OPC_VSRA_H,
    OPC_VSRA_W,
    OPC_VSRA_D,
    OPC_VSRAI_B,
    OPC_VSRAI_H,
    OPC_VSRAI_W,
    OPC_VSRAI_D,
    OPC_VROTR_B,
    OPC_VROTR_H,
    OPC_VROTR_W,
    OPC_VROTR_D,
    OPC_VROTRI_B,
    OPC_VROTRI_H,
    OPC_VROTRI_W,
    OPC_VROTRI_D,
    OPC_VSLLWIL_H_B,
    OPC_VSLLWIL_W_H,
    OPC_VSLLWIL_D_W,
    OPC_VEXTL_Q_D,
    OPC_VSLLWIL_HU_BU,
    OPC_VSLLWIL_WU_HU,
    OPC_VSLLWIL_DU_WU,
    OPC_VEXTL_QU_DU,
    OPC_VSRLR_B,
    OPC_VSRLR_H,
    OPC_VSRLR_W,
    OPC_VSRLR_D,
    OPC_VSRLRI_B,
    OPC_VSRLRI_H,
    OPC_VSRLRI_W,
    OPC_VSRLRI_D,
    OPC_VSRAR_B,
    OPC_VSRAR_H,
    OPC_VSRAR_W,
    OPC_VSRAR_D,
    OPC_VSRARI_B,
    OPC_VSRARI_H,
    OPC_VSRARI_W,
    OPC_VSRARI_D,
    OPC_VSRLN_B_H,
    OPC_VSRLN_H_W,
    OPC_VSRLN_W_D,
    OPC_VSRAN_B_H,
    OPC_VSRAN_H_W,
    OPC_VSRAN_W_D,
    OPC_VSRLNI_B_H,
    OPC_VSRLNI_H_W,
    OPC_VSRLNI_W_D,
    OPC_VSRLNI_D_Q,
    OPC_VSRANI_B_H,
    OPC_VSRANI_H_W,
    OPC_VSRANI_W_D,
    OPC_VSRANI_D_Q,
    OPC_VSRLRN_B_H,
    OPC_VSRLRN_H_W,
    OPC_VSRLRN_W_D,
    OPC_VSRARN_B_H,
    OPC_VSRARN_H_W,
    OPC_VSRARN_W_D,
    OPC_VSRLRNI_B_H,
    OPC_VSRLRNI_H_W,
    OPC_VSRLRNI_W_D,
    OPC_VSRLRNI_D_Q,
    OPC_VSRARNI_B_H,
    OPC_VSRARNI_H_W,
    OPC_VSRARNI_W_D,
    OPC_VSRARNI_D_Q,
    OPC_VSSRLN_B_H,
    OPC_VSSRLN_H_W,
    OPC_VSSRLN_W_D,
    OPC_VSSRAN_B_H,
    OPC_VSSRAN_H_W,
    OPC_VSSRAN_W_D,
    OPC_VSSRLN_BU_H,
    OPC_VSSRLN_HU_W,
    OPC_VSSRLN_WU_D,
    OPC_VSSRAN_BU_H,
    OPC_VSSRAN_HU_W,
    OPC_VSSRAN_WU_D,
    OPC_VSSRLNI_B_H,
    OPC_VSSRLNI_H_W,
    OPC_VSSRLNI_W_D,
    OPC_VSSRLNI_D_Q,
    OPC_VSSRANI_B_H,
    OPC_VSSRANI_H_W,
    OPC_VSSRANI_W_D,
    OPC_VSSRANI_D_Q,
    OPC_VSSRLNI_BU_H,
    OPC_VSSRLNI_HU_W,
    OPC_VSSRLNI_WU_D,
    OPC_VSSRLNI_DU_Q,
    OPC_VSSRANI_BU_H,
    OPC_VSSRANI_HU_W,
    OPC_VSSRANI_WU_D,
    OPC_VSSRANI_DU_Q,
    OPC_VSSRLRN_B_H,
    OPC_VSSRLRN_H_W,
    OPC_VSSRLRN_W_D,
    OPC_VSSRARN_B_H,
    OPC_VSSRARN_H_W,
    OPC_VSSRARN_W_D,
    OPC_VSSRLRN_BU_H,
    OPC_VSSRLRN_HU_W,
    OPC_VSSRLRN_WU_D,
    OPC_VSSRARN_BU_H,
    OPC_VSSRARN_HU_W,
    OPC_VSSRARN_WU_D,
    OPC_VSSRLRNI_B_H,
    OPC_VSSRLRNI_H_W,
    OPC_VSSRLRNI_W_D,
    OPC_VSSRLRNI_D_Q,
    OPC_VSSRARNI_B_H,
    OPC_VSSRARNI_H_W,
    OPC_VSSRARNI_W_D,
    OPC_VSSRARNI_D_Q,
    OPC_VSSRLRNI_BU_H,
    OPC_VSSRLRNI_HU_W,
    OPC_VSSRLRNI_WU_D,
    OPC_VSSRLRNI_DU_Q,
    OPC_VSSRARNI_BU_H,
    OPC_VSSRARNI_HU_W,
    OPC_VSSRARNI_WU_D,
    OPC_VSSRARNI_DU_Q,
    OPC_VCLO_B,
    OPC_VCLO_H,
    OPC_VCLO_W,
    OPC_VCLO_D,
    OPC_VCLZ_B,
    OPC_VCLZ_H,
    OPC_VCLZ_W,
    OPC_VCLZ_D,
    OPC_VPCNT_B,
    OPC_VPCNT_H,
    OPC_VPCNT_W,
    OPC_VPCNT_D,
    OPC_VBITCLR_B,
    OPC_VBITCLR_H,
    OPC_VBITCLR_W,
    OPC_VBITCLR_D,
    OPC_VBITCLRI_B,
    OPC_VBITCLRI_H,
    OPC_VBITCLRI_W,
    OPC_VBITCLRI_D,
    OPC_VBITSET_B,
    OPC_VBITSET_H,
    OPC_VBITSET_W,
    OPC_VBITSET_D,
    OPC_VBITSETI_B,
    OPC_VBITSETI_H,
    OPC_VBITSETI_W,
    OPC_VBITSETI_D,
    OPC_VBITREV_B,
    OPC_VBITREV_H,
    OPC_VBITREV_W,
    OPC_VBITREV_D,
    OPC_VBITREVI_B,
    OPC_VBITREVI_H,
    OPC_VBITREVI_W,
    OPC_VBITREVI_D,
    OPC_VFRSTP_B,
    OPC_VFRSTP_H,
    OPC_VFRSTPI_B,
    OPC_VFRSTPI_H,
    OPC_VFADD_S,
    OPC_VFADD_D,
    OPC_VFSUB_S,
    OPC_VFSUB_D,
    OPC_VFMUL_S,
    OPC_VFMUL_D,
    OPC_VFDIV_S,
    OPC_VFDIV_D,
    OPC_VFMADD_S,
    OPC_VFMADD_D,
    OPC_VFMSUB_S,
    OPC_VFMSUB_D,
    OPC_VFNMADD_S,
    OPC_VFNMADD_D,
    OPC_VFNMSUB_S,
    OPC_VFNMSUB_D,
    OPC_VFMAX_S,
    OPC_VFMAX_D,
    OPC_VFMIN_S,
    OPC_VFMIN_D,
    OPC_VFMAXA_S,
    OPC_VFMAXA_D,
    OPC_VFMINA_S,
    OPC_VFMINA_D,
    OPC_VFLOGB_S,
    OPC_VFLOGB_D,
    OPC_VFCLASS_S,
    OPC_VFCLASS_D,
    OPC_VFSQRT_S,
    OPC_VFSQRT_D,
    OPC_VFRECIP_S,
    OPC_VFRECIP_D,
    OPC_VFRSQRT_S,
    OPC_VFRSQRT_D,
    OPC_VFCVTL_S_H,
    OPC_VFCVTH_S_H,
    OPC_VFCVTL_D_S,
    OPC_VFCVTH_D_S,
    OPC_VFCVT_H_S,
    OPC_VFCVT_S_D,
    OPC_VFRINT_S,
    OPC_VFRINT_D,
    OPC_VFRINTRM_S,
    OPC_VFRINTRM_D,
    OPC_VFRINTRP_S,
    OPC_VFRINTRP_D,
    OPC_VFRINTRZ_S,
    OPC_VFRINTRZ_D,
    OPC_VFRINTRNE_S,
    OPC_VFRINTRNE_D,
    OPC_VFTINT_W_S,
    OPC_VFTINT_L_D,
    OPC_VFTINTRM_W_S,
    OPC_VFTINTRM_L_D,
    OPC_VFTINTRP_W_S,
    OPC_VFTINTRP_L_D,
    OPC_VFTINTRZ_W_S,
    OPC_VFTINTRZ_L_D,
    OPC_VFTINTRNE_W_S,
    OPC_VFTINTRNE_L_D,
    OPC_VFTINT_WU_S,
    OPC_VFTINT_LU_D,
    OPC_VFTINTRZ_WU_S,
    OPC_VFTINTRZ_LU_D,
    OPC_VFTINT_W_D,
    OPC_VFTINTRM_W_D,
    OPC_VFTINTRP_W_D,
    OPC_VFTINTRZ_W_D,
    OPC_VFTINTRNE_W_D,
    OPC_VFTINTL_L_S,
    OPC_VFTINTH_L_S,
    OPC_VFTINTRML_L_S,
    OPC_VFTINTRMH_L_S,
    OPC_VFTINTRPL_L_S,
    OPC_VFTINTRPH_L_S,
    OPC_VFTINTRZL_L_S,
    OPC_VFTINTRZH_L_S,
    OPC_VFTINTRNEL_L_S,
    OPC_VFTINTRNEH_L_S,
    OPC_VFFINT_S_W,
    OPC_VFFINT_S_WU,
    OPC_VFFINT_D_L,
    OPC_VFFINT_D_LU,
    OPC_VFFINTL_D_W,
    OPC_VFFINTH_D_W,
    OPC_VFFINT_S_L,
    OPC_VSEQ_B,
    OPC_VSEQ_H,
    OPC_VSEQ_W,
    OPC_VSEQ_D,
    OPC_VSEQI_B,
    OPC_VSEQI_H,
    OPC_VSEQI_W,
    OPC_VSEQI_D,
    OPC_VSLE_B,
    OPC_VSLE_H,
    OPC_VSLE_W,
    OPC_VSLE_D,
    OPC_VSLEI_B,
    OPC_VSLEI_H,
    OPC_VSLEI_W,
    OPC_VSLEI_D,
    OPC_VSLE_BU,
    OPC_VSLE_HU,
    OPC_VSLE_WU,
    OPC_VSLE_DU,
    OPC_VSLEI_BU,
    OPC_VSLEI_HU,
    OPC_VSLEI_WU,
    OPC_VSLEI_DU,
    OPC_VSLT_B,
    OPC_VSLT_H,
    OPC_VSLT_W,
    OPC_VSLT_D,
    OPC_VSLTI_B,
    OPC_VSLTI_H,
    OPC_VSLTI_W,
    OPC_VSLTI_D,
    OPC_VSLT_BU,
    OPC_VSLT_HU,
    OPC_VSLT_WU,
    OPC_VSLT_DU,
    OPC_VSLTI_BU,
    OPC_VSLTI_HU,
    OPC_VSLTI_WU,
    OPC_VSLTI_DU,
    OPC_VFCMP_COND_S,
    OPC_VFCMP_COND_D,
    OPC_VBITSEL_V,
    OPC_VBITSELI_B,
    OPC_VSETEQZ_V,
    OPC_VSETNEZ_V,
    OPC_VSETANYEQZ_B,
    OPC_VSETANYEQZ_H,
    OPC_VSETANYEQZ_W,
    OPC_VSETANYEQZ_D,
    OPC_VSETALLNEZ_B,
    OPC_VSETALLNEZ_H,
    OPC_VSETALLNEZ_W,
    OPC_VSETALLNEZ_D,
    OPC_VINSGR2VR_B,
    OPC_VINSGR2VR_H,
    OPC_VINSGR2VR_W,
    OPC_VINSGR2VR_D,
    OPC_VPICKVE2GR_B,
    OPC_VPICKVE2GR_H,
    OPC_VPICKVE2GR_W,
    OPC_VPICKVE2GR_D,
    OPC_VPICKVE2GR_BU,
    OPC_VPICKVE2GR_HU,
    OPC_VPICKVE2GR_WU,
    OPC_VPICKVE2GR_DU,
    OPC_VREPLGR2VR_B,
    OPC_VREPLGR2VR_H,
    OPC_VREPLGR2VR_W,
    OPC_VREPLGR2VR_D,
    OPC_VREPLVE_B,
    OPC_VREPLVE_H,
    OPC_VREPLVE_W,
    OPC_VREPLVE_D,
    OPC_VREPLVEI_B,
    OPC_VREPLVEI_H,
    OPC_VREPLVEI_W,
    OPC_VREPLVEI_D,
    OPC_VBSLL_V,
    OPC_VBSRL_V,
    OPC_VPACKEV_B,
    OPC_VPACKEV_H,
    OPC_VPACKEV_W,
    OPC_VPACKEV_D,
    OPC_VPACKOD_B,
    OPC_VPACKOD_H,
    OPC_VPACKOD_W,
    OPC_VPACKOD_D,
    OPC_VPICKEV_B,
    OPC_VPICKEV_H,
    OPC_VPICKEV_W,
    OPC_VPICKEV_D,
    OPC_VPICKOD_B,
    OPC_VPICKOD_H,
    OPC_VPICKOD_W,
    OPC_VPICKOD_D,
    OPC_VILVL_B,
    OPC_VILVL_H,
    OPC_VILVL_W,
    OPC_VILVL_D,
    OPC_VILVH_B,
    OPC_VILVH_H,
    OPC_VILVH_W,
    OPC_VILVH_D,
    OPC_VSHUF_B,
    OPC_VSHUF_H,
    OPC_VSHUF_W,
    OPC_VSHUF_D,
    OPC_VSHUF4I_B,
    OPC_VSHUF4I_H,
    OPC_VSHUF4I_W,
    OPC_VSHUF4I_D,
    OPC_VPERMI_W,
    OPC_VEXTRINS_D,
    OPC_VEXTRINS_W,
    OPC_VEXTRINS_H,
    OPC_VEXTRINS_B,
    OPC_VLD,
    OPC_VST,
    OPC_VLDX,
    OPC_VSTX,
    OPC_VLDREPL_D,
    OPC_VLDREPL_W,
    OPC_VLDREPL_H,
    OPC_VLDREPL_B,
    OPC_VSTELM_D,
    OPC_VSTELM_W,
    OPC_VSTELM_H,
    OPC_VSTELM_B,
    OPC_XVLD,
    OPC_XVST,
} LoongArchOpc;

typedef struct{ 
    LoongArchOpc opc;
    u_int8_t dst_reg; //rd
    u_int8_t dst_lsx_reg; //vd
    u_int8_t dst_lasx_reg; //xd
    u_int8_t dst_fcsr; //fcsr
    u_int8_t dst_sccrach; //sd 
    u_int8_t dst_freg; //fd
    u_int8_t dst_cfreg; //cd

    u_int8_t num_srcs;
    u_int8_t srcs_regs[3]; //rj rk

    u_int8_t num_lsx;
    u_int8_t lsx_src[3]; // vj vk va
    
    u_int8_t num_lasx;
    u_int8_t lasx_src[3]; //xj xk xa

    u_int8_t num_fcsr;
    u_int8_t srcs_fcsrregs[3]; //fcsr_1

    u_int8_t num_sccrach;
    u_int8_t sccrach_regs[3]; //sj

    u_int8_t num_cfreg;
    u_int8_t srcs_cfregs[3]; //cj ca
    
    u_int8_t num_fsrcs;
    u_int8_t srcs_fregs[3]; // fj fk fa


    u_int16_t csr; //csr 

    u_int8_t sa; // sa2 sa3
    u_int8_t msb; // msbw msbd
    u_int8_t lsb; // lsbw lsbd
    
    u_int16_t code; //code
    u_int8_t code_2; //code_2

    u_int16_t level; //level
    u_int16_t level_3; //level_3
    u_int8_t seq; //seq

    u_int8_t fcond; //fcond

    u_int16_t hint; //hint hint_4 hint5 

    u_int8_t op; // op bt_op
    u_int8_t ptr; // ptr
    u_int8_t cond; // cond_1 cond_2

    u_int16_t ui; //ui5 ui6 ui3 ui4 ui8 ui12 uimm
    u_int16_t _ui; //imm2

    int32_t si; // si12 si16 si20 si14 si5 simm
    int32_t offs; //offs1 offs2 offs3
    u_int32_t origin_binary;

    int nb_opnds;
    int opnds[4];
} LoongArchInsInfo;


typedef struct
{
    u_int8_t kill;
    u_int8_t require_set[4];
    u_int8_t gr_count;
} reg_info;
typedef enum
{
    cv,
    format_0,
    format_1,
    format_10,
    format_11,
    format_12,
    format_13,
    format_14,
    format_15,
    format_16,
    format_17,
    format_18,
    format_19,
    format_2,
    format_20,
    format_21,
    format_22,
    format_23,
    format_24,
    format_25,
    format_26,
    format_27,
    format_28,
    format_29,
    format_3,
    format_30,
    format_31,
    format_32,
    format_33,
    format_34,
    format_35,
    format_36,
    format_37,
    format_38,
    format_39,
    format_4,
    format_40,
    format_41,
    format_42,
    format_43,
    format_44,
    format_45,
    format_46,
    format_47,
    format_48,
    format_49,
    format_5,
    format_50,
    format_51,
    format_52,
    format_53,
    format_54,
    format_55,
    format_56,
    format_57,
    format_58,
    format_59,
    format_6,
    format_60,
    format_61,
    format_62,
    format_63,
    format_64,
    format_7,
    format_8,
    format_9,
    rv_ui1,
    rv_ui2,
    rv_ui3,
    rv_ui4,
    v_i13,
    vr,
    vr_i10,
    vr_i11,
    vr_i12,
    vr_i8i1,
    vr_i8i2,
    vr_i8i3,
    vr_i8i4,
    vr_i9,
    vr_ui1,
    vr_ui2,
    vr_ui3,
    vr_ui4,
    vrr,
    vv,
    vv_i5,
    vv_ui1,
    vv_ui2,
    vv_ui3,
    vv_ui4,
    vv_ui5,
    vv_ui6,
    vv_ui7,
    vv_ui8,
    vvr,
    vvv,
    vvv_fcond,
    vvvv,
    xr_i12,
} formats;
void print_cv(std::ostream& out, LoongArchInsInfo *info);
void print_format_0(std::ostream& out, LoongArchInsInfo *info);
void print_format_1(std::ostream& out, LoongArchInsInfo *info);
void print_format_10(std::ostream& out, LoongArchInsInfo *info);
void print_format_11(std::ostream& out, LoongArchInsInfo *info);
void print_format_12(std::ostream& out, LoongArchInsInfo *info);
void print_format_13(std::ostream& out, LoongArchInsInfo *info);
void print_format_14(std::ostream& out, LoongArchInsInfo *info);
void print_format_15(std::ostream& out, LoongArchInsInfo *info);
void print_format_16(std::ostream& out, LoongArchInsInfo *info);
void print_format_17(std::ostream& out, LoongArchInsInfo *info);
void print_format_18(std::ostream& out, LoongArchInsInfo *info);
void print_format_19(std::ostream& out, LoongArchInsInfo *info);
void print_format_2(std::ostream& out, LoongArchInsInfo *info);
void print_format_20(std::ostream& out, LoongArchInsInfo *info);
void print_format_21(std::ostream& out, LoongArchInsInfo *info);
void print_format_22(std::ostream& out, LoongArchInsInfo *info);
void print_format_23(std::ostream& out, LoongArchInsInfo *info);
void print_format_24(std::ostream& out, LoongArchInsInfo *info);
void print_format_25(std::ostream& out, LoongArchInsInfo *info);
void print_format_26(std::ostream& out, LoongArchInsInfo *info);
void print_format_27(std::ostream& out, LoongArchInsInfo *info);
void print_format_28(std::ostream& out, LoongArchInsInfo *info);
void print_format_29(std::ostream& out, LoongArchInsInfo *info);
void print_format_3(std::ostream& out, LoongArchInsInfo *info);
void print_format_30(std::ostream& out, LoongArchInsInfo *info);
void print_format_31(std::ostream& out, LoongArchInsInfo *info);
void print_format_32(std::ostream& out, LoongArchInsInfo *info);
void print_format_33(std::ostream& out, LoongArchInsInfo *info);
void print_format_34(std::ostream& out, LoongArchInsInfo *info);
void print_format_35(std::ostream& out, LoongArchInsInfo *info);
void print_format_36(std::ostream& out, LoongArchInsInfo *info);
void print_format_37(std::ostream& out, LoongArchInsInfo *info);
void print_format_38(std::ostream& out, LoongArchInsInfo *info);
void print_format_39(std::ostream& out, LoongArchInsInfo *info);
void print_format_4(std::ostream& out, LoongArchInsInfo *info);
void print_format_40(std::ostream& out, LoongArchInsInfo *info);
void print_format_41(std::ostream& out, LoongArchInsInfo *info);
void print_format_42(std::ostream& out, LoongArchInsInfo *info);
void print_format_43(std::ostream& out, LoongArchInsInfo *info);
void print_format_44(std::ostream& out, LoongArchInsInfo *info);
void print_format_45(std::ostream& out, LoongArchInsInfo *info);
void print_format_46(std::ostream& out, LoongArchInsInfo *info);
void print_format_47(std::ostream& out, LoongArchInsInfo *info);
void print_format_48(std::ostream& out, LoongArchInsInfo *info);
void print_format_49(std::ostream& out, LoongArchInsInfo *info);
void print_format_5(std::ostream& out, LoongArchInsInfo *info);
void print_format_50(std::ostream& out, LoongArchInsInfo *info);
void print_format_51(std::ostream& out, LoongArchInsInfo *info);
void print_format_52(std::ostream& out, LoongArchInsInfo *info);
void print_format_53(std::ostream& out, LoongArchInsInfo *info);
void print_format_54(std::ostream& out, LoongArchInsInfo *info);
void print_format_55(std::ostream& out, LoongArchInsInfo *info);
void print_format_56(std::ostream& out, LoongArchInsInfo *info);
void print_format_57(std::ostream& out, LoongArchInsInfo *info);
void print_format_58(std::ostream& out, LoongArchInsInfo *info);
void print_format_59(std::ostream& out, LoongArchInsInfo *info);
void print_format_6(std::ostream& out, LoongArchInsInfo *info);
void print_format_60(std::ostream& out, LoongArchInsInfo *info);
void print_format_61(std::ostream& out, LoongArchInsInfo *info);
void print_format_62(std::ostream& out, LoongArchInsInfo *info);
void print_format_63(std::ostream& out, LoongArchInsInfo *info);
void print_format_64(std::ostream& out, LoongArchInsInfo *info);
void print_format_7(std::ostream& out, LoongArchInsInfo *info);
void print_format_8(std::ostream& out, LoongArchInsInfo *info);
void print_format_9(std::ostream& out, LoongArchInsInfo *info);
void print_rv_ui1(std::ostream& out, LoongArchInsInfo *info);
void print_rv_ui2(std::ostream& out, LoongArchInsInfo *info);
void print_rv_ui3(std::ostream& out, LoongArchInsInfo *info);
void print_rv_ui4(std::ostream& out, LoongArchInsInfo *info);
void print_v_i13(std::ostream& out, LoongArchInsInfo *info);
void print_vr(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i10(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i11(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i12(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i8i1(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i8i2(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i8i3(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i8i4(std::ostream& out, LoongArchInsInfo *info);
void print_vr_i9(std::ostream& out, LoongArchInsInfo *info);
void print_vr_ui1(std::ostream& out, LoongArchInsInfo *info);
void print_vr_ui2(std::ostream& out, LoongArchInsInfo *info);
void print_vr_ui3(std::ostream& out, LoongArchInsInfo *info);
void print_vr_ui4(std::ostream& out, LoongArchInsInfo *info);
void print_vrr(std::ostream& out, LoongArchInsInfo *info);
void print_vv(std::ostream& out, LoongArchInsInfo *info);
void print_vv_i5(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui1(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui2(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui3(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui4(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui5(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui6(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui7(std::ostream& out, LoongArchInsInfo *info);
void print_vv_ui8(std::ostream& out, LoongArchInsInfo *info);
void print_vvr(std::ostream& out, LoongArchInsInfo *info);
void print_vvv(std::ostream& out, LoongArchInsInfo *info);
void print_vvv_fcond(std::ostream& out, LoongArchInsInfo *info);
void print_vvvv(std::ostream& out, LoongArchInsInfo *info);
void print_xr_i12(std::ostream& out, LoongArchInsInfo *info);
void cv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_0_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_10_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_11_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_12_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_13_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_14_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_15_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_16_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_17_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_18_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_19_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_20_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_21_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_22_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_23_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_24_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_25_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_26_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_27_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_28_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_29_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_30_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_31_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_32_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_33_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_34_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_35_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_36_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_37_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_38_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_39_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_40_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_41_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_42_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_43_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_44_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_45_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_46_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_47_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_48_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_49_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_5_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_50_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_51_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_52_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_53_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_54_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_55_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_56_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_57_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_58_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_59_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_6_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_60_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_61_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_62_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_63_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_64_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_7_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_8_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void format_9_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void rv_ui1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void rv_ui2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void rv_ui3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void rv_ui4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void v_i13_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i10_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i11_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i12_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i8i1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i8i2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i8i3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i8i4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_i9_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_ui1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_ui2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_ui3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vr_ui4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vrr_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_i5_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui5_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui6_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui7_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vv_ui8_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vvr_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vvv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vvv_fcond_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void vvvv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void xr_i12_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require);
void encode_encode_cv(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_0(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_1(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_10(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_11(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_12(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_13(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_14(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_15(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_16(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_17(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_18(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_19(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_2(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_20(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_21(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_22(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_23(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_24(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_25(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_26(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_27(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_28(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_29(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_3(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_30(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_31(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_32(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_33(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_34(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_35(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_36(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_37(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_38(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_39(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_4(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_40(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_41(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_42(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_43(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_44(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_45(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_46(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_47(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_48(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_49(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_5(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_50(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_51(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_52(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_53(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_54(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_55(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_56(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_57(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_58(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_59(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_6(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_60(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_61(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_62(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_63(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_64(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_7(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_8(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_format_9(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_rv_ui1(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_rv_ui2(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_rv_ui3(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_rv_ui4(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_v_i13(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i10(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i11(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i12(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i8i1(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i8i2(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i8i3(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i8i4(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_i9(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_ui1(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_ui2(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_ui3(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vr_ui4(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vrr(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_i5(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui1(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui2(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui3(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui4(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui5(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui6(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui7(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vv_ui8(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vvr(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vvv(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vvv_fcond(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_vvvv(LoongArchInsInfo* info, uint32_t* pinsn);
void encode_encode_xr_i12(LoongArchInsInfo* info, uint32_t* pinsn);
uint32_t encode(LoongArchInsInfo*);
bool decode(LoongArchInsInfo* info, uint32_t insn);
#endif
