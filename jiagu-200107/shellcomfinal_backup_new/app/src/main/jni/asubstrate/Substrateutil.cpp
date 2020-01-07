/*
 * Substrateutil.cpp

 *
 *  Created on: 2016Äê8ÔÂ12ÈÕ
 *      Author: frankozhang
 */

#include "./include/Substrateutil.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
//#include "AndroidProtectCommonall.h"
#include <android/log.h>
#define T$MOV_R2_0 (0x2200)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , "ccc" , __VA_ARGS__)
static inline bool T$pcrel$b_w_1(uint16_t ic) {
    return ((ic & 0xf800) == 0xf000);
}

static inline bool T$pcrel$b_w_2(uint16_t ic) {
    return ((ic & 0xC000) == 0x8000) && ((ic & 0x1000) == 0x1000);
}

/*uint32_t getAddr(uint16_t *area, uint16_t thumb1, uint16_t thumb2){
	LOGI("find address\n");
	union {
		uint16_t value;
		struct {
			uint16_t imm10 : 10;
			uint16_t s : 1;
			uint16_t : 5;
		};
	} t4_1 = {thumb1};

	union {
		uint16_t value;
		struct {
			uint16_t imm11 : 11;
			uint16_t j2 : 1;
			uint16_t a : 1;
			uint16_t j1 : 1;
			uint16_t : 2;
		};
	} t4_2 = {thumb2};

	intptr_t jump = 0;
	LOGI("jump 0:%08x\n", jump);
	jump |= (t4_1.s << 24);
	LOGI("jump 01:%08x\n", jump);
	LOGI("~(t4_1.s ^ t4_2.j1):%d\n", ~(t4_1.s ^ t4_2.j1));
	jump |= ((~(t4_1.s ^ t4_2.j1)) << 23);
	LOGI("jump 2:%08x\n", jump);
	LOGI("~(t4_1.s ^ t4_2.j2):%d\n", ~(t4_1.s ^ t4_2.j2));
	jump |= ((~(t4_1.s ^ t4_2.j2)) << 22);
	LOGI("jump 3:%08x\n", jump);
	jump |= (t4_1.imm10 << 12);
	LOGI("jump 4:%08x\n", jump);
	jump |= (t4_2.imm11 << 1);
	LOGI("jump 5:%08x\n", jump);
	jump <<= 5;
	jump >>= 5;

	LOGI("jump:%08x\n", jump);

	uint32_t dlopen_ext_addr = reinterpret_cast<uint32_t>(area + 1) + 4 + jump;
	LOGI("dlopen_ext address:%08x\n", dlopen_ext_addr);
	dlopen_ext_addr += 1;
	LOGI("dlopen_ext address:%08x\n", dlopen_ext_addr);

	return dlopen_ext_addr;
}*/

uint32_t getAddr(uint16_t *area, uint16_t thumb1, uint16_t thumb2){
	LOGI("find address\n");
	union {
		uint16_t value;
		struct {
			uint16_t imm10 : 10;
			uint16_t s : 1;
			uint16_t : 5;
		};
	} t4_1 = {thumb1};

	union {
		uint16_t value;
		struct {
			uint16_t imm11 : 11;
			uint16_t j2 : 1;
			uint16_t a : 1;
			uint16_t j1 : 1;
			uint16_t : 2;
		};
	} t4_2 = {thumb2};

	intptr_t jump = 0;
	LOGI("jump 0:%08x\n", jump);
	jump |= (t4_1.s << 24);
	LOGI("jump 1:%08x\n", jump);
	LOGI("~(t4_1.s ^ t4_2.j1):%d\n", (~(t4_1.s ^ t4_2.j1))&0x1);
	jump |= (((~(t4_1.s ^ t4_2.j1))&0x1) << 23);
	LOGI("jump 2:%08x\n", jump);
	LOGI("~(t4_1.s ^ t4_2.j2):%d\n", (~(t4_1.s ^ t4_2.j2))&0x1);
	jump |= (((~(t4_1.s ^ t4_2.j2))&0x1) << 22);
	LOGI("jump 3:%08x\n", jump);
	jump |= (t4_1.imm10 << 12);
	LOGI("jump 4:%08x\n", jump);
	jump |= (t4_2.imm11 << 1);
	LOGI("jump 5:%08x\n", jump);
	jump <<= 7;
	jump >>= 7;

	LOGI("jump:%08x\n", jump);

	uint32_t dlopen_ext_addr = reinterpret_cast<uint32_t>(area + 1) + 4 + jump;
	LOGI("dlopen_ext address:%08x\n", dlopen_ext_addr);
	dlopen_ext_addr += 1;
	LOGI("dlopen_ext address:%08x\n", dlopen_ext_addr);

	return dlopen_ext_addr;
}


