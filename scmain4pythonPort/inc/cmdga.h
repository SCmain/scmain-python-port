/***************************************************************\
 *
 * Program:     low-level command, command interpreter and Opcode table.
 *
 * File:        cmdga.h
 *
 * Functions:
 *
 * Description: Functions which act as an interface for the LowLevel
 *              commands. These functions are local to the low-level command module.
 *
 * Modification history:
 * Rev      ECO#    Date    Author          Brief Description
 *
\***************************************************************/

#ifndef _H_CMDGA_H
#define _H_CMDGA_H

int ex_ABM(instr_ptr instr);
int ex_AMPI( instr_ptr instr );
int ex_HOME(instr_ptr instr);
int ex_FE(instr_ptr instr);
int ex_MATS(instr_ptr instr);
int ex_MOVA(instr_ptr instr);
int ex_MOVR(instr_ptr instr);
int ex_MTCR(instr_ptr instr);
int ex_MTCS(instr_ptr instr);
int ex_MTPS(instr_ptr instr);
int ex_RETH(instr_ptr instr);
int ex_STROK(instr_ptr instr);
int ex_STOP(instr_ptr instr);
int ex_SVOF(instr_ptr instr);
int ex_SVON(instr_ptr instr);
int ex_WMO(instr_ptr instr);
int ex_MIP(instr_ptr instr);
int ex_MHS(instr_ptr instr);

// new commands
int ex_XQFN(instr_ptr instr);
int ex_XQGT(instr_ptr instr);

int ex_GMAC(instr_ptr instr);
int ex_XRC(instr_ptr instr);
int ex_XQU(instr_ptr instr);
int ex_XDM(instr_ptr instr);
int ex_SEDF(instr_ptr instr);
int ex_REDF(instr_ptr instr);
int ex_WRCT(instr_ptr instr);
int ex_LDCT(instr_ptr instr);
int ex_WRPD(instr_ptr instr);
int ex_LDPD(instr_ptr instr);
int ex_WXRC(instr_ptr instr);
int ex_RXRC(instr_ptr instr);
int ex_RARM(instr_ptr instr);
int ex_XCLR(instr_ptr instr);
int ex_XWCI(instr_ptr instr);
int ex_XRWC(instr_ptr instr);
int ex_XSWC(instr_ptr instr);
int ex_XITM(instr_ptr instr);
int ex_XRTM(instr_ptr instr);
int ex_XSMX(instr_ptr instr);
int ex_XRMX(instr_ptr instr);
int ex_XRFA(instr_ptr instr);
int ex_XSFT(instr_ptr instr);
int ex_XENL(instr_ptr instr);
int ex_XREL(instr_ptr instr);
int ex_XRMR(instr_ptr instr);
int ex_XMPA(instr_ptr instr);
int ex_XFDC(instr_ptr instr);
int ex_XMVC(instr_ptr instr);
int ex_XSEE(instr_ptr instr);
int ex_XREE(instr_ptr instr);
int ex_XSVS(instr_ptr instr);
int ex_XRVS(instr_ptr instr);
int ex_XSVA(instr_ptr instr);
int ex_XRVA(instr_ptr instr);
int ex_XSVD(instr_ptr instr);
int ex_XRVD(instr_ptr instr);
int ex_XSAV(instr_ptr instr);
int ex_XSRF(instr_ptr instr);
int ex_XRRF(instr_ptr instr);
int ex_XR2W(instr_ptr instr);
int ex_XW2R(instr_ptr instr);
int ex_XW2E(instr_ptr instr);
int ex_XE2W(instr_ptr instr);
int ex_XMCD(instr_ptr instr);
#endif
