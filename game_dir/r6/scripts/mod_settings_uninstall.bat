cd %~dp0..\..\
DEL /S /Q red4ext\plugins\mod_settings\mod_settings.dll archive\pc\mod\ModSettings.archive r6\scripts\mod_settings\mod_settings.module.reds r6\scripts\mod_settings\mod_settings.packed.reds
@RD /S /Q red4ext\plugins\mod_settings r6\scripts\mod_settings
DEL /S /Q red4ext\plugins\mod_settings_uninstall.bat r6\scripts\mod_settings_uninstall.bat
