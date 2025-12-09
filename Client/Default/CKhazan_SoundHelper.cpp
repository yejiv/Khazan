#include "ClientPch.h"
#include "CKhazan_SoundHelper.h"

CKhazan_SoundHelper::CKhazan_SoundHelper()
{
}

HRESULT CKhazan_SoundHelper::Initialize()
{

	AllSoundGroups.reserve(PC_SOUND_GROUPTYPE_END);

	auto EmplaceGroup = [ & ] (PC_SOUND_GROUPTYPE type, const wstring& key, size_t count) {
		SOUND_GROUP group;
		MakeSoundKeyHelper(key, count, group.SoundKeys);
		AllSoundGroups.emplace(type, std::move(group));
		};

	// 여러 개의 키를 하나의 그룹에 넣는 헬퍼 함수 정의
	auto EmplaceGroupMulti = [ & ] (PC_SOUND_GROUPTYPE type, initializer_list<pair<wstring, size_t>> keys) {
		SOUND_GROUP group;
		size_t total_count = 0;
		for ( const auto& p:keys ) { total_count += p.second; }
		group.SoundKeys.reserve(total_count); // 전체 공간 예약

		for ( const auto& p:keys ) {
			MakeSoundKeyHelper(p.first, p.second, group.SoundKeys);
		}
		AllSoundGroups.emplace(type, std::move(group));
		};

	// =================================================================================
	// 1. Idle & Move
	// =================================================================================

	EmplaceGroup(IDLE, TEXT("vo_kz_effort_idle_breath_01 (Korean(KR))"), 4);
	EmplaceGroup(IDLE_RATTLE, TEXT("foley_kz_idle_rattle_01 (SFX)"), 11);

	EmplaceGroup(MOVE_INJURE_R, TEXT("footstep_kz_walk_deepsnow_injure_r_01 (SFX)"), 8);
	EmplaceGroup(MOVE_INJURE_L, TEXT("footstep_kz_walk_deepsnow_injure_l_01 (SFX)"), 7);
	EmplaceGroup(MOVE_WALK, TEXT("khazan_footstep_stone_walk_01 (SFX)"), 23);
	EmplaceGroup(MOVE_RUN, TEXT("khazan_footstep_concrete_run_01 (SFX)"), 35);
	EmplaceGroup(MOVE_SPRINT, TEXT("vo_kz_effort_sprint_01 (Korean(KR))"), 15);
	//EmplaceGroupMulti(MOVE_SPRINT_STOP, { {TEXT("PLYR_FootStep_Sprint_Stop_01 (SFX)"), 3},{TEXT("khazan_sprint_stop_stone_01 (SFX)"), 7} });
	EmplaceGroup(MOVE_SPRINT_STOP, TEXT("khazan_sprint_stop_stone_01 (SFX)"), 7);
	EmplaceGroup(MOVE_SPRINT_RATTLE, TEXT("sprint_stop_rattle_kz_01 (SFX)"), 7);
	EmplaceGroup(MOVE_DODGE_FRONT, TEXT("PLYR_Dodge_Front_01 (SFX)"), 1);
	EmplaceGroup(MOVE_DODGE_REAR, TEXT("PLYR_Dodge_Rear_01 (SFX)"), 1);
	EmplaceGroup(MOVE_DODGE_SIDE, TEXT("khazan_dodge_stone_01 (SFX)"), 7);
	EmplaceGroup(MOVE_FALL, TEXT("PLYR_Movement_Fall_01 (SFX)"), 1);
	EmplaceGroup(MOVE_RANDING, TEXT("PLYR_Movement_Landing_Concrete_01 (SFX)"), 3);
	EmplaceGroup(MOVE_CLIMB, TEXT("PLYR_Movement_Climb_Move_Wood_01 (SFX)"), 8);
	EmplaceGroup(MOVE_CLIMB_END, TEXT("PLYR_Movement_Climb_End_Wood_01 (SFX)"), 2);


	// =================================================================================
	// 2. Spear Attack & Skill
	// =================================================================================

	EmplaceGroup(ATTACK_SPEAR_WEAK1, TEXT("efx_spear_weakatk01_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_SPEAR_WEAK2, TEXT("efx_spear_weakatk02_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_SPEAR_WEAK3, TEXT("efx_spear_weakatk03_01 (SFX)"), 4);

	// 약공 차징 세트
	EmplaceGroup(ATTACK_SPEAR_WEAK_CHARGING_START, TEXT("efx_spear_common_charge_a_start_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_SPEAR_WEAK_CHARGING, TEXT("efx_spear_common_charge_a_01 (SFX)"), 5);
	EmplaceGroup(ATTACK_SPEAR_WEAK_CHARGING_ATTACK, TEXT("efx_spear_chargeatk_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_SPEAR_WEAK_CHARGING_IMPACT, TEXT("efx_spear_chargeatk_impact_01 (SFX)"), 3);

	EmplaceGroup(ATTACK_SPEAR_STRONG1, TEXT("efx_spear_strongatk01_a_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_SPEAR_STRONG2, TEXT("efx_spear_strongatk02_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_SPEAR_STRONG2_FOLEY, TEXT("efx_spear_strongatk02_foley_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_SPEAR_STRONG3, TEXT("efx_spear_strongatk03_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_SPEAR_STRONG3_SWIRL, TEXT("efx_spear_strongatk03_swirl_01 (SFX)"), 3);

	// 강공 차징 세트 (약공과 동일 키 사용)
	EmplaceGroup(ATTACK_SPEAR_STRONG_CHARGING_START, TEXT("efx_spear_common_charge_a_start_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_SPEAR_STRONG_CHARGING, TEXT("efx_spear_common_charge_a_01 (SFX)"), 5);
	EmplaceGroup(ATTACK_SPEAR_STRONG_CHARGING_ATTACK, TEXT("efx_spear_chargeatk_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_SPEAR_STRONG_CHARGING_IMPACT, TEXT("efx_spear_chargeatk_impact_01 (SFX)"), 3);

	EmplaceGroup(ATTACK_SPEAR_DODGE, TEXT("efx_spear_common_stab_hits_01 (SFX)"), 7);
	EmplaceGroup(ATTACK_SPEAR_WEAK_SPRINT, TEXT("PLYR_Spear_Swish_Normal_01 (SFX)"), 6);
	EmplaceGroup(ATTACK_SPEAR_STRONG_SPRINT, TEXT("PLYR_Spear_Swish_Middle_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_SPEAR_BRUTAL1, TEXT("PLYR_Spear_Swish_Strong_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_SPEAR_BRUTAL2, TEXT("efx_spear_grappleatk02_01 (SFX)"), 1);
	EmplaceGroup(ATTACK_SPEAR_FALL, TEXT("PLYR_Spear_FallAtk_Explo_01 (SFX)"), 1);

	// Spear Skill
	EmplaceGroup(SKILL_SPEAR_MOONLIGHT_SLASH, TEXT("efx_spear_lightningspear_swish_01 (SFX)"), 5);
	EmplaceGroup(SKILL_SPEAR_FULL_MOON, TEXT("efx_spear_trance_swish_01 (SFX)"), 6);
	EmplaceGroup(SKILL_SPEAR_SHADOW_SLASH, TEXT("efx_spear_strongbtk01_b_01 (SFX)"), 4);

	// 나선 찌르기
	EmplaceGroup(SKILL_SPEAR_SPIRAL_THRUST_START, TEXT("efx_spear_spiralspear_lv2_start_01 (SFX)"), 1);
	EmplaceGroup(SKILL_SPEAR_SPIRAL_THRUST_IMPACT, TEXT("efx_spear_spiralspear_lv2_impact_01 (SFX)"), 1);
	EmplaceGroup(SKILL_SPEAR_SPIRAL_THRUST_END, TEXT("efx_spear_spiralspear_lv2_end_01 (SFX)"), 3);

	// 나선 찌르기 : 소용돌이
	EmplaceGroup(SKILL_SPEAR_SPIRAL_THRUST_WHIRLWIND_START, TEXT("efx_spear_spiralspear_lv3_start_01 (SFX)"), 1);
	EmplaceGroup(SKILL_SPEAR_SPIRAL_THRUST_WHIRLWIND_IMPACT, TEXT("efx_spear_spiralspear_lv2_impact_01 (SFX)"), 1);
	EmplaceGroup(SKILL_SPEAR_SPIRAL_THRUST_WHIRLWIND_END, TEXT("efx_spear_spiralspear_lv3_end_01 (SFX)"), 3);

	EmplaceGroup(SKILL_SPEAR_ASSAULT, TEXT("efx_pc_spear_seismickick_atk (SFX)"), 1);
	EmplaceGroup(SKILL_SPEAR_MOMENT_SLASH, TEXT("efx_spear_spiralspear_end_01 (SFX)"), 3);
	EmplaceGroup(SKILL_SPEAR_CRITICAL_STRIKE, TEXT("efx_spear_crescent_a_start_01 (SFX)"), 3);

	// 그림자 참격
	EmplaceGroup(SKILL_SPEAR_SHADOW_CLEAVE_START, TEXT("efx_spear_moonveil_start_01 (SFX)"), 3);
	EmplaceGroup(SKILL_SPEAR_SHADOW_CLEAVE_IMPACT, TEXT("efx_spear_moonveil_suc_impact_01 (SFX)"), 2);


	// =================================================================================
	// 3. GS Attack & Skill
	// =================================================================================

	EmplaceGroup(ATTACK_GS_WEAK1, TEXT("efx_gsword_weakatk01_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_GS_WEAK1_IMPACT, TEXT("efx_gsword_weakatk01_impact_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_GS_WEAK2, TEXT("efx_gsword_weakatk02_01 (SFX)"), 4);
	// 약공 임팩트2: 없음 -> 맵에 삽입하지 않음 (노티파이에서 무시)
	EmplaceGroup(ATTACK_GS_WEAK3, TEXT("efx_gsword_weakatk03_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_GS_WEAK3_IMPACT, TEXT("efx_gsword_weakatk03_impact_01 (SFX)"), 6);
	EmplaceGroup(ATTACK_GS_WEAK3_FOLEY, TEXT("efx_gsword_weakatk03_foley_end_01 (SFX)"), 4);

	// 약공 차징 
	EmplaceGroup(ATTACK_GS_WEAK1_CHARGING, TEXT("efx_gsword_weakatk03_charge_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_GS_WEAK1_CHARGING_ATTACK, TEXT("efx_gsword_weakatk_chargeatk_01 (SFX)"), 4);
	EmplaceGroup(ATTACK_GS_WEAK2_CHARGING, TEXT("efx_gsword_weakatk03_charge_01 (SFX)"), 4); // 동일 키
	EmplaceGroup(ATTACK_GS_WEAK2_CHARGING_ATTACK, TEXT("efx_gsword_weakatk_chargeatk_01 (SFX)"), 4); // 동일 키

	//강공
	EmplaceGroup(ATTACK_GS_STRONG, TEXT("PLYR_GSword_Swish_Strong_01 (SFX)"), 3);

	//강공 차징
	EmplaceGroup(ATTACK_GS_STRONG_CHARGING, TEXT("PLYR_GSword_StrongAtk_Charge_01 (SFX)"), 1);
	EmplaceGroup(ATTACK_GS_STRONG_CHARGING_ATTACK, TEXT("PLYR_GSword_StrongAtk_Explo_01 (SFX)"), 1);

	//닷지공격
	EmplaceGroup(ATTACK_GS_DODGE_ATTACK, TEXT("PLYR_GSword_DodgeAtk_01 (SFX)"), 3);
	EmplaceGroup(ATTACK_GS_DODGE_IMPACT, TEXT("efx_gsword_dodgeatk_impact_01 (SFX)"), 4);

	//브루탈
	EmplaceGroup(ATTACK_GS_BRUTAL_START, TEXT("efx_gsword_com_grapple_start_01 (SFX)"), 1);
	EmplaceGroup(ATTACK_GS_BRUTAL1, TEXT("efx_gsword_com_grapple_atk1_01 (SFX)"), 1);
	EmplaceGroup(ATTACK_GS_BRUTAL2, TEXT("efx_gsword_com_grapple_atk2_01 (SFX)"), 1);

	//낙하
	EmplaceGroup(ATTACK_GS_FALL, TEXT("PLYR_GSword_FallAtk_Explo_01 (SFX)"), 1);

	// GS Skill
	// 숨통 끊기
	EmplaceGroup(SKILL_GS_BREATHTAKING_START, TEXT("efx_gsword_ghostslash_start_01 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_BREATHTAKING_ATTACK, TEXT("efx_gsword_ghostslash_swish_01 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_BREATHTAKING_SUCCESS, TEXT("efx_gsword_ghostslash_success_swish_01 (SFX)"), 1);

	// 숨통 끊기 : 태동
	EmplaceGroup(SKILL_GS_BREATHTAKING_EMBRYONIC_START, TEXT("efx_gsword_ghostslash_start_02 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_BREATHTAKING_EMBRYONIC_ATTACK, TEXT("efx_gsword_ghostslash_swish_02 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_BREATHTAKING_EMBRYONIC_SUCCESS, TEXT("efx_gsword_ghostslash_success_swish_02 (SFX)"), 1);

	// 숨통 끊기 : 선혈
	EmplaceGroup(SKILL_GS_BREATHTAKING_BLOODSHED_CHARGING, TEXT("efx_gsword_ghostslash_charge_3_01 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_BREATHTAKING_BLOODSHED_CHARGING_SUCCESS, TEXT("efx_pc_gsword_ghostslash_charge_suc (SFX)"), 1);
	EmplaceGroup(SKILL_GS_BREATHTAKING_BLOODSHED_ATTACK, TEXT("efx_gsword_ghostslash_swish_03 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_BREATHTAKING_BLOODSHED_SUCCESS, TEXT("efx_gsword_ghostslash_success_swish_03 (SFX)"), 1);

	// 거인 사냥
	EmplaceGroup(SKILL_GS_GIANTHUNT_CHARGING,TEXT("efx_gsword_common_charge_blood_a_01 (SFX)"), 3 );
	EmplaceGroup(SKILL_GS_GIANTHUNT_ATTACK, TEXT("efx_pc_gsword_com_explo_md_01 (SFX)"), 2);

	// 귀신 : 어둠의 그림자
	EmplaceGroup(SKILL_GS_PHANTOM_SHADOWOFDARKNESS_CHARGING, TEXT("efx_pc_gsword_soulbringergreatsword_charge (SFX)"), 1);
	EmplaceGroup(SKILL_GS_PHANTOM_SHADOWOFDARKNESS_ATTACK, TEXT("efx_pc_gsword_com_explo_md_01 (SFX)"), 2);

	// 한계 극복
	EmplaceGroup(SKILL_GS_LIMIT_BREAK_CHARGING, TEXT("efx_gsword_apocalypse_charge_01 (SFX)"), 3);
	EmplaceGroup(SKILL_GS_LIMIT_BREAK_LOOP, TEXT("efx_gsword_apocalypse_loop_01 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_LIMIT_BREAK_ATTACK, TEXT("efx_gsword_apocalypse_sword_01 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_LIMIT_BREAK_END, TEXT("efx_gsword_apocalypse_finish_01 (SFX)"), 3);

	// 정면 돌파
	EmplaceGroup(SKILL_GS_BREAK_THROUGH_CHARGING, TEXT("efx_gsword_common_charge_c_01 (SFX)"), 1	);
	EmplaceGroup(SKILL_GS_BREAK_THROUGH_ATTACK, TEXT("efx_pc_gsword_com_explo_small_01 (SFX)"), 3);

	// 거대한 포효
	EmplaceGroup(SKILL_GS_WARCRY_CHARGING, TEXT("efx_pc_gsword_wardeclaration_charge (SFX)"), 1);
	EmplaceGroup(SKILL_GS_WARCRY_ATTACK, TEXT("efx_pc_gsword_wardeclaration (SFX)"), 1);

	// 내재된 분노
	EmplaceGroup(SKILL_GS_INNER_FURY_CHARGING, TEXT("efx_gsword_ragingfury_charge_01 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_INNER_FURY_CHARGING_SUCCESS, TEXT("efx_gsword_ragingfury_charge_suc_01 (SFX)"), 1);
	EmplaceGroup(SKILL_GS_INNER_FURY_ATTACK, TEXT("efx_gsword_ragingfury_explo_suc_01 (SFX)"), 1);


	// =================================================================================
	// 4. Damaged, Guard
	// =================================================================================

	EmplaceGroup(DAMAGED_NORMAL, TEXT("vo_pc_dmg_01 (Korean(KR))"), 7);
	EmplaceGroup(DAMAGED_HARD, TEXT("vo_kz_effort_dmg_injured_01 (Korean(KR))"), 3);

	EmplaceGroup(GUARD_ON_SPEAR, TEXT("efx_pc_spear_guard_on_01 (SFX)"), 4);
	EmplaceGroup(GUARD_ON_GS, TEXT("efx_pc_gsword_guard_on_01 (SFX)"), 4);
	EmplaceGroup(GUARD_FOLEY, TEXT("efx_pc_spear_guard_on_foley_01 (SFX)"), 5);
	EmplaceGroup(GUARD_OFF_SPEAR, TEXT("efx_pc_dualaxesword_guard_off_01 (SFX)"), 4);
	EmplaceGroup(GUARD_OFF_GS, TEXT("efx_pc_gsword_guard_off_01 (SFX)"), 3);

	EmplaceGroup(JUSTGUARD_EFFECT, TEXT("efx_pc_justguard_suc_spark_01 (SFX)"), 3);
	EmplaceGroup(JUSTGUARD, TEXT("efx_superarmor_gsword_layer_01 (SFX)"), 6);

	// =================================================================================
	// 5. Interaction
	// =================================================================================

	EmplaceGroup(INTERACTION_LACRIMA_ON, TEXT("efx_kz_lacrima_on (SFX)"), 1);
	EmplaceGroup(INTERACTION_LACRIMA_OFF, TEXT("efx_kz_lacrima_off (SFX)"), 1);
	EmplaceGroup(INTERACTION_LACRIMA_GAIN, TEXT("efx_kz_lacrima_gain (SFX)"), 1);
	EmplaceGroup(INTERACTION_LACRIMA_GET, TEXT("efx_kz_lacrima_get (SFX)"), 1);
	EmplaceGroup(INTERACTION_HEAL, TEXT("PLYR_ItemDrink_HP_01 (SFX)"), 1);
	EmplaceGroup(INTERACTION_LANTERN_ON, TEXT("efx_lantern_on_01 (SFX)"), 3);
	EmplaceGroup(INTERACTION_LANTERN_OFF, TEXT("efx_lantern_off_01 (SFX)"), 1);
	EmplaceGroup(INTERACTION_FRIENDLY, TEXT("efx_apc_friendly_interaction (SFX)"), 1);
	EmplaceGroup(INTERACTION_STATUE, TEXT("efx_kz_stone_statue_rotate_01 (SFX)"), 3);

	// 무기 장착/해제
	EmplaceGroup(INTERACTION_SPEAR_ARMED, TEXT("efx_pc_spear_armed_01 (SFX)"), 4);
	EmplaceGroup(INTERACTION_SPEAR_UNARMED, TEXT("efx_pc_spear_unarmed_01 (SFX)"), 4);
	EmplaceGroup(INTERACTION_GS_ARMED, TEXT("efx_pc_gsword_armed_01 (SFX)"), 3);
	EmplaceGroup(INTERACTION_GS_UNARMED, TEXT("efx_pc_gsword_unarmed_01 (SFX)"), 4);

	return S_OK;
}

_wstring CKhazan_SoundHelper::Get_NextSoundKey(PC_SOUND_GROUPTYPE type)
{
	auto iter = AllSoundGroups.find(type);
	if ( iter != AllSoundGroups.end() )
		return iter->second.Get_NextKey();

	return TEXT("");
}

void CKhazan_SoundHelper::MakeSoundKeyHelper(_wstring baseKey, size_t count, vector<_wstring>& resultKeys)
{
	if ( count==0 ) return;

	// 마지막 숫자 부분 찾기
	// 문자열을 뒤에서부터 검사하여 숫자의 시작 위치를 찾기
	size_t digit_end = baseKey.size();
	size_t digit_start = string::npos;

	// 뒤에서부터 처음으로 숫자가 아닌 문자를 만나는 지점 = 숫자의 시작
	for ( size_t i = baseKey.size(); i>0; --i ) {
		if ( !isdigit(baseKey[ i-1 ]) ) {
			digit_start = i;
			break;
		}
	}
	// 숫자가 전혀 없거나 유효하지 않은 형식
	if ( digit_start==std::string::npos ) return;
	
	_wstring number_str = baseKey.substr(digit_start, digit_end-digit_start);
	_int initial_number = _wtoi(number_str.c_str());
	size_t padding_width = number_str.length();

	_wstring prefix = baseKey.substr(0, digit_start);

	resultKeys.reserve(resultKeys.size()+count);

	for ( size_t i = 0; i<count; ++i ) {
		int current_number = initial_number+( int ) i;

		// 숫자를 원래의 패딩(예: 01, 02)을 유지하며 문자열로 변환
		wstringstream ws;
		ws<<prefix;
		// setfill(L'0')로 0 채우기를 설정하고, setw로 너비를 설정
		ws<<setfill(L'0')<<setw(padding_width)<<current_number;

		resultKeys.emplace_back(ws.str());
	}
}

CKhazan_SoundHelper* CKhazan_SoundHelper::Create()
{
	CKhazan_SoundHelper* pInstance = new CKhazan_SoundHelper;
	if ( FAILED(pInstance->Initialize()) )
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CKhazan_SoundHelper"));
	}
	return pInstance;
}

void CKhazan_SoundHelper::Free()
{
	__super::Free();
}
