#pragma once

namespace Cheat
{
	namespace CheatFeatures
	{
		extern int SpeedometerVectorPosition;
		extern int AimbotBoneVectorPosition;
		extern int MeasurementSystemVectorPosition;
		extern int ImpactAmmoVectorPosition;
		extern int CustomAmmoVectorPosition;
		extern int FontTypeVectorPosition;
		extern int AnimationsVectorPosition;
		extern int ScenariosVectorPosition;
		extern int PedMovementVectorPosition;
		extern int OpenVehicleDoorPosition;
		extern int CloseVehicleDoorPosition;
		extern int FastSuperRunPosition;
		extern int selectedPlayer;
		extern int PlayerOpacityInt;
		extern bool BlockMaliciousScriptEvents;
		extern bool BlockAllScriptEvents;
		extern bool HideOwnIPAddress;
		extern bool HideVehicleInfoAndPreview;
		extern bool CursorGUINavigationEnabled;
		extern bool ShowJoiningPlayersNotification;
		extern bool LogChatMessages;
		extern bool GTAODeveloperMode;
		extern bool VehicleSpawnerSpawnInsideVehicle;
		extern bool VehicleSpawnerDeleteOldVehicle;
		extern bool VehicleSpawnerSpawnWithBlip;
		extern bool VehicleSpawnerSpawnWithGodmode;
		extern bool VehicleSpawnerSpawnMaxUpgraded;
		extern bool VehicleSpawnerSpawnAirVehicleAir;
		extern bool HidePlayerInformationBox;
		extern bool HideSelectableInformationBox;
		extern bool ControllableAnimations;

		void NonLooped();
		void Looped();
		extern bool GodmodeBool;
		void Godmode(bool toggle);
		extern bool NeverWantedBool;
		void NeverWanted(bool toggle);
		extern bool NoWeaponReloadBool;
		void NoWeaponReload();
		extern bool SlowMotionBool;
		void SlowMotion(bool toggle);
		extern bool WorldBlackoutBool;
		void WorldBlackout(bool toggle);
		extern float GravityGunEntityDistance;
		extern bool GravityGunBool;
		void GravityGun();
		extern bool HideHUDBool;
		void HideHUD();
		extern bool NoGravityBool;
		void NoGravity(bool toggle);
		extern bool WorldSnowLocalBool;
		void WorldSnowLocal(bool toggle);
		extern bool AutoTeleportToWaypointBool;
		void AutoTeleportToWaypoint();
		extern bool OneHitKillBool;
		void OneHitKill(bool toggle);
		extern bool PauseTimeBool;
		void PauseTime(bool toggle);
		extern bool ExplosiveMeleeBool;
		void ExplosiveMelee();
		extern bool OrbitalCannonCooldownBypassBool;
		void OrbitalCannonCooldownBypass();
		extern bool ProtectionVoteKickBool;
		void ProtectionVoteKick(bool toggle);
		extern bool ProtectionFreezeBool;
		void ProtectionFreeze(bool toggle);
		extern bool ProtectionWorldEventsBool;
		void ProtectionWorldEvents(bool toggle);
		extern bool ProtectionVehicleBool;
		void ProtectionVehicle(bool toggle);
		extern bool ProtectionAlterWantedLevelBool;
		void ProtectionAlterWantedLevel(bool toggle);
		extern bool ProtectionGiveRemoveWeaponsBool;
		void ProtectionGiveRemoveWeapons(bool toggle);
		extern bool SuperJumpBool;
		void SuperJump();
		extern bool FastRunBool;
		void FastRun(bool toggle);
		extern bool ShowFPSBool;
		void ShowFPS();
		extern bool JumpAroundModeBool;
		void JumpAroundMode();
		extern bool VehicleHornBoostBool;
		void VehicleHornBoost();
		extern bool FakeWantedLevelBool;
		void FakeWantedLevel(bool toggle);
		extern bool VehicleGodmodeBool;
		void VehicleGodmode(bool toggle);
		extern bool VehicleInvisibleBool;
		void VehicleInvisible(bool toggle);
		extern bool PlayerInvisibleBool;
		void PlayerInvisible(bool toggle);
		extern bool MobileRadioBool;
		void MobileRadio(bool toggle);
		extern bool WeaponRapidFireBool;
		void WeaponRapidFire();
		extern bool PlayerIgnoredBool;
		void PlayerIgnored(bool toggle);
		extern bool NoClipBool;
		extern bool NoClipWasEnabled;
		void NoClip();
		extern bool RainbowVehicleBool;
		void RainbowVehicle();
		extern bool DeleteGunBool;
		void DeleteGun();
		extern bool EntityInformationGunBool;
		void EntityInformationGun();
		extern bool SpectatePlayerBool;
		void SpectatePlayer(bool toggle);
		extern bool NoRagdollAndSeatbeltBool;
		void NoRagdollAndSeatbelt(bool toggle);
		extern bool FreezeSelectedPlayerBool;
		void FreezeSelectedPlayer();
		extern bool FreezeAllPlayersBool;
		void FreezeAllPlayers();
		extern bool TriggerBot_ShootNPCBool;
		extern bool TriggerBot_ShootPlayersBool;
		extern bool TriggerBotBool;
		void TriggerBot();
		extern bool SuperBrakesBool;
		void SuperBrakes();
		extern bool TinyPlayerBool;
		void TinyPlayer(bool toggle);
		extern bool UnlimitedRocketBoostBool;
		void UnlimitedRocketBoost();
		extern bool VehicleGunBool;
		extern std::string VehicleGun_VehicleNameString;
		void VehicleGun();
		extern bool PlayerESPBool;
		void PlayerESP();
		extern bool OffRadarBool;
		extern bool OffRadarWasEnabled;
		void OffRadar();
		extern bool ExplodeLoopSelectedPlayerBool;
		void ExplodeLoopSelectedPlayer();
		extern bool DriveOnWaterBool;
		void DriveOnWater();
		extern bool SuperManBool;
		void SuperMan();
		extern bool ShakeCamSelectedPlayerBool;
		void ShakeCamSelectedPlayer();
		extern bool RainbowGunBool;
		void RainbowGun();
		extern bool DisablePhoneBool;
		void DisablePhone();
		extern bool NoIdleKickBool;
		void NoIdleKick();
		extern bool CopsTurnBlindEyeBool;
		extern bool CopsTurnBlindEyeWasEnabled;
		void CopsTurnBlindEye();
		extern bool MoneyDropBool;
		extern int MoneyDropDelay;
		extern int MoneyDropDelayPreviousTick;
		void MoneyDrop();
		extern bool VehicleWeaponsBool;
		extern bool VehicleWeapons_TankRounds;
		extern bool VehicleWeapons_VehicleRockets;
		extern bool VehicleWeapons_Fireworks;
		extern bool VehicleWeapons_DrawLaser;
		void VehicleWeapons();
		extern bool ShowSessionInformationBool;
		void ShowSessionInformation();
		extern bool AutoGiveAllWeaponsBool;
		void AutoGiveAllWeapons();
		extern bool AutoGiveAllWeaponUpgradesBool;
		void AutoGiveAllWeaponUpgrades();
		extern bool CrossHairBool;
		extern bool CrossHairADSOnlyBool;
		void CrossHair();
		extern bool FreeCamBool;
		void FreeCam(bool toggle);
		extern bool CartoonGunBool;
		void CartoonGun();
		extern bool RGBDiscoBool;
		extern bool RGBDiscoFirstCall;
		void RGBDisco();
	}
}