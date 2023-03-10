#include "Disassmbler/disassmbler.hpp"


/* Register alias */
/*
const char *gpr_name[] = {
    "$zero" , "$ra" , "$tp" , "$sp" , "$a0" , "$a1" , "$a2" , "$a3" ,
    "$a4"   , "$a5" , "$a6" , "$a7" , "$t0" , "$t1" , "$t2" , "$t3" ,
    "$t4"   , "$t5" , "$t6" , "$t7" , "$t8" , "$x"  , "$fp" , "$s0" ,
    "$s1"   , "$s2" , "$s3" , "$s4" , "$s5" , "$s6" , "$s7" , "$s8" ,
};
*/

const char *gpr_name[] = {
    "$zero", "$itmp5", "$bt_tp", "$bt_sp", "$bt_a0",  "$bt_a1", "$x86_r8", "$x86_r9",
    "$x86_r10", "$x86_r11", "$x86_eflag", "$itemp6", "$itemp0", "$itemp1", "$itemp2", "$itemp3",
    "$itemp4", "$x86_r12", "$x86_r13", "$x86_r14", "$x86_r15", "$next_x86_addr", "$last_exec_tb", "$x86_eax",
    "$x86_ecx", "$x86_edx", "$x86_ebx", "$x86_esp", "$x86_ebp", "$x86_esi", "$x86_edi", "$la_env",
};

const char *fpr_name[] = {
    "$fa0"  , "$fa1"  , "$fa2"  , "$fa3"  ,
    "$fa4"  , "$fa5"  , "$fa6"  , "$fa7"  ,
    "$ft0"  , "$ft1"  , "$ft2"  , "$ft3"  ,
    "$ft4"  , "$ft5"  , "$ft6"  , "$ft7"  ,
    "$ft8"  , "$ft9"  , "$ft10" , "$ft11" ,
    "$ft12" , "$ft13" , "$ft14" , "$ft15" ,
    "$fs0"  , "$fs1"  , "$fs2"  , "$fs3"  ,
    "$fs4"  , "$fs5"  , "$fs6"  , "$s7"   ,
};

const char* lsx_name[] = {
    "$V0", "$V1", "$V2", "$V3", "$V4", "$V5", "$V6", "$V7",
    "$V8", "$V9", "$V10", "$V11", "$V12", "$V13", "$V14", "$V15",
    "$V16", "$V17", "$V18", "$V19", "$V20", "$V21", "$V22", "$V23",
    "$V24", "$V25", "$V26", "$V27", "$V28", "$V29", "$V30", "$V31",
};

const char* lasx_name[] = {
    "$X0", "$X1", "$X2", "$X3", "$X4", "$X5", "$X6", "$X7",
    "$X8", "$X9", "$X10", "$X11", "$X12", "$X13", "$X14", "$X15",
    "$X16", "$X17", "$X18", "$X19", "$X20", "$X21", "$X22", "$X23",
    "$X24", "$X25", "$X26", "$X27", "$X28", "$X29", "$X30", "$X31",
};

const char *scr_name[] = {
    "$ss0" , "$ss1" , "$ss2" , "$ss3",
};

const char *fcsr_name[] = {
    "fcsr0", "fcsr1", "fcsr2", "fcsr3"
};

const char *cc_name[] = {
    "$fcc0" , "$fcc1" , "$fcc2" , "$fcc3" ,
    "$fcc4" , "$fcc5" , "$fcc6" , "$fcc7" ,
};
const char *opcode_name[] = {
    [OPC_CLO_W] = "clo.w",
    [OPC_CLZ_W] = "clz.w",
    [OPC_CTO_W] = "cto.w",
    [OPC_CTZ_W] = "ctz.w",
    [OPC_CLO_D] = "clo.d",
    [OPC_CLZ_D] = "clz.d",
    [OPC_CTO_D] = "cto.d",
    [OPC_CTZ_D] = "ctz.d",
    [OPC_REVB_2H] = "revb.2h",
    [OPC_REVB_4H] = "revb.4h",
    [OPC_REVB_2W] = "revb.2w",
    [OPC_REVB_D] = "revb.d",
    [OPC_REVH_2W] = "revh.2w",
    [OPC_REVH_D] = "revh.d",
    [OPC_BITREV_4B] = "bitrev.4b",
    [OPC_BITREV_8B] = "bitrev.8b",
    [OPC_BITREV_W] = "bitrev.w",
    [OPC_BITREV_D] = "bitrev.d",
    [OPC_EXT_W_H] = "ext.w.h",
    [OPC_EXT_W_B] = "ext.w.b",
    [OPC_RDTIMEL_W] = "rdtimel.w",
    [OPC_RDTIMEH_W] = "rdtimeh.w",
    [OPC_RDTIME_D] = "rdtime.d",
    [OPC_CPUCFG] = "cpucfg",
    [OPC_ASRTLE_D] = "asrtle.d",
    [OPC_ASRTGT_D] = "asrtgt.d",
    [OPC_ALSL_W] = "alsl.w",
    [OPC_ALSL_WU] = "alsl.wu",
    [OPC_BYTEPICK_W] = "bytepick.w",
    [OPC_BYTEPICK_D] = "bytepick.d",
    [OPC_ADD_W] = "add.w",
    [OPC_ADD_D] = "add.d",
    [OPC_SUB_W] = "sub.w",
    [OPC_SUB_D] = "sub.d",
    [OPC_SLT] = "slt",
    [OPC_SLTU] = "sltu",
    [OPC_MASKEQZ] = "maskeqz",
    [OPC_MASKNEZ] = "masknez",
    [OPC_NOR] = "nor",
    [OPC_AND] = "and",
    [OPC_OR] = "or",
    [OPC_XOR] = "xor",
    [OPC_ORN] = "orn",
    [OPC_ANDN] = "andn",
    [OPC_SLL_W] = "sll.w",
    [OPC_SRL_W] = "srl.w",
    [OPC_SRA_W] = "sra.w",
    [OPC_SLL_D] = "sll.d",
    [OPC_SRL_D] = "srl.d",
    [OPC_SRA_D] = "sra.d",
    [OPC_ROTR_W] = "rotr.w",
    [OPC_ROTR_D] = "rotr.d",
    [OPC_MUL_W] = "mul.w",
    [OPC_MULH_W] = "mulh.w",
    [OPC_MULH_WU] = "mulh.wu",
    [OPC_MUL_D] = "mul.d",
    [OPC_MULH_D] = "mulh.d",
    [OPC_MULH_DU] = "mulh.du",
    [OPC_MULW_D_W] = "mulw.d.w",
    [OPC_MULW_D_WU] = "mulw.d.wu",
    [OPC_DIV_W] = "div.w",
    [OPC_MOD_W] = "mod.w",
    [OPC_DIV_WU] = "div.wu",
    [OPC_MOD_WU] = "mod.wu",
    [OPC_DIV_D] = "div.d",
    [OPC_MOD_D] = "mod.d",
    [OPC_DIV_DU] = "div.du",
    [OPC_MOD_DU] = "mod.du",
    [OPC_CRC_W_B_W] = "crc.w.b.w",
    [OPC_CRC_W_H_W] = "crc.w.h.w",
    [OPC_CRC_W_W_W] = "crc.w.w.w",
    [OPC_CRC_W_D_W] = "crc.w.d.w",
    [OPC_CRCC_W_B_W] = "crcc.w.b.w",
    [OPC_CRCC_W_H_W] = "crcc.w.h.w",
    [OPC_CRCC_W_W_W] = "crcc.w.w.w",
    [OPC_CRCC_W_D_W] = "crcc.w.d.w",
    [OPC_BREAK] = "break",
    [OPC_DBCL] = "dbcl",
    [OPC_SYSCALL] = "syscall",
    [OPC_ALSL_D] = "alsl.d",
    [OPC_SLLI_W] = "slli.w",
    [OPC_SLLI_D] = "slli.d",
    [OPC_SRLI_W] = "srli.w",
    [OPC_SRLI_D] = "srli.d",
    [OPC_SRAI_W] = "srai.w",
    [OPC_SRAI_D] = "srai.d",
    [OPC_ROTRI_W] = "rotri.w",
    [OPC_ROTRI_D] = "rotri.d",
    [OPC_BSTRINS_W] = "bstrins.w",
    [OPC_BSTRPICK_W] = "bstrpick.w",
    [OPC_BSTRINS_D] = "bstrins.d",
    [OPC_BSTRPICK_D] = "bstrpick.d",
    [OPC_FADD_S] = "fadd.s",
    [OPC_FADD_D] = "fadd.d",
    [OPC_FSUB_S] = "fsub.s",
    [OPC_FSUB_D] = "fsub.d",
    [OPC_FMUL_S] = "fmul.s",
    [OPC_FMUL_D] = "fmul.d",
    [OPC_FDIV_S] = "fdiv.s",
    [OPC_FDIV_D] = "fdiv.d",
    [OPC_FMAX_S] = "fmax.s",
    [OPC_FMAX_D] = "fmax.d",
    [OPC_FMIN_S] = "fmin.s",
    [OPC_FMIN_D] = "fmin.d",
    [OPC_FMAXA_S] = "fmaxa.s",
    [OPC_FMAXA_D] = "fmaxa.d",
    [OPC_FMINA_S] = "fmina.s",
    [OPC_FMINA_D] = "fmina.d",
    [OPC_FSCALEB_S] = "fscaleb.s",
    [OPC_FSCALEB_D] = "fscaleb.d",
    [OPC_FCOPYSIGN_S] = "fcopysign.s",
    [OPC_FCOPYSIGN_D] = "fcopysign.d",
    [OPC_FABS_S] = "fabs.s",
    [OPC_FABS_D] = "fabs.d",
    [OPC_FNEG_S] = "fneg.s",
    [OPC_FNEG_D] = "fneg.d",
    [OPC_FLOGB_S] = "flogb.s",
    [OPC_FLOGB_D] = "flogb.d",
    [OPC_FCLASS_S] = "fclass.s",
    [OPC_FCLASS_D] = "fclass.d",
    [OPC_FSQRT_S] = "fsqrt.s",
    [OPC_FSQRT_D] = "fsqrt.d",
    [OPC_FRECIP_S] = "frecip.s",
    [OPC_FRECIP_D] = "frecip.d",
    [OPC_FRSQRT_S] = "frsqrt.s",
    [OPC_FRSQRT_D] = "frsqrt.d",
    [OPC_FMOV_S] = "fmov.s",
    [OPC_FMOV_D] = "fmov.d",
    [OPC_MOVGR2FR_W] = "movgr2fr.w",
    [OPC_MOVGR2FR_D] = "movgr2fr.d",
    [OPC_MOVGR2FRH_W] = "movgr2frh.w",
    [OPC_MOVFR2GR_S] = "movfr2gr.s",
    [OPC_MOVFR2GR_D] = "movfr2gr.d",
    [OPC_MOVFRH2GR_S] = "movfrh2gr.s",
    [OPC_MOVGR2FCSR] = "movgr2fcsr",
    [OPC_MOVFCSR2GR] = "movfcsr2gr",
    [OPC_MOVFR2CF] = "movfr2cf",
    [OPC_MOVCF2FR] = "movcf2fr",
    [OPC_MOVGR2CF] = "movgr2cf",
    [OPC_MOVCF2GR] = "movcf2gr",
    [OPC_FCVT_S_D] = "fcvt.s.d",
    [OPC_FCVT_D_S] = "fcvt.d.s",
    [OPC_FTINTRM_W_S] = "ftintrm.w.s",
    [OPC_FTINTRM_W_D] = "ftintrm.w.d",
    [OPC_FTINTRM_L_S] = "ftintrm.l.s",
    [OPC_FTINTRM_L_D] = "ftintrm.l.d",
    [OPC_FTINTRP_W_S] = "ftintrp.w.s",
    [OPC_FTINTRP_W_D] = "ftintrp.w.d",
    [OPC_FTINTRP_L_S] = "ftintrp.l.s",
    [OPC_FTINTRP_L_D] = "ftintrp.l.d",
    [OPC_FTINTRZ_W_S] = "ftintrz.w.s",
    [OPC_FTINTRZ_W_D] = "ftintrz.w.d",
    [OPC_FTINTRZ_L_S] = "ftintrz.l.s",
    [OPC_FTINTRZ_L_D] = "ftintrz.l.d",
    [OPC_FTINTRNE_W_S] = "ftintrne.w.s",
    [OPC_FTINTRNE_W_D] = "ftintrne.w.d",
    [OPC_FTINTRNE_L_S] = "ftintrne.l.s",
    [OPC_FTINTRNE_L_D] = "ftintrne.l.d",
    [OPC_FTINT_W_S] = "ftint.w.s",
    [OPC_FTINT_W_D] = "ftint.w.d",
    [OPC_FTINT_L_S] = "ftint.l.s",
    [OPC_FTINT_L_D] = "ftint.l.d",
    [OPC_FFINT_S_W] = "ffint.s.w",
    [OPC_FFINT_S_L] = "ffint.s.l",
    [OPC_FFINT_D_W] = "ffint.d.w",
    [OPC_FFINT_D_L] = "ffint.d.l",
    [OPC_FRINT_S] = "frint.s",
    [OPC_FRINT_D] = "frint.d",
    [OPC_SLTI] = "slti",
    [OPC_SLTUI] = "sltui",
    [OPC_ADDI_W] = "addi.w",
    [OPC_ADDI_D] = "addi.d",
    [OPC_LU52I_D] = "lu52i.d",
    [OPC_ANDI] = "andi",
    [OPC_ORI] = "ori",
    [OPC_XORI] = "xori",
    [OPC_CSRRD] = "csrrd",
    [OPC_CSRWR] = "csrwr",
    [OPC_CSRXCHG] = "csrxchg",
    [OPC_CACOP] = "cacop",
    [OPC_LDDIR] = "lddir",
    [OPC_LDPTE] = "ldpte",
    [OPC_IOCSRRD_B] = "iocsrrd.b",
    [OPC_IOCSRRD_H] = "iocsrrd.h",
    [OPC_IOCSRRD_W] = "iocsrrd.w",
    [OPC_IOCSRRD_D] = "iocsrrd.d",
    [OPC_IOCSRWR_B] = "iocsrwr.b",
    [OPC_IOCSRWR_H] = "iocsrwr.h",
    [OPC_IOCSRWR_W] = "iocsrwr.w",
    [OPC_IOCSRWR_D] = "iocsrwr.d",
    [OPC_TLBCLR] = "tlbclr",
    [OPC_TLBFLUSH] = "tlbflush",
    [OPC_TLBSRCH] = "tlbsrch",
    [OPC_TLBRD] = "tlbrd",
    [OPC_TLBWR] = "tlbwr",
    [OPC_TLBFILL] = "tlbfill",
    [OPC_ERTN] = "ertn",
    [OPC_IDLE] = "idle",
    [OPC_INVTLB] = "invtlb",
    [OPC_FMADD_S] = "fmadd.s",
    [OPC_FMADD_D] = "fmadd.d",
    [OPC_FMSUB_S] = "fmsub.s",
    [OPC_FMSUB_D] = "fmsub.d",
    [OPC_FNMADD_S] = "fnmadd.s",
    [OPC_FNMADD_D] = "fnmadd.d",
    [OPC_FNMSUB_S] = "fnmsub.s",
    [OPC_FNMSUB_D] = "fnmsub.d",
    [OPC_FCMP_CAF_S] = "fcmp.caf.s",
    [OPC_FCMP_SAF_S] = "fcmp.saf.s",
    [OPC_FCMP_CLT_S] = "fcmp.clt.s",
    [OPC_FCMP_STL_S] = "fcmp.stl.s",
    [OPC_FCMP_CEQ_S] = "fcmp.ceq.s",
    [OPC_FCMP_SEQ_S] = "fcmp.seq.s",
    [OPC_FCMP_CLE_S] = "fcmp.cle.s",
    [OPC_FCMP_SLE_S] = "fcmp.sle.s",
    [OPC_FCMP_CUN_S] = "fcmp.cun.s",
    [OPC_FCMP_SUN_S] = "fcmp.sun.s",
    [OPC_FCMP_CULT_S] = "fcmp.cult.s",
    [OPC_FCMP_SULT_S] = "fcmp.sult.s",
    [OPC_FCMP_CUEQ_S] = "fcmp.cueq.s",
    [OPC_FCMP_SUEQ_S] = "fcmp.sueq.s",
    [OPC_FCMP_CULE_S] = "fcmp.cule.s",
    [OPC_FCMP_SULE_S] = "fcmp.sule.s",
    [OPC_FCMP_CNE_S] = "fcmp.cne.s",
    [OPC_FCMP_SNE_S] = "fcmp.sne.s",
    [OPC_FCMP_COR_S] = "fcmp.cor.s",
    [OPC_FCMP_SOR_S] = "fcmp.sor.s",
    [OPC_FCMP_CUNE_S] = "fcmp.cune.s",
    [OPC_FCMP_SUNE_S] = "fcmp.sune.s",
    [OPC_FCMP_CAF_D] = "fcmp.caf.d",
    [OPC_FCMP_SAF_D] = "fcmp.saf.d",
    [OPC_FCMP_CLT_D] = "fcmp.clt.d",
    [OPC_FCMP_STL_D] = "fcmp.stl.d",
    [OPC_FCMP_CEQ_D] = "fcmp.ceq.d",
    [OPC_FCMP_SEQ_D] = "fcmp.seq.d",
    [OPC_FCMP_CLE_D] = "fcmp.cle.d",
    [OPC_FCMP_SLE_D] = "fcmp.sle.d",
    [OPC_FCMP_CUN_D] = "fcmp.cun.d",
    [OPC_FCMP_SUN_D] = "fcmp.sun.d",
    [OPC_FCMP_CULT_D] = "fcmp.cult.d",
    [OPC_FCMP_SULT_D] = "fcmp.sult.d",
    [OPC_FCMP_CUEQ_D] = "fcmp.cueq.d",
    [OPC_FCMP_SUEQ_D] = "fcmp.sueq.d",
    [OPC_FCMP_CULE_D] = "fcmp.cule.d",
    [OPC_FCMP_SULE_D] = "fcmp.sule.d",
    [OPC_FCMP_CNE_D] = "fcmp.cne.d",
    [OPC_FCMP_SNE_D] = "fcmp.sne.d",
    [OPC_FCMP_COR_D] = "fcmp.cor.d",
    [OPC_FCMP_SOR_D] = "fcmp.sor.d",
    [OPC_FCMP_CUNE_D] = "fcmp.cune.d",
    [OPC_FCMP_SUNE_D] = "fcmp.sune.d",
    [OPC_FSEL] = "fsel",
    [OPC_ADDU16I_D] = "addu16i.d",
    [OPC_LU12I_W] = "lu12i.w",
    [OPC_LU32I_D] = "lu32i.d",
    [OPC_PCADDI] = "pcaddi",
    [OPC_PCALAU12I] = "pcalau12i",
    [OPC_PCADDU12I] = "pcaddu12i",
    [OPC_PCADDU18I] = "pcaddu18i",
    [OPC_LL_W] = "ll.w",
    [OPC_SC_W] = "sc.w",
    [OPC_LL_D] = "ll.d",
    [OPC_SC_D] = "sc.d",
    [OPC_LDPTR_W] = "ldptr.w",
    [OPC_STPTR_W] = "stptr.w",
    [OPC_LDPTR_D] = "ldptr.d",
    [OPC_STPTR_D] = "stptr.d",
    [OPC_LD_B] = "ld.b",
    [OPC_LD_H] = "ld.h",
    [OPC_LD_W] = "ld.w",
    [OPC_LD_D] = "ld.d",
    [OPC_ST_B] = "st.b",
    [OPC_ST_H] = "st.h",
    [OPC_ST_W] = "st.w",
    [OPC_ST_D] = "st.d",
    [OPC_LD_BU] = "ld.bu",
    [OPC_LD_HU] = "ld.hu",
    [OPC_LD_WU] = "ld.wu",
    [OPC_PRELD] = "preld",
    [OPC_FLD_S] = "fld.s",
    [OPC_FST_S] = "fst.s",
    [OPC_FLD_D] = "fld.d",
    [OPC_FST_D] = "fst.d",
    [OPC_LDX_B] = "ldx.b",
    [OPC_LDX_H] = "ldx.h",
    [OPC_LDX_W] = "ldx.w",
    [OPC_LDX_D] = "ldx.d",
    [OPC_STX_B] = "stx.b",
    [OPC_STX_H] = "stx.h",
    [OPC_STX_W] = "stx.w",
    [OPC_STX_D] = "stx.d",
    [OPC_LDX_BU] = "ldx.bu",
    [OPC_LDX_HU] = "ldx.hu",
    [OPC_LDX_WU] = "ldx.wu",
    [OPC_PRELDX] = "preldx",
    [OPC_FLDX_S] = "fldx.s",
    [OPC_FLDX_D] = "fldx.d",
    [OPC_FSTX_S] = "fstx.s",
    [OPC_FSTX_D] = "fstx.d",
    [OPC_AMSWAP_W] = "amswap.w",
    [OPC_AMSWAP_D] = "amswap.d",
    [OPC_AMADD_W] = "amadd.w",
    [OPC_AMADD_D] = "amadd.d",
    [OPC_AMAND_W] = "amand.w",
    [OPC_AMAND_D] = "amand.d",
    [OPC_AMOR_W] = "amor.w",
    [OPC_AMOR_D] = "amor.d",
    [OPC_AMXOR_W] = "amxor.w",
    [OPC_AMXOR_D] = "amxor.d",
    [OPC_AMMAX_W] = "ammax.w",
    [OPC_AMMAX_D] = "ammax.d",
    [OPC_AMMIN_W] = "ammin.w",
    [OPC_AMMIN_D] = "ammin.d",
    [OPC_AMMAX_WU] = "ammax.wu",
    [OPC_AMMAX_DU] = "ammax.du",
    [OPC_AMMIN_WU] = "ammin.wu",
    [OPC_AMMIN_DU] = "ammin.du",
    [OPC_AMSWAP_DB_W] = "amswap.db.w",
    [OPC_AMSWAP_DB_D] = "amswap.db.d",
    [OPC_AMADD_DB_W] = "amadd.db.w",
    [OPC_AMADD_DB_D] = "amadd.db.d",
    [OPC_AMAND_DB_W] = "amand.db.w",
    [OPC_AMAND_DB_D] = "amand.db.d",
    [OPC_AMOR_DB_W] = "amor.db.w",
    [OPC_AMOR_DB_D] = "amor.db.d",
    [OPC_AMXOR_DB_W] = "amxor.db.w",
    [OPC_AMXOR_DB_D] = "amxor.db.d",
    [OPC_AMMAX_DB_W] = "ammax.db.w",
    [OPC_AMMAX_DB_D] = "ammax.db.d",
    [OPC_AMMIN_DB_W] = "ammin.db.w",
    [OPC_AMMIN_DB_D] = "ammin.db.d",
    [OPC_AMMAX_DB_WU] = "ammax.db.wu",
    [OPC_AMMAX_DB_DU] = "ammax.db.du",
    [OPC_AMMIN_DB_WU] = "ammin.db.wu",
    [OPC_AMMIN_DB_DU] = "ammin.db.du",
    [OPC_DBAR] = "dbar",
    [OPC_IBAR] = "ibar",
    [OPC_FLDGT_S] = "fldgt.s",
    [OPC_FLDGT_D] = "fldgt.d",
    [OPC_FLDLE_S] = "fldle.s",
    [OPC_FLDLE_D] = "fldle.d",
    [OPC_FSTGT_S] = "fstgt.s",
    [OPC_FSTGT_D] = "fstgt.d",
    [OPC_FSTLE_S] = "fstle.s",
    [OPC_FSTLE_D] = "fstle.d",
    [OPC_LDGT_B] = "ldgt.b",
    [OPC_LDGT_H] = "ldgt.h",
    [OPC_LDGT_W] = "ldgt.w",
    [OPC_LDGT_D] = "ldgt.d",
    [OPC_LDLE_B] = "ldle.b",
    [OPC_LDLE_H] = "ldle.h",
    [OPC_LDLE_W] = "ldle.w",
    [OPC_LDLE_D] = "ldle.d",
    [OPC_STGT_B] = "stgt.b",
    [OPC_STGT_H] = "stgt.h",
    [OPC_STGT_W] = "stgt.w",
    [OPC_STGT_D] = "stgt.d",
    [OPC_STLE_B] = "stle.b",
    [OPC_STLE_H] = "stle.h",
    [OPC_STLE_W] = "stle.w",
    [OPC_STLE_D] = "stle.d",
    [OPC_BEQZ] = "beqz",
    [OPC_BNEZ] = "bnez",
    [OPC_BCEQZ] = "bceqz",
    [OPC_BCNEZ] = "bcnez",
    [OPC_JIRL] = "jirl",
    [OPC_B] = "b",
    [OPC_BL] = "bl",
    [OPC_BEQ] = "beq",
    [OPC_BNE] = "bne",
    [OPC_BLT] = "blt",
    [OPC_BGE] = "bge",
    [OPC_BLTU] = "bltu",
    [OPC_BGEU] = "bgeu",
    [OPC_GR2SCR] = "gr2scr",
    [OPC_SRC2GR] = "src2gr",
    [OPC_X86MTTOP] = "x86mttop",
    [OPC_X86MFTOP] = "x86mftop",
    [OPC_SETX86LOOPE] = "setx86loope",
    [OPC_SETX86LOOPNE] = "setx86loopne",
    [OPC_X86INC_B] = "x86inc.b",
    [OPC_X86INC_H] = "x86inc.h",
    [OPC_X86INC_W] = "x86inc.w",
    [OPC_X86INC_D] = "x86inc.d",
    [OPC_X86DEC_B] = "x86dec.b",
    [OPC_X86DEC_H] = "x86dec.h",
    [OPC_X86DEC_W] = "x86dec.w",
    [OPC_X86DEC_D] = "x86dec.d",
    [OPC_X86SETTM] = "x86settm",
    [OPC_X86CLRTM] = "x86clrtm",
    [OPC_X86INCTOP] = "x86inctop",
    [OPC_X86DECTOP] = "x86dectop",
    [OPC_ROTR_B] = "rotr.b",
    [OPC_ROTR_H] = "rotr.h",
    [OPC_ADDU12I_W] = "addu12i.w",
    [OPC_ADDU12I_D] = "addu12i.d",
    [OPC_HVCL] = "hvcl",
    [OPC_ADC_B] = "adc.b",
    [OPC_ADC_H] = "adc.h",
    [OPC_ADC_W] = "adc.w",
    [OPC_ADC_D] = "adc.d",
    [OPC_SBC_B] = "sbc.b",
    [OPC_SBC_H] = "sbc.h",
    [OPC_SBC_W] = "sbc.w",
    [OPC_SBC_D] = "sbc.d",
    [OPC_RCR_B] = "rcr.b",
    [OPC_RCR_H] = "rcr.h",
    [OPC_RCR_W] = "rcr.w",
    [OPC_RCR_D] = "rcr.d",
    [OPC_ARMMOVE] = "armmove",
    [OPC_SETX86J] = "setx86j",
    [OPC_SETARMJ] = "setarmj",
    [OPC_ARMADD_W] = "armadd.w",
    [OPC_ARMSUB_W] = "armsub.w",
    [OPC_ARMADC_W] = "armadc.w",
    [OPC_ARMSBC_W] = "armsbc.w",
    [OPC_ARMAND_W] = "armand.w",
    [OPC_ARMOR_W] = "armor.w",
    [OPC_ARMXOR_W] = "armxor.w",
    [OPC_ARMSLL_W] = "armsll.w",
    [OPC_ARMSRL_W] = "armsrl.w",
    [OPC_ARMSRA_W] = "armsra.w",
    [OPC_ARMROTR_W] = "armrotr.w",
    [OPC_ARMSLLI_W] = "armslli.w",
    [OPC_ARMSRLI_W] = "armsrli.w",
    [OPC_ARMSRAI_W] = "armsrai.w",
    [OPC_ARMROTRI_W] = "armrotri.w",
    [OPC_X86MUL_B] = "x86mul.b",
    [OPC_X86MUL_H] = "x86mul.h",
    [OPC_X86MUL_W] = "x86mul.w",
    [OPC_X86MUL_D] = "x86mul.d",
    [OPC_X86MUL_BU] = "x86mul.bu",
    [OPC_X86MUL_HU] = "x86mul.hu",
    [OPC_X86MUL_WU] = "x86mul.wu",
    [OPC_X86MUL_DU] = "x86mul.du",
    [OPC_X86ADD_WU] = "x86add.wu",
    [OPC_X86ADD_DU] = "x86add.du",
    [OPC_X86SUB_WU] = "x86sub.wu",
    [OPC_X86SUB_DU] = "x86sub.du",
    [OPC_X86ADD_B] = "x86add.b",
    [OPC_X86ADD_H] = "x86add.h",
    [OPC_X86ADD_W] = "x86add.w",
    [OPC_X86ADD_D] = "x86add.d",
    [OPC_X86SUB_B] = "x86sub.b",
    [OPC_X86SUB_H] = "x86sub.h",
    [OPC_X86SUB_W] = "x86sub.w",
    [OPC_X86SUB_D] = "x86sub.d",
    [OPC_X86ADC_B] = "x86adc.b",
    [OPC_X86ADC_H] = "x86adc.h",
    [OPC_X86ADC_W] = "x86adc.w",
    [OPC_X86ADC_D] = "x86adc.d",
    [OPC_X86SBC_B] = "x86sbc.b",
    [OPC_X86SBC_H] = "x86sbc.h",
    [OPC_X86SBC_W] = "x86sbc.w",
    [OPC_X86SBC_D] = "x86sbc.d",
    [OPC_X86SLL_B] = "x86sll.b",
    [OPC_X86SLL_H] = "x86sll.h",
    [OPC_X86SLL_W] = "x86sll.w",
    [OPC_X86SLL_D] = "x86sll.d",
    [OPC_X86SRL_B] = "x86srl.b",
    [OPC_X86SRL_H] = "x86srl.h",
    [OPC_X86SRL_W] = "x86srl.w",
    [OPC_X86SRL_D] = "x86srl.d",
    [OPC_X86SRA_B] = "x86sra.b",
    [OPC_X86SRA_H] = "x86sra.h",
    [OPC_X86SRA_W] = "x86sra.w",
    [OPC_X86SRA_D] = "x86sra.d",
    [OPC_X86ROTR_B] = "x86rotr.b",
    [OPC_X86ROTR_H] = "x86rotr.h",
    [OPC_X86ROTR_D] = "x86rotr.d",
    [OPC_X86ROTR_W] = "x86rotr.w",
    [OPC_X86ROTL_B] = "x86rotl.b",
    [OPC_X86ROTL_H] = "x86rotl.h",
    [OPC_X86ROTL_W] = "x86rotl.w",
    [OPC_X86ROTL_D] = "x86rotl.d",
    [OPC_X86RCR_B] = "x86rcr.b",
    [OPC_X86RCR_H] = "x86rcr.h",
    [OPC_X86RCR_W] = "x86rcr.w",
    [OPC_X86RCR_D] = "x86rcr.d",
    [OPC_X86RCL_B] = "x86rcl.b",
    [OPC_X86RCL_H] = "x86rcl.h",
    [OPC_X86RCL_W] = "x86rcl.w",
    [OPC_X86RCL_D] = "x86rcl.d",
    [OPC_X86AND_B] = "x86and.b",
    [OPC_X86AND_H] = "x86and.h",
    [OPC_X86AND_W] = "x86and.w",
    [OPC_X86AND_D] = "x86and.d",
    [OPC_X86OR_B] = "x86or.b",
    [OPC_X86OR_H] = "x86or.h",
    [OPC_X86OR_W] = "x86or.w",
    [OPC_X86OR_D] = "x86or.d",
    [OPC_X86XOR_B] = "x86xor.b",
    [OPC_X86XOR_H] = "x86xor.h",
    [OPC_X86XOR_W] = "x86xor.w",
    [OPC_X86XOR_D] = "x86xor.d",
    [OPC_ARMNOT_W] = "armnot.w",
    [OPC_ARMMOV_W] = "armmov.w",
    [OPC_ARMMOV_D] = "armmov.d",
    [OPC_ARMRRX_W] = "armrrx.w",
    [OPC_ROTRI_B] = "rotri.b",
    [OPC_ROTRI_H] = "rotri.h",
    [OPC_RCRI_B] = "rcri.b",
    [OPC_RCRI_H] = "rcri.h",
    [OPC_RCRI_W] = "rcri.w",
    [OPC_RCRI_D] = "rcri.d",
    [OPC_X86SLLI_B] = "x86slli.b",
    [OPC_X86SLLI_H] = "x86slli.h",
    [OPC_X86SLLI_W] = "x86slli.w",
    [OPC_X86SLLI_D] = "x86slli.d",
    [OPC_X86SRLI_B] = "x86srli.b",
    [OPC_X86SRLI_H] = "x86srli.h",
    [OPC_X86SRLI_W] = "x86srli.w",
    [OPC_X86SRLI_D] = "x86srli.d",
    [OPC_X86SRAI_B] = "x86srai.b",
    [OPC_X86SRAI_H] = "x86srai.h",
    [OPC_X86SRAI_W] = "x86srai.w",
    [OPC_X86SRAI_D] = "x86srai.d",
    [OPC_X86ROTRI_B] = "x86rotri.b",
    [OPC_X86ROTRI_H] = "x86rotri.h",
    [OPC_X86ROTRI_W] = "x86rotri.w",
    [OPC_X86ROTRI_D] = "x86rotri.d",
    [OPC_X86RCRI_B] = "x86rcri.b",
    [OPC_X86RCRI_H] = "x86rcri.h",
    [OPC_X86RCRI_W] = "x86rcri.w",
    [OPC_X86RCRI_D] = "x86rcri.d",
    [OPC_X86ROTLI_B] = "x86rotli.b",
    [OPC_X86ROTLI_H] = "x86rotli.h",
    [OPC_X86ROTLI_W] = "x86rotli.w",
    [OPC_X86ROTLI_D] = "x86rotli.d",
    [OPC_X86RCLI_B] = "x86rcli.b",
    [OPC_X86RCLI_H] = "x86rcli.h",
    [OPC_X86RCLI_W] = "x86rcli.w",
    [OPC_X86RCLI_D] = "x86rcli.d",
    [OPC_X86SETTAG] = "x86settag",
    [OPC_X86MFFLAG] = "x86mfflag",
    [OPC_X86MTFLAG] = "x86mtflag",
    [OPC_ARMMFFLAG] = "armmfflag",
    [OPC_ARMMTFLAG] = "armmtflag",
    [OPC_FCVT_LD_D] = "fcvt.ld.d",
    [OPC_FCVT_UD_D] = "fcvt.ud.d",
    [OPC_FCVT_D_LC] = "fcvt.d.lc",
    [OPC_GCSRRD] = "gcsrrd",
    [OPC_GCSRWR] = "gcsrwr",
    [OPC_GCSRXCHG] = "gcsrxchg",
    [OPC_GTLBCLR] = "gtlbclr",
    [OPC_GTLBFLUSH] = "gtlbflush",
    [OPC_GTLBSRCH] = "gtlbsrch",
    [OPC_GTLBRD] = "gtlbrd",
    [OPC_GTLBWR] = "gtlbwr",
    [OPC_GTLBFILL] = "gtlbfill",
    [OPC_SCFGR_W] = "scfgr.w",
    [OPC_SCFGR_D] = "scfgr.d",
    [OPC_SCFGW_W] = "scfgw.w",
    [OPC_SCFGW_D] = "scfgw.d",
    [OPC_SNOP] = "snop",
    [OPC_LDL_W] = "ldl.w",
    [OPC_LDR_W] = "ldr.w",
    [OPC_LDL_D] = "ldl.d",
    [OPC_LDR_D] = "ldr.d",
    [OPC_STL_W] = "stl.w",
    [OPC_STR_W] = "str.w",
    [OPC_STL_D] = "stl.d",
    [OPC_STR_D] = "str.d",
    [OPC_JISCR0] = "jiscr0",
    [OPC_JISCR1] = "jiscr1",
    [OPC_VADD_B] = "vadd.b",
    [OPC_VADD_H] = "vadd.h",
    [OPC_VADD_W] = "vadd.w",
    [OPC_VADD_D] = "vadd.d",
    [OPC_VADD_Q] = "vadd.q",
    [OPC_VSUB_B] = "vsub.b",
    [OPC_VSUB_H] = "vsub.h",
    [OPC_VSUB_W] = "vsub.w",
    [OPC_VSUB_D] = "vsub.d",
    [OPC_VSUB_Q] = "vsub.q",
    [OPC_VADDI_BU] = "vaddi.bu",
    [OPC_VADDI_HU] = "vaddi.hu",
    [OPC_VADDI_WU] = "vaddi.wu",
    [OPC_VADDI_DU] = "vaddi.du",
    [OPC_VSUBI_BU] = "vsubi.bu",
    [OPC_VSUBI_HU] = "vsubi.hu",
    [OPC_VSUBI_WU] = "vsubi.wu",
    [OPC_VSUBI_DU] = "vsubi.du",
    [OPC_VNEG_B] = "vneg.b",
    [OPC_VNEG_H] = "vneg.h",
    [OPC_VNEG_W] = "vneg.w",
    [OPC_VNEG_D] = "vneg.d",
    [OPC_VSADD_B] = "vsadd.b",
    [OPC_VSADD_H] = "vsadd.h",
    [OPC_VSADD_W] = "vsadd.w",
    [OPC_VSADD_D] = "vsadd.d",
    [OPC_VSADD_BU] = "vsadd.bu",
    [OPC_VSADD_HU] = "vsadd.hu",
    [OPC_VSADD_WU] = "vsadd.wu",
    [OPC_VSADD_DU] = "vsadd.du",
    [OPC_VSSUB_B] = "vssub.b",
    [OPC_VSSUB_H] = "vssub.h",
    [OPC_VSSUB_W] = "vssub.w",
    [OPC_VSSUB_D] = "vssub.d",
    [OPC_VSSUB_BU] = "vssub.bu",
    [OPC_VSSUB_HU] = "vssub.hu",
    [OPC_VSSUB_WU] = "vssub.wu",
    [OPC_VSSUB_DU] = "vssub.du",
    [OPC_VHADDW_H_B] = "vhaddw.h.b",
    [OPC_VHADDW_W_H] = "vhaddw.w.h",
    [OPC_VHADDW_D_W] = "vhaddw.d.w",
    [OPC_VHADDW_Q_D] = "vhaddw.q.d",
    [OPC_VHADDW_HU_BU] = "vhaddw.hu.bu",
    [OPC_VHADDW_WU_HU] = "vhaddw.wu.hu",
    [OPC_VHADDW_DU_WU] = "vhaddw.du.wu",
    [OPC_VHADDW_QU_DU] = "vhaddw.qu.du",
    [OPC_VHSUBW_H_B] = "vhsubw.h.b",
    [OPC_VHSUBW_W_H] = "vhsubw.w.h",
    [OPC_VHSUBW_D_W] = "vhsubw.d.w",
    [OPC_VHSUBW_Q_D] = "vhsubw.q.d",
    [OPC_VHSUBW_HU_BU] = "vhsubw.hu.bu",
    [OPC_VHSUBW_WU_HU] = "vhsubw.wu.hu",
    [OPC_VHSUBW_DU_WU] = "vhsubw.du.wu",
    [OPC_VHSUBW_QU_DU] = "vhsubw.qu.du",
    [OPC_VADDWEV_H_B] = "vaddwev.h.b",
    [OPC_VADDWEV_W_H] = "vaddwev.w.h",
    [OPC_VADDWEV_D_W] = "vaddwev.d.w",
    [OPC_VADDWEV_Q_D] = "vaddwev.q.d",
    [OPC_VADDWOD_H_B] = "vaddwod.h.b",
    [OPC_VADDWOD_W_H] = "vaddwod.w.h",
    [OPC_VADDWOD_D_W] = "vaddwod.d.w",
    [OPC_VADDWOD_Q_D] = "vaddwod.q.d",
    [OPC_VSUBWEV_H_B] = "vsubwev.h.b",
    [OPC_VSUBWEV_W_H] = "vsubwev.w.h",
    [OPC_VSUBWEV_D_W] = "vsubwev.d.w",
    [OPC_VSUBWEV_Q_D] = "vsubwev.q.d",
    [OPC_VSUBWOD_H_B] = "vsubwod.h.b",
    [OPC_VSUBWOD_W_H] = "vsubwod.w.h",
    [OPC_VSUBWOD_D_W] = "vsubwod.d.w",
    [OPC_VSUBWOD_Q_D] = "vsubwod.q.d",
    [OPC_VADDWEV_H_BU] = "vaddwev.h.bu",
    [OPC_VADDWEV_W_HU] = "vaddwev.w.hu",
    [OPC_VADDWEV_D_WU] = "vaddwev.d.wu",
    [OPC_VADDWEV_Q_DU] = "vaddwev.q.du",
    [OPC_VADDWOD_H_BU] = "vaddwod.h.bu",
    [OPC_VADDWOD_W_HU] = "vaddwod.w.hu",
    [OPC_VADDWOD_D_WU] = "vaddwod.d.wu",
    [OPC_VADDWOD_Q_DU] = "vaddwod.q.du",
    [OPC_VSUBWEV_H_BU] = "vsubwev.h.bu",
    [OPC_VSUBWEV_W_HU] = "vsubwev.w.hu",
    [OPC_VSUBWEV_D_WU] = "vsubwev.d.wu",
    [OPC_VSUBWEV_Q_DU] = "vsubwev.q.du",
    [OPC_VSUBWOD_H_BU] = "vsubwod.h.bu",
    [OPC_VSUBWOD_W_HU] = "vsubwod.w.hu",
    [OPC_VSUBWOD_D_WU] = "vsubwod.d.wu",
    [OPC_VSUBWOD_Q_DU] = "vsubwod.q.du",
    [OPC_VADDWEV_H_BU_B] = "vaddwev.h.bu.b",
    [OPC_VADDWEV_W_HU_H] = "vaddwev.w.hu.h",
    [OPC_VADDWEV_D_WU_W] = "vaddwev.d.wu.w",
    [OPC_VADDWEV_Q_DU_D] = "vaddwev.q.du.d",
    [OPC_VADDWOD_H_BU_B] = "vaddwod.h.bu.b",
    [OPC_VADDWOD_W_HU_H] = "vaddwod.w.hu.h",
    [OPC_VADDWOD_D_WU_W] = "vaddwod.d.wu.w",
    [OPC_VADDWOD_Q_DU_D] = "vaddwod.q.du.d",
    [OPC_VAVG_B] = "vavg.b",
    [OPC_VAVG_H] = "vavg.h",
    [OPC_VAVG_W] = "vavg.w",
    [OPC_VAVG_D] = "vavg.d",
    [OPC_VAVG_BU] = "vavg.bu",
    [OPC_VAVG_HU] = "vavg.hu",
    [OPC_VAVG_WU] = "vavg.wu",
    [OPC_VAVG_DU] = "vavg.du",
    [OPC_VAVGR_B] = "vavgr.b",
    [OPC_VAVGR_H] = "vavgr.h",
    [OPC_VAVGR_W] = "vavgr.w",
    [OPC_VAVGR_D] = "vavgr.d",
    [OPC_VAVGR_BU] = "vavgr.bu",
    [OPC_VAVGR_HU] = "vavgr.hu",
    [OPC_VAVGR_WU] = "vavgr.wu",
    [OPC_VAVGR_DU] = "vavgr.du",
    [OPC_VABSD_B] = "vabsd.b",
    [OPC_VABSD_H] = "vabsd.h",
    [OPC_VABSD_W] = "vabsd.w",
    [OPC_VABSD_D] = "vabsd.d",
    [OPC_VABSD_BU] = "vabsd.bu",
    [OPC_VABSD_HU] = "vabsd.hu",
    [OPC_VABSD_WU] = "vabsd.wu",
    [OPC_VABSD_DU] = "vabsd.du",
    [OPC_VADDA_B] = "vadda.b",
    [OPC_VADDA_H] = "vadda.h",
    [OPC_VADDA_W] = "vadda.w",
    [OPC_VADDA_D] = "vadda.d",
    [OPC_VMAX_B] = "vmax.b",
    [OPC_VMAX_H] = "vmax.h",
    [OPC_VMAX_W] = "vmax.w",
    [OPC_VMAX_D] = "vmax.d",
    [OPC_VMAXI_B] = "vmaxi.b",
    [OPC_VMAXI_H] = "vmaxi.h",
    [OPC_VMAXI_W] = "vmaxi.w",
    [OPC_VMAXI_D] = "vmaxi.d",
    [OPC_VMAX_BU] = "vmax.bu",
    [OPC_VMAX_HU] = "vmax.hu",
    [OPC_VMAX_WU] = "vmax.wu",
    [OPC_VMAX_DU] = "vmax.du",
    [OPC_VMAXI_BU] = "vmaxi.bu",
    [OPC_VMAXI_HU] = "vmaxi.hu",
    [OPC_VMAXI_WU] = "vmaxi.wu",
    [OPC_VMAXI_DU] = "vmaxi.du",
    [OPC_VMIN_B] = "vmin.b",
    [OPC_VMIN_H] = "vmin.h",
    [OPC_VMIN_W] = "vmin.w",
    [OPC_VMIN_D] = "vmin.d",
    [OPC_VMINI_B] = "vmini.b",
    [OPC_VMINI_H] = "vmini.h",
    [OPC_VMINI_W] = "vmini.w",
    [OPC_VMINI_D] = "vmini.d",
    [OPC_VMIN_BU] = "vmin.bu",
    [OPC_VMIN_HU] = "vmin.hu",
    [OPC_VMIN_WU] = "vmin.wu",
    [OPC_VMIN_DU] = "vmin.du",
    [OPC_VMINI_BU] = "vmini.bu",
    [OPC_VMINI_HU] = "vmini.hu",
    [OPC_VMINI_WU] = "vmini.wu",
    [OPC_VMINI_DU] = "vmini.du",
    [OPC_VMUL_B] = "vmul.b",
    [OPC_VMUL_H] = "vmul.h",
    [OPC_VMUL_W] = "vmul.w",
    [OPC_VMUL_D] = "vmul.d",
    [OPC_VMUH_B] = "vmuh.b",
    [OPC_VMUH_H] = "vmuh.h",
    [OPC_VMUH_W] = "vmuh.w",
    [OPC_VMUH_D] = "vmuh.d",
    [OPC_VMUH_BU] = "vmuh.bu",
    [OPC_VMUH_HU] = "vmuh.hu",
    [OPC_VMUH_WU] = "vmuh.wu",
    [OPC_VMUH_DU] = "vmuh.du",
    [OPC_VMULWEV_H_B] = "vmulwev.h.b",
    [OPC_VMULWEV_W_H] = "vmulwev.w.h",
    [OPC_VMULWEV_D_W] = "vmulwev.d.w",
    [OPC_VMULWEV_Q_D] = "vmulwev.q.d",
    [OPC_VMULWOD_H_B] = "vmulwod.h.b",
    [OPC_VMULWOD_W_H] = "vmulwod.w.h",
    [OPC_VMULWOD_D_W] = "vmulwod.d.w",
    [OPC_VMULWOD_Q_D] = "vmulwod.q.d",
    [OPC_VMULWEV_H_BU] = "vmulwev.h.bu",
    [OPC_VMULWEV_W_HU] = "vmulwev.w.hu",
    [OPC_VMULWEV_D_WU] = "vmulwev.d.wu",
    [OPC_VMULWEV_Q_DU] = "vmulwev.q.du",
    [OPC_VMULWOD_H_BU] = "vmulwod.h.bu",
    [OPC_VMULWOD_W_HU] = "vmulwod.w.hu",
    [OPC_VMULWOD_D_WU] = "vmulwod.d.wu",
    [OPC_VMULWOD_Q_DU] = "vmulwod.q.du",
    [OPC_VMULWEV_H_BU_B] = "vmulwev.h.bu.b",
    [OPC_VMULWEV_W_HU_H] = "vmulwev.w.hu.h",
    [OPC_VMULWEV_D_WU_W] = "vmulwev.d.wu.w",
    [OPC_VMULWEV_Q_DU_D] = "vmulwev.q.du.d",
    [OPC_VMULWOD_H_BU_B] = "vmulwod.h.bu.b",
    [OPC_VMULWOD_W_HU_H] = "vmulwod.w.hu.h",
    [OPC_VMULWOD_D_WU_W] = "vmulwod.d.wu.w",
    [OPC_VMULWOD_Q_DU_D] = "vmulwod.q.du.d",
    [OPC_VMADD_B] = "vmadd.b",
    [OPC_VMADD_H] = "vmadd.h",
    [OPC_VMADD_W] = "vmadd.w",
    [OPC_VMADD_D] = "vmadd.d",
    [OPC_VMSUB_B] = "vmsub.b",
    [OPC_VMSUB_H] = "vmsub.h",
    [OPC_VMSUB_W] = "vmsub.w",
    [OPC_VMSUB_D] = "vmsub.d",
    [OPC_VMADDWEV_H_B] = "vmaddwev.h.b",
    [OPC_VMADDWEV_W_H] = "vmaddwev.w.h",
    [OPC_VMADDWEV_D_W] = "vmaddwev.d.w",
    [OPC_VMADDWEV_Q_D] = "vmaddwev.q.d",
    [OPC_VMADDWOD_H_B] = "vmaddwod.h.b",
    [OPC_VMADDWOD_W_H] = "vmaddwod.w.h",
    [OPC_VMADDWOD_D_W] = "vmaddwod.d.w",
    [OPC_VMADDWOD_Q_D] = "vmaddwod.q.d",
    [OPC_VMADDWEV_H_BU] = "vmaddwev.h.bu",
    [OPC_VMADDWEV_W_HU] = "vmaddwev.w.hu",
    [OPC_VMADDWEV_D_WU] = "vmaddwev.d.wu",
    [OPC_VMADDWEV_Q_DU] = "vmaddwev.q.du",
    [OPC_VMADDWOD_H_BU] = "vmaddwod.h.bu",
    [OPC_VMADDWOD_W_HU] = "vmaddwod.w.hu",
    [OPC_VMADDWOD_D_WU] = "vmaddwod.d.wu",
    [OPC_VMADDWOD_Q_DU] = "vmaddwod.q.du",
    [OPC_VMADDWEV_H_BU_B] = "vmaddwev.h.bu.b",
    [OPC_VMADDWEV_W_HU_H] = "vmaddwev.w.hu.h",
    [OPC_VMADDWEV_D_WU_W] = "vmaddwev.d.wu.w",
    [OPC_VMADDWEV_Q_DU_D] = "vmaddwev.q.du.d",
    [OPC_VMADDWOD_H_BU_B] = "vmaddwod.h.bu.b",
    [OPC_VMADDWOD_W_HU_H] = "vmaddwod.w.hu.h",
    [OPC_VMADDWOD_D_WU_W] = "vmaddwod.d.wu.w",
    [OPC_VMADDWOD_Q_DU_D] = "vmaddwod.q.du.d",
    [OPC_VDIV_B] = "vdiv.b",
    [OPC_VDIV_H] = "vdiv.h",
    [OPC_VDIV_W] = "vdiv.w",
    [OPC_VDIV_D] = "vdiv.d",
    [OPC_VDIV_BU] = "vdiv.bu",
    [OPC_VDIV_HU] = "vdiv.hu",
    [OPC_VDIV_WU] = "vdiv.wu",
    [OPC_VDIV_DU] = "vdiv.du",
    [OPC_VMOD_B] = "vmod.b",
    [OPC_VMOD_H] = "vmod.h",
    [OPC_VMOD_W] = "vmod.w",
    [OPC_VMOD_D] = "vmod.d",
    [OPC_VMOD_BU] = "vmod.bu",
    [OPC_VMOD_HU] = "vmod.hu",
    [OPC_VMOD_WU] = "vmod.wu",
    [OPC_VMOD_DU] = "vmod.du",
    [OPC_VSAT_B] = "vsat.b",
    [OPC_VSAT_H] = "vsat.h",
    [OPC_VSAT_W] = "vsat.w",
    [OPC_VSAT_D] = "vsat.d",
    [OPC_VSAT_BU] = "vsat.bu",
    [OPC_VSAT_HU] = "vsat.hu",
    [OPC_VSAT_WU] = "vsat.wu",
    [OPC_VSAT_DU] = "vsat.du",
    [OPC_VEXTH_H_B] = "vexth.h.b",
    [OPC_VEXTH_W_H] = "vexth.w.h",
    [OPC_VEXTH_D_W] = "vexth.d.w",
    [OPC_VEXTH_Q_D] = "vexth.q.d",
    [OPC_VEXTH_HU_BU] = "vexth.hu.bu",
    [OPC_VEXTH_WU_HU] = "vexth.wu.hu",
    [OPC_VEXTH_DU_WU] = "vexth.du.wu",
    [OPC_VEXTH_QU_DU] = "vexth.qu.du",
    [OPC_VSIGNCOV_B] = "vsigncov.b",
    [OPC_VSIGNCOV_H] = "vsigncov.h",
    [OPC_VSIGNCOV_W] = "vsigncov.w",
    [OPC_VSIGNCOV_D] = "vsigncov.d",
    [OPC_VMSKLTZ_B] = "vmskltz.b",
    [OPC_VMSKLTZ_H] = "vmskltz.h",
    [OPC_VMSKLTZ_W] = "vmskltz.w",
    [OPC_VMSKLTZ_D] = "vmskltz.d",
    [OPC_VMSKGEZ_B] = "vmskgez.b",
    [OPC_VMSKNZ_B] = "vmsknz.b",
    [OPC_VLDI] = "vldi",
    [OPC_VAND_V] = "vand.v",
    [OPC_VOR_V] = "vor.v",
    [OPC_VXOR_V] = "vxor.v",
    [OPC_VNOR_V] = "vnor.v",
    [OPC_VANDN_V] = "vandn.v",
    [OPC_VORN_V] = "vorn.v",
    [OPC_VANDI_B] = "vandi.b",
    [OPC_VORI_B] = "vori.b",
    [OPC_VXORI_B] = "vxori.b",
    [OPC_VNORI_B] = "vnori.b",
    [OPC_VSLL_B] = "vsll.b",
    [OPC_VSLL_H] = "vsll.h",
    [OPC_VSLL_W] = "vsll.w",
    [OPC_VSLL_D] = "vsll.d",
    [OPC_VSLLI_B] = "vslli.b",
    [OPC_VSLLI_H] = "vslli.h",
    [OPC_VSLLI_W] = "vslli.w",
    [OPC_VSLLI_D] = "vslli.d",
    [OPC_VSRL_B] = "vsrl.b",
    [OPC_VSRL_H] = "vsrl.h",
    [OPC_VSRL_W] = "vsrl.w",
    [OPC_VSRL_D] = "vsrl.d",
    [OPC_VSRLI_B] = "vsrli.b",
    [OPC_VSRLI_H] = "vsrli.h",
    [OPC_VSRLI_W] = "vsrli.w",
    [OPC_VSRLI_D] = "vsrli.d",
    [OPC_VSRA_B] = "vsra.b",
    [OPC_VSRA_H] = "vsra.h",
    [OPC_VSRA_W] = "vsra.w",
    [OPC_VSRA_D] = "vsra.d",
    [OPC_VSRAI_B] = "vsrai.b",
    [OPC_VSRAI_H] = "vsrai.h",
    [OPC_VSRAI_W] = "vsrai.w",
    [OPC_VSRAI_D] = "vsrai.d",
    [OPC_VROTR_B] = "vrotr.b",
    [OPC_VROTR_H] = "vrotr.h",
    [OPC_VROTR_W] = "vrotr.w",
    [OPC_VROTR_D] = "vrotr.d",
    [OPC_VROTRI_B] = "vrotri.b",
    [OPC_VROTRI_H] = "vrotri.h",
    [OPC_VROTRI_W] = "vrotri.w",
    [OPC_VROTRI_D] = "vrotri.d",
    [OPC_VSLLWIL_H_B] = "vsllwil.h.b",
    [OPC_VSLLWIL_W_H] = "vsllwil.w.h",
    [OPC_VSLLWIL_D_W] = "vsllwil.d.w",
    [OPC_VEXTL_Q_D] = "vextl.q.d",
    [OPC_VSLLWIL_HU_BU] = "vsllwil.hu.bu",
    [OPC_VSLLWIL_WU_HU] = "vsllwil.wu.hu",
    [OPC_VSLLWIL_DU_WU] = "vsllwil.du.wu",
    [OPC_VEXTL_QU_DU] = "vextl.qu.du",
    [OPC_VSRLR_B] = "vsrlr.b",
    [OPC_VSRLR_H] = "vsrlr.h",
    [OPC_VSRLR_W] = "vsrlr.w",
    [OPC_VSRLR_D] = "vsrlr.d",
    [OPC_VSRLRI_B] = "vsrlri.b",
    [OPC_VSRLRI_H] = "vsrlri.h",
    [OPC_VSRLRI_W] = "vsrlri.w",
    [OPC_VSRLRI_D] = "vsrlri.d",
    [OPC_VSRAR_B] = "vsrar.b",
    [OPC_VSRAR_H] = "vsrar.h",
    [OPC_VSRAR_W] = "vsrar.w",
    [OPC_VSRAR_D] = "vsrar.d",
    [OPC_VSRARI_B] = "vsrari.b",
    [OPC_VSRARI_H] = "vsrari.h",
    [OPC_VSRARI_W] = "vsrari.w",
    [OPC_VSRARI_D] = "vsrari.d",
    [OPC_VSRLN_B_H] = "vsrln.b.h",
    [OPC_VSRLN_H_W] = "vsrln.h.w",
    [OPC_VSRLN_W_D] = "vsrln.w.d",
    [OPC_VSRAN_B_H] = "vsran.b.h",
    [OPC_VSRAN_H_W] = "vsran.h.w",
    [OPC_VSRAN_W_D] = "vsran.w.d",
    [OPC_VSRLNI_B_H] = "vsrlni.b.h",
    [OPC_VSRLNI_H_W] = "vsrlni.h.w",
    [OPC_VSRLNI_W_D] = "vsrlni.w.d",
    [OPC_VSRLNI_D_Q] = "vsrlni.d.q",
    [OPC_VSRANI_B_H] = "vsrani.b.h",
    [OPC_VSRANI_H_W] = "vsrani.h.w",
    [OPC_VSRANI_W_D] = "vsrani.w.d",
    [OPC_VSRANI_D_Q] = "vsrani.d.q",
    [OPC_VSRLRN_B_H] = "vsrlrn.b.h",
    [OPC_VSRLRN_H_W] = "vsrlrn.h.w",
    [OPC_VSRLRN_W_D] = "vsrlrn.w.d",
    [OPC_VSRARN_B_H] = "vsrarn.b.h",
    [OPC_VSRARN_H_W] = "vsrarn.h.w",
    [OPC_VSRARN_W_D] = "vsrarn.w.d",
    [OPC_VSRLRNI_B_H] = "vsrlrni.b.h",
    [OPC_VSRLRNI_H_W] = "vsrlrni.h.w",
    [OPC_VSRLRNI_W_D] = "vsrlrni.w.d",
    [OPC_VSRLRNI_D_Q] = "vsrlrni.d.q",
    [OPC_VSRARNI_B_H] = "vsrarni.b.h",
    [OPC_VSRARNI_H_W] = "vsrarni.h.w",
    [OPC_VSRARNI_W_D] = "vsrarni.w.d",
    [OPC_VSRARNI_D_Q] = "vsrarni.d.q",
    [OPC_VSSRLN_B_H] = "vssrln.b.h",
    [OPC_VSSRLN_H_W] = "vssrln.h.w",
    [OPC_VSSRLN_W_D] = "vssrln.w.d",
    [OPC_VSSRAN_B_H] = "vssran.b.h",
    [OPC_VSSRAN_H_W] = "vssran.h.w",
    [OPC_VSSRAN_W_D] = "vssran.w.d",
    [OPC_VSSRLN_BU_H] = "vssrln.bu.h",
    [OPC_VSSRLN_HU_W] = "vssrln.hu.w",
    [OPC_VSSRLN_WU_D] = "vssrln.wu.d",
    [OPC_VSSRAN_BU_H] = "vssran.bu.h",
    [OPC_VSSRAN_HU_W] = "vssran.hu.w",
    [OPC_VSSRAN_WU_D] = "vssran.wu.d",
    [OPC_VSSRLNI_B_H] = "vssrlni.b.h",
    [OPC_VSSRLNI_H_W] = "vssrlni.h.w",
    [OPC_VSSRLNI_W_D] = "vssrlni.w.d",
    [OPC_VSSRLNI_D_Q] = "vssrlni.d.q",
    [OPC_VSSRANI_B_H] = "vssrani.b.h",
    [OPC_VSSRANI_H_W] = "vssrani.h.w",
    [OPC_VSSRANI_W_D] = "vssrani.w.d",
    [OPC_VSSRANI_D_Q] = "vssrani.d.q",
    [OPC_VSSRLNI_BU_H] = "vssrlni.bu.h",
    [OPC_VSSRLNI_HU_W] = "vssrlni.hu.w",
    [OPC_VSSRLNI_WU_D] = "vssrlni.wu.d",
    [OPC_VSSRLNI_DU_Q] = "vssrlni.du.q",
    [OPC_VSSRANI_BU_H] = "vssrani.bu.h",
    [OPC_VSSRANI_HU_W] = "vssrani.hu.w",
    [OPC_VSSRANI_WU_D] = "vssrani.wu.d",
    [OPC_VSSRANI_DU_Q] = "vssrani.du.q",
    [OPC_VSSRLRN_B_H] = "vssrlrn.b.h",
    [OPC_VSSRLRN_H_W] = "vssrlrn.h.w",
    [OPC_VSSRLRN_W_D] = "vssrlrn.w.d",
    [OPC_VSSRARN_B_H] = "vssrarn.b.h",
    [OPC_VSSRARN_H_W] = "vssrarn.h.w",
    [OPC_VSSRARN_W_D] = "vssrarn.w.d",
    [OPC_VSSRLRN_BU_H] = "vssrlrn.bu.h",
    [OPC_VSSRLRN_HU_W] = "vssrlrn.hu.w",
    [OPC_VSSRLRN_WU_D] = "vssrlrn.wu.d",
    [OPC_VSSRARN_BU_H] = "vssrarn.bu.h",
    [OPC_VSSRARN_HU_W] = "vssrarn.hu.w",
    [OPC_VSSRARN_WU_D] = "vssrarn.wu.d",
    [OPC_VSSRLRNI_B_H] = "vssrlrni.b.h",
    [OPC_VSSRLRNI_H_W] = "vssrlrni.h.w",
    [OPC_VSSRLRNI_W_D] = "vssrlrni.w.d",
    [OPC_VSSRLRNI_D_Q] = "vssrlrni.d.q",
    [OPC_VSSRARNI_B_H] = "vssrarni.b.h",
    [OPC_VSSRARNI_H_W] = "vssrarni.h.w",
    [OPC_VSSRARNI_W_D] = "vssrarni.w.d",
    [OPC_VSSRARNI_D_Q] = "vssrarni.d.q",
    [OPC_VSSRLRNI_BU_H] = "vssrlrni.bu.h",
    [OPC_VSSRLRNI_HU_W] = "vssrlrni.hu.w",
    [OPC_VSSRLRNI_WU_D] = "vssrlrni.wu.d",
    [OPC_VSSRLRNI_DU_Q] = "vssrlrni.du.q",
    [OPC_VSSRARNI_BU_H] = "vssrarni.bu.h",
    [OPC_VSSRARNI_HU_W] = "vssrarni.hu.w",
    [OPC_VSSRARNI_WU_D] = "vssrarni.wu.d",
    [OPC_VSSRARNI_DU_Q] = "vssrarni.du.q",
    [OPC_VCLO_B] = "vclo.b",
    [OPC_VCLO_H] = "vclo.h",
    [OPC_VCLO_W] = "vclo.w",
    [OPC_VCLO_D] = "vclo.d",
    [OPC_VCLZ_B] = "vclz.b",
    [OPC_VCLZ_H] = "vclz.h",
    [OPC_VCLZ_W] = "vclz.w",
    [OPC_VCLZ_D] = "vclz.d",
    [OPC_VPCNT_B] = "vpcnt.b",
    [OPC_VPCNT_H] = "vpcnt.h",
    [OPC_VPCNT_W] = "vpcnt.w",
    [OPC_VPCNT_D] = "vpcnt.d",
    [OPC_VBITCLR_B] = "vbitclr.b",
    [OPC_VBITCLR_H] = "vbitclr.h",
    [OPC_VBITCLR_W] = "vbitclr.w",
    [OPC_VBITCLR_D] = "vbitclr.d",
    [OPC_VBITCLRI_B] = "vbitclri.b",
    [OPC_VBITCLRI_H] = "vbitclri.h",
    [OPC_VBITCLRI_W] = "vbitclri.w",
    [OPC_VBITCLRI_D] = "vbitclri.d",
    [OPC_VBITSET_B] = "vbitset.b",
    [OPC_VBITSET_H] = "vbitset.h",
    [OPC_VBITSET_W] = "vbitset.w",
    [OPC_VBITSET_D] = "vbitset.d",
    [OPC_VBITSETI_B] = "vbitseti.b",
    [OPC_VBITSETI_H] = "vbitseti.h",
    [OPC_VBITSETI_W] = "vbitseti.w",
    [OPC_VBITSETI_D] = "vbitseti.d",
    [OPC_VBITREV_B] = "vbitrev.b",
    [OPC_VBITREV_H] = "vbitrev.h",
    [OPC_VBITREV_W] = "vbitrev.w",
    [OPC_VBITREV_D] = "vbitrev.d",
    [OPC_VBITREVI_B] = "vbitrevi.b",
    [OPC_VBITREVI_H] = "vbitrevi.h",
    [OPC_VBITREVI_W] = "vbitrevi.w",
    [OPC_VBITREVI_D] = "vbitrevi.d",
    [OPC_VFRSTP_B] = "vfrstp.b",
    [OPC_VFRSTP_H] = "vfrstp.h",
    [OPC_VFRSTPI_B] = "vfrstpi.b",
    [OPC_VFRSTPI_H] = "vfrstpi.h",
    [OPC_VFADD_S] = "vfadd.s",
    [OPC_VFADD_D] = "vfadd.d",
    [OPC_VFSUB_S] = "vfsub.s",
    [OPC_VFSUB_D] = "vfsub.d",
    [OPC_VFMUL_S] = "vfmul.s",
    [OPC_VFMUL_D] = "vfmul.d",
    [OPC_VFDIV_S] = "vfdiv.s",
    [OPC_VFDIV_D] = "vfdiv.d",
    [OPC_VFMADD_S] = "vfmadd.s",
    [OPC_VFMADD_D] = "vfmadd.d",
    [OPC_VFMSUB_S] = "vfmsub.s",
    [OPC_VFMSUB_D] = "vfmsub.d",
    [OPC_VFNMADD_S] = "vfnmadd.s",
    [OPC_VFNMADD_D] = "vfnmadd.d",
    [OPC_VFNMSUB_S] = "vfnmsub.s",
    [OPC_VFNMSUB_D] = "vfnmsub.d",
    [OPC_VFMAX_S] = "vfmax.s",
    [OPC_VFMAX_D] = "vfmax.d",
    [OPC_VFMIN_S] = "vfmin.s",
    [OPC_VFMIN_D] = "vfmin.d",
    [OPC_VFMAXA_S] = "vfmaxa.s",
    [OPC_VFMAXA_D] = "vfmaxa.d",
    [OPC_VFMINA_S] = "vfmina.s",
    [OPC_VFMINA_D] = "vfmina.d",
    [OPC_VFLOGB_S] = "vflogb.s",
    [OPC_VFLOGB_D] = "vflogb.d",
    [OPC_VFCLASS_S] = "vfclass.s",
    [OPC_VFCLASS_D] = "vfclass.d",
    [OPC_VFSQRT_S] = "vfsqrt.s",
    [OPC_VFSQRT_D] = "vfsqrt.d",
    [OPC_VFRECIP_S] = "vfrecip.s",
    [OPC_VFRECIP_D] = "vfrecip.d",
    [OPC_VFRSQRT_S] = "vfrsqrt.s",
    [OPC_VFRSQRT_D] = "vfrsqrt.d",
    [OPC_VFCVTL_S_H] = "vfcvtl.s.h",
    [OPC_VFCVTH_S_H] = "vfcvth.s.h",
    [OPC_VFCVTL_D_S] = "vfcvtl.d.s",
    [OPC_VFCVTH_D_S] = "vfcvth.d.s",
    [OPC_VFCVT_H_S] = "vfcvt.h.s",
    [OPC_VFCVT_S_D] = "vfcvt.s.d",
    [OPC_VFRINT_S] = "vfrint.s",
    [OPC_VFRINT_D] = "vfrint.d",
    [OPC_VFRINTRM_S] = "vfrintrm.s",
    [OPC_VFRINTRM_D] = "vfrintrm.d",
    [OPC_VFRINTRP_S] = "vfrintrp.s",
    [OPC_VFRINTRP_D] = "vfrintrp.d",
    [OPC_VFRINTRZ_S] = "vfrintrz.s",
    [OPC_VFRINTRZ_D] = "vfrintrz.d",
    [OPC_VFRINTRNE_S] = "vfrintrne.s",
    [OPC_VFRINTRNE_D] = "vfrintrne.d",
    [OPC_VFTINT_W_S] = "vftint.w.s",
    [OPC_VFTINT_L_D] = "vftint.l.d",
    [OPC_VFTINTRM_W_S] = "vftintrm.w.s",
    [OPC_VFTINTRM_L_D] = "vftintrm.l.d",
    [OPC_VFTINTRP_W_S] = "vftintrp.w.s",
    [OPC_VFTINTRP_L_D] = "vftintrp.l.d",
    [OPC_VFTINTRZ_W_S] = "vftintrz.w.s",
    [OPC_VFTINTRZ_L_D] = "vftintrz.l.d",
    [OPC_VFTINTRNE_W_S] = "vftintrne.w.s",
    [OPC_VFTINTRNE_L_D] = "vftintrne.l.d",
    [OPC_VFTINT_WU_S] = "vftint.wu.s",
    [OPC_VFTINT_LU_D] = "vftint.lu.d",
    [OPC_VFTINTRZ_WU_S] = "vftintrz.wu.s",
    [OPC_VFTINTRZ_LU_D] = "vftintrz.lu.d",
    [OPC_VFTINT_W_D] = "vftint.w.d",
    [OPC_VFTINTRM_W_D] = "vftintrm.w.d",
    [OPC_VFTINTRP_W_D] = "vftintrp.w.d",
    [OPC_VFTINTRZ_W_D] = "vftintrz.w.d",
    [OPC_VFTINTRNE_W_D] = "vftintrne.w.d",
    [OPC_VFTINTL_L_S] = "vftintl.l.s",
    [OPC_VFTINTH_L_S] = "vftinth.l.s",
    [OPC_VFTINTRML_L_S] = "vftintrml.l.s",
    [OPC_VFTINTRMH_L_S] = "vftintrmh.l.s",
    [OPC_VFTINTRPL_L_S] = "vftintrpl.l.s",
    [OPC_VFTINTRPH_L_S] = "vftintrph.l.s",
    [OPC_VFTINTRZL_L_S] = "vftintrzl.l.s",
    [OPC_VFTINTRZH_L_S] = "vftintrzh.l.s",
    [OPC_VFTINTRNEL_L_S] = "vftintrnel.l.s",
    [OPC_VFTINTRNEH_L_S] = "vftintrneh.l.s",
    [OPC_VFFINT_S_W] = "vffint.s.w",
    [OPC_VFFINT_S_WU] = "vffint.s.wu",
    [OPC_VFFINT_D_L] = "vffint.d.l",
    [OPC_VFFINT_D_LU] = "vffint.d.lu",
    [OPC_VFFINTL_D_W] = "vffintl.d.w",
    [OPC_VFFINTH_D_W] = "vffinth.d.w",
    [OPC_VFFINT_S_L] = "vffint.s.l",
    [OPC_VSEQ_B] = "vseq.b",
    [OPC_VSEQ_H] = "vseq.h",
    [OPC_VSEQ_W] = "vseq.w",
    [OPC_VSEQ_D] = "vseq.d",
    [OPC_VSEQI_B] = "vseqi.b",
    [OPC_VSEQI_H] = "vseqi.h",
    [OPC_VSEQI_W] = "vseqi.w",
    [OPC_VSEQI_D] = "vseqi.d",
    [OPC_VSLE_B] = "vsle.b",
    [OPC_VSLE_H] = "vsle.h",
    [OPC_VSLE_W] = "vsle.w",
    [OPC_VSLE_D] = "vsle.d",
    [OPC_VSLEI_B] = "vslei.b",
    [OPC_VSLEI_H] = "vslei.h",
    [OPC_VSLEI_W] = "vslei.w",
    [OPC_VSLEI_D] = "vslei.d",
    [OPC_VSLE_BU] = "vsle.bu",
    [OPC_VSLE_HU] = "vsle.hu",
    [OPC_VSLE_WU] = "vsle.wu",
    [OPC_VSLE_DU] = "vsle.du",
    [OPC_VSLEI_BU] = "vslei.bu",
    [OPC_VSLEI_HU] = "vslei.hu",
    [OPC_VSLEI_WU] = "vslei.wu",
    [OPC_VSLEI_DU] = "vslei.du",
    [OPC_VSLT_B] = "vslt.b",
    [OPC_VSLT_H] = "vslt.h",
    [OPC_VSLT_W] = "vslt.w",
    [OPC_VSLT_D] = "vslt.d",
    [OPC_VSLTI_B] = "vslti.b",
    [OPC_VSLTI_H] = "vslti.h",
    [OPC_VSLTI_W] = "vslti.w",
    [OPC_VSLTI_D] = "vslti.d",
    [OPC_VSLT_BU] = "vslt.bu",
    [OPC_VSLT_HU] = "vslt.hu",
    [OPC_VSLT_WU] = "vslt.wu",
    [OPC_VSLT_DU] = "vslt.du",
    [OPC_VSLTI_BU] = "vslti.bu",
    [OPC_VSLTI_HU] = "vslti.hu",
    [OPC_VSLTI_WU] = "vslti.wu",
    [OPC_VSLTI_DU] = "vslti.du",
    [OPC_VFCMP_COND_S] = "vfcmp.cond.s",
    [OPC_VFCMP_COND_D] = "vfcmp.cond.d",
    [OPC_VBITSEL_V] = "vbitsel.v",
    [OPC_VBITSELI_B] = "vbitseli.b",
    [OPC_VSETEQZ_V] = "vseteqz.v",
    [OPC_VSETNEZ_V] = "vsetnez.v",
    [OPC_VSETANYEQZ_B] = "vsetanyeqz.b",
    [OPC_VSETANYEQZ_H] = "vsetanyeqz.h",
    [OPC_VSETANYEQZ_W] = "vsetanyeqz.w",
    [OPC_VSETANYEQZ_D] = "vsetanyeqz.d",
    [OPC_VSETALLNEZ_B] = "vsetallnez.b",
    [OPC_VSETALLNEZ_H] = "vsetallnez.h",
    [OPC_VSETALLNEZ_W] = "vsetallnez.w",
    [OPC_VSETALLNEZ_D] = "vsetallnez.d",
    [OPC_VINSGR2VR_B] = "vinsgr2vr.b",
    [OPC_VINSGR2VR_H] = "vinsgr2vr.h",
    [OPC_VINSGR2VR_W] = "vinsgr2vr.w",
    [OPC_VINSGR2VR_D] = "vinsgr2vr.d",
    [OPC_VPICKVE2GR_B] = "vpickve2gr.b",
    [OPC_VPICKVE2GR_H] = "vpickve2gr.h",
    [OPC_VPICKVE2GR_W] = "vpickve2gr.w",
    [OPC_VPICKVE2GR_D] = "vpickve2gr.d",
    [OPC_VPICKVE2GR_BU] = "vpickve2gr.bu",
    [OPC_VPICKVE2GR_HU] = "vpickve2gr.hu",
    [OPC_VPICKVE2GR_WU] = "vpickve2gr.wu",
    [OPC_VPICKVE2GR_DU] = "vpickve2gr.du",
    [OPC_VREPLGR2VR_B] = "vreplgr2vr.b",
    [OPC_VREPLGR2VR_H] = "vreplgr2vr.h",
    [OPC_VREPLGR2VR_W] = "vreplgr2vr.w",
    [OPC_VREPLGR2VR_D] = "vreplgr2vr.d",
    [OPC_VREPLVE_B] = "vreplve.b",
    [OPC_VREPLVE_H] = "vreplve.h",
    [OPC_VREPLVE_W] = "vreplve.w",
    [OPC_VREPLVE_D] = "vreplve.d",
    [OPC_VREPLVEI_B] = "vreplvei.b",
    [OPC_VREPLVEI_H] = "vreplvei.h",
    [OPC_VREPLVEI_W] = "vreplvei.w",
    [OPC_VREPLVEI_D] = "vreplvei.d",
    [OPC_VBSLL_V] = "vbsll.v",
    [OPC_VBSRL_V] = "vbsrl.v",
    [OPC_VPACKEV_B] = "vpackev.b",
    [OPC_VPACKEV_H] = "vpackev.h",
    [OPC_VPACKEV_W] = "vpackev.w",
    [OPC_VPACKEV_D] = "vpackev.d",
    [OPC_VPACKOD_B] = "vpackod.b",
    [OPC_VPACKOD_H] = "vpackod.h",
    [OPC_VPACKOD_W] = "vpackod.w",
    [OPC_VPACKOD_D] = "vpackod.d",
    [OPC_VPICKEV_B] = "vpickev.b",
    [OPC_VPICKEV_H] = "vpickev.h",
    [OPC_VPICKEV_W] = "vpickev.w",
    [OPC_VPICKEV_D] = "vpickev.d",
    [OPC_VPICKOD_B] = "vpickod.b",
    [OPC_VPICKOD_H] = "vpickod.h",
    [OPC_VPICKOD_W] = "vpickod.w",
    [OPC_VPICKOD_D] = "vpickod.d",
    [OPC_VILVL_B] = "vilvl.b",
    [OPC_VILVL_H] = "vilvl.h",
    [OPC_VILVL_W] = "vilvl.w",
    [OPC_VILVL_D] = "vilvl.d",
    [OPC_VILVH_B] = "vilvh.b",
    [OPC_VILVH_H] = "vilvh.h",
    [OPC_VILVH_W] = "vilvh.w",
    [OPC_VILVH_D] = "vilvh.d",
    [OPC_VSHUF_B] = "vshuf.b",
    [OPC_VSHUF_H] = "vshuf.h",
    [OPC_VSHUF_W] = "vshuf.w",
    [OPC_VSHUF_D] = "vshuf.d",
    [OPC_VSHUF4I_B] = "vshuf4i.b",
    [OPC_VSHUF4I_H] = "vshuf4i.h",
    [OPC_VSHUF4I_W] = "vshuf4i.w",
    [OPC_VSHUF4I_D] = "vshuf4i.d",
    [OPC_VPERMI_W] = "vpermi.w",
    [OPC_VEXTRINS_D] = "vextrins.d",
    [OPC_VEXTRINS_W] = "vextrins.w",
    [OPC_VEXTRINS_H] = "vextrins.h",
    [OPC_VEXTRINS_B] = "vextrins.b",
    [OPC_VLD] = "vld",
    [OPC_VST] = "vst",
    [OPC_VLDX] = "vldx",
    [OPC_VSTX] = "vstx",
    [OPC_VLDREPL_D] = "vldrepl.d",
    [OPC_VLDREPL_W] = "vldrepl.w",
    [OPC_VLDREPL_H] = "vldrepl.h",
    [OPC_VLDREPL_B] = "vldrepl.b",
    [OPC_VSTELM_D] = "vstelm.d",
    [OPC_VSTELM_W] = "vstelm.w",
    [OPC_VSTELM_H] = "vstelm.h",
    [OPC_VSTELM_B] = "vstelm.b",
    [OPC_XVLD] = "xvld",
    [OPC_XVST] = "xvst",
};
    
    
    static inline uint32_t extract32(uint32_t value, int start, int length)
    {
    	assert(start >= 0 && length > 0 && length <= 32 - start);
    	return (value >> start) & (~0U >> (32 - length));
    }

    static inline int32_t sextract32(uint32_t value, int start, int length)
    {
    	assert(start >= 0 && length > 0 && length <= 32 - start);
    	return ((int32_t)(value << (32 - length - start))) >> (32 - length);
    }

    static inline uint32_t deposit32(uint32_t value, int start, int length,
                                 uint32_t fieldval)
    {
    	uint32_t mask;
    	assert(start >= 0 && length > 0 && length <= 32 - start);
    	mask = (~0U >> (32 - length)) << start;
    	return (value & ~mask) | ((fieldval << start) & mask);
    }

    
static inline void encode32(uint32_t* pinsn, uint32_t field, u_int8_t start, u_int8_t len)
{
    assert(start < 32 && len < 32 && start+len <= 32);
    uint32_t mask = (1<<len)-1;

    field &= mask;
    field <<= start;
    *pinsn |= field;
    return;
}

static inline void encode32_muti_field(uint32_t* pinsn, uint32_t field, u_int8_t* start_and_len, u_int8_t size)
{
    u_int8_t total_len = 0;
    for(int i=1;i<size;i+=2)
            total_len += start_and_len[i];

    uint32_t mask = (1<<total_len)-1;

    field &= mask;
    for(int i=0; i<size; i+=2)
    {
        mask = (1<<start_and_len[i+1])-1;
        uint32_t field_term = field & mask;
        encode32(pinsn, field_term, start_and_len[i], start_and_len[i+1]);
        field >>= start_and_len[i+1];
    }
}
    static void decode_extract_cv(LoongArchInsInfo *info, uint32_t insn)
{
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[0] = extract32(insn, 5, 5); //vj
    info->dst_cfreg = extract32(insn, 0, 3); //cd
    info->opnds[1] = extract32(insn, 0, 3); //cd
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_format_0(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_1(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->srcs_regs[1]  = extract32(insn, 10, 5);//rk
    info->opnds[1] = extract32(insn, 10, 5); //rk
    info->nb_opnds = 2;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_10(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_fregs[0]  = extract32(insn, 5, 5);//fj
    info->opnds[1] = extract32(insn, 5, 5); //fj
    info->srcs_fregs[1]  = extract32(insn, 10, 5);//fk
    info->opnds[2] = extract32(insn, 10, 5); //fk
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 2;
    info->num_lsx = 0;
}

static void decode_extract_format_11(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_fregs[0]  = extract32(insn, 5, 5);//fj
    info->opnds[1] = extract32(insn, 5, 5); //fj
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 1;
    info->num_lsx = 0;
}

static void decode_extract_format_12(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_13(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_fregs[0]  = extract32(insn, 5, 5);//fj
    info->opnds[1] = extract32(insn, 5, 5); //fj
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 1;
    info->num_lsx = 0;
}

static void decode_extract_format_14(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_fcsr = extract32(insn, 0, 5); //fcsr
    info->opnds[0] = extract32(insn, 0, 5); //fcsr
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_15(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_fcsrregs[0]  = extract32(insn, 5, 5);//fcsr_1
    info->opnds[1] = extract32(insn, 5, 5); //fcsr_1
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 1;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_16(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_cfreg = extract32(insn, 0, 3); //cd
    info->opnds[0] = extract32(insn, 0, 3); //cd
    info->srcs_fregs[0]  = extract32(insn, 5, 5);//fj
    info->opnds[1] = extract32(insn, 5, 5); //fj
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 1;
    info->num_lsx = 0;
}

static void decode_extract_format_17(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_cfregs[0]  = extract32(insn, 5, 3);//cj
    info->opnds[1] = extract32(insn, 5, 3); //cj
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 1;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_18(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_cfreg = extract32(insn, 0, 3); //cd
    info->opnds[0] = extract32(insn, 0, 3); //cd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_19(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_cfregs[0]  = extract32(insn, 5, 3);//cj
    info->opnds[1] = extract32(insn, 5, 3); //cj
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 1;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_2(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->srcs_regs[1]  = extract32(insn, 10, 5);//rk
    info->opnds[2] = extract32(insn, 10, 5); //rk
    info->sa = extract32(insn, 15, 2); //sa2
    info->opnds[3] = extract32(insn, 15, 2); //sa2
    info->nb_opnds = 4;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_20(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->si = sextract32(insn, 10, 12); //si12
    info->opnds[2] = sextract32(insn, 10, 12); //si12
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_21(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->ui = extract32(insn, 10, 12); //ui12
    info->opnds[2] = extract32(insn, 10, 12); //ui12
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_22(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->csr = extract32(insn, 10, 14); //csr
    info->opnds[1] = extract32(insn, 10, 14); //csr
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_23(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->csr = extract32(insn, 10, 14); //csr
    info->opnds[2] = extract32(insn, 10, 14); //csr
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_24(LoongArchInsInfo *info, uint32_t insn)
{
    info->code_2 = extract32(insn, 0, 5); //code_2
    info->opnds[0] = extract32(insn, 0, 5); //code_2
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->si = sextract32(insn, 10, 12); //si12
    info->opnds[2] = sextract32(insn, 10, 12); //si12
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_25(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->level = extract32(insn, 10, 8); //level
    info->opnds[2] = extract32(insn, 10, 8); //level
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_26(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->seq = extract32(insn, 10, 8); //seq
    info->opnds[1] = extract32(insn, 10, 8); //seq
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_27(LoongArchInsInfo *info, uint32_t insn)
{
    info->nb_opnds = 0;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_28(LoongArchInsInfo *info, uint32_t insn)
{
    info->level_3 = extract32(insn, 0, 15); //level_3
    info->opnds[0] = extract32(insn, 0, 15); //level_3
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_29(LoongArchInsInfo *info, uint32_t insn)
{
    info->op = extract32(insn, 0, 5); //op
    info->opnds[0] = extract32(insn, 0, 5); //op
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->srcs_regs[1]  = extract32(insn, 10, 5);//rk
    info->opnds[2] = extract32(insn, 10, 5); //rk
    info->nb_opnds = 3;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_3(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->srcs_regs[1]  = extract32(insn, 10, 5);//rk
    info->opnds[2] = extract32(insn, 10, 5); //rk
    info->sa = extract32(insn, 15, 3); //sa3
    info->opnds[3] = extract32(insn, 15, 3); //sa3
    info->nb_opnds = 4;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_30(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_fregs[0]  = extract32(insn, 5, 5);//fj
    info->opnds[1] = extract32(insn, 5, 5); //fj
    info->srcs_fregs[1]  = extract32(insn, 10, 5);//fk
    info->opnds[2] = extract32(insn, 10, 5); //fk
    info->srcs_fregs[2]  = extract32(insn, 15, 5);//fa
    info->opnds[3] = extract32(insn, 15, 5); //fa
    info->nb_opnds = 4;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 3;
    info->num_lsx = 0;
}

static void decode_extract_format_31(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_cfreg = extract32(insn, 0, 3); //cd
    info->opnds[0] = extract32(insn, 0, 3); //cd
    info->srcs_fregs[0]  = extract32(insn, 5, 5);//fj
    info->opnds[1] = extract32(insn, 5, 5); //fj
    info->srcs_fregs[1]  = extract32(insn, 10, 5);//fk
    info->opnds[2] = extract32(insn, 10, 5); //fk
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 2;
    info->num_lsx = 0;
}

static void decode_extract_format_32(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_fregs[0]  = extract32(insn, 5, 5);//fj
    info->opnds[1] = extract32(insn, 5, 5); //fj
    info->srcs_fregs[1]  = extract32(insn, 10, 5);//fk
    info->opnds[2] = extract32(insn, 10, 5); //fk
    info->srcs_cfregs[0]  = extract32(insn, 15, 3);//ca
    info->opnds[3] = extract32(insn, 15, 3); //ca
    info->nb_opnds = 4;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 1;
    info->num_sccrach = 0;
    info->num_fsrcs = 2;
    info->num_lsx = 0;
}

static void decode_extract_format_33(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->si = sextract32(insn, 10, 16); //si16
    info->opnds[2] = sextract32(insn, 10, 16); //si16
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_34(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->si = sextract32(insn, 5, 20); //si20
    info->opnds[1] = sextract32(insn, 5, 20); //si20
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_35(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->si = sextract32(insn, 10, 14); //si14
    info->opnds[2] = sextract32(insn, 10, 14); //si14
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_36(LoongArchInsInfo *info, uint32_t insn)
{
    info->hint = extract32(insn, 0, 5); //hint
    info->opnds[0] = extract32(insn, 0, 5); //hint
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->si = sextract32(insn, 10, 12); //si12
    info->opnds[2] = sextract32(insn, 10, 12); //si12
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_37(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->si = sextract32(insn, 10, 12); //si12
    info->opnds[2] = sextract32(insn, 10, 12); //si12
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_38(LoongArchInsInfo *info, uint32_t insn)
{
    info->hint = extract32(insn, 0, 5); //hint
    info->opnds[0] = extract32(insn, 0, 5); //hint
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->srcs_regs[1]  = extract32(insn, 10, 5);//rk
    info->opnds[2] = extract32(insn, 10, 5); //rk
    info->nb_opnds = 3;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_39(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_freg = extract32(insn, 0, 5); //fd
    info->opnds[0] = extract32(insn, 0, 5); //fd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->srcs_regs[1]  = extract32(insn, 10, 5);//rk
    info->opnds[2] = extract32(insn, 10, 5); //rk
    info->nb_opnds = 3;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_4(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->srcs_regs[1]  = extract32(insn, 10, 5);//rk
    info->opnds[2] = extract32(insn, 10, 5); //rk
    info->nb_opnds = 3;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_40(LoongArchInsInfo *info, uint32_t insn)
{
    info->hint = extract32(insn, 0, 15); //hint_4
    info->opnds[0] = extract32(insn, 0, 15); //hint_4
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_41(LoongArchInsInfo *info, uint32_t insn)
{
    info->hint = extract32(insn, 0, 15); //hint_5
    info->opnds[0] = extract32(insn, 0, 15); //hint_5
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_42(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->offs = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 5)); //offs2
    info->opnds[1] = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 5)); //offs2
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_43(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_cfregs[0]  = extract32(insn, 5, 3);//cj
    info->opnds[0] = extract32(insn, 5, 3); //cj
    info->offs = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 5)); //offs2
    info->opnds[1] = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 5)); //offs2
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 1;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_44(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->offs = sextract32(insn, 10, 16); //offs1
    info->opnds[2] = sextract32(insn, 10, 16); //offs1
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_45(LoongArchInsInfo *info, uint32_t insn)
{
    info->offs = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 10)); //offs3
    info->opnds[0] = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 10)); //offs3
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_46(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->dst_sccrach = extract32(insn, 0, 2); //sd
    info->opnds[1] = extract32(insn, 0, 2); //sd
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_47(LoongArchInsInfo *info, uint32_t insn)
{
    info->sccrach_regs[0]  = extract32(insn, 5, 2);//sj
    info->opnds[0] = extract32(insn, 5, 2); //sj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[1] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 1;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_48(LoongArchInsInfo *info, uint32_t insn)
{
    info->ptr = extract32(insn, 5, 3); //ptr
    info->opnds[0] = extract32(insn, 5, 3); //ptr
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_49(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_5(LoongArchInsInfo *info, uint32_t insn)
{
    info->code = extract32(insn, 0, 15); //code
    info->opnds[0] = extract32(insn, 0, 15); //code
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_50(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 1;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_51(LoongArchInsInfo *info, uint32_t insn)
{
    info->si = sextract32(insn, 10, 5); //si5
    info->opnds[0] = sextract32(insn, 10, 5); //si5
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_52(LoongArchInsInfo *info, uint32_t insn)
{
    info->cond = extract32(insn, 10, 4); //cond_1
    info->opnds[0] = extract32(insn, 10, 4); //cond_1
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_53(LoongArchInsInfo *info, uint32_t insn)
{
    info->cond = extract32(insn, 10, 4); //cond_1
    info->opnds[0] = extract32(insn, 10, 4); //cond_1
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[1] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_54(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 10, 5);//rk
    info->opnds[0] = extract32(insn, 10, 5); //rk
    info->srcs_regs[1]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->cond = extract32(insn, 0, 4); //cond_2
    info->opnds[2] = extract32(insn, 0, 4); //cond_2
    info->nb_opnds = 3;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_55(LoongArchInsInfo *info, uint32_t insn)
{
    info->cond = extract32(insn, 10, 4); //cond_1
    info->opnds[0] = extract32(insn, 10, 4); //cond_1
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_56(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 3); //ui3
    info->opnds[0] = extract32(insn, 10, 3); //ui3
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_57(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 4); //ui4
    info->opnds[0] = extract32(insn, 10, 4); //ui4
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_58(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 3); //ui3
    info->opnds[0] = extract32(insn, 10, 3); //ui3
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_59(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 4); //ui4
    info->opnds[0] = extract32(insn, 10, 4); //ui4
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_6(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->ui = extract32(insn, 10, 5); //ui5
    info->opnds[2] = extract32(insn, 10, 5); //ui5
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_60(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 5); //ui5
    info->opnds[0] = extract32(insn, 10, 5); //ui5
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_61(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 6); //ui6
    info->opnds[0] = extract32(insn, 10, 6); //ui6
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_62(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 8); //ui8
    info->opnds[0] = extract32(insn, 10, 8); //ui8
    info->op = extract32(insn, 5, 5); //bt_op
    info->opnds[1] = extract32(insn, 5, 5); //bt_op
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_63(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 8); //ui8
    info->opnds[0] = extract32(insn, 10, 8); //ui8
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[1] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_64(LoongArchInsInfo *info, uint32_t insn)
{
    info->offs = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 5)); //offs2
    info->opnds[0] = deposit32(extract32(insn, 10, 16), 16, 16, sextract32(insn, 0, 5)); //offs2
    info->nb_opnds = 1;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_7(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->ui = extract32(insn, 10, 6); //ui6
    info->opnds[2] = extract32(insn, 10, 6); //ui6
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_8(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->msb = extract32(insn, 16, 5); //msbw
    info->opnds[2] = extract32(insn, 16, 5); //msbw
    info->lsb = extract32(insn, 10, 5); //lsbw
    info->opnds[3] = extract32(insn, 10, 5); //lsbw
    info->nb_opnds = 4;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_format_9(LoongArchInsInfo *info, uint32_t insn)
{
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[0] = extract32(insn, 0, 5); //rd
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->msb = extract32(insn, 16, 6); //msbd
    info->opnds[2] = extract32(insn, 16, 6); //msbd
    info->lsb = extract32(insn, 10, 6); //lsbd
    info->opnds[3] = extract32(insn, 10, 6); //lsbd
    info->nb_opnds = 4;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_rv_ui1(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 1); //imm
    info->opnds[0] = extract32(insn, 10, 1); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_rv_ui2(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 2); //imm
    info->opnds[0] = extract32(insn, 10, 2); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_rv_ui3(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 3); //imm
    info->opnds[0] = extract32(insn, 10, 3); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_rv_ui4(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 4); //imm
    info->opnds[0] = extract32(insn, 10, 4); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_reg = extract32(insn, 0, 5); //rd
    info->opnds[2] = extract32(insn, 0, 5); //rd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_v_i13(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 5, 13); //imm
    info->opnds[0] = extract32(insn, 5, 13); //imm
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[1] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[1] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 2;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i10(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[1] = extract32(insn, 0, 5); //vd
    info->si = sextract32(insn, 10, 10); //imm
    info->opnds[2] = sextract32(insn, 10, 10); //imm
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i11(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[1] = extract32(insn, 0, 5); //vd
    info->si = sextract32(insn, 10, 11); //imm
    info->opnds[2] = sextract32(insn, 10, 11); //imm
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i12(LoongArchInsInfo *info, uint32_t insn)
{
    info->si = sextract32(insn, 10, 12); //imm
    info->opnds[0] = sextract32(insn, 10, 12); //imm
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i8i1(LoongArchInsInfo *info, uint32_t insn)
{
    info->_ui = extract32(insn, 18, 1); //imm2
    info->opnds[0] = extract32(insn, 18, 1); //imm2
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->si = sextract32(insn, 10, 8); //imm
    info->opnds[3] = sextract32(insn, 10, 8); //imm
    info->nb_opnds = 4;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i8i2(LoongArchInsInfo *info, uint32_t insn)
{
    info->_ui = extract32(insn, 18, 2); //imm2
    info->opnds[0] = extract32(insn, 18, 2); //imm2
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->si = sextract32(insn, 10, 8); //imm
    info->opnds[3] = sextract32(insn, 10, 8); //imm
    info->nb_opnds = 4;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i8i3(LoongArchInsInfo *info, uint32_t insn)
{
    info->_ui = extract32(insn, 18, 3); //imm2
    info->opnds[0] = extract32(insn, 18, 3); //imm2
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->si = sextract32(insn, 10, 8); //imm
    info->opnds[3] = sextract32(insn, 10, 8); //imm
    info->nb_opnds = 4;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i8i4(LoongArchInsInfo *info, uint32_t insn)
{
    info->_ui = extract32(insn, 18, 4); //imm2
    info->opnds[0] = extract32(insn, 18, 4); //imm2
    info->si = sextract32(insn, 10, 8); //imm
    info->opnds[1] = sextract32(insn, 10, 8); //imm
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[2] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[3] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 4;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_i9(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[0] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[1] = extract32(insn, 0, 5); //vd
    info->si = sextract32(insn, 10, 9); //imm
    info->opnds[2] = sextract32(insn, 10, 9); //imm
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_ui1(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 1); //imm
    info->opnds[0] = extract32(insn, 10, 1); //imm
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_ui2(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 2); //imm
    info->opnds[0] = extract32(insn, 10, 2); //imm
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_ui3(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 3); //imm
    info->opnds[0] = extract32(insn, 10, 3); //imm
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vr_ui4(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 4); //imm
    info->opnds[0] = extract32(insn, 10, 4); //imm
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vrr(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 10, 5);//rk
    info->opnds[0] = extract32(insn, 10, 5); //rk
    info->srcs_regs[1]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 2;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

static void decode_extract_vv(LoongArchInsInfo *info, uint32_t insn)
{
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[0] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[1] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 2;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_i5(LoongArchInsInfo *info, uint32_t insn)
{
    info->si = sextract32(insn, 10, 5); //imm
    info->opnds[0] = sextract32(insn, 10, 5); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui1(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 1); //imm
    info->opnds[0] = extract32(insn, 10, 1); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui2(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 2); //imm
    info->opnds[0] = extract32(insn, 10, 2); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui3(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 3); //imm
    info->opnds[0] = extract32(insn, 10, 3); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui4(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 4); //imm
    info->opnds[0] = extract32(insn, 10, 4); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui5(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 5); //imm
    info->opnds[0] = extract32(insn, 10, 5); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui6(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 6); //imm
    info->opnds[0] = extract32(insn, 10, 6); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui7(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 7); //imm
    info->opnds[0] = extract32(insn, 10, 7); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vv_ui8(LoongArchInsInfo *info, uint32_t insn)
{
    info->ui = extract32(insn, 10, 8); //imm
    info->opnds[0] = extract32(insn, 10, 8); //imm
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vvr(LoongArchInsInfo *info, uint32_t insn)
{
    info->srcs_regs[0]  = extract32(insn, 10, 5);//rk
    info->opnds[0] = extract32(insn, 10, 5); //rk
    info->lsx_src[0]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 1;
}

static void decode_extract_vvv(LoongArchInsInfo *info, uint32_t insn)
{
    info->lsx_src[0]  = extract32(insn, 10, 5);//vk
    info->opnds[0] = extract32(insn, 10, 5); //vk
    info->lsx_src[1]  = extract32(insn, 5, 5);//vj
    info->opnds[1] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[2] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 3;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 2;
}

static void decode_extract_vvv_fcond(LoongArchInsInfo *info, uint32_t insn)
{
    info->fcond = extract32(insn, 15, 5); //fcond
    info->opnds[0] = extract32(insn, 15, 5); //fcond
    info->lsx_src[0]  = extract32(insn, 10, 5);//vk
    info->opnds[1] = extract32(insn, 10, 5); //vk
    info->lsx_src[1]  = extract32(insn, 5, 5);//vj
    info->opnds[2] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[3] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 4;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 2;
}

static void decode_extract_vvvv(LoongArchInsInfo *info, uint32_t insn)
{
    info->lsx_src[0]  = extract32(insn, 15, 5);//va
    info->opnds[0] = extract32(insn, 15, 5); //va
    info->lsx_src[1]  = extract32(insn, 10, 5);//vk
    info->opnds[1] = extract32(insn, 10, 5); //vk
    info->lsx_src[2]  = extract32(insn, 5, 5);//vj
    info->opnds[2] = extract32(insn, 5, 5); //vj
    info->dst_lsx_reg = extract32(insn, 0, 5); //vd
    info->opnds[3] = extract32(insn, 0, 5); //vd
    info->nb_opnds = 4;
    info->num_srcs = 0;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 3;
}

static void decode_extract_xr_i12(LoongArchInsInfo *info, uint32_t insn)
{
    info->si = sextract32(insn, 10, 12); //imm
    info->opnds[0] = sextract32(insn, 10, 12); //imm
    info->srcs_regs[0]  = extract32(insn, 5, 5);//rj
    info->opnds[1] = extract32(insn, 5, 5); //rj
    info->dst_lasx_reg = extract32(insn, 0, 5); //xd
    info->opnds[2] = extract32(insn, 0, 5); //xd
    info->nb_opnds = 3;
    info->num_srcs = 1;
    info->num_fcsr = 0;
    info->num_cfreg = 0;
    info->num_sccrach = 0;
    info->num_fsrcs = 0;
    info->num_lsx = 0;
}

bool decode(LoongArchInsInfo *info, uint32_t insn)
{
    switch ((insn >> 26) & 0x3f) {
    case 0x0:
        /* 000000.. ........ ........ ........ */
        switch ((insn >> 22) & 0xf) {
        case 0x0:
            /* 00000000 00...... ........ ........ */
            switch ((insn >> 18) & 0xf) {
            case 0x0:
                /* 00000000 000000.. ........ ........ */
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00000000 0....... ........ */
                    switch ((insn >> 10) & 0x1f) {
                    case 0x2:
                        /* 00000000 00000000 000010.. ........ */
                        decode_extract_format_46(info, insn);
                        switch ((insn >> 2) & 0x7) {
                        case 0x0:
                            /* 00000000 00000000 000010.. ...000.. */
                            /* ./loongarch.decode.2:429 */
                            info->opc = OPC_GR2SCR;
                            return true;
                            break;
                        }
                        break;
                    case 0x3:
                        /* 00000000 00000000 000011.. ........ */
                        decode_extract_format_47(info, insn);
                        switch ((insn >> 7) & 0x7) {
                        case 0x0:
                            /* 00000000 00000000 00001100 0....... */
                            /* ./loongarch.decode.2:430 */
                            info->opc = OPC_SRC2GR;
                            return true;
                            break;
                        }
                        break;
                    case 0x4:
                        /* 00000000 00000000 000100.. ........ */
                        /* ./loongarch.decode.2:52 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CLO_W;
                        return true;
                        break;
                    case 0x5:
                        /* 00000000 00000000 000101.. ........ */
                        /* ./loongarch.decode.2:53 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CLZ_W;
                        return true;
                        break;
                    case 0x6:
                        /* 00000000 00000000 000110.. ........ */
                        /* ./loongarch.decode.2:54 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CTO_W;
                        return true;
                        break;
                    case 0x7:
                        /* 00000000 00000000 000111.. ........ */
                        /* ./loongarch.decode.2:55 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CTZ_W;
                        return true;
                        break;
                    case 0x8:
                        /* 00000000 00000000 001000.. ........ */
                        /* ./loongarch.decode.2:56 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CLO_D;
                        return true;
                        break;
                    case 0x9:
                        /* 00000000 00000000 001001.. ........ */
                        /* ./loongarch.decode.2:57 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CLZ_D;
                        return true;
                        break;
                    case 0xa:
                        /* 00000000 00000000 001010.. ........ */
                        /* ./loongarch.decode.2:58 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CTO_D;
                        return true;
                        break;
                    case 0xb:
                        /* 00000000 00000000 001011.. ........ */
                        /* ./loongarch.decode.2:59 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CTZ_D;
                        return true;
                        break;
                    case 0xc:
                        /* 00000000 00000000 001100.. ........ */
                        /* ./loongarch.decode.2:60 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_REVB_2H;
                        return true;
                        break;
                    case 0xd:
                        /* 00000000 00000000 001101.. ........ */
                        /* ./loongarch.decode.2:61 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_REVB_4H;
                        return true;
                        break;
                    case 0xe:
                        /* 00000000 00000000 001110.. ........ */
                        /* ./loongarch.decode.2:62 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_REVB_2W;
                        return true;
                        break;
                    case 0xf:
                        /* 00000000 00000000 001111.. ........ */
                        /* ./loongarch.decode.2:63 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_REVB_D;
                        return true;
                        break;
                    case 0x10:
                        /* 00000000 00000000 010000.. ........ */
                        /* ./loongarch.decode.2:64 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_REVH_2W;
                        return true;
                        break;
                    case 0x11:
                        /* 00000000 00000000 010001.. ........ */
                        /* ./loongarch.decode.2:65 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_REVH_D;
                        return true;
                        break;
                    case 0x12:
                        /* 00000000 00000000 010010.. ........ */
                        /* ./loongarch.decode.2:66 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_BITREV_4B;
                        return true;
                        break;
                    case 0x13:
                        /* 00000000 00000000 010011.. ........ */
                        /* ./loongarch.decode.2:67 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_BITREV_8B;
                        return true;
                        break;
                    case 0x14:
                        /* 00000000 00000000 010100.. ........ */
                        /* ./loongarch.decode.2:68 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_BITREV_W;
                        return true;
                        break;
                    case 0x15:
                        /* 00000000 00000000 010101.. ........ */
                        /* ./loongarch.decode.2:69 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_BITREV_D;
                        return true;
                        break;
                    case 0x16:
                        /* 00000000 00000000 010110.. ........ */
                        /* ./loongarch.decode.2:70 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_EXT_W_H;
                        return true;
                        break;
                    case 0x17:
                        /* 00000000 00000000 010111.. ........ */
                        /* ./loongarch.decode.2:71 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_EXT_W_B;
                        return true;
                        break;
                    case 0x18:
                        /* 00000000 00000000 011000.. ........ */
                        /* ./loongarch.decode.2:72 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_RDTIMEL_W;
                        return true;
                        break;
                    case 0x19:
                        /* 00000000 00000000 011001.. ........ */
                        /* ./loongarch.decode.2:73 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_RDTIMEH_W;
                        return true;
                        break;
                    case 0x1a:
                        /* 00000000 00000000 011010.. ........ */
                        /* ./loongarch.decode.2:74 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_RDTIME_D;
                        return true;
                        break;
                    case 0x1b:
                        /* 00000000 00000000 011011.. ........ */
                        /* ./loongarch.decode.2:75 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_CPUCFG;
                        return true;
                        break;
                    case 0x1c:
                        /* 00000000 00000000 011100.. ........ */
                        decode_extract_format_48(info, insn);
                        switch (insn & 0x0000031f) {
                        case 0x00000000:
                            /* 00000000 00000000 01110000 ...00000 */
                            /* ./loongarch.decode.2:431 */
                            info->opc = OPC_X86MTTOP;
                            return true;
                            break;
                        }
                        break;
                    case 0x1d:
                        /* 00000000 00000000 011101.. ........ */
                        decode_extract_format_49(info, insn);
                        switch ((insn >> 5) & 0x1f) {
                        case 0x0:
                            /* 00000000 00000000 01110100 000..... */
                            /* ./loongarch.decode.2:432 */
                            info->opc = OPC_X86MFTOP;
                            return true;
                            break;
                        }
                        break;
                    case 0x1e:
                        /* 00000000 00000000 011110.. ........ */
                        /* ./loongarch.decode.2:433 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_SETX86LOOPE;
                        return true;
                        break;
                    case 0x1f:
                        /* 00000000 00000000 011111.. ........ */
                        /* ./loongarch.decode.2:434 */
                        decode_extract_format_0(info, insn);
                        info->opc = OPC_SETX86LOOPNE;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 00000000 00000000 1....... ........ */
                    switch (insn & 0x00007c1f) {
                    case 0x00000000:
                        /* 00000000 00000000 100000.. ...00000 */
                        /* ./loongarch.decode.2:435 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86INC_B;
                        return true;
                        break;
                    case 0x00000001:
                        /* 00000000 00000000 100000.. ...00001 */
                        /* ./loongarch.decode.2:436 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86INC_H;
                        return true;
                        break;
                    case 0x00000002:
                        /* 00000000 00000000 100000.. ...00010 */
                        /* ./loongarch.decode.2:437 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86INC_W;
                        return true;
                        break;
                    case 0x00000003:
                        /* 00000000 00000000 100000.. ...00011 */
                        /* ./loongarch.decode.2:438 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86INC_D;
                        return true;
                        break;
                    case 0x00000004:
                        /* 00000000 00000000 100000.. ...00100 */
                        /* ./loongarch.decode.2:439 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86DEC_B;
                        return true;
                        break;
                    case 0x00000005:
                        /* 00000000 00000000 100000.. ...00101 */
                        /* ./loongarch.decode.2:440 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86DEC_H;
                        return true;
                        break;
                    case 0x00000006:
                        /* 00000000 00000000 100000.. ...00110 */
                        /* ./loongarch.decode.2:441 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86DEC_W;
                        return true;
                        break;
                    case 0x00000007:
                        /* 00000000 00000000 100000.. ...00111 */
                        /* ./loongarch.decode.2:442 */
                        decode_extract_format_50(info, insn);
                        info->opc = OPC_X86DEC_D;
                        return true;
                        break;
                    case 0x00000008:
                        /* 00000000 00000000 100000.. ...01000 */
                        decode_extract_format_27(info, insn);
                        switch ((insn >> 5) & 0x1f) {
                        case 0x0:
                            /* 00000000 00000000 10000000 00001000 */
                            /* ./loongarch.decode.2:443 */
                            info->opc = OPC_X86SETTM;
                            return true;
                            break;
                        case 0x1:
                            /* 00000000 00000000 10000000 00101000 */
                            /* ./loongarch.decode.2:444 */
                            info->opc = OPC_X86CLRTM;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000009:
                        /* 00000000 00000000 100000.. ...01001 */
                        decode_extract_format_27(info, insn);
                        switch ((insn >> 5) & 0x1f) {
                        case 0x0:
                            /* 00000000 00000000 10000000 00001001 */
                            /* ./loongarch.decode.2:445 */
                            info->opc = OPC_X86INCTOP;
                            return true;
                            break;
                        case 0x1:
                            /* 00000000 00000000 10000000 00101001 */
                            /* ./loongarch.decode.2:446 */
                            info->opc = OPC_X86DECTOP;
                            return true;
                            break;
                        }
                        break;
                    }
                    break;
                case 0x2:
                    /* 00000000 00000001 0....... ........ */
                    decode_extract_format_1(info, insn);
                    switch (insn & 0x0000001f) {
                    case 0x00000000:
                        /* 00000000 00000001 0....... ...00000 */
                        /* ./loongarch.decode.2:76 */
                        info->opc = OPC_ASRTLE_D;
                        return true;
                        break;
                    }
                    break;
                case 0x3:
                    /* 00000000 00000001 1....... ........ */
                    decode_extract_format_1(info, insn);
                    switch (insn & 0x0000001f) {
                    case 0x00000000:
                        /* 00000000 00000001 1....... ...00000 */
                        /* ./loongarch.decode.2:77 */
                        info->opc = OPC_ASRTGT_D;
                        return true;
                        break;
                    }
                    break;
                }
                break;
            case 0x1:
                /* 00000000 000001.. ........ ........ */
                decode_extract_format_2(info, insn);
                switch ((insn >> 17) & 0x1) {
                case 0x0:
                    /* 00000000 0000010. ........ ........ */
                    /* ./loongarch.decode.2:78 */
                    info->opc = OPC_ALSL_W;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 0000011. ........ ........ */
                    /* ./loongarch.decode.2:79 */
                    info->opc = OPC_ALSL_WU;
                    return true;
                    break;
                }
                break;
            case 0x2:
                /* 00000000 000010.. ........ ........ */
                decode_extract_format_2(info, insn);
                switch ((insn >> 17) & 0x1) {
                case 0x0:
                    /* 00000000 0000100. ........ ........ */
                    /* ./loongarch.decode.2:80 */
                    info->opc = OPC_BYTEPICK_W;
                    return true;
                    break;
                }
                break;
            case 0x3:
                /* 00000000 000011.. ........ ........ */
                /* ./loongarch.decode.2:81 */
                decode_extract_format_3(info, insn);
                info->opc = OPC_BYTEPICK_D;
                return true;
                break;
            case 0x4:
                /* 00000000 000100.. ........ ........ */
                decode_extract_format_4(info, insn);
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00010000 0....... ........ */
                    /* ./loongarch.decode.2:82 */
                    info->opc = OPC_ADD_W;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00010000 1....... ........ */
                    /* ./loongarch.decode.2:83 */
                    info->opc = OPC_ADD_D;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00010001 0....... ........ */
                    /* ./loongarch.decode.2:84 */
                    info->opc = OPC_SUB_W;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00010001 1....... ........ */
                    /* ./loongarch.decode.2:85 */
                    info->opc = OPC_SUB_D;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00010010 0....... ........ */
                    /* ./loongarch.decode.2:86 */
                    info->opc = OPC_SLT;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00010010 1....... ........ */
                    /* ./loongarch.decode.2:87 */
                    info->opc = OPC_SLTU;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00010011 0....... ........ */
                    /* ./loongarch.decode.2:88 */
                    info->opc = OPC_MASKEQZ;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00010011 1....... ........ */
                    /* ./loongarch.decode.2:89 */
                    info->opc = OPC_MASKNEZ;
                    return true;
                    break;
                }
                break;
            case 0x5:
                /* 00000000 000101.. ........ ........ */
                decode_extract_format_4(info, insn);
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00010100 0....... ........ */
                    /* ./loongarch.decode.2:90 */
                    info->opc = OPC_NOR;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00010100 1....... ........ */
                    /* ./loongarch.decode.2:91 */
                    info->opc = OPC_AND;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00010101 0....... ........ */
                    /* ./loongarch.decode.2:92 */
                    info->opc = OPC_OR;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00010101 1....... ........ */
                    /* ./loongarch.decode.2:93 */
                    info->opc = OPC_XOR;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00010110 0....... ........ */
                    /* ./loongarch.decode.2:94 */
                    info->opc = OPC_ORN;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00010110 1....... ........ */
                    /* ./loongarch.decode.2:95 */
                    info->opc = OPC_ANDN;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00010111 0....... ........ */
                    /* ./loongarch.decode.2:96 */
                    info->opc = OPC_SLL_W;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00010111 1....... ........ */
                    /* ./loongarch.decode.2:97 */
                    info->opc = OPC_SRL_W;
                    return true;
                    break;
                }
                break;
            case 0x6:
                /* 00000000 000110.. ........ ........ */
                decode_extract_format_4(info, insn);
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00011000 0....... ........ */
                    /* ./loongarch.decode.2:98 */
                    info->opc = OPC_SRA_W;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00011000 1....... ........ */
                    /* ./loongarch.decode.2:99 */
                    info->opc = OPC_SLL_D;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00011001 0....... ........ */
                    /* ./loongarch.decode.2:100 */
                    info->opc = OPC_SRL_D;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00011001 1....... ........ */
                    /* ./loongarch.decode.2:101 */
                    info->opc = OPC_SRA_D;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00011010 0....... ........ */
                    /* ./loongarch.decode.2:447 */
                    info->opc = OPC_ROTR_B;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00011010 1....... ........ */
                    /* ./loongarch.decode.2:448 */
                    info->opc = OPC_ROTR_H;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00011011 0....... ........ */
                    /* ./loongarch.decode.2:102 */
                    info->opc = OPC_ROTR_W;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00011011 1....... ........ */
                    /* ./loongarch.decode.2:103 */
                    info->opc = OPC_ROTR_D;
                    return true;
                    break;
                }
                break;
            case 0x7:
                /* 00000000 000111.. ........ ........ */
                decode_extract_format_4(info, insn);
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00011100 0....... ........ */
                    /* ./loongarch.decode.2:104 */
                    info->opc = OPC_MUL_W;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00011100 1....... ........ */
                    /* ./loongarch.decode.2:105 */
                    info->opc = OPC_MULH_W;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00011101 0....... ........ */
                    /* ./loongarch.decode.2:106 */
                    info->opc = OPC_MULH_WU;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00011101 1....... ........ */
                    /* ./loongarch.decode.2:107 */
                    info->opc = OPC_MUL_D;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00011110 0....... ........ */
                    /* ./loongarch.decode.2:108 */
                    info->opc = OPC_MULH_D;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00011110 1....... ........ */
                    /* ./loongarch.decode.2:109 */
                    info->opc = OPC_MULH_DU;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00011111 0....... ........ */
                    /* ./loongarch.decode.2:110 */
                    info->opc = OPC_MULW_D_W;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00011111 1....... ........ */
                    /* ./loongarch.decode.2:111 */
                    info->opc = OPC_MULW_D_WU;
                    return true;
                    break;
                }
                break;
            case 0x8:
                /* 00000000 001000.. ........ ........ */
                decode_extract_format_4(info, insn);
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00100000 0....... ........ */
                    /* ./loongarch.decode.2:112 */
                    info->opc = OPC_DIV_W;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00100000 1....... ........ */
                    /* ./loongarch.decode.2:113 */
                    info->opc = OPC_MOD_W;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00100001 0....... ........ */
                    /* ./loongarch.decode.2:114 */
                    info->opc = OPC_DIV_WU;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00100001 1....... ........ */
                    /* ./loongarch.decode.2:115 */
                    info->opc = OPC_MOD_WU;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00100010 0....... ........ */
                    /* ./loongarch.decode.2:116 */
                    info->opc = OPC_DIV_D;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00100010 1....... ........ */
                    /* ./loongarch.decode.2:117 */
                    info->opc = OPC_MOD_D;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00100011 0....... ........ */
                    /* ./loongarch.decode.2:118 */
                    info->opc = OPC_DIV_DU;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00100011 1....... ........ */
                    /* ./loongarch.decode.2:119 */
                    info->opc = OPC_MOD_DU;
                    return true;
                    break;
                }
                break;
            case 0x9:
                /* 00000000 001001.. ........ ........ */
                decode_extract_format_4(info, insn);
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00100100 0....... ........ */
                    /* ./loongarch.decode.2:120 */
                    info->opc = OPC_CRC_W_B_W;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00100100 1....... ........ */
                    /* ./loongarch.decode.2:121 */
                    info->opc = OPC_CRC_W_H_W;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00100101 0....... ........ */
                    /* ./loongarch.decode.2:122 */
                    info->opc = OPC_CRC_W_W_W;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00100101 1....... ........ */
                    /* ./loongarch.decode.2:123 */
                    info->opc = OPC_CRC_W_D_W;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00100110 0....... ........ */
                    /* ./loongarch.decode.2:124 */
                    info->opc = OPC_CRCC_W_B_W;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00100110 1....... ........ */
                    /* ./loongarch.decode.2:125 */
                    info->opc = OPC_CRCC_W_H_W;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00100111 0....... ........ */
                    /* ./loongarch.decode.2:126 */
                    info->opc = OPC_CRCC_W_W_W;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00100111 1....... ........ */
                    /* ./loongarch.decode.2:127 */
                    info->opc = OPC_CRCC_W_D_W;
                    return true;
                    break;
                }
                break;
            case 0xa:
                /* 00000000 001010.. ........ ........ */
                switch ((insn >> 15) & 0x7) {
                case 0x2:
                    /* 00000000 00101001 0....... ........ */
                    /* ./loongarch.decode.2:450 */
                    decode_extract_format_51(info, insn);
                    info->opc = OPC_ADDU12I_D;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00101001 1....... ........ */
                    /* ./loongarch.decode.2:449 */
                    decode_extract_format_51(info, insn);
                    info->opc = OPC_ADDU12I_W;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00101010 0....... ........ */
                    /* ./loongarch.decode.2:128 */
                    decode_extract_format_5(info, insn);
                    info->opc = OPC_BREAK;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00101010 1....... ........ */
                    /* ./loongarch.decode.2:129 */
                    decode_extract_format_5(info, insn);
                    info->opc = OPC_DBCL;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00101011 0....... ........ */
                    /* ./loongarch.decode.2:130 */
                    decode_extract_format_5(info, insn);
                    info->opc = OPC_SYSCALL;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00101011 1....... ........ */
                    /* ./loongarch.decode.2:451 */
                    decode_extract_format_5(info, insn);
                    info->opc = OPC_HVCL;
                    return true;
                    break;
                }
                break;
            case 0xb:
                /* 00000000 001011.. ........ ........ */
                decode_extract_format_2(info, insn);
                switch ((insn >> 17) & 0x1) {
                case 0x0:
                    /* 00000000 0010110. ........ ........ */
                    /* ./loongarch.decode.2:131 */
                    info->opc = OPC_ALSL_D;
                    return true;
                    break;
                }
                break;
            case 0xc:
                /* 00000000 001100.. ........ ........ */
                decode_extract_format_4(info, insn);
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00110000 0....... ........ */
                    /* ./loongarch.decode.2:452 */
                    info->opc = OPC_ADC_B;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00110000 1....... ........ */
                    /* ./loongarch.decode.2:453 */
                    info->opc = OPC_ADC_H;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00110001 0....... ........ */
                    /* ./loongarch.decode.2:454 */
                    info->opc = OPC_ADC_W;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00110001 1....... ........ */
                    /* ./loongarch.decode.2:455 */
                    info->opc = OPC_ADC_D;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00110010 0....... ........ */
                    /* ./loongarch.decode.2:456 */
                    info->opc = OPC_SBC_B;
                    return true;
                    break;
                case 0x5:
                    /* 00000000 00110010 1....... ........ */
                    /* ./loongarch.decode.2:457 */
                    info->opc = OPC_SBC_H;
                    return true;
                    break;
                case 0x6:
                    /* 00000000 00110011 0....... ........ */
                    /* ./loongarch.decode.2:458 */
                    info->opc = OPC_SBC_W;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 00110011 1....... ........ */
                    /* ./loongarch.decode.2:459 */
                    info->opc = OPC_SBC_D;
                    return true;
                    break;
                }
                break;
            case 0xd:
                /* 00000000 001101.. ........ ........ */
                switch ((insn >> 15) & 0x7) {
                case 0x0:
                    /* 00000000 00110100 0....... ........ */
                    /* ./loongarch.decode.2:460 */
                    decode_extract_format_4(info, insn);
                    info->opc = OPC_RCR_B;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 00110100 1....... ........ */
                    /* ./loongarch.decode.2:461 */
                    decode_extract_format_4(info, insn);
                    info->opc = OPC_RCR_H;
                    return true;
                    break;
                case 0x2:
                    /* 00000000 00110101 0....... ........ */
                    /* ./loongarch.decode.2:462 */
                    decode_extract_format_4(info, insn);
                    info->opc = OPC_RCR_W;
                    return true;
                    break;
                case 0x3:
                    /* 00000000 00110101 1....... ........ */
                    /* ./loongarch.decode.2:463 */
                    decode_extract_format_4(info, insn);
                    info->opc = OPC_RCR_D;
                    return true;
                    break;
                case 0x4:
                    /* 00000000 00110110 0....... ........ */
                    decode_extract_format_52(info, insn);
                    switch ((insn >> 14) & 0x1) {
                    case 0x1:
                        /* 00000000 00110110 01...... ........ */
                        /* ./loongarch.decode.2:464 */
                        info->opc = OPC_ARMMOVE;
                        return true;
                        break;
                    }
                    break;
                case 0x5:
                    /* 00000000 00110110 1....... ........ */
                    decode_extract_format_53(info, insn);
                    switch (insn & 0x000043e0) {
                    case 0x00000000:
                        /* 00000000 00110110 10....00 000..... */
                        /* ./loongarch.decode.2:465 */
                        info->opc = OPC_SETX86J;
                        return true;
                        break;
                    case 0x00004000:
                        /* 00000000 00110110 11....00 000..... */
                        /* ./loongarch.decode.2:466 */
                        info->opc = OPC_SETARMJ;
                        return true;
                        break;
                    }
                    break;
                case 0x6:
                    /* 00000000 00110111 0....... ........ */
                    decode_extract_format_54(info, insn);
                    switch ((insn >> 4) & 0x1) {
                    case 0x1:
                        /* 00000000 00110111 0....... ...1.... */
                        /* ./loongarch.decode.2:467 */
                        info->opc = OPC_ARMADD_W;
                        return true;
                        break;
                    }
                    break;
                case 0x7:
                    /* 00000000 00110111 1....... ........ */
                    decode_extract_format_54(info, insn);
                    switch ((insn >> 4) & 0x1) {
                    case 0x1:
                        /* 00000000 00110111 1....... ...1.... */
                        /* ./loongarch.decode.2:468 */
                        info->opc = OPC_ARMSUB_W;
                        return true;
                        break;
                    }
                    break;
                }
                break;
            case 0xe:
                /* 00000000 001110.. ........ ........ */
                decode_extract_format_54(info, insn);
                switch (insn & 0x00038010) {
                case 0x00000010:
                    /* 00000000 00111000 0....... ...1.... */
                    /* ./loongarch.decode.2:469 */
                    info->opc = OPC_ARMADC_W;
                    return true;
                    break;
                case 0x00008010:
                    /* 00000000 00111000 1....... ...1.... */
                    /* ./loongarch.decode.2:470 */
                    info->opc = OPC_ARMSBC_W;
                    return true;
                    break;
                case 0x00010010:
                    /* 00000000 00111001 0....... ...1.... */
                    /* ./loongarch.decode.2:471 */
                    info->opc = OPC_ARMAND_W;
                    return true;
                    break;
                case 0x00018010:
                    /* 00000000 00111001 1....... ...1.... */
                    /* ./loongarch.decode.2:472 */
                    info->opc = OPC_ARMOR_W;
                    return true;
                    break;
                case 0x00020010:
                    /* 00000000 00111010 0....... ...1.... */
                    /* ./loongarch.decode.2:473 */
                    info->opc = OPC_ARMXOR_W;
                    return true;
                    break;
                case 0x00028010:
                    /* 00000000 00111010 1....... ...1.... */
                    /* ./loongarch.decode.2:474 */
                    info->opc = OPC_ARMSLL_W;
                    return true;
                    break;
                case 0x00030010:
                    /* 00000000 00111011 0....... ...1.... */
                    /* ./loongarch.decode.2:475 */
                    info->opc = OPC_ARMSRL_W;
                    return true;
                    break;
                case 0x00038010:
                    /* 00000000 00111011 1....... ...1.... */
                    /* ./loongarch.decode.2:476 */
                    info->opc = OPC_ARMSRA_W;
                    return true;
                    break;
                }
                break;
            case 0xf:
                /* 00000000 001111.. ........ ........ */
                switch (insn & 0x00038010) {
                case 0x00000010:
                    /* 00000000 00111100 0....... ...1.... */
                    /* ./loongarch.decode.2:477 */
                    decode_extract_format_54(info, insn);
                    info->opc = OPC_ARMROTR_W;
                    return true;
                    break;
                case 0x00008010:
                    /* 00000000 00111100 1....... ...1.... */
                    /* ./loongarch.decode.2:478 */
                    decode_extract_format_54(info, insn);
                    info->opc = OPC_ARMSLLI_W;
                    return true;
                    break;
                case 0x00010010:
                    /* 00000000 00111101 0....... ...1.... */
                    /* ./loongarch.decode.2:479 */
                    decode_extract_format_54(info, insn);
                    info->opc = OPC_ARMSRLI_W;
                    return true;
                    break;
                case 0x00018010:
                    /* 00000000 00111101 1....... ...1.... */
                    /* ./loongarch.decode.2:480 */
                    decode_extract_format_54(info, insn);
                    info->opc = OPC_ARMSRAI_W;
                    return true;
                    break;
                case 0x00020010:
                    /* 00000000 00111110 0....... ...1.... */
                    /* ./loongarch.decode.2:481 */
                    decode_extract_format_54(info, insn);
                    info->opc = OPC_ARMROTRI_W;
                    return true;
                    break;
                case 0x00028000:
                    /* 00000000 00111110 1....... ...0.... */
                    decode_extract_format_1(info, insn);
                    switch (insn & 0x0000000f) {
                    case 0x00000000:
                        /* 00000000 00111110 1....... ...00000 */
                        /* ./loongarch.decode.2:482 */
                        info->opc = OPC_X86MUL_B;
                        return true;
                        break;
                    case 0x00000001:
                        /* 00000000 00111110 1....... ...00001 */
                        /* ./loongarch.decode.2:483 */
                        info->opc = OPC_X86MUL_H;
                        return true;
                        break;
                    case 0x00000002:
                        /* 00000000 00111110 1....... ...00010 */
                        /* ./loongarch.decode.2:484 */
                        info->opc = OPC_X86MUL_W;
                        return true;
                        break;
                    case 0x00000003:
                        /* 00000000 00111110 1....... ...00011 */
                        /* ./loongarch.decode.2:485 */
                        info->opc = OPC_X86MUL_D;
                        return true;
                        break;
                    case 0x00000004:
                        /* 00000000 00111110 1....... ...00100 */
                        /* ./loongarch.decode.2:486 */
                        info->opc = OPC_X86MUL_BU;
                        return true;
                        break;
                    case 0x00000005:
                        /* 00000000 00111110 1....... ...00101 */
                        /* ./loongarch.decode.2:487 */
                        info->opc = OPC_X86MUL_HU;
                        return true;
                        break;
                    case 0x00000006:
                        /* 00000000 00111110 1....... ...00110 */
                        /* ./loongarch.decode.2:488 */
                        info->opc = OPC_X86MUL_WU;
                        return true;
                        break;
                    case 0x00000007:
                        /* 00000000 00111110 1....... ...00111 */
                        /* ./loongarch.decode.2:489 */
                        info->opc = OPC_X86MUL_DU;
                        return true;
                        break;
                    }
                    break;
                case 0x00030000:
                    /* 00000000 00111111 0....... ...0.... */
                    decode_extract_format_1(info, insn);
                    switch (insn & 0x0000000f) {
                    case 0x00000000:
                        /* 00000000 00111111 0....... ...00000 */
                        /* ./loongarch.decode.2:490 */
                        info->opc = OPC_X86ADD_WU;
                        return true;
                        break;
                    case 0x00000001:
                        /* 00000000 00111111 0....... ...00001 */
                        /* ./loongarch.decode.2:491 */
                        info->opc = OPC_X86ADD_DU;
                        return true;
                        break;
                    case 0x00000002:
                        /* 00000000 00111111 0....... ...00010 */
                        /* ./loongarch.decode.2:492 */
                        info->opc = OPC_X86SUB_WU;
                        return true;
                        break;
                    case 0x00000003:
                        /* 00000000 00111111 0....... ...00011 */
                        /* ./loongarch.decode.2:493 */
                        info->opc = OPC_X86SUB_DU;
                        return true;
                        break;
                    case 0x00000004:
                        /* 00000000 00111111 0....... ...00100 */
                        /* ./loongarch.decode.2:494 */
                        info->opc = OPC_X86ADD_B;
                        return true;
                        break;
                    case 0x00000005:
                        /* 00000000 00111111 0....... ...00101 */
                        /* ./loongarch.decode.2:495 */
                        info->opc = OPC_X86ADD_H;
                        return true;
                        break;
                    case 0x00000006:
                        /* 00000000 00111111 0....... ...00110 */
                        /* ./loongarch.decode.2:496 */
                        info->opc = OPC_X86ADD_W;
                        return true;
                        break;
                    case 0x00000007:
                        /* 00000000 00111111 0....... ...00111 */
                        /* ./loongarch.decode.2:497 */
                        info->opc = OPC_X86ADD_D;
                        return true;
                        break;
                    case 0x00000008:
                        /* 00000000 00111111 0....... ...01000 */
                        /* ./loongarch.decode.2:498 */
                        info->opc = OPC_X86SUB_B;
                        return true;
                        break;
                    case 0x00000009:
                        /* 00000000 00111111 0....... ...01001 */
                        /* ./loongarch.decode.2:499 */
                        info->opc = OPC_X86SUB_H;
                        return true;
                        break;
                    case 0x0000000a:
                        /* 00000000 00111111 0....... ...01010 */
                        /* ./loongarch.decode.2:500 */
                        info->opc = OPC_X86SUB_W;
                        return true;
                        break;
                    case 0x0000000b:
                        /* 00000000 00111111 0....... ...01011 */
                        /* ./loongarch.decode.2:501 */
                        info->opc = OPC_X86SUB_D;
                        return true;
                        break;
                    case 0x0000000c:
                        /* 00000000 00111111 0....... ...01100 */
                        /* ./loongarch.decode.2:502 */
                        info->opc = OPC_X86ADC_B;
                        return true;
                        break;
                    case 0x0000000d:
                        /* 00000000 00111111 0....... ...01101 */
                        /* ./loongarch.decode.2:503 */
                        info->opc = OPC_X86ADC_H;
                        return true;
                        break;
                    case 0x0000000e:
                        /* 00000000 00111111 0....... ...01110 */
                        /* ./loongarch.decode.2:504 */
                        info->opc = OPC_X86ADC_W;
                        return true;
                        break;
                    case 0x0000000f:
                        /* 00000000 00111111 0....... ...01111 */
                        /* ./loongarch.decode.2:505 */
                        info->opc = OPC_X86ADC_D;
                        return true;
                        break;
                    }
                    break;
                case 0x00030010:
                    /* 00000000 00111111 0....... ...1.... */
                    decode_extract_format_1(info, insn);
                    switch (insn & 0x0000000f) {
                    case 0x00000000:
                        /* 00000000 00111111 0....... ...10000 */
                        /* ./loongarch.decode.2:506 */
                        info->opc = OPC_X86SBC_B;
                        return true;
                        break;
                    case 0x00000001:
                        /* 00000000 00111111 0....... ...10001 */
                        /* ./loongarch.decode.2:507 */
                        info->opc = OPC_X86SBC_H;
                        return true;
                        break;
                    case 0x00000002:
                        /* 00000000 00111111 0....... ...10010 */
                        /* ./loongarch.decode.2:508 */
                        info->opc = OPC_X86SBC_W;
                        return true;
                        break;
                    case 0x00000003:
                        /* 00000000 00111111 0....... ...10011 */
                        /* ./loongarch.decode.2:509 */
                        info->opc = OPC_X86SBC_D;
                        return true;
                        break;
                    case 0x00000004:
                        /* 00000000 00111111 0....... ...10100 */
                        /* ./loongarch.decode.2:510 */
                        info->opc = OPC_X86SLL_B;
                        return true;
                        break;
                    case 0x00000005:
                        /* 00000000 00111111 0....... ...10101 */
                        /* ./loongarch.decode.2:511 */
                        info->opc = OPC_X86SLL_H;
                        return true;
                        break;
                    case 0x00000006:
                        /* 00000000 00111111 0....... ...10110 */
                        /* ./loongarch.decode.2:512 */
                        info->opc = OPC_X86SLL_W;
                        return true;
                        break;
                    case 0x00000007:
                        /* 00000000 00111111 0....... ...10111 */
                        /* ./loongarch.decode.2:513 */
                        info->opc = OPC_X86SLL_D;
                        return true;
                        break;
                    case 0x00000008:
                        /* 00000000 00111111 0....... ...11000 */
                        /* ./loongarch.decode.2:514 */
                        info->opc = OPC_X86SRL_B;
                        return true;
                        break;
                    case 0x00000009:
                        /* 00000000 00111111 0....... ...11001 */
                        /* ./loongarch.decode.2:515 */
                        info->opc = OPC_X86SRL_H;
                        return true;
                        break;
                    case 0x0000000a:
                        /* 00000000 00111111 0....... ...11010 */
                        /* ./loongarch.decode.2:516 */
                        info->opc = OPC_X86SRL_W;
                        return true;
                        break;
                    case 0x0000000b:
                        /* 00000000 00111111 0....... ...11011 */
                        /* ./loongarch.decode.2:517 */
                        info->opc = OPC_X86SRL_D;
                        return true;
                        break;
                    case 0x0000000c:
                        /* 00000000 00111111 0....... ...11100 */
                        /* ./loongarch.decode.2:518 */
                        info->opc = OPC_X86SRA_B;
                        return true;
                        break;
                    case 0x0000000d:
                        /* 00000000 00111111 0....... ...11101 */
                        /* ./loongarch.decode.2:519 */
                        info->opc = OPC_X86SRA_H;
                        return true;
                        break;
                    case 0x0000000e:
                        /* 00000000 00111111 0....... ...11110 */
                        /* ./loongarch.decode.2:520 */
                        info->opc = OPC_X86SRA_W;
                        return true;
                        break;
                    case 0x0000000f:
                        /* 00000000 00111111 0....... ...11111 */
                        /* ./loongarch.decode.2:521 */
                        info->opc = OPC_X86SRA_D;
                        return true;
                        break;
                    }
                    break;
                case 0x00038000:
                    /* 00000000 00111111 1....... ...0.... */
                    decode_extract_format_1(info, insn);
                    switch (insn & 0x0000000f) {
                    case 0x00000000:
                        /* 00000000 00111111 1....... ...00000 */
                        /* ./loongarch.decode.2:522 */
                        info->opc = OPC_X86ROTR_B;
                        return true;
                        break;
                    case 0x00000001:
                        /* 00000000 00111111 1....... ...00001 */
                        /* ./loongarch.decode.2:523 */
                        info->opc = OPC_X86ROTR_H;
                        return true;
                        break;
                    case 0x00000002:
                        /* 00000000 00111111 1....... ...00010 */
                        /* ./loongarch.decode.2:524 */
                        info->opc = OPC_X86ROTR_D;
                        return true;
                        break;
                    case 0x00000003:
                        /* 00000000 00111111 1....... ...00011 */
                        /* ./loongarch.decode.2:525 */
                        info->opc = OPC_X86ROTR_W;
                        return true;
                        break;
                    case 0x00000004:
                        /* 00000000 00111111 1....... ...00100 */
                        /* ./loongarch.decode.2:526 */
                        info->opc = OPC_X86ROTL_B;
                        return true;
                        break;
                    case 0x00000005:
                        /* 00000000 00111111 1....... ...00101 */
                        /* ./loongarch.decode.2:527 */
                        info->opc = OPC_X86ROTL_H;
                        return true;
                        break;
                    case 0x00000006:
                        /* 00000000 00111111 1....... ...00110 */
                        /* ./loongarch.decode.2:528 */
                        info->opc = OPC_X86ROTL_W;
                        return true;
                        break;
                    case 0x00000007:
                        /* 00000000 00111111 1....... ...00111 */
                        /* ./loongarch.decode.2:529 */
                        info->opc = OPC_X86ROTL_D;
                        return true;
                        break;
                    case 0x00000008:
                        /* 00000000 00111111 1....... ...01000 */
                        /* ./loongarch.decode.2:530 */
                        info->opc = OPC_X86RCR_B;
                        return true;
                        break;
                    case 0x00000009:
                        /* 00000000 00111111 1....... ...01001 */
                        /* ./loongarch.decode.2:531 */
                        info->opc = OPC_X86RCR_H;
                        return true;
                        break;
                    case 0x0000000a:
                        /* 00000000 00111111 1....... ...01010 */
                        /* ./loongarch.decode.2:532 */
                        info->opc = OPC_X86RCR_W;
                        return true;
                        break;
                    case 0x0000000b:
                        /* 00000000 00111111 1....... ...01011 */
                        /* ./loongarch.decode.2:533 */
                        info->opc = OPC_X86RCR_D;
                        return true;
                        break;
                    case 0x0000000c:
                        /* 00000000 00111111 1....... ...01100 */
                        /* ./loongarch.decode.2:534 */
                        info->opc = OPC_X86RCL_B;
                        return true;
                        break;
                    case 0x0000000d:
                        /* 00000000 00111111 1....... ...01101 */
                        /* ./loongarch.decode.2:535 */
                        info->opc = OPC_X86RCL_H;
                        return true;
                        break;
                    case 0x0000000e:
                        /* 00000000 00111111 1....... ...01110 */
                        /* ./loongarch.decode.2:536 */
                        info->opc = OPC_X86RCL_W;
                        return true;
                        break;
                    case 0x0000000f:
                        /* 00000000 00111111 1....... ...01111 */
                        /* ./loongarch.decode.2:537 */
                        info->opc = OPC_X86RCL_D;
                        return true;
                        break;
                    }
                    break;
                case 0x00038010:
                    /* 00000000 00111111 1....... ...1.... */
                    switch (insn & 0x0000000f) {
                    case 0x00000000:
                        /* 00000000 00111111 1....... ...10000 */
                        /* ./loongarch.decode.2:538 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86AND_B;
                        return true;
                        break;
                    case 0x00000001:
                        /* 00000000 00111111 1....... ...10001 */
                        /* ./loongarch.decode.2:539 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86AND_H;
                        return true;
                        break;
                    case 0x00000002:
                        /* 00000000 00111111 1....... ...10010 */
                        /* ./loongarch.decode.2:540 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86AND_W;
                        return true;
                        break;
                    case 0x00000003:
                        /* 00000000 00111111 1....... ...10011 */
                        /* ./loongarch.decode.2:541 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86AND_D;
                        return true;
                        break;
                    case 0x00000004:
                        /* 00000000 00111111 1....... ...10100 */
                        /* ./loongarch.decode.2:542 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86OR_B;
                        return true;
                        break;
                    case 0x00000005:
                        /* 00000000 00111111 1....... ...10101 */
                        /* ./loongarch.decode.2:543 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86OR_H;
                        return true;
                        break;
                    case 0x00000006:
                        /* 00000000 00111111 1....... ...10110 */
                        /* ./loongarch.decode.2:544 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86OR_W;
                        return true;
                        break;
                    case 0x00000007:
                        /* 00000000 00111111 1....... ...10111 */
                        /* ./loongarch.decode.2:545 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86OR_D;
                        return true;
                        break;
                    case 0x00000008:
                        /* 00000000 00111111 1....... ...11000 */
                        /* ./loongarch.decode.2:546 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86XOR_B;
                        return true;
                        break;
                    case 0x00000009:
                        /* 00000000 00111111 1....... ...11001 */
                        /* ./loongarch.decode.2:547 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86XOR_H;
                        return true;
                        break;
                    case 0x0000000a:
                        /* 00000000 00111111 1....... ...11010 */
                        /* ./loongarch.decode.2:548 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86XOR_W;
                        return true;
                        break;
                    case 0x0000000b:
                        /* 00000000 00111111 1....... ...11011 */
                        /* ./loongarch.decode.2:549 */
                        decode_extract_format_1(info, insn);
                        info->opc = OPC_X86XOR_D;
                        return true;
                        break;
                    case 0x0000000c:
                        /* 00000000 00111111 1....... ...11100 */
                        decode_extract_format_55(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 00000000 00111111 11...... ...11100 */
                            /* ./loongarch.decode.2:550 */
                            info->opc = OPC_ARMNOT_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000000d:
                        /* 00000000 00111111 1....... ...11101 */
                        decode_extract_format_55(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 00000000 00111111 11...... ...11101 */
                            /* ./loongarch.decode.2:551 */
                            info->opc = OPC_ARMMOV_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000000e:
                        /* 00000000 00111111 1....... ...11110 */
                        decode_extract_format_55(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 00000000 00111111 11...... ...11110 */
                            /* ./loongarch.decode.2:552 */
                            info->opc = OPC_ARMMOV_D;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000000f:
                        /* 00000000 00111111 1....... ...11111 */
                        decode_extract_format_55(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 00000000 00111111 11...... ...11111 */
                            /* ./loongarch.decode.2:553 */
                            info->opc = OPC_ARMRRX_W;
                            return true;
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        case 0x1:
            /* 00000000 01...... ........ ........ */
            switch ((insn >> 21) & 0x1) {
            case 0x0:
                /* 00000000 010..... ........ ........ */
                switch ((insn >> 18) & 0x7) {
                case 0x0:
                    /* 00000000 010000.. ........ ........ */
                    switch ((insn >> 16) & 0x3) {
                    case 0x0:
                        /* 00000000 01000000 ........ ........ */
                        decode_extract_format_6(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01000000 1....... ........ */
                            /* ./loongarch.decode.2:132 */
                            info->opc = OPC_SLLI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 00000000 01000001 ........ ........ */
                        /* ./loongarch.decode.2:133 */
                        decode_extract_format_7(info, insn);
                        info->opc = OPC_SLLI_D;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 00000000 010001.. ........ ........ */
                    switch ((insn >> 16) & 0x3) {
                    case 0x0:
                        /* 00000000 01000100 ........ ........ */
                        decode_extract_format_6(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01000100 1....... ........ */
                            /* ./loongarch.decode.2:134 */
                            info->opc = OPC_SRLI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 00000000 01000101 ........ ........ */
                        /* ./loongarch.decode.2:135 */
                        decode_extract_format_7(info, insn);
                        info->opc = OPC_SRLI_D;
                        return true;
                        break;
                    }
                    break;
                case 0x2:
                    /* 00000000 010010.. ........ ........ */
                    switch ((insn >> 16) & 0x3) {
                    case 0x0:
                        /* 00000000 01001000 ........ ........ */
                        decode_extract_format_6(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01001000 1....... ........ */
                            /* ./loongarch.decode.2:136 */
                            info->opc = OPC_SRAI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 00000000 01001001 ........ ........ */
                        /* ./loongarch.decode.2:137 */
                        decode_extract_format_7(info, insn);
                        info->opc = OPC_SRAI_D;
                        return true;
                        break;
                    }
                    break;
                case 0x3:
                    /* 00000000 010011.. ........ ........ */
                    switch ((insn >> 16) & 0x3) {
                    case 0x0:
                        /* 00000000 01001100 ........ ........ */
                        switch ((insn >> 15) & 0x1) {
                        case 0x0:
                            /* 00000000 01001100 0....... ........ */
                            switch ((insn >> 14) & 0x1) {
                            case 0x0:
                                /* 00000000 01001100 00...... ........ */
                                decode_extract_format_56(info, insn);
                                switch ((insn >> 13) & 0x1) {
                                case 0x1:
                                    /* 00000000 01001100 001..... ........ */
                                    /* ./loongarch.decode.2:554 */
                                    info->opc = OPC_ROTRI_B;
                                    return true;
                                    break;
                                }
                                break;
                            case 0x1:
                                /* 00000000 01001100 01...... ........ */
                                /* ./loongarch.decode.2:555 */
                                decode_extract_format_57(info, insn);
                                info->opc = OPC_ROTRI_H;
                                return true;
                                break;
                            }
                            break;
                        case 0x1:
                            /* 00000000 01001100 1....... ........ */
                            /* ./loongarch.decode.2:138 */
                            decode_extract_format_6(info, insn);
                            info->opc = OPC_ROTRI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 00000000 01001101 ........ ........ */
                        /* ./loongarch.decode.2:139 */
                        decode_extract_format_7(info, insn);
                        info->opc = OPC_ROTRI_D;
                        return true;
                        break;
                    }
                    break;
                case 0x4:
                    /* 00000000 010100.. ........ ........ */
                    switch ((insn >> 16) & 0x3) {
                    case 0x0:
                        /* 00000000 01010000 ........ ........ */
                        switch ((insn >> 15) & 0x1) {
                        case 0x0:
                            /* 00000000 01010000 0....... ........ */
                            switch ((insn >> 14) & 0x1) {
                            case 0x0:
                                /* 00000000 01010000 00...... ........ */
                                decode_extract_format_56(info, insn);
                                switch ((insn >> 13) & 0x1) {
                                case 0x1:
                                    /* 00000000 01010000 001..... ........ */
                                    /* ./loongarch.decode.2:556 */
                                    info->opc = OPC_RCRI_B;
                                    return true;
                                    break;
                                }
                                break;
                            case 0x1:
                                /* 00000000 01010000 01...... ........ */
                                /* ./loongarch.decode.2:557 */
                                decode_extract_format_57(info, insn);
                                info->opc = OPC_RCRI_H;
                                return true;
                                break;
                            }
                            break;
                        case 0x1:
                            /* 00000000 01010000 1....... ........ */
                            /* ./loongarch.decode.2:558 */
                            decode_extract_format_6(info, insn);
                            info->opc = OPC_RCRI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 00000000 01010001 ........ ........ */
                        /* ./loongarch.decode.2:559 */
                        decode_extract_format_7(info, insn);
                        info->opc = OPC_RCRI_D;
                        return true;
                        break;
                    }
                    break;
                case 0x5:
                    /* 00000000 010101.. ........ ........ */
                    switch (insn & 0x0003001f) {
                    case 0x00000000:
                        /* 00000000 01010100 ........ ...00000 */
                        decode_extract_format_58(info, insn);
                        switch ((insn >> 13) & 0x7) {
                        case 0x1:
                            /* 00000000 01010100 001..... ...00000 */
                            /* ./loongarch.decode.2:560 */
                            info->opc = OPC_X86SLLI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000001:
                        /* 00000000 01010100 ........ ...00001 */
                        decode_extract_format_59(info, insn);
                        switch ((insn >> 14) & 0x3) {
                        case 0x1:
                            /* 00000000 01010100 01...... ...00001 */
                            /* ./loongarch.decode.2:561 */
                            info->opc = OPC_X86SLLI_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000002:
                        /* 00000000 01010100 ........ ...00010 */
                        decode_extract_format_60(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01010100 1....... ...00010 */
                            /* ./loongarch.decode.2:562 */
                            info->opc = OPC_X86SLLI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000004:
                        /* 00000000 01010100 ........ ...00100 */
                        decode_extract_format_58(info, insn);
                        switch ((insn >> 13) & 0x7) {
                        case 0x1:
                            /* 00000000 01010100 001..... ...00100 */
                            /* ./loongarch.decode.2:564 */
                            info->opc = OPC_X86SRLI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000005:
                        /* 00000000 01010100 ........ ...00101 */
                        decode_extract_format_59(info, insn);
                        switch ((insn >> 14) & 0x3) {
                        case 0x1:
                            /* 00000000 01010100 01...... ...00101 */
                            /* ./loongarch.decode.2:565 */
                            info->opc = OPC_X86SRLI_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000006:
                        /* 00000000 01010100 ........ ...00110 */
                        decode_extract_format_60(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01010100 1....... ...00110 */
                            /* ./loongarch.decode.2:566 */
                            info->opc = OPC_X86SRLI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000008:
                        /* 00000000 01010100 ........ ...01000 */
                        decode_extract_format_58(info, insn);
                        switch ((insn >> 13) & 0x7) {
                        case 0x1:
                            /* 00000000 01010100 001..... ...01000 */
                            /* ./loongarch.decode.2:568 */
                            info->opc = OPC_X86SRAI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000009:
                        /* 00000000 01010100 ........ ...01001 */
                        decode_extract_format_59(info, insn);
                        switch ((insn >> 14) & 0x3) {
                        case 0x1:
                            /* 00000000 01010100 01...... ...01001 */
                            /* ./loongarch.decode.2:569 */
                            info->opc = OPC_X86SRAI_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000000a:
                        /* 00000000 01010100 ........ ...01010 */
                        decode_extract_format_60(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01010100 1....... ...01010 */
                            /* ./loongarch.decode.2:570 */
                            info->opc = OPC_X86SRAI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000000c:
                        /* 00000000 01010100 ........ ...01100 */
                        decode_extract_format_58(info, insn);
                        switch ((insn >> 13) & 0x7) {
                        case 0x1:
                            /* 00000000 01010100 001..... ...01100 */
                            /* ./loongarch.decode.2:572 */
                            info->opc = OPC_X86ROTRI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000000d:
                        /* 00000000 01010100 ........ ...01101 */
                        decode_extract_format_59(info, insn);
                        switch ((insn >> 14) & 0x3) {
                        case 0x1:
                            /* 00000000 01010100 01...... ...01101 */
                            /* ./loongarch.decode.2:573 */
                            info->opc = OPC_X86ROTRI_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000000e:
                        /* 00000000 01010100 ........ ...01110 */
                        decode_extract_format_60(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01010100 1....... ...01110 */
                            /* ./loongarch.decode.2:574 */
                            info->opc = OPC_X86ROTRI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000010:
                        /* 00000000 01010100 ........ ...10000 */
                        decode_extract_format_58(info, insn);
                        switch ((insn >> 13) & 0x7) {
                        case 0x1:
                            /* 00000000 01010100 001..... ...10000 */
                            /* ./loongarch.decode.2:576 */
                            info->opc = OPC_X86RCRI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000011:
                        /* 00000000 01010100 ........ ...10001 */
                        decode_extract_format_59(info, insn);
                        switch ((insn >> 14) & 0x3) {
                        case 0x1:
                            /* 00000000 01010100 01...... ...10001 */
                            /* ./loongarch.decode.2:577 */
                            info->opc = OPC_X86RCRI_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000012:
                        /* 00000000 01010100 ........ ...10010 */
                        decode_extract_format_60(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01010100 1....... ...10010 */
                            /* ./loongarch.decode.2:578 */
                            info->opc = OPC_X86RCRI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000014:
                        /* 00000000 01010100 ........ ...10100 */
                        decode_extract_format_58(info, insn);
                        switch ((insn >> 13) & 0x7) {
                        case 0x1:
                            /* 00000000 01010100 001..... ...10100 */
                            /* ./loongarch.decode.2:580 */
                            info->opc = OPC_X86ROTLI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000015:
                        /* 00000000 01010100 ........ ...10101 */
                        decode_extract_format_59(info, insn);
                        switch ((insn >> 14) & 0x3) {
                        case 0x1:
                            /* 00000000 01010100 01...... ...10101 */
                            /* ./loongarch.decode.2:581 */
                            info->opc = OPC_X86ROTLI_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000016:
                        /* 00000000 01010100 ........ ...10110 */
                        decode_extract_format_60(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01010100 1....... ...10110 */
                            /* ./loongarch.decode.2:582 */
                            info->opc = OPC_X86ROTLI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000018:
                        /* 00000000 01010100 ........ ...11000 */
                        decode_extract_format_58(info, insn);
                        switch ((insn >> 13) & 0x7) {
                        case 0x1:
                            /* 00000000 01010100 001..... ...11000 */
                            /* ./loongarch.decode.2:584 */
                            info->opc = OPC_X86RCLI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x00000019:
                        /* 00000000 01010100 ........ ...11001 */
                        decode_extract_format_59(info, insn);
                        switch ((insn >> 14) & 0x3) {
                        case 0x1:
                            /* 00000000 01010100 01...... ...11001 */
                            /* ./loongarch.decode.2:585 */
                            info->opc = OPC_X86RCLI_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x0000001a:
                        /* 00000000 01010100 ........ ...11010 */
                        decode_extract_format_60(info, insn);
                        switch ((insn >> 15) & 0x1) {
                        case 0x1:
                            /* 00000000 01010100 1....... ...11010 */
                            /* ./loongarch.decode.2:586 */
                            info->opc = OPC_X86RCLI_W;
                            return true;
                            break;
                        }
                        break;
                    case 0x00010003:
                        /* 00000000 01010101 ........ ...00011 */
                        /* ./loongarch.decode.2:563 */
                        decode_extract_format_61(info, insn);
                        info->opc = OPC_X86SLLI_D;
                        return true;
                        break;
                    case 0x00010007:
                        /* 00000000 01010101 ........ ...00111 */
                        /* ./loongarch.decode.2:567 */
                        decode_extract_format_61(info, insn);
                        info->opc = OPC_X86SRLI_D;
                        return true;
                        break;
                    case 0x0001000b:
                        /* 00000000 01010101 ........ ...01011 */
                        /* ./loongarch.decode.2:571 */
                        decode_extract_format_61(info, insn);
                        info->opc = OPC_X86SRAI_D;
                        return true;
                        break;
                    case 0x0001000f:
                        /* 00000000 01010101 ........ ...01111 */
                        /* ./loongarch.decode.2:575 */
                        decode_extract_format_61(info, insn);
                        info->opc = OPC_X86ROTRI_D;
                        return true;
                        break;
                    case 0x00010013:
                        /* 00000000 01010101 ........ ...10011 */
                        /* ./loongarch.decode.2:579 */
                        decode_extract_format_61(info, insn);
                        info->opc = OPC_X86RCRI_D;
                        return true;
                        break;
                    case 0x00010017:
                        /* 00000000 01010101 ........ ...10111 */
                        /* ./loongarch.decode.2:583 */
                        decode_extract_format_61(info, insn);
                        info->opc = OPC_X86ROTLI_D;
                        return true;
                        break;
                    case 0x0001001b:
                        /* 00000000 01010101 ........ ...11011 */
                        /* ./loongarch.decode.2:587 */
                        decode_extract_format_61(info, insn);
                        info->opc = OPC_X86RCLI_D;
                        return true;
                        break;
                    }
                    break;
                case 0x6:
                    /* 00000000 010110.. ........ ........ */
                    /* ./loongarch.decode.2:588 */
                    decode_extract_format_62(info, insn);
                    info->opc = OPC_X86SETTAG;
                    return true;
                    break;
                case 0x7:
                    /* 00000000 010111.. ........ ........ */
                    decode_extract_format_63(info, insn);
                    switch ((insn >> 5) & 0x1f) {
                    case 0x0:
                        /* 00000000 010111.. ......00 000..... */
                        /* ./loongarch.decode.2:589 */
                        info->opc = OPC_X86MFFLAG;
                        return true;
                        break;
                    case 0x1:
                        /* 00000000 010111.. ......00 001..... */
                        /* ./loongarch.decode.2:590 */
                        info->opc = OPC_X86MTFLAG;
                        return true;
                        break;
                    case 0x2:
                        /* 00000000 010111.. ......00 010..... */
                        /* ./loongarch.decode.2:591 */
                        info->opc = OPC_ARMMFFLAG;
                        return true;
                        break;
                    case 0x3:
                        /* 00000000 010111.. ......00 011..... */
                        /* ./loongarch.decode.2:592 */
                        info->opc = OPC_ARMMTFLAG;
                        return true;
                        break;
                    }
                    break;
                }
                break;
            case 0x1:
                /* 00000000 011..... ........ ........ */
                decode_extract_format_8(info, insn);
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 00000000 011..... 0....... ........ */
                    /* ./loongarch.decode.2:140 */
                    info->opc = OPC_BSTRINS_W;
                    return true;
                    break;
                case 0x1:
                    /* 00000000 011..... 1....... ........ */
                    /* ./loongarch.decode.2:141 */
                    info->opc = OPC_BSTRPICK_W;
                    return true;
                    break;
                }
                break;
            }
            break;
        case 0x2:
            /* 00000000 10...... ........ ........ */
            /* ./loongarch.decode.2:142 */
            decode_extract_format_9(info, insn);
            info->opc = OPC_BSTRINS_D;
            return true;
            break;
        case 0x3:
            /* 00000000 11...... ........ ........ */
            /* ./loongarch.decode.2:143 */
            decode_extract_format_9(info, insn);
            info->opc = OPC_BSTRPICK_D;
            return true;
            break;
        case 0x4:
            /* 00000001 00...... ........ ........ */
            switch ((insn >> 15) & 0x7f) {
            case 0x1:
                /* 00000001 00000000 1....... ........ */
                /* ./loongarch.decode.2:144 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FADD_S;
                return true;
                break;
            case 0x2:
                /* 00000001 00000001 0....... ........ */
                /* ./loongarch.decode.2:145 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FADD_D;
                return true;
                break;
            case 0x5:
                /* 00000001 00000010 1....... ........ */
                /* ./loongarch.decode.2:146 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FSUB_S;
                return true;
                break;
            case 0x6:
                /* 00000001 00000011 0....... ........ */
                /* ./loongarch.decode.2:147 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FSUB_D;
                return true;
                break;
            case 0x9:
                /* 00000001 00000100 1....... ........ */
                /* ./loongarch.decode.2:148 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMUL_S;
                return true;
                break;
            case 0xa:
                /* 00000001 00000101 0....... ........ */
                /* ./loongarch.decode.2:149 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMUL_D;
                return true;
                break;
            case 0xd:
                /* 00000001 00000110 1....... ........ */
                /* ./loongarch.decode.2:150 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FDIV_S;
                return true;
                break;
            case 0xe:
                /* 00000001 00000111 0....... ........ */
                /* ./loongarch.decode.2:151 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FDIV_D;
                return true;
                break;
            case 0x11:
                /* 00000001 00001000 1....... ........ */
                /* ./loongarch.decode.2:152 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMAX_S;
                return true;
                break;
            case 0x12:
                /* 00000001 00001001 0....... ........ */
                /* ./loongarch.decode.2:153 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMAX_D;
                return true;
                break;
            case 0x15:
                /* 00000001 00001010 1....... ........ */
                /* ./loongarch.decode.2:154 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMIN_S;
                return true;
                break;
            case 0x16:
                /* 00000001 00001011 0....... ........ */
                /* ./loongarch.decode.2:155 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMIN_D;
                return true;
                break;
            case 0x19:
                /* 00000001 00001100 1....... ........ */
                /* ./loongarch.decode.2:156 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMAXA_S;
                return true;
                break;
            case 0x1a:
                /* 00000001 00001101 0....... ........ */
                /* ./loongarch.decode.2:157 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMAXA_D;
                return true;
                break;
            case 0x1d:
                /* 00000001 00001110 1....... ........ */
                /* ./loongarch.decode.2:158 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMINA_S;
                return true;
                break;
            case 0x1e:
                /* 00000001 00001111 0....... ........ */
                /* ./loongarch.decode.2:159 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FMINA_D;
                return true;
                break;
            case 0x21:
                /* 00000001 00010000 1....... ........ */
                /* ./loongarch.decode.2:160 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FSCALEB_S;
                return true;
                break;
            case 0x22:
                /* 00000001 00010001 0....... ........ */
                /* ./loongarch.decode.2:161 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FSCALEB_D;
                return true;
                break;
            case 0x25:
                /* 00000001 00010010 1....... ........ */
                /* ./loongarch.decode.2:162 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FCOPYSIGN_S;
                return true;
                break;
            case 0x26:
                /* 00000001 00010011 0....... ........ */
                /* ./loongarch.decode.2:163 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FCOPYSIGN_D;
                return true;
                break;
            case 0x28:
                /* 00000001 00010100 0....... ........ */
                decode_extract_format_11(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x1:
                    /* 00000001 00010100 000001.. ........ */
                    /* ./loongarch.decode.2:164 */
                    info->opc = OPC_FABS_S;
                    return true;
                    break;
                case 0x2:
                    /* 00000001 00010100 000010.. ........ */
                    /* ./loongarch.decode.2:165 */
                    info->opc = OPC_FABS_D;
                    return true;
                    break;
                case 0x5:
                    /* 00000001 00010100 000101.. ........ */
                    /* ./loongarch.decode.2:166 */
                    info->opc = OPC_FNEG_S;
                    return true;
                    break;
                case 0x6:
                    /* 00000001 00010100 000110.. ........ */
                    /* ./loongarch.decode.2:167 */
                    info->opc = OPC_FNEG_D;
                    return true;
                    break;
                case 0x9:
                    /* 00000001 00010100 001001.. ........ */
                    /* ./loongarch.decode.2:168 */
                    info->opc = OPC_FLOGB_S;
                    return true;
                    break;
                case 0xa:
                    /* 00000001 00010100 001010.. ........ */
                    /* ./loongarch.decode.2:169 */
                    info->opc = OPC_FLOGB_D;
                    return true;
                    break;
                case 0xd:
                    /* 00000001 00010100 001101.. ........ */
                    /* ./loongarch.decode.2:170 */
                    info->opc = OPC_FCLASS_S;
                    return true;
                    break;
                case 0xe:
                    /* 00000001 00010100 001110.. ........ */
                    /* ./loongarch.decode.2:171 */
                    info->opc = OPC_FCLASS_D;
                    return true;
                    break;
                case 0x11:
                    /* 00000001 00010100 010001.. ........ */
                    /* ./loongarch.decode.2:172 */
                    info->opc = OPC_FSQRT_S;
                    return true;
                    break;
                case 0x12:
                    /* 00000001 00010100 010010.. ........ */
                    /* ./loongarch.decode.2:173 */
                    info->opc = OPC_FSQRT_D;
                    return true;
                    break;
                case 0x15:
                    /* 00000001 00010100 010101.. ........ */
                    /* ./loongarch.decode.2:174 */
                    info->opc = OPC_FRECIP_S;
                    return true;
                    break;
                case 0x16:
                    /* 00000001 00010100 010110.. ........ */
                    /* ./loongarch.decode.2:175 */
                    info->opc = OPC_FRECIP_D;
                    return true;
                    break;
                case 0x19:
                    /* 00000001 00010100 011001.. ........ */
                    /* ./loongarch.decode.2:176 */
                    info->opc = OPC_FRSQRT_S;
                    return true;
                    break;
                case 0x1a:
                    /* 00000001 00010100 011010.. ........ */
                    /* ./loongarch.decode.2:177 */
                    info->opc = OPC_FRSQRT_D;
                    return true;
                    break;
                }
                break;
            case 0x29:
                /* 00000001 00010100 1....... ........ */
                switch ((insn >> 10) & 0x1f) {
                case 0x5:
                    /* 00000001 00010100 100101.. ........ */
                    /* ./loongarch.decode.2:178 */
                    decode_extract_format_11(info, insn);
                    info->opc = OPC_FMOV_S;
                    return true;
                    break;
                case 0x6:
                    /* 00000001 00010100 100110.. ........ */
                    /* ./loongarch.decode.2:179 */
                    decode_extract_format_11(info, insn);
                    info->opc = OPC_FMOV_D;
                    return true;
                    break;
                case 0x9:
                    /* 00000001 00010100 101001.. ........ */
                    /* ./loongarch.decode.2:180 */
                    decode_extract_format_12(info, insn);
                    info->opc = OPC_MOVGR2FR_W;
                    return true;
                    break;
                case 0xa:
                    /* 00000001 00010100 101010.. ........ */
                    /* ./loongarch.decode.2:181 */
                    decode_extract_format_12(info, insn);
                    info->opc = OPC_MOVGR2FR_D;
                    return true;
                    break;
                case 0xb:
                    /* 00000001 00010100 101011.. ........ */
                    /* ./loongarch.decode.2:182 */
                    decode_extract_format_12(info, insn);
                    info->opc = OPC_MOVGR2FRH_W;
                    return true;
                    break;
                case 0xd:
                    /* 00000001 00010100 101101.. ........ */
                    /* ./loongarch.decode.2:183 */
                    decode_extract_format_13(info, insn);
                    info->opc = OPC_MOVFR2GR_S;
                    return true;
                    break;
                case 0xe:
                    /* 00000001 00010100 101110.. ........ */
                    /* ./loongarch.decode.2:184 */
                    decode_extract_format_13(info, insn);
                    info->opc = OPC_MOVFR2GR_D;
                    return true;
                    break;
                case 0xf:
                    /* 00000001 00010100 101111.. ........ */
                    /* ./loongarch.decode.2:185 */
                    decode_extract_format_13(info, insn);
                    info->opc = OPC_MOVFRH2GR_S;
                    return true;
                    break;
                case 0x10:
                    /* 00000001 00010100 110000.. ........ */
                    /* ./loongarch.decode.2:186 */
                    decode_extract_format_14(info, insn);
                    info->opc = OPC_MOVGR2FCSR;
                    return true;
                    break;
                case 0x12:
                    /* 00000001 00010100 110010.. ........ */
                    /* ./loongarch.decode.2:187 */
                    decode_extract_format_15(info, insn);
                    info->opc = OPC_MOVFCSR2GR;
                    return true;
                    break;
                case 0x14:
                    /* 00000001 00010100 110100.. ........ */
                    decode_extract_format_16(info, insn);
                    switch ((insn >> 3) & 0x3) {
                    case 0x0:
                        /* 00000001 00010100 110100.. ...00... */
                        /* ./loongarch.decode.2:188 */
                        info->opc = OPC_MOVFR2CF;
                        return true;
                        break;
                    }
                    break;
                case 0x15:
                    /* 00000001 00010100 110101.. ........ */
                    decode_extract_format_17(info, insn);
                    switch ((insn >> 8) & 0x3) {
                    case 0x0:
                        /* 00000001 00010100 11010100 ........ */
                        /* ./loongarch.decode.2:189 */
                        info->opc = OPC_MOVCF2FR;
                        return true;
                        break;
                    }
                    break;
                case 0x16:
                    /* 00000001 00010100 110110.. ........ */
                    decode_extract_format_18(info, insn);
                    switch ((insn >> 3) & 0x3) {
                    case 0x0:
                        /* 00000001 00010100 110110.. ...00... */
                        /* ./loongarch.decode.2:190 */
                        info->opc = OPC_MOVGR2CF;
                        return true;
                        break;
                    }
                    break;
                case 0x17:
                    /* 00000001 00010100 110111.. ........ */
                    decode_extract_format_19(info, insn);
                    switch ((insn >> 8) & 0x3) {
                    case 0x0:
                        /* 00000001 00010100 11011100 ........ */
                        /* ./loongarch.decode.2:191 */
                        info->opc = OPC_MOVCF2GR;
                        return true;
                        break;
                    }
                    break;
                case 0x18:
                    /* 00000001 00010100 111000.. ........ */
                    /* ./loongarch.decode.2:593 */
                    decode_extract_format_11(info, insn);
                    info->opc = OPC_FCVT_LD_D;
                    return true;
                    break;
                case 0x19:
                    /* 00000001 00010100 111001.. ........ */
                    /* ./loongarch.decode.2:594 */
                    decode_extract_format_11(info, insn);
                    info->opc = OPC_FCVT_UD_D;
                    return true;
                    break;
                }
                break;
            case 0x2a:
                /* 00000001 00010101 0....... ........ */
                /* ./loongarch.decode.2:595 */
                decode_extract_format_10(info, insn);
                info->opc = OPC_FCVT_D_LC;
                return true;
                break;
            case 0x32:
                /* 00000001 00011001 0....... ........ */
                decode_extract_format_11(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x6:
                    /* 00000001 00011001 000110.. ........ */
                    /* ./loongarch.decode.2:192 */
                    info->opc = OPC_FCVT_S_D;
                    return true;
                    break;
                case 0x9:
                    /* 00000001 00011001 001001.. ........ */
                    /* ./loongarch.decode.2:193 */
                    info->opc = OPC_FCVT_D_S;
                    return true;
                    break;
                }
                break;
            case 0x34:
                /* 00000001 00011010 0....... ........ */
                decode_extract_format_11(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x1:
                    /* 00000001 00011010 000001.. ........ */
                    /* ./loongarch.decode.2:194 */
                    info->opc = OPC_FTINTRM_W_S;
                    return true;
                    break;
                case 0x2:
                    /* 00000001 00011010 000010.. ........ */
                    /* ./loongarch.decode.2:195 */
                    info->opc = OPC_FTINTRM_W_D;
                    return true;
                    break;
                case 0x9:
                    /* 00000001 00011010 001001.. ........ */
                    /* ./loongarch.decode.2:196 */
                    info->opc = OPC_FTINTRM_L_S;
                    return true;
                    break;
                case 0xa:
                    /* 00000001 00011010 001010.. ........ */
                    /* ./loongarch.decode.2:197 */
                    info->opc = OPC_FTINTRM_L_D;
                    return true;
                    break;
                case 0x11:
                    /* 00000001 00011010 010001.. ........ */
                    /* ./loongarch.decode.2:198 */
                    info->opc = OPC_FTINTRP_W_S;
                    return true;
                    break;
                case 0x12:
                    /* 00000001 00011010 010010.. ........ */
                    /* ./loongarch.decode.2:199 */
                    info->opc = OPC_FTINTRP_W_D;
                    return true;
                    break;
                case 0x19:
                    /* 00000001 00011010 011001.. ........ */
                    /* ./loongarch.decode.2:200 */
                    info->opc = OPC_FTINTRP_L_S;
                    return true;
                    break;
                case 0x1a:
                    /* 00000001 00011010 011010.. ........ */
                    /* ./loongarch.decode.2:201 */
                    info->opc = OPC_FTINTRP_L_D;
                    return true;
                    break;
                }
                break;
            case 0x35:
                /* 00000001 00011010 1....... ........ */
                decode_extract_format_11(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x1:
                    /* 00000001 00011010 100001.. ........ */
                    /* ./loongarch.decode.2:202 */
                    info->opc = OPC_FTINTRZ_W_S;
                    return true;
                    break;
                case 0x2:
                    /* 00000001 00011010 100010.. ........ */
                    /* ./loongarch.decode.2:203 */
                    info->opc = OPC_FTINTRZ_W_D;
                    return true;
                    break;
                case 0x9:
                    /* 00000001 00011010 101001.. ........ */
                    /* ./loongarch.decode.2:204 */
                    info->opc = OPC_FTINTRZ_L_S;
                    return true;
                    break;
                case 0xa:
                    /* 00000001 00011010 101010.. ........ */
                    /* ./loongarch.decode.2:205 */
                    info->opc = OPC_FTINTRZ_L_D;
                    return true;
                    break;
                case 0x11:
                    /* 00000001 00011010 110001.. ........ */
                    /* ./loongarch.decode.2:206 */
                    info->opc = OPC_FTINTRNE_W_S;
                    return true;
                    break;
                case 0x12:
                    /* 00000001 00011010 110010.. ........ */
                    /* ./loongarch.decode.2:207 */
                    info->opc = OPC_FTINTRNE_W_D;
                    return true;
                    break;
                case 0x19:
                    /* 00000001 00011010 111001.. ........ */
                    /* ./loongarch.decode.2:208 */
                    info->opc = OPC_FTINTRNE_L_S;
                    return true;
                    break;
                case 0x1a:
                    /* 00000001 00011010 111010.. ........ */
                    /* ./loongarch.decode.2:209 */
                    info->opc = OPC_FTINTRNE_L_D;
                    return true;
                    break;
                }
                break;
            case 0x36:
                /* 00000001 00011011 0....... ........ */
                decode_extract_format_11(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x1:
                    /* 00000001 00011011 000001.. ........ */
                    /* ./loongarch.decode.2:210 */
                    info->opc = OPC_FTINT_W_S;
                    return true;
                    break;
                case 0x2:
                    /* 00000001 00011011 000010.. ........ */
                    /* ./loongarch.decode.2:211 */
                    info->opc = OPC_FTINT_W_D;
                    return true;
                    break;
                case 0x9:
                    /* 00000001 00011011 001001.. ........ */
                    /* ./loongarch.decode.2:212 */
                    info->opc = OPC_FTINT_L_S;
                    return true;
                    break;
                case 0xa:
                    /* 00000001 00011011 001010.. ........ */
                    /* ./loongarch.decode.2:213 */
                    info->opc = OPC_FTINT_L_D;
                    return true;
                    break;
                }
                break;
            case 0x3a:
                /* 00000001 00011101 0....... ........ */
                decode_extract_format_11(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x4:
                    /* 00000001 00011101 000100.. ........ */
                    /* ./loongarch.decode.2:214 */
                    info->opc = OPC_FFINT_S_W;
                    return true;
                    break;
                case 0x6:
                    /* 00000001 00011101 000110.. ........ */
                    /* ./loongarch.decode.2:215 */
                    info->opc = OPC_FFINT_S_L;
                    return true;
                    break;
                case 0x8:
                    /* 00000001 00011101 001000.. ........ */
                    /* ./loongarch.decode.2:216 */
                    info->opc = OPC_FFINT_D_W;
                    return true;
                    break;
                case 0xa:
                    /* 00000001 00011101 001010.. ........ */
                    /* ./loongarch.decode.2:217 */
                    info->opc = OPC_FFINT_D_L;
                    return true;
                    break;
                }
                break;
            case 0x3c:
                /* 00000001 00011110 0....... ........ */
                decode_extract_format_11(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x11:
                    /* 00000001 00011110 010001.. ........ */
                    /* ./loongarch.decode.2:218 */
                    info->opc = OPC_FRINT_S;
                    return true;
                    break;
                case 0x12:
                    /* 00000001 00011110 010010.. ........ */
                    /* ./loongarch.decode.2:219 */
                    info->opc = OPC_FRINT_D;
                    return true;
                    break;
                }
                break;
            }
            break;
        case 0x8:
            /* 00000010 00...... ........ ........ */
            /* ./loongarch.decode.2:220 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_SLTI;
            return true;
            break;
        case 0x9:
            /* 00000010 01...... ........ ........ */
            /* ./loongarch.decode.2:221 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_SLTUI;
            return true;
            break;
        case 0xa:
            /* 00000010 10...... ........ ........ */
            /* ./loongarch.decode.2:222 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_ADDI_W;
            return true;
            break;
        case 0xb:
            /* 00000010 11...... ........ ........ */
            /* ./loongarch.decode.2:223 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_ADDI_D;
            return true;
            break;
        case 0xc:
            /* 00000011 00...... ........ ........ */
            /* ./loongarch.decode.2:224 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LU52I_D;
            return true;
            break;
        case 0xd:
            /* 00000011 01...... ........ ........ */
            /* ./loongarch.decode.2:225 */
            decode_extract_format_21(info, insn);
            info->opc = OPC_ANDI;
            return true;
            break;
        case 0xe:
            /* 00000011 10...... ........ ........ */
            /* ./loongarch.decode.2:226 */
            decode_extract_format_21(info, insn);
            info->opc = OPC_ORI;
            return true;
            break;
        case 0xf:
            /* 00000011 11...... ........ ........ */
            /* ./loongarch.decode.2:227 */
            decode_extract_format_21(info, insn);
            info->opc = OPC_XORI;
            return true;
            break;
        }
        break;
    case 0x1:
        /* 000001.. ........ ........ ........ */
        switch ((insn >> 24) & 0x3) {
        case 0x0:
            /* 00000100 ........ ........ ........ */
            if ((insn & 0x000003e0) == 0x00000000) {
                /* 00000100 ........ ......00 000..... */
                /* ./loongarch.decode.2:229 */
                decode_extract_format_22(info, insn);
                info->opc = OPC_CSRRD;
                return true;
            }
            if ((insn & 0x000003e0) == 0x00000020) {
                /* 00000100 ........ ......00 001..... */
                /* ./loongarch.decode.2:230 */
                decode_extract_format_22(info, insn);
                info->opc = OPC_CSRWR;
                return true;
            }
            /* ./loongarch.decode.2:231 */
            decode_extract_format_23(info, insn);
            info->opc = OPC_CSRXCHG;
            return true;
            break;
        case 0x1:
            /* 00000101 ........ ........ ........ */
            if ((insn & 0x000003e0) == 0x00000000) {
                /* 00000101 ........ ......00 000..... */
                /* ./loongarch.decode.2:597 */
                decode_extract_format_22(info, insn);
                info->opc = OPC_GCSRRD;
                return true;
            }
            if ((insn & 0x000003e0) == 0x00000020) {
                /* 00000101 ........ ......00 001..... */
                /* ./loongarch.decode.2:598 */
                decode_extract_format_22(info, insn);
                info->opc = OPC_GCSRWR;
                return true;
            }
            /* ./loongarch.decode.2:599 */
            decode_extract_format_23(info, insn);
            info->opc = OPC_GCSRXCHG;
            return true;
            break;
        case 0x2:
            /* 00000110 ........ ........ ........ */
            switch ((insn >> 22) & 0x3) {
            case 0x0:
                /* 00000110 00...... ........ ........ */
                /* ./loongarch.decode.2:233 */
                decode_extract_format_24(info, insn);
                info->opc = OPC_CACOP;
                return true;
                break;
            case 0x1:
                /* 00000110 01...... ........ ........ */
                switch ((insn >> 18) & 0xf) {
                case 0x0:
                    /* 00000110 010000.. ........ ........ */
                    /* ./loongarch.decode.2:234 */
                    decode_extract_format_25(info, insn);
                    info->opc = OPC_LDDIR;
                    return true;
                    break;
                case 0x1:
                    /* 00000110 010001.. ........ ........ */
                    decode_extract_format_26(info, insn);
                    switch (insn & 0x0000001f) {
                    case 0x00000000:
                        /* 00000110 010001.. ........ ...00000 */
                        /* ./loongarch.decode.2:235 */
                        info->opc = OPC_LDPTE;
                        return true;
                        break;
                    }
                    break;
                case 0x2:
                    /* 00000110 010010.. ........ ........ */
                    switch ((insn >> 15) & 0x7) {
                    case 0x0:
                        /* 00000110 01001000 0....... ........ */
                        switch ((insn >> 10) & 0x1f) {
                        case 0x0:
                            /* 00000110 01001000 000000.. ........ */
                            /* ./loongarch.decode.2:236 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRRD_B;
                            return true;
                            break;
                        case 0x1:
                            /* 00000110 01001000 000001.. ........ */
                            /* ./loongarch.decode.2:237 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRRD_H;
                            return true;
                            break;
                        case 0x2:
                            /* 00000110 01001000 000010.. ........ */
                            /* ./loongarch.decode.2:238 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRRD_W;
                            return true;
                            break;
                        case 0x3:
                            /* 00000110 01001000 000011.. ........ */
                            /* ./loongarch.decode.2:239 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRRD_D;
                            return true;
                            break;
                        case 0x4:
                            /* 00000110 01001000 000100.. ........ */
                            /* ./loongarch.decode.2:240 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRWR_B;
                            return true;
                            break;
                        case 0x5:
                            /* 00000110 01001000 000101.. ........ */
                            /* ./loongarch.decode.2:241 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRWR_H;
                            return true;
                            break;
                        case 0x6:
                            /* 00000110 01001000 000110.. ........ */
                            /* ./loongarch.decode.2:242 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRWR_W;
                            return true;
                            break;
                        case 0x7:
                            /* 00000110 01001000 000111.. ........ */
                            /* ./loongarch.decode.2:243 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_IOCSRWR_D;
                            return true;
                            break;
                        case 0x8:
                            /* 00000110 01001000 001000.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001000 00100000 00000000 */
                                /* ./loongarch.decode.2:244 */
                                info->opc = OPC_TLBCLR;
                                return true;
                                break;
                            case 0x00000001:
                                /* 00000110 01001000 00100000 00000001 */
                                /* ./loongarch.decode.2:601 */
                                info->opc = OPC_GTLBCLR;
                                return true;
                                break;
                            }
                            break;
                        case 0x9:
                            /* 00000110 01001000 001001.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001000 00100100 00000000 */
                                /* ./loongarch.decode.2:245 */
                                info->opc = OPC_TLBFLUSH;
                                return true;
                                break;
                            case 0x00000001:
                                /* 00000110 01001000 00100100 00000001 */
                                /* ./loongarch.decode.2:602 */
                                info->opc = OPC_GTLBFLUSH;
                                return true;
                                break;
                            }
                            break;
                        case 0xa:
                            /* 00000110 01001000 001010.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001000 00101000 00000000 */
                                /* ./loongarch.decode.2:246 */
                                info->opc = OPC_TLBSRCH;
                                return true;
                                break;
                            case 0x00000001:
                                /* 00000110 01001000 00101000 00000001 */
                                /* ./loongarch.decode.2:603 */
                                info->opc = OPC_GTLBSRCH;
                                return true;
                                break;
                            }
                            break;
                        case 0xb:
                            /* 00000110 01001000 001011.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001000 00101100 00000000 */
                                /* ./loongarch.decode.2:247 */
                                info->opc = OPC_TLBRD;
                                return true;
                                break;
                            case 0x00000001:
                                /* 00000110 01001000 00101100 00000001 */
                                /* ./loongarch.decode.2:604 */
                                info->opc = OPC_GTLBRD;
                                return true;
                                break;
                            }
                            break;
                        case 0xc:
                            /* 00000110 01001000 001100.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001000 00110000 00000000 */
                                /* ./loongarch.decode.2:248 */
                                info->opc = OPC_TLBWR;
                                return true;
                                break;
                            case 0x00000001:
                                /* 00000110 01001000 00110000 00000001 */
                                /* ./loongarch.decode.2:605 */
                                info->opc = OPC_GTLBWR;
                                return true;
                                break;
                            }
                            break;
                        case 0xd:
                            /* 00000110 01001000 001101.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001000 00110100 00000000 */
                                /* ./loongarch.decode.2:249 */
                                info->opc = OPC_TLBFILL;
                                return true;
                                break;
                            case 0x00000001:
                                /* 00000110 01001000 00110100 00000001 */
                                /* ./loongarch.decode.2:606 */
                                info->opc = OPC_GTLBFILL;
                                return true;
                                break;
                            }
                            break;
                        case 0xe:
                            /* 00000110 01001000 001110.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001000 00111000 00000000 */
                                /* ./loongarch.decode.2:250 */
                                info->opc = OPC_ERTN;
                                return true;
                                break;
                            }
                            break;
                        }
                        break;
                    case 0x1:
                        /* 00000110 01001000 1....... ........ */
                        /* ./loongarch.decode.2:251 */
                        decode_extract_format_28(info, insn);
                        info->opc = OPC_IDLE;
                        return true;
                        break;
                    case 0x2:
                        /* 00000110 01001001 0....... ........ */
                        switch ((insn >> 10) & 0x1f) {
                        case 0x8:
                            /* 00000110 01001001 001000.. ........ */
                            /* ./loongarch.decode.2:607 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_SCFGR_W;
                            return true;
                            break;
                        case 0x9:
                            /* 00000110 01001001 001001.. ........ */
                            /* ./loongarch.decode.2:608 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_SCFGR_D;
                            return true;
                            break;
                        case 0xa:
                            /* 00000110 01001001 001010.. ........ */
                            /* ./loongarch.decode.2:609 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_SCFGW_W;
                            return true;
                            break;
                        case 0xb:
                            /* 00000110 01001001 001011.. ........ */
                            /* ./loongarch.decode.2:610 */
                            decode_extract_format_0(info, insn);
                            info->opc = OPC_SCFGW_D;
                            return true;
                            break;
                        case 0xc:
                            /* 00000110 01001001 001100.. ........ */
                            decode_extract_format_27(info, insn);
                            switch (insn & 0x000003ff) {
                            case 0x00000000:
                                /* 00000110 01001001 00110000 00000000 */
                                /* ./loongarch.decode.2:611 */
                                info->opc = OPC_SNOP;
                                return true;
                                break;
                            }
                            break;
                        }
                        break;
                    case 0x3:
                        /* 00000110 01001001 1....... ........ */
                        /* ./loongarch.decode.2:252 */
                        decode_extract_format_29(info, insn);
                        info->opc = OPC_INVTLB;
                        return true;
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 0x2:
        /* 000010.. ........ ........ ........ */
        switch ((insn >> 20) & 0x3f) {
        case 0x1:
            /* 00001000 0001.... ........ ........ */
            /* ./loongarch.decode.2:253 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FMADD_S;
            return true;
            break;
        case 0x2:
            /* 00001000 0010.... ........ ........ */
            /* ./loongarch.decode.2:254 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FMADD_D;
            return true;
            break;
        case 0x5:
            /* 00001000 0101.... ........ ........ */
            /* ./loongarch.decode.2:255 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FMSUB_S;
            return true;
            break;
        case 0x6:
            /* 00001000 0110.... ........ ........ */
            /* ./loongarch.decode.2:256 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FMSUB_D;
            return true;
            break;
        case 0x9:
            /* 00001000 1001.... ........ ........ */
            /* ./loongarch.decode.2:257 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FNMADD_S;
            return true;
            break;
        case 0xa:
            /* 00001000 1010.... ........ ........ */
            /* ./loongarch.decode.2:258 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FNMADD_D;
            return true;
            break;
        case 0xd:
            /* 00001000 1101.... ........ ........ */
            /* ./loongarch.decode.2:259 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FNMSUB_S;
            return true;
            break;
        case 0xe:
            /* 00001000 1110.... ........ ........ */
            /* ./loongarch.decode.2:260 */
            decode_extract_format_30(info, insn);
            info->opc = OPC_FNMSUB_D;
            return true;
            break;
        case 0x11:
            /* 00001001 0001.... ........ ........ */
            /* ./loongarch.decode.2:1213 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFMADD_S;
            return true;
            break;
        case 0x12:
            /* 00001001 0010.... ........ ........ */
            /* ./loongarch.decode.2:1214 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFMADD_D;
            return true;
            break;
        case 0x15:
            /* 00001001 0101.... ........ ........ */
            /* ./loongarch.decode.2:1215 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFMSUB_S;
            return true;
            break;
        case 0x16:
            /* 00001001 0110.... ........ ........ */
            /* ./loongarch.decode.2:1216 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFMSUB_D;
            return true;
            break;
        case 0x19:
            /* 00001001 1001.... ........ ........ */
            /* ./loongarch.decode.2:1217 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFNMADD_S;
            return true;
            break;
        case 0x1a:
            /* 00001001 1010.... ........ ........ */
            /* ./loongarch.decode.2:1218 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFNMADD_D;
            return true;
            break;
        case 0x1d:
            /* 00001001 1101.... ........ ........ */
            /* ./loongarch.decode.2:1219 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFNMSUB_S;
            return true;
            break;
        case 0x1e:
            /* 00001001 1110.... ........ ........ */
            /* ./loongarch.decode.2:1220 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VFNMSUB_D;
            return true;
            break;
        }
        break;
    case 0x3:
        /* 000011.. ........ ........ ........ */
        switch ((insn >> 20) & 0x3f) {
        case 0x1:
            /* 00001100 0001.... ........ ........ */
            decode_extract_format_31(info, insn);
            switch (insn & 0x000f8018) {
            case 0x00000000:
                /* 00001100 00010000 0....... ...00... */
                /* ./loongarch.decode.2:261 */
                info->opc = OPC_FCMP_CAF_S;
                return true;
                break;
            case 0x00008000:
                /* 00001100 00010000 1....... ...00... */
                /* ./loongarch.decode.2:262 */
                info->opc = OPC_FCMP_SAF_S;
                return true;
                break;
            case 0x00010000:
                /* 00001100 00010001 0....... ...00... */
                /* ./loongarch.decode.2:263 */
                info->opc = OPC_FCMP_CLT_S;
                return true;
                break;
            case 0x00018000:
                /* 00001100 00010001 1....... ...00... */
                /* ./loongarch.decode.2:264 */
                info->opc = OPC_FCMP_STL_S;
                return true;
                break;
            case 0x00020000:
                /* 00001100 00010010 0....... ...00... */
                /* ./loongarch.decode.2:265 */
                info->opc = OPC_FCMP_CEQ_S;
                return true;
                break;
            case 0x00028000:
                /* 00001100 00010010 1....... ...00... */
                /* ./loongarch.decode.2:266 */
                info->opc = OPC_FCMP_SEQ_S;
                return true;
                break;
            case 0x00030000:
                /* 00001100 00010011 0....... ...00... */
                /* ./loongarch.decode.2:267 */
                info->opc = OPC_FCMP_CLE_S;
                return true;
                break;
            case 0x00038000:
                /* 00001100 00010011 1....... ...00... */
                /* ./loongarch.decode.2:268 */
                info->opc = OPC_FCMP_SLE_S;
                return true;
                break;
            case 0x00040000:
                /* 00001100 00010100 0....... ...00... */
                /* ./loongarch.decode.2:269 */
                info->opc = OPC_FCMP_CUN_S;
                return true;
                break;
            case 0x00048000:
                /* 00001100 00010100 1....... ...00... */
                /* ./loongarch.decode.2:270 */
                info->opc = OPC_FCMP_SUN_S;
                return true;
                break;
            case 0x00050000:
                /* 00001100 00010101 0....... ...00... */
                /* ./loongarch.decode.2:271 */
                info->opc = OPC_FCMP_CULT_S;
                return true;
                break;
            case 0x00058000:
                /* 00001100 00010101 1....... ...00... */
                /* ./loongarch.decode.2:272 */
                info->opc = OPC_FCMP_SULT_S;
                return true;
                break;
            case 0x00060000:
                /* 00001100 00010110 0....... ...00... */
                /* ./loongarch.decode.2:273 */
                info->opc = OPC_FCMP_CUEQ_S;
                return true;
                break;
            case 0x00068000:
                /* 00001100 00010110 1....... ...00... */
                /* ./loongarch.decode.2:274 */
                info->opc = OPC_FCMP_SUEQ_S;
                return true;
                break;
            case 0x00070000:
                /* 00001100 00010111 0....... ...00... */
                /* ./loongarch.decode.2:275 */
                info->opc = OPC_FCMP_CULE_S;
                return true;
                break;
            case 0x00078000:
                /* 00001100 00010111 1....... ...00... */
                /* ./loongarch.decode.2:276 */
                info->opc = OPC_FCMP_SULE_S;
                return true;
                break;
            case 0x00080000:
                /* 00001100 00011000 0....... ...00... */
                /* ./loongarch.decode.2:277 */
                info->opc = OPC_FCMP_CNE_S;
                return true;
                break;
            case 0x00088000:
                /* 00001100 00011000 1....... ...00... */
                /* ./loongarch.decode.2:278 */
                info->opc = OPC_FCMP_SNE_S;
                return true;
                break;
            case 0x000a0000:
                /* 00001100 00011010 0....... ...00... */
                /* ./loongarch.decode.2:279 */
                info->opc = OPC_FCMP_COR_S;
                return true;
                break;
            case 0x000a8000:
                /* 00001100 00011010 1....... ...00... */
                /* ./loongarch.decode.2:280 */
                info->opc = OPC_FCMP_SOR_S;
                return true;
                break;
            case 0x000c0000:
                /* 00001100 00011100 0....... ...00... */
                /* ./loongarch.decode.2:281 */
                info->opc = OPC_FCMP_CUNE_S;
                return true;
                break;
            case 0x000c8000:
                /* 00001100 00011100 1....... ...00... */
                /* ./loongarch.decode.2:282 */
                info->opc = OPC_FCMP_SUNE_S;
                return true;
                break;
            }
            break;
        case 0x2:
            /* 00001100 0010.... ........ ........ */
            decode_extract_format_31(info, insn);
            switch (insn & 0x000f8018) {
            case 0x00000000:
                /* 00001100 00100000 0....... ...00... */
                /* ./loongarch.decode.2:283 */
                info->opc = OPC_FCMP_CAF_D;
                return true;
                break;
            case 0x00008000:
                /* 00001100 00100000 1....... ...00... */
                /* ./loongarch.decode.2:284 */
                info->opc = OPC_FCMP_SAF_D;
                return true;
                break;
            case 0x00010000:
                /* 00001100 00100001 0....... ...00... */
                /* ./loongarch.decode.2:285 */
                info->opc = OPC_FCMP_CLT_D;
                return true;
                break;
            case 0x00018000:
                /* 00001100 00100001 1....... ...00... */
                /* ./loongarch.decode.2:286 */
                info->opc = OPC_FCMP_STL_D;
                return true;
                break;
            case 0x00020000:
                /* 00001100 00100010 0....... ...00... */
                /* ./loongarch.decode.2:287 */
                info->opc = OPC_FCMP_CEQ_D;
                return true;
                break;
            case 0x00028000:
                /* 00001100 00100010 1....... ...00... */
                /* ./loongarch.decode.2:288 */
                info->opc = OPC_FCMP_SEQ_D;
                return true;
                break;
            case 0x00030000:
                /* 00001100 00100011 0....... ...00... */
                /* ./loongarch.decode.2:289 */
                info->opc = OPC_FCMP_CLE_D;
                return true;
                break;
            case 0x00038000:
                /* 00001100 00100011 1....... ...00... */
                /* ./loongarch.decode.2:290 */
                info->opc = OPC_FCMP_SLE_D;
                return true;
                break;
            case 0x00040000:
                /* 00001100 00100100 0....... ...00... */
                /* ./loongarch.decode.2:291 */
                info->opc = OPC_FCMP_CUN_D;
                return true;
                break;
            case 0x00048000:
                /* 00001100 00100100 1....... ...00... */
                /* ./loongarch.decode.2:292 */
                info->opc = OPC_FCMP_SUN_D;
                return true;
                break;
            case 0x00050000:
                /* 00001100 00100101 0....... ...00... */
                /* ./loongarch.decode.2:293 */
                info->opc = OPC_FCMP_CULT_D;
                return true;
                break;
            case 0x00058000:
                /* 00001100 00100101 1....... ...00... */
                /* ./loongarch.decode.2:294 */
                info->opc = OPC_FCMP_SULT_D;
                return true;
                break;
            case 0x00060000:
                /* 00001100 00100110 0....... ...00... */
                /* ./loongarch.decode.2:295 */
                info->opc = OPC_FCMP_CUEQ_D;
                return true;
                break;
            case 0x00068000:
                /* 00001100 00100110 1....... ...00... */
                /* ./loongarch.decode.2:296 */
                info->opc = OPC_FCMP_SUEQ_D;
                return true;
                break;
            case 0x00070000:
                /* 00001100 00100111 0....... ...00... */
                /* ./loongarch.decode.2:297 */
                info->opc = OPC_FCMP_CULE_D;
                return true;
                break;
            case 0x00078000:
                /* 00001100 00100111 1....... ...00... */
                /* ./loongarch.decode.2:298 */
                info->opc = OPC_FCMP_SULE_D;
                return true;
                break;
            case 0x00080000:
                /* 00001100 00101000 0....... ...00... */
                /* ./loongarch.decode.2:299 */
                info->opc = OPC_FCMP_CNE_D;
                return true;
                break;
            case 0x00088000:
                /* 00001100 00101000 1....... ...00... */
                /* ./loongarch.decode.2:300 */
                info->opc = OPC_FCMP_SNE_D;
                return true;
                break;
            case 0x000a0000:
                /* 00001100 00101010 0....... ...00... */
                /* ./loongarch.decode.2:301 */
                info->opc = OPC_FCMP_COR_D;
                return true;
                break;
            case 0x000a8000:
                /* 00001100 00101010 1....... ...00... */
                /* ./loongarch.decode.2:302 */
                info->opc = OPC_FCMP_SOR_D;
                return true;
                break;
            case 0x000c0000:
                /* 00001100 00101100 0....... ...00... */
                /* ./loongarch.decode.2:303 */
                info->opc = OPC_FCMP_CUNE_D;
                return true;
                break;
            case 0x000c8000:
                /* 00001100 00101100 1....... ...00... */
                /* ./loongarch.decode.2:304 */
                info->opc = OPC_FCMP_SUNE_D;
                return true;
                break;
            }
            break;
        case 0x5:
            /* 00001100 0101.... ........ ........ */
            /* ./loongarch.decode.2:1345 */
            decode_extract_vvv_fcond(info, insn);
            info->opc = OPC_VFCMP_COND_S;
            return true;
            break;
        case 0x6:
            /* 00001100 0110.... ........ ........ */
            /* ./loongarch.decode.2:1346 */
            decode_extract_vvv_fcond(info, insn);
            info->opc = OPC_VFCMP_COND_D;
            return true;
            break;
        case 0x10:
            /* 00001101 0000.... ........ ........ */
            decode_extract_format_32(info, insn);
            switch ((insn >> 18) & 0x3) {
            case 0x0:
                /* 00001101 000000.. ........ ........ */
                /* ./loongarch.decode.2:305 */
                info->opc = OPC_FSEL;
                return true;
                break;
            }
            break;
        case 0x11:
            /* 00001101 0001.... ........ ........ */
            /* ./loongarch.decode.2:1348 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VBITSEL_V;
            return true;
            break;
        case 0x15:
            /* 00001101 0101.... ........ ........ */
            /* ./loongarch.decode.2:1420 */
            decode_extract_vvvv(info, insn);
            info->opc = OPC_VSHUF_B;
            return true;
            break;
        }
        break;
    case 0x4:
        /* 000100.. ........ ........ ........ */
        /* ./loongarch.decode.2:306 */
        decode_extract_format_33(info, insn);
        info->opc = OPC_ADDU16I_D;
        return true;
        break;
    case 0x5:
        /* 000101.. ........ ........ ........ */
        decode_extract_format_34(info, insn);
        switch ((insn >> 25) & 0x1) {
        case 0x0:
            /* 0001010. ........ ........ ........ */
            /* ./loongarch.decode.2:307 */
            info->opc = OPC_LU12I_W;
            return true;
            break;
        case 0x1:
            /* 0001011. ........ ........ ........ */
            /* ./loongarch.decode.2:308 */
            info->opc = OPC_LU32I_D;
            return true;
            break;
        }
        break;
    case 0x6:
        /* 000110.. ........ ........ ........ */
        decode_extract_format_34(info, insn);
        switch ((insn >> 25) & 0x1) {
        case 0x0:
            /* 0001100. ........ ........ ........ */
            /* ./loongarch.decode.2:309 */
            info->opc = OPC_PCADDI;
            return true;
            break;
        case 0x1:
            /* 0001101. ........ ........ ........ */
            /* ./loongarch.decode.2:310 */
            info->opc = OPC_PCALAU12I;
            return true;
            break;
        }
        break;
    case 0x7:
        /* 000111.. ........ ........ ........ */
        decode_extract_format_34(info, insn);
        switch ((insn >> 25) & 0x1) {
        case 0x0:
            /* 0001110. ........ ........ ........ */
            /* ./loongarch.decode.2:311 */
            info->opc = OPC_PCADDU12I;
            return true;
            break;
        case 0x1:
            /* 0001111. ........ ........ ........ */
            /* ./loongarch.decode.2:312 */
            info->opc = OPC_PCADDU18I;
            return true;
            break;
        }
        break;
    case 0x8:
        /* 001000.. ........ ........ ........ */
        decode_extract_format_35(info, insn);
        switch ((insn >> 24) & 0x3) {
        case 0x0:
            /* 00100000 ........ ........ ........ */
            /* ./loongarch.decode.2:313 */
            info->opc = OPC_LL_W;
            return true;
            break;
        case 0x1:
            /* 00100001 ........ ........ ........ */
            /* ./loongarch.decode.2:314 */
            info->opc = OPC_SC_W;
            return true;
            break;
        case 0x2:
            /* 00100010 ........ ........ ........ */
            /* ./loongarch.decode.2:315 */
            info->opc = OPC_LL_D;
            return true;
            break;
        case 0x3:
            /* 00100011 ........ ........ ........ */
            /* ./loongarch.decode.2:316 */
            info->opc = OPC_SC_D;
            return true;
            break;
        }
        break;
    case 0x9:
        /* 001001.. ........ ........ ........ */
        decode_extract_format_35(info, insn);
        switch ((insn >> 24) & 0x3) {
        case 0x0:
            /* 00100100 ........ ........ ........ */
            /* ./loongarch.decode.2:317 */
            info->opc = OPC_LDPTR_W;
            return true;
            break;
        case 0x1:
            /* 00100101 ........ ........ ........ */
            /* ./loongarch.decode.2:318 */
            info->opc = OPC_STPTR_W;
            return true;
            break;
        case 0x2:
            /* 00100110 ........ ........ ........ */
            /* ./loongarch.decode.2:319 */
            info->opc = OPC_LDPTR_D;
            return true;
            break;
        case 0x3:
            /* 00100111 ........ ........ ........ */
            /* ./loongarch.decode.2:320 */
            info->opc = OPC_STPTR_D;
            return true;
            break;
        }
        break;
    case 0xa:
        /* 001010.. ........ ........ ........ */
        switch ((insn >> 22) & 0xf) {
        case 0x0:
            /* 00101000 00...... ........ ........ */
            /* ./loongarch.decode.2:321 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LD_B;
            return true;
            break;
        case 0x1:
            /* 00101000 01...... ........ ........ */
            /* ./loongarch.decode.2:322 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LD_H;
            return true;
            break;
        case 0x2:
            /* 00101000 10...... ........ ........ */
            /* ./loongarch.decode.2:323 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LD_W;
            return true;
            break;
        case 0x3:
            /* 00101000 11...... ........ ........ */
            /* ./loongarch.decode.2:324 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LD_D;
            return true;
            break;
        case 0x4:
            /* 00101001 00...... ........ ........ */
            /* ./loongarch.decode.2:325 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_ST_B;
            return true;
            break;
        case 0x5:
            /* 00101001 01...... ........ ........ */
            /* ./loongarch.decode.2:326 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_ST_H;
            return true;
            break;
        case 0x6:
            /* 00101001 10...... ........ ........ */
            /* ./loongarch.decode.2:327 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_ST_W;
            return true;
            break;
        case 0x7:
            /* 00101001 11...... ........ ........ */
            /* ./loongarch.decode.2:328 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_ST_D;
            return true;
            break;
        case 0x8:
            /* 00101010 00...... ........ ........ */
            /* ./loongarch.decode.2:329 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LD_BU;
            return true;
            break;
        case 0x9:
            /* 00101010 01...... ........ ........ */
            /* ./loongarch.decode.2:330 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LD_HU;
            return true;
            break;
        case 0xa:
            /* 00101010 10...... ........ ........ */
            /* ./loongarch.decode.2:331 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LD_WU;
            return true;
            break;
        case 0xb:
            /* 00101010 11...... ........ ........ */
            /* ./loongarch.decode.2:332 */
            decode_extract_format_36(info, insn);
            info->opc = OPC_PRELD;
            return true;
            break;
        case 0xc:
            /* 00101011 00...... ........ ........ */
            /* ./loongarch.decode.2:333 */
            decode_extract_format_37(info, insn);
            info->opc = OPC_FLD_S;
            return true;
            break;
        case 0xd:
            /* 00101011 01...... ........ ........ */
            /* ./loongarch.decode.2:334 */
            decode_extract_format_37(info, insn);
            info->opc = OPC_FST_S;
            return true;
            break;
        case 0xe:
            /* 00101011 10...... ........ ........ */
            /* ./loongarch.decode.2:335 */
            decode_extract_format_37(info, insn);
            info->opc = OPC_FLD_D;
            return true;
            break;
        case 0xf:
            /* 00101011 11...... ........ ........ */
            /* ./loongarch.decode.2:336 */
            decode_extract_format_37(info, insn);
            info->opc = OPC_FST_D;
            return true;
            break;
        }
        break;
    case 0xb:
        /* 001011.. ........ ........ ........ */
        switch ((insn >> 22) & 0xf) {
        case 0x0:
            /* 00101100 00...... ........ ........ */
            /* ./loongarch.decode.2:1436 */
            decode_extract_vr_i12(info, insn);
            info->opc = OPC_VLD;
            return true;
            break;
        case 0x1:
            /* 00101100 01...... ........ ........ */
            /* ./loongarch.decode.2:1437 */
            decode_extract_vr_i12(info, insn);
            info->opc = OPC_VST;
            return true;
            break;
        case 0x2:
            /* 00101100 10...... ........ ........ */
            /* ./loongarch.decode.2:1455 */
            decode_extract_xr_i12(info, insn);
            info->opc = OPC_XVLD;
            return true;
            break;
        case 0x3:
            /* 00101100 11...... ........ ........ */
            /* ./loongarch.decode.2:1456 */
            decode_extract_xr_i12(info, insn);
            info->opc = OPC_XVST;
            return true;
            break;
        case 0x8:
            /* 00101110 00...... ........ ........ */
            /* ./loongarch.decode.2:612 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LDL_W;
            return true;
            break;
        case 0x9:
            /* 00101110 01...... ........ ........ */
            /* ./loongarch.decode.2:613 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LDR_W;
            return true;
            break;
        case 0xa:
            /* 00101110 10...... ........ ........ */
            /* ./loongarch.decode.2:614 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LDL_D;
            return true;
            break;
        case 0xb:
            /* 00101110 11...... ........ ........ */
            /* ./loongarch.decode.2:615 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_LDR_D;
            return true;
            break;
        case 0xc:
            /* 00101111 00...... ........ ........ */
            /* ./loongarch.decode.2:616 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_STL_W;
            return true;
            break;
        case 0xd:
            /* 00101111 01...... ........ ........ */
            /* ./loongarch.decode.2:617 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_STR_W;
            return true;
            break;
        case 0xe:
            /* 00101111 10...... ........ ........ */
            /* ./loongarch.decode.2:618 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_STL_D;
            return true;
            break;
        case 0xf:
            /* 00101111 11...... ........ ........ */
            /* ./loongarch.decode.2:619 */
            decode_extract_format_20(info, insn);
            info->opc = OPC_STR_D;
            return true;
            break;
        }
        break;
    case 0xc:
        /* 001100.. ........ ........ ........ */
        switch ((insn >> 22) & 0xf) {
        case 0x0:
            /* 00110000 00...... ........ ........ */
            switch ((insn >> 20) & 0x3) {
            case 0x1:
                /* 00110000 0001.... ........ ........ */
                decode_extract_vr_i9(info, insn);
                switch ((insn >> 19) & 0x1) {
                case 0x0:
                    /* 00110000 00010... ........ ........ */
                    /* ./loongarch.decode.2:1441 */
                    info->opc = OPC_VLDREPL_D;
                    return true;
                    break;
                }
                break;
            case 0x2:
                /* 00110000 0010.... ........ ........ */
                /* ./loongarch.decode.2:1442 */
                decode_extract_vr_i10(info, insn);
                info->opc = OPC_VLDREPL_W;
                return true;
                break;
            }
            break;
        case 0x1:
            /* 00110000 01...... ........ ........ */
            decode_extract_vr_i11(info, insn);
            switch ((insn >> 21) & 0x1) {
            case 0x0:
                /* 00110000 010..... ........ ........ */
                /* ./loongarch.decode.2:1443 */
                info->opc = OPC_VLDREPL_H;
                return true;
                break;
            }
            break;
        case 0x2:
            /* 00110000 10...... ........ ........ */
            /* ./loongarch.decode.2:1444 */
            decode_extract_vr_i12(info, insn);
            info->opc = OPC_VLDREPL_B;
            return true;
            break;
        case 0x4:
            /* 00110001 00...... ........ ........ */
            switch ((insn >> 20) & 0x3) {
            case 0x1:
                /* 00110001 0001.... ........ ........ */
                decode_extract_vr_i8i1(info, insn);
                switch ((insn >> 19) & 0x1) {
                case 0x0:
                    /* 00110001 00010... ........ ........ */
                    /* ./loongarch.decode.2:1445 */
                    info->opc = OPC_VSTELM_D;
                    return true;
                    break;
                }
                break;
            case 0x2:
                /* 00110001 0010.... ........ ........ */
                /* ./loongarch.decode.2:1446 */
                decode_extract_vr_i8i2(info, insn);
                info->opc = OPC_VSTELM_W;
                return true;
                break;
            }
            break;
        case 0x5:
            /* 00110001 01...... ........ ........ */
            decode_extract_vr_i8i3(info, insn);
            switch ((insn >> 21) & 0x1) {
            case 0x0:
                /* 00110001 010..... ........ ........ */
                /* ./loongarch.decode.2:1447 */
                info->opc = OPC_VSTELM_H;
                return true;
                break;
            }
            break;
        case 0x6:
            /* 00110001 10...... ........ ........ */
            /* ./loongarch.decode.2:1448 */
            decode_extract_vr_i8i4(info, insn);
            info->opc = OPC_VSTELM_B;
            return true;
            break;
        }
        break;
    case 0xe:
        /* 001110.. ........ ........ ........ */
        switch ((insn >> 15) & 0x7ff) {
        case 0x0:
            /* 00111000 00000000 0....... ........ */
            /* ./loongarch.decode.2:337 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDX_B;
            return true;
            break;
        case 0x8:
            /* 00111000 00000100 0....... ........ */
            /* ./loongarch.decode.2:338 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDX_H;
            return true;
            break;
        case 0x10:
            /* 00111000 00001000 0....... ........ */
            /* ./loongarch.decode.2:339 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDX_W;
            return true;
            break;
        case 0x18:
            /* 00111000 00001100 0....... ........ */
            /* ./loongarch.decode.2:340 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDX_D;
            return true;
            break;
        case 0x20:
            /* 00111000 00010000 0....... ........ */
            /* ./loongarch.decode.2:341 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STX_B;
            return true;
            break;
        case 0x28:
            /* 00111000 00010100 0....... ........ */
            /* ./loongarch.decode.2:342 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STX_H;
            return true;
            break;
        case 0x30:
            /* 00111000 00011000 0....... ........ */
            /* ./loongarch.decode.2:343 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STX_W;
            return true;
            break;
        case 0x38:
            /* 00111000 00011100 0....... ........ */
            /* ./loongarch.decode.2:344 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STX_D;
            return true;
            break;
        case 0x40:
            /* 00111000 00100000 0....... ........ */
            /* ./loongarch.decode.2:345 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDX_BU;
            return true;
            break;
        case 0x48:
            /* 00111000 00100100 0....... ........ */
            /* ./loongarch.decode.2:346 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDX_HU;
            return true;
            break;
        case 0x50:
            /* 00111000 00101000 0....... ........ */
            /* ./loongarch.decode.2:347 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDX_WU;
            return true;
            break;
        case 0x58:
            /* 00111000 00101100 0....... ........ */
            /* ./loongarch.decode.2:348 */
            decode_extract_format_38(info, insn);
            info->opc = OPC_PRELDX;
            return true;
            break;
        case 0x60:
            /* 00111000 00110000 0....... ........ */
            /* ./loongarch.decode.2:349 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FLDX_S;
            return true;
            break;
        case 0x68:
            /* 00111000 00110100 0....... ........ */
            /* ./loongarch.decode.2:350 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FLDX_D;
            return true;
            break;
        case 0x70:
            /* 00111000 00111000 0....... ........ */
            /* ./loongarch.decode.2:351 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FSTX_S;
            return true;
            break;
        case 0x78:
            /* 00111000 00111100 0....... ........ */
            /* ./loongarch.decode.2:352 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FSTX_D;
            return true;
            break;
        case 0x80:
            /* 00111000 01000000 0....... ........ */
            /* ./loongarch.decode.2:1438 */
            decode_extract_vrr(info, insn);
            info->opc = OPC_VLDX;
            return true;
            break;
        case 0x88:
            /* 00111000 01000100 0....... ........ */
            /* ./loongarch.decode.2:1439 */
            decode_extract_vrr(info, insn);
            info->opc = OPC_VSTX;
            return true;
            break;
        case 0xc0:
            /* 00111000 01100000 0....... ........ */
            /* ./loongarch.decode.2:353 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMSWAP_W;
            return true;
            break;
        case 0xc1:
            /* 00111000 01100000 1....... ........ */
            /* ./loongarch.decode.2:354 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMSWAP_D;
            return true;
            break;
        case 0xc2:
            /* 00111000 01100001 0....... ........ */
            /* ./loongarch.decode.2:355 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMADD_W;
            return true;
            break;
        case 0xc3:
            /* 00111000 01100001 1....... ........ */
            /* ./loongarch.decode.2:356 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMADD_D;
            return true;
            break;
        case 0xc4:
            /* 00111000 01100010 0....... ........ */
            /* ./loongarch.decode.2:357 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMAND_W;
            return true;
            break;
        case 0xc5:
            /* 00111000 01100010 1....... ........ */
            /* ./loongarch.decode.2:358 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMAND_D;
            return true;
            break;
        case 0xc6:
            /* 00111000 01100011 0....... ........ */
            /* ./loongarch.decode.2:359 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMOR_W;
            return true;
            break;
        case 0xc7:
            /* 00111000 01100011 1....... ........ */
            /* ./loongarch.decode.2:360 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMOR_D;
            return true;
            break;
        case 0xc8:
            /* 00111000 01100100 0....... ........ */
            /* ./loongarch.decode.2:361 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMXOR_W;
            return true;
            break;
        case 0xc9:
            /* 00111000 01100100 1....... ........ */
            /* ./loongarch.decode.2:362 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMXOR_D;
            return true;
            break;
        case 0xca:
            /* 00111000 01100101 0....... ........ */
            /* ./loongarch.decode.2:363 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_W;
            return true;
            break;
        case 0xcb:
            /* 00111000 01100101 1....... ........ */
            /* ./loongarch.decode.2:364 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_D;
            return true;
            break;
        case 0xcc:
            /* 00111000 01100110 0....... ........ */
            /* ./loongarch.decode.2:365 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_W;
            return true;
            break;
        case 0xcd:
            /* 00111000 01100110 1....... ........ */
            /* ./loongarch.decode.2:366 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_D;
            return true;
            break;
        case 0xce:
            /* 00111000 01100111 0....... ........ */
            /* ./loongarch.decode.2:367 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_WU;
            return true;
            break;
        case 0xcf:
            /* 00111000 01100111 1....... ........ */
            /* ./loongarch.decode.2:368 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_DU;
            return true;
            break;
        case 0xd0:
            /* 00111000 01101000 0....... ........ */
            /* ./loongarch.decode.2:369 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_WU;
            return true;
            break;
        case 0xd1:
            /* 00111000 01101000 1....... ........ */
            /* ./loongarch.decode.2:370 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_DU;
            return true;
            break;
        case 0xd2:
            /* 00111000 01101001 0....... ........ */
            /* ./loongarch.decode.2:371 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMSWAP_DB_W;
            return true;
            break;
        case 0xd3:
            /* 00111000 01101001 1....... ........ */
            /* ./loongarch.decode.2:372 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMSWAP_DB_D;
            return true;
            break;
        case 0xd4:
            /* 00111000 01101010 0....... ........ */
            /* ./loongarch.decode.2:373 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMADD_DB_W;
            return true;
            break;
        case 0xd5:
            /* 00111000 01101010 1....... ........ */
            /* ./loongarch.decode.2:374 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMADD_DB_D;
            return true;
            break;
        case 0xd6:
            /* 00111000 01101011 0....... ........ */
            /* ./loongarch.decode.2:375 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMAND_DB_W;
            return true;
            break;
        case 0xd7:
            /* 00111000 01101011 1....... ........ */
            /* ./loongarch.decode.2:376 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMAND_DB_D;
            return true;
            break;
        case 0xd8:
            /* 00111000 01101100 0....... ........ */
            /* ./loongarch.decode.2:377 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMOR_DB_W;
            return true;
            break;
        case 0xd9:
            /* 00111000 01101100 1....... ........ */
            /* ./loongarch.decode.2:378 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMOR_DB_D;
            return true;
            break;
        case 0xda:
            /* 00111000 01101101 0....... ........ */
            /* ./loongarch.decode.2:379 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMXOR_DB_W;
            return true;
            break;
        case 0xdb:
            /* 00111000 01101101 1....... ........ */
            /* ./loongarch.decode.2:380 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMXOR_DB_D;
            return true;
            break;
        case 0xdc:
            /* 00111000 01101110 0....... ........ */
            /* ./loongarch.decode.2:381 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_DB_W;
            return true;
            break;
        case 0xdd:
            /* 00111000 01101110 1....... ........ */
            /* ./loongarch.decode.2:382 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_DB_D;
            return true;
            break;
        case 0xde:
            /* 00111000 01101111 0....... ........ */
            /* ./loongarch.decode.2:383 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_DB_W;
            return true;
            break;
        case 0xdf:
            /* 00111000 01101111 1....... ........ */
            /* ./loongarch.decode.2:384 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_DB_D;
            return true;
            break;
        case 0xe0:
            /* 00111000 01110000 0....... ........ */
            /* ./loongarch.decode.2:385 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_DB_WU;
            return true;
            break;
        case 0xe1:
            /* 00111000 01110000 1....... ........ */
            /* ./loongarch.decode.2:386 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMAX_DB_DU;
            return true;
            break;
        case 0xe2:
            /* 00111000 01110001 0....... ........ */
            /* ./loongarch.decode.2:387 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_DB_WU;
            return true;
            break;
        case 0xe3:
            /* 00111000 01110001 1....... ........ */
            /* ./loongarch.decode.2:388 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_AMMIN_DB_DU;
            return true;
            break;
        case 0xe4:
            /* 00111000 01110010 0....... ........ */
            /* ./loongarch.decode.2:389 */
            decode_extract_format_40(info, insn);
            info->opc = OPC_DBAR;
            return true;
            break;
        case 0xe5:
            /* 00111000 01110010 1....... ........ */
            /* ./loongarch.decode.2:390 */
            decode_extract_format_41(info, insn);
            info->opc = OPC_IBAR;
            return true;
            break;
        case 0xe8:
            /* 00111000 01110100 0....... ........ */
            /* ./loongarch.decode.2:391 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FLDGT_S;
            return true;
            break;
        case 0xe9:
            /* 00111000 01110100 1....... ........ */
            /* ./loongarch.decode.2:392 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FLDGT_D;
            return true;
            break;
        case 0xea:
            /* 00111000 01110101 0....... ........ */
            /* ./loongarch.decode.2:393 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FLDLE_S;
            return true;
            break;
        case 0xeb:
            /* 00111000 01110101 1....... ........ */
            /* ./loongarch.decode.2:394 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FLDLE_D;
            return true;
            break;
        case 0xec:
            /* 00111000 01110110 0....... ........ */
            /* ./loongarch.decode.2:395 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FSTGT_S;
            return true;
            break;
        case 0xed:
            /* 00111000 01110110 1....... ........ */
            /* ./loongarch.decode.2:396 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FSTGT_D;
            return true;
            break;
        case 0xee:
            /* 00111000 01110111 0....... ........ */
            /* ./loongarch.decode.2:397 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FSTLE_S;
            return true;
            break;
        case 0xef:
            /* 00111000 01110111 1....... ........ */
            /* ./loongarch.decode.2:398 */
            decode_extract_format_39(info, insn);
            info->opc = OPC_FSTLE_D;
            return true;
            break;
        case 0xf0:
            /* 00111000 01111000 0....... ........ */
            /* ./loongarch.decode.2:399 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDGT_B;
            return true;
            break;
        case 0xf1:
            /* 00111000 01111000 1....... ........ */
            /* ./loongarch.decode.2:400 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDGT_H;
            return true;
            break;
        case 0xf2:
            /* 00111000 01111001 0....... ........ */
            /* ./loongarch.decode.2:401 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDGT_W;
            return true;
            break;
        case 0xf3:
            /* 00111000 01111001 1....... ........ */
            /* ./loongarch.decode.2:402 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDGT_D;
            return true;
            break;
        case 0xf4:
            /* 00111000 01111010 0....... ........ */
            /* ./loongarch.decode.2:403 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDLE_B;
            return true;
            break;
        case 0xf5:
            /* 00111000 01111010 1....... ........ */
            /* ./loongarch.decode.2:404 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDLE_H;
            return true;
            break;
        case 0xf6:
            /* 00111000 01111011 0....... ........ */
            /* ./loongarch.decode.2:405 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDLE_W;
            return true;
            break;
        case 0xf7:
            /* 00111000 01111011 1....... ........ */
            /* ./loongarch.decode.2:406 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_LDLE_D;
            return true;
            break;
        case 0xf8:
            /* 00111000 01111100 0....... ........ */
            /* ./loongarch.decode.2:407 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STGT_B;
            return true;
            break;
        case 0xf9:
            /* 00111000 01111100 1....... ........ */
            /* ./loongarch.decode.2:408 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STGT_H;
            return true;
            break;
        case 0xfa:
            /* 00111000 01111101 0....... ........ */
            /* ./loongarch.decode.2:409 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STGT_W;
            return true;
            break;
        case 0xfb:
            /* 00111000 01111101 1....... ........ */
            /* ./loongarch.decode.2:410 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STGT_D;
            return true;
            break;
        case 0xfc:
            /* 00111000 01111110 0....... ........ */
            /* ./loongarch.decode.2:411 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STLE_B;
            return true;
            break;
        case 0xfd:
            /* 00111000 01111110 1....... ........ */
            /* ./loongarch.decode.2:412 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STLE_H;
            return true;
            break;
        case 0xfe:
            /* 00111000 01111111 0....... ........ */
            /* ./loongarch.decode.2:413 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STLE_W;
            return true;
            break;
        case 0xff:
            /* 00111000 01111111 1....... ........ */
            /* ./loongarch.decode.2:414 */
            decode_extract_format_4(info, insn);
            info->opc = OPC_STLE_D;
            return true;
            break;
        }
        break;
    case 0x10:
        /* 010000.. ........ ........ ........ */
        /* ./loongarch.decode.2:415 */
        decode_extract_format_42(info, insn);
        info->opc = OPC_BEQZ;
        return true;
        break;
    case 0x11:
        /* 010001.. ........ ........ ........ */
        /* ./loongarch.decode.2:416 */
        decode_extract_format_42(info, insn);
        info->opc = OPC_BNEZ;
        return true;
        break;
    case 0x12:
        /* 010010.. ........ ........ ........ */
        switch ((insn >> 8) & 0x3) {
        case 0x0:
            /* 010010.. ........ ......00 ........ */
            /* ./loongarch.decode.2:417 */
            decode_extract_format_43(info, insn);
            info->opc = OPC_BCEQZ;
            return true;
            break;
        case 0x1:
            /* 010010.. ........ ......01 ........ */
            /* ./loongarch.decode.2:418 */
            decode_extract_format_43(info, insn);
            info->opc = OPC_BCNEZ;
            return true;
            break;
        case 0x2:
            /* 010010.. ........ ......10 ........ */
            decode_extract_format_64(info, insn);
            switch ((insn >> 5) & 0x7) {
            case 0x0:
                /* 010010.. ........ ......10 000..... */
                /* ./loongarch.decode.2:620 */
                info->opc = OPC_JISCR0;
                return true;
                break;
            }
            break;
        case 0x3:
            /* 010010.. ........ ......11 ........ */
            decode_extract_format_64(info, insn);
            switch ((insn >> 5) & 0x7) {
            case 0x0:
                /* 010010.. ........ ......11 000..... */
                /* ./loongarch.decode.2:621 */
                info->opc = OPC_JISCR1;
                return true;
                break;
            }
            break;
        }
        break;
    case 0x13:
        /* 010011.. ........ ........ ........ */
        /* ./loongarch.decode.2:419 */
        decode_extract_format_44(info, insn);
        info->opc = OPC_JIRL;
        return true;
        break;
    case 0x14:
        /* 010100.. ........ ........ ........ */
        /* ./loongarch.decode.2:420 */
        decode_extract_format_45(info, insn);
        info->opc = OPC_B;
        return true;
        break;
    case 0x15:
        /* 010101.. ........ ........ ........ */
        /* ./loongarch.decode.2:421 */
        decode_extract_format_45(info, insn);
        info->opc = OPC_BL;
        return true;
        break;
    case 0x16:
        /* 010110.. ........ ........ ........ */
        /* ./loongarch.decode.2:422 */
        decode_extract_format_44(info, insn);
        info->opc = OPC_BEQ;
        return true;
        break;
    case 0x17:
        /* 010111.. ........ ........ ........ */
        /* ./loongarch.decode.2:423 */
        decode_extract_format_44(info, insn);
        info->opc = OPC_BNE;
        return true;
        break;
    case 0x18:
        /* 011000.. ........ ........ ........ */
        /* ./loongarch.decode.2:424 */
        decode_extract_format_44(info, insn);
        info->opc = OPC_BLT;
        return true;
        break;
    case 0x19:
        /* 011001.. ........ ........ ........ */
        /* ./loongarch.decode.2:425 */
        decode_extract_format_44(info, insn);
        info->opc = OPC_BGE;
        return true;
        break;
    case 0x1a:
        /* 011010.. ........ ........ ........ */
        /* ./loongarch.decode.2:426 */
        decode_extract_format_44(info, insn);
        info->opc = OPC_BLTU;
        return true;
        break;
    case 0x1b:
        /* 011011.. ........ ........ ........ */
        /* ./loongarch.decode.2:427 */
        decode_extract_format_44(info, insn);
        info->opc = OPC_BGEU;
        return true;
        break;
    case 0x1c:
        /* 011100.. ........ ........ ........ */
        switch ((insn >> 18) & 0xff) {
        case 0x0:
            /* 01110000 000000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00000000 0....... ........ */
                /* ./loongarch.decode.2:1302 */
                info->opc = OPC_VSEQ_B;
                return true;
                break;
            case 0x1:
                /* 01110000 00000000 1....... ........ */
                /* ./loongarch.decode.2:1303 */
                info->opc = OPC_VSEQ_H;
                return true;
                break;
            case 0x2:
                /* 01110000 00000001 0....... ........ */
                /* ./loongarch.decode.2:1304 */
                info->opc = OPC_VSEQ_W;
                return true;
                break;
            case 0x3:
                /* 01110000 00000001 1....... ........ */
                /* ./loongarch.decode.2:1305 */
                info->opc = OPC_VSEQ_D;
                return true;
                break;
            case 0x4:
                /* 01110000 00000010 0....... ........ */
                /* ./loongarch.decode.2:1311 */
                info->opc = OPC_VSLE_B;
                return true;
                break;
            case 0x5:
                /* 01110000 00000010 1....... ........ */
                /* ./loongarch.decode.2:1312 */
                info->opc = OPC_VSLE_H;
                return true;
                break;
            case 0x6:
                /* 01110000 00000011 0....... ........ */
                /* ./loongarch.decode.2:1313 */
                info->opc = OPC_VSLE_W;
                return true;
                break;
            case 0x7:
                /* 01110000 00000011 1....... ........ */
                /* ./loongarch.decode.2:1314 */
                info->opc = OPC_VSLE_D;
                return true;
                break;
            }
            break;
        case 0x1:
            /* 01110000 000001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00000100 0....... ........ */
                /* ./loongarch.decode.2:1319 */
                info->opc = OPC_VSLE_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 00000100 1....... ........ */
                /* ./loongarch.decode.2:1320 */
                info->opc = OPC_VSLE_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 00000101 0....... ........ */
                /* ./loongarch.decode.2:1321 */
                info->opc = OPC_VSLE_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 00000101 1....... ........ */
                /* ./loongarch.decode.2:1322 */
                info->opc = OPC_VSLE_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 00000110 0....... ........ */
                /* ./loongarch.decode.2:1328 */
                info->opc = OPC_VSLT_B;
                return true;
                break;
            case 0x5:
                /* 01110000 00000110 1....... ........ */
                /* ./loongarch.decode.2:1329 */
                info->opc = OPC_VSLT_H;
                return true;
                break;
            case 0x6:
                /* 01110000 00000111 0....... ........ */
                /* ./loongarch.decode.2:1330 */
                info->opc = OPC_VSLT_W;
                return true;
                break;
            case 0x7:
                /* 01110000 00000111 1....... ........ */
                /* ./loongarch.decode.2:1331 */
                info->opc = OPC_VSLT_D;
                return true;
                break;
            }
            break;
        case 0x2:
            /* 01110000 000010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00001000 0....... ........ */
                /* ./loongarch.decode.2:1336 */
                info->opc = OPC_VSLT_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 00001000 1....... ........ */
                /* ./loongarch.decode.2:1337 */
                info->opc = OPC_VSLT_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 00001001 0....... ........ */
                /* ./loongarch.decode.2:1338 */
                info->opc = OPC_VSLT_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 00001001 1....... ........ */
                /* ./loongarch.decode.2:1339 */
                info->opc = OPC_VSLT_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 00001010 0....... ........ */
                /* ./loongarch.decode.2:697 */
                info->opc = OPC_VADD_B;
                return true;
                break;
            case 0x5:
                /* 01110000 00001010 1....... ........ */
                /* ./loongarch.decode.2:698 */
                info->opc = OPC_VADD_H;
                return true;
                break;
            case 0x6:
                /* 01110000 00001011 0....... ........ */
                /* ./loongarch.decode.2:699 */
                info->opc = OPC_VADD_W;
                return true;
                break;
            case 0x7:
                /* 01110000 00001011 1....... ........ */
                /* ./loongarch.decode.2:700 */
                info->opc = OPC_VADD_D;
                return true;
                break;
            }
            break;
        case 0x3:
            /* 01110000 000011.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00001100 0....... ........ */
                /* ./loongarch.decode.2:702 */
                info->opc = OPC_VSUB_B;
                return true;
                break;
            case 0x1:
                /* 01110000 00001100 1....... ........ */
                /* ./loongarch.decode.2:703 */
                info->opc = OPC_VSUB_H;
                return true;
                break;
            case 0x2:
                /* 01110000 00001101 0....... ........ */
                /* ./loongarch.decode.2:704 */
                info->opc = OPC_VSUB_W;
                return true;
                break;
            case 0x3:
                /* 01110000 00001101 1....... ........ */
                /* ./loongarch.decode.2:705 */
                info->opc = OPC_VSUB_D;
                return true;
                break;
            }
            break;
        case 0x7:
            /* 01110000 000111.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110000 00011110 0....... ........ */
                /* ./loongarch.decode.2:758 */
                info->opc = OPC_VADDWEV_H_B;
                return true;
                break;
            case 0x5:
                /* 01110000 00011110 1....... ........ */
                /* ./loongarch.decode.2:759 */
                info->opc = OPC_VADDWEV_W_H;
                return true;
                break;
            case 0x6:
                /* 01110000 00011111 0....... ........ */
                /* ./loongarch.decode.2:760 */
                info->opc = OPC_VADDWEV_D_W;
                return true;
                break;
            case 0x7:
                /* 01110000 00011111 1....... ........ */
                /* ./loongarch.decode.2:761 */
                info->opc = OPC_VADDWEV_Q_D;
                return true;
                break;
            }
            break;
        case 0x8:
            /* 01110000 001000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00100000 0....... ........ */
                /* ./loongarch.decode.2:767 */
                info->opc = OPC_VSUBWEV_H_B;
                return true;
                break;
            case 0x1:
                /* 01110000 00100000 1....... ........ */
                /* ./loongarch.decode.2:768 */
                info->opc = OPC_VSUBWEV_W_H;
                return true;
                break;
            case 0x2:
                /* 01110000 00100001 0....... ........ */
                /* ./loongarch.decode.2:769 */
                info->opc = OPC_VSUBWEV_D_W;
                return true;
                break;
            case 0x3:
                /* 01110000 00100001 1....... ........ */
                /* ./loongarch.decode.2:770 */
                info->opc = OPC_VSUBWEV_Q_D;
                return true;
                break;
            case 0x4:
                /* 01110000 00100010 0....... ........ */
                /* ./loongarch.decode.2:762 */
                info->opc = OPC_VADDWOD_H_B;
                return true;
                break;
            case 0x5:
                /* 01110000 00100010 1....... ........ */
                /* ./loongarch.decode.2:763 */
                info->opc = OPC_VADDWOD_W_H;
                return true;
                break;
            case 0x6:
                /* 01110000 00100011 0....... ........ */
                /* ./loongarch.decode.2:764 */
                info->opc = OPC_VADDWOD_D_W;
                return true;
                break;
            case 0x7:
                /* 01110000 00100011 1....... ........ */
                /* ./loongarch.decode.2:765 */
                info->opc = OPC_VADDWOD_Q_D;
                return true;
                break;
            }
            break;
        case 0x9:
            /* 01110000 001001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00100100 0....... ........ */
                /* ./loongarch.decode.2:771 */
                info->opc = OPC_VSUBWOD_H_B;
                return true;
                break;
            case 0x1:
                /* 01110000 00100100 1....... ........ */
                /* ./loongarch.decode.2:772 */
                info->opc = OPC_VSUBWOD_W_H;
                return true;
                break;
            case 0x2:
                /* 01110000 00100101 0....... ........ */
                /* ./loongarch.decode.2:773 */
                info->opc = OPC_VSUBWOD_D_W;
                return true;
                break;
            case 0x3:
                /* 01110000 00100101 1....... ........ */
                /* ./loongarch.decode.2:774 */
                info->opc = OPC_VSUBWOD_Q_D;
                return true;
                break;
            }
            break;
        case 0xb:
            /* 01110000 001011.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110000 00101110 0....... ........ */
                /* ./loongarch.decode.2:776 */
                info->opc = OPC_VADDWEV_H_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 00101110 1....... ........ */
                /* ./loongarch.decode.2:777 */
                info->opc = OPC_VADDWEV_W_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 00101111 0....... ........ */
                /* ./loongarch.decode.2:778 */
                info->opc = OPC_VADDWEV_D_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 00101111 1....... ........ */
                /* ./loongarch.decode.2:779 */
                info->opc = OPC_VADDWEV_Q_DU;
                return true;
                break;
            }
            break;
        case 0xc:
            /* 01110000 001100.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00110000 0....... ........ */
                /* ./loongarch.decode.2:785 */
                info->opc = OPC_VSUBWEV_H_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 00110000 1....... ........ */
                /* ./loongarch.decode.2:786 */
                info->opc = OPC_VSUBWEV_W_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 00110001 0....... ........ */
                /* ./loongarch.decode.2:787 */
                info->opc = OPC_VSUBWEV_D_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 00110001 1....... ........ */
                /* ./loongarch.decode.2:788 */
                info->opc = OPC_VSUBWEV_Q_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 00110010 0....... ........ */
                /* ./loongarch.decode.2:780 */
                info->opc = OPC_VADDWOD_H_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 00110010 1....... ........ */
                /* ./loongarch.decode.2:781 */
                info->opc = OPC_VADDWOD_W_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 00110011 0....... ........ */
                /* ./loongarch.decode.2:782 */
                info->opc = OPC_VADDWOD_D_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 00110011 1....... ........ */
                /* ./loongarch.decode.2:783 */
                info->opc = OPC_VADDWOD_Q_DU;
                return true;
                break;
            }
            break;
        case 0xd:
            /* 01110000 001101.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 00110100 0....... ........ */
                /* ./loongarch.decode.2:789 */
                info->opc = OPC_VSUBWOD_H_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 00110100 1....... ........ */
                /* ./loongarch.decode.2:790 */
                info->opc = OPC_VSUBWOD_W_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 00110101 0....... ........ */
                /* ./loongarch.decode.2:791 */
                info->opc = OPC_VSUBWOD_D_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 00110101 1....... ........ */
                /* ./loongarch.decode.2:792 */
                info->opc = OPC_VSUBWOD_Q_DU;
                return true;
                break;
            }
            break;
        case 0xf:
            /* 01110000 001111.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110000 00111110 0....... ........ */
                /* ./loongarch.decode.2:794 */
                info->opc = OPC_VADDWEV_H_BU_B;
                return true;
                break;
            case 0x5:
                /* 01110000 00111110 1....... ........ */
                /* ./loongarch.decode.2:795 */
                info->opc = OPC_VADDWEV_W_HU_H;
                return true;
                break;
            case 0x6:
                /* 01110000 00111111 0....... ........ */
                /* ./loongarch.decode.2:796 */
                info->opc = OPC_VADDWEV_D_WU_W;
                return true;
                break;
            case 0x7:
                /* 01110000 00111111 1....... ........ */
                /* ./loongarch.decode.2:797 */
                info->opc = OPC_VADDWEV_Q_DU_D;
                return true;
                break;
            }
            break;
        case 0x10:
            /* 01110000 010000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01000000 0....... ........ */
                /* ./loongarch.decode.2:798 */
                info->opc = OPC_VADDWOD_H_BU_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01000000 1....... ........ */
                /* ./loongarch.decode.2:799 */
                info->opc = OPC_VADDWOD_W_HU_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01000001 0....... ........ */
                /* ./loongarch.decode.2:800 */
                info->opc = OPC_VADDWOD_D_WU_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01000001 1....... ........ */
                /* ./loongarch.decode.2:801 */
                info->opc = OPC_VADDWOD_Q_DU_D;
                return true;
                break;
            }
            break;
        case 0x11:
            /* 01110000 010001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110000 01000110 0....... ........ */
                /* ./loongarch.decode.2:722 */
                info->opc = OPC_VSADD_B;
                return true;
                break;
            case 0x5:
                /* 01110000 01000110 1....... ........ */
                /* ./loongarch.decode.2:723 */
                info->opc = OPC_VSADD_H;
                return true;
                break;
            case 0x6:
                /* 01110000 01000111 0....... ........ */
                /* ./loongarch.decode.2:724 */
                info->opc = OPC_VSADD_W;
                return true;
                break;
            case 0x7:
                /* 01110000 01000111 1....... ........ */
                /* ./loongarch.decode.2:725 */
                info->opc = OPC_VSADD_D;
                return true;
                break;
            }
            break;
        case 0x12:
            /* 01110000 010010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01001000 0....... ........ */
                /* ./loongarch.decode.2:731 */
                info->opc = OPC_VSSUB_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01001000 1....... ........ */
                /* ./loongarch.decode.2:732 */
                info->opc = OPC_VSSUB_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01001001 0....... ........ */
                /* ./loongarch.decode.2:733 */
                info->opc = OPC_VSSUB_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01001001 1....... ........ */
                /* ./loongarch.decode.2:734 */
                info->opc = OPC_VSSUB_D;
                return true;
                break;
            case 0x4:
                /* 01110000 01001010 0....... ........ */
                /* ./loongarch.decode.2:726 */
                info->opc = OPC_VSADD_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 01001010 1....... ........ */
                /* ./loongarch.decode.2:727 */
                info->opc = OPC_VSADD_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 01001011 0....... ........ */
                /* ./loongarch.decode.2:728 */
                info->opc = OPC_VSADD_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 01001011 1....... ........ */
                /* ./loongarch.decode.2:729 */
                info->opc = OPC_VSADD_DU;
                return true;
                break;
            }
            break;
        case 0x13:
            /* 01110000 010011.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01001100 0....... ........ */
                /* ./loongarch.decode.2:735 */
                info->opc = OPC_VSSUB_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 01001100 1....... ........ */
                /* ./loongarch.decode.2:736 */
                info->opc = OPC_VSSUB_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 01001101 0....... ........ */
                /* ./loongarch.decode.2:737 */
                info->opc = OPC_VSSUB_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 01001101 1....... ........ */
                /* ./loongarch.decode.2:738 */
                info->opc = OPC_VSSUB_DU;
                return true;
                break;
            }
            break;
        case 0x15:
            /* 01110000 010101.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01010100 0....... ........ */
                /* ./loongarch.decode.2:740 */
                info->opc = OPC_VHADDW_H_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01010100 1....... ........ */
                /* ./loongarch.decode.2:741 */
                info->opc = OPC_VHADDW_W_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01010101 0....... ........ */
                /* ./loongarch.decode.2:742 */
                info->opc = OPC_VHADDW_D_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01010101 1....... ........ */
                /* ./loongarch.decode.2:743 */
                info->opc = OPC_VHADDW_Q_D;
                return true;
                break;
            case 0x4:
                /* 01110000 01010110 0....... ........ */
                /* ./loongarch.decode.2:749 */
                info->opc = OPC_VHSUBW_H_B;
                return true;
                break;
            case 0x5:
                /* 01110000 01010110 1....... ........ */
                /* ./loongarch.decode.2:750 */
                info->opc = OPC_VHSUBW_W_H;
                return true;
                break;
            case 0x6:
                /* 01110000 01010111 0....... ........ */
                /* ./loongarch.decode.2:751 */
                info->opc = OPC_VHSUBW_D_W;
                return true;
                break;
            case 0x7:
                /* 01110000 01010111 1....... ........ */
                /* ./loongarch.decode.2:752 */
                info->opc = OPC_VHSUBW_Q_D;
                return true;
                break;
            }
            break;
        case 0x16:
            /* 01110000 010110.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01011000 0....... ........ */
                /* ./loongarch.decode.2:744 */
                info->opc = OPC_VHADDW_HU_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 01011000 1....... ........ */
                /* ./loongarch.decode.2:745 */
                info->opc = OPC_VHADDW_WU_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 01011001 0....... ........ */
                /* ./loongarch.decode.2:746 */
                info->opc = OPC_VHADDW_DU_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 01011001 1....... ........ */
                /* ./loongarch.decode.2:747 */
                info->opc = OPC_VHADDW_QU_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 01011010 0....... ........ */
                /* ./loongarch.decode.2:753 */
                info->opc = OPC_VHSUBW_HU_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 01011010 1....... ........ */
                /* ./loongarch.decode.2:754 */
                info->opc = OPC_VHSUBW_WU_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 01011011 0....... ........ */
                /* ./loongarch.decode.2:755 */
                info->opc = OPC_VHSUBW_DU_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 01011011 1....... ........ */
                /* ./loongarch.decode.2:756 */
                info->opc = OPC_VHSUBW_QU_DU;
                return true;
                break;
            }
            break;
        case 0x17:
            /* 01110000 010111.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01011100 0....... ........ */
                /* ./loongarch.decode.2:829 */
                info->opc = OPC_VADDA_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01011100 1....... ........ */
                /* ./loongarch.decode.2:830 */
                info->opc = OPC_VADDA_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01011101 0....... ........ */
                /* ./loongarch.decode.2:831 */
                info->opc = OPC_VADDA_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01011101 1....... ........ */
                /* ./loongarch.decode.2:832 */
                info->opc = OPC_VADDA_D;
                return true;
                break;
            }
            break;
        case 0x18:
            /* 01110000 011000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01100000 0....... ........ */
                /* ./loongarch.decode.2:820 */
                info->opc = OPC_VABSD_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01100000 1....... ........ */
                /* ./loongarch.decode.2:821 */
                info->opc = OPC_VABSD_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01100001 0....... ........ */
                /* ./loongarch.decode.2:822 */
                info->opc = OPC_VABSD_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01100001 1....... ........ */
                /* ./loongarch.decode.2:823 */
                info->opc = OPC_VABSD_D;
                return true;
                break;
            case 0x4:
                /* 01110000 01100010 0....... ........ */
                /* ./loongarch.decode.2:824 */
                info->opc = OPC_VABSD_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 01100010 1....... ........ */
                /* ./loongarch.decode.2:825 */
                info->opc = OPC_VABSD_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 01100011 0....... ........ */
                /* ./loongarch.decode.2:826 */
                info->opc = OPC_VABSD_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 01100011 1....... ........ */
                /* ./loongarch.decode.2:827 */
                info->opc = OPC_VABSD_DU;
                return true;
                break;
            }
            break;
        case 0x19:
            /* 01110000 011001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01100100 0....... ........ */
                /* ./loongarch.decode.2:803 */
                info->opc = OPC_VAVG_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01100100 1....... ........ */
                /* ./loongarch.decode.2:804 */
                info->opc = OPC_VAVG_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01100101 0....... ........ */
                /* ./loongarch.decode.2:805 */
                info->opc = OPC_VAVG_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01100101 1....... ........ */
                /* ./loongarch.decode.2:806 */
                info->opc = OPC_VAVG_D;
                return true;
                break;
            case 0x4:
                /* 01110000 01100110 0....... ........ */
                /* ./loongarch.decode.2:807 */
                info->opc = OPC_VAVG_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 01100110 1....... ........ */
                /* ./loongarch.decode.2:808 */
                info->opc = OPC_VAVG_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 01100111 0....... ........ */
                /* ./loongarch.decode.2:809 */
                info->opc = OPC_VAVG_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 01100111 1....... ........ */
                /* ./loongarch.decode.2:810 */
                info->opc = OPC_VAVG_DU;
                return true;
                break;
            }
            break;
        case 0x1a:
            /* 01110000 011010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01101000 0....... ........ */
                /* ./loongarch.decode.2:811 */
                info->opc = OPC_VAVGR_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01101000 1....... ........ */
                /* ./loongarch.decode.2:812 */
                info->opc = OPC_VAVGR_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01101001 0....... ........ */
                /* ./loongarch.decode.2:813 */
                info->opc = OPC_VAVGR_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01101001 1....... ........ */
                /* ./loongarch.decode.2:814 */
                info->opc = OPC_VAVGR_D;
                return true;
                break;
            case 0x4:
                /* 01110000 01101010 0....... ........ */
                /* ./loongarch.decode.2:815 */
                info->opc = OPC_VAVGR_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 01101010 1....... ........ */
                /* ./loongarch.decode.2:816 */
                info->opc = OPC_VAVGR_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 01101011 0....... ........ */
                /* ./loongarch.decode.2:817 */
                info->opc = OPC_VAVGR_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 01101011 1....... ........ */
                /* ./loongarch.decode.2:818 */
                info->opc = OPC_VAVGR_DU;
                return true;
                break;
            }
            break;
        case 0x1c:
            /* 01110000 011100.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01110000 0....... ........ */
                /* ./loongarch.decode.2:834 */
                info->opc = OPC_VMAX_B;
                return true;
                break;
            case 0x1:
                /* 01110000 01110000 1....... ........ */
                /* ./loongarch.decode.2:835 */
                info->opc = OPC_VMAX_H;
                return true;
                break;
            case 0x2:
                /* 01110000 01110001 0....... ........ */
                /* ./loongarch.decode.2:836 */
                info->opc = OPC_VMAX_W;
                return true;
                break;
            case 0x3:
                /* 01110000 01110001 1....... ........ */
                /* ./loongarch.decode.2:837 */
                info->opc = OPC_VMAX_D;
                return true;
                break;
            case 0x4:
                /* 01110000 01110010 0....... ........ */
                /* ./loongarch.decode.2:851 */
                info->opc = OPC_VMIN_B;
                return true;
                break;
            case 0x5:
                /* 01110000 01110010 1....... ........ */
                /* ./loongarch.decode.2:852 */
                info->opc = OPC_VMIN_H;
                return true;
                break;
            case 0x6:
                /* 01110000 01110011 0....... ........ */
                /* ./loongarch.decode.2:853 */
                info->opc = OPC_VMIN_W;
                return true;
                break;
            case 0x7:
                /* 01110000 01110011 1....... ........ */
                /* ./loongarch.decode.2:854 */
                info->opc = OPC_VMIN_D;
                return true;
                break;
            }
            break;
        case 0x1d:
            /* 01110000 011101.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 01110100 0....... ........ */
                /* ./loongarch.decode.2:842 */
                info->opc = OPC_VMAX_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 01110100 1....... ........ */
                /* ./loongarch.decode.2:843 */
                info->opc = OPC_VMAX_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 01110101 0....... ........ */
                /* ./loongarch.decode.2:844 */
                info->opc = OPC_VMAX_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 01110101 1....... ........ */
                /* ./loongarch.decode.2:845 */
                info->opc = OPC_VMAX_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 01110110 0....... ........ */
                /* ./loongarch.decode.2:859 */
                info->opc = OPC_VMIN_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 01110110 1....... ........ */
                /* ./loongarch.decode.2:860 */
                info->opc = OPC_VMIN_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 01110111 0....... ........ */
                /* ./loongarch.decode.2:861 */
                info->opc = OPC_VMIN_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 01110111 1....... ........ */
                /* ./loongarch.decode.2:862 */
                info->opc = OPC_VMIN_DU;
                return true;
                break;
            }
            break;
        case 0x21:
            /* 01110000 100001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10000100 0....... ........ */
                /* ./loongarch.decode.2:868 */
                info->opc = OPC_VMUL_B;
                return true;
                break;
            case 0x1:
                /* 01110000 10000100 1....... ........ */
                /* ./loongarch.decode.2:869 */
                info->opc = OPC_VMUL_H;
                return true;
                break;
            case 0x2:
                /* 01110000 10000101 0....... ........ */
                /* ./loongarch.decode.2:870 */
                info->opc = OPC_VMUL_W;
                return true;
                break;
            case 0x3:
                /* 01110000 10000101 1....... ........ */
                /* ./loongarch.decode.2:871 */
                info->opc = OPC_VMUL_D;
                return true;
                break;
            case 0x4:
                /* 01110000 10000110 0....... ........ */
                /* ./loongarch.decode.2:872 */
                info->opc = OPC_VMUH_B;
                return true;
                break;
            case 0x5:
                /* 01110000 10000110 1....... ........ */
                /* ./loongarch.decode.2:873 */
                info->opc = OPC_VMUH_H;
                return true;
                break;
            case 0x6:
                /* 01110000 10000111 0....... ........ */
                /* ./loongarch.decode.2:874 */
                info->opc = OPC_VMUH_W;
                return true;
                break;
            case 0x7:
                /* 01110000 10000111 1....... ........ */
                /* ./loongarch.decode.2:875 */
                info->opc = OPC_VMUH_D;
                return true;
                break;
            }
            break;
        case 0x22:
            /* 01110000 100010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10001000 0....... ........ */
                /* ./loongarch.decode.2:876 */
                info->opc = OPC_VMUH_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 10001000 1....... ........ */
                /* ./loongarch.decode.2:877 */
                info->opc = OPC_VMUH_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 10001001 0....... ........ */
                /* ./loongarch.decode.2:878 */
                info->opc = OPC_VMUH_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 10001001 1....... ........ */
                /* ./loongarch.decode.2:879 */
                info->opc = OPC_VMUH_DU;
                return true;
                break;
            }
            break;
        case 0x24:
            /* 01110000 100100.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10010000 0....... ........ */
                /* ./loongarch.decode.2:881 */
                info->opc = OPC_VMULWEV_H_B;
                return true;
                break;
            case 0x1:
                /* 01110000 10010000 1....... ........ */
                /* ./loongarch.decode.2:882 */
                info->opc = OPC_VMULWEV_W_H;
                return true;
                break;
            case 0x2:
                /* 01110000 10010001 0....... ........ */
                /* ./loongarch.decode.2:883 */
                info->opc = OPC_VMULWEV_D_W;
                return true;
                break;
            case 0x3:
                /* 01110000 10010001 1....... ........ */
                /* ./loongarch.decode.2:884 */
                info->opc = OPC_VMULWEV_Q_D;
                return true;
                break;
            case 0x4:
                /* 01110000 10010010 0....... ........ */
                /* ./loongarch.decode.2:885 */
                info->opc = OPC_VMULWOD_H_B;
                return true;
                break;
            case 0x5:
                /* 01110000 10010010 1....... ........ */
                /* ./loongarch.decode.2:886 */
                info->opc = OPC_VMULWOD_W_H;
                return true;
                break;
            case 0x6:
                /* 01110000 10010011 0....... ........ */
                /* ./loongarch.decode.2:887 */
                info->opc = OPC_VMULWOD_D_W;
                return true;
                break;
            case 0x7:
                /* 01110000 10010011 1....... ........ */
                /* ./loongarch.decode.2:888 */
                info->opc = OPC_VMULWOD_Q_D;
                return true;
                break;
            }
            break;
        case 0x26:
            /* 01110000 100110.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10011000 0....... ........ */
                /* ./loongarch.decode.2:889 */
                info->opc = OPC_VMULWEV_H_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 10011000 1....... ........ */
                /* ./loongarch.decode.2:890 */
                info->opc = OPC_VMULWEV_W_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 10011001 0....... ........ */
                /* ./loongarch.decode.2:891 */
                info->opc = OPC_VMULWEV_D_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 10011001 1....... ........ */
                /* ./loongarch.decode.2:892 */
                info->opc = OPC_VMULWEV_Q_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 10011010 0....... ........ */
                /* ./loongarch.decode.2:893 */
                info->opc = OPC_VMULWOD_H_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 10011010 1....... ........ */
                /* ./loongarch.decode.2:894 */
                info->opc = OPC_VMULWOD_W_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 10011011 0....... ........ */
                /* ./loongarch.decode.2:895 */
                info->opc = OPC_VMULWOD_D_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 10011011 1....... ........ */
                /* ./loongarch.decode.2:896 */
                info->opc = OPC_VMULWOD_Q_DU;
                return true;
                break;
            }
            break;
        case 0x28:
            /* 01110000 101000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10100000 0....... ........ */
                /* ./loongarch.decode.2:897 */
                info->opc = OPC_VMULWEV_H_BU_B;
                return true;
                break;
            case 0x1:
                /* 01110000 10100000 1....... ........ */
                /* ./loongarch.decode.2:898 */
                info->opc = OPC_VMULWEV_W_HU_H;
                return true;
                break;
            case 0x2:
                /* 01110000 10100001 0....... ........ */
                /* ./loongarch.decode.2:899 */
                info->opc = OPC_VMULWEV_D_WU_W;
                return true;
                break;
            case 0x3:
                /* 01110000 10100001 1....... ........ */
                /* ./loongarch.decode.2:900 */
                info->opc = OPC_VMULWEV_Q_DU_D;
                return true;
                break;
            case 0x4:
                /* 01110000 10100010 0....... ........ */
                /* ./loongarch.decode.2:901 */
                info->opc = OPC_VMULWOD_H_BU_B;
                return true;
                break;
            case 0x5:
                /* 01110000 10100010 1....... ........ */
                /* ./loongarch.decode.2:902 */
                info->opc = OPC_VMULWOD_W_HU_H;
                return true;
                break;
            case 0x6:
                /* 01110000 10100011 0....... ........ */
                /* ./loongarch.decode.2:903 */
                info->opc = OPC_VMULWOD_D_WU_W;
                return true;
                break;
            case 0x7:
                /* 01110000 10100011 1....... ........ */
                /* ./loongarch.decode.2:904 */
                info->opc = OPC_VMULWOD_Q_DU_D;
                return true;
                break;
            }
            break;
        case 0x2a:
            /* 01110000 101010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10101000 0....... ........ */
                /* ./loongarch.decode.2:906 */
                info->opc = OPC_VMADD_B;
                return true;
                break;
            case 0x1:
                /* 01110000 10101000 1....... ........ */
                /* ./loongarch.decode.2:907 */
                info->opc = OPC_VMADD_H;
                return true;
                break;
            case 0x2:
                /* 01110000 10101001 0....... ........ */
                /* ./loongarch.decode.2:908 */
                info->opc = OPC_VMADD_W;
                return true;
                break;
            case 0x3:
                /* 01110000 10101001 1....... ........ */
                /* ./loongarch.decode.2:909 */
                info->opc = OPC_VMADD_D;
                return true;
                break;
            case 0x4:
                /* 01110000 10101010 0....... ........ */
                /* ./loongarch.decode.2:910 */
                info->opc = OPC_VMSUB_B;
                return true;
                break;
            case 0x5:
                /* 01110000 10101010 1....... ........ */
                /* ./loongarch.decode.2:911 */
                info->opc = OPC_VMSUB_H;
                return true;
                break;
            case 0x6:
                /* 01110000 10101011 0....... ........ */
                /* ./loongarch.decode.2:912 */
                info->opc = OPC_VMSUB_W;
                return true;
                break;
            case 0x7:
                /* 01110000 10101011 1....... ........ */
                /* ./loongarch.decode.2:913 */
                info->opc = OPC_VMSUB_D;
                return true;
                break;
            }
            break;
        case 0x2b:
            /* 01110000 101011.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10101100 0....... ........ */
                /* ./loongarch.decode.2:915 */
                info->opc = OPC_VMADDWEV_H_B;
                return true;
                break;
            case 0x1:
                /* 01110000 10101100 1....... ........ */
                /* ./loongarch.decode.2:916 */
                info->opc = OPC_VMADDWEV_W_H;
                return true;
                break;
            case 0x2:
                /* 01110000 10101101 0....... ........ */
                /* ./loongarch.decode.2:917 */
                info->opc = OPC_VMADDWEV_D_W;
                return true;
                break;
            case 0x3:
                /* 01110000 10101101 1....... ........ */
                /* ./loongarch.decode.2:918 */
                info->opc = OPC_VMADDWEV_Q_D;
                return true;
                break;
            case 0x4:
                /* 01110000 10101110 0....... ........ */
                /* ./loongarch.decode.2:919 */
                info->opc = OPC_VMADDWOD_H_B;
                return true;
                break;
            case 0x5:
                /* 01110000 10101110 1....... ........ */
                /* ./loongarch.decode.2:920 */
                info->opc = OPC_VMADDWOD_W_H;
                return true;
                break;
            case 0x6:
                /* 01110000 10101111 0....... ........ */
                /* ./loongarch.decode.2:921 */
                info->opc = OPC_VMADDWOD_D_W;
                return true;
                break;
            case 0x7:
                /* 01110000 10101111 1....... ........ */
                /* ./loongarch.decode.2:922 */
                info->opc = OPC_VMADDWOD_Q_D;
                return true;
                break;
            }
            break;
        case 0x2d:
            /* 01110000 101101.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10110100 0....... ........ */
                /* ./loongarch.decode.2:923 */
                info->opc = OPC_VMADDWEV_H_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 10110100 1....... ........ */
                /* ./loongarch.decode.2:924 */
                info->opc = OPC_VMADDWEV_W_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 10110101 0....... ........ */
                /* ./loongarch.decode.2:925 */
                info->opc = OPC_VMADDWEV_D_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 10110101 1....... ........ */
                /* ./loongarch.decode.2:926 */
                info->opc = OPC_VMADDWEV_Q_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 10110110 0....... ........ */
                /* ./loongarch.decode.2:927 */
                info->opc = OPC_VMADDWOD_H_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 10110110 1....... ........ */
                /* ./loongarch.decode.2:928 */
                info->opc = OPC_VMADDWOD_W_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 10110111 0....... ........ */
                /* ./loongarch.decode.2:929 */
                info->opc = OPC_VMADDWOD_D_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 10110111 1....... ........ */
                /* ./loongarch.decode.2:930 */
                info->opc = OPC_VMADDWOD_Q_DU;
                return true;
                break;
            }
            break;
        case 0x2f:
            /* 01110000 101111.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 10111100 0....... ........ */
                /* ./loongarch.decode.2:931 */
                info->opc = OPC_VMADDWEV_H_BU_B;
                return true;
                break;
            case 0x1:
                /* 01110000 10111100 1....... ........ */
                /* ./loongarch.decode.2:932 */
                info->opc = OPC_VMADDWEV_W_HU_H;
                return true;
                break;
            case 0x2:
                /* 01110000 10111101 0....... ........ */
                /* ./loongarch.decode.2:933 */
                info->opc = OPC_VMADDWEV_D_WU_W;
                return true;
                break;
            case 0x3:
                /* 01110000 10111101 1....... ........ */
                /* ./loongarch.decode.2:934 */
                info->opc = OPC_VMADDWEV_Q_DU_D;
                return true;
                break;
            case 0x4:
                /* 01110000 10111110 0....... ........ */
                /* ./loongarch.decode.2:935 */
                info->opc = OPC_VMADDWOD_H_BU_B;
                return true;
                break;
            case 0x5:
                /* 01110000 10111110 1....... ........ */
                /* ./loongarch.decode.2:936 */
                info->opc = OPC_VMADDWOD_W_HU_H;
                return true;
                break;
            case 0x6:
                /* 01110000 10111111 0....... ........ */
                /* ./loongarch.decode.2:937 */
                info->opc = OPC_VMADDWOD_D_WU_W;
                return true;
                break;
            case 0x7:
                /* 01110000 10111111 1....... ........ */
                /* ./loongarch.decode.2:938 */
                info->opc = OPC_VMADDWOD_Q_DU_D;
                return true;
                break;
            }
            break;
        case 0x38:
            /* 01110000 111000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 11100000 0....... ........ */
                /* ./loongarch.decode.2:940 */
                info->opc = OPC_VDIV_B;
                return true;
                break;
            case 0x1:
                /* 01110000 11100000 1....... ........ */
                /* ./loongarch.decode.2:941 */
                info->opc = OPC_VDIV_H;
                return true;
                break;
            case 0x2:
                /* 01110000 11100001 0....... ........ */
                /* ./loongarch.decode.2:942 */
                info->opc = OPC_VDIV_W;
                return true;
                break;
            case 0x3:
                /* 01110000 11100001 1....... ........ */
                /* ./loongarch.decode.2:943 */
                info->opc = OPC_VDIV_D;
                return true;
                break;
            case 0x4:
                /* 01110000 11100010 0....... ........ */
                /* ./loongarch.decode.2:948 */
                info->opc = OPC_VMOD_B;
                return true;
                break;
            case 0x5:
                /* 01110000 11100010 1....... ........ */
                /* ./loongarch.decode.2:949 */
                info->opc = OPC_VMOD_H;
                return true;
                break;
            case 0x6:
                /* 01110000 11100011 0....... ........ */
                /* ./loongarch.decode.2:950 */
                info->opc = OPC_VMOD_W;
                return true;
                break;
            case 0x7:
                /* 01110000 11100011 1....... ........ */
                /* ./loongarch.decode.2:951 */
                info->opc = OPC_VMOD_D;
                return true;
                break;
            }
            break;
        case 0x39:
            /* 01110000 111001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 11100100 0....... ........ */
                /* ./loongarch.decode.2:944 */
                info->opc = OPC_VDIV_BU;
                return true;
                break;
            case 0x1:
                /* 01110000 11100100 1....... ........ */
                /* ./loongarch.decode.2:945 */
                info->opc = OPC_VDIV_HU;
                return true;
                break;
            case 0x2:
                /* 01110000 11100101 0....... ........ */
                /* ./loongarch.decode.2:946 */
                info->opc = OPC_VDIV_WU;
                return true;
                break;
            case 0x3:
                /* 01110000 11100101 1....... ........ */
                /* ./loongarch.decode.2:947 */
                info->opc = OPC_VDIV_DU;
                return true;
                break;
            case 0x4:
                /* 01110000 11100110 0....... ........ */
                /* ./loongarch.decode.2:952 */
                info->opc = OPC_VMOD_BU;
                return true;
                break;
            case 0x5:
                /* 01110000 11100110 1....... ........ */
                /* ./loongarch.decode.2:953 */
                info->opc = OPC_VMOD_HU;
                return true;
                break;
            case 0x6:
                /* 01110000 11100111 0....... ........ */
                /* ./loongarch.decode.2:954 */
                info->opc = OPC_VMOD_WU;
                return true;
                break;
            case 0x7:
                /* 01110000 11100111 1....... ........ */
                /* ./loongarch.decode.2:955 */
                info->opc = OPC_VMOD_DU;
                return true;
                break;
            }
            break;
        case 0x3a:
            /* 01110000 111010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 11101000 0....... ........ */
                /* ./loongarch.decode.2:1003 */
                info->opc = OPC_VSLL_B;
                return true;
                break;
            case 0x1:
                /* 01110000 11101000 1....... ........ */
                /* ./loongarch.decode.2:1004 */
                info->opc = OPC_VSLL_H;
                return true;
                break;
            case 0x2:
                /* 01110000 11101001 0....... ........ */
                /* ./loongarch.decode.2:1005 */
                info->opc = OPC_VSLL_W;
                return true;
                break;
            case 0x3:
                /* 01110000 11101001 1....... ........ */
                /* ./loongarch.decode.2:1006 */
                info->opc = OPC_VSLL_D;
                return true;
                break;
            case 0x4:
                /* 01110000 11101010 0....... ........ */
                /* ./loongarch.decode.2:1012 */
                info->opc = OPC_VSRL_B;
                return true;
                break;
            case 0x5:
                /* 01110000 11101010 1....... ........ */
                /* ./loongarch.decode.2:1013 */
                info->opc = OPC_VSRL_H;
                return true;
                break;
            case 0x6:
                /* 01110000 11101011 0....... ........ */
                /* ./loongarch.decode.2:1014 */
                info->opc = OPC_VSRL_W;
                return true;
                break;
            case 0x7:
                /* 01110000 11101011 1....... ........ */
                /* ./loongarch.decode.2:1015 */
                info->opc = OPC_VSRL_D;
                return true;
                break;
            }
            break;
        case 0x3b:
            /* 01110000 111011.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 11101100 0....... ........ */
                /* ./loongarch.decode.2:1021 */
                info->opc = OPC_VSRA_B;
                return true;
                break;
            case 0x1:
                /* 01110000 11101100 1....... ........ */
                /* ./loongarch.decode.2:1022 */
                info->opc = OPC_VSRA_H;
                return true;
                break;
            case 0x2:
                /* 01110000 11101101 0....... ........ */
                /* ./loongarch.decode.2:1023 */
                info->opc = OPC_VSRA_W;
                return true;
                break;
            case 0x3:
                /* 01110000 11101101 1....... ........ */
                /* ./loongarch.decode.2:1024 */
                info->opc = OPC_VSRA_D;
                return true;
                break;
            case 0x4:
                /* 01110000 11101110 0....... ........ */
                /* ./loongarch.decode.2:1030 */
                info->opc = OPC_VROTR_B;
                return true;
                break;
            case 0x5:
                /* 01110000 11101110 1....... ........ */
                /* ./loongarch.decode.2:1031 */
                info->opc = OPC_VROTR_H;
                return true;
                break;
            case 0x6:
                /* 01110000 11101111 0....... ........ */
                /* ./loongarch.decode.2:1032 */
                info->opc = OPC_VROTR_W;
                return true;
                break;
            case 0x7:
                /* 01110000 11101111 1....... ........ */
                /* ./loongarch.decode.2:1033 */
                info->opc = OPC_VROTR_D;
                return true;
                break;
            }
            break;
        case 0x3c:
            /* 01110000 111100.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110000 11110000 0....... ........ */
                /* ./loongarch.decode.2:1048 */
                info->opc = OPC_VSRLR_B;
                return true;
                break;
            case 0x1:
                /* 01110000 11110000 1....... ........ */
                /* ./loongarch.decode.2:1049 */
                info->opc = OPC_VSRLR_H;
                return true;
                break;
            case 0x2:
                /* 01110000 11110001 0....... ........ */
                /* ./loongarch.decode.2:1050 */
                info->opc = OPC_VSRLR_W;
                return true;
                break;
            case 0x3:
                /* 01110000 11110001 1....... ........ */
                /* ./loongarch.decode.2:1051 */
                info->opc = OPC_VSRLR_D;
                return true;
                break;
            case 0x4:
                /* 01110000 11110010 0....... ........ */
                /* ./loongarch.decode.2:1057 */
                info->opc = OPC_VSRAR_B;
                return true;
                break;
            case 0x5:
                /* 01110000 11110010 1....... ........ */
                /* ./loongarch.decode.2:1058 */
                info->opc = OPC_VSRAR_H;
                return true;
                break;
            case 0x6:
                /* 01110000 11110011 0....... ........ */
                /* ./loongarch.decode.2:1059 */
                info->opc = OPC_VSRAR_W;
                return true;
                break;
            case 0x7:
                /* 01110000 11110011 1....... ........ */
                /* ./loongarch.decode.2:1060 */
                info->opc = OPC_VSRAR_D;
                return true;
                break;
            }
            break;
        case 0x3d:
            /* 01110000 111101.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110000 11110100 1....... ........ */
                /* ./loongarch.decode.2:1066 */
                info->opc = OPC_VSRLN_B_H;
                return true;
                break;
            case 0x2:
                /* 01110000 11110101 0....... ........ */
                /* ./loongarch.decode.2:1067 */
                info->opc = OPC_VSRLN_H_W;
                return true;
                break;
            case 0x3:
                /* 01110000 11110101 1....... ........ */
                /* ./loongarch.decode.2:1068 */
                info->opc = OPC_VSRLN_W_D;
                return true;
                break;
            case 0x5:
                /* 01110000 11110110 1....... ........ */
                /* ./loongarch.decode.2:1069 */
                info->opc = OPC_VSRAN_B_H;
                return true;
                break;
            case 0x6:
                /* 01110000 11110111 0....... ........ */
                /* ./loongarch.decode.2:1070 */
                info->opc = OPC_VSRAN_H_W;
                return true;
                break;
            case 0x7:
                /* 01110000 11110111 1....... ........ */
                /* ./loongarch.decode.2:1071 */
                info->opc = OPC_VSRAN_W_D;
                return true;
                break;
            }
            break;
        case 0x3e:
            /* 01110000 111110.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110000 11111000 1....... ........ */
                /* ./loongarch.decode.2:1082 */
                info->opc = OPC_VSRLRN_B_H;
                return true;
                break;
            case 0x2:
                /* 01110000 11111001 0....... ........ */
                /* ./loongarch.decode.2:1083 */
                info->opc = OPC_VSRLRN_H_W;
                return true;
                break;
            case 0x3:
                /* 01110000 11111001 1....... ........ */
                /* ./loongarch.decode.2:1084 */
                info->opc = OPC_VSRLRN_W_D;
                return true;
                break;
            case 0x5:
                /* 01110000 11111010 1....... ........ */
                /* ./loongarch.decode.2:1085 */
                info->opc = OPC_VSRARN_B_H;
                return true;
                break;
            case 0x6:
                /* 01110000 11111011 0....... ........ */
                /* ./loongarch.decode.2:1086 */
                info->opc = OPC_VSRARN_H_W;
                return true;
                break;
            case 0x7:
                /* 01110000 11111011 1....... ........ */
                /* ./loongarch.decode.2:1087 */
                info->opc = OPC_VSRARN_W_D;
                return true;
                break;
            }
            break;
        case 0x3f:
            /* 01110000 111111.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110000 11111100 1....... ........ */
                /* ./loongarch.decode.2:1098 */
                info->opc = OPC_VSSRLN_B_H;
                return true;
                break;
            case 0x2:
                /* 01110000 11111101 0....... ........ */
                /* ./loongarch.decode.2:1099 */
                info->opc = OPC_VSSRLN_H_W;
                return true;
                break;
            case 0x3:
                /* 01110000 11111101 1....... ........ */
                /* ./loongarch.decode.2:1100 */
                info->opc = OPC_VSSRLN_W_D;
                return true;
                break;
            case 0x5:
                /* 01110000 11111110 1....... ........ */
                /* ./loongarch.decode.2:1101 */
                info->opc = OPC_VSSRAN_B_H;
                return true;
                break;
            case 0x6:
                /* 01110000 11111111 0....... ........ */
                /* ./loongarch.decode.2:1102 */
                info->opc = OPC_VSSRAN_H_W;
                return true;
                break;
            case 0x7:
                /* 01110000 11111111 1....... ........ */
                /* ./loongarch.decode.2:1103 */
                info->opc = OPC_VSSRAN_W_D;
                return true;
                break;
            }
            break;
        case 0x40:
            /* 01110001 000000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110001 00000000 1....... ........ */
                /* ./loongarch.decode.2:1128 */
                info->opc = OPC_VSSRLRN_B_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00000001 0....... ........ */
                /* ./loongarch.decode.2:1129 */
                info->opc = OPC_VSSRLRN_H_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00000001 1....... ........ */
                /* ./loongarch.decode.2:1130 */
                info->opc = OPC_VSSRLRN_W_D;
                return true;
                break;
            case 0x5:
                /* 01110001 00000010 1....... ........ */
                /* ./loongarch.decode.2:1131 */
                info->opc = OPC_VSSRARN_B_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00000011 0....... ........ */
                /* ./loongarch.decode.2:1132 */
                info->opc = OPC_VSSRARN_H_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00000011 1....... ........ */
                /* ./loongarch.decode.2:1133 */
                info->opc = OPC_VSSRARN_W_D;
                return true;
                break;
            }
            break;
        case 0x41:
            /* 01110001 000001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110001 00000100 1....... ........ */
                /* ./loongarch.decode.2:1104 */
                info->opc = OPC_VSSRLN_BU_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00000101 0....... ........ */
                /* ./loongarch.decode.2:1105 */
                info->opc = OPC_VSSRLN_HU_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00000101 1....... ........ */
                /* ./loongarch.decode.2:1106 */
                info->opc = OPC_VSSRLN_WU_D;
                return true;
                break;
            case 0x5:
                /* 01110001 00000110 1....... ........ */
                /* ./loongarch.decode.2:1107 */
                info->opc = OPC_VSSRAN_BU_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00000111 0....... ........ */
                /* ./loongarch.decode.2:1108 */
                info->opc = OPC_VSSRAN_HU_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00000111 1....... ........ */
                /* ./loongarch.decode.2:1109 */
                info->opc = OPC_VSSRAN_WU_D;
                return true;
                break;
            }
            break;
        case 0x42:
            /* 01110001 000010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110001 00001000 1....... ........ */
                /* ./loongarch.decode.2:1134 */
                info->opc = OPC_VSSRLRN_BU_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00001001 0....... ........ */
                /* ./loongarch.decode.2:1135 */
                info->opc = OPC_VSSRLRN_HU_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00001001 1....... ........ */
                /* ./loongarch.decode.2:1136 */
                info->opc = OPC_VSSRLRN_WU_D;
                return true;
                break;
            case 0x5:
                /* 01110001 00001010 1....... ........ */
                /* ./loongarch.decode.2:1137 */
                info->opc = OPC_VSSRARN_BU_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00001011 0....... ........ */
                /* ./loongarch.decode.2:1138 */
                info->opc = OPC_VSSRARN_HU_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00001011 1....... ........ */
                /* ./loongarch.decode.2:1139 */
                info->opc = OPC_VSSRARN_WU_D;
                return true;
                break;
            }
            break;
        case 0x43:
            /* 01110001 000011.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110001 00001100 0....... ........ */
                /* ./loongarch.decode.2:1172 */
                info->opc = OPC_VBITCLR_B;
                return true;
                break;
            case 0x1:
                /* 01110001 00001100 1....... ........ */
                /* ./loongarch.decode.2:1173 */
                info->opc = OPC_VBITCLR_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00001101 0....... ........ */
                /* ./loongarch.decode.2:1174 */
                info->opc = OPC_VBITCLR_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00001101 1....... ........ */
                /* ./loongarch.decode.2:1175 */
                info->opc = OPC_VBITCLR_D;
                return true;
                break;
            case 0x4:
                /* 01110001 00001110 0....... ........ */
                /* ./loongarch.decode.2:1180 */
                info->opc = OPC_VBITSET_B;
                return true;
                break;
            case 0x5:
                /* 01110001 00001110 1....... ........ */
                /* ./loongarch.decode.2:1181 */
                info->opc = OPC_VBITSET_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00001111 0....... ........ */
                /* ./loongarch.decode.2:1182 */
                info->opc = OPC_VBITSET_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00001111 1....... ........ */
                /* ./loongarch.decode.2:1183 */
                info->opc = OPC_VBITSET_D;
                return true;
                break;
            }
            break;
        case 0x44:
            /* 01110001 000100.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110001 00010000 0....... ........ */
                /* ./loongarch.decode.2:1188 */
                info->opc = OPC_VBITREV_B;
                return true;
                break;
            case 0x1:
                /* 01110001 00010000 1....... ........ */
                /* ./loongarch.decode.2:1189 */
                info->opc = OPC_VBITREV_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00010001 0....... ........ */
                /* ./loongarch.decode.2:1190 */
                info->opc = OPC_VBITREV_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00010001 1....... ........ */
                /* ./loongarch.decode.2:1191 */
                info->opc = OPC_VBITREV_D;
                return true;
                break;
            }
            break;
        case 0x45:
            /* 01110001 000101.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110001 00010110 0....... ........ */
                /* ./loongarch.decode.2:1393 */
                info->opc = OPC_VPACKEV_B;
                return true;
                break;
            case 0x5:
                /* 01110001 00010110 1....... ........ */
                /* ./loongarch.decode.2:1394 */
                info->opc = OPC_VPACKEV_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00010111 0....... ........ */
                /* ./loongarch.decode.2:1395 */
                info->opc = OPC_VPACKEV_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00010111 1....... ........ */
                /* ./loongarch.decode.2:1396 */
                info->opc = OPC_VPACKEV_D;
                return true;
                break;
            }
            break;
        case 0x46:
            /* 01110001 000110.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110001 00011000 0....... ........ */
                /* ./loongarch.decode.2:1397 */
                info->opc = OPC_VPACKOD_B;
                return true;
                break;
            case 0x1:
                /* 01110001 00011000 1....... ........ */
                /* ./loongarch.decode.2:1398 */
                info->opc = OPC_VPACKOD_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00011001 0....... ........ */
                /* ./loongarch.decode.2:1399 */
                info->opc = OPC_VPACKOD_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00011001 1....... ........ */
                /* ./loongarch.decode.2:1400 */
                info->opc = OPC_VPACKOD_D;
                return true;
                break;
            case 0x4:
                /* 01110001 00011010 0....... ........ */
                /* ./loongarch.decode.2:1411 */
                info->opc = OPC_VILVL_B;
                return true;
                break;
            case 0x5:
                /* 01110001 00011010 1....... ........ */
                /* ./loongarch.decode.2:1412 */
                info->opc = OPC_VILVL_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00011011 0....... ........ */
                /* ./loongarch.decode.2:1413 */
                info->opc = OPC_VILVL_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00011011 1....... ........ */
                /* ./loongarch.decode.2:1414 */
                info->opc = OPC_VILVL_D;
                return true;
                break;
            }
            break;
        case 0x47:
            /* 01110001 000111.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110001 00011100 0....... ........ */
                /* ./loongarch.decode.2:1415 */
                info->opc = OPC_VILVH_B;
                return true;
                break;
            case 0x1:
                /* 01110001 00011100 1....... ........ */
                /* ./loongarch.decode.2:1416 */
                info->opc = OPC_VILVH_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00011101 0....... ........ */
                /* ./loongarch.decode.2:1417 */
                info->opc = OPC_VILVH_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00011101 1....... ........ */
                /* ./loongarch.decode.2:1418 */
                info->opc = OPC_VILVH_D;
                return true;
                break;
            case 0x4:
                /* 01110001 00011110 0....... ........ */
                /* ./loongarch.decode.2:1402 */
                info->opc = OPC_VPICKEV_B;
                return true;
                break;
            case 0x5:
                /* 01110001 00011110 1....... ........ */
                /* ./loongarch.decode.2:1403 */
                info->opc = OPC_VPICKEV_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00011111 0....... ........ */
                /* ./loongarch.decode.2:1404 */
                info->opc = OPC_VPICKEV_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00011111 1....... ........ */
                /* ./loongarch.decode.2:1405 */
                info->opc = OPC_VPICKEV_D;
                return true;
                break;
            }
            break;
        case 0x48:
            /* 01110001 001000.. ........ ........ */
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110001 00100000 0....... ........ */
                /* ./loongarch.decode.2:1406 */
                decode_extract_vvv(info, insn);
                info->opc = OPC_VPICKOD_B;
                return true;
                break;
            case 0x1:
                /* 01110001 00100000 1....... ........ */
                /* ./loongarch.decode.2:1407 */
                decode_extract_vvv(info, insn);
                info->opc = OPC_VPICKOD_H;
                return true;
                break;
            case 0x2:
                /* 01110001 00100001 0....... ........ */
                /* ./loongarch.decode.2:1408 */
                decode_extract_vvv(info, insn);
                info->opc = OPC_VPICKOD_W;
                return true;
                break;
            case 0x3:
                /* 01110001 00100001 1....... ........ */
                /* ./loongarch.decode.2:1409 */
                decode_extract_vvv(info, insn);
                info->opc = OPC_VPICKOD_D;
                return true;
                break;
            case 0x4:
                /* 01110001 00100010 0....... ........ */
                /* ./loongarch.decode.2:1381 */
                decode_extract_vvr(info, insn);
                info->opc = OPC_VREPLVE_B;
                return true;
                break;
            case 0x5:
                /* 01110001 00100010 1....... ........ */
                /* ./loongarch.decode.2:1382 */
                decode_extract_vvr(info, insn);
                info->opc = OPC_VREPLVE_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00100011 0....... ........ */
                /* ./loongarch.decode.2:1383 */
                decode_extract_vvr(info, insn);
                info->opc = OPC_VREPLVE_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00100011 1....... ........ */
                /* ./loongarch.decode.2:1384 */
                decode_extract_vvr(info, insn);
                info->opc = OPC_VREPLVE_D;
                return true;
                break;
            }
            break;
        case 0x49:
            /* 01110001 001001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110001 00100110 0....... ........ */
                /* ./loongarch.decode.2:991 */
                info->opc = OPC_VAND_V;
                return true;
                break;
            case 0x5:
                /* 01110001 00100110 1....... ........ */
                /* ./loongarch.decode.2:992 */
                info->opc = OPC_VOR_V;
                return true;
                break;
            case 0x6:
                /* 01110001 00100111 0....... ........ */
                /* ./loongarch.decode.2:993 */
                info->opc = OPC_VXOR_V;
                return true;
                break;
            case 0x7:
                /* 01110001 00100111 1....... ........ */
                /* ./loongarch.decode.2:994 */
                info->opc = OPC_VNOR_V;
                return true;
                break;
            }
            break;
        case 0x4a:
            /* 01110001 001010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110001 00101000 0....... ........ */
                /* ./loongarch.decode.2:995 */
                info->opc = OPC_VANDN_V;
                return true;
                break;
            case 0x1:
                /* 01110001 00101000 1....... ........ */
                /* ./loongarch.decode.2:996 */
                info->opc = OPC_VORN_V;
                return true;
                break;
            case 0x6:
                /* 01110001 00101011 0....... ........ */
                /* ./loongarch.decode.2:1197 */
                info->opc = OPC_VFRSTP_B;
                return true;
                break;
            case 0x7:
                /* 01110001 00101011 1....... ........ */
                /* ./loongarch.decode.2:1198 */
                info->opc = OPC_VFRSTP_H;
                return true;
                break;
            }
            break;
        case 0x4b:
            /* 01110001 001011.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x2:
                /* 01110001 00101101 0....... ........ */
                /* ./loongarch.decode.2:701 */
                info->opc = OPC_VADD_Q;
                return true;
                break;
            case 0x3:
                /* 01110001 00101101 1....... ........ */
                /* ./loongarch.decode.2:706 */
                info->opc = OPC_VSUB_Q;
                return true;
                break;
            case 0x4:
                /* 01110001 00101110 0....... ........ */
                /* ./loongarch.decode.2:975 */
                info->opc = OPC_VSIGNCOV_B;
                return true;
                break;
            case 0x5:
                /* 01110001 00101110 1....... ........ */
                /* ./loongarch.decode.2:976 */
                info->opc = OPC_VSIGNCOV_H;
                return true;
                break;
            case 0x6:
                /* 01110001 00101111 0....... ........ */
                /* ./loongarch.decode.2:977 */
                info->opc = OPC_VSIGNCOV_W;
                return true;
                break;
            case 0x7:
                /* 01110001 00101111 1....... ........ */
                /* ./loongarch.decode.2:978 */
                info->opc = OPC_VSIGNCOV_D;
                return true;
                break;
            }
            break;
        case 0x4c:
            /* 01110001 001100.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110001 00110000 1....... ........ */
                /* ./loongarch.decode.2:1204 */
                info->opc = OPC_VFADD_S;
                return true;
                break;
            case 0x2:
                /* 01110001 00110001 0....... ........ */
                /* ./loongarch.decode.2:1205 */
                info->opc = OPC_VFADD_D;
                return true;
                break;
            case 0x5:
                /* 01110001 00110010 1....... ........ */
                /* ./loongarch.decode.2:1206 */
                info->opc = OPC_VFSUB_S;
                return true;
                break;
            case 0x6:
                /* 01110001 00110011 0....... ........ */
                /* ./loongarch.decode.2:1207 */
                info->opc = OPC_VFSUB_D;
                return true;
                break;
            }
            break;
        case 0x4e:
            /* 01110001 001110.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110001 00111000 1....... ........ */
                /* ./loongarch.decode.2:1208 */
                info->opc = OPC_VFMUL_S;
                return true;
                break;
            case 0x2:
                /* 01110001 00111001 0....... ........ */
                /* ./loongarch.decode.2:1209 */
                info->opc = OPC_VFMUL_D;
                return true;
                break;
            case 0x5:
                /* 01110001 00111010 1....... ........ */
                /* ./loongarch.decode.2:1210 */
                info->opc = OPC_VFDIV_S;
                return true;
                break;
            case 0x6:
                /* 01110001 00111011 0....... ........ */
                /* ./loongarch.decode.2:1211 */
                info->opc = OPC_VFDIV_D;
                return true;
                break;
            }
            break;
        case 0x4f:
            /* 01110001 001111.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110001 00111100 1....... ........ */
                /* ./loongarch.decode.2:1222 */
                info->opc = OPC_VFMAX_S;
                return true;
                break;
            case 0x2:
                /* 01110001 00111101 0....... ........ */
                /* ./loongarch.decode.2:1223 */
                info->opc = OPC_VFMAX_D;
                return true;
                break;
            case 0x5:
                /* 01110001 00111110 1....... ........ */
                /* ./loongarch.decode.2:1224 */
                info->opc = OPC_VFMIN_S;
                return true;
                break;
            case 0x6:
                /* 01110001 00111111 0....... ........ */
                /* ./loongarch.decode.2:1225 */
                info->opc = OPC_VFMIN_D;
                return true;
                break;
            }
            break;
        case 0x50:
            /* 01110001 010000.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x1:
                /* 01110001 01000000 1....... ........ */
                /* ./loongarch.decode.2:1227 */
                info->opc = OPC_VFMAXA_S;
                return true;
                break;
            case 0x2:
                /* 01110001 01000001 0....... ........ */
                /* ./loongarch.decode.2:1228 */
                info->opc = OPC_VFMAXA_D;
                return true;
                break;
            case 0x5:
                /* 01110001 01000010 1....... ........ */
                /* ./loongarch.decode.2:1229 */
                info->opc = OPC_VFMINA_S;
                return true;
                break;
            case 0x6:
                /* 01110001 01000011 0....... ........ */
                /* ./loongarch.decode.2:1230 */
                info->opc = OPC_VFMINA_D;
                return true;
                break;
            }
            break;
        case 0x51:
            /* 01110001 010001.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110001 01000110 0....... ........ */
                /* ./loongarch.decode.2:1249 */
                info->opc = OPC_VFCVT_H_S;
                return true;
                break;
            case 0x5:
                /* 01110001 01000110 1....... ........ */
                /* ./loongarch.decode.2:1250 */
                info->opc = OPC_VFCVT_S_D;
                return true;
                break;
            }
            break;
        case 0x52:
            /* 01110001 010010.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110001 01001000 0....... ........ */
                /* ./loongarch.decode.2:1299 */
                info->opc = OPC_VFFINT_S_L;
                return true;
                break;
            case 0x3:
                /* 01110001 01001001 1....... ........ */
                /* ./loongarch.decode.2:1277 */
                info->opc = OPC_VFTINT_W_D;
                return true;
                break;
            case 0x4:
                /* 01110001 01001010 0....... ........ */
                /* ./loongarch.decode.2:1278 */
                info->opc = OPC_VFTINTRM_W_D;
                return true;
                break;
            case 0x5:
                /* 01110001 01001010 1....... ........ */
                /* ./loongarch.decode.2:1279 */
                info->opc = OPC_VFTINTRP_W_D;
                return true;
                break;
            case 0x6:
                /* 01110001 01001011 0....... ........ */
                /* ./loongarch.decode.2:1280 */
                info->opc = OPC_VFTINTRZ_W_D;
                return true;
                break;
            case 0x7:
                /* 01110001 01001011 1....... ........ */
                /* ./loongarch.decode.2:1281 */
                info->opc = OPC_VFTINTRNE_W_D;
                return true;
                break;
            }
            break;
        case 0x5e:
            /* 01110001 011110.. ........ ........ */
            decode_extract_vvv(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x5:
                /* 01110001 01111010 1....... ........ */
                /* ./loongarch.decode.2:1421 */
                info->opc = OPC_VSHUF_H;
                return true;
                break;
            case 0x6:
                /* 01110001 01111011 0....... ........ */
                /* ./loongarch.decode.2:1422 */
                info->opc = OPC_VSHUF_W;
                return true;
                break;
            case 0x7:
                /* 01110001 01111011 1....... ........ */
                /* ./loongarch.decode.2:1423 */
                info->opc = OPC_VSHUF_D;
                return true;
                break;
            }
            break;
        case 0xa0:
            /* 01110010 100000.. ........ ........ */
            decode_extract_vv_i5(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110010 10000000 0....... ........ */
                /* ./loongarch.decode.2:1306 */
                info->opc = OPC_VSEQI_B;
                return true;
                break;
            case 0x1:
                /* 01110010 10000000 1....... ........ */
                /* ./loongarch.decode.2:1307 */
                info->opc = OPC_VSEQI_H;
                return true;
                break;
            case 0x2:
                /* 01110010 10000001 0....... ........ */
                /* ./loongarch.decode.2:1308 */
                info->opc = OPC_VSEQI_W;
                return true;
                break;
            case 0x3:
                /* 01110010 10000001 1....... ........ */
                /* ./loongarch.decode.2:1309 */
                info->opc = OPC_VSEQI_D;
                return true;
                break;
            case 0x4:
                /* 01110010 10000010 0....... ........ */
                /* ./loongarch.decode.2:1315 */
                info->opc = OPC_VSLEI_B;
                return true;
                break;
            case 0x5:
                /* 01110010 10000010 1....... ........ */
                /* ./loongarch.decode.2:1316 */
                info->opc = OPC_VSLEI_H;
                return true;
                break;
            case 0x6:
                /* 01110010 10000011 0....... ........ */
                /* ./loongarch.decode.2:1317 */
                info->opc = OPC_VSLEI_W;
                return true;
                break;
            case 0x7:
                /* 01110010 10000011 1....... ........ */
                /* ./loongarch.decode.2:1318 */
                info->opc = OPC_VSLEI_D;
                return true;
                break;
            }
            break;
        case 0xa1:
            /* 01110010 100001.. ........ ........ */
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110010 10000100 0....... ........ */
                /* ./loongarch.decode.2:1323 */
                decode_extract_vv_ui5(info, insn);
                info->opc = OPC_VSLEI_BU;
                return true;
                break;
            case 0x1:
                /* 01110010 10000100 1....... ........ */
                /* ./loongarch.decode.2:1324 */
                decode_extract_vv_ui5(info, insn);
                info->opc = OPC_VSLEI_HU;
                return true;
                break;
            case 0x2:
                /* 01110010 10000101 0....... ........ */
                /* ./loongarch.decode.2:1325 */
                decode_extract_vv_ui5(info, insn);
                info->opc = OPC_VSLEI_WU;
                return true;
                break;
            case 0x3:
                /* 01110010 10000101 1....... ........ */
                /* ./loongarch.decode.2:1326 */
                decode_extract_vv_ui5(info, insn);
                info->opc = OPC_VSLEI_DU;
                return true;
                break;
            case 0x4:
                /* 01110010 10000110 0....... ........ */
                /* ./loongarch.decode.2:1332 */
                decode_extract_vv_i5(info, insn);
                info->opc = OPC_VSLTI_B;
                return true;
                break;
            case 0x5:
                /* 01110010 10000110 1....... ........ */
                /* ./loongarch.decode.2:1333 */
                decode_extract_vv_i5(info, insn);
                info->opc = OPC_VSLTI_H;
                return true;
                break;
            case 0x6:
                /* 01110010 10000111 0....... ........ */
                /* ./loongarch.decode.2:1334 */
                decode_extract_vv_i5(info, insn);
                info->opc = OPC_VSLTI_W;
                return true;
                break;
            case 0x7:
                /* 01110010 10000111 1....... ........ */
                /* ./loongarch.decode.2:1335 */
                decode_extract_vv_i5(info, insn);
                info->opc = OPC_VSLTI_D;
                return true;
                break;
            }
            break;
        case 0xa2:
            /* 01110010 100010.. ........ ........ */
            decode_extract_vv_ui5(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110010 10001000 0....... ........ */
                /* ./loongarch.decode.2:1340 */
                info->opc = OPC_VSLTI_BU;
                return true;
                break;
            case 0x1:
                /* 01110010 10001000 1....... ........ */
                /* ./loongarch.decode.2:1341 */
                info->opc = OPC_VSLTI_HU;
                return true;
                break;
            case 0x2:
                /* 01110010 10001001 0....... ........ */
                /* ./loongarch.decode.2:1342 */
                info->opc = OPC_VSLTI_WU;
                return true;
                break;
            case 0x3:
                /* 01110010 10001001 1....... ........ */
                /* ./loongarch.decode.2:1343 */
                info->opc = OPC_VSLTI_DU;
                return true;
                break;
            case 0x4:
                /* 01110010 10001010 0....... ........ */
                /* ./loongarch.decode.2:708 */
                info->opc = OPC_VADDI_BU;
                return true;
                break;
            case 0x5:
                /* 01110010 10001010 1....... ........ */
                /* ./loongarch.decode.2:709 */
                info->opc = OPC_VADDI_HU;
                return true;
                break;
            case 0x6:
                /* 01110010 10001011 0....... ........ */
                /* ./loongarch.decode.2:710 */
                info->opc = OPC_VADDI_WU;
                return true;
                break;
            case 0x7:
                /* 01110010 10001011 1....... ........ */
                /* ./loongarch.decode.2:711 */
                info->opc = OPC_VADDI_DU;
                return true;
                break;
            }
            break;
        case 0xa3:
            /* 01110010 100011.. ........ ........ */
            decode_extract_vv_ui5(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110010 10001100 0....... ........ */
                /* ./loongarch.decode.2:712 */
                info->opc = OPC_VSUBI_BU;
                return true;
                break;
            case 0x1:
                /* 01110010 10001100 1....... ........ */
                /* ./loongarch.decode.2:713 */
                info->opc = OPC_VSUBI_HU;
                return true;
                break;
            case 0x2:
                /* 01110010 10001101 0....... ........ */
                /* ./loongarch.decode.2:714 */
                info->opc = OPC_VSUBI_WU;
                return true;
                break;
            case 0x3:
                /* 01110010 10001101 1....... ........ */
                /* ./loongarch.decode.2:715 */
                info->opc = OPC_VSUBI_DU;
                return true;
                break;
            case 0x4:
                /* 01110010 10001110 0....... ........ */
                /* ./loongarch.decode.2:1390 */
                info->opc = OPC_VBSLL_V;
                return true;
                break;
            case 0x5:
                /* 01110010 10001110 1....... ........ */
                /* ./loongarch.decode.2:1391 */
                info->opc = OPC_VBSRL_V;
                return true;
                break;
            }
            break;
        case 0xa4:
            /* 01110010 100100.. ........ ........ */
            decode_extract_vv_i5(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110010 10010000 0....... ........ */
                /* ./loongarch.decode.2:838 */
                info->opc = OPC_VMAXI_B;
                return true;
                break;
            case 0x1:
                /* 01110010 10010000 1....... ........ */
                /* ./loongarch.decode.2:839 */
                info->opc = OPC_VMAXI_H;
                return true;
                break;
            case 0x2:
                /* 01110010 10010001 0....... ........ */
                /* ./loongarch.decode.2:840 */
                info->opc = OPC_VMAXI_W;
                return true;
                break;
            case 0x3:
                /* 01110010 10010001 1....... ........ */
                /* ./loongarch.decode.2:841 */
                info->opc = OPC_VMAXI_D;
                return true;
                break;
            case 0x4:
                /* 01110010 10010010 0....... ........ */
                /* ./loongarch.decode.2:855 */
                info->opc = OPC_VMINI_B;
                return true;
                break;
            case 0x5:
                /* 01110010 10010010 1....... ........ */
                /* ./loongarch.decode.2:856 */
                info->opc = OPC_VMINI_H;
                return true;
                break;
            case 0x6:
                /* 01110010 10010011 0....... ........ */
                /* ./loongarch.decode.2:857 */
                info->opc = OPC_VMINI_W;
                return true;
                break;
            case 0x7:
                /* 01110010 10010011 1....... ........ */
                /* ./loongarch.decode.2:858 */
                info->opc = OPC_VMINI_D;
                return true;
                break;
            }
            break;
        case 0xa5:
            /* 01110010 100101.. ........ ........ */
            decode_extract_vv_ui5(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110010 10010100 0....... ........ */
                /* ./loongarch.decode.2:846 */
                info->opc = OPC_VMAXI_BU;
                return true;
                break;
            case 0x1:
                /* 01110010 10010100 1....... ........ */
                /* ./loongarch.decode.2:847 */
                info->opc = OPC_VMAXI_HU;
                return true;
                break;
            case 0x2:
                /* 01110010 10010101 0....... ........ */
                /* ./loongarch.decode.2:848 */
                info->opc = OPC_VMAXI_WU;
                return true;
                break;
            case 0x3:
                /* 01110010 10010101 1....... ........ */
                /* ./loongarch.decode.2:849 */
                info->opc = OPC_VMAXI_DU;
                return true;
                break;
            case 0x4:
                /* 01110010 10010110 0....... ........ */
                /* ./loongarch.decode.2:863 */
                info->opc = OPC_VMINI_BU;
                return true;
                break;
            case 0x5:
                /* 01110010 10010110 1....... ........ */
                /* ./loongarch.decode.2:864 */
                info->opc = OPC_VMINI_HU;
                return true;
                break;
            case 0x6:
                /* 01110010 10010111 0....... ........ */
                /* ./loongarch.decode.2:865 */
                info->opc = OPC_VMINI_WU;
                return true;
                break;
            case 0x7:
                /* 01110010 10010111 1....... ........ */
                /* ./loongarch.decode.2:866 */
                info->opc = OPC_VMINI_DU;
                return true;
                break;
            }
            break;
        case 0xa6:
            /* 01110010 100110.. ........ ........ */
            decode_extract_vv_ui5(info, insn);
            switch ((insn >> 15) & 0x7) {
            case 0x4:
                /* 01110010 10011010 0....... ........ */
                /* ./loongarch.decode.2:1199 */
                info->opc = OPC_VFRSTPI_B;
                return true;
                break;
            case 0x5:
                /* 01110010 10011010 1....... ........ */
                /* ./loongarch.decode.2:1200 */
                info->opc = OPC_VFRSTPI_H;
                return true;
                break;
            }
            break;
        case 0xa7:
            /* 01110010 100111.. ........ ........ */
            switch ((insn >> 10) & 0xff) {
            case 0x0:
                /* 01110010 10011100 000000.. ........ */
                /* ./loongarch.decode.2:1158 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLO_B;
                return true;
                break;
            case 0x1:
                /* 01110010 10011100 000001.. ........ */
                /* ./loongarch.decode.2:1159 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLO_H;
                return true;
                break;
            case 0x2:
                /* 01110010 10011100 000010.. ........ */
                /* ./loongarch.decode.2:1160 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLO_W;
                return true;
                break;
            case 0x3:
                /* 01110010 10011100 000011.. ........ */
                /* ./loongarch.decode.2:1161 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLO_D;
                return true;
                break;
            case 0x4:
                /* 01110010 10011100 000100.. ........ */
                /* ./loongarch.decode.2:1162 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLZ_B;
                return true;
                break;
            case 0x5:
                /* 01110010 10011100 000101.. ........ */
                /* ./loongarch.decode.2:1163 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLZ_H;
                return true;
                break;
            case 0x6:
                /* 01110010 10011100 000110.. ........ */
                /* ./loongarch.decode.2:1164 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLZ_W;
                return true;
                break;
            case 0x7:
                /* 01110010 10011100 000111.. ........ */
                /* ./loongarch.decode.2:1165 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VCLZ_D;
                return true;
                break;
            case 0x8:
                /* 01110010 10011100 001000.. ........ */
                /* ./loongarch.decode.2:1167 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VPCNT_B;
                return true;
                break;
            case 0x9:
                /* 01110010 10011100 001001.. ........ */
                /* ./loongarch.decode.2:1168 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VPCNT_H;
                return true;
                break;
            case 0xa:
                /* 01110010 10011100 001010.. ........ */
                /* ./loongarch.decode.2:1169 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VPCNT_W;
                return true;
                break;
            case 0xb:
                /* 01110010 10011100 001011.. ........ */
                /* ./loongarch.decode.2:1170 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VPCNT_D;
                return true;
                break;
            case 0xc:
                /* 01110010 10011100 001100.. ........ */
                /* ./loongarch.decode.2:717 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VNEG_B;
                return true;
                break;
            case 0xd:
                /* 01110010 10011100 001101.. ........ */
                /* ./loongarch.decode.2:718 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VNEG_H;
                return true;
                break;
            case 0xe:
                /* 01110010 10011100 001110.. ........ */
                /* ./loongarch.decode.2:719 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VNEG_W;
                return true;
                break;
            case 0xf:
                /* 01110010 10011100 001111.. ........ */
                /* ./loongarch.decode.2:720 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VNEG_D;
                return true;
                break;
            case 0x10:
                /* 01110010 10011100 010000.. ........ */
                /* ./loongarch.decode.2:980 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VMSKLTZ_B;
                return true;
                break;
            case 0x11:
                /* 01110010 10011100 010001.. ........ */
                /* ./loongarch.decode.2:981 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VMSKLTZ_H;
                return true;
                break;
            case 0x12:
                /* 01110010 10011100 010010.. ........ */
                /* ./loongarch.decode.2:982 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VMSKLTZ_W;
                return true;
                break;
            case 0x13:
                /* 01110010 10011100 010011.. ........ */
                /* ./loongarch.decode.2:983 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VMSKLTZ_D;
                return true;
                break;
            case 0x14:
                /* 01110010 10011100 010100.. ........ */
                /* ./loongarch.decode.2:985 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VMSKGEZ_B;
                return true;
                break;
            case 0x18:
                /* 01110010 10011100 011000.. ........ */
                /* ./loongarch.decode.2:986 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VMSKNZ_B;
                return true;
                break;
            case 0x26:
                /* 01110010 10011100 100110.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 100110.. ...00... */
                    /* ./loongarch.decode.2:1352 */
                    info->opc = OPC_VSETEQZ_V;
                    return true;
                    break;
                }
                break;
            case 0x27:
                /* 01110010 10011100 100111.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 100111.. ...00... */
                    /* ./loongarch.decode.2:1353 */
                    info->opc = OPC_VSETNEZ_V;
                    return true;
                    break;
                }
                break;
            case 0x28:
                /* 01110010 10011100 101000.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101000.. ...00... */
                    /* ./loongarch.decode.2:1354 */
                    info->opc = OPC_VSETANYEQZ_B;
                    return true;
                    break;
                }
                break;
            case 0x29:
                /* 01110010 10011100 101001.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101001.. ...00... */
                    /* ./loongarch.decode.2:1355 */
                    info->opc = OPC_VSETANYEQZ_H;
                    return true;
                    break;
                }
                break;
            case 0x2a:
                /* 01110010 10011100 101010.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101010.. ...00... */
                    /* ./loongarch.decode.2:1356 */
                    info->opc = OPC_VSETANYEQZ_W;
                    return true;
                    break;
                }
                break;
            case 0x2b:
                /* 01110010 10011100 101011.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101011.. ...00... */
                    /* ./loongarch.decode.2:1357 */
                    info->opc = OPC_VSETANYEQZ_D;
                    return true;
                    break;
                }
                break;
            case 0x2c:
                /* 01110010 10011100 101100.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101100.. ...00... */
                    /* ./loongarch.decode.2:1358 */
                    info->opc = OPC_VSETALLNEZ_B;
                    return true;
                    break;
                }
                break;
            case 0x2d:
                /* 01110010 10011100 101101.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101101.. ...00... */
                    /* ./loongarch.decode.2:1359 */
                    info->opc = OPC_VSETALLNEZ_H;
                    return true;
                    break;
                }
                break;
            case 0x2e:
                /* 01110010 10011100 101110.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101110.. ...00... */
                    /* ./loongarch.decode.2:1360 */
                    info->opc = OPC_VSETALLNEZ_W;
                    return true;
                    break;
                }
                break;
            case 0x2f:
                /* 01110010 10011100 101111.. ........ */
                decode_extract_cv(info, insn);
                switch ((insn >> 3) & 0x3) {
                case 0x0:
                    /* 01110010 10011100 101111.. ...00... */
                    /* ./loongarch.decode.2:1361 */
                    info->opc = OPC_VSETALLNEZ_D;
                    return true;
                    break;
                }
                break;
            case 0x31:
                /* 01110010 10011100 110001.. ........ */
                /* ./loongarch.decode.2:1232 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFLOGB_S;
                return true;
                break;
            case 0x32:
                /* 01110010 10011100 110010.. ........ */
                /* ./loongarch.decode.2:1233 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFLOGB_D;
                return true;
                break;
            case 0x35:
                /* 01110010 10011100 110101.. ........ */
                /* ./loongarch.decode.2:1235 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFCLASS_S;
                return true;
                break;
            case 0x36:
                /* 01110010 10011100 110110.. ........ */
                /* ./loongarch.decode.2:1236 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFCLASS_D;
                return true;
                break;
            case 0x39:
                /* 01110010 10011100 111001.. ........ */
                /* ./loongarch.decode.2:1238 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFSQRT_S;
                return true;
                break;
            case 0x3a:
                /* 01110010 10011100 111010.. ........ */
                /* ./loongarch.decode.2:1239 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFSQRT_D;
                return true;
                break;
            case 0x3d:
                /* 01110010 10011100 111101.. ........ */
                /* ./loongarch.decode.2:1240 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRECIP_S;
                return true;
                break;
            case 0x3e:
                /* 01110010 10011100 111110.. ........ */
                /* ./loongarch.decode.2:1241 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRECIP_D;
                return true;
                break;
            case 0x41:
                /* 01110010 10011101 000001.. ........ */
                /* ./loongarch.decode.2:1242 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRSQRT_S;
                return true;
                break;
            case 0x42:
                /* 01110010 10011101 000010.. ........ */
                /* ./loongarch.decode.2:1243 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRSQRT_D;
                return true;
                break;
            case 0x4d:
                /* 01110010 10011101 001101.. ........ */
                /* ./loongarch.decode.2:1252 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINT_S;
                return true;
                break;
            case 0x4e:
                /* 01110010 10011101 001110.. ........ */
                /* ./loongarch.decode.2:1253 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINT_D;
                return true;
                break;
            case 0x51:
                /* 01110010 10011101 010001.. ........ */
                /* ./loongarch.decode.2:1254 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRM_S;
                return true;
                break;
            case 0x52:
                /* 01110010 10011101 010010.. ........ */
                /* ./loongarch.decode.2:1255 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRM_D;
                return true;
                break;
            case 0x55:
                /* 01110010 10011101 010101.. ........ */
                /* ./loongarch.decode.2:1256 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRP_S;
                return true;
                break;
            case 0x56:
                /* 01110010 10011101 010110.. ........ */
                /* ./loongarch.decode.2:1257 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRP_D;
                return true;
                break;
            case 0x59:
                /* 01110010 10011101 011001.. ........ */
                /* ./loongarch.decode.2:1258 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRZ_S;
                return true;
                break;
            case 0x5a:
                /* 01110010 10011101 011010.. ........ */
                /* ./loongarch.decode.2:1259 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRZ_D;
                return true;
                break;
            case 0x5d:
                /* 01110010 10011101 011101.. ........ */
                /* ./loongarch.decode.2:1260 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRNE_S;
                return true;
                break;
            case 0x5e:
                /* 01110010 10011101 011110.. ........ */
                /* ./loongarch.decode.2:1261 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFRINTRNE_D;
                return true;
                break;
            case 0x7a:
                /* 01110010 10011101 111010.. ........ */
                /* ./loongarch.decode.2:1245 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFCVTL_S_H;
                return true;
                break;
            case 0x7b:
                /* 01110010 10011101 111011.. ........ */
                /* ./loongarch.decode.2:1246 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFCVTH_S_H;
                return true;
                break;
            case 0x7c:
                /* 01110010 10011101 111100.. ........ */
                /* ./loongarch.decode.2:1247 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFCVTL_D_S;
                return true;
                break;
            case 0x7d:
                /* 01110010 10011101 111101.. ........ */
                /* ./loongarch.decode.2:1248 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFCVTH_D_S;
                return true;
                break;
            case 0x80:
                /* 01110010 10011110 000000.. ........ */
                /* ./loongarch.decode.2:1293 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFFINT_S_W;
                return true;
                break;
            case 0x81:
                /* 01110010 10011110 000001.. ........ */
                /* ./loongarch.decode.2:1294 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFFINT_S_WU;
                return true;
                break;
            case 0x82:
                /* 01110010 10011110 000010.. ........ */
                /* ./loongarch.decode.2:1295 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFFINT_D_L;
                return true;
                break;
            case 0x83:
                /* 01110010 10011110 000011.. ........ */
                /* ./loongarch.decode.2:1296 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFFINT_D_LU;
                return true;
                break;
            case 0x84:
                /* 01110010 10011110 000100.. ........ */
                /* ./loongarch.decode.2:1297 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFFINTL_D_W;
                return true;
                break;
            case 0x85:
                /* 01110010 10011110 000101.. ........ */
                /* ./loongarch.decode.2:1298 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFFINTH_D_W;
                return true;
                break;
            case 0x8c:
                /* 01110010 10011110 001100.. ........ */
                /* ./loongarch.decode.2:1263 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINT_W_S;
                return true;
                break;
            case 0x8d:
                /* 01110010 10011110 001101.. ........ */
                /* ./loongarch.decode.2:1264 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINT_L_D;
                return true;
                break;
            case 0x8e:
                /* 01110010 10011110 001110.. ........ */
                /* ./loongarch.decode.2:1265 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRM_W_S;
                return true;
                break;
            case 0x8f:
                /* 01110010 10011110 001111.. ........ */
                /* ./loongarch.decode.2:1266 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRM_L_D;
                return true;
                break;
            case 0x90:
                /* 01110010 10011110 010000.. ........ */
                /* ./loongarch.decode.2:1267 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRP_W_S;
                return true;
                break;
            case 0x91:
                /* 01110010 10011110 010001.. ........ */
                /* ./loongarch.decode.2:1268 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRP_L_D;
                return true;
                break;
            case 0x92:
                /* 01110010 10011110 010010.. ........ */
                /* ./loongarch.decode.2:1269 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRZ_W_S;
                return true;
                break;
            case 0x93:
                /* 01110010 10011110 010011.. ........ */
                /* ./loongarch.decode.2:1270 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRZ_L_D;
                return true;
                break;
            case 0x94:
                /* 01110010 10011110 010100.. ........ */
                /* ./loongarch.decode.2:1271 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRNE_W_S;
                return true;
                break;
            case 0x95:
                /* 01110010 10011110 010101.. ........ */
                /* ./loongarch.decode.2:1272 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRNE_L_D;
                return true;
                break;
            case 0x96:
                /* 01110010 10011110 010110.. ........ */
                /* ./loongarch.decode.2:1273 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINT_WU_S;
                return true;
                break;
            case 0x97:
                /* 01110010 10011110 010111.. ........ */
                /* ./loongarch.decode.2:1274 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINT_LU_D;
                return true;
                break;
            case 0x9c:
                /* 01110010 10011110 011100.. ........ */
                /* ./loongarch.decode.2:1275 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRZ_WU_S;
                return true;
                break;
            case 0x9d:
                /* 01110010 10011110 011101.. ........ */
                /* ./loongarch.decode.2:1276 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRZ_LU_D;
                return true;
                break;
            case 0xa0:
                /* 01110010 10011110 100000.. ........ */
                /* ./loongarch.decode.2:1282 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTL_L_S;
                return true;
                break;
            case 0xa1:
                /* 01110010 10011110 100001.. ........ */
                /* ./loongarch.decode.2:1283 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTH_L_S;
                return true;
                break;
            case 0xa2:
                /* 01110010 10011110 100010.. ........ */
                /* ./loongarch.decode.2:1284 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRML_L_S;
                return true;
                break;
            case 0xa3:
                /* 01110010 10011110 100011.. ........ */
                /* ./loongarch.decode.2:1285 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRMH_L_S;
                return true;
                break;
            case 0xa4:
                /* 01110010 10011110 100100.. ........ */
                /* ./loongarch.decode.2:1286 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRPL_L_S;
                return true;
                break;
            case 0xa5:
                /* 01110010 10011110 100101.. ........ */
                /* ./loongarch.decode.2:1287 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRPH_L_S;
                return true;
                break;
            case 0xa6:
                /* 01110010 10011110 100110.. ........ */
                /* ./loongarch.decode.2:1288 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRZL_L_S;
                return true;
                break;
            case 0xa7:
                /* 01110010 10011110 100111.. ........ */
                /* ./loongarch.decode.2:1289 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRZH_L_S;
                return true;
                break;
            case 0xa8:
                /* 01110010 10011110 101000.. ........ */
                /* ./loongarch.decode.2:1290 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRNEL_L_S;
                return true;
                break;
            case 0xa9:
                /* 01110010 10011110 101001.. ........ */
                /* ./loongarch.decode.2:1291 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VFTINTRNEH_L_S;
                return true;
                break;
            case 0xb8:
                /* 01110010 10011110 111000.. ........ */
                /* ./loongarch.decode.2:966 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_H_B;
                return true;
                break;
            case 0xb9:
                /* 01110010 10011110 111001.. ........ */
                /* ./loongarch.decode.2:967 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_W_H;
                return true;
                break;
            case 0xba:
                /* 01110010 10011110 111010.. ........ */
                /* ./loongarch.decode.2:968 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_D_W;
                return true;
                break;
            case 0xbb:
                /* 01110010 10011110 111011.. ........ */
                /* ./loongarch.decode.2:969 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_Q_D;
                return true;
                break;
            case 0xbc:
                /* 01110010 10011110 111100.. ........ */
                /* ./loongarch.decode.2:970 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_HU_BU;
                return true;
                break;
            case 0xbd:
                /* 01110010 10011110 111101.. ........ */
                /* ./loongarch.decode.2:971 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_WU_HU;
                return true;
                break;
            case 0xbe:
                /* 01110010 10011110 111110.. ........ */
                /* ./loongarch.decode.2:972 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_DU_WU;
                return true;
                break;
            case 0xbf:
                /* 01110010 10011110 111111.. ........ */
                /* ./loongarch.decode.2:973 */
                decode_extract_vv(info, insn);
                info->opc = OPC_VEXTH_QU_DU;
                return true;
                break;
            case 0xc0:
                /* 01110010 10011111 000000.. ........ */
                /* ./loongarch.decode.2:1376 */
                decode_extract_vr(info, insn);
                info->opc = OPC_VREPLGR2VR_B;
                return true;
                break;
            case 0xc1:
                /* 01110010 10011111 000001.. ........ */
                /* ./loongarch.decode.2:1377 */
                decode_extract_vr(info, insn);
                info->opc = OPC_VREPLGR2VR_H;
                return true;
                break;
            case 0xc2:
                /* 01110010 10011111 000010.. ........ */
                /* ./loongarch.decode.2:1378 */
                decode_extract_vr(info, insn);
                info->opc = OPC_VREPLGR2VR_W;
                return true;
                break;
            case 0xc3:
                /* 01110010 10011111 000011.. ........ */
                /* ./loongarch.decode.2:1379 */
                decode_extract_vr(info, insn);
                info->opc = OPC_VREPLGR2VR_D;
                return true;
                break;
            }
            break;
        case 0xa8:
            /* 01110010 101000.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110010 10100000 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110010 10100000 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110010 10100000 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110010 10100000 001..... ........ */
                            /* ./loongarch.decode.2:1034 */
                            info->opc = OPC_VROTRI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110010 10100000 01...... ........ */
                        /* ./loongarch.decode.2:1035 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VROTRI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110010 10100000 1....... ........ */
                    /* ./loongarch.decode.2:1036 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VROTRI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110010 10100001 ........ ........ */
                /* ./loongarch.decode.2:1037 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VROTRI_D;
                return true;
                break;
            }
            break;
        case 0xa9:
            /* 01110010 101001.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110010 10100100 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110010 10100100 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110010 10100100 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110010 10100100 001..... ........ */
                            /* ./loongarch.decode.2:1052 */
                            info->opc = OPC_VSRLRI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110010 10100100 01...... ........ */
                        /* ./loongarch.decode.2:1053 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VSRLRI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110010 10100100 1....... ........ */
                    /* ./loongarch.decode.2:1054 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VSRLRI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110010 10100101 ........ ........ */
                /* ./loongarch.decode.2:1055 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VSRLRI_D;
                return true;
                break;
            }
            break;
        case 0xaa:
            /* 01110010 101010.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110010 10101000 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110010 10101000 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110010 10101000 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110010 10101000 001..... ........ */
                            /* ./loongarch.decode.2:1061 */
                            info->opc = OPC_VSRARI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110010 10101000 01...... ........ */
                        /* ./loongarch.decode.2:1062 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VSRARI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110010 10101000 1....... ........ */
                    /* ./loongarch.decode.2:1063 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VSRARI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110010 10101001 ........ ........ */
                /* ./loongarch.decode.2:1064 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VSRARI_D;
                return true;
                break;
            }
            break;
        case 0xba:
            /* 01110010 111010.. ........ ........ */
            switch ((insn >> 14) & 0xf) {
            case 0xe:
                /* 01110010 11101011 10...... ........ */
                /* ./loongarch.decode.2:1363 */
                decode_extract_vr_ui4(info, insn);
                info->opc = OPC_VINSGR2VR_B;
                return true;
                break;
            case 0xf:
                /* 01110010 11101011 11...... ........ */
                switch ((insn >> 13) & 0x1) {
                case 0x0:
                    /* 01110010 11101011 110..... ........ */
                    /* ./loongarch.decode.2:1364 */
                    decode_extract_vr_ui3(info, insn);
                    info->opc = OPC_VINSGR2VR_H;
                    return true;
                    break;
                case 0x1:
                    /* 01110010 11101011 111..... ........ */
                    switch ((insn >> 12) & 0x1) {
                    case 0x0:
                        /* 01110010 11101011 1110.... ........ */
                        /* ./loongarch.decode.2:1365 */
                        decode_extract_vr_ui2(info, insn);
                        info->opc = OPC_VINSGR2VR_W;
                        return true;
                        break;
                    case 0x1:
                        /* 01110010 11101011 1111.... ........ */
                        decode_extract_vr_ui1(info, insn);
                        switch ((insn >> 11) & 0x1) {
                        case 0x0:
                            /* 01110010 11101011 11110... ........ */
                            /* ./loongarch.decode.2:1366 */
                            info->opc = OPC_VINSGR2VR_D;
                            return true;
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        case 0xbb:
            /* 01110010 111011.. ........ ........ */
            switch ((insn >> 14) & 0xf) {
            case 0xe:
                /* 01110010 11101111 10...... ........ */
                /* ./loongarch.decode.2:1367 */
                decode_extract_rv_ui4(info, insn);
                info->opc = OPC_VPICKVE2GR_B;
                return true;
                break;
            case 0xf:
                /* 01110010 11101111 11...... ........ */
                switch ((insn >> 13) & 0x1) {
                case 0x0:
                    /* 01110010 11101111 110..... ........ */
                    /* ./loongarch.decode.2:1368 */
                    decode_extract_rv_ui3(info, insn);
                    info->opc = OPC_VPICKVE2GR_H;
                    return true;
                    break;
                case 0x1:
                    /* 01110010 11101111 111..... ........ */
                    switch ((insn >> 12) & 0x1) {
                    case 0x0:
                        /* 01110010 11101111 1110.... ........ */
                        /* ./loongarch.decode.2:1369 */
                        decode_extract_rv_ui2(info, insn);
                        info->opc = OPC_VPICKVE2GR_W;
                        return true;
                        break;
                    case 0x1:
                        /* 01110010 11101111 1111.... ........ */
                        decode_extract_rv_ui1(info, insn);
                        switch ((insn >> 11) & 0x1) {
                        case 0x0:
                            /* 01110010 11101111 11110... ........ */
                            /* ./loongarch.decode.2:1370 */
                            info->opc = OPC_VPICKVE2GR_D;
                            return true;
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        case 0xbc:
            /* 01110010 111100.. ........ ........ */
            switch ((insn >> 14) & 0xf) {
            case 0xe:
                /* 01110010 11110011 10...... ........ */
                /* ./loongarch.decode.2:1371 */
                decode_extract_rv_ui4(info, insn);
                info->opc = OPC_VPICKVE2GR_BU;
                return true;
                break;
            case 0xf:
                /* 01110010 11110011 11...... ........ */
                switch ((insn >> 13) & 0x1) {
                case 0x0:
                    /* 01110010 11110011 110..... ........ */
                    /* ./loongarch.decode.2:1372 */
                    decode_extract_rv_ui3(info, insn);
                    info->opc = OPC_VPICKVE2GR_HU;
                    return true;
                    break;
                case 0x1:
                    /* 01110010 11110011 111..... ........ */
                    switch ((insn >> 12) & 0x1) {
                    case 0x0:
                        /* 01110010 11110011 1110.... ........ */
                        /* ./loongarch.decode.2:1373 */
                        decode_extract_rv_ui2(info, insn);
                        info->opc = OPC_VPICKVE2GR_WU;
                        return true;
                        break;
                    case 0x1:
                        /* 01110010 11110011 1111.... ........ */
                        decode_extract_rv_ui1(info, insn);
                        switch ((insn >> 11) & 0x1) {
                        case 0x0:
                            /* 01110010 11110011 11110... ........ */
                            /* ./loongarch.decode.2:1374 */
                            info->opc = OPC_VPICKVE2GR_DU;
                            return true;
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        case 0xbd:
            /* 01110010 111101.. ........ ........ */
            switch ((insn >> 14) & 0xf) {
            case 0xe:
                /* 01110010 11110111 10...... ........ */
                /* ./loongarch.decode.2:1385 */
                decode_extract_vv_ui4(info, insn);
                info->opc = OPC_VREPLVEI_B;
                return true;
                break;
            case 0xf:
                /* 01110010 11110111 11...... ........ */
                switch ((insn >> 13) & 0x1) {
                case 0x0:
                    /* 01110010 11110111 110..... ........ */
                    /* ./loongarch.decode.2:1386 */
                    decode_extract_vv_ui3(info, insn);
                    info->opc = OPC_VREPLVEI_H;
                    return true;
                    break;
                case 0x1:
                    /* 01110010 11110111 111..... ........ */
                    switch ((insn >> 12) & 0x1) {
                    case 0x0:
                        /* 01110010 11110111 1110.... ........ */
                        /* ./loongarch.decode.2:1387 */
                        decode_extract_vv_ui2(info, insn);
                        info->opc = OPC_VREPLVEI_W;
                        return true;
                        break;
                    case 0x1:
                        /* 01110010 11110111 1111.... ........ */
                        decode_extract_vv_ui1(info, insn);
                        switch ((insn >> 11) & 0x1) {
                        case 0x0:
                            /* 01110010 11110111 11110... ........ */
                            /* ./loongarch.decode.2:1388 */
                            info->opc = OPC_VREPLVEI_D;
                            return true;
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        case 0xc2:
            /* 01110011 000010.. ........ ........ */
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110011 00001000 0....... ........ */
                switch ((insn >> 14) & 0x1) {
                case 0x0:
                    /* 01110011 00001000 00...... ........ */
                    decode_extract_vv_ui3(info, insn);
                    switch ((insn >> 13) & 0x1) {
                    case 0x1:
                        /* 01110011 00001000 001..... ........ */
                        /* ./loongarch.decode.2:1039 */
                        info->opc = OPC_VSLLWIL_H_B;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00001000 01...... ........ */
                    /* ./loongarch.decode.2:1040 */
                    decode_extract_vv_ui4(info, insn);
                    info->opc = OPC_VSLLWIL_W_H;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00001000 1....... ........ */
                /* ./loongarch.decode.2:1041 */
                decode_extract_vv_ui5(info, insn);
                info->opc = OPC_VSLLWIL_D_W;
                return true;
                break;
            case 0x2:
                /* 01110011 00001001 0....... ........ */
                decode_extract_vv(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x0:
                    /* 01110011 00001001 000000.. ........ */
                    /* ./loongarch.decode.2:1042 */
                    info->opc = OPC_VEXTL_Q_D;
                    return true;
                    break;
                }
                break;
            }
            break;
        case 0xc3:
            /* 01110011 000011.. ........ ........ */
            switch ((insn >> 15) & 0x7) {
            case 0x0:
                /* 01110011 00001100 0....... ........ */
                switch ((insn >> 14) & 0x1) {
                case 0x0:
                    /* 01110011 00001100 00...... ........ */
                    decode_extract_vv_ui3(info, insn);
                    switch ((insn >> 13) & 0x1) {
                    case 0x1:
                        /* 01110011 00001100 001..... ........ */
                        /* ./loongarch.decode.2:1043 */
                        info->opc = OPC_VSLLWIL_HU_BU;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00001100 01...... ........ */
                    /* ./loongarch.decode.2:1044 */
                    decode_extract_vv_ui4(info, insn);
                    info->opc = OPC_VSLLWIL_WU_HU;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00001100 1....... ........ */
                /* ./loongarch.decode.2:1045 */
                decode_extract_vv_ui5(info, insn);
                info->opc = OPC_VSLLWIL_DU_WU;
                return true;
                break;
            case 0x2:
                /* 01110011 00001101 0....... ........ */
                decode_extract_vv(info, insn);
                switch ((insn >> 10) & 0x1f) {
                case 0x0:
                    /* 01110011 00001101 000000.. ........ */
                    /* ./loongarch.decode.2:1046 */
                    info->opc = OPC_VEXTL_QU_DU;
                    return true;
                    break;
                }
                break;
            }
            break;
        case 0xc4:
            /* 01110011 000100.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00010000 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00010000 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00010000 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00010000 001..... ........ */
                            /* ./loongarch.decode.2:1176 */
                            info->opc = OPC_VBITCLRI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00010000 01...... ........ */
                        /* ./loongarch.decode.2:1177 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VBITCLRI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00010000 1....... ........ */
                    /* ./loongarch.decode.2:1178 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VBITCLRI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00010001 ........ ........ */
                /* ./loongarch.decode.2:1179 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VBITCLRI_D;
                return true;
                break;
            }
            break;
        case 0xc5:
            /* 01110011 000101.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00010100 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00010100 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00010100 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00010100 001..... ........ */
                            /* ./loongarch.decode.2:1184 */
                            info->opc = OPC_VBITSETI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00010100 01...... ........ */
                        /* ./loongarch.decode.2:1185 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VBITSETI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00010100 1....... ........ */
                    /* ./loongarch.decode.2:1186 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VBITSETI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00010101 ........ ........ */
                /* ./loongarch.decode.2:1187 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VBITSETI_D;
                return true;
                break;
            }
            break;
        case 0xc6:
            /* 01110011 000110.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00011000 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00011000 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00011000 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00011000 001..... ........ */
                            /* ./loongarch.decode.2:1192 */
                            info->opc = OPC_VBITREVI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00011000 01...... ........ */
                        /* ./loongarch.decode.2:1193 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VBITREVI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00011000 1....... ........ */
                    /* ./loongarch.decode.2:1194 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VBITREVI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00011001 ........ ........ */
                /* ./loongarch.decode.2:1195 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VBITREVI_D;
                return true;
                break;
            }
            break;
        case 0xc9:
            /* 01110011 001001.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00100100 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00100100 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00100100 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00100100 001..... ........ */
                            /* ./loongarch.decode.2:957 */
                            info->opc = OPC_VSAT_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00100100 01...... ........ */
                        /* ./loongarch.decode.2:958 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VSAT_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00100100 1....... ........ */
                    /* ./loongarch.decode.2:959 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VSAT_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00100101 ........ ........ */
                /* ./loongarch.decode.2:960 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VSAT_D;
                return true;
                break;
            }
            break;
        case 0xca:
            /* 01110011 001010.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00101000 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00101000 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00101000 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00101000 001..... ........ */
                            /* ./loongarch.decode.2:961 */
                            info->opc = OPC_VSAT_BU;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00101000 01...... ........ */
                        /* ./loongarch.decode.2:962 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VSAT_HU;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00101000 1....... ........ */
                    /* ./loongarch.decode.2:963 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VSAT_WU;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00101001 ........ ........ */
                /* ./loongarch.decode.2:964 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VSAT_DU;
                return true;
                break;
            }
            break;
        case 0xcb:
            /* 01110011 001011.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00101100 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00101100 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00101100 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00101100 001..... ........ */
                            /* ./loongarch.decode.2:1007 */
                            info->opc = OPC_VSLLI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00101100 01...... ........ */
                        /* ./loongarch.decode.2:1008 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VSLLI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00101100 1....... ........ */
                    /* ./loongarch.decode.2:1009 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VSLLI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00101101 ........ ........ */
                /* ./loongarch.decode.2:1010 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VSLLI_D;
                return true;
                break;
            }
            break;
        case 0xcc:
            /* 01110011 001100.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00110000 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00110000 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00110000 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00110000 001..... ........ */
                            /* ./loongarch.decode.2:1016 */
                            info->opc = OPC_VSRLI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00110000 01...... ........ */
                        /* ./loongarch.decode.2:1017 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VSRLI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00110000 1....... ........ */
                    /* ./loongarch.decode.2:1018 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VSRLI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00110001 ........ ........ */
                /* ./loongarch.decode.2:1019 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VSRLI_D;
                return true;
                break;
            }
            break;
        case 0xcd:
            /* 01110011 001101.. ........ ........ */
            switch ((insn >> 16) & 0x3) {
            case 0x0:
                /* 01110011 00110100 ........ ........ */
                switch ((insn >> 15) & 0x1) {
                case 0x0:
                    /* 01110011 00110100 0....... ........ */
                    switch ((insn >> 14) & 0x1) {
                    case 0x0:
                        /* 01110011 00110100 00...... ........ */
                        decode_extract_vv_ui3(info, insn);
                        switch ((insn >> 13) & 0x1) {
                        case 0x1:
                            /* 01110011 00110100 001..... ........ */
                            /* ./loongarch.decode.2:1025 */
                            info->opc = OPC_VSRAI_B;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 00110100 01...... ........ */
                        /* ./loongarch.decode.2:1026 */
                        decode_extract_vv_ui4(info, insn);
                        info->opc = OPC_VSRAI_H;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 00110100 1....... ........ */
                    /* ./loongarch.decode.2:1027 */
                    decode_extract_vv_ui5(info, insn);
                    info->opc = OPC_VSRAI_W;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 00110101 ........ ........ */
                /* ./loongarch.decode.2:1028 */
                decode_extract_vv_ui6(info, insn);
                info->opc = OPC_VSRAI_D;
                return true;
                break;
            }
            break;
        case 0xd0:
            /* 01110011 010000.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0100000. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01000000 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01000000 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01000000 01...... ........ */
                            /* ./loongarch.decode.2:1073 */
                            info->opc = OPC_VSRLNI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01000000 1....... ........ */
                        /* ./loongarch.decode.2:1074 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSRLNI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01000001 ........ ........ */
                    /* ./loongarch.decode.2:1075 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSRLNI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0100001. ........ ........ */
                /* ./loongarch.decode.2:1076 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSRLNI_D_Q;
                return true;
                break;
            }
            break;
        case 0xd1:
            /* 01110011 010001.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0100010. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01000100 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01000100 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01000100 01...... ........ */
                            /* ./loongarch.decode.2:1089 */
                            info->opc = OPC_VSRLRNI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01000100 1....... ........ */
                        /* ./loongarch.decode.2:1090 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSRLRNI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01000101 ........ ........ */
                    /* ./loongarch.decode.2:1091 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSRLRNI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0100011. ........ ........ */
                /* ./loongarch.decode.2:1092 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSRLRNI_D_Q;
                return true;
                break;
            }
            break;
        case 0xd2:
            /* 01110011 010010.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0100100. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01001000 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01001000 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01001000 01...... ........ */
                            /* ./loongarch.decode.2:1111 */
                            info->opc = OPC_VSSRLNI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01001000 1....... ........ */
                        /* ./loongarch.decode.2:1112 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRLNI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01001001 ........ ........ */
                    /* ./loongarch.decode.2:1113 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRLNI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0100101. ........ ........ */
                /* ./loongarch.decode.2:1114 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRLNI_D_Q;
                return true;
                break;
            }
            break;
        case 0xd3:
            /* 01110011 010011.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0100110. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01001100 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01001100 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01001100 01...... ........ */
                            /* ./loongarch.decode.2:1119 */
                            info->opc = OPC_VSSRLNI_BU_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01001100 1....... ........ */
                        /* ./loongarch.decode.2:1120 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRLNI_HU_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01001101 ........ ........ */
                    /* ./loongarch.decode.2:1121 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRLNI_WU_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0100111. ........ ........ */
                /* ./loongarch.decode.2:1122 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRLNI_DU_Q;
                return true;
                break;
            }
            break;
        case 0xd4:
            /* 01110011 010100.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0101000. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01010000 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01010000 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01010000 01...... ........ */
                            /* ./loongarch.decode.2:1141 */
                            info->opc = OPC_VSSRLRNI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01010000 1....... ........ */
                        /* ./loongarch.decode.2:1142 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRLRNI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01010001 ........ ........ */
                    /* ./loongarch.decode.2:1143 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRLRNI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0101001. ........ ........ */
                /* ./loongarch.decode.2:1144 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRLRNI_D_Q;
                return true;
                break;
            }
            break;
        case 0xd5:
            /* 01110011 010101.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0101010. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01010100 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01010100 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01010100 01...... ........ */
                            /* ./loongarch.decode.2:1149 */
                            info->opc = OPC_VSSRLRNI_BU_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01010100 1....... ........ */
                        /* ./loongarch.decode.2:1150 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRLRNI_HU_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01010101 ........ ........ */
                    /* ./loongarch.decode.2:1151 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRLRNI_WU_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0101011. ........ ........ */
                /* ./loongarch.decode.2:1152 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRLRNI_DU_Q;
                return true;
                break;
            }
            break;
        case 0xd6:
            /* 01110011 010110.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0101100. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01011000 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01011000 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01011000 01...... ........ */
                            /* ./loongarch.decode.2:1077 */
                            info->opc = OPC_VSRANI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01011000 1....... ........ */
                        /* ./loongarch.decode.2:1078 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSRANI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01011001 ........ ........ */
                    /* ./loongarch.decode.2:1079 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSRANI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0101101. ........ ........ */
                /* ./loongarch.decode.2:1080 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSRANI_D_Q;
                return true;
                break;
            }
            break;
        case 0xd7:
            /* 01110011 010111.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0101110. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01011100 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01011100 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01011100 01...... ........ */
                            /* ./loongarch.decode.2:1093 */
                            info->opc = OPC_VSRARNI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01011100 1....... ........ */
                        /* ./loongarch.decode.2:1094 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSRARNI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01011101 ........ ........ */
                    /* ./loongarch.decode.2:1095 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSRARNI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0101111. ........ ........ */
                /* ./loongarch.decode.2:1096 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSRARNI_D_Q;
                return true;
                break;
            }
            break;
        case 0xd8:
            /* 01110011 011000.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0110000. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01100000 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01100000 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01100000 01...... ........ */
                            /* ./loongarch.decode.2:1115 */
                            info->opc = OPC_VSSRANI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01100000 1....... ........ */
                        /* ./loongarch.decode.2:1116 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRANI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01100001 ........ ........ */
                    /* ./loongarch.decode.2:1117 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRANI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0110001. ........ ........ */
                /* ./loongarch.decode.2:1118 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRANI_D_Q;
                return true;
                break;
            }
            break;
        case 0xd9:
            /* 01110011 011001.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0110010. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01100100 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01100100 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01100100 01...... ........ */
                            /* ./loongarch.decode.2:1123 */
                            info->opc = OPC_VSSRANI_BU_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01100100 1....... ........ */
                        /* ./loongarch.decode.2:1124 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRANI_HU_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01100101 ........ ........ */
                    /* ./loongarch.decode.2:1125 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRANI_WU_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0110011. ........ ........ */
                /* ./loongarch.decode.2:1126 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRANI_DU_Q;
                return true;
                break;
            }
            break;
        case 0xda:
            /* 01110011 011010.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0110100. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01101000 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01101000 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01101000 01...... ........ */
                            /* ./loongarch.decode.2:1145 */
                            info->opc = OPC_VSSRARNI_B_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01101000 1....... ........ */
                        /* ./loongarch.decode.2:1146 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRARNI_H_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01101001 ........ ........ */
                    /* ./loongarch.decode.2:1147 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRARNI_W_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0110101. ........ ........ */
                /* ./loongarch.decode.2:1148 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRARNI_D_Q;
                return true;
                break;
            }
            break;
        case 0xdb:
            /* 01110011 011011.. ........ ........ */
            switch ((insn >> 17) & 0x1) {
            case 0x0:
                /* 01110011 0110110. ........ ........ */
                switch ((insn >> 16) & 0x1) {
                case 0x0:
                    /* 01110011 01101100 ........ ........ */
                    switch ((insn >> 15) & 0x1) {
                    case 0x0:
                        /* 01110011 01101100 0....... ........ */
                        decode_extract_vv_ui4(info, insn);
                        switch ((insn >> 14) & 0x1) {
                        case 0x1:
                            /* 01110011 01101100 01...... ........ */
                            /* ./loongarch.decode.2:1153 */
                            info->opc = OPC_VSSRARNI_BU_H;
                            return true;
                            break;
                        }
                        break;
                    case 0x1:
                        /* 01110011 01101100 1....... ........ */
                        /* ./loongarch.decode.2:1154 */
                        decode_extract_vv_ui5(info, insn);
                        info->opc = OPC_VSSRARNI_HU_W;
                        return true;
                        break;
                    }
                    break;
                case 0x1:
                    /* 01110011 01101101 ........ ........ */
                    /* ./loongarch.decode.2:1155 */
                    decode_extract_vv_ui6(info, insn);
                    info->opc = OPC_VSSRARNI_WU_D;
                    return true;
                    break;
                }
                break;
            case 0x1:
                /* 01110011 0110111. ........ ........ */
                /* ./loongarch.decode.2:1156 */
                decode_extract_vv_ui7(info, insn);
                info->opc = OPC_VSSRARNI_DU_Q;
                return true;
                break;
            }
            break;
        case 0xe0:
            /* 01110011 100000.. ........ ........ */
            /* ./loongarch.decode.2:1431 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VEXTRINS_D;
            return true;
            break;
        case 0xe1:
            /* 01110011 100001.. ........ ........ */
            /* ./loongarch.decode.2:1432 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VEXTRINS_W;
            return true;
            break;
        case 0xe2:
            /* 01110011 100010.. ........ ........ */
            /* ./loongarch.decode.2:1433 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VEXTRINS_H;
            return true;
            break;
        case 0xe3:
            /* 01110011 100011.. ........ ........ */
            /* ./loongarch.decode.2:1434 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VEXTRINS_B;
            return true;
            break;
        case 0xe4:
            /* 01110011 100100.. ........ ........ */
            /* ./loongarch.decode.2:1424 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VSHUF4I_B;
            return true;
            break;
        case 0xe5:
            /* 01110011 100101.. ........ ........ */
            /* ./loongarch.decode.2:1425 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VSHUF4I_H;
            return true;
            break;
        case 0xe6:
            /* 01110011 100110.. ........ ........ */
            /* ./loongarch.decode.2:1426 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VSHUF4I_W;
            return true;
            break;
        case 0xe7:
            /* 01110011 100111.. ........ ........ */
            /* ./loongarch.decode.2:1427 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VSHUF4I_D;
            return true;
            break;
        case 0xf1:
            /* 01110011 110001.. ........ ........ */
            /* ./loongarch.decode.2:1350 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VBITSELI_B;
            return true;
            break;
        case 0xf4:
            /* 01110011 110100.. ........ ........ */
            /* ./loongarch.decode.2:998 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VANDI_B;
            return true;
            break;
        case 0xf5:
            /* 01110011 110101.. ........ ........ */
            /* ./loongarch.decode.2:999 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VORI_B;
            return true;
            break;
        case 0xf6:
            /* 01110011 110110.. ........ ........ */
            /* ./loongarch.decode.2:1000 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VXORI_B;
            return true;
            break;
        case 0xf7:
            /* 01110011 110111.. ........ ........ */
            /* ./loongarch.decode.2:1001 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VNORI_B;
            return true;
            break;
        case 0xf8:
            /* 01110011 111000.. ........ ........ */
            /* ./loongarch.decode.2:988 */
            decode_extract_v_i13(info, insn);
            info->opc = OPC_VLDI;
            return true;
            break;
        case 0xf9:
            /* 01110011 111001.. ........ ........ */
            /* ./loongarch.decode.2:1429 */
            decode_extract_vv_ui8(info, insn);
            info->opc = OPC_VPERMI_W;
            return true;
            break;
        }
        break;
    }
    return false;
}
uint32_t opc2bin[] = 
{
    [OPC_GR2SCR]=2048,
    [OPC_SRC2GR]=3072,
    [OPC_CLO_W]=4096,
    [OPC_CLZ_W]=5120,
    [OPC_CTO_W]=6144,
    [OPC_CTZ_W]=7168,
    [OPC_CLO_D]=8192,
    [OPC_CLZ_D]=9216,
    [OPC_CTO_D]=10240,
    [OPC_CTZ_D]=11264,
    [OPC_REVB_2H]=12288,
    [OPC_REVB_4H]=13312,
    [OPC_REVB_2W]=14336,
    [OPC_REVB_D]=15360,
    [OPC_REVH_2W]=16384,
    [OPC_REVH_D]=17408,
    [OPC_BITREV_4B]=18432,
    [OPC_BITREV_8B]=19456,
    [OPC_BITREV_W]=20480,
    [OPC_BITREV_D]=21504,
    [OPC_EXT_W_H]=22528,
    [OPC_EXT_W_B]=23552,
    [OPC_RDTIMEL_W]=24576,
    [OPC_RDTIMEH_W]=25600,
    [OPC_RDTIME_D]=26624,
    [OPC_CPUCFG]=27648,
    [OPC_X86MTTOP]=28672,
    [OPC_X86MFTOP]=29696,
    [OPC_SETX86LOOPE]=30720,
    [OPC_SETX86LOOPNE]=31744,
    [OPC_X86INC_B]=32768,
    [OPC_X86INC_H]=32769,
    [OPC_X86INC_W]=32770,
    [OPC_X86INC_D]=32771,
    [OPC_X86DEC_B]=32772,
    [OPC_X86DEC_H]=32773,
    [OPC_X86DEC_W]=32774,
    [OPC_X86DEC_D]=32775,
    [OPC_X86SETTM]=32776,
    [OPC_X86CLRTM]=32808,
    [OPC_X86INCTOP]=32777,
    [OPC_X86DECTOP]=32809,
    [OPC_ASRTLE_D]=65536,
    [OPC_ASRTGT_D]=98304,
    [OPC_ALSL_W]=262144,
    [OPC_ALSL_WU]=393216,
    [OPC_BYTEPICK_W]=524288,
    [OPC_BYTEPICK_D]=786432,
    [OPC_ADD_W]=1048576,
    [OPC_ADD_D]=1081344,
    [OPC_SUB_W]=1114112,
    [OPC_SUB_D]=1146880,
    [OPC_SLT]=1179648,
    [OPC_SLTU]=1212416,
    [OPC_MASKEQZ]=1245184,
    [OPC_MASKNEZ]=1277952,
    [OPC_NOR]=1310720,
    [OPC_AND]=1343488,
    [OPC_OR]=1376256,
    [OPC_XOR]=1409024,
    [OPC_ORN]=1441792,
    [OPC_ANDN]=1474560,
    [OPC_SLL_W]=1507328,
    [OPC_SRL_W]=1540096,
    [OPC_SRA_W]=1572864,
    [OPC_SLL_D]=1605632,
    [OPC_SRL_D]=1638400,
    [OPC_SRA_D]=1671168,
    [OPC_ROTR_B]=1703936,
    [OPC_ROTR_H]=1736704,
    [OPC_ROTR_W]=1769472,
    [OPC_ROTR_D]=1802240,
    [OPC_MUL_W]=1835008,
    [OPC_MULH_W]=1867776,
    [OPC_MULH_WU]=1900544,
    [OPC_MUL_D]=1933312,
    [OPC_MULH_D]=1966080,
    [OPC_MULH_DU]=1998848,
    [OPC_MULW_D_W]=2031616,
    [OPC_MULW_D_WU]=2064384,
    [OPC_DIV_W]=2097152,
    [OPC_MOD_W]=2129920,
    [OPC_DIV_WU]=2162688,
    [OPC_MOD_WU]=2195456,
    [OPC_DIV_D]=2228224,
    [OPC_MOD_D]=2260992,
    [OPC_DIV_DU]=2293760,
    [OPC_MOD_DU]=2326528,
    [OPC_CRC_W_B_W]=2359296,
    [OPC_CRC_W_H_W]=2392064,
    [OPC_CRC_W_W_W]=2424832,
    [OPC_CRC_W_D_W]=2457600,
    [OPC_CRCC_W_B_W]=2490368,
    [OPC_CRCC_W_H_W]=2523136,
    [OPC_CRCC_W_W_W]=2555904,
    [OPC_CRCC_W_D_W]=2588672,
    [OPC_ADDU12I_D]=2686976,
    [OPC_ADDU12I_W]=2719744,
    [OPC_BREAK]=2752512,
    [OPC_DBCL]=2785280,
    [OPC_SYSCALL]=2818048,
    [OPC_HVCL]=2850816,
    [OPC_ALSL_D]=2883584,
    [OPC_ADC_B]=3145728,
    [OPC_ADC_H]=3178496,
    [OPC_ADC_W]=3211264,
    [OPC_ADC_D]=3244032,
    [OPC_SBC_B]=3276800,
    [OPC_SBC_H]=3309568,
    [OPC_SBC_W]=3342336,
    [OPC_SBC_D]=3375104,
    [OPC_RCR_B]=3407872,
    [OPC_RCR_H]=3440640,
    [OPC_RCR_W]=3473408,
    [OPC_RCR_D]=3506176,
    [OPC_ARMMOVE]=3555328,
    [OPC_SETX86J]=3571712,
    [OPC_SETARMJ]=3588096,
    [OPC_ARMADD_W]=3604496,
    [OPC_ARMSUB_W]=3637264,
    [OPC_ARMADC_W]=3670032,
    [OPC_ARMSBC_W]=3702800,
    [OPC_ARMAND_W]=3735568,
    [OPC_ARMOR_W]=3768336,
    [OPC_ARMXOR_W]=3801104,
    [OPC_ARMSLL_W]=3833872,
    [OPC_ARMSRL_W]=3866640,
    [OPC_ARMSRA_W]=3899408,
    [OPC_ARMROTR_W]=3932176,
    [OPC_ARMSLLI_W]=3964944,
    [OPC_ARMSRLI_W]=3997712,
    [OPC_ARMSRAI_W]=4030480,
    [OPC_ARMROTRI_W]=4063248,
    [OPC_X86MUL_B]=4096000,
    [OPC_X86MUL_H]=4096001,
    [OPC_X86MUL_W]=4096002,
    [OPC_X86MUL_D]=4096003,
    [OPC_X86MUL_BU]=4096004,
    [OPC_X86MUL_HU]=4096005,
    [OPC_X86MUL_WU]=4096006,
    [OPC_X86MUL_DU]=4096007,
    [OPC_X86ADD_WU]=4128768,
    [OPC_X86ADD_DU]=4128769,
    [OPC_X86SUB_WU]=4128770,
    [OPC_X86SUB_DU]=4128771,
    [OPC_X86ADD_B]=4128772,
    [OPC_X86ADD_H]=4128773,
    [OPC_X86ADD_W]=4128774,
    [OPC_X86ADD_D]=4128775,
    [OPC_X86SUB_B]=4128776,
    [OPC_X86SUB_H]=4128777,
    [OPC_X86SUB_W]=4128778,
    [OPC_X86SUB_D]=4128779,
    [OPC_X86ADC_B]=4128780,
    [OPC_X86ADC_H]=4128781,
    [OPC_X86ADC_W]=4128782,
    [OPC_X86ADC_D]=4128783,
    [OPC_X86SBC_B]=4128784,
    [OPC_X86SBC_H]=4128785,
    [OPC_X86SBC_W]=4128786,
    [OPC_X86SBC_D]=4128787,
    [OPC_X86SLL_B]=4128788,
    [OPC_X86SLL_H]=4128789,
    [OPC_X86SLL_W]=4128790,
    [OPC_X86SLL_D]=4128791,
    [OPC_X86SRL_B]=4128792,
    [OPC_X86SRL_H]=4128793,
    [OPC_X86SRL_W]=4128794,
    [OPC_X86SRL_D]=4128795,
    [OPC_X86SRA_B]=4128796,
    [OPC_X86SRA_H]=4128797,
    [OPC_X86SRA_W]=4128798,
    [OPC_X86SRA_D]=4128799,
    [OPC_X86ROTR_B]=4161536,
    [OPC_X86ROTR_H]=4161537,
    [OPC_X86ROTR_D]=4161538,
    [OPC_X86ROTR_W]=4161539,
    [OPC_X86ROTL_B]=4161540,
    [OPC_X86ROTL_H]=4161541,
    [OPC_X86ROTL_W]=4161542,
    [OPC_X86ROTL_D]=4161543,
    [OPC_X86RCR_B]=4161544,
    [OPC_X86RCR_H]=4161545,
    [OPC_X86RCR_W]=4161546,
    [OPC_X86RCR_D]=4161547,
    [OPC_X86RCL_B]=4161548,
    [OPC_X86RCL_H]=4161549,
    [OPC_X86RCL_W]=4161550,
    [OPC_X86RCL_D]=4161551,
    [OPC_X86AND_B]=4161552,
    [OPC_X86AND_H]=4161553,
    [OPC_X86AND_W]=4161554,
    [OPC_X86AND_D]=4161555,
    [OPC_X86OR_B]=4161556,
    [OPC_X86OR_H]=4161557,
    [OPC_X86OR_W]=4161558,
    [OPC_X86OR_D]=4161559,
    [OPC_X86XOR_B]=4161560,
    [OPC_X86XOR_H]=4161561,
    [OPC_X86XOR_W]=4161562,
    [OPC_X86XOR_D]=4161563,
    [OPC_ARMNOT_W]=4177948,
    [OPC_ARMMOV_W]=4177949,
    [OPC_ARMMOV_D]=4177950,
    [OPC_ARMRRX_W]=4177951,
    [OPC_SLLI_W]=4227072,
    [OPC_SLLI_D]=4259840,
    [OPC_SRLI_W]=4489216,
    [OPC_SRLI_D]=4521984,
    [OPC_SRAI_W]=4751360,
    [OPC_SRAI_D]=4784128,
    [OPC_ROTRI_B]=4988928,
    [OPC_ROTRI_H]=4997120,
    [OPC_ROTRI_W]=5013504,
    [OPC_ROTRI_D]=5046272,
    [OPC_RCRI_B]=5251072,
    [OPC_RCRI_H]=5259264,
    [OPC_RCRI_W]=5275648,
    [OPC_RCRI_D]=5308416,
    [OPC_X86SLLI_B]=5513216,
    [OPC_X86SLLI_H]=5521409,
    [OPC_X86SLLI_W]=5537794,
    [OPC_X86SRLI_B]=5513220,
    [OPC_X86SRLI_H]=5521413,
    [OPC_X86SRLI_W]=5537798,
    [OPC_X86SRAI_B]=5513224,
    [OPC_X86SRAI_H]=5521417,
    [OPC_X86SRAI_W]=5537802,
    [OPC_X86ROTRI_B]=5513228,
    [OPC_X86ROTRI_H]=5521421,
    [OPC_X86ROTRI_W]=5537806,
    [OPC_X86RCRI_B]=5513232,
    [OPC_X86RCRI_H]=5521425,
    [OPC_X86RCRI_W]=5537810,
    [OPC_X86ROTLI_B]=5513236,
    [OPC_X86ROTLI_H]=5521429,
    [OPC_X86ROTLI_W]=5537814,
    [OPC_X86RCLI_B]=5513240,
    [OPC_X86RCLI_H]=5521433,
    [OPC_X86RCLI_W]=5537818,
    [OPC_X86SLLI_D]=5570563,
    [OPC_X86SRLI_D]=5570567,
    [OPC_X86SRAI_D]=5570571,
    [OPC_X86ROTRI_D]=5570575,
    [OPC_X86RCRI_D]=5570579,
    [OPC_X86ROTLI_D]=5570583,
    [OPC_X86RCLI_D]=5570587,
    [OPC_X86SETTAG]=5767168,
    [OPC_X86MFFLAG]=6029312,
    [OPC_X86MTFLAG]=6029344,
    [OPC_ARMMFFLAG]=6029376,
    [OPC_ARMMTFLAG]=6029408,
    [OPC_BSTRINS_W]=6291456,
    [OPC_BSTRPICK_W]=6324224,
    [OPC_BSTRINS_D]=8388608,
    [OPC_BSTRPICK_D]=12582912,
    [OPC_FADD_S]=16809984,
    [OPC_FADD_D]=16842752,
    [OPC_FSUB_S]=16941056,
    [OPC_FSUB_D]=16973824,
    [OPC_FMUL_S]=17072128,
    [OPC_FMUL_D]=17104896,
    [OPC_FDIV_S]=17203200,
    [OPC_FDIV_D]=17235968,
    [OPC_FMAX_S]=17334272,
    [OPC_FMAX_D]=17367040,
    [OPC_FMIN_S]=17465344,
    [OPC_FMIN_D]=17498112,
    [OPC_FMAXA_S]=17596416,
    [OPC_FMAXA_D]=17629184,
    [OPC_FMINA_S]=17727488,
    [OPC_FMINA_D]=17760256,
    [OPC_FSCALEB_S]=17858560,
    [OPC_FSCALEB_D]=17891328,
    [OPC_FCOPYSIGN_S]=17989632,
    [OPC_FCOPYSIGN_D]=18022400,
    [OPC_FABS_S]=18088960,
    [OPC_FABS_D]=18089984,
    [OPC_FNEG_S]=18093056,
    [OPC_FNEG_D]=18094080,
    [OPC_FLOGB_S]=18097152,
    [OPC_FLOGB_D]=18098176,
    [OPC_FCLASS_S]=18101248,
    [OPC_FCLASS_D]=18102272,
    [OPC_FSQRT_S]=18105344,
    [OPC_FSQRT_D]=18106368,
    [OPC_FRECIP_S]=18109440,
    [OPC_FRECIP_D]=18110464,
    [OPC_FRSQRT_S]=18113536,
    [OPC_FRSQRT_D]=18114560,
    [OPC_FMOV_S]=18125824,
    [OPC_FMOV_D]=18126848,
    [OPC_MOVGR2FR_W]=18129920,
    [OPC_MOVGR2FR_D]=18130944,
    [OPC_MOVGR2FRH_W]=18131968,
    [OPC_MOVFR2GR_S]=18134016,
    [OPC_MOVFR2GR_D]=18135040,
    [OPC_MOVFRH2GR_S]=18136064,
    [OPC_MOVGR2FCSR]=18137088,
    [OPC_MOVFCSR2GR]=18139136,
    [OPC_MOVFR2CF]=18141184,
    [OPC_MOVCF2FR]=18142208,
    [OPC_MOVGR2CF]=18143232,
    [OPC_MOVCF2GR]=18144256,
    [OPC_FCVT_LD_D]=18145280,
    [OPC_FCVT_UD_D]=18146304,
    [OPC_FCVT_D_LC]=18153472,
    [OPC_FCVT_S_D]=18421760,
    [OPC_FCVT_D_S]=18424832,
    [OPC_FTINTRM_W_S]=18482176,
    [OPC_FTINTRM_W_D]=18483200,
    [OPC_FTINTRM_L_S]=18490368,
    [OPC_FTINTRM_L_D]=18491392,
    [OPC_FTINTRP_W_S]=18498560,
    [OPC_FTINTRP_W_D]=18499584,
    [OPC_FTINTRP_L_S]=18506752,
    [OPC_FTINTRP_L_D]=18507776,
    [OPC_FTINTRZ_W_S]=18514944,
    [OPC_FTINTRZ_W_D]=18515968,
    [OPC_FTINTRZ_L_S]=18523136,
    [OPC_FTINTRZ_L_D]=18524160,
    [OPC_FTINTRNE_W_S]=18531328,
    [OPC_FTINTRNE_W_D]=18532352,
    [OPC_FTINTRNE_L_S]=18539520,
    [OPC_FTINTRNE_L_D]=18540544,
    [OPC_FTINT_W_S]=18547712,
    [OPC_FTINT_W_D]=18548736,
    [OPC_FTINT_L_S]=18555904,
    [OPC_FTINT_L_D]=18556928,
    [OPC_FFINT_S_W]=18681856,
    [OPC_FFINT_S_L]=18683904,
    [OPC_FFINT_D_W]=18685952,
    [OPC_FFINT_D_L]=18688000,
    [OPC_FRINT_S]=18760704,
    [OPC_FRINT_D]=18761728,
    [OPC_SLTI]=33554432,
    [OPC_SLTUI]=37748736,
    [OPC_ADDI_W]=41943040,
    [OPC_ADDI_D]=46137344,
    [OPC_LU52I_D]=50331648,
    [OPC_ANDI]=54525952,
    [OPC_ORI]=58720256,
    [OPC_XORI]=62914560,
    [OPC_CSRRD]=67108864,
    [OPC_CSRWR]=67108896,
    [OPC_CSRXCHG]=67108864,
    [OPC_GCSRRD]=83886080,
    [OPC_GCSRWR]=83886112,
    [OPC_GCSRXCHG]=83886080,
    [OPC_CACOP]=100663296,
    [OPC_LDDIR]=104857600,
    [OPC_LDPTE]=105119744,
    [OPC_IOCSRRD_B]=105381888,
    [OPC_IOCSRRD_H]=105382912,
    [OPC_IOCSRRD_W]=105383936,
    [OPC_IOCSRRD_D]=105384960,
    [OPC_IOCSRWR_B]=105385984,
    [OPC_IOCSRWR_H]=105387008,
    [OPC_IOCSRWR_W]=105388032,
    [OPC_IOCSRWR_D]=105389056,
    [OPC_TLBCLR]=105390080,
    [OPC_GTLBCLR]=105390081,
    [OPC_TLBFLUSH]=105391104,
    [OPC_GTLBFLUSH]=105391105,
    [OPC_TLBSRCH]=105392128,
    [OPC_GTLBSRCH]=105392129,
    [OPC_TLBRD]=105393152,
    [OPC_GTLBRD]=105393153,
    [OPC_TLBWR]=105394176,
    [OPC_GTLBWR]=105394177,
    [OPC_TLBFILL]=105395200,
    [OPC_GTLBFILL]=105395201,
    [OPC_ERTN]=105396224,
    [OPC_IDLE]=105414656,
    [OPC_SCFGR_W]=105455616,
    [OPC_SCFGR_D]=105456640,
    [OPC_SCFGW_W]=105457664,
    [OPC_SCFGW_D]=105458688,
    [OPC_SNOP]=105459712,
    [OPC_INVTLB]=105480192,
    [OPC_FMADD_S]=135266304,
    [OPC_FMADD_D]=136314880,
    [OPC_FMSUB_S]=139460608,
    [OPC_FMSUB_D]=140509184,
    [OPC_FNMADD_S]=143654912,
    [OPC_FNMADD_D]=144703488,
    [OPC_FNMSUB_S]=147849216,
    [OPC_FNMSUB_D]=148897792,
    [OPC_VFMADD_S]=152043520,
    [OPC_VFMADD_D]=153092096,
    [OPC_VFMSUB_S]=156237824,
    [OPC_VFMSUB_D]=157286400,
    [OPC_VFNMADD_S]=160432128,
    [OPC_VFNMADD_D]=161480704,
    [OPC_VFNMSUB_S]=164626432,
    [OPC_VFNMSUB_D]=165675008,
    [OPC_FCMP_CAF_S]=202375168,
    [OPC_FCMP_SAF_S]=202407936,
    [OPC_FCMP_CLT_S]=202440704,
    [OPC_FCMP_STL_S]=202473472,
    [OPC_FCMP_CEQ_S]=202506240,
    [OPC_FCMP_SEQ_S]=202539008,
    [OPC_FCMP_CLE_S]=202571776,
    [OPC_FCMP_SLE_S]=202604544,
    [OPC_FCMP_CUN_S]=202637312,
    [OPC_FCMP_SUN_S]=202670080,
    [OPC_FCMP_CULT_S]=202702848,
    [OPC_FCMP_SULT_S]=202735616,
    [OPC_FCMP_CUEQ_S]=202768384,
    [OPC_FCMP_SUEQ_S]=202801152,
    [OPC_FCMP_CULE_S]=202833920,
    [OPC_FCMP_SULE_S]=202866688,
    [OPC_FCMP_CNE_S]=202899456,
    [OPC_FCMP_SNE_S]=202932224,
    [OPC_FCMP_COR_S]=203030528,
    [OPC_FCMP_SOR_S]=203063296,
    [OPC_FCMP_CUNE_S]=203161600,
    [OPC_FCMP_SUNE_S]=203194368,
    [OPC_FCMP_CAF_D]=203423744,
    [OPC_FCMP_SAF_D]=203456512,
    [OPC_FCMP_CLT_D]=203489280,
    [OPC_FCMP_STL_D]=203522048,
    [OPC_FCMP_CEQ_D]=203554816,
    [OPC_FCMP_SEQ_D]=203587584,
    [OPC_FCMP_CLE_D]=203620352,
    [OPC_FCMP_SLE_D]=203653120,
    [OPC_FCMP_CUN_D]=203685888,
    [OPC_FCMP_SUN_D]=203718656,
    [OPC_FCMP_CULT_D]=203751424,
    [OPC_FCMP_SULT_D]=203784192,
    [OPC_FCMP_CUEQ_D]=203816960,
    [OPC_FCMP_SUEQ_D]=203849728,
    [OPC_FCMP_CULE_D]=203882496,
    [OPC_FCMP_SULE_D]=203915264,
    [OPC_FCMP_CNE_D]=203948032,
    [OPC_FCMP_SNE_D]=203980800,
    [OPC_FCMP_COR_D]=204079104,
    [OPC_FCMP_SOR_D]=204111872,
    [OPC_FCMP_CUNE_D]=204210176,
    [OPC_FCMP_SUNE_D]=204242944,
    [OPC_VFCMP_COND_S]=206569472,
    [OPC_VFCMP_COND_D]=207618048,
    [OPC_FSEL]=218103808,
    [OPC_VBITSEL_V]=219152384,
    [OPC_VSHUF_B]=223346688,
    [OPC_ADDU16I_D]=268435456,
    [OPC_LU12I_W]=335544320,
    [OPC_LU32I_D]=369098752,
    [OPC_PCADDI]=402653184,
    [OPC_PCALAU12I]=436207616,
    [OPC_PCADDU12I]=469762048,
    [OPC_PCADDU18I]=503316480,
    [OPC_LL_W]=536870912,
    [OPC_SC_W]=553648128,
    [OPC_LL_D]=570425344,
    [OPC_SC_D]=587202560,
    [OPC_LDPTR_W]=603979776,
    [OPC_STPTR_W]=620756992,
    [OPC_LDPTR_D]=637534208,
    [OPC_STPTR_D]=654311424,
    [OPC_LD_B]=671088640,
    [OPC_LD_H]=675282944,
    [OPC_LD_W]=679477248,
    [OPC_LD_D]=683671552,
    [OPC_ST_B]=687865856,
    [OPC_ST_H]=692060160,
    [OPC_ST_W]=696254464,
    [OPC_ST_D]=700448768,
    [OPC_LD_BU]=704643072,
    [OPC_LD_HU]=708837376,
    [OPC_LD_WU]=713031680,
    [OPC_PRELD]=717225984,
    [OPC_FLD_S]=721420288,
    [OPC_FST_S]=725614592,
    [OPC_FLD_D]=729808896,
    [OPC_FST_D]=734003200,
    [OPC_VLD]=738197504,
    [OPC_VST]=742391808,
    [OPC_XVLD]=746586112,
    [OPC_XVST]=750780416,
    [OPC_LDL_W]=771751936,
    [OPC_LDR_W]=775946240,
    [OPC_LDL_D]=780140544,
    [OPC_LDR_D]=784334848,
    [OPC_STL_W]=788529152,
    [OPC_STR_W]=792723456,
    [OPC_STL_D]=796917760,
    [OPC_STR_D]=801112064,
    [OPC_VLDREPL_D]=806354944,
    [OPC_VLDREPL_W]=807403520,
    [OPC_VLDREPL_H]=809500672,
    [OPC_VLDREPL_B]=813694976,
    [OPC_VSTELM_D]=823132160,
    [OPC_VSTELM_W]=824180736,
    [OPC_VSTELM_H]=826277888,
    [OPC_VSTELM_B]=830472192,
    [OPC_LDX_B]=939524096,
    [OPC_LDX_H]=939786240,
    [OPC_LDX_W]=940048384,
    [OPC_LDX_D]=940310528,
    [OPC_STX_B]=940572672,
    [OPC_STX_H]=940834816,
    [OPC_STX_W]=941096960,
    [OPC_STX_D]=941359104,
    [OPC_LDX_BU]=941621248,
    [OPC_LDX_HU]=941883392,
    [OPC_LDX_WU]=942145536,
    [OPC_PRELDX]=942407680,
    [OPC_FLDX_S]=942669824,
    [OPC_FLDX_D]=942931968,
    [OPC_FSTX_S]=943194112,
    [OPC_FSTX_D]=943456256,
    [OPC_VLDX]=943718400,
    [OPC_VSTX]=943980544,
    [OPC_AMSWAP_W]=945815552,
    [OPC_AMSWAP_D]=945848320,
    [OPC_AMADD_W]=945881088,
    [OPC_AMADD_D]=945913856,
    [OPC_AMAND_W]=945946624,
    [OPC_AMAND_D]=945979392,
    [OPC_AMOR_W]=946012160,
    [OPC_AMOR_D]=946044928,
    [OPC_AMXOR_W]=946077696,
    [OPC_AMXOR_D]=946110464,
    [OPC_AMMAX_W]=946143232,
    [OPC_AMMAX_D]=946176000,
    [OPC_AMMIN_W]=946208768,
    [OPC_AMMIN_D]=946241536,
    [OPC_AMMAX_WU]=946274304,
    [OPC_AMMAX_DU]=946307072,
    [OPC_AMMIN_WU]=946339840,
    [OPC_AMMIN_DU]=946372608,
    [OPC_AMSWAP_DB_W]=946405376,
    [OPC_AMSWAP_DB_D]=946438144,
    [OPC_AMADD_DB_W]=946470912,
    [OPC_AMADD_DB_D]=946503680,
    [OPC_AMAND_DB_W]=946536448,
    [OPC_AMAND_DB_D]=946569216,
    [OPC_AMOR_DB_W]=946601984,
    [OPC_AMOR_DB_D]=946634752,
    [OPC_AMXOR_DB_W]=946667520,
    [OPC_AMXOR_DB_D]=946700288,
    [OPC_AMMAX_DB_W]=946733056,
    [OPC_AMMAX_DB_D]=946765824,
    [OPC_AMMIN_DB_W]=946798592,
    [OPC_AMMIN_DB_D]=946831360,
    [OPC_AMMAX_DB_WU]=946864128,
    [OPC_AMMAX_DB_DU]=946896896,
    [OPC_AMMIN_DB_WU]=946929664,
    [OPC_AMMIN_DB_DU]=946962432,
    [OPC_DBAR]=946995200,
    [OPC_IBAR]=947027968,
    [OPC_FLDGT_S]=947126272,
    [OPC_FLDGT_D]=947159040,
    [OPC_FLDLE_S]=947191808,
    [OPC_FLDLE_D]=947224576,
    [OPC_FSTGT_S]=947257344,
    [OPC_FSTGT_D]=947290112,
    [OPC_FSTLE_S]=947322880,
    [OPC_FSTLE_D]=947355648,
    [OPC_LDGT_B]=947388416,
    [OPC_LDGT_H]=947421184,
    [OPC_LDGT_W]=947453952,
    [OPC_LDGT_D]=947486720,
    [OPC_LDLE_B]=947519488,
    [OPC_LDLE_H]=947552256,
    [OPC_LDLE_W]=947585024,
    [OPC_LDLE_D]=947617792,
    [OPC_STGT_B]=947650560,
    [OPC_STGT_H]=947683328,
    [OPC_STGT_W]=947716096,
    [OPC_STGT_D]=947748864,
    [OPC_STLE_B]=947781632,
    [OPC_STLE_H]=947814400,
    [OPC_STLE_W]=947847168,
    [OPC_STLE_D]=947879936,
    [OPC_BEQZ]=1073741824,
    [OPC_BNEZ]=1140850688,
    [OPC_BCEQZ]=1207959552,
    [OPC_BCNEZ]=1207959808,
    [OPC_JISCR0]=1207960064,
    [OPC_JISCR1]=1207960320,
    [OPC_JIRL]=1275068416,
    [OPC_B]=1342177280,
    [OPC_BL]=1409286144,
    [OPC_BEQ]=1476395008,
    [OPC_BNE]=1543503872,
    [OPC_BLT]=1610612736,
    [OPC_BGE]=1677721600,
    [OPC_BLTU]=1744830464,
    [OPC_BGEU]=1811939328,
    [OPC_VSEQ_B]=1879048192,
    [OPC_VSEQ_H]=1879080960,
    [OPC_VSEQ_W]=1879113728,
    [OPC_VSEQ_D]=1879146496,
    [OPC_VSLE_B]=1879179264,
    [OPC_VSLE_H]=1879212032,
    [OPC_VSLE_W]=1879244800,
    [OPC_VSLE_D]=1879277568,
    [OPC_VSLE_BU]=1879310336,
    [OPC_VSLE_HU]=1879343104,
    [OPC_VSLE_WU]=1879375872,
    [OPC_VSLE_DU]=1879408640,
    [OPC_VSLT_B]=1879441408,
    [OPC_VSLT_H]=1879474176,
    [OPC_VSLT_W]=1879506944,
    [OPC_VSLT_D]=1879539712,
    [OPC_VSLT_BU]=1879572480,
    [OPC_VSLT_HU]=1879605248,
    [OPC_VSLT_WU]=1879638016,
    [OPC_VSLT_DU]=1879670784,
    [OPC_VADD_B]=1879703552,
    [OPC_VADD_H]=1879736320,
    [OPC_VADD_W]=1879769088,
    [OPC_VADD_D]=1879801856,
    [OPC_VSUB_B]=1879834624,
    [OPC_VSUB_H]=1879867392,
    [OPC_VSUB_W]=1879900160,
    [OPC_VSUB_D]=1879932928,
    [OPC_VADDWEV_H_B]=1881014272,
    [OPC_VADDWEV_W_H]=1881047040,
    [OPC_VADDWEV_D_W]=1881079808,
    [OPC_VADDWEV_Q_D]=1881112576,
    [OPC_VSUBWEV_H_B]=1881145344,
    [OPC_VSUBWEV_W_H]=1881178112,
    [OPC_VSUBWEV_D_W]=1881210880,
    [OPC_VSUBWEV_Q_D]=1881243648,
    [OPC_VADDWOD_H_B]=1881276416,
    [OPC_VADDWOD_W_H]=1881309184,
    [OPC_VADDWOD_D_W]=1881341952,
    [OPC_VADDWOD_Q_D]=1881374720,
    [OPC_VSUBWOD_H_B]=1881407488,
    [OPC_VSUBWOD_W_H]=1881440256,
    [OPC_VSUBWOD_D_W]=1881473024,
    [OPC_VSUBWOD_Q_D]=1881505792,
    [OPC_VADDWEV_H_BU]=1882062848,
    [OPC_VADDWEV_W_HU]=1882095616,
    [OPC_VADDWEV_D_WU]=1882128384,
    [OPC_VADDWEV_Q_DU]=1882161152,
    [OPC_VSUBWEV_H_BU]=1882193920,
    [OPC_VSUBWEV_W_HU]=1882226688,
    [OPC_VSUBWEV_D_WU]=1882259456,
    [OPC_VSUBWEV_Q_DU]=1882292224,
    [OPC_VADDWOD_H_BU]=1882324992,
    [OPC_VADDWOD_W_HU]=1882357760,
    [OPC_VADDWOD_D_WU]=1882390528,
    [OPC_VADDWOD_Q_DU]=1882423296,
    [OPC_VSUBWOD_H_BU]=1882456064,
    [OPC_VSUBWOD_W_HU]=1882488832,
    [OPC_VSUBWOD_D_WU]=1882521600,
    [OPC_VSUBWOD_Q_DU]=1882554368,
    [OPC_VADDWEV_H_BU_B]=1883111424,
    [OPC_VADDWEV_W_HU_H]=1883144192,
    [OPC_VADDWEV_D_WU_W]=1883176960,
    [OPC_VADDWEV_Q_DU_D]=1883209728,
    [OPC_VADDWOD_H_BU_B]=1883242496,
    [OPC_VADDWOD_W_HU_H]=1883275264,
    [OPC_VADDWOD_D_WU_W]=1883308032,
    [OPC_VADDWOD_Q_DU_D]=1883340800,
    [OPC_VSADD_B]=1883635712,
    [OPC_VSADD_H]=1883668480,
    [OPC_VSADD_W]=1883701248,
    [OPC_VSADD_D]=1883734016,
    [OPC_VSSUB_B]=1883766784,
    [OPC_VSSUB_H]=1883799552,
    [OPC_VSSUB_W]=1883832320,
    [OPC_VSSUB_D]=1883865088,
    [OPC_VSADD_BU]=1883897856,
    [OPC_VSADD_HU]=1883930624,
    [OPC_VSADD_WU]=1883963392,
    [OPC_VSADD_DU]=1883996160,
    [OPC_VSSUB_BU]=1884028928,
    [OPC_VSSUB_HU]=1884061696,
    [OPC_VSSUB_WU]=1884094464,
    [OPC_VSSUB_DU]=1884127232,
    [OPC_VHADDW_H_B]=1884553216,
    [OPC_VHADDW_W_H]=1884585984,
    [OPC_VHADDW_D_W]=1884618752,
    [OPC_VHADDW_Q_D]=1884651520,
    [OPC_VHSUBW_H_B]=1884684288,
    [OPC_VHSUBW_W_H]=1884717056,
    [OPC_VHSUBW_D_W]=1884749824,
    [OPC_VHSUBW_Q_D]=1884782592,
    [OPC_VHADDW_HU_BU]=1884815360,
    [OPC_VHADDW_WU_HU]=1884848128,
    [OPC_VHADDW_DU_WU]=1884880896,
    [OPC_VHADDW_QU_DU]=1884913664,
    [OPC_VHSUBW_HU_BU]=1884946432,
    [OPC_VHSUBW_WU_HU]=1884979200,
    [OPC_VHSUBW_DU_WU]=1885011968,
    [OPC_VHSUBW_QU_DU]=1885044736,
    [OPC_VADDA_B]=1885077504,
    [OPC_VADDA_H]=1885110272,
    [OPC_VADDA_W]=1885143040,
    [OPC_VADDA_D]=1885175808,
    [OPC_VABSD_B]=1885339648,
    [OPC_VABSD_H]=1885372416,
    [OPC_VABSD_W]=1885405184,
    [OPC_VABSD_D]=1885437952,
    [OPC_VABSD_BU]=1885470720,
    [OPC_VABSD_HU]=1885503488,
    [OPC_VABSD_WU]=1885536256,
    [OPC_VABSD_DU]=1885569024,
    [OPC_VAVG_B]=1885601792,
    [OPC_VAVG_H]=1885634560,
    [OPC_VAVG_W]=1885667328,
    [OPC_VAVG_D]=1885700096,
    [OPC_VAVG_BU]=1885732864,
    [OPC_VAVG_HU]=1885765632,
    [OPC_VAVG_WU]=1885798400,
    [OPC_VAVG_DU]=1885831168,
    [OPC_VAVGR_B]=1885863936,
    [OPC_VAVGR_H]=1885896704,
    [OPC_VAVGR_W]=1885929472,
    [OPC_VAVGR_D]=1885962240,
    [OPC_VAVGR_BU]=1885995008,
    [OPC_VAVGR_HU]=1886027776,
    [OPC_VAVGR_WU]=1886060544,
    [OPC_VAVGR_DU]=1886093312,
    [OPC_VMAX_B]=1886388224,
    [OPC_VMAX_H]=1886420992,
    [OPC_VMAX_W]=1886453760,
    [OPC_VMAX_D]=1886486528,
    [OPC_VMIN_B]=1886519296,
    [OPC_VMIN_H]=1886552064,
    [OPC_VMIN_W]=1886584832,
    [OPC_VMIN_D]=1886617600,
    [OPC_VMAX_BU]=1886650368,
    [OPC_VMAX_HU]=1886683136,
    [OPC_VMAX_WU]=1886715904,
    [OPC_VMAX_DU]=1886748672,
    [OPC_VMIN_BU]=1886781440,
    [OPC_VMIN_HU]=1886814208,
    [OPC_VMIN_WU]=1886846976,
    [OPC_VMIN_DU]=1886879744,
    [OPC_VMUL_B]=1887698944,
    [OPC_VMUL_H]=1887731712,
    [OPC_VMUL_W]=1887764480,
    [OPC_VMUL_D]=1887797248,
    [OPC_VMUH_B]=1887830016,
    [OPC_VMUH_H]=1887862784,
    [OPC_VMUH_W]=1887895552,
    [OPC_VMUH_D]=1887928320,
    [OPC_VMUH_BU]=1887961088,
    [OPC_VMUH_HU]=1887993856,
    [OPC_VMUH_WU]=1888026624,
    [OPC_VMUH_DU]=1888059392,
    [OPC_VMULWEV_H_B]=1888485376,
    [OPC_VMULWEV_W_H]=1888518144,
    [OPC_VMULWEV_D_W]=1888550912,
    [OPC_VMULWEV_Q_D]=1888583680,
    [OPC_VMULWOD_H_B]=1888616448,
    [OPC_VMULWOD_W_H]=1888649216,
    [OPC_VMULWOD_D_W]=1888681984,
    [OPC_VMULWOD_Q_D]=1888714752,
    [OPC_VMULWEV_H_BU]=1889009664,
    [OPC_VMULWEV_W_HU]=1889042432,
    [OPC_VMULWEV_D_WU]=1889075200,
    [OPC_VMULWEV_Q_DU]=1889107968,
    [OPC_VMULWOD_H_BU]=1889140736,
    [OPC_VMULWOD_W_HU]=1889173504,
    [OPC_VMULWOD_D_WU]=1889206272,
    [OPC_VMULWOD_Q_DU]=1889239040,
    [OPC_VMULWEV_H_BU_B]=1889533952,
    [OPC_VMULWEV_W_HU_H]=1889566720,
    [OPC_VMULWEV_D_WU_W]=1889599488,
    [OPC_VMULWEV_Q_DU_D]=1889632256,
    [OPC_VMULWOD_H_BU_B]=1889665024,
    [OPC_VMULWOD_W_HU_H]=1889697792,
    [OPC_VMULWOD_D_WU_W]=1889730560,
    [OPC_VMULWOD_Q_DU_D]=1889763328,
    [OPC_VMADD_B]=1890058240,
    [OPC_VMADD_H]=1890091008,
    [OPC_VMADD_W]=1890123776,
    [OPC_VMADD_D]=1890156544,
    [OPC_VMSUB_B]=1890189312,
    [OPC_VMSUB_H]=1890222080,
    [OPC_VMSUB_W]=1890254848,
    [OPC_VMSUB_D]=1890287616,
    [OPC_VMADDWEV_H_B]=1890320384,
    [OPC_VMADDWEV_W_H]=1890353152,
    [OPC_VMADDWEV_D_W]=1890385920,
    [OPC_VMADDWEV_Q_D]=1890418688,
    [OPC_VMADDWOD_H_B]=1890451456,
    [OPC_VMADDWOD_W_H]=1890484224,
    [OPC_VMADDWOD_D_W]=1890516992,
    [OPC_VMADDWOD_Q_D]=1890549760,
    [OPC_VMADDWEV_H_BU]=1890844672,
    [OPC_VMADDWEV_W_HU]=1890877440,
    [OPC_VMADDWEV_D_WU]=1890910208,
    [OPC_VMADDWEV_Q_DU]=1890942976,
    [OPC_VMADDWOD_H_BU]=1890975744,
    [OPC_VMADDWOD_W_HU]=1891008512,
    [OPC_VMADDWOD_D_WU]=1891041280,
    [OPC_VMADDWOD_Q_DU]=1891074048,
    [OPC_VMADDWEV_H_BU_B]=1891368960,
    [OPC_VMADDWEV_W_HU_H]=1891401728,
    [OPC_VMADDWEV_D_WU_W]=1891434496,
    [OPC_VMADDWEV_Q_DU_D]=1891467264,
    [OPC_VMADDWOD_H_BU_B]=1891500032,
    [OPC_VMADDWOD_W_HU_H]=1891532800,
    [OPC_VMADDWOD_D_WU_W]=1891565568,
    [OPC_VMADDWOD_Q_DU_D]=1891598336,
    [OPC_VDIV_B]=1893728256,
    [OPC_VDIV_H]=1893761024,
    [OPC_VDIV_W]=1893793792,
    [OPC_VDIV_D]=1893826560,
    [OPC_VMOD_B]=1893859328,
    [OPC_VMOD_H]=1893892096,
    [OPC_VMOD_W]=1893924864,
    [OPC_VMOD_D]=1893957632,
    [OPC_VDIV_BU]=1893990400,
    [OPC_VDIV_HU]=1894023168,
    [OPC_VDIV_WU]=1894055936,
    [OPC_VDIV_DU]=1894088704,
    [OPC_VMOD_BU]=1894121472,
    [OPC_VMOD_HU]=1894154240,
    [OPC_VMOD_WU]=1894187008,
    [OPC_VMOD_DU]=1894219776,
    [OPC_VSLL_B]=1894252544,
    [OPC_VSLL_H]=1894285312,
    [OPC_VSLL_W]=1894318080,
    [OPC_VSLL_D]=1894350848,
    [OPC_VSRL_B]=1894383616,
    [OPC_VSRL_H]=1894416384,
    [OPC_VSRL_W]=1894449152,
    [OPC_VSRL_D]=1894481920,
    [OPC_VSRA_B]=1894514688,
    [OPC_VSRA_H]=1894547456,
    [OPC_VSRA_W]=1894580224,
    [OPC_VSRA_D]=1894612992,
    [OPC_VROTR_B]=1894645760,
    [OPC_VROTR_H]=1894678528,
    [OPC_VROTR_W]=1894711296,
    [OPC_VROTR_D]=1894744064,
    [OPC_VSRLR_B]=1894776832,
    [OPC_VSRLR_H]=1894809600,
    [OPC_VSRLR_W]=1894842368,
    [OPC_VSRLR_D]=1894875136,
    [OPC_VSRAR_B]=1894907904,
    [OPC_VSRAR_H]=1894940672,
    [OPC_VSRAR_W]=1894973440,
    [OPC_VSRAR_D]=1895006208,
    [OPC_VSRLN_B_H]=1895071744,
    [OPC_VSRLN_H_W]=1895104512,
    [OPC_VSRLN_W_D]=1895137280,
    [OPC_VSRAN_B_H]=1895202816,
    [OPC_VSRAN_H_W]=1895235584,
    [OPC_VSRAN_W_D]=1895268352,
    [OPC_VSRLRN_B_H]=1895333888,
    [OPC_VSRLRN_H_W]=1895366656,
    [OPC_VSRLRN_W_D]=1895399424,
    [OPC_VSRARN_B_H]=1895464960,
    [OPC_VSRARN_H_W]=1895497728,
    [OPC_VSRARN_W_D]=1895530496,
    [OPC_VSSRLN_B_H]=1895596032,
    [OPC_VSSRLN_H_W]=1895628800,
    [OPC_VSSRLN_W_D]=1895661568,
    [OPC_VSSRAN_B_H]=1895727104,
    [OPC_VSSRAN_H_W]=1895759872,
    [OPC_VSSRAN_W_D]=1895792640,
    [OPC_VSSRLRN_B_H]=1895858176,
    [OPC_VSSRLRN_H_W]=1895890944,
    [OPC_VSSRLRN_W_D]=1895923712,
    [OPC_VSSRARN_B_H]=1895989248,
    [OPC_VSSRARN_H_W]=1896022016,
    [OPC_VSSRARN_W_D]=1896054784,
    [OPC_VSSRLN_BU_H]=1896120320,
    [OPC_VSSRLN_HU_W]=1896153088,
    [OPC_VSSRLN_WU_D]=1896185856,
    [OPC_VSSRAN_BU_H]=1896251392,
    [OPC_VSSRAN_HU_W]=1896284160,
    [OPC_VSSRAN_WU_D]=1896316928,
    [OPC_VSSRLRN_BU_H]=1896382464,
    [OPC_VSSRLRN_HU_W]=1896415232,
    [OPC_VSSRLRN_WU_D]=1896448000,
    [OPC_VSSRARN_BU_H]=1896513536,
    [OPC_VSSRARN_HU_W]=1896546304,
    [OPC_VSSRARN_WU_D]=1896579072,
    [OPC_VBITCLR_B]=1896611840,
    [OPC_VBITCLR_H]=1896644608,
    [OPC_VBITCLR_W]=1896677376,
    [OPC_VBITCLR_D]=1896710144,
    [OPC_VBITSET_B]=1896742912,
    [OPC_VBITSET_H]=1896775680,
    [OPC_VBITSET_W]=1896808448,
    [OPC_VBITSET_D]=1896841216,
    [OPC_VBITREV_B]=1896873984,
    [OPC_VBITREV_H]=1896906752,
    [OPC_VBITREV_W]=1896939520,
    [OPC_VBITREV_D]=1896972288,
    [OPC_VPACKEV_B]=1897267200,
    [OPC_VPACKEV_H]=1897299968,
    [OPC_VPACKEV_W]=1897332736,
    [OPC_VPACKEV_D]=1897365504,
    [OPC_VPACKOD_B]=1897398272,
    [OPC_VPACKOD_H]=1897431040,
    [OPC_VPACKOD_W]=1897463808,
    [OPC_VPACKOD_D]=1897496576,
    [OPC_VILVL_B]=1897529344,
    [OPC_VILVL_H]=1897562112,
    [OPC_VILVL_W]=1897594880,
    [OPC_VILVL_D]=1897627648,
    [OPC_VILVH_B]=1897660416,
    [OPC_VILVH_H]=1897693184,
    [OPC_VILVH_W]=1897725952,
    [OPC_VILVH_D]=1897758720,
    [OPC_VPICKEV_B]=1897791488,
    [OPC_VPICKEV_H]=1897824256,
    [OPC_VPICKEV_W]=1897857024,
    [OPC_VPICKEV_D]=1897889792,
    [OPC_VPICKOD_B]=1897922560,
    [OPC_VPICKOD_H]=1897955328,
    [OPC_VPICKOD_W]=1897988096,
    [OPC_VPICKOD_D]=1898020864,
    [OPC_VREPLVE_B]=1898053632,
    [OPC_VREPLVE_H]=1898086400,
    [OPC_VREPLVE_W]=1898119168,
    [OPC_VREPLVE_D]=1898151936,
    [OPC_VAND_V]=1898315776,
    [OPC_VOR_V]=1898348544,
    [OPC_VXOR_V]=1898381312,
    [OPC_VNOR_V]=1898414080,
    [OPC_VANDN_V]=1898446848,
    [OPC_VORN_V]=1898479616,
    [OPC_VFRSTP_B]=1898643456,
    [OPC_VFRSTP_H]=1898676224,
    [OPC_VADD_Q]=1898774528,
    [OPC_VSUB_Q]=1898807296,
    [OPC_VSIGNCOV_B]=1898840064,
    [OPC_VSIGNCOV_H]=1898872832,
    [OPC_VSIGNCOV_W]=1898905600,
    [OPC_VSIGNCOV_D]=1898938368,
    [OPC_VFADD_S]=1899003904,
    [OPC_VFADD_D]=1899036672,
    [OPC_VFSUB_S]=1899134976,
    [OPC_VFSUB_D]=1899167744,
    [OPC_VFMUL_S]=1899528192,
    [OPC_VFMUL_D]=1899560960,
    [OPC_VFDIV_S]=1899659264,
    [OPC_VFDIV_D]=1899692032,
    [OPC_VFMAX_S]=1899790336,
    [OPC_VFMAX_D]=1899823104,
    [OPC_VFMIN_S]=1899921408,
    [OPC_VFMIN_D]=1899954176,
    [OPC_VFMAXA_S]=1900052480,
    [OPC_VFMAXA_D]=1900085248,
    [OPC_VFMINA_S]=1900183552,
    [OPC_VFMINA_D]=1900216320,
    [OPC_VFCVT_H_S]=1900412928,
    [OPC_VFCVT_S_D]=1900445696,
    [OPC_VFFINT_S_L]=1900544000,
    [OPC_VFTINT_W_D]=1900642304,
    [OPC_VFTINTRM_W_D]=1900675072,
    [OPC_VFTINTRP_W_D]=1900707840,
    [OPC_VFTINTRZ_W_D]=1900740608,
    [OPC_VFTINTRNE_W_D]=1900773376,
    [OPC_VSHUF_H]=1903853568,
    [OPC_VSHUF_W]=1903886336,
    [OPC_VSHUF_D]=1903919104,
    [OPC_VSEQI_B]=1920991232,
    [OPC_VSEQI_H]=1921024000,
    [OPC_VSEQI_W]=1921056768,
    [OPC_VSEQI_D]=1921089536,
    [OPC_VSLEI_B]=1921122304,
    [OPC_VSLEI_H]=1921155072,
    [OPC_VSLEI_W]=1921187840,
    [OPC_VSLEI_D]=1921220608,
    [OPC_VSLEI_BU]=1921253376,
    [OPC_VSLEI_HU]=1921286144,
    [OPC_VSLEI_WU]=1921318912,
    [OPC_VSLEI_DU]=1921351680,
    [OPC_VSLTI_B]=1921384448,
    [OPC_VSLTI_H]=1921417216,
    [OPC_VSLTI_W]=1921449984,
    [OPC_VSLTI_D]=1921482752,
    [OPC_VSLTI_BU]=1921515520,
    [OPC_VSLTI_HU]=1921548288,
    [OPC_VSLTI_WU]=1921581056,
    [OPC_VSLTI_DU]=1921613824,
    [OPC_VADDI_BU]=1921646592,
    [OPC_VADDI_HU]=1921679360,
    [OPC_VADDI_WU]=1921712128,
    [OPC_VADDI_DU]=1921744896,
    [OPC_VSUBI_BU]=1921777664,
    [OPC_VSUBI_HU]=1921810432,
    [OPC_VSUBI_WU]=1921843200,
    [OPC_VSUBI_DU]=1921875968,
    [OPC_VBSLL_V]=1921908736,
    [OPC_VBSRL_V]=1921941504,
    [OPC_VMAXI_B]=1922039808,
    [OPC_VMAXI_H]=1922072576,
    [OPC_VMAXI_W]=1922105344,
    [OPC_VMAXI_D]=1922138112,
    [OPC_VMINI_B]=1922170880,
    [OPC_VMINI_H]=1922203648,
    [OPC_VMINI_W]=1922236416,
    [OPC_VMINI_D]=1922269184,
    [OPC_VMAXI_BU]=1922301952,
    [OPC_VMAXI_HU]=1922334720,
    [OPC_VMAXI_WU]=1922367488,
    [OPC_VMAXI_DU]=1922400256,
    [OPC_VMINI_BU]=1922433024,
    [OPC_VMINI_HU]=1922465792,
    [OPC_VMINI_WU]=1922498560,
    [OPC_VMINI_DU]=1922531328,
    [OPC_VFRSTPI_B]=1922695168,
    [OPC_VFRSTPI_H]=1922727936,
    [OPC_VCLO_B]=1922826240,
    [OPC_VCLO_H]=1922827264,
    [OPC_VCLO_W]=1922828288,
    [OPC_VCLO_D]=1922829312,
    [OPC_VCLZ_B]=1922830336,
    [OPC_VCLZ_H]=1922831360,
    [OPC_VCLZ_W]=1922832384,
    [OPC_VCLZ_D]=1922833408,
    [OPC_VPCNT_B]=1922834432,
    [OPC_VPCNT_H]=1922835456,
    [OPC_VPCNT_W]=1922836480,
    [OPC_VPCNT_D]=1922837504,
    [OPC_VNEG_B]=1922838528,
    [OPC_VNEG_H]=1922839552,
    [OPC_VNEG_W]=1922840576,
    [OPC_VNEG_D]=1922841600,
    [OPC_VMSKLTZ_B]=1922842624,
    [OPC_VMSKLTZ_H]=1922843648,
    [OPC_VMSKLTZ_W]=1922844672,
    [OPC_VMSKLTZ_D]=1922845696,
    [OPC_VMSKGEZ_B]=1922846720,
    [OPC_VMSKNZ_B]=1922850816,
    [OPC_VSETEQZ_V]=1922865152,
    [OPC_VSETNEZ_V]=1922866176,
    [OPC_VSETANYEQZ_B]=1922867200,
    [OPC_VSETANYEQZ_H]=1922868224,
    [OPC_VSETANYEQZ_W]=1922869248,
    [OPC_VSETANYEQZ_D]=1922870272,
    [OPC_VSETALLNEZ_B]=1922871296,
    [OPC_VSETALLNEZ_H]=1922872320,
    [OPC_VSETALLNEZ_W]=1922873344,
    [OPC_VSETALLNEZ_D]=1922874368,
    [OPC_VFLOGB_S]=1922876416,
    [OPC_VFLOGB_D]=1922877440,
    [OPC_VFCLASS_S]=1922880512,
    [OPC_VFCLASS_D]=1922881536,
    [OPC_VFSQRT_S]=1922884608,
    [OPC_VFSQRT_D]=1922885632,
    [OPC_VFRECIP_S]=1922888704,
    [OPC_VFRECIP_D]=1922889728,
    [OPC_VFRSQRT_S]=1922892800,
    [OPC_VFRSQRT_D]=1922893824,
    [OPC_VFRINT_S]=1922905088,
    [OPC_VFRINT_D]=1922906112,
    [OPC_VFRINTRM_S]=1922909184,
    [OPC_VFRINTRM_D]=1922910208,
    [OPC_VFRINTRP_S]=1922913280,
    [OPC_VFRINTRP_D]=1922914304,
    [OPC_VFRINTRZ_S]=1922917376,
    [OPC_VFRINTRZ_D]=1922918400,
    [OPC_VFRINTRNE_S]=1922921472,
    [OPC_VFRINTRNE_D]=1922922496,
    [OPC_VFCVTL_S_H]=1922951168,
    [OPC_VFCVTH_S_H]=1922952192,
    [OPC_VFCVTL_D_S]=1922953216,
    [OPC_VFCVTH_D_S]=1922954240,
    [OPC_VFFINT_S_W]=1922957312,
    [OPC_VFFINT_S_WU]=1922958336,
    [OPC_VFFINT_D_L]=1922959360,
    [OPC_VFFINT_D_LU]=1922960384,
    [OPC_VFFINTL_D_W]=1922961408,
    [OPC_VFFINTH_D_W]=1922962432,
    [OPC_VFTINT_W_S]=1922969600,
    [OPC_VFTINT_L_D]=1922970624,
    [OPC_VFTINTRM_W_S]=1922971648,
    [OPC_VFTINTRM_L_D]=1922972672,
    [OPC_VFTINTRP_W_S]=1922973696,
    [OPC_VFTINTRP_L_D]=1922974720,
    [OPC_VFTINTRZ_W_S]=1922975744,
    [OPC_VFTINTRZ_L_D]=1922976768,
    [OPC_VFTINTRNE_W_S]=1922977792,
    [OPC_VFTINTRNE_L_D]=1922978816,
    [OPC_VFTINT_WU_S]=1922979840,
    [OPC_VFTINT_LU_D]=1922980864,
    [OPC_VFTINTRZ_WU_S]=1922985984,
    [OPC_VFTINTRZ_LU_D]=1922987008,
    [OPC_VFTINTL_L_S]=1922990080,
    [OPC_VFTINTH_L_S]=1922991104,
    [OPC_VFTINTRML_L_S]=1922992128,
    [OPC_VFTINTRMH_L_S]=1922993152,
    [OPC_VFTINTRPL_L_S]=1922994176,
    [OPC_VFTINTRPH_L_S]=1922995200,
    [OPC_VFTINTRZL_L_S]=1922996224,
    [OPC_VFTINTRZH_L_S]=1922997248,
    [OPC_VFTINTRNEL_L_S]=1922998272,
    [OPC_VFTINTRNEH_L_S]=1922999296,
    [OPC_VEXTH_H_B]=1923014656,
    [OPC_VEXTH_W_H]=1923015680,
    [OPC_VEXTH_D_W]=1923016704,
    [OPC_VEXTH_Q_D]=1923017728,
    [OPC_VEXTH_HU_BU]=1923018752,
    [OPC_VEXTH_WU_HU]=1923019776,
    [OPC_VEXTH_DU_WU]=1923020800,
    [OPC_VEXTH_QU_DU]=1923021824,
    [OPC_VREPLGR2VR_B]=1923022848,
    [OPC_VREPLGR2VR_H]=1923023872,
    [OPC_VREPLGR2VR_W]=1923024896,
    [OPC_VREPLGR2VR_D]=1923025920,
    [OPC_VROTRI_B]=1923096576,
    [OPC_VROTRI_H]=1923104768,
    [OPC_VROTRI_W]=1923121152,
    [OPC_VROTRI_D]=1923153920,
    [OPC_VSRLRI_B]=1923358720,
    [OPC_VSRLRI_H]=1923366912,
    [OPC_VSRLRI_W]=1923383296,
    [OPC_VSRLRI_D]=1923416064,
    [OPC_VSRARI_B]=1923620864,
    [OPC_VSRARI_H]=1923629056,
    [OPC_VSRARI_W]=1923645440,
    [OPC_VSRARI_D]=1923678208,
    [OPC_VINSGR2VR_B]=1928036352,
    [OPC_VINSGR2VR_H]=1928052736,
    [OPC_VINSGR2VR_W]=1928060928,
    [OPC_VINSGR2VR_D]=1928065024,
    [OPC_VPICKVE2GR_B]=1928298496,
    [OPC_VPICKVE2GR_H]=1928314880,
    [OPC_VPICKVE2GR_W]=1928323072,
    [OPC_VPICKVE2GR_D]=1928327168,
    [OPC_VPICKVE2GR_BU]=1928560640,
    [OPC_VPICKVE2GR_HU]=1928577024,
    [OPC_VPICKVE2GR_WU]=1928585216,
    [OPC_VPICKVE2GR_DU]=1928589312,
    [OPC_VREPLVEI_B]=1928822784,
    [OPC_VREPLVEI_H]=1928839168,
    [OPC_VREPLVEI_W]=1928847360,
    [OPC_VREPLVEI_D]=1928851456,
    [OPC_VSLLWIL_H_B]=1929912320,
    [OPC_VSLLWIL_W_H]=1929920512,
    [OPC_VSLLWIL_D_W]=1929936896,
    [OPC_VEXTL_Q_D]=1929969664,
    [OPC_VSLLWIL_HU_BU]=1930174464,
    [OPC_VSLLWIL_WU_HU]=1930182656,
    [OPC_VSLLWIL_DU_WU]=1930199040,
    [OPC_VEXTL_QU_DU]=1930231808,
    [OPC_VBITCLRI_B]=1930436608,
    [OPC_VBITCLRI_H]=1930444800,
    [OPC_VBITCLRI_W]=1930461184,
    [OPC_VBITCLRI_D]=1930493952,
    [OPC_VBITSETI_B]=1930698752,
    [OPC_VBITSETI_H]=1930706944,
    [OPC_VBITSETI_W]=1930723328,
    [OPC_VBITSETI_D]=1930756096,
    [OPC_VBITREVI_B]=1930960896,
    [OPC_VBITREVI_H]=1930969088,
    [OPC_VBITREVI_W]=1930985472,
    [OPC_VBITREVI_D]=1931018240,
    [OPC_VSAT_B]=1931747328,
    [OPC_VSAT_H]=1931755520,
    [OPC_VSAT_W]=1931771904,
    [OPC_VSAT_D]=1931804672,
    [OPC_VSAT_BU]=1932009472,
    [OPC_VSAT_HU]=1932017664,
    [OPC_VSAT_WU]=1932034048,
    [OPC_VSAT_DU]=1932066816,
    [OPC_VSLLI_B]=1932271616,
    [OPC_VSLLI_H]=1932279808,
    [OPC_VSLLI_W]=1932296192,
    [OPC_VSLLI_D]=1932328960,
    [OPC_VSRLI_B]=1932533760,
    [OPC_VSRLI_H]=1932541952,
    [OPC_VSRLI_W]=1932558336,
    [OPC_VSRLI_D]=1932591104,
    [OPC_VSRAI_B]=1932795904,
    [OPC_VSRAI_H]=1932804096,
    [OPC_VSRAI_W]=1932820480,
    [OPC_VSRAI_D]=1932853248,
    [OPC_VSRLNI_B_H]=1933590528,
    [OPC_VSRLNI_H_W]=1933606912,
    [OPC_VSRLNI_W_D]=1933639680,
    [OPC_VSRLNI_D_Q]=1933705216,
    [OPC_VSRLRNI_B_H]=1933852672,
    [OPC_VSRLRNI_H_W]=1933869056,
    [OPC_VSRLRNI_W_D]=1933901824,
    [OPC_VSRLRNI_D_Q]=1933967360,
    [OPC_VSSRLNI_B_H]=1934114816,
    [OPC_VSSRLNI_H_W]=1934131200,
    [OPC_VSSRLNI_W_D]=1934163968,
    [OPC_VSSRLNI_D_Q]=1934229504,
    [OPC_VSSRLNI_BU_H]=1934376960,
    [OPC_VSSRLNI_HU_W]=1934393344,
    [OPC_VSSRLNI_WU_D]=1934426112,
    [OPC_VSSRLNI_DU_Q]=1934491648,
    [OPC_VSSRLRNI_B_H]=1934639104,
    [OPC_VSSRLRNI_H_W]=1934655488,
    [OPC_VSSRLRNI_W_D]=1934688256,
    [OPC_VSSRLRNI_D_Q]=1934753792,
    [OPC_VSSRLRNI_BU_H]=1934901248,
    [OPC_VSSRLRNI_HU_W]=1934917632,
    [OPC_VSSRLRNI_WU_D]=1934950400,
    [OPC_VSSRLRNI_DU_Q]=1935015936,
    [OPC_VSRANI_B_H]=1935163392,
    [OPC_VSRANI_H_W]=1935179776,
    [OPC_VSRANI_W_D]=1935212544,
    [OPC_VSRANI_D_Q]=1935278080,
    [OPC_VSRARNI_B_H]=1935425536,
    [OPC_VSRARNI_H_W]=1935441920,
    [OPC_VSRARNI_W_D]=1935474688,
    [OPC_VSRARNI_D_Q]=1935540224,
    [OPC_VSSRANI_B_H]=1935687680,
    [OPC_VSSRANI_H_W]=1935704064,
    [OPC_VSSRANI_W_D]=1935736832,
    [OPC_VSSRANI_D_Q]=1935802368,
    [OPC_VSSRANI_BU_H]=1935949824,
    [OPC_VSSRANI_HU_W]=1935966208,
    [OPC_VSSRANI_WU_D]=1935998976,
    [OPC_VSSRANI_DU_Q]=1936064512,
    [OPC_VSSRARNI_B_H]=1936211968,
    [OPC_VSSRARNI_H_W]=1936228352,
    [OPC_VSSRARNI_W_D]=1936261120,
    [OPC_VSSRARNI_D_Q]=1936326656,
    [OPC_VSSRARNI_BU_H]=1936474112,
    [OPC_VSSRARNI_HU_W]=1936490496,
    [OPC_VSSRARNI_WU_D]=1936523264,
    [OPC_VSSRARNI_DU_Q]=1936588800,
    [OPC_VEXTRINS_D]=1937768448,
    [OPC_VEXTRINS_W]=1938030592,
    [OPC_VEXTRINS_H]=1938292736,
    [OPC_VEXTRINS_B]=1938554880,
    [OPC_VSHUF4I_B]=1938817024,
    [OPC_VSHUF4I_H]=1939079168,
    [OPC_VSHUF4I_W]=1939341312,
    [OPC_VSHUF4I_D]=1939603456,
    [OPC_VBITSELI_B]=1942224896,
    [OPC_VANDI_B]=1943011328,
    [OPC_VORI_B]=1943273472,
    [OPC_VXORI_B]=1943535616,
    [OPC_VNORI_B]=1943797760,
    [OPC_VLDI]=1944059904,
    [OPC_VPERMI_W]=1944322048,
};
int insn2format[] = {
    [OPC_CLO_W] =format_0,
    [OPC_CLZ_W] =format_0,
    [OPC_CTO_W] =format_0,
    [OPC_CTZ_W] =format_0,
    [OPC_CLO_D] =format_0,
    [OPC_CLZ_D] =format_0,
    [OPC_CTO_D] =format_0,
    [OPC_CTZ_D] =format_0,
    [OPC_REVB_2H] =format_0,
    [OPC_REVB_4H] =format_0,
    [OPC_REVB_2W] =format_0,
    [OPC_REVB_D] =format_0,
    [OPC_REVH_2W] =format_0,
    [OPC_REVH_D] =format_0,
    [OPC_BITREV_4B] =format_0,
    [OPC_BITREV_8B] =format_0,
    [OPC_BITREV_W] =format_0,
    [OPC_BITREV_D] =format_0,
    [OPC_EXT_W_H] =format_0,
    [OPC_EXT_W_B] =format_0,
    [OPC_RDTIMEL_W] =format_0,
    [OPC_RDTIMEH_W] =format_0,
    [OPC_RDTIME_D] =format_0,
    [OPC_CPUCFG] =format_0,
    [OPC_ASRTLE_D] =format_1,
    [OPC_ASRTGT_D] =format_1,
    [OPC_ALSL_W] =format_2,
    [OPC_ALSL_WU] =format_2,
    [OPC_BYTEPICK_W] =format_2,
    [OPC_BYTEPICK_D] =format_3,
    [OPC_ADD_W] =format_4,
    [OPC_ADD_D] =format_4,
    [OPC_SUB_W] =format_4,
    [OPC_SUB_D] =format_4,
    [OPC_SLT] =format_4,
    [OPC_SLTU] =format_4,
    [OPC_MASKEQZ] =format_4,
    [OPC_MASKNEZ] =format_4,
    [OPC_NOR] =format_4,
    [OPC_AND] =format_4,
    [OPC_OR] =format_4,
    [OPC_XOR] =format_4,
    [OPC_ORN] =format_4,
    [OPC_ANDN] =format_4,
    [OPC_SLL_W] =format_4,
    [OPC_SRL_W] =format_4,
    [OPC_SRA_W] =format_4,
    [OPC_SLL_D] =format_4,
    [OPC_SRL_D] =format_4,
    [OPC_SRA_D] =format_4,
    [OPC_ROTR_W] =format_4,
    [OPC_ROTR_D] =format_4,
    [OPC_MUL_W] =format_4,
    [OPC_MULH_W] =format_4,
    [OPC_MULH_WU] =format_4,
    [OPC_MUL_D] =format_4,
    [OPC_MULH_D] =format_4,
    [OPC_MULH_DU] =format_4,
    [OPC_MULW_D_W] =format_4,
    [OPC_MULW_D_WU] =format_4,
    [OPC_DIV_W] =format_4,
    [OPC_MOD_W] =format_4,
    [OPC_DIV_WU] =format_4,
    [OPC_MOD_WU] =format_4,
    [OPC_DIV_D] =format_4,
    [OPC_MOD_D] =format_4,
    [OPC_DIV_DU] =format_4,
    [OPC_MOD_DU] =format_4,
    [OPC_CRC_W_B_W] =format_4,
    [OPC_CRC_W_H_W] =format_4,
    [OPC_CRC_W_W_W] =format_4,
    [OPC_CRC_W_D_W] =format_4,
    [OPC_CRCC_W_B_W] =format_4,
    [OPC_CRCC_W_H_W] =format_4,
    [OPC_CRCC_W_W_W] =format_4,
    [OPC_CRCC_W_D_W] =format_4,
    [OPC_BREAK] =format_5,
    [OPC_DBCL] =format_5,
    [OPC_SYSCALL] =format_5,
    [OPC_ALSL_D] =format_2,
    [OPC_SLLI_W] =format_6,
    [OPC_SLLI_D] =format_7,
    [OPC_SRLI_W] =format_6,
    [OPC_SRLI_D] =format_7,
    [OPC_SRAI_W] =format_6,
    [OPC_SRAI_D] =format_7,
    [OPC_ROTRI_W] =format_6,
    [OPC_ROTRI_D] =format_7,
    [OPC_BSTRINS_W] =format_8,
    [OPC_BSTRPICK_W] =format_8,
    [OPC_BSTRINS_D] =format_9,
    [OPC_BSTRPICK_D] =format_9,
    [OPC_FADD_S] =format_10,
    [OPC_FADD_D] =format_10,
    [OPC_FSUB_S] =format_10,
    [OPC_FSUB_D] =format_10,
    [OPC_FMUL_S] =format_10,
    [OPC_FMUL_D] =format_10,
    [OPC_FDIV_S] =format_10,
    [OPC_FDIV_D] =format_10,
    [OPC_FMAX_S] =format_10,
    [OPC_FMAX_D] =format_10,
    [OPC_FMIN_S] =format_10,
    [OPC_FMIN_D] =format_10,
    [OPC_FMAXA_S] =format_10,
    [OPC_FMAXA_D] =format_10,
    [OPC_FMINA_S] =format_10,
    [OPC_FMINA_D] =format_10,
    [OPC_FSCALEB_S] =format_10,
    [OPC_FSCALEB_D] =format_10,
    [OPC_FCOPYSIGN_S] =format_10,
    [OPC_FCOPYSIGN_D] =format_10,
    [OPC_FABS_S] =format_11,
    [OPC_FABS_D] =format_11,
    [OPC_FNEG_S] =format_11,
    [OPC_FNEG_D] =format_11,
    [OPC_FLOGB_S] =format_11,
    [OPC_FLOGB_D] =format_11,
    [OPC_FCLASS_S] =format_11,
    [OPC_FCLASS_D] =format_11,
    [OPC_FSQRT_S] =format_11,
    [OPC_FSQRT_D] =format_11,
    [OPC_FRECIP_S] =format_11,
    [OPC_FRECIP_D] =format_11,
    [OPC_FRSQRT_S] =format_11,
    [OPC_FRSQRT_D] =format_11,
    [OPC_FMOV_S] =format_11,
    [OPC_FMOV_D] =format_11,
    [OPC_MOVGR2FR_W] =format_12,
    [OPC_MOVGR2FR_D] =format_12,
    [OPC_MOVGR2FRH_W] =format_12,
    [OPC_MOVFR2GR_S] =format_13,
    [OPC_MOVFR2GR_D] =format_13,
    [OPC_MOVFRH2GR_S] =format_13,
    [OPC_MOVGR2FCSR] =format_14,
    [OPC_MOVFCSR2GR] =format_15,
    [OPC_MOVFR2CF] =format_16,
    [OPC_MOVCF2FR] =format_17,
    [OPC_MOVGR2CF] =format_18,
    [OPC_MOVCF2GR] =format_19,
    [OPC_FCVT_S_D] =format_11,
    [OPC_FCVT_D_S] =format_11,
    [OPC_FTINTRM_W_S] =format_11,
    [OPC_FTINTRM_W_D] =format_11,
    [OPC_FTINTRM_L_S] =format_11,
    [OPC_FTINTRM_L_D] =format_11,
    [OPC_FTINTRP_W_S] =format_11,
    [OPC_FTINTRP_W_D] =format_11,
    [OPC_FTINTRP_L_S] =format_11,
    [OPC_FTINTRP_L_D] =format_11,
    [OPC_FTINTRZ_W_S] =format_11,
    [OPC_FTINTRZ_W_D] =format_11,
    [OPC_FTINTRZ_L_S] =format_11,
    [OPC_FTINTRZ_L_D] =format_11,
    [OPC_FTINTRNE_W_S] =format_11,
    [OPC_FTINTRNE_W_D] =format_11,
    [OPC_FTINTRNE_L_S] =format_11,
    [OPC_FTINTRNE_L_D] =format_11,
    [OPC_FTINT_W_S] =format_11,
    [OPC_FTINT_W_D] =format_11,
    [OPC_FTINT_L_S] =format_11,
    [OPC_FTINT_L_D] =format_11,
    [OPC_FFINT_S_W] =format_11,
    [OPC_FFINT_S_L] =format_11,
    [OPC_FFINT_D_W] =format_11,
    [OPC_FFINT_D_L] =format_11,
    [OPC_FRINT_S] =format_11,
    [OPC_FRINT_D] =format_11,
    [OPC_SLTI] =format_20,
    [OPC_SLTUI] =format_20,
    [OPC_ADDI_W] =format_20,
    [OPC_ADDI_D] =format_20,
    [OPC_LU52I_D] =format_20,
    [OPC_ANDI] =format_21,
    [OPC_ORI] =format_21,
    [OPC_XORI] =format_21,
    [OPC_CSRRD] =format_22,
    [OPC_CSRWR] =format_22,
    [OPC_CSRXCHG] =format_23,
    [OPC_CACOP] =format_24,
    [OPC_LDDIR] =format_25,
    [OPC_LDPTE] =format_26,
    [OPC_IOCSRRD_B] =format_0,
    [OPC_IOCSRRD_H] =format_0,
    [OPC_IOCSRRD_W] =format_0,
    [OPC_IOCSRRD_D] =format_0,
    [OPC_IOCSRWR_B] =format_0,
    [OPC_IOCSRWR_H] =format_0,
    [OPC_IOCSRWR_W] =format_0,
    [OPC_IOCSRWR_D] =format_0,
    [OPC_TLBCLR] =format_27,
    [OPC_TLBFLUSH] =format_27,
    [OPC_TLBSRCH] =format_27,
    [OPC_TLBRD] =format_27,
    [OPC_TLBWR] =format_27,
    [OPC_TLBFILL] =format_27,
    [OPC_ERTN] =format_27,
    [OPC_IDLE] =format_28,
    [OPC_INVTLB] =format_29,
    [OPC_FMADD_S] =format_30,
    [OPC_FMADD_D] =format_30,
    [OPC_FMSUB_S] =format_30,
    [OPC_FMSUB_D] =format_30,
    [OPC_FNMADD_S] =format_30,
    [OPC_FNMADD_D] =format_30,
    [OPC_FNMSUB_S] =format_30,
    [OPC_FNMSUB_D] =format_30,
    [OPC_FCMP_CAF_S] =format_31,
    [OPC_FCMP_SAF_S] =format_31,
    [OPC_FCMP_CLT_S] =format_31,
    [OPC_FCMP_STL_S] =format_31,
    [OPC_FCMP_CEQ_S] =format_31,
    [OPC_FCMP_SEQ_S] =format_31,
    [OPC_FCMP_CLE_S] =format_31,
    [OPC_FCMP_SLE_S] =format_31,
    [OPC_FCMP_CUN_S] =format_31,
    [OPC_FCMP_SUN_S] =format_31,
    [OPC_FCMP_CULT_S] =format_31,
    [OPC_FCMP_SULT_S] =format_31,
    [OPC_FCMP_CUEQ_S] =format_31,
    [OPC_FCMP_SUEQ_S] =format_31,
    [OPC_FCMP_CULE_S] =format_31,
    [OPC_FCMP_SULE_S] =format_31,
    [OPC_FCMP_CNE_S] =format_31,
    [OPC_FCMP_SNE_S] =format_31,
    [OPC_FCMP_COR_S] =format_31,
    [OPC_FCMP_SOR_S] =format_31,
    [OPC_FCMP_CUNE_S] =format_31,
    [OPC_FCMP_SUNE_S] =format_31,
    [OPC_FCMP_CAF_D] =format_31,
    [OPC_FCMP_SAF_D] =format_31,
    [OPC_FCMP_CLT_D] =format_31,
    [OPC_FCMP_STL_D] =format_31,
    [OPC_FCMP_CEQ_D] =format_31,
    [OPC_FCMP_SEQ_D] =format_31,
    [OPC_FCMP_CLE_D] =format_31,
    [OPC_FCMP_SLE_D] =format_31,
    [OPC_FCMP_CUN_D] =format_31,
    [OPC_FCMP_SUN_D] =format_31,
    [OPC_FCMP_CULT_D] =format_31,
    [OPC_FCMP_SULT_D] =format_31,
    [OPC_FCMP_CUEQ_D] =format_31,
    [OPC_FCMP_SUEQ_D] =format_31,
    [OPC_FCMP_CULE_D] =format_31,
    [OPC_FCMP_SULE_D] =format_31,
    [OPC_FCMP_CNE_D] =format_31,
    [OPC_FCMP_SNE_D] =format_31,
    [OPC_FCMP_COR_D] =format_31,
    [OPC_FCMP_SOR_D] =format_31,
    [OPC_FCMP_CUNE_D] =format_31,
    [OPC_FCMP_SUNE_D] =format_31,
    [OPC_FSEL] =format_32,
    [OPC_ADDU16I_D] =format_33,
    [OPC_LU12I_W] =format_34,
    [OPC_LU32I_D] =format_34,
    [OPC_PCADDI] =format_34,
    [OPC_PCALAU12I] =format_34,
    [OPC_PCADDU12I] =format_34,
    [OPC_PCADDU18I] =format_34,
    [OPC_LL_W] =format_35,
    [OPC_SC_W] =format_35,
    [OPC_LL_D] =format_35,
    [OPC_SC_D] =format_35,
    [OPC_LDPTR_W] =format_35,
    [OPC_STPTR_W] =format_35,
    [OPC_LDPTR_D] =format_35,
    [OPC_STPTR_D] =format_35,
    [OPC_LD_B] =format_20,
    [OPC_LD_H] =format_20,
    [OPC_LD_W] =format_20,
    [OPC_LD_D] =format_20,
    [OPC_ST_B] =format_20,
    [OPC_ST_H] =format_20,
    [OPC_ST_W] =format_20,
    [OPC_ST_D] =format_20,
    [OPC_LD_BU] =format_20,
    [OPC_LD_HU] =format_20,
    [OPC_LD_WU] =format_20,
    [OPC_PRELD] =format_36,
    [OPC_FLD_S] =format_37,
    [OPC_FST_S] =format_37,
    [OPC_FLD_D] =format_37,
    [OPC_FST_D] =format_37,
    [OPC_LDX_B] =format_4,
    [OPC_LDX_H] =format_4,
    [OPC_LDX_W] =format_4,
    [OPC_LDX_D] =format_4,
    [OPC_STX_B] =format_4,
    [OPC_STX_H] =format_4,
    [OPC_STX_W] =format_4,
    [OPC_STX_D] =format_4,
    [OPC_LDX_BU] =format_4,
    [OPC_LDX_HU] =format_4,
    [OPC_LDX_WU] =format_4,
    [OPC_PRELDX] =format_38,
    [OPC_FLDX_S] =format_39,
    [OPC_FLDX_D] =format_39,
    [OPC_FSTX_S] =format_39,
    [OPC_FSTX_D] =format_39,
    [OPC_AMSWAP_W] =format_4,
    [OPC_AMSWAP_D] =format_4,
    [OPC_AMADD_W] =format_4,
    [OPC_AMADD_D] =format_4,
    [OPC_AMAND_W] =format_4,
    [OPC_AMAND_D] =format_4,
    [OPC_AMOR_W] =format_4,
    [OPC_AMOR_D] =format_4,
    [OPC_AMXOR_W] =format_4,
    [OPC_AMXOR_D] =format_4,
    [OPC_AMMAX_W] =format_4,
    [OPC_AMMAX_D] =format_4,
    [OPC_AMMIN_W] =format_4,
    [OPC_AMMIN_D] =format_4,
    [OPC_AMMAX_WU] =format_4,
    [OPC_AMMAX_DU] =format_4,
    [OPC_AMMIN_WU] =format_4,
    [OPC_AMMIN_DU] =format_4,
    [OPC_AMSWAP_DB_W] =format_4,
    [OPC_AMSWAP_DB_D] =format_4,
    [OPC_AMADD_DB_W] =format_4,
    [OPC_AMADD_DB_D] =format_4,
    [OPC_AMAND_DB_W] =format_4,
    [OPC_AMAND_DB_D] =format_4,
    [OPC_AMOR_DB_W] =format_4,
    [OPC_AMOR_DB_D] =format_4,
    [OPC_AMXOR_DB_W] =format_4,
    [OPC_AMXOR_DB_D] =format_4,
    [OPC_AMMAX_DB_W] =format_4,
    [OPC_AMMAX_DB_D] =format_4,
    [OPC_AMMIN_DB_W] =format_4,
    [OPC_AMMIN_DB_D] =format_4,
    [OPC_AMMAX_DB_WU] =format_4,
    [OPC_AMMAX_DB_DU] =format_4,
    [OPC_AMMIN_DB_WU] =format_4,
    [OPC_AMMIN_DB_DU] =format_4,
    [OPC_DBAR] =format_40,
    [OPC_IBAR] =format_41,
    [OPC_FLDGT_S] =format_39,
    [OPC_FLDGT_D] =format_39,
    [OPC_FLDLE_S] =format_39,
    [OPC_FLDLE_D] =format_39,
    [OPC_FSTGT_S] =format_39,
    [OPC_FSTGT_D] =format_39,
    [OPC_FSTLE_S] =format_39,
    [OPC_FSTLE_D] =format_39,
    [OPC_LDGT_B] =format_4,
    [OPC_LDGT_H] =format_4,
    [OPC_LDGT_W] =format_4,
    [OPC_LDGT_D] =format_4,
    [OPC_LDLE_B] =format_4,
    [OPC_LDLE_H] =format_4,
    [OPC_LDLE_W] =format_4,
    [OPC_LDLE_D] =format_4,
    [OPC_STGT_B] =format_4,
    [OPC_STGT_H] =format_4,
    [OPC_STGT_W] =format_4,
    [OPC_STGT_D] =format_4,
    [OPC_STLE_B] =format_4,
    [OPC_STLE_H] =format_4,
    [OPC_STLE_W] =format_4,
    [OPC_STLE_D] =format_4,
    [OPC_BEQZ] =format_42,
    [OPC_BNEZ] =format_42,
    [OPC_BCEQZ] =format_43,
    [OPC_BCNEZ] =format_43,
    [OPC_JIRL] =format_44,
    [OPC_B] =format_45,
    [OPC_BL] =format_45,
    [OPC_BEQ] =format_44,
    [OPC_BNE] =format_44,
    [OPC_BLT] =format_44,
    [OPC_BGE] =format_44,
    [OPC_BLTU] =format_44,
    [OPC_BGEU] =format_44,
    [OPC_GR2SCR] =format_46,
    [OPC_SRC2GR] =format_47,
    [OPC_X86MTTOP] =format_48,
    [OPC_X86MFTOP] =format_49,
    [OPC_SETX86LOOPE] =format_0,
    [OPC_SETX86LOOPNE] =format_0,
    [OPC_X86INC_B] =format_50,
    [OPC_X86INC_H] =format_50,
    [OPC_X86INC_W] =format_50,
    [OPC_X86INC_D] =format_50,
    [OPC_X86DEC_B] =format_50,
    [OPC_X86DEC_H] =format_50,
    [OPC_X86DEC_W] =format_50,
    [OPC_X86DEC_D] =format_50,
    [OPC_X86SETTM] =format_27,
    [OPC_X86CLRTM] =format_27,
    [OPC_X86INCTOP] =format_27,
    [OPC_X86DECTOP] =format_27,
    [OPC_ROTR_B] =format_4,
    [OPC_ROTR_H] =format_4,
    [OPC_ADDU12I_W] =format_51,
    [OPC_ADDU12I_D] =format_51,
    [OPC_HVCL] =format_5,
    [OPC_ADC_B] =format_4,
    [OPC_ADC_H] =format_4,
    [OPC_ADC_W] =format_4,
    [OPC_ADC_D] =format_4,
    [OPC_SBC_B] =format_4,
    [OPC_SBC_H] =format_4,
    [OPC_SBC_W] =format_4,
    [OPC_SBC_D] =format_4,
    [OPC_RCR_B] =format_4,
    [OPC_RCR_H] =format_4,
    [OPC_RCR_W] =format_4,
    [OPC_RCR_D] =format_4,
    [OPC_ARMMOVE] =format_52,
    [OPC_SETX86J] =format_53,
    [OPC_SETARMJ] =format_53,
    [OPC_ARMADD_W] =format_54,
    [OPC_ARMSUB_W] =format_54,
    [OPC_ARMADC_W] =format_54,
    [OPC_ARMSBC_W] =format_54,
    [OPC_ARMAND_W] =format_54,
    [OPC_ARMOR_W] =format_54,
    [OPC_ARMXOR_W] =format_54,
    [OPC_ARMSLL_W] =format_54,
    [OPC_ARMSRL_W] =format_54,
    [OPC_ARMSRA_W] =format_54,
    [OPC_ARMROTR_W] =format_54,
    [OPC_ARMSLLI_W] =format_54,
    [OPC_ARMSRLI_W] =format_54,
    [OPC_ARMSRAI_W] =format_54,
    [OPC_ARMROTRI_W] =format_54,
    [OPC_X86MUL_B] =format_1,
    [OPC_X86MUL_H] =format_1,
    [OPC_X86MUL_W] =format_1,
    [OPC_X86MUL_D] =format_1,
    [OPC_X86MUL_BU] =format_1,
    [OPC_X86MUL_HU] =format_1,
    [OPC_X86MUL_WU] =format_1,
    [OPC_X86MUL_DU] =format_1,
    [OPC_X86ADD_WU] =format_1,
    [OPC_X86ADD_DU] =format_1,
    [OPC_X86SUB_WU] =format_1,
    [OPC_X86SUB_DU] =format_1,
    [OPC_X86ADD_B] =format_1,
    [OPC_X86ADD_H] =format_1,
    [OPC_X86ADD_W] =format_1,
    [OPC_X86ADD_D] =format_1,
    [OPC_X86SUB_B] =format_1,
    [OPC_X86SUB_H] =format_1,
    [OPC_X86SUB_W] =format_1,
    [OPC_X86SUB_D] =format_1,
    [OPC_X86ADC_B] =format_1,
    [OPC_X86ADC_H] =format_1,
    [OPC_X86ADC_W] =format_1,
    [OPC_X86ADC_D] =format_1,
    [OPC_X86SBC_B] =format_1,
    [OPC_X86SBC_H] =format_1,
    [OPC_X86SBC_W] =format_1,
    [OPC_X86SBC_D] =format_1,
    [OPC_X86SLL_B] =format_1,
    [OPC_X86SLL_H] =format_1,
    [OPC_X86SLL_W] =format_1,
    [OPC_X86SLL_D] =format_1,
    [OPC_X86SRL_B] =format_1,
    [OPC_X86SRL_H] =format_1,
    [OPC_X86SRL_W] =format_1,
    [OPC_X86SRL_D] =format_1,
    [OPC_X86SRA_B] =format_1,
    [OPC_X86SRA_H] =format_1,
    [OPC_X86SRA_W] =format_1,
    [OPC_X86SRA_D] =format_1,
    [OPC_X86ROTR_B] =format_1,
    [OPC_X86ROTR_H] =format_1,
    [OPC_X86ROTR_D] =format_1,
    [OPC_X86ROTR_W] =format_1,
    [OPC_X86ROTL_B] =format_1,
    [OPC_X86ROTL_H] =format_1,
    [OPC_X86ROTL_W] =format_1,
    [OPC_X86ROTL_D] =format_1,
    [OPC_X86RCR_B] =format_1,
    [OPC_X86RCR_H] =format_1,
    [OPC_X86RCR_W] =format_1,
    [OPC_X86RCR_D] =format_1,
    [OPC_X86RCL_B] =format_1,
    [OPC_X86RCL_H] =format_1,
    [OPC_X86RCL_W] =format_1,
    [OPC_X86RCL_D] =format_1,
    [OPC_X86AND_B] =format_1,
    [OPC_X86AND_H] =format_1,
    [OPC_X86AND_W] =format_1,
    [OPC_X86AND_D] =format_1,
    [OPC_X86OR_B] =format_1,
    [OPC_X86OR_H] =format_1,
    [OPC_X86OR_W] =format_1,
    [OPC_X86OR_D] =format_1,
    [OPC_X86XOR_B] =format_1,
    [OPC_X86XOR_H] =format_1,
    [OPC_X86XOR_W] =format_1,
    [OPC_X86XOR_D] =format_1,
    [OPC_ARMNOT_W] =format_55,
    [OPC_ARMMOV_W] =format_55,
    [OPC_ARMMOV_D] =format_55,
    [OPC_ARMRRX_W] =format_55,
    [OPC_ROTRI_B] =format_56,
    [OPC_ROTRI_H] =format_57,
    [OPC_RCRI_B] =format_56,
    [OPC_RCRI_H] =format_57,
    [OPC_RCRI_W] =format_6,
    [OPC_RCRI_D] =format_7,
    [OPC_X86SLLI_B] =format_58,
    [OPC_X86SLLI_H] =format_59,
    [OPC_X86SLLI_W] =format_60,
    [OPC_X86SLLI_D] =format_61,
    [OPC_X86SRLI_B] =format_58,
    [OPC_X86SRLI_H] =format_59,
    [OPC_X86SRLI_W] =format_60,
    [OPC_X86SRLI_D] =format_61,
    [OPC_X86SRAI_B] =format_58,
    [OPC_X86SRAI_H] =format_59,
    [OPC_X86SRAI_W] =format_60,
    [OPC_X86SRAI_D] =format_61,
    [OPC_X86ROTRI_B] =format_58,
    [OPC_X86ROTRI_H] =format_59,
    [OPC_X86ROTRI_W] =format_60,
    [OPC_X86ROTRI_D] =format_61,
    [OPC_X86RCRI_B] =format_58,
    [OPC_X86RCRI_H] =format_59,
    [OPC_X86RCRI_W] =format_60,
    [OPC_X86RCRI_D] =format_61,
    [OPC_X86ROTLI_B] =format_58,
    [OPC_X86ROTLI_H] =format_59,
    [OPC_X86ROTLI_W] =format_60,
    [OPC_X86ROTLI_D] =format_61,
    [OPC_X86RCLI_B] =format_58,
    [OPC_X86RCLI_H] =format_59,
    [OPC_X86RCLI_W] =format_60,
    [OPC_X86RCLI_D] =format_61,
    [OPC_X86SETTAG] =format_62,
    [OPC_X86MFFLAG] =format_63,
    [OPC_X86MTFLAG] =format_63,
    [OPC_ARMMFFLAG] =format_63,
    [OPC_ARMMTFLAG] =format_63,
    [OPC_FCVT_LD_D] =format_11,
    [OPC_FCVT_UD_D] =format_11,
    [OPC_FCVT_D_LC] =format_10,
    [OPC_GCSRRD] =format_22,
    [OPC_GCSRWR] =format_22,
    [OPC_GCSRXCHG] =format_23,
    [OPC_GTLBCLR] =format_27,
    [OPC_GTLBFLUSH] =format_27,
    [OPC_GTLBSRCH] =format_27,
    [OPC_GTLBRD] =format_27,
    [OPC_GTLBWR] =format_27,
    [OPC_GTLBFILL] =format_27,
    [OPC_SCFGR_W] =format_0,
    [OPC_SCFGR_D] =format_0,
    [OPC_SCFGW_W] =format_0,
    [OPC_SCFGW_D] =format_0,
    [OPC_SNOP] =format_27,
    [OPC_LDL_W] =format_20,
    [OPC_LDR_W] =format_20,
    [OPC_LDL_D] =format_20,
    [OPC_LDR_D] =format_20,
    [OPC_STL_W] =format_20,
    [OPC_STR_W] =format_20,
    [OPC_STL_D] =format_20,
    [OPC_STR_D] =format_20,
    [OPC_JISCR0] =format_64,
    [OPC_JISCR1] =format_64,
    [OPC_VADD_B] =vvv,
    [OPC_VADD_H] =vvv,
    [OPC_VADD_W] =vvv,
    [OPC_VADD_D] =vvv,
    [OPC_VADD_Q] =vvv,
    [OPC_VSUB_B] =vvv,
    [OPC_VSUB_H] =vvv,
    [OPC_VSUB_W] =vvv,
    [OPC_VSUB_D] =vvv,
    [OPC_VSUB_Q] =vvv,
    [OPC_VADDI_BU] =vv_ui5,
    [OPC_VADDI_HU] =vv_ui5,
    [OPC_VADDI_WU] =vv_ui5,
    [OPC_VADDI_DU] =vv_ui5,
    [OPC_VSUBI_BU] =vv_ui5,
    [OPC_VSUBI_HU] =vv_ui5,
    [OPC_VSUBI_WU] =vv_ui5,
    [OPC_VSUBI_DU] =vv_ui5,
    [OPC_VNEG_B] =vv,
    [OPC_VNEG_H] =vv,
    [OPC_VNEG_W] =vv,
    [OPC_VNEG_D] =vv,
    [OPC_VSADD_B] =vvv,
    [OPC_VSADD_H] =vvv,
    [OPC_VSADD_W] =vvv,
    [OPC_VSADD_D] =vvv,
    [OPC_VSADD_BU] =vvv,
    [OPC_VSADD_HU] =vvv,
    [OPC_VSADD_WU] =vvv,
    [OPC_VSADD_DU] =vvv,
    [OPC_VSSUB_B] =vvv,
    [OPC_VSSUB_H] =vvv,
    [OPC_VSSUB_W] =vvv,
    [OPC_VSSUB_D] =vvv,
    [OPC_VSSUB_BU] =vvv,
    [OPC_VSSUB_HU] =vvv,
    [OPC_VSSUB_WU] =vvv,
    [OPC_VSSUB_DU] =vvv,
    [OPC_VHADDW_H_B] =vvv,
    [OPC_VHADDW_W_H] =vvv,
    [OPC_VHADDW_D_W] =vvv,
    [OPC_VHADDW_Q_D] =vvv,
    [OPC_VHADDW_HU_BU] =vvv,
    [OPC_VHADDW_WU_HU] =vvv,
    [OPC_VHADDW_DU_WU] =vvv,
    [OPC_VHADDW_QU_DU] =vvv,
    [OPC_VHSUBW_H_B] =vvv,
    [OPC_VHSUBW_W_H] =vvv,
    [OPC_VHSUBW_D_W] =vvv,
    [OPC_VHSUBW_Q_D] =vvv,
    [OPC_VHSUBW_HU_BU] =vvv,
    [OPC_VHSUBW_WU_HU] =vvv,
    [OPC_VHSUBW_DU_WU] =vvv,
    [OPC_VHSUBW_QU_DU] =vvv,
    [OPC_VADDWEV_H_B] =vvv,
    [OPC_VADDWEV_W_H] =vvv,
    [OPC_VADDWEV_D_W] =vvv,
    [OPC_VADDWEV_Q_D] =vvv,
    [OPC_VADDWOD_H_B] =vvv,
    [OPC_VADDWOD_W_H] =vvv,
    [OPC_VADDWOD_D_W] =vvv,
    [OPC_VADDWOD_Q_D] =vvv,
    [OPC_VSUBWEV_H_B] =vvv,
    [OPC_VSUBWEV_W_H] =vvv,
    [OPC_VSUBWEV_D_W] =vvv,
    [OPC_VSUBWEV_Q_D] =vvv,
    [OPC_VSUBWOD_H_B] =vvv,
    [OPC_VSUBWOD_W_H] =vvv,
    [OPC_VSUBWOD_D_W] =vvv,
    [OPC_VSUBWOD_Q_D] =vvv,
    [OPC_VADDWEV_H_BU] =vvv,
    [OPC_VADDWEV_W_HU] =vvv,
    [OPC_VADDWEV_D_WU] =vvv,
    [OPC_VADDWEV_Q_DU] =vvv,
    [OPC_VADDWOD_H_BU] =vvv,
    [OPC_VADDWOD_W_HU] =vvv,
    [OPC_VADDWOD_D_WU] =vvv,
    [OPC_VADDWOD_Q_DU] =vvv,
    [OPC_VSUBWEV_H_BU] =vvv,
    [OPC_VSUBWEV_W_HU] =vvv,
    [OPC_VSUBWEV_D_WU] =vvv,
    [OPC_VSUBWEV_Q_DU] =vvv,
    [OPC_VSUBWOD_H_BU] =vvv,
    [OPC_VSUBWOD_W_HU] =vvv,
    [OPC_VSUBWOD_D_WU] =vvv,
    [OPC_VSUBWOD_Q_DU] =vvv,
    [OPC_VADDWEV_H_BU_B] =vvv,
    [OPC_VADDWEV_W_HU_H] =vvv,
    [OPC_VADDWEV_D_WU_W] =vvv,
    [OPC_VADDWEV_Q_DU_D] =vvv,
    [OPC_VADDWOD_H_BU_B] =vvv,
    [OPC_VADDWOD_W_HU_H] =vvv,
    [OPC_VADDWOD_D_WU_W] =vvv,
    [OPC_VADDWOD_Q_DU_D] =vvv,
    [OPC_VAVG_B] =vvv,
    [OPC_VAVG_H] =vvv,
    [OPC_VAVG_W] =vvv,
    [OPC_VAVG_D] =vvv,
    [OPC_VAVG_BU] =vvv,
    [OPC_VAVG_HU] =vvv,
    [OPC_VAVG_WU] =vvv,
    [OPC_VAVG_DU] =vvv,
    [OPC_VAVGR_B] =vvv,
    [OPC_VAVGR_H] =vvv,
    [OPC_VAVGR_W] =vvv,
    [OPC_VAVGR_D] =vvv,
    [OPC_VAVGR_BU] =vvv,
    [OPC_VAVGR_HU] =vvv,
    [OPC_VAVGR_WU] =vvv,
    [OPC_VAVGR_DU] =vvv,
    [OPC_VABSD_B] =vvv,
    [OPC_VABSD_H] =vvv,
    [OPC_VABSD_W] =vvv,
    [OPC_VABSD_D] =vvv,
    [OPC_VABSD_BU] =vvv,
    [OPC_VABSD_HU] =vvv,
    [OPC_VABSD_WU] =vvv,
    [OPC_VABSD_DU] =vvv,
    [OPC_VADDA_B] =vvv,
    [OPC_VADDA_H] =vvv,
    [OPC_VADDA_W] =vvv,
    [OPC_VADDA_D] =vvv,
    [OPC_VMAX_B] =vvv,
    [OPC_VMAX_H] =vvv,
    [OPC_VMAX_W] =vvv,
    [OPC_VMAX_D] =vvv,
    [OPC_VMAXI_B] =vv_i5,
    [OPC_VMAXI_H] =vv_i5,
    [OPC_VMAXI_W] =vv_i5,
    [OPC_VMAXI_D] =vv_i5,
    [OPC_VMAX_BU] =vvv,
    [OPC_VMAX_HU] =vvv,
    [OPC_VMAX_WU] =vvv,
    [OPC_VMAX_DU] =vvv,
    [OPC_VMAXI_BU] =vv_ui5,
    [OPC_VMAXI_HU] =vv_ui5,
    [OPC_VMAXI_WU] =vv_ui5,
    [OPC_VMAXI_DU] =vv_ui5,
    [OPC_VMIN_B] =vvv,
    [OPC_VMIN_H] =vvv,
    [OPC_VMIN_W] =vvv,
    [OPC_VMIN_D] =vvv,
    [OPC_VMINI_B] =vv_i5,
    [OPC_VMINI_H] =vv_i5,
    [OPC_VMINI_W] =vv_i5,
    [OPC_VMINI_D] =vv_i5,
    [OPC_VMIN_BU] =vvv,
    [OPC_VMIN_HU] =vvv,
    [OPC_VMIN_WU] =vvv,
    [OPC_VMIN_DU] =vvv,
    [OPC_VMINI_BU] =vv_ui5,
    [OPC_VMINI_HU] =vv_ui5,
    [OPC_VMINI_WU] =vv_ui5,
    [OPC_VMINI_DU] =vv_ui5,
    [OPC_VMUL_B] =vvv,
    [OPC_VMUL_H] =vvv,
    [OPC_VMUL_W] =vvv,
    [OPC_VMUL_D] =vvv,
    [OPC_VMUH_B] =vvv,
    [OPC_VMUH_H] =vvv,
    [OPC_VMUH_W] =vvv,
    [OPC_VMUH_D] =vvv,
    [OPC_VMUH_BU] =vvv,
    [OPC_VMUH_HU] =vvv,
    [OPC_VMUH_WU] =vvv,
    [OPC_VMUH_DU] =vvv,
    [OPC_VMULWEV_H_B] =vvv,
    [OPC_VMULWEV_W_H] =vvv,
    [OPC_VMULWEV_D_W] =vvv,
    [OPC_VMULWEV_Q_D] =vvv,
    [OPC_VMULWOD_H_B] =vvv,
    [OPC_VMULWOD_W_H] =vvv,
    [OPC_VMULWOD_D_W] =vvv,
    [OPC_VMULWOD_Q_D] =vvv,
    [OPC_VMULWEV_H_BU] =vvv,
    [OPC_VMULWEV_W_HU] =vvv,
    [OPC_VMULWEV_D_WU] =vvv,
    [OPC_VMULWEV_Q_DU] =vvv,
    [OPC_VMULWOD_H_BU] =vvv,
    [OPC_VMULWOD_W_HU] =vvv,
    [OPC_VMULWOD_D_WU] =vvv,
    [OPC_VMULWOD_Q_DU] =vvv,
    [OPC_VMULWEV_H_BU_B] =vvv,
    [OPC_VMULWEV_W_HU_H] =vvv,
    [OPC_VMULWEV_D_WU_W] =vvv,
    [OPC_VMULWEV_Q_DU_D] =vvv,
    [OPC_VMULWOD_H_BU_B] =vvv,
    [OPC_VMULWOD_W_HU_H] =vvv,
    [OPC_VMULWOD_D_WU_W] =vvv,
    [OPC_VMULWOD_Q_DU_D] =vvv,
    [OPC_VMADD_B] =vvv,
    [OPC_VMADD_H] =vvv,
    [OPC_VMADD_W] =vvv,
    [OPC_VMADD_D] =vvv,
    [OPC_VMSUB_B] =vvv,
    [OPC_VMSUB_H] =vvv,
    [OPC_VMSUB_W] =vvv,
    [OPC_VMSUB_D] =vvv,
    [OPC_VMADDWEV_H_B] =vvv,
    [OPC_VMADDWEV_W_H] =vvv,
    [OPC_VMADDWEV_D_W] =vvv,
    [OPC_VMADDWEV_Q_D] =vvv,
    [OPC_VMADDWOD_H_B] =vvv,
    [OPC_VMADDWOD_W_H] =vvv,
    [OPC_VMADDWOD_D_W] =vvv,
    [OPC_VMADDWOD_Q_D] =vvv,
    [OPC_VMADDWEV_H_BU] =vvv,
    [OPC_VMADDWEV_W_HU] =vvv,
    [OPC_VMADDWEV_D_WU] =vvv,
    [OPC_VMADDWEV_Q_DU] =vvv,
    [OPC_VMADDWOD_H_BU] =vvv,
    [OPC_VMADDWOD_W_HU] =vvv,
    [OPC_VMADDWOD_D_WU] =vvv,
    [OPC_VMADDWOD_Q_DU] =vvv,
    [OPC_VMADDWEV_H_BU_B] =vvv,
    [OPC_VMADDWEV_W_HU_H] =vvv,
    [OPC_VMADDWEV_D_WU_W] =vvv,
    [OPC_VMADDWEV_Q_DU_D] =vvv,
    [OPC_VMADDWOD_H_BU_B] =vvv,
    [OPC_VMADDWOD_W_HU_H] =vvv,
    [OPC_VMADDWOD_D_WU_W] =vvv,
    [OPC_VMADDWOD_Q_DU_D] =vvv,
    [OPC_VDIV_B] =vvv,
    [OPC_VDIV_H] =vvv,
    [OPC_VDIV_W] =vvv,
    [OPC_VDIV_D] =vvv,
    [OPC_VDIV_BU] =vvv,
    [OPC_VDIV_HU] =vvv,
    [OPC_VDIV_WU] =vvv,
    [OPC_VDIV_DU] =vvv,
    [OPC_VMOD_B] =vvv,
    [OPC_VMOD_H] =vvv,
    [OPC_VMOD_W] =vvv,
    [OPC_VMOD_D] =vvv,
    [OPC_VMOD_BU] =vvv,
    [OPC_VMOD_HU] =vvv,
    [OPC_VMOD_WU] =vvv,
    [OPC_VMOD_DU] =vvv,
    [OPC_VSAT_B] =vv_ui3,
    [OPC_VSAT_H] =vv_ui4,
    [OPC_VSAT_W] =vv_ui5,
    [OPC_VSAT_D] =vv_ui6,
    [OPC_VSAT_BU] =vv_ui3,
    [OPC_VSAT_HU] =vv_ui4,
    [OPC_VSAT_WU] =vv_ui5,
    [OPC_VSAT_DU] =vv_ui6,
    [OPC_VEXTH_H_B] =vv,
    [OPC_VEXTH_W_H] =vv,
    [OPC_VEXTH_D_W] =vv,
    [OPC_VEXTH_Q_D] =vv,
    [OPC_VEXTH_HU_BU] =vv,
    [OPC_VEXTH_WU_HU] =vv,
    [OPC_VEXTH_DU_WU] =vv,
    [OPC_VEXTH_QU_DU] =vv,
    [OPC_VSIGNCOV_B] =vvv,
    [OPC_VSIGNCOV_H] =vvv,
    [OPC_VSIGNCOV_W] =vvv,
    [OPC_VSIGNCOV_D] =vvv,
    [OPC_VMSKLTZ_B] =vv,
    [OPC_VMSKLTZ_H] =vv,
    [OPC_VMSKLTZ_W] =vv,
    [OPC_VMSKLTZ_D] =vv,
    [OPC_VMSKGEZ_B] =vv,
    [OPC_VMSKNZ_B] =vv,
    [OPC_VLDI] =v_i13,
    [OPC_VAND_V] =vvv,
    [OPC_VOR_V] =vvv,
    [OPC_VXOR_V] =vvv,
    [OPC_VNOR_V] =vvv,
    [OPC_VANDN_V] =vvv,
    [OPC_VORN_V] =vvv,
    [OPC_VANDI_B] =vv_ui8,
    [OPC_VORI_B] =vv_ui8,
    [OPC_VXORI_B] =vv_ui8,
    [OPC_VNORI_B] =vv_ui8,
    [OPC_VSLL_B] =vvv,
    [OPC_VSLL_H] =vvv,
    [OPC_VSLL_W] =vvv,
    [OPC_VSLL_D] =vvv,
    [OPC_VSLLI_B] =vv_ui3,
    [OPC_VSLLI_H] =vv_ui4,
    [OPC_VSLLI_W] =vv_ui5,
    [OPC_VSLLI_D] =vv_ui6,
    [OPC_VSRL_B] =vvv,
    [OPC_VSRL_H] =vvv,
    [OPC_VSRL_W] =vvv,
    [OPC_VSRL_D] =vvv,
    [OPC_VSRLI_B] =vv_ui3,
    [OPC_VSRLI_H] =vv_ui4,
    [OPC_VSRLI_W] =vv_ui5,
    [OPC_VSRLI_D] =vv_ui6,
    [OPC_VSRA_B] =vvv,
    [OPC_VSRA_H] =vvv,
    [OPC_VSRA_W] =vvv,
    [OPC_VSRA_D] =vvv,
    [OPC_VSRAI_B] =vv_ui3,
    [OPC_VSRAI_H] =vv_ui4,
    [OPC_VSRAI_W] =vv_ui5,
    [OPC_VSRAI_D] =vv_ui6,
    [OPC_VROTR_B] =vvv,
    [OPC_VROTR_H] =vvv,
    [OPC_VROTR_W] =vvv,
    [OPC_VROTR_D] =vvv,
    [OPC_VROTRI_B] =vv_ui3,
    [OPC_VROTRI_H] =vv_ui4,
    [OPC_VROTRI_W] =vv_ui5,
    [OPC_VROTRI_D] =vv_ui6,
    [OPC_VSLLWIL_H_B] =vv_ui3,
    [OPC_VSLLWIL_W_H] =vv_ui4,
    [OPC_VSLLWIL_D_W] =vv_ui5,
    [OPC_VEXTL_Q_D] =vv,
    [OPC_VSLLWIL_HU_BU] =vv_ui3,
    [OPC_VSLLWIL_WU_HU] =vv_ui4,
    [OPC_VSLLWIL_DU_WU] =vv_ui5,
    [OPC_VEXTL_QU_DU] =vv,
    [OPC_VSRLR_B] =vvv,
    [OPC_VSRLR_H] =vvv,
    [OPC_VSRLR_W] =vvv,
    [OPC_VSRLR_D] =vvv,
    [OPC_VSRLRI_B] =vv_ui3,
    [OPC_VSRLRI_H] =vv_ui4,
    [OPC_VSRLRI_W] =vv_ui5,
    [OPC_VSRLRI_D] =vv_ui6,
    [OPC_VSRAR_B] =vvv,
    [OPC_VSRAR_H] =vvv,
    [OPC_VSRAR_W] =vvv,
    [OPC_VSRAR_D] =vvv,
    [OPC_VSRARI_B] =vv_ui3,
    [OPC_VSRARI_H] =vv_ui4,
    [OPC_VSRARI_W] =vv_ui5,
    [OPC_VSRARI_D] =vv_ui6,
    [OPC_VSRLN_B_H] =vvv,
    [OPC_VSRLN_H_W] =vvv,
    [OPC_VSRLN_W_D] =vvv,
    [OPC_VSRAN_B_H] =vvv,
    [OPC_VSRAN_H_W] =vvv,
    [OPC_VSRAN_W_D] =vvv,
    [OPC_VSRLNI_B_H] =vv_ui4,
    [OPC_VSRLNI_H_W] =vv_ui5,
    [OPC_VSRLNI_W_D] =vv_ui6,
    [OPC_VSRLNI_D_Q] =vv_ui7,
    [OPC_VSRANI_B_H] =vv_ui4,
    [OPC_VSRANI_H_W] =vv_ui5,
    [OPC_VSRANI_W_D] =vv_ui6,
    [OPC_VSRANI_D_Q] =vv_ui7,
    [OPC_VSRLRN_B_H] =vvv,
    [OPC_VSRLRN_H_W] =vvv,
    [OPC_VSRLRN_W_D] =vvv,
    [OPC_VSRARN_B_H] =vvv,
    [OPC_VSRARN_H_W] =vvv,
    [OPC_VSRARN_W_D] =vvv,
    [OPC_VSRLRNI_B_H] =vv_ui4,
    [OPC_VSRLRNI_H_W] =vv_ui5,
    [OPC_VSRLRNI_W_D] =vv_ui6,
    [OPC_VSRLRNI_D_Q] =vv_ui7,
    [OPC_VSRARNI_B_H] =vv_ui4,
    [OPC_VSRARNI_H_W] =vv_ui5,
    [OPC_VSRARNI_W_D] =vv_ui6,
    [OPC_VSRARNI_D_Q] =vv_ui7,
    [OPC_VSSRLN_B_H] =vvv,
    [OPC_VSSRLN_H_W] =vvv,
    [OPC_VSSRLN_W_D] =vvv,
    [OPC_VSSRAN_B_H] =vvv,
    [OPC_VSSRAN_H_W] =vvv,
    [OPC_VSSRAN_W_D] =vvv,
    [OPC_VSSRLN_BU_H] =vvv,
    [OPC_VSSRLN_HU_W] =vvv,
    [OPC_VSSRLN_WU_D] =vvv,
    [OPC_VSSRAN_BU_H] =vvv,
    [OPC_VSSRAN_HU_W] =vvv,
    [OPC_VSSRAN_WU_D] =vvv,
    [OPC_VSSRLNI_B_H] =vv_ui4,
    [OPC_VSSRLNI_H_W] =vv_ui5,
    [OPC_VSSRLNI_W_D] =vv_ui6,
    [OPC_VSSRLNI_D_Q] =vv_ui7,
    [OPC_VSSRANI_B_H] =vv_ui4,
    [OPC_VSSRANI_H_W] =vv_ui5,
    [OPC_VSSRANI_W_D] =vv_ui6,
    [OPC_VSSRANI_D_Q] =vv_ui7,
    [OPC_VSSRLNI_BU_H] =vv_ui4,
    [OPC_VSSRLNI_HU_W] =vv_ui5,
    [OPC_VSSRLNI_WU_D] =vv_ui6,
    [OPC_VSSRLNI_DU_Q] =vv_ui7,
    [OPC_VSSRANI_BU_H] =vv_ui4,
    [OPC_VSSRANI_HU_W] =vv_ui5,
    [OPC_VSSRANI_WU_D] =vv_ui6,
    [OPC_VSSRANI_DU_Q] =vv_ui7,
    [OPC_VSSRLRN_B_H] =vvv,
    [OPC_VSSRLRN_H_W] =vvv,
    [OPC_VSSRLRN_W_D] =vvv,
    [OPC_VSSRARN_B_H] =vvv,
    [OPC_VSSRARN_H_W] =vvv,
    [OPC_VSSRARN_W_D] =vvv,
    [OPC_VSSRLRN_BU_H] =vvv,
    [OPC_VSSRLRN_HU_W] =vvv,
    [OPC_VSSRLRN_WU_D] =vvv,
    [OPC_VSSRARN_BU_H] =vvv,
    [OPC_VSSRARN_HU_W] =vvv,
    [OPC_VSSRARN_WU_D] =vvv,
    [OPC_VSSRLRNI_B_H] =vv_ui4,
    [OPC_VSSRLRNI_H_W] =vv_ui5,
    [OPC_VSSRLRNI_W_D] =vv_ui6,
    [OPC_VSSRLRNI_D_Q] =vv_ui7,
    [OPC_VSSRARNI_B_H] =vv_ui4,
    [OPC_VSSRARNI_H_W] =vv_ui5,
    [OPC_VSSRARNI_W_D] =vv_ui6,
    [OPC_VSSRARNI_D_Q] =vv_ui7,
    [OPC_VSSRLRNI_BU_H] =vv_ui4,
    [OPC_VSSRLRNI_HU_W] =vv_ui5,
    [OPC_VSSRLRNI_WU_D] =vv_ui6,
    [OPC_VSSRLRNI_DU_Q] =vv_ui7,
    [OPC_VSSRARNI_BU_H] =vv_ui4,
    [OPC_VSSRARNI_HU_W] =vv_ui5,
    [OPC_VSSRARNI_WU_D] =vv_ui6,
    [OPC_VSSRARNI_DU_Q] =vv_ui7,
    [OPC_VCLO_B] =vv,
    [OPC_VCLO_H] =vv,
    [OPC_VCLO_W] =vv,
    [OPC_VCLO_D] =vv,
    [OPC_VCLZ_B] =vv,
    [OPC_VCLZ_H] =vv,
    [OPC_VCLZ_W] =vv,
    [OPC_VCLZ_D] =vv,
    [OPC_VPCNT_B] =vv,
    [OPC_VPCNT_H] =vv,
    [OPC_VPCNT_W] =vv,
    [OPC_VPCNT_D] =vv,
    [OPC_VBITCLR_B] =vvv,
    [OPC_VBITCLR_H] =vvv,
    [OPC_VBITCLR_W] =vvv,
    [OPC_VBITCLR_D] =vvv,
    [OPC_VBITCLRI_B] =vv_ui3,
    [OPC_VBITCLRI_H] =vv_ui4,
    [OPC_VBITCLRI_W] =vv_ui5,
    [OPC_VBITCLRI_D] =vv_ui6,
    [OPC_VBITSET_B] =vvv,
    [OPC_VBITSET_H] =vvv,
    [OPC_VBITSET_W] =vvv,
    [OPC_VBITSET_D] =vvv,
    [OPC_VBITSETI_B] =vv_ui3,
    [OPC_VBITSETI_H] =vv_ui4,
    [OPC_VBITSETI_W] =vv_ui5,
    [OPC_VBITSETI_D] =vv_ui6,
    [OPC_VBITREV_B] =vvv,
    [OPC_VBITREV_H] =vvv,
    [OPC_VBITREV_W] =vvv,
    [OPC_VBITREV_D] =vvv,
    [OPC_VBITREVI_B] =vv_ui3,
    [OPC_VBITREVI_H] =vv_ui4,
    [OPC_VBITREVI_W] =vv_ui5,
    [OPC_VBITREVI_D] =vv_ui6,
    [OPC_VFRSTP_B] =vvv,
    [OPC_VFRSTP_H] =vvv,
    [OPC_VFRSTPI_B] =vv_ui5,
    [OPC_VFRSTPI_H] =vv_ui5,
    [OPC_VFADD_S] =vvv,
    [OPC_VFADD_D] =vvv,
    [OPC_VFSUB_S] =vvv,
    [OPC_VFSUB_D] =vvv,
    [OPC_VFMUL_S] =vvv,
    [OPC_VFMUL_D] =vvv,
    [OPC_VFDIV_S] =vvv,
    [OPC_VFDIV_D] =vvv,
    [OPC_VFMADD_S] =vvvv,
    [OPC_VFMADD_D] =vvvv,
    [OPC_VFMSUB_S] =vvvv,
    [OPC_VFMSUB_D] =vvvv,
    [OPC_VFNMADD_S] =vvvv,
    [OPC_VFNMADD_D] =vvvv,
    [OPC_VFNMSUB_S] =vvvv,
    [OPC_VFNMSUB_D] =vvvv,
    [OPC_VFMAX_S] =vvv,
    [OPC_VFMAX_D] =vvv,
    [OPC_VFMIN_S] =vvv,
    [OPC_VFMIN_D] =vvv,
    [OPC_VFMAXA_S] =vvv,
    [OPC_VFMAXA_D] =vvv,
    [OPC_VFMINA_S] =vvv,
    [OPC_VFMINA_D] =vvv,
    [OPC_VFLOGB_S] =vv,
    [OPC_VFLOGB_D] =vv,
    [OPC_VFCLASS_S] =vv,
    [OPC_VFCLASS_D] =vv,
    [OPC_VFSQRT_S] =vv,
    [OPC_VFSQRT_D] =vv,
    [OPC_VFRECIP_S] =vv,
    [OPC_VFRECIP_D] =vv,
    [OPC_VFRSQRT_S] =vv,
    [OPC_VFRSQRT_D] =vv,
    [OPC_VFCVTL_S_H] =vv,
    [OPC_VFCVTH_S_H] =vv,
    [OPC_VFCVTL_D_S] =vv,
    [OPC_VFCVTH_D_S] =vv,
    [OPC_VFCVT_H_S] =vvv,
    [OPC_VFCVT_S_D] =vvv,
    [OPC_VFRINT_S] =vv,
    [OPC_VFRINT_D] =vv,
    [OPC_VFRINTRM_S] =vv,
    [OPC_VFRINTRM_D] =vv,
    [OPC_VFRINTRP_S] =vv,
    [OPC_VFRINTRP_D] =vv,
    [OPC_VFRINTRZ_S] =vv,
    [OPC_VFRINTRZ_D] =vv,
    [OPC_VFRINTRNE_S] =vv,
    [OPC_VFRINTRNE_D] =vv,
    [OPC_VFTINT_W_S] =vv,
    [OPC_VFTINT_L_D] =vv,
    [OPC_VFTINTRM_W_S] =vv,
    [OPC_VFTINTRM_L_D] =vv,
    [OPC_VFTINTRP_W_S] =vv,
    [OPC_VFTINTRP_L_D] =vv,
    [OPC_VFTINTRZ_W_S] =vv,
    [OPC_VFTINTRZ_L_D] =vv,
    [OPC_VFTINTRNE_W_S] =vv,
    [OPC_VFTINTRNE_L_D] =vv,
    [OPC_VFTINT_WU_S] =vv,
    [OPC_VFTINT_LU_D] =vv,
    [OPC_VFTINTRZ_WU_S] =vv,
    [OPC_VFTINTRZ_LU_D] =vv,
    [OPC_VFTINT_W_D] =vvv,
    [OPC_VFTINTRM_W_D] =vvv,
    [OPC_VFTINTRP_W_D] =vvv,
    [OPC_VFTINTRZ_W_D] =vvv,
    [OPC_VFTINTRNE_W_D] =vvv,
    [OPC_VFTINTL_L_S] =vv,
    [OPC_VFTINTH_L_S] =vv,
    [OPC_VFTINTRML_L_S] =vv,
    [OPC_VFTINTRMH_L_S] =vv,
    [OPC_VFTINTRPL_L_S] =vv,
    [OPC_VFTINTRPH_L_S] =vv,
    [OPC_VFTINTRZL_L_S] =vv,
    [OPC_VFTINTRZH_L_S] =vv,
    [OPC_VFTINTRNEL_L_S] =vv,
    [OPC_VFTINTRNEH_L_S] =vv,
    [OPC_VFFINT_S_W] =vv,
    [OPC_VFFINT_S_WU] =vv,
    [OPC_VFFINT_D_L] =vv,
    [OPC_VFFINT_D_LU] =vv,
    [OPC_VFFINTL_D_W] =vv,
    [OPC_VFFINTH_D_W] =vv,
    [OPC_VFFINT_S_L] =vvv,
    [OPC_VSEQ_B] =vvv,
    [OPC_VSEQ_H] =vvv,
    [OPC_VSEQ_W] =vvv,
    [OPC_VSEQ_D] =vvv,
    [OPC_VSEQI_B] =vv_i5,
    [OPC_VSEQI_H] =vv_i5,
    [OPC_VSEQI_W] =vv_i5,
    [OPC_VSEQI_D] =vv_i5,
    [OPC_VSLE_B] =vvv,
    [OPC_VSLE_H] =vvv,
    [OPC_VSLE_W] =vvv,
    [OPC_VSLE_D] =vvv,
    [OPC_VSLEI_B] =vv_i5,
    [OPC_VSLEI_H] =vv_i5,
    [OPC_VSLEI_W] =vv_i5,
    [OPC_VSLEI_D] =vv_i5,
    [OPC_VSLE_BU] =vvv,
    [OPC_VSLE_HU] =vvv,
    [OPC_VSLE_WU] =vvv,
    [OPC_VSLE_DU] =vvv,
    [OPC_VSLEI_BU] =vv_ui5,
    [OPC_VSLEI_HU] =vv_ui5,
    [OPC_VSLEI_WU] =vv_ui5,
    [OPC_VSLEI_DU] =vv_ui5,
    [OPC_VSLT_B] =vvv,
    [OPC_VSLT_H] =vvv,
    [OPC_VSLT_W] =vvv,
    [OPC_VSLT_D] =vvv,
    [OPC_VSLTI_B] =vv_i5,
    [OPC_VSLTI_H] =vv_i5,
    [OPC_VSLTI_W] =vv_i5,
    [OPC_VSLTI_D] =vv_i5,
    [OPC_VSLT_BU] =vvv,
    [OPC_VSLT_HU] =vvv,
    [OPC_VSLT_WU] =vvv,
    [OPC_VSLT_DU] =vvv,
    [OPC_VSLTI_BU] =vv_ui5,
    [OPC_VSLTI_HU] =vv_ui5,
    [OPC_VSLTI_WU] =vv_ui5,
    [OPC_VSLTI_DU] =vv_ui5,
    [OPC_VFCMP_COND_S] =vvv_fcond,
    [OPC_VFCMP_COND_D] =vvv_fcond,
    [OPC_VBITSEL_V] =vvvv,
    [OPC_VBITSELI_B] =vv_ui8,
    [OPC_VSETEQZ_V] =cv,
    [OPC_VSETNEZ_V] =cv,
    [OPC_VSETANYEQZ_B] =cv,
    [OPC_VSETANYEQZ_H] =cv,
    [OPC_VSETANYEQZ_W] =cv,
    [OPC_VSETANYEQZ_D] =cv,
    [OPC_VSETALLNEZ_B] =cv,
    [OPC_VSETALLNEZ_H] =cv,
    [OPC_VSETALLNEZ_W] =cv,
    [OPC_VSETALLNEZ_D] =cv,
    [OPC_VINSGR2VR_B] =vr_ui4,
    [OPC_VINSGR2VR_H] =vr_ui3,
    [OPC_VINSGR2VR_W] =vr_ui2,
    [OPC_VINSGR2VR_D] =vr_ui1,
    [OPC_VPICKVE2GR_B] =rv_ui4,
    [OPC_VPICKVE2GR_H] =rv_ui3,
    [OPC_VPICKVE2GR_W] =rv_ui2,
    [OPC_VPICKVE2GR_D] =rv_ui1,
    [OPC_VPICKVE2GR_BU] =rv_ui4,
    [OPC_VPICKVE2GR_HU] =rv_ui3,
    [OPC_VPICKVE2GR_WU] =rv_ui2,
    [OPC_VPICKVE2GR_DU] =rv_ui1,
    [OPC_VREPLGR2VR_B] =vr,
    [OPC_VREPLGR2VR_H] =vr,
    [OPC_VREPLGR2VR_W] =vr,
    [OPC_VREPLGR2VR_D] =vr,
    [OPC_VREPLVE_B] =vvr,
    [OPC_VREPLVE_H] =vvr,
    [OPC_VREPLVE_W] =vvr,
    [OPC_VREPLVE_D] =vvr,
    [OPC_VREPLVEI_B] =vv_ui4,
    [OPC_VREPLVEI_H] =vv_ui3,
    [OPC_VREPLVEI_W] =vv_ui2,
    [OPC_VREPLVEI_D] =vv_ui1,
    [OPC_VBSLL_V] =vv_ui5,
    [OPC_VBSRL_V] =vv_ui5,
    [OPC_VPACKEV_B] =vvv,
    [OPC_VPACKEV_H] =vvv,
    [OPC_VPACKEV_W] =vvv,
    [OPC_VPACKEV_D] =vvv,
    [OPC_VPACKOD_B] =vvv,
    [OPC_VPACKOD_H] =vvv,
    [OPC_VPACKOD_W] =vvv,
    [OPC_VPACKOD_D] =vvv,
    [OPC_VPICKEV_B] =vvv,
    [OPC_VPICKEV_H] =vvv,
    [OPC_VPICKEV_W] =vvv,
    [OPC_VPICKEV_D] =vvv,
    [OPC_VPICKOD_B] =vvv,
    [OPC_VPICKOD_H] =vvv,
    [OPC_VPICKOD_W] =vvv,
    [OPC_VPICKOD_D] =vvv,
    [OPC_VILVL_B] =vvv,
    [OPC_VILVL_H] =vvv,
    [OPC_VILVL_W] =vvv,
    [OPC_VILVL_D] =vvv,
    [OPC_VILVH_B] =vvv,
    [OPC_VILVH_H] =vvv,
    [OPC_VILVH_W] =vvv,
    [OPC_VILVH_D] =vvv,
    [OPC_VSHUF_B] =vvvv,
    [OPC_VSHUF_H] =vvv,
    [OPC_VSHUF_W] =vvv,
    [OPC_VSHUF_D] =vvv,
    [OPC_VSHUF4I_B] =vv_ui8,
    [OPC_VSHUF4I_H] =vv_ui8,
    [OPC_VSHUF4I_W] =vv_ui8,
    [OPC_VSHUF4I_D] =vv_ui8,
    [OPC_VPERMI_W] =vv_ui8,
    [OPC_VEXTRINS_D] =vv_ui8,
    [OPC_VEXTRINS_W] =vv_ui8,
    [OPC_VEXTRINS_H] =vv_ui8,
    [OPC_VEXTRINS_B] =vv_ui8,
    [OPC_VLD] =vr_i12,
    [OPC_VST] =vr_i12,
    [OPC_VLDX] =vrr,
    [OPC_VSTX] =vrr,
    [OPC_VLDREPL_D] =vr_i9,
    [OPC_VLDREPL_W] =vr_i10,
    [OPC_VLDREPL_H] =vr_i11,
    [OPC_VLDREPL_B] =vr_i12,
    [OPC_VSTELM_D] =vr_i8i1,
    [OPC_VSTELM_W] =vr_i8i2,
    [OPC_VSTELM_H] =vr_i8i3,
    [OPC_VSTELM_B] =vr_i8i4,
    [OPC_XVLD] =xr_i12,
    [OPC_XVST] =xr_i12,
};
void (*encodefuncs[])(LoongArchInsInfo* , uint32_t*) = {
    [format_0] = encode_encode_format_0,
    [format_1] = encode_encode_format_1,
    [format_2] = encode_encode_format_2,
    [format_3] = encode_encode_format_3,
    [format_4] = encode_encode_format_4,
    [format_5] = encode_encode_format_5,
    [format_6] = encode_encode_format_6,
    [format_7] = encode_encode_format_7,
    [format_8] = encode_encode_format_8,
    [format_9] = encode_encode_format_9,
    [format_10] = encode_encode_format_10,
    [format_11] = encode_encode_format_11,
    [format_12] = encode_encode_format_12,
    [format_13] = encode_encode_format_13,
    [format_14] = encode_encode_format_14,
    [format_15] = encode_encode_format_15,
    [format_16] = encode_encode_format_16,
    [format_17] = encode_encode_format_17,
    [format_18] = encode_encode_format_18,
    [format_19] = encode_encode_format_19,
    [format_20] = encode_encode_format_20,
    [format_21] = encode_encode_format_21,
    [format_22] = encode_encode_format_22,
    [format_23] = encode_encode_format_23,
    [format_24] = encode_encode_format_24,
    [format_25] = encode_encode_format_25,
    [format_26] = encode_encode_format_26,
    [format_27] = encode_encode_format_27,
    [format_28] = encode_encode_format_28,
    [format_29] = encode_encode_format_29,
    [format_30] = encode_encode_format_30,
    [format_31] = encode_encode_format_31,
    [format_32] = encode_encode_format_32,
    [format_33] = encode_encode_format_33,
    [format_34] = encode_encode_format_34,
    [format_35] = encode_encode_format_35,
    [format_36] = encode_encode_format_36,
    [format_37] = encode_encode_format_37,
    [format_38] = encode_encode_format_38,
    [format_39] = encode_encode_format_39,
    [format_40] = encode_encode_format_40,
    [format_41] = encode_encode_format_41,
    [format_42] = encode_encode_format_42,
    [format_43] = encode_encode_format_43,
    [format_44] = encode_encode_format_44,
    [format_45] = encode_encode_format_45,
    [format_46] = encode_encode_format_46,
    [format_47] = encode_encode_format_47,
    [format_48] = encode_encode_format_48,
    [format_49] = encode_encode_format_49,
    [format_50] = encode_encode_format_50,
    [format_51] = encode_encode_format_51,
    [format_52] = encode_encode_format_52,
    [format_53] = encode_encode_format_53,
    [format_54] = encode_encode_format_54,
    [format_55] = encode_encode_format_55,
    [format_56] = encode_encode_format_56,
    [format_57] = encode_encode_format_57,
    [format_58] = encode_encode_format_58,
    [format_59] = encode_encode_format_59,
    [format_60] = encode_encode_format_60,
    [format_61] = encode_encode_format_61,
    [format_62] = encode_encode_format_62,
    [format_63] = encode_encode_format_63,
    [format_64] = encode_encode_format_64,
    [vvv] = encode_encode_vvv,
    [vvr] = encode_encode_vvr,
    [vrr] = encode_encode_vrr,
    [vv_ui1] = encode_encode_vv_ui1,
    [vv_ui2] = encode_encode_vv_ui2,
    [vv_ui3] = encode_encode_vv_ui3,
    [vv_ui4] = encode_encode_vv_ui4,
    [vv_ui5] = encode_encode_vv_ui5,
    [vv_ui6] = encode_encode_vv_ui6,
    [vv_ui7] = encode_encode_vv_ui7,
    [vv_ui8] = encode_encode_vv_ui8,
    [vv_i5] = encode_encode_vv_i5,
    [vv] = encode_encode_vv,
    [vvvv] = encode_encode_vvvv,
    [vvv_fcond] = encode_encode_vvv_fcond,
    [cv] = encode_encode_cv,
    [vr_i9] = encode_encode_vr_i9,
    [vr_i10] = encode_encode_vr_i10,
    [vr_i11] = encode_encode_vr_i11,
    [vr_i12] = encode_encode_vr_i12,
    [vr_ui4] = encode_encode_vr_ui4,
    [vr_ui3] = encode_encode_vr_ui3,
    [vr_ui2] = encode_encode_vr_ui2,
    [vr_ui1] = encode_encode_vr_ui1,
    [rv_ui4] = encode_encode_rv_ui4,
    [rv_ui3] = encode_encode_rv_ui3,
    [rv_ui2] = encode_encode_rv_ui2,
    [rv_ui1] = encode_encode_rv_ui1,
    [vr] = encode_encode_vr,
    [vr_i8i1] = encode_encode_vr_i8i1,
    [vr_i8i2] = encode_encode_vr_i8i2,
    [vr_i8i3] = encode_encode_vr_i8i3,
    [vr_i8i4] = encode_encode_vr_i8i4,
    [v_i13] = encode_encode_v_i13,
    [xr_i12] = encode_encode_xr_i12,
};
void (*gen_require_func_table[]) (LoongArchInsInfo*, reg_info*) = {
[cv] = cv_gen_require,
[format_0] = format_0_gen_require,
[format_1] = format_1_gen_require,
[format_10] = format_10_gen_require,
[format_11] = format_11_gen_require,
[format_12] = format_12_gen_require,
[format_13] = format_13_gen_require,
[format_14] = format_14_gen_require,
[format_15] = format_15_gen_require,
[format_16] = format_16_gen_require,
[format_17] = format_17_gen_require,
[format_18] = format_18_gen_require,
[format_19] = format_19_gen_require,
[format_2] = format_2_gen_require,
[format_20] = format_20_gen_require,
[format_21] = format_21_gen_require,
[format_22] = format_22_gen_require,
[format_23] = format_23_gen_require,
[format_24] = format_24_gen_require,
[format_25] = format_25_gen_require,
[format_26] = format_26_gen_require,
[format_27] = format_27_gen_require,
[format_28] = format_28_gen_require,
[format_29] = format_29_gen_require,
[format_3] = format_3_gen_require,
[format_30] = format_30_gen_require,
[format_31] = format_31_gen_require,
[format_32] = format_32_gen_require,
[format_33] = format_33_gen_require,
[format_34] = format_34_gen_require,
[format_35] = format_35_gen_require,
[format_36] = format_36_gen_require,
[format_37] = format_37_gen_require,
[format_38] = format_38_gen_require,
[format_39] = format_39_gen_require,
[format_4] = format_4_gen_require,
[format_40] = format_40_gen_require,
[format_41] = format_41_gen_require,
[format_42] = format_42_gen_require,
[format_43] = format_43_gen_require,
[format_44] = format_44_gen_require,
[format_45] = format_45_gen_require,
[format_46] = format_46_gen_require,
[format_47] = format_47_gen_require,
[format_48] = format_48_gen_require,
[format_49] = format_49_gen_require,
[format_5] = format_5_gen_require,
[format_50] = format_50_gen_require,
[format_51] = format_51_gen_require,
[format_52] = format_52_gen_require,
[format_53] = format_53_gen_require,
[format_54] = format_54_gen_require,
[format_55] = format_55_gen_require,
[format_56] = format_56_gen_require,
[format_57] = format_57_gen_require,
[format_58] = format_58_gen_require,
[format_59] = format_59_gen_require,
[format_6] = format_6_gen_require,
[format_60] = format_60_gen_require,
[format_61] = format_61_gen_require,
[format_62] = format_62_gen_require,
[format_63] = format_63_gen_require,
[format_64] = format_64_gen_require,
[format_7] = format_7_gen_require,
[format_8] = format_8_gen_require,
[format_9] = format_9_gen_require,
[rv_ui1] = rv_ui1_gen_require,
[rv_ui2] = rv_ui2_gen_require,
[rv_ui3] = rv_ui3_gen_require,
[rv_ui4] = rv_ui4_gen_require,
[v_i13] = v_i13_gen_require,
[vr] = vr_gen_require,
[vr_i10] = vr_i10_gen_require,
[vr_i11] = vr_i11_gen_require,
[vr_i12] = vr_i12_gen_require,
[vr_i8i1] = vr_i8i1_gen_require,
[vr_i8i2] = vr_i8i2_gen_require,
[vr_i8i3] = vr_i8i3_gen_require,
[vr_i8i4] = vr_i8i4_gen_require,
[vr_i9] = vr_i9_gen_require,
[vr_ui1] = vr_ui1_gen_require,
[vr_ui2] = vr_ui2_gen_require,
[vr_ui3] = vr_ui3_gen_require,
[vr_ui4] = vr_ui4_gen_require,
[vrr] = vrr_gen_require,
[vv] = vv_gen_require,
[vv_i5] = vv_i5_gen_require,
[vv_ui1] = vv_ui1_gen_require,
[vv_ui2] = vv_ui2_gen_require,
[vv_ui3] = vv_ui3_gen_require,
[vv_ui4] = vv_ui4_gen_require,
[vv_ui5] = vv_ui5_gen_require,
[vv_ui6] = vv_ui6_gen_require,
[vv_ui7] = vv_ui7_gen_require,
[vv_ui8] = vv_ui8_gen_require,
[vvr] = vvr_gen_require,
[vvv] = vvv_gen_require,
[vvv_fcond] = vvv_fcond_gen_require,
[vvvv] = vvvv_gen_require,
[xr_i12] = xr_i12_gen_require,
};
void (*print_func_table[]) (std::ostream&, LoongArchInsInfo*) = {
    [cv] = print_cv,
    [format_0] = print_format_0,
    [format_1] = print_format_1,
    [format_10] = print_format_10,
    [format_11] = print_format_11,
    [format_12] = print_format_12,
    [format_13] = print_format_13,
    [format_14] = print_format_14,
    [format_15] = print_format_15,
    [format_16] = print_format_16,
    [format_17] = print_format_17,
    [format_18] = print_format_18,
    [format_19] = print_format_19,
    [format_2] = print_format_2,
    [format_20] = print_format_20,
    [format_21] = print_format_21,
    [format_22] = print_format_22,
    [format_23] = print_format_23,
    [format_24] = print_format_24,
    [format_25] = print_format_25,
    [format_26] = print_format_26,
    [format_27] = print_format_27,
    [format_28] = print_format_28,
    [format_29] = print_format_29,
    [format_3] = print_format_3,
    [format_30] = print_format_30,
    [format_31] = print_format_31,
    [format_32] = print_format_32,
    [format_33] = print_format_33,
    [format_34] = print_format_34,
    [format_35] = print_format_35,
    [format_36] = print_format_36,
    [format_37] = print_format_37,
    [format_38] = print_format_38,
    [format_39] = print_format_39,
    [format_4] = print_format_4,
    [format_40] = print_format_40,
    [format_41] = print_format_41,
    [format_42] = print_format_42,
    [format_43] = print_format_43,
    [format_44] = print_format_44,
    [format_45] = print_format_45,
    [format_46] = print_format_46,
    [format_47] = print_format_47,
    [format_48] = print_format_48,
    [format_49] = print_format_49,
    [format_5] = print_format_5,
    [format_50] = print_format_50,
    [format_51] = print_format_51,
    [format_52] = print_format_52,
    [format_53] = print_format_53,
    [format_54] = print_format_54,
    [format_55] = print_format_55,
    [format_56] = print_format_56,
    [format_57] = print_format_57,
    [format_58] = print_format_58,
    [format_59] = print_format_59,
    [format_6] = print_format_6,
    [format_60] = print_format_60,
    [format_61] = print_format_61,
    [format_62] = print_format_62,
    [format_63] = print_format_63,
    [format_64] = print_format_64,
    [format_7] = print_format_7,
    [format_8] = print_format_8,
    [format_9] = print_format_9,
    [rv_ui1] = print_rv_ui1,
    [rv_ui2] = print_rv_ui2,
    [rv_ui3] = print_rv_ui3,
    [rv_ui4] = print_rv_ui4,
    [v_i13] = print_v_i13,
    [vr] = print_vr,
    [vr_i10] = print_vr_i10,
    [vr_i11] = print_vr_i11,
    [vr_i12] = print_vr_i12,
    [vr_i8i1] = print_vr_i8i1,
    [vr_i8i2] = print_vr_i8i2,
    [vr_i8i3] = print_vr_i8i3,
    [vr_i8i4] = print_vr_i8i4,
    [vr_i9] = print_vr_i9,
    [vr_ui1] = print_vr_ui1,
    [vr_ui2] = print_vr_ui2,
    [vr_ui3] = print_vr_ui3,
    [vr_ui4] = print_vr_ui4,
    [vrr] = print_vrr,
    [vv] = print_vv,
    [vv_i5] = print_vv_i5,
    [vv_ui1] = print_vv_ui1,
    [vv_ui2] = print_vv_ui2,
    [vv_ui3] = print_vv_ui3,
    [vv_ui4] = print_vv_ui4,
    [vv_ui5] = print_vv_ui5,
    [vv_ui6] = print_vv_ui6,
    [vv_ui7] = print_vv_ui7,
    [vv_ui8] = print_vv_ui8,
    [vvr] = print_vvr,
    [vvv] = print_vvv,
    [vvv_fcond] = print_vvv_fcond,
    [vvvv] = print_vvvv,
    [xr_i12] = print_xr_i12,
};
void encode_encode_cv(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_cfreg,0,3);
}

void encode_encode_format_0(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_1(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->srcs_regs[1],10,5);
}

void encode_encode_format_10(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_fregs[0],5,5);
encode32(pinsn, info->srcs_fregs[1],10,5);
}

void encode_encode_format_11(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_fregs[0],5,5);
}

void encode_encode_format_12(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_13(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_fregs[0],5,5);
}

void encode_encode_format_14(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_fcsr,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_15(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_fcsrregs[0],5,5);
}

void encode_encode_format_16(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_cfreg,0,3);
encode32(pinsn, info->srcs_fregs[0],5,5);
}

void encode_encode_format_17(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_cfregs[0],5,3);
}

void encode_encode_format_18(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_cfreg,0,3);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_19(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_cfregs[0],5,3);
}

void encode_encode_format_2(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->srcs_regs[1],10,5);
encode32(pinsn, info->sa,15,2);
}

void encode_encode_format_20(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->si,10,12);
}

void encode_encode_format_21(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->ui,10,12);
}

void encode_encode_format_22(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->csr,10,14);
}

void encode_encode_format_23(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->csr,10,14);
}

void encode_encode_format_24(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->code_2,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->si,10,12);
}

void encode_encode_format_25(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->level,10,8);
}

void encode_encode_format_26(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->seq,10,8);
}

void encode_encode_format_27(LoongArchInsInfo *info, uint32_t* pinsn)
{
}

void encode_encode_format_28(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->level_3,0,15);
}

void encode_encode_format_29(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->op,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->srcs_regs[1],10,5);
}

void encode_encode_format_3(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->srcs_regs[1],10,5);
encode32(pinsn, info->sa,15,3);
}

void encode_encode_format_30(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_fregs[0],5,5);
encode32(pinsn, info->srcs_fregs[1],10,5);
encode32(pinsn, info->srcs_fregs[2],15,5);
}

void encode_encode_format_31(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_cfreg,0,3);
encode32(pinsn, info->srcs_fregs[0],5,5);
encode32(pinsn, info->srcs_fregs[1],10,5);
}

void encode_encode_format_32(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_fregs[0],5,5);
encode32(pinsn, info->srcs_fregs[1],10,5);
encode32(pinsn, info->srcs_cfregs[0],15,3);
}

void encode_encode_format_33(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->si,10,16);
}

void encode_encode_format_34(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->si,5,20);
}

void encode_encode_format_35(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->si,10,14);
}

void encode_encode_format_36(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->hint,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->si,10,12);
}

void encode_encode_format_37(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->si,10,12);
}

void encode_encode_format_38(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->hint,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->srcs_regs[1],10,5);
}

void encode_encode_format_39(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_freg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->srcs_regs[1],10,5);
}

void encode_encode_format_4(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->srcs_regs[1],10,5);
}

void encode_encode_format_40(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->hint,0,15);
}

void encode_encode_format_41(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->hint,0,15);
}

void encode_encode_format_42(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
u_int8_t start_and_len[4];
start_and_len[0]=10;
start_and_len[1]=16;
start_and_len[2]=0;
start_and_len[3]=5;
encode32_muti_field(pinsn, info->offs, start_and_len, 4);
}

void encode_encode_format_43(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_cfregs[0],5,3);
u_int8_t start_and_len[4];
start_and_len[0]=10;
start_and_len[1]=16;
start_and_len[2]=0;
start_and_len[3]=5;
encode32_muti_field(pinsn, info->offs, start_and_len, 4);
}

void encode_encode_format_44(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->offs,10,16);
}

void encode_encode_format_45(LoongArchInsInfo *info, uint32_t* pinsn)
{
u_int8_t start_and_len[4];
start_and_len[0]=10;
start_and_len[1]=16;
start_and_len[2]=0;
start_and_len[3]=10;
encode32_muti_field(pinsn, info->offs, start_and_len, 4);
}

void encode_encode_format_46(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_sccrach,0,2);
}

void encode_encode_format_47(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->sccrach_regs[0],5,2);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_48(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ptr,5,3);
}

void encode_encode_format_49(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_5(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->code,0,15);
}

void encode_encode_format_50(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_51(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->si,10,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_52(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->cond,10,4);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_53(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->cond,10,4);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_54(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],10,5);
encode32(pinsn, info->srcs_regs[1],5,5);
encode32(pinsn, info->cond,0,4);
}

void encode_encode_format_55(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->cond,10,4);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_56(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,3);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_57(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,4);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_58(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,3);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_59(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,4);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_6(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->ui,10,5);
}

void encode_encode_format_60(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,5);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_61(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,6);
encode32(pinsn, info->srcs_regs[0],5,5);
}

void encode_encode_format_62(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,8);
encode32(pinsn, info->op,5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_63(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,8);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_format_64(LoongArchInsInfo *info, uint32_t* pinsn)
{
u_int8_t start_and_len[4];
start_and_len[0]=10;
start_and_len[1]=16;
start_and_len[2]=0;
start_and_len[3]=5;
encode32_muti_field(pinsn, info->offs, start_and_len, 4);
}

void encode_encode_format_7(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->ui,10,6);
}

void encode_encode_format_8(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->msb,16,5);
encode32(pinsn, info->lsb,10,5);
}

void encode_encode_format_9(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->dst_reg,0,5);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->msb,16,6);
encode32(pinsn, info->lsb,10,6);
}

void encode_encode_rv_ui1(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,1);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_rv_ui2(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,2);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_rv_ui3(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,3);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_rv_ui4(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,4);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_reg,0,5);
}

void encode_encode_v_i13(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,5,13);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vr(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vr_i10(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
encode32(pinsn, info->si,10,10);
}

void encode_encode_vr_i11(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
encode32(pinsn, info->si,10,11);
}

void encode_encode_vr_i12(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->si,10,12);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vr_i8i1(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->_ui,18,1);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
encode32(pinsn, info->si,10,8);
}

void encode_encode_vr_i8i2(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->_ui,18,2);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
encode32(pinsn, info->si,10,8);
}

void encode_encode_vr_i8i3(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->_ui,18,3);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
encode32(pinsn, info->si,10,8);
}

void encode_encode_vr_i8i4(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->_ui,18,4);
encode32(pinsn, info->si,10,8);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vr_i9(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
encode32(pinsn, info->si,10,9);
}

void encode_encode_vr_ui1(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,1);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vr_ui2(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,2);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vr_ui3(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,3);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vr_ui4(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,4);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vrr(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],10,5);
encode32(pinsn, info->srcs_regs[1],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_i5(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->si,10,5);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui1(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,1);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui2(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,2);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui3(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,3);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui4(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,4);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui5(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,5);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui6(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,6);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui7(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,7);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vv_ui8(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->ui,10,8);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vvr(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->srcs_regs[0],10,5);
encode32(pinsn, info->lsx_src[0],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vvv(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->lsx_src[0],10,5);
encode32(pinsn, info->lsx_src[1],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vvv_fcond(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->fcond,15,5);
encode32(pinsn, info->lsx_src[0],10,5);
encode32(pinsn, info->lsx_src[1],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_vvvv(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->lsx_src[0],15,5);
encode32(pinsn, info->lsx_src[1],10,5);
encode32(pinsn, info->lsx_src[2],5,5);
encode32(pinsn, info->dst_lsx_reg,0,5);
}

void encode_encode_xr_i12(LoongArchInsInfo *info, uint32_t* pinsn)
{
encode32(pinsn, info->si,10,12);
encode32(pinsn, info->srcs_regs[0],5,5);
encode32(pinsn, info->dst_lasx_reg,0,5);
}

void print_cv(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<cc_name[info->dst_cfreg];
out<<" ";
out<<std::endl;
}

void print_format_0(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_1(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_10(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<fpr_name[info->srcs_fregs[0]] <<" ";
out<<" ";
out<<fpr_name[info->srcs_fregs[1]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_11(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<fpr_name[info->srcs_fregs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_12(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_13(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<fpr_name[info->srcs_fregs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_14(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fcsr_name[info->dst_fcsr];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_15(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<fcsr_name[info->srcs_fcsrregs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_16(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<cc_name[info->dst_cfreg];
out<<" ";
out<<fpr_name[info->srcs_fregs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_17(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<cc_name[info->srcs_cfregs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_18(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<cc_name[info->dst_cfreg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_19(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<cc_name[info->srcs_cfregs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_2(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::dec<<info->sa;
out<<" ";
out<<std::endl;
}

void print_format_20(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_format_21(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<std::endl;
}

void print_format_22(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::dec<<info->csr;
out<<" ";
out<<std::endl;
}

void print_format_23(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->csr;
out<<" ";
out<<std::endl;
}

void print_format_24(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->code_2;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_format_25(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->level;
out<<" ";
out<<std::endl;
}

void print_format_26(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->seq;
out<<" ";
out<<std::endl;
}

void print_format_27(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::endl;
}

void print_format_28(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->level_3;
out<<" ";
out<<std::endl;
}

void print_format_29(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->op;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_3(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::dec<<info->sa;
out<<" ";
out<<std::endl;
}

void print_format_30(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<fpr_name[info->srcs_fregs[0]] <<" ";
out<<" ";
out<<fpr_name[info->srcs_fregs[1]] <<" ";
out<<" ";
out<<fpr_name[info->srcs_fregs[2]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_31(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<cc_name[info->dst_cfreg];
out<<" ";
out<<fpr_name[info->srcs_fregs[0]] <<" ";
out<<" ";
out<<fpr_name[info->srcs_fregs[1]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_32(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<fpr_name[info->srcs_fregs[0]] <<" ";
out<<" ";
out<<fpr_name[info->srcs_fregs[1]] <<" ";
out<<" ";
out<<cc_name[info->srcs_cfregs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_33(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_format_34(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_format_35(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_format_36(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->hint;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_format_37(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_format_38(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->hint;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_39(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<fpr_name[info->dst_freg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_4(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_40(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->hint;
out<<" ";
out<<std::endl;
}

void print_format_41(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->hint;
out<<" ";
out<<std::endl;
}

void print_format_42(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->offs;
out<<" ";
out<<std::endl;
}

void print_format_43(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<cc_name[info->srcs_cfregs[0]] <<" ";
out<<" ";
out<<std::dec<<info->offs;
out<<" ";
out<<std::endl;
}

void print_format_44(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->offs;
out<<" ";
out<<std::endl;
}

void print_format_45(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->offs;
out<<" ";
out<<std::endl;
}

void print_format_46(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<scr_name[info->dst_sccrach];
out<<" ";
out<<std::endl;
}

void print_format_47(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<scr_name[info->sccrach_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_48(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ptr;
out<<" ";
out<<std::endl;
}

void print_format_49(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_5(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->code;
out<<" ";
out<<std::endl;
}

void print_format_50(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_51(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_52(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->cond;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_53(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->cond;
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_54(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<std::dec<<info->cond;
out<<" ";
out<<std::endl;
}

void print_format_55(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->cond;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_56(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_57(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_58(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_59(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_6(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<std::endl;
}

void print_format_60(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_61(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::endl;
}

void print_format_62(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<std::dec<<info->op;
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_63(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_format_64(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->offs;
out<<" ";
out<<std::endl;
}

void print_format_7(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<std::endl;
}

void print_format_8(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->msb;
out<<" ";
out<<std::dec<<info->lsb;
out<<" ";
out<<std::endl;
}

void print_format_9(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<std::dec<<info->msb;
out<<" ";
out<<std::dec<<info->lsb;
out<<" ";
out<<std::endl;
}

void print_rv_ui1(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_rv_ui2(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_rv_ui3(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_rv_ui4(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<gpr_name[info->dst_reg];
out<<" ";
out<<std::endl;
}

void print_v_i13(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vr(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vr_i10(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_vr_i11(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_vr_i12(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vr_i8i1(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->_ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_vr_i8i2(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->_ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_vr_i8i3(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->_ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_vr_i8i4(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->_ui;
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vr_i9(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<std::endl;
}

void print_vr_ui1(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vr_ui2(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vr_ui3(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vr_ui4(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vrr(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<gpr_name[info->srcs_regs[1]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_i5(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui1(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui2(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui3(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui4(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui5(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui6(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui7(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vv_ui8(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->ui;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vvr(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vvv(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->lsx_src[1]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vvv_fcond(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->fcond;
out<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->lsx_src[1]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_vvvv(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<lsx_name[info->lsx_src[0]] <<" ";
out<<" ";
out<<lsx_name[info->lsx_src[1]] <<" ";
out<<" ";
out<<lsx_name[info->lsx_src[2]] <<" ";
out<<" ";
out<<lsx_name[info->dst_lsx_reg];
out<<" ";
out<<std::endl;
}

void print_xr_i12(std::ostream& out, LoongArchInsInfo *info)
{
out<<opcode_name[info->opc]<<" ";
out<<std::dec<<info->si;
out<<" ";
out<<gpr_name[info->srcs_regs[0]] <<" ";
out<<" ";
out<<lasx_name[info->dst_lasx_reg];
out<<" ";
out<<std::endl;
}

void cv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_0_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_10_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_11_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_12_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_13_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_14_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_15_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_16_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_17_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_18_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_19_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_20_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_21_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_22_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_23_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_24_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_25_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_26_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_27_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_28_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_29_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_30_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_31_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_32_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_33_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_34_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_35_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_36_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_37_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_38_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_39_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_40_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_41_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_42_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_43_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_44_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_45_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_46_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_47_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_48_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_49_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_5_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_50_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_51_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->kill = insn->dst_reg;

}
void format_52_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->kill = insn->dst_reg;

}
void format_53_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_54_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void format_55_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_56_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->kill = insn->dst_reg;

}
void format_57_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->kill = insn->dst_reg;

}
void format_58_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_59_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_6_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_60_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_61_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_62_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_63_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void format_64_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void format_7_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_8_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void format_9_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void rv_ui1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void rv_ui2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void rv_ui3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void rv_ui4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->kill = insn->dst_reg;

}
void v_i13_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vr_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i10_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i11_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i12_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i8i1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i8i2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i8i3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i8i4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_i9_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_ui1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_ui2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_ui3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vr_ui4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vrr_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[1];

}
void vv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_i5_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui1_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui2_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui3_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui4_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui5_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui6_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui7_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vv_ui8_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vvr_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}
void vvv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vvv_fcond_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void vvvv_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{

}
void xr_i12_gen_require(LoongArchInsInfo* insn, reg_info* gen_and_require)
{
gen_and_require->require_set[gen_and_require->gr_count++] = insn->srcs_regs[0];

}

    uint32_t encode(LoongArchInsInfo* info)
    {
        if(info->opc == OPC_INVALID)
                return info->origin_binary;
        uint32_t insn = opc2bin[info->opc];
        uint format = insn2format[info->opc];
        encodefuncs[format](info, &insn);
        return insn;
    }
 