int dlopen_hook(void* symbol, unsigned int &new_addr){
	new_addr = 0;
	if (symbol == NULL){

		//return false;
		return -1;
	}
#ifdef __arm__

	if ((reinterpret_cast<uintptr_t>(symbol) & 0x1) == 0){
		LOGI("arm and goto hook\n");
		//return true;
		return 1;
	}else{

		uint16_t *area( reinterpret_cast<uint16_t *>(reinterpret_cast<uintptr_t>(symbol) & ~0x1));
		unsigned align((reinterpret_cast<uintptr_t>(area) & 0x2) == 0 ? 0 : 1);
		uint16_t *thumb(area + align);



		if( (thumb[0] == T$MOV_R2_0) && T$pcrel$b_w_1(thumb[1]) && T$pcrel$b_w_2(thumb[2]) ){
			new_addr = getAddr(area, thumb[1], thumb[2]);
			return 2;
		}if( (thumb[0] == T$MOV_R2_0) && T$pcrel$b_w_1(thumb[2]) && T$pcrel$b_w_2(thumb[3]) ){
			new_addr = getAddr(area, thumb[2], thumb[3]);
			return 3;
		}else{
			LOGI("thumb goto hook\n");
			//return true;
			return 1;
		}
	}
#endif
	return 1;
}

/*
bool dlopen_hook(void* symbol, unsigned int &new_addr){
#ifdef __arm__
	if (symbol == NULL){
		new_addr = 0;
		return false;
	}
	if ((reinterpret_cast<uintptr_t>(symbol) & 0x1) == 0){
		//LOGI("arm and goto hook\n");
		return true;
	}else{
		//LOGI("thumb\n");
		uint16_t *area( reinterpret_cast<uint16_t  *>(reinterpret_cast<uintptr_t>(symbol) & ~0x1));
		unsigned align((reinterpret_cast<uintptr_t>(area) & 0x2) == 0 ? 0 : 1);
		uint16_t *thumb(area + align);

		if( (thumb[0] == T$MOV_R2_0) && T$pcrel$b_w_1(thumb[1]) && T$pcrel$b_w_2(thumb[2]) ){
			//LOGI("find address\n");
			union {
				uint16_t value;
				struct {
					uint16_t imm10 : 10;
					uint16_t s : 1;
					uint16_t : 5;
				};
			} t4_1 = {thumb[1]};

			union {
				uint16_t value;
				struct {
					uint16_t imm11 : 11;
					uint16_t j2 : 1;
					uint16_t a : 1;
					uint16_t j1 : 1;
					uint16_t : 2;
				};
			} t4_2 = {thumb[2]};

			intptr_t jump = 0;
			//LOGI("jump 0:%08x\n", jump);
			jump |= (t4_1.s << 24);
			//LOGI("jump 01:%08x\n", jump);
		//	if(t4_1.s == 1){
		//		jump |= (0xFE << 24);
		//		HLOGI("jump 1:%08x\n", jump);
		//	}
			//LOGI("~(t4_1.s ^ t4_2.j1):%d\n", ~(t4_1.s ^ t4_2.j1));
			jump |= ((~(t4_1.s ^ t4_2.j1)) << 23);
			//LOGI("jump 2:%08x\n", jump);
			//LOGI("~(t4_1.s ^ t4_2.j2):%d\n", ~(t4_1.s ^ t4_2.j2));
			jump |= ((~(t4_1.s ^ t4_2.j2)) << 22);
			//LOGI("jump 3:%08x\n", jump);
			jump |= (t4_1.imm10 << 12);
			//LOGI("jump 4:%08x\n", jump);
			jump |= (t4_2.imm11 << 1);
			//LOGI("jump 5:%08x\n", jump);
			jump <<= 5;
			jump >>= 5;

			//LOGI("jump:%08x\n", jump);

			uint32_t dlopen_ext_addr = reinterpret_cast<uint32_t>(area + 1) + 4 + jump;
			//LOGI("dlopen_ext address:%08x\n", dlopen_ext_addr);
			dlopen_ext_addr += 1;
			//LOGI("dlopen_ext address:%08x\n", dlopen_ext_addr);

			new_addr = dlopen_ext_addr;
			return false;
		}else{
			//LOGI("thumb goto hook\n");
			return true;
		}
	}
#endif
	return true;
}
*/

